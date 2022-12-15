// Pinout:
#define D1 8
#define D2 9

// H-bridge states
#define HBRIDGE_OFF 0
#define HBRIDGE_FORWARD 1
#define HBRIDGE_BACKWARD 2

// Functions
void hbridge_setup();
void hbridge_control(uint8_t state);
void __hbridge_off();
void __hbridge_forward();
void __hbridge_backward();

// Setup H-bridge
void hbridge_setup()
{
    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);
}

// Control H-bridge
void hbridge_control(uint8_t state)
{
    if (state == HBRIDGE_OFF)
        __hbridge_off();
    else if (state == HBRIDGE_FORWARD)
        __hbridge_forward();
    else if (state == HBRIDGE_BACKWARD)
        __hbridge_backward();
}

// Turn off H-bridge
void __hbridge_off()
{
    digitalWrite(D1, LOW);
    digitalWrite(D2, LOW);
}

// Turn on H-bridge forward
void __hbridge_forward()
{
    digitalWrite(D1, HIGH);
    digitalWrite(D2, LOW);
}

// Turn on H-bridge backward
void __hbridge_backward()
{
    digitalWrite(D1, LOW);
    digitalWrite(D2, HIGH);
}