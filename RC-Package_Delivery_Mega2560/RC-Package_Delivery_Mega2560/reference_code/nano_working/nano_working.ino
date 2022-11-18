// for nano
#include <Arduino.h>
#include <Servo.h>

// configs
#define debug 1

// PWM Part
#define pin_interrupt 2 //read_pin 2
long pulse_width = 0;

// Servo Part
#define servo_pin 10
Servo my_servo;
int servo_val = 0;

// dc-motor part
#define pin_dir 11
#define pin_enable 12
#define pin_pwm 13

void setup()
{
  //if (debug)
    //Serial.begin(115200);

  // PWM Part
  pinMode(pin_interrupt, INPUT);

  // Servo Part
  my_servo.attach(servo_pin);
  //pinMode(servo_pin, OUTPUT);

  // dc-motor Part
  pinMode(pin_dir, OUTPUT);
  pinMode(pin_enable, OUTPUT);
  pinMode(pin_pwm, OUTPUT);
}

void loop()
{
  // debug
  //if (debug)
    //Serial.println();

  // read pulsewidth
  pulse_width = pulseIn(pin_interrupt, HIGH, 40000);

  // write to servo
  write_to_servo();

  // write to dc-motor
  write_to_dc_motor();

//  delay(100);
}

// debug mode
void print()
{
  //Serial.println(pulse_width);
}

// write to servo
void write_to_servo()
{
  //if (debug)
    //Serial.println(pulse_width);
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
    writePWM(50);
    digitalWrite(pin_dir, 1);
    digitalWrite(pin_enable, 1);
  }
  else if (pulse_width < 1300 && pulse_width > 1000)
  {
    writePWM(50);
    digitalWrite(pin_dir, 0);
    digitalWrite(pin_enable, 1);
  }
  else
  {
    writePWM(0);
    digitalWrite(pin_dir, 0);
    digitalWrite(pin_enable, 0);
  }
}

//PWM without analogWrite
void writePWM(int percent){
  int val = 16*((float)percent/100);
  digitalWrite(pin_pwm, HIGH);
  delay(val); // 16/(percent/10)
  digitalWrite(pin_pwm, LOW);
  delay(16-val);
}
