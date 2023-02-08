#include <Arduino.h>
#include <Servo.h>

// Local includes
#include "h-bridge.h"
#include "switches.h"

// Global defines
#define HOLD    2
#define FAST    50
#define MED     25
#define SLOW    5
// SERVO STATES
#define SERVO_OPEN 110
#define SERVO_CLOSE 10
#define SERVO_PIN 10

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

int servo_pos = SERVO_CLOSE;
uint16_t result = 0;

int RC_high_time = micros();
int RC_PWM;
int RC_PWM_stored;

// Servo object
Servo servo;

int speed = FAST;

void setup()
{
  // put your setup code here, to run once:
  hbridge_setup();
  switches_setup();
  pinMode(pin_in_control_n0, INPUT_PULLUP);
  pinMode(pin_in_control_n1, INPUT_PULLUP);
  pinMode(pin_in_control_n2, INPUT_PULLUP);
  pinMode(pin_in_control_n3, INPUT_PULLUP);

  pinMode(SERVO_PIN, OUTPUT);
  servo.attach(SERVO_PIN);

  // Set pin mode interrupt
  pinMode(pwm_in, INPUT);
  attachInterrupt(digitalPinToInterrupt(pwm_in), read_me, CHANGE);

}

void loop()
{
  // Read the nibble from the Pi or switch
  in_control_nibble = read_nibble_control();

  // Wind in range
  if (RC_PWM_stored > 1700 && RC_PWM_stored < 2000){
    in_control_nibble = 0b00001101;
  }
    
  // Wind out range
  else if (RC_PWM_stored > 1000 && RC_PWM_stored < 1300){
    in_control_nibble = 0b00001100;
  }

  // Switch on nibble value
  switch (in_control_nibble) {
    case 15:  // Reset  1111
      reset();
      break;

    case 14:  // Hold rope  1110
      hbridge_control(HBRIDGE_FORWARD, HOLD);
      servo_pos = SERVO_CLOSE;
      servo.write(servo_pos);
      break;

    case 13:  // Wind rope in 1101
      servo_pos = SERVO_CLOSE;
      servo.write(servo_pos);
      hbridge_control(HBRIDGE_FORWARD, 15);
      // hbridge_control(HBRIDGE_FORWARD, FAST);
      // delay(3500);
      // hbridge_control(HBRIDGE_FORWARD, SLOW);
      // delay(1500);
      // hbridge_control(HBRIDGE_OFF, 0);
      break;

    case 12:  // Release rope 1100
      servo_pos = SERVO_OPEN;
      servo.write(servo_pos);
      hbridge_control(HBRIDGE_BACKWARD, FAST);
      if(speed-20 <= MED && speed > SLOW){
        hbridge_control(HBRIDGE_BACKWARD, MED);
      }
      else if (speed-40 <= SLOW && speed > 0)
      {
        hbridge_control(HBRIDGE_BACKWARD, SLOW);
      }
      else if (speed <= 0)
      {
        hbridge_control(HBRIDGE_OFF, 0);
        delay(1000);
      }
      // hbridge_control(HBRIDGE_BACKWARD, FAST);
      // delay(1500);
      // hbridge_control(HBRIDGE_BACKWARD, MED);
      // delay(2000);
      // hbridge_control(HBRIDGE_BACKWARD, SLOW);
      // delay(2000);
      // hbridge_control(HBRIDGE_OFF, 0);
      break;

    case 11: // Open servo     1011
      servo_pos = SERVO_OPEN;
      servo.write(servo_pos);
      break;

    default:
      break;
  }
  if (speed > 0){
    speed -= 1;
  }
  else{
    speed = 100;
  }
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

void read_rc()
{
  // Read rc
  result = pulseIn(pwm_in, HIGH);
  return;
}

void reset() {
  servo_pos = SERVO_CLOSE;
  servo.write(servo_pos);
  hbridge_control(HBRIDGE_OFF, 0);
  speed = 100;
  return;
}

void read_me()
{
  RC_PWM = micros() - RC_high_time;
  if (RC_PWM < 10000)
  {
    RC_PWM_stored = RC_PWM;
  }
  RC_high_time = micros();
} // copy store all values from temporary array another array after 15 reading