// for nano
#include <Arduino.h>
#include <Servo.h>
#include <SoftPWM.h>

// configs
#define debug 1

// PWM Part
#define read_pin 2
int pulse_width = 0;

// Servo Part
#define servo_pin 10
Servo my_servo;

// dc-motor part
#define pin_dir 11
#define pin_enable 12
#define pin_pwm 13

void setup()
{
  if (debug)
    Serial.begin(9600);

  // PWM Part
  pinMode(read_pin, INPUT);

  // Servo Part
  pinMode(servo_pin, OUTPUT);
  my_servo.attach(servo_pin);

  // dc-motor Part
  pinMode(pin_dir, OUTPUT);
  pinMode(pin_enable, OUTPUT);
  pinMode(pin_pwm, OUTPUT);

  SoftPWMBegin();
  SoftPWMSet(pin_pwm, 0);
}

void loop()
{
  // debug
  if (debug)
    print();

  // read pulsewidth
  pulse_width = pulseIn(read_pin, HIGH, 40000);

  // write to servo
  write_to_servo();

  // write to dc-motor
  //write_to_dc_motor();
}

// debug mode
void print()
{
  Serial.println(pulse_width);
}

// write to servo
void write_to_servo()
{
  if (pulse_width > 1700 && pulse_width < 2000)
  {
    my_servo.write(110);
  }
  else if (pulse_width < 1300 && pulse_width > 1000)
  {
    my_servo.write(10);
  }
  else
  {
    my_servo.write(10);
  }
}

// write to dc-motor
void write_to_dc_motor()
{
  if (pulse_width > 1700 && pulse_width < 2000)
  {
    SoftPWMSetPercent(pin_pwm, 50);
    digitalWrite(pin_dir, 1);
    digitalWrite(pin_enable, 1);
  }
  else if (pulse_width < 1300 && pulse_width > 1000)
  {
    SoftPWMSetPercent(pin_pwm, 50);
    digitalWrite(pin_dir, 0);
    digitalWrite(pin_enable, 1);
  }
  else
  {
    SoftPWMSetPercent(pin_pwm, 0);
    digitalWrite(pin_dir, 0);
    digitalWrite(pin_enable, 0);
  }
}
