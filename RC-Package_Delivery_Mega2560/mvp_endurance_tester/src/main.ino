#include <Arduino.h>
#include "debug.h"
#include "motor.h"
#include "servo.h"
#include "rc.h"

// CONFIGURATION
#define DEBUG 0

void setup()
{
  // put your setup code here, to run once:
  if (DEBUG)
    debug_setup();

  // Setup motor
  motor_setup();

  // Setup servo
  servo_setup();

  // Setup rc
  rc_setup();
}

void loop()
{
  // put your main code here, to run repeatedly:
  // Check debug
  if (DEBUG)
    debug_loop();

  // Read encoder
  uint8_t encoder_value = read_encoder();

  // Read nibble
  uint8_t nibble_value = read_nibble();

  // Read rc
  uint8_t rc_value = rc_read();
  
  // Change nibble, if rc present
  if (rc_read() != 0)
    nibble_value = rc_value;

  // Stop motor
  if (nibble_value == 0b00001111)
  {
    servo_control(SERVO_CLOSE);
    motor_control(MOTOR_STOP);
  }

  // Hold motor
  else if (nibble_value == 0b00001110)
  {
    servo_control(SERVO_CLOSE);
    motor_control(MOTOR_HOLD);
  }

  // Wind in
  else if (nibble_value == 0b00001101)
  {
    servo_control(SERVO_CLOSE);
    motor_control(MOTOR_WIND_IN);
  }

  // Wind out
  else if (nibble_value == 0b00001100)
  {
    servo_control(SERVO_OPEN);
    motor_control(WIND_OUT);
  }
}