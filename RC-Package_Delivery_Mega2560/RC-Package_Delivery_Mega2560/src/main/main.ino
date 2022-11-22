// for nano
#include <Arduino.h>
// #include <SoftPWM.h>
// #include <SoftPWM_timer.h>

#include <Servo.h>

#define CCW 0
#define CW 1
#define DISABLE 0
#define ENABLE 1
#define CLOSE 10
#define OPEN 110
#define HOLD 5
#define WIND_IN_FAST 40 // 20 is the correct ~32 sec     12.50%
#define WIND_IN_SLOW 15 // 20 is the correct ~32 sec     12.50%
// PWM exists between 10->90 have slow in the very start, then fast for the most time, then go slow in the end 66% is approx 5s
#define RELEASE_SLOW 12 // 12 is the correct     2.50%
#define RELEASE_MED 30  // 40 is the correct    37.00%
#define RELEASE_FAST 85 // 85 is the correct    93.75%

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

#define pin_dir 11
#define pin_enable 12
#define pin_pwm 13

#define pin_interrupt 2

int pwm_target = 66; // 66 is the desired speed with 4m on 7.1 s
int pwm_test = 14;
int pwm_zero = 9;

int timer_release_start = 0;
int timer_release_current = 0;

// Used for controlling the statemachine, RaspberryPi, micro switch and RC signal control this
int in_control_n0 = 1;
int in_control_n1 = 1;
int in_control_n2 = 1;
int in_control_n3 = 1;
byte in_control_nibble = 0b00000000;

int in_encoder_b1;
int in_encoder_b2;
int in_encoder_b3;
int in_encoder_b4;
int in_encoder_b5;
int in_encoder_b6;
int in_encoder_b7;
int in_encoder_b8;
int in_encoder_b_sign;
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

  pinMode(pin_encoder_in_b_1, INPUT);
  pinMode(pin_encoder_in_b_2, INPUT);
  pinMode(pin_encoder_in_b_3, INPUT);
  pinMode(pin_encoder_in_b_4, INPUT);
  pinMode(pin_encoder_in_b_5, INPUT);
  pinMode(pin_encoder_in_b_6, INPUT);
  pinMode(pin_encoder_in_b_7, INPUT);
  pinMode(pin_encoder_in_b_8, INPUT);
  pinMode(pin_encoder_in_b_sign, INPUT_PULLUP);

  pinMode(pin_interrupt, INPUT);

  pinMode(pin_in_control_n0, INPUT_PULLUP);
  pinMode(pin_in_control_n1, INPUT_PULLUP);
  pinMode(pin_in_control_n2, INPUT_PULLUP);
  pinMode(pin_in_control_n3, INPUT_PULLUP);

  pinMode(pin_servo, OUTPUT);
  myservo.attach(pin_servo);
  attachInterrupt(digitalPinToInterrupt(pin_interrupt), read_me, CHANGE);

  //  delay(300);
  in_encoder_byte = read_encoder_byte();
  rev_at_startup = in_encoder_byte;
}

void loop()
{
  read_rc();

  // Read the nibble from the Pi or switch
  in_control_nibble = read_nibble_control();
  // Read the byte from the encoder
  in_encoder_byte = read_encoder_byte();

  if (RC_PWM_stored > 1700 && RC_PWM_stored < 2000)
  { // Signal from remote to release, overwrite the signal
    in_control_nibble = 0b00001101;
  }
  else if (RC_PWM_stored < 1300 && RC_PWM_stored > 1000)
  { // Signal from remote to release, overwrite the signal
    in_control_nibble = 0b00001100;
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

  digitalWrite(pin_encoder_out_dir, dir); // change dir in encoder to match
  writePWM(pin_pwm, pwm);                 // write pwm to motor
  // SoftPWMSetPercent(pin_pwm, pwm);
  digitalWrite(pin_dir, dir);
  myservo.write(servo_pos);
  digitalWrite(pin_enable, enable);

  delay(100);
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

  Serial.println(in_encoder_b_sign);

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

int read_nibble_control()
{
  byte return_nibble = 0b00000000;

  in_control_n0 = digitalRead(pin_in_control_n0);
  in_control_n1 = digitalRead(pin_in_control_n1);
  in_control_n2 = digitalRead(pin_in_control_n2);
  in_control_n3 = digitalRead(pin_in_control_n3);

  // Serial.print("nibbleread from switch or pi: ");
  // Serial.print(in_control_n3);
  // Serial.print(in_control_n2);
  // Serial.print(in_control_n1);
  // Serial.print(in_control_n0);
  // Serial.println();

  // control signal = pin_in_control_n3*4 + pin_in_control_n2*2 + pin_in_control_n1*1 + pin_in_control_n0; // between 0 -> 15
  bitWrite(return_nibble, 0, in_control_n0);
  bitWrite(return_nibble, 1, in_control_n1);
  bitWrite(return_nibble, 2, in_control_n2);
  bitWrite(return_nibble, 3, in_control_n3);

  return return_nibble;
}

void reset()
{
  // Serial.println("Resetting");
  servo_pos = CLOSE;
  enable = DISABLE;
  dir = CW;
  pwm = HOLD;
  rev_at_startup = in_encoder_byte;
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

void read_rc()
{
  int i, j, k = 0;
  for (k = 14; k > -1; k--)
  {
    if (ch1[k] > 10000)
    {
      j = k;
    }
  } // detecting separation space 10000us in that another array
  for (i = 1; i <= 6; i++)
  {
    ch[i] = (ch1[i + j]);
  }
} // assign 6 channel values after separation space
