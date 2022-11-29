#include <Arduino.h>
#include "debug.h"
#include "motor.h"
#include "servo.h"

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
}