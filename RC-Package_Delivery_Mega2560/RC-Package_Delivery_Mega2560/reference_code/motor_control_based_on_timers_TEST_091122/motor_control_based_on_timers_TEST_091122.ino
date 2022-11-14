#include <Arduino.h>
#include <SoftPWM.h>
#include <SoftPWM_timer.h>

#include <Servo.h>

#define CCW 0
#define CW 1
#define DISABLE 0
#define ENABLE 1
#define CLOSE 10
#define OPEN 110
#define HOLD 5
// 20 is the correct ~32 sec     12.50%
#define WIND_IN_SLOW 15 // 15 is the correct     6.25%   ~0.08m/s
#define WIND_IN_FAST 35 // 35 is the correct    31.25%   ~0.38m/s
// PWM exists between 10->90 have slow in the very start, then fast for the most time, then go slow in the end 66% is approx 5s
#define RELEASE_SLOW 12 // 12 is the correct     2.50%   ~0.03m/s
#define RELEASE_MED 40  // 40 is the correct    37.00%   ~0.47m/s
#define RELEASE_FAST 85 // 85 is the correct    93.75%   ~1.16m/s

Servo myservo; // create servo object to control a servo
               // a maximum of eight servo objects can be created
int pin_servo = 1;

int pin_interrupt = 2;

// pin_in is connected to only micro switch but it is possible to wire this to the raspberry pi also
int pin_in_control_n0 = 3;
int pin_in_control_n1 = 4;
int pin_in_control_n2 = 5;
int pin_in_control_n3 = 6;

int pin_dir = 11;
int pin_enable = 12;
int pin_pwm = 13;

int pin_pwm_test = 14;
int pin_pwm_zero = 15;

int pwm_target = 66; // 66 is the desired speed with 4m on 7.1 s
int pwm_test = 14;
int pwm_zero = 9;

long timer_out_start = 0;
long timer_out_current = 0;
bool bool_timer_out = false;

long timer_in_start = 0;
long timer_in_current = 0;
bool bool_timer_in = false;

// Used for controlling the statemachine, RaspberryPi, micro switch and RC signal control this
int in_control_n0 = 1;
int in_control_n1 = 1;
int in_control_n2 = 1;
int in_control_n3 = 1;
byte in_control_nibble = 0b00000000;

int servo_pos = CLOSE;
int enable = DISABLE;
int dir = CW;
int pwm = 0;

int old_servo_pos = CLOSE;
int old_enable = DISABLE;
int old_dir = CW;
int old_pwm = 0;

unsigned long int a, b, c;
int x[15], ch1[15], ch[7], i;
int RC_high_time = micros();
int RC_PWM;
int RC_PWM_stored;

void setup()
{
  // Serial.begin(9600);

  // Initialize
  SoftPWMBegin();

  // Create and set pin 13 To 0 (off) //& 14 & 15 to 0 (off)
  SoftPWMSet(pin_pwm, 0);

  pinMode(pin_enable, OUTPUT);
  pinMode(pin_dir, OUTPUT);

  pinMode(pin_interrupt, INPUT);

  pinMode(pin_in_control_n0, INPUT_PULLUP);
  pinMode(pin_in_control_n1, INPUT_PULLUP);
  pinMode(pin_in_control_n2, INPUT_PULLUP);
  pinMode(pin_in_control_n3, INPUT_PULLUP);

  myservo.attach(pin_servo);
  attachInterrupt(digitalPinToInterrupt(pin_interrupt), read_me, CHANGE);

  //  delay(300);
}

void loop()
{
  //  RCValue = pulseIn(pin_interrupt, HIGH,25000);

  read_rc();

  // Read the nibble from the Pi
  in_control_nibble = read_nibble_control();
  // Serial.print("in_control_nibble: ");
  // Serial.println(in_control_nibble);

  // Serial.print("pwm: ");
  // Serial.println(pwm);
  // Serial.println(RC_PWM_stored);

  // Serial.println();

  if (RC_PWM_stored > 1700 && RC_PWM_stored < 2000)
  { // Signal from remote to release, overwrite the signal
    in_control_nibble = 0b00001101;
  }
  else if (RC_PWM_stored < 1300 && RC_PWM_stored > 1000)
  { // Signal from remote to release, overwrite the signal
    in_control_nibble = 0b00001100;
  }
  //  if(ch[1] > 1700 && ch[1] < 2000){  // Signal from remote to release, overwrite the signal
  //    in_control_nibble = 0b00001101;
  //  }
  //  else if(ch[1] < 1300 && ch[1] > 1000){  // Signal from remote to release, overwrite the signal
  //    in_control_nibble = 0b00001100;
  //  }

  // switch case in reverse order bc of inernal pullup
  switch (in_control_nibble)
  {
  case 15: // Reset          1111
    reset();
    break;
  case 14: // Hold rope      1110
    pwm = HOLD;
    dir = CCW;
    enable = ENABLE;
    servo_pos = CLOSE;
    bool_timer_out = false;
    bool_timer_in = false;
    break;
  case 13: // Wind rope in   1101
    // Serial.println("Wind_in");
    if (bool_timer_in == false)
    {
      bool_timer_in = true;
      timer_in_start = millis() / 100;
    }
    timer_in_current = millis() / 100;

    if (timer_in_current < (timer_in_start + 112))
    { // 10 sec * 0.38m/s = 3.8m -> 4m-3.8m=0.2m  // 100
      pwm = WIND_IN_FAST;
    }
    else if (timer_in_current < (timer_in_start + 154))
    { // 0.2m / 0.08m/s = 3.42m = 2.5s       // 130
      pwm = WIND_IN_SLOW;
    }
    else
    { // Stop automatic if 400ms has passed
      pwm = 0;
    }
    dir = CCW;
    enable = ENABLE;
    servo_pos = CLOSE;
    bool_timer_out = false;
    // rev_after_wind_in = in_encoder_byte; // Set the rev_after_wind_in to keep track of where we are
    break;
  case 12: // Release rope   1100 // CORRECT!! 12 = 1100. DID THIS TO CONTROL WITH SWITCH, remember to revert!
    // Serial.print("Release: ");
    if (bool_timer_out == false)
    {
      bool_timer_out = true;
      timer_out_start = millis() / 100;
    }
    timer_out_current = millis() / 100;

    if (timer_out_current < (timer_out_start + 10))
    {                     // 1 sec * 0.03m/s = 0.03m -> 4m-0.03m=3.97m
      pwm = RELEASE_SLOW; // in the start
      // Serial.println("slow");
    }
    else if (timer_out_current < (timer_out_start + 70))
    {                     //(3.97m - 0.5m) / 1.16m/s = 2.99s ~3s => 0.5m
      pwm = RELEASE_FAST; // in the middle
      // Serial.println("fast");
    }
    else if (timer_out_current < (timer_out_start + 99))
    {                    // 0.5m / 0.47m/s = 1.06s -> 0m
      pwm = RELEASE_MED; // at last
      // Serial.println("med");
    }
    else
    {
      pwm = 0;
      // Serial.println("none");
    }
    servo_pos = OPEN;
    dir = CW;
    enable = ENABLE;
    bool_timer_in = false;

    // rev_after_release = in_encoder_byte;
    break;
  case 11: // Open servo     1011
    servo_pos = OPEN;
    bool_timer_out = false;
    bool_timer_in = false;
    break;
  default:

    break;
  }

  Serial.print("servo pos: ");
  Serial.println(servo_pos);

  Serial.print("pwm: ");
  Serial.println(pwm);

  SoftPWMSetPercent(pin_pwm, pwm);
  digitalWrite(pin_dir, dir);
  myservo.write(servo_pos);
  digitalWrite(pin_enable, enable);

  delay(100);
}

int read_nibble_control()
{
  byte return_nibble = 0b00000000;

  in_control_n0 = digitalRead(pin_in_control_n0);
  in_control_n1 = digitalRead(pin_in_control_n1);
  in_control_n2 = digitalRead(pin_in_control_n2);
  in_control_n3 = digitalRead(pin_in_control_n3);

  // Serial.print("nibbleread: ");
  // Serial.print(in_control_n0);
  // Serial.print(in_control_n1);
  // Serial.print(in_control_n2);
  // Serial.print(in_control_n3);
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
  servo_pos = CLOSE;
  enable = DISABLE;
  dir = CW;
  pwm = HOLD;
  bool_timer_out = false;
  bool_timer_in = false;
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

  // //this code reads value from RC reciever from PPM pin (Pin 2 or 3)
  // //this code gives channel values from 0-1000 values
  // //    -: ABHILASH :-    //
  //  a=micros(); //store time value a when pin value falling
  //  c=a-b;      //calculating time inbetween two peaks
  //  b=a;        //
  //  x[i]=c;     //storing 15 value in array
  //  i=i+1;
  //  if(i==15)
  //  {
  //    for(int j=0;j<15;j++)
  //    {
  //      ch1[j]=x[j];
  //    }
  //    i = 0;
  //  }
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
