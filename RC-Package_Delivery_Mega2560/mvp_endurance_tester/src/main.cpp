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

  // Command motor
  if (rc_value == WIND_IN)
    motor_wind_in();
  else if (rc_value == WIND_OUT)
    motor_wind_out();
}