#include <Arduino.h>

// Local includes
#include "servo.h"
#include "h-bridge.h"
#include "switches.h"

// Global defines
#define DISABLE 0
#define ENABLE  1
#define HOLD    5
#define FAST    100
#define MED     50
#define SLOW    10

// Interrupt pin
int pwm_in = 2;

// Used for controlling the statemachine, RaspberryPi, micro switch and RC signal control this
int in_control_n0 = 1;
int in_control_n1 = 1;
int in_control_n2 = 1;
int in_control_n3 = 1;
byte in_control_nibble = 0b00000000;

// pin_in is connected to only micro switch but it is possible to wire this to the raspberry pi also
int pin_in_control_n0 = 3;
int pin_in_control_n1 = 4;
int pin_in_control_n2 = 5;
int pin_in_control_n3 = 6;

int servo_pos = CLOSE;
int enable = DISABLE;
int dir = CW;
int pwm = 0;

int old_servo_pos = CLOSE;
int old_enable = DISABLE;
int old_dir = CW;
int old_pwm = 0;

void setup()
{
  // put your setup code here, to run once:
  servo_setup();
  hbridge_setup();
  switches_setup();
  pinMode(pin_in_control_n0, INPUT_PULLUP);
  pinMode(pin_in_control_n1, INPUT_PULLUP);
  pinMode(pin_in_control_n2, INPUT_PULLUP);
  pinMode(pin_in_control_n3, INPUT_PULLUP);

  // Set pin mode interrupt
  pinMode(pwm_in, INPUT);
}

void loop()
{
  pwm = read_rc();

  // Read the nibble from the Pi or switch
  in_control_nibble = read_nibble_control();

  // Switch on nibble value
  switch (in_control_nibble) {
    case 15:  // Reset  1111
      reset();
      break;

    case 14:  // Hold rope  1110
      hbridge_control(HBRIDGE_FORWARD, HOLD);
      servo_pos = SERVO_CLOSE;
      break;

    case 13:  // Wind rope in 1101
      hbridge_control(HBRIDGE_FORWARD, FAST);
      delay(3000);
      hbridge_control(HBRIDGE_FORWARD, SLOW);
      delay(1000);
      hbridge_control(HBRIDGE_OFF, 0);
      servo_pos = SERVO_CLOSE;
      break;

    case 12:  // Release rope 1100
      hbridge_control(HBRIDGE_BACKWARD, FAST);
      delay(1500);
      hbridge_control(HBRIDGE_BACKWARD, MED);
      delay(1500);
      hbridge_control(HBRIDGE_BACKWARD, SLOW);
      delay(1000);
      hbridge_control(HBRIDGE_OFF, 0);
      servo_pos = SERVO_OPEN;
      break;

    case 11: // Open servo     1011
      servo_pos = SERVO_OPEN;

    default:
      break;
  }
  servo_control(servo_pos);

  delay(100);
}

int read_nibble_control() {
  byte return_nibble = 0b00000000;

  in_control_n0 = digitalRead(pin_in_control_n0);
  in_control_n1 = digitalRead(pin_in_control_n1);
  in_control_n2 = digitalRead(pin_in_control_n2);
  in_control_n3 = digitalRead(pin_in_control_n3);

  bitWrite(return_nibble, 0, in_control_n0);
  bitWrite(return_nibble, 1, in_control_n1);
  bitWrite(return_nibble, 2, in_control_n2);
  bitWrite(return_nibble, 3, in_control_n3);

  return return_nibble;
}

uint8_t read_rc()
{
  // Read rc
  uint16_t result = pulseIn(pwm_in, HIGH);

  // Wind in range
  if (result > 1700 && result < 2000){
    in_control_nibble = 0b00001101;
    return HBRIDGE_FORWARD;
  }
    
  // Wind out range
  else if (result > 1000 && result < 1300){
    in_control_nibble = 0b00001100;
    return HBRIDGE_BACKWARD;
  }

  // No signal
  else
    return 0;
}

void reset() {
  servo_pos = SERVO_CLOSE;
  hbridge_control(HBRIDGE_FORWARD, 5);
  return;
}