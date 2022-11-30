#include "BQ76940.h"

uint8_t BQ76940_init(void)
{
	uint8_t error = 0;
    I2C_init();

	// Enable ADC
	error |= I2C_write(SYS_CTRL1, ADC_EN);
	if(!(I2C_read(SYS_CTRL1, 1) & ADC_EN)){ 
        return ERROR_ADC; 
    }

	// Write 0x19 to CC_CFG according to datasheet page 29
	error |= I2C_write(CC_CFG, 0x19);

    // Get BQ offset and gain and evaluate it
    offset = I2C_read(ADCOFFSET, 1);
    gain = BQ7690_getGain();
	if((gain<365) | (gain>396)) { 
        return ERROR_GAIN; 
    }
	
    // Write the Undervoltage and Overvoltage thresholds
	error |= BQ76940_setUV();
	error |= BQ76940_setOV();

    // for 30 A
    // I2C_write(PROTECT1, 0x00);	// write PROTECT1
    // I2C_write(PROTECT2, 0x00);	// write PROTECT2
    // I2C_write(PROTECT3, 0x50);	// write PROTECT3

    // for 190 A
	error |= I2C_write(PROTECT1, 0x0B);	// write PROTECT1
	error |= I2C_write(PROTECT2, 0x9C);	// write PROTECT2
	error |= I2C_write(PROTECT3, 0x50);	// write PROTECT3

	// clear SYS-STAT for init
	if( I2C_read(SYS_STAT, 1)){ 
        ISR_getSTAT(); 
    }

	// doublecheck if bq is ready
	if(I2C_read(SYS_STAT, 1) & DEVICE_XREADY){
		// DEVICE_XREADY is set
		// write 1 in DEVICE_XREADY to clear it
		I2C_write(SYS_STAT, DEVICE_XREADY);
		// check again
		if(I2C_read(SYS_STAT, 1) & DEVICE_XREADY) { 
            return ERROR_XREADY; 
        }
	}

	// enable countinous reading of the Coulomb Counter
    // I2C_write(SYS_CTRL2, CC_EN);	// sets ALERT at 250ms interval to high


	return error; // 0 if successful
}

// Enter Ship Mode
uint8_t BQ76940_enterShipMode()
{
	uint8_t error = 0;
	uint8_t ctrl1 = I2C_read(SYS_CTRL1, 1);

	// clear SHUT_A and SHUT_B
	ctrl1 &= 0xFC;
	error |= I2C_write(SYS_CTRL1, ctrl1);

	// SHUT_A=0, SHUT_B=1
	ctrl1 |= 0x01;
	error |= I2C_write(SYS_CTRL1, ctrl1);

	// SHUT_A=1, SHUT_B=0
	ctrl1 |= 0x01;
	error |= I2C_write(SYS_CTRL1, ctrl1);

	return error; // 0 if successful
}

// Gets the Gain value from the ADCGAIN registers
uint16_t BQ7690_getGain(){
    uint8_t reg1 = I2C_read(ADCGAIN1, 1);
	uint8_t reg2 = I2C_read(ADCGAIN2, 1);

	reg1 &= 0b00001100;

	return (365 + ((reg1 << 1)|(reg2 >> 5)));
}

// Gets the Pack Voltage read from the BAT register
uint16_t BQ76940_getPackVoltage(){
    uint32_t packADC = I2C_read(BAT_HI, 2);
	return (((4*gain*(packADC))/1000)+(NUMBER_OF_CELLS*offset));	// Cellstack Voltage in mV
}

// Gets the Cell Voltage read from the parameter cell register
uint16_t BQ76940_getCellVoltage(uint8_t cellRegister){
    uint32_t cellADC = 0;
	cellADC = I2C_read(cellRegister, 2);

	return (((cellADC*gain)/1000)+offset);				// returns voltage in mV
}

// Gets the Coulomb Counter value
int16_t BQ76940_getCurrent(){
    current = I2C_read(CC_HI, 2);	
	return ((int32_t)current*(844/R_SHUNT))/100;
}

// Gets the Temperature value
int16_t BQ76940_getTemp(){
	uint8_t ctrl1 = (I2C_read(SYS_CTRL1, 1));
	int32_t temp = 0;
    if( !(ctrl1 & TEMP_SEL) ) 
    { 
        I2C_write(SYS_CTRL1, (ctrl1  | (TEMP_SEL)) ); 
        delay(2000); 
    } // wait for update
    
    temp = I2C_read(TS1_HI, 2);

    // Calculations made from datasheet page 20
    temp = (temp*382)/1000;
    temp = (10000*temp)/(3300-temp);
    return ((-26.38*log(temp)) + 91.798); //See Excel document NTC103AT_temp_res_correlation.xlsx

}

// Sets the Overvoltage threshold
uint8_t BQ76940_setOV(void){
	return ( I2C_write(OV_TRIP, tripCalculator(OV_THERSHOLD)) ); // 0 => successful
}

// Sets the Undervoltage threshold
uint8_t BQ76940_setUV(void){
	return (I2C_write(UV_TRIP, tripCalculator(UV_THERSHOLD)) ); // 0 => successful
}

uint8_t tripCalculator(uint32_t threshold){
	threshold -= offset;
	threshold *= 1000;
	threshold /= gain;
	threshold++;
	threshold >>= 4;

	return ((uint8_t)threshold);
}



void ISR_init(void)
{
	attachInterrupt(digitalPinToInterrupt(2), ISR_getSTAT, RISING);
}

// Gets the STAT register from the newest ALERT interrupt
void ISR_getSTAT(void)
{
	uint8_t stat = I2C_read(SYS_STAT, 1);

	if(stat & (CC_READY))		// ADC for current is ready
	{

		current = BQ76940_getCurrent();
	}

	if(stat & (OVRD_ALERT))	// external fault detected
	{
//		uart_send("Externer Fehler\n");
	}

	if(stat & (DEVICE_XREADY))	// internal fault
	{
//		uart_send("Interner Fehler\n");

		// check internal error
		if(stat & (UV))		// undervoltage
		{
//			uart_send("Entladeschlussspg. erreicht!\n");
		}
		if(stat & (OV))		// overvoltage
		{
//			uart_send("Ladeschlussspg. erreicht!\n");
		}
		if(stat & (SCD))	// short circuit
		{
//			uart_send("Kurzschluss!\n");
		}
		if(stat & (OCD))	// over current
		{
//			uart_send("Ueberstrom!\n");
		}
	}

delay(5000);

	I2C_write(SYS_STAT, stat); // clear SYS_STAT to pulldown ALERT

}