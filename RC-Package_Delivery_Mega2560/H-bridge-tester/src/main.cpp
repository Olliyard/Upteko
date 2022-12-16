#include <Arduino.h>

// Debug
#define DEBUG 0

// Local includes
#include "servo.h"
#include "h-bridge.h"
#include "switches.h"

void setup()
{
  // put your setup code here, to run once:
  servo_setup();
  hbridge_setup();
  switches_setup();
}

void loop()
{
  uint8_t speed = 100;
  // put your main code here, to run repeatedly:
  hbridge_control(HBRIDGE_FORWARD, speed);
  delay(3000);

  hbridge_control(HBRIDGE_OFF, 0);
  delay(3000);

  hbridge_control(HBRIDGE_BACKWARD, speed);
  delay(3000);

  hbridge_control(HBRIDGE_OFF, 0);
  delay(3000);
}