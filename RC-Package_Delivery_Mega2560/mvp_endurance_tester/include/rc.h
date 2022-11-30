#define RC_PIN 2
#define WIND_IN 1   
#define WIND_OUT 2

void rc_setup()
{
    // Set pin mode interrupt
    pinMode(RC_PIN, INPUT);
}

uint8_t rc_read()
{
    // Read rc
    uint16_t result = pulseIn(RC_PIN, HIGH);

    // Wind in range
    if (result > 1700 && result < 2000)
        return WIND_IN;

    // Wind out range
    else if (result > 1000 && result < 1300)
        return WIND_OUT;

    // No signal
    else
        return 0;
}