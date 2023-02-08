#include <Servo.h>

// SERVO STATES
#define SERVO_OPEN 110
#define SERVO_CLOSE 10

// Functions
void servo_setup();
void servo_control(uint8_t state);
void servo_test();
void __servo_open();
void __servo_close();

// Servo object
Servo servo;

// Setup servo
void servo_setup()
{
    servo.attach(10);
}

// Control servo
void servo_control(uint8_t state)
{
    if (state == SERVO_OPEN)
        __servo_open();
    else if (state == SERVO_CLOSE)
        __servo_close();
}

void servo_test()
{
    servo_control(SERVO_OPEN);
    delay(1000);
    servo_control(SERVO_CLOSE);
    delay(1000);
}

// Open servo
void __servo_open()
{
    servo.write(SERVO_OPEN);
}

// Close servo
void __servo_close()
{
    servo.write(SERVO_CLOSE);
}