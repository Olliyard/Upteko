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
  // put your main code here, to run repeatedly:
  if (DEBUG)
  {
    Serial.begin(9600);
    Serial.println("DEBUG MODE");
    servo_test();
    exit(1);
  }

  // Check switches. Turn on H-bridge accordingly
  if (read_switches() == SWITCH_POS)
    hbridge_control(HBRIDGE_FORWARD);

  else if (read_switches() == SWITCH_NEG)
    hbridge_control(HBRIDGE_BACKWARD);

  else
    hbridge_control(HBRIDGE_OFF);
}