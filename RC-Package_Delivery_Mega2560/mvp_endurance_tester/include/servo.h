#include <Servo.h>

Servo servo;

void servo_setup() {
  servo.attach(10);
}