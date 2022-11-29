#include <Servo.h>

// SERVO STATES
#define SERVO_OPEN 110
#define SERVO_CLOSE 10

void __servo_open();
void __servo_close();

Servo servo;

void servo_setup()
{
    servo.attach(10);
}

void servo_control(uint8_t state)
{
    if (state == SERVO_OPEN)
        __servo_open();
    else if (state == SERVO_CLOSE)
        __servo_close();
}

// Open servo
void __servo_open()
{
    servo.write(110);
}

// Close servo
void __servo_close()
{
    servo.write(10);
}