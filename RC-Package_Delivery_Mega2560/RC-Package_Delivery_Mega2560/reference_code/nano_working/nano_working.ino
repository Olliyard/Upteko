// for nano
#include <Arduino.h>
#include <Servo.h>

// configs
#define debug 1

// PWM Part
#define pin_interrupt 2 // read_pin 2
long pulse_width = 0;

// Servo Part
#define servo_pin 10
Servo my_servo;

// dc-motor part
#define pin_dir 11
#define pin_enable 12
#define pin_pwm 13

// encoder part
#define pin_encoder_in_b_1 19 //-> 4
#define pin_encoder_in_b_2 18 //-> 5
#define pin_encoder_in_b_3 17
#define pin_encoder_in_b_4 16
#define pin_encoder_in_b_5 15
#define pin_encoder_in_b_6 14
#define pin_encoder_in_b_7 7
#define pin_encoder_in_b_8 8    //-> 11
#define pin_encoder_in_b_sign 9 // 1 is minus

#define pin_encoder_out_dir 0 // 1 is minus
int in_encoder_byte = 0;

void setup()
{
  if (debug)
    Serial.begin(115200);

  // PWM Part
  pinMode(pin_interrupt, INPUT);

  // Servo Part
  pinMode(servo_pin, OUTPUT);
  my_servo.attach(servo_pin);

  // dc-motor Part
  pinMode(pin_dir, OUTPUT);
  pinMode(pin_enable, OUTPUT);
  pinMode(pin_pwm, OUTPUT);

  // encoder part
  pinMode(pin_encoder_out_dir, OUTPUT);

  pinMode(pin_encoder_in_b_1, INPUT_PULLUP);
  pinMode(pin_encoder_in_b_2, INPUT_PULLUP);
  pinMode(pin_encoder_in_b_3, INPUT_PULLUP);
  pinMode(pin_encoder_in_b_4, INPUT_PULLUP);
  pinMode(pin_encoder_in_b_5, INPUT_PULLUP);
  pinMode(pin_encoder_in_b_6, INPUT_PULLUP);
  pinMode(pin_encoder_in_b_7, INPUT_PULLUP);
  pinMode(pin_encoder_in_b_8, INPUT_PULLUP);
  pinMode(pin_encoder_in_b_sign, INPUT_PULLUP);
}

void loop()
{
  in_encoder_byte = read_encoder_byte();
  Serial.println(in_encoder_byte);
  
  // read pulsewidth
  pulse_width = pulseIn(pin_interrupt, HIGH, 40000);

  // write to servo
  write_to_servo();
  // write to dc-motor
  write_to_dc_motor();
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

// PWM without analogWrite
void writePWM(int percent)
{
  int val = 16 * ((float)percent / 100);
  digitalWrite(pin_pwm, HIGH);
  delay(val); // 16/(percent/10)
  digitalWrite(pin_pwm, LOW);
  delay(16 - val);
}


int read_encoder_byte() {
  int return_result = 0;
  int in_byte = 0;

  int in_encoder_b1 = digitalRead(pin_encoder_in_b_1);
  int in_encoder_b2 = digitalRead(pin_encoder_in_b_2);
  int in_encoder_b3 = digitalRead(pin_encoder_in_b_3);
  int in_encoder_b4 = digitalRead(pin_encoder_in_b_4);
  int in_encoder_b5 = digitalRead(pin_encoder_in_b_5);
  int in_encoder_b6 = digitalRead(pin_encoder_in_b_6);
  int in_encoder_b7 = digitalRead(pin_encoder_in_b_7);
  int in_encoder_b8 = digitalRead(pin_encoder_in_b_8);
  int in_encoder_b_sign = digitalRead(pin_encoder_in_b_sign);

  //  in_byte = in_b8*128 + in_b7*64 + in_b6*32 + in_b5*16 + in_b4*8 + in_b3*4 + in_b2*2 + in_b1; // between 0 -> 15
  bitWrite(in_byte, 0, in_encoder_b1);
  bitWrite(in_byte, 1, in_encoder_b2);
  bitWrite(in_byte, 2, in_encoder_b3);
  bitWrite(in_byte, 3, in_encoder_b4);
  bitWrite(in_byte, 4, in_encoder_b5);
  bitWrite(in_byte, 5, in_encoder_b6);
  bitWrite(in_byte, 6, in_encoder_b7);
  bitWrite(in_byte, 7, in_encoder_b8);

  Serial.print(in_encoder_b8);
  Serial.print(in_encoder_b7);
  Serial.print(in_encoder_b6);
  Serial.print(in_encoder_b5);
  Serial.print(in_encoder_b4);
  Serial.print(in_encoder_b3);
  Serial.print(in_encoder_b2);
  Serial.print(in_encoder_b1);
  Serial.print(" ");

  Serial.print(in_encoder_b_sign);

  if (in_encoder_b_sign) {
    //Serial.print("putting MINUS on in_byte bc pin is: ");
    //Serial.println(in_encoder_b_sign);
    return_result = -in_byte;
  }
  else {
    //Serial.print("putting PLUS on in_byte bc pin is: ");
    //Serial.println(in_encoder_b_sign);
    return_result = in_byte;
  }

  return return_result;
}
