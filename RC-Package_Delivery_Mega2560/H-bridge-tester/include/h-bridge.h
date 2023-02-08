#include <SoftPWM.h>

// Pinout:
#define D1 8
#define D2 9

// H-bridge states
#define HBRIDGE_OFF 0
#define HBRIDGE_FORWARD 1
#define HBRIDGE_BACKWARD 2

// Functions
void hbridge_setup();
void hbridge_control(uint8_t state, uint8_t speed);
void __hbridge_off();
void __hbridge_forward(uint8_t speed);
void __hbridge_backward(uint8_t speed);

// Setup H-bridge
void hbridge_setup()
{
    SoftPWMBegin();
    SoftPWMSet(D1, 0);
    SoftPWMSet(D2, 0);
}

// Control H-bridge
void hbridge_control(uint8_t state, uint8_t speed)
{
    if (state == HBRIDGE_OFF)
        __hbridge_off();

    else if (state == HBRIDGE_FORWARD)
        __hbridge_forward(speed);

    else if (state == HBRIDGE_BACKWARD)
        __hbridge_backward(speed);
}

// Turn off H-bridge
void __hbridge_off()
{
    SoftPWMSet(D1, 0);
    SoftPWMSet(D2, 0);
}

// Turn on H-bridge forward
void __hbridge_forward(uint8_t speed)
{
    SoftPWMSetPercent(D1, speed);
    SoftPWMSetPercent(D2, 0);
}

// Turn on H-bridge backward
void __hbridge_backward(uint8_t speed)
{
    SoftPWMSetPercent(D1, 0);
    SoftPWMSetPercent(D2, speed);
}