
// INT
#define INT_PIN 2

void rc_setup()
{
    // Set pin mode interrupt
    pinMode(INT_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(pin_interrupt), read_me, CHANGE);
}