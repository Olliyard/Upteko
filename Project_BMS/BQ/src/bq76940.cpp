uint8_t bq76940_init(void)
{
    uint8_t error = 0;

    // make sure the bq is booted up--->set TS1 to 3.3V and back to VSS
    // maybe set temp-mode for internal or external temp here

    // enable ADC
    error |= registerWrite(SYS_CTRL1, ADC_EN);
    // check if ADC is active
    if (!(registerRead(SYS_CTRL1) & ADC_EN))
    {
        return ERROR_ADC;
    }

    // write 0x19 to CC_CFG according to datasheet page 39
    error |= registerWrite(CC_CFG, 0x19);

    gain = gainRead(); // get gain
    if ((gain < 365) | (gain > 396))
    {
        return ERROR_GAIN;
    }; // check gain

    offset = offsetRead(); // get offset
    // if( (offset<0x00) | (offset>0xFF) ) { return 1; };	// dont check offset

    error |= UVtripWrite(); // write OV_TRIP
    error |= OVtripWrite(); // write UV_TRIP

    // für 30 A Hauptschalter // Versuchszwecke
    //	error |= registerWrite(PROTECT1, 0x00);	// write PROTECT1
    //	error |= registerWrite(PROTECT2, 0x00);	// write PROTECT2
    //	error |= registerWrite(PROTECT3, 0x50);	// write PROTECT3

    // für 190 A Hauptschalter
    error |= registerWrite(PROTECT1, 0x0B); // write PROTECT1
    error |= registerWrite(PROTECT2, 0x9C); // write PROTECT2
    error |= registerWrite(PROTECT3, 0x50); // write PROTECT3

    // clear SYS-STAT for init
    if (registerRead(SYS_STAT))
    {
        checkSTAT();
    }

    // doublecheck if bq is ready
    if (registerRead(SYS_STAT) & DEVICE_XREADY)
    {
        // DEVICE_XREADY is set
        // write 1 in DEVICE_XREADY to clear it
        error |= registerWrite(SYS_STAT, DEVICE_XREADY);
        // check again
        if (registerRead(SYS_STAT) & DEVICE_XREADY)
        {
            return ERROR_XREADY;
        }
    }

    // enable countinous reading of the Coulomb Counter
    //	error |= registerWrite(SYS_CTRL2, CC_EN);	// sets ALERT at 250ms interval to high

    return error; // 0 if successful
}

uint8_t enterShipMode(void)
{
    uint8_t error = 0;
    uint8_t ctrl1 = registerRead(SYS_CTRL1);

    // clear SHUT_A and SHUT_B
    ctrl1 &= 0xFC;
    error |= registerWrite(SYS_CTRL1, ctrl1);

    // SHUT_A=0, SHUT_B=1
    ctrl1 |= 0x01;
    error |= registerWrite(SYS_CTRL1, ctrl1);

    // SHUT_A=1, SHUT_B=0
    ctrl1 |= 0x01;
    error |= registerWrite(SYS_CTRL1, ctrl1);

    return error; // 0 if successful
}

uint16_t gainRead(void)
{
    uint8_t reg1 = registerRead(ADCGAIN1);
    uint8_t reg2 = registerRead(ADCGAIN2);

    reg1 &= 0b00001100;

    return (365 + ((reg1 << 1) | (reg2 >> 5)));
}

int8_t offsetRead(void)
{
    return registerRead(ADCOFFSET);
}

uint16_t PackVoltageRead(void)
{
    uint32_t packADC = registerDoubleRead(BAT_HI);

    return (((4 * gain * (packADC)) / 1000) + (NUMBER_OF_CELLS * offset)); // Cellstack Voltage in mV
}

uint16_t CellVoltageRead(uint8_t cellregister) // need VCX_HI !!!!!
{
    uint32_t cellADC = 0;

    cellADC = registerDoubleRead(cellregister);

    return (((cellADC * gain) / 1000) + offset); // returns voltage in mV
}

uint8_t CellArrayVoltageRead(uint16_t cellV[])
{
    uint8_t i = 0;
    uint8_t error = 0;

    error = registerArrayRead(VC1_HI, cellV, NUMBER_OF_CELLS);

    for (; i < NUMBER_OF_CELLS; i++)
    {
        cellV[i] = ((((uint32_t)cellV[i] * gain) / 1000) + offset);
    }

    return error;
}

// calculates a Voltage back in ADC and strips the middle 8-Bits
uint8_t tripCalculator(uint32_t thershold)
{
    thershold -= offset;
    thershold *= 1000;
    thershold /= gain;
    thershold++; // Nachkommastellenkorrektur
    thershold >>= 4;

    return ((uint8_t)thershold);
}

uint8_t OVtripWrite(void)
{
    return (registerWrite(OV_TRIP, tripCalculator(OV_THERSHOLD))); // 0 => successful
}

uint8_t UVtripWrite(void)
{
    return (registerWrite(UV_TRIP, tripCalculator(UV_THERSHOLD))); // 0 => successful
}

// measures one of the 4 Tempsensors(1xint., 3xext.)
// number: 0 for internal, 1 for TS1, 2 for TS2 and 3 for TS3
// TEMP_SEL: internal=0, external=1
// notice that a 2-s delay is needed by switching internal and external
int16_t TempRead(uint8_t number)
{
    uint8_t ctrl1 = (registerRead(SYS_CTRL1));
    int32_t temp = 0;

    // calc internal temp
    if (number == 0)
    {
        // check if TEMP_SEL=1 for externel-mode, then clear TEMP_SET and wait
        if (ctrl1 & TEMP_SEL)
        {
            registerWrite(SYS_CTRL1, (ctrl1 & ~(TEMP_SEL)));
            _delay_ms(2000);
        }

        // read tempADC
        temp = registerDoubleRead(TS2_HI);

        // real: -60°C..150°C in (return:) -6000c°C..15000c°C [DS_S.24]
        return (25000 - ((((temp * 382) / 1000) - 1200) * 238)) / 10;
    }

    // calc externel temp
    // check if TEMP_SEL=0 for internel-mode, then set TEMP_SET and wait
    if (!(ctrl1 & TEMP_SEL))
    {
        registerWrite(SYS_CTRL1, (ctrl1 | (TEMP_SEL)));
        _delay_ms(2000);
    } // wait for update

    // read tempADC
    temp = registerDoubleRead(TS1_HI + 2 * (number - 1));

    // calc R_TS [DS_S.23]
    temp = (temp * 382) / 1000;            // mV
    temp = (10000 * temp) / (3300 - temp); // V/A
    temp = RtoT((uint16_t)temp);

    // return TSX in °C in 5°C-Schritten
    return temp;
}

int16_t CurrentRead(void)
{
    int16_t cur = registerDoubleRead(CC_HI);

    cur = ((int32_t)cur * (844 / R_SHUNT)) / 100;

    // measurement area: -327.68A..327.67A // Gültigkeitsbereich(return): -32768cA..32767cA // Auflösung: 30mA
    return cur;
    // 8.44µV=844cµV
    // I=U/R=cµV/(µV/A)=CµVA/µV=cA
}

int16_t RtoT(uint32_t resistance)
{
    int16_t temp;

    if (resistance < 449442)
        temp = -55;
    if (resistance < 335006)
        temp = -50;
    if (resistance < 251866)
        temp = -45;
    if (resistance < 190953)
        temp = -40;
    if (resistance < 145953)
        temp = -35;
    if (resistance < 112440)
        temp = -30;
    if (resistance < 87285)
        temp = -25;
    if (resistance < 68260)
        temp = -20;
    if (resistance < 53762)
        temp = -15;
    if (resistance < 42636)
        temp = -10;
    if (resistance < 34038)
        temp = -5;
    if (resistance < 27348)
        temp = 0;
    if (resistance < 22108)
        temp = 5;
    if (resistance < 17979)
        temp = 10;
    if (resistance < 14706)
        temp = 15;
    if (resistance < 12094)
        temp = 20;
    if (resistance < 10000)
        temp = 25;
    if (resistance < 8311)
        temp = 30;
    if (resistance < 6941)
        temp = 35;
    if (resistance < 5825)
        temp = 40;
    if (resistance < 4911)
        temp = 45;
    if (resistance < 4158)
        temp = 50;
    if (resistance < 3536)
        temp = 55;
    if (resistance < 3020)
        temp = 60;
    if (resistance < 2589)
        temp = 65;
    if (resistance < 2228)
        temp = 70;
    if (resistance < 1925)
        temp = 75;
    if (resistance < 1668)
        temp = 80;
    if (resistance < 1451)
        temp = 85;
    if (resistance < 1267)
        temp = 90;
    if (resistance < 1109)
        temp = 95;
    if (resistance < 974)
        temp = 100;
    if (resistance < 858)
        temp = 105;
    if (resistance < 758)
        temp = 110;
    if (resistance < 672)
        temp = 115;
    if (resistance < 597)
        temp = 120;
    if (resistance < 532)
        temp = 125;
    if (resistance < 475)
        temp = 130;
    if (resistance < 425)
        temp = 135;
    if (resistance < 382)
        temp = 140;
    if (resistance < 343)
        temp = 145;
    if (resistance < 310)
        temp = 150;

    return temp;
}
