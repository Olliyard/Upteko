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
#define WIND_IN_FAST 40      // 20 is the correct ~32 sec     12.50%
#define WIND_IN_SLOW 15      // 20 is the correct ~32 sec     12.50%
// PWM exists between 10->90 have slow in the very start, then fast for the most time, then go slow in the end 66% is approx 5s
#define RELEASE_SLOW 12 // 12 is the correct     2.50%
#define RELEASE_MED  30 // 40 is the correct    37.00%
#define RELEASE_FAST 85 // 85 is the correct    93.75%


Servo myservo;  // create servo object to control a servo
// a maximum of eight servo objects can be created
int pin_servo = 10;

int pin_interrupt = 2;

// pin_in is connected to only micro switch but it is possible to wire this to the raspberry pi also
int pin_in_control_n0 = 3;
int pin_in_control_n1 = 4;
int pin_in_control_n2 = 5;
int pin_in_control_n3 = 6;

int pin_encoder_in_b_1 = 19; //-> 4
int pin_encoder_in_b_2 = 18; //-> 5
int pin_encoder_in_b_3 = 17;
int pin_encoder_in_b_4 = 16;
int pin_encoder_in_b_5 = 15;
int pin_encoder_in_b_6 = 14;
int pin_encoder_in_b_7 = 7;
int pin_encoder_in_b_8 = 8;  //-> 11
int pin_encoder_in_b_sign = 9; // 1 is minus

int pin_encoder_out_dir = 0; // 1 is minus

int pin_dir = 11;
int pin_enable = 12;
int pin_pwm = 13;

int pwm_target = 66;  // 66 is the desired speed with 4m on 7.1 s
int pwm_test = 14;
int pwm_zero = 9;

int timer_release_start = 0;
int timer_release_current = 0;
bool bool_timer_release = false;

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

int servo_pos = CLOSE;
int enable = DISABLE;
int dir = CW;
int pwm = 0;

int old_servo_pos = CLOSE;
int old_enable = DISABLE;
int old_dir = CW;
int old_pwm = 0;

int motor_rev_for_4m = 32;//127; // target is 31.8revs for 4m. We have 4 pulses on the encoder so 31.8*4=127.2

// Here it should go all the way in and stop just before there are no more rope on the dogleash
int motor_rev_in_target = -motor_rev_for_4m;//3; // The motor overshoots, so -3 is a good first guess for in
// Make sure it goes all the way out, it is ok if it goes more out
int motor_rev_out_target = motor_rev_for_4m;//10; // The motor overshoots, so +10 is a good first guess for out
int rev_at_startup = 0;
int rev_after_wind_in = 0;
int rev_after_release = 0;

unsigned long int a, b, c;
int x[15], ch1[15], ch[7], i;

void setup()
{
  Serial.begin(9600);

  // Initialize
  SoftPWMBegin();

  // Create and set pin 13 To 0 (off) //& 14 & 15 to 0 (off)
  SoftPWMSet(pin_pwm, 0);

  pinMode(pin_enable, OUTPUT);
  pinMode(pin_dir, OUTPUT);

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

  pinMode(pin_interrupt, INPUT_PULLUP);

  pinMode(pin_in_control_n0, INPUT_PULLUP);
  pinMode(pin_in_control_n1, INPUT_PULLUP);
  pinMode(pin_in_control_n2, INPUT_PULLUP);
  pinMode(pin_in_control_n3, INPUT_PULLUP);

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

  if (ch[1] > 1700 && ch[1] < 2000) { // Signal from remote to wind in (switch up), overwrite the signal
    in_control_nibble = 0b00001101;
  }
  else if (ch[1] < 1300 && ch[1] > 1000) { // Signal from remote to release (switch down), overwrite the signal
    in_control_nibble = 0b00001100;
  }

  //Serial.print("ch[1] is: ");
  //Serial.println(ch[1]);

  //Serial.print("in_control_nibble: ");
  //Serial.println(in_control_nibble);
  
  //Serial.print("rev_at_startup: ");
  //Serial.println(rev_at_startup);

  //Serial.print("rev_after_wind_in: ");
  //Serial.println(rev_after_wind_in);

  //Serial.print("rev_after_release: ");
  //Serial.println(rev_after_release);

  //Serial.print("motor_rev_out_target: ");
  //Serial.println(motor_rev_out_target);

  //Serial.print("motor_rev_in_target: ");
  //Serial.println(motor_rev_in_target);

  //Serial.print("in_encoder_byte: ");
  //Serial.println(in_encoder_byte);

  Serial.print("servo pos: ");
  Serial.println(servo_pos);
  
  Serial.print("pwm: ");
  Serial.println(pwm);

  // switch case in reverse order bc of inernal pullup
  switch (in_control_nibble) {
    case 15: // Reset          1111
      reset();
      break;
    case 14: // Hold rope      1110
      pwm = HOLD;
      dir = CCW;
      enable = ENABLE;
      servo_pos = CLOSE;
      bool_timer_release = false;
      break;
    case 13: // Wind rope in   1101
      //Serial.println("Wind_in");
      // -39                -52              0                   -50                   10
      if (in_encoder_byte > (rev_at_startup + motor_rev_in_target + 15)) { //Do it fast until there are 10 revs left then do it slow
        pwm = WIND_IN_FAST;
      }
      //      -39                -52              0                   -50                   10
      else if (in_encoder_byte > (rev_at_startup + motor_rev_in_target - 5)) { //In the end, do it slow for 7 revs over max
        pwm = WIND_IN_SLOW;
      }
      else {
        pwm = 0;
      }
      dir = CCW;
      enable = ENABLE;
      servo_pos = CLOSE;
      bool_timer_release = false;
      //rev_after_wind_in = in_encoder_byte; // Set the rev_after_wind_in to keep track of where we are
      break;

    case 12: // Release rope   1100 // CORRECT!! 12 = 1100. DID THIS TO CONTROL WITH SWITCH, remember to revert!
      //Serial.print("Release: ");
      //In the following examples start at -14

//      // -39                -52              0                   2
//      if (in_encoder_byte < (rev_at_startup + 1)) { //if we are are in the first 2 (0.5) revs. Start out slow
//        pwm = RELEASE_SLOW; //in the start
//        //Serial.println("slow");
//      }
      //  -40->-29           -52              0                   24
      if (in_encoder_byte < (rev_at_startup + 13)) { //if we are after 12 and before 130
        pwm = RELEASE_FAST; //in the middle
        //Serial.println("fast");
      }
      //      -28->-17            -52              0                  No more rope          -16 ~4 revs ~0.5m
      else if (in_encoder_byte < (rev_at_startup + motor_rev_out_target)) { //if we are after 12 and before 130
        pwm = RELEASE_MED; //at last
        //pwm = RELEASE_SLOW; //in the start
        //Serial.println("med");
      }
      //      -16+               -52               0                  No more rope
      else if (in_encoder_byte < (rev_at_startup + motor_rev_out_target + 7)) { //motor_rev_out_target){ //if we are all the way out
        pwm = 0;
        //Serial.println("none");
      }
      servo_pos = OPEN;
      dir = CW;
      enable = ENABLE;
      //rev_after_release = in_encoder_byte;
      break;
    case 11: // Open servo     1011
      //Serial.println("Open servo");
      servo_pos = OPEN;
      bool_timer_release = false;
    default:

      break;
  }

  digitalWrite(pin_encoder_out_dir, dir); // change dir in encoder to match
  SoftPWMSetPercent(pin_pwm, pwm);
  digitalWrite(pin_dir, dir);
  myservo.write(servo_pos);
  digitalWrite(pin_enable, enable);

  //Serial.println();

  delay(100);
}


int read_encoder_byte() {
  int return_result = 0;
  int in_byte = 0;

  in_encoder_b1 = digitalRead(pin_encoder_in_b_1);
  in_encoder_b2 = digitalRead(pin_encoder_in_b_2);
  in_encoder_b3 = digitalRead(pin_encoder_in_b_3);
  in_encoder_b4 = digitalRead(pin_encoder_in_b_4);
  in_encoder_b5 = digitalRead(pin_encoder_in_b_5);
  in_encoder_b6 = digitalRead(pin_encoder_in_b_6);
  in_encoder_b7 = digitalRead(pin_encoder_in_b_7);
  in_encoder_b8 = digitalRead(pin_encoder_in_b_8);
  in_encoder_b_sign = digitalRead(pin_encoder_in_b_sign);

  //  in_byte = in_b8*128 + in_b7*64 + in_b6*32 + in_b5*16 + in_b4*8 + in_b3*4 + in_b2*2 + in_b1; // between 0 -> 15
  bitWrite(in_byte, 0, in_encoder_b1);
  bitWrite(in_byte, 1, in_encoder_b2);
  bitWrite(in_byte, 2, in_encoder_b3);
  bitWrite(in_byte, 3, in_encoder_b4);
  bitWrite(in_byte, 4, in_encoder_b5);
  bitWrite(in_byte, 5, in_encoder_b6);
  bitWrite(in_byte, 6, in_encoder_b7);
  bitWrite(in_byte, 7, in_encoder_b8);

  //Serial.print(in_encoder_b8);
  //Serial.print(in_encoder_b7);
  //Serial.print(in_encoder_b6);
  //Serial.print(in_encoder_b5);
  //Serial.print(in_encoder_b4);
  //Serial.print(in_encoder_b3);
  //Serial.print(in_encoder_b2);
  //Serial.print(in_encoder_b1);
  //Serial.print(" ");

  //Serial.print(in_encoder_b_sign);

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

int read_nibble_control() {
  byte return_nibble = 0b00000000;

  in_control_n0 = digitalRead(pin_in_control_n0);
  in_control_n1 = digitalRead(pin_in_control_n1);
  in_control_n2 = digitalRead(pin_in_control_n2);
  in_control_n3 = digitalRead(pin_in_control_n3);

  //Serial.print("nibbleread from switch or pi: ");
  //Serial.print(in_control_n3);
  //Serial.print(in_control_n2);
  //Serial.print(in_control_n1);
  //Serial.print(in_control_n0);
  //Serial.println();

  //control signal = pin_in_control_n3*4 + pin_in_control_n2*2 + pin_in_control_n1*1 + pin_in_control_n0; // between 0 -> 15
  bitWrite(return_nibble, 0, in_control_n0);
  bitWrite(return_nibble, 1, in_control_n1);
  bitWrite(return_nibble, 2, in_control_n2);
  bitWrite(return_nibble, 3, in_control_n3);

  return return_nibble;
}

void reset() {
  //Serial.println("Resetting");
  servo_pos = CLOSE;
  enable = DISABLE;
  dir = CW;
  pwm = HOLD;
  bool_timer_release = false;
  rev_at_startup = in_encoder_byte;
  return;
}

void read_me()  {
  //this code reads value from RC reciever from PPM pin (Pin 2 or 3)
  //this code gives channel values from 0-1000 values
  //    -: ABHILASH :-    //
  a = micros(); //store time value a when pin value falling
  c = a - b;  //calculating time inbetween two peaks
  b = a;      //
  x[i] = c;   //storing 15 value in array
  i = i + 1;
  if (i == 15)
  {
    for (int j = 0; j < 15; j++)
    {
      ch1[j] = x[j];
    }
    i = 0;
  }
}//copy store all values from temporary array another array after 15 reading

void read_rc() {
  int i, j, k = 0;
  for (k = 14; k > -1; k--)
  {
    if (ch1[k] > 10000) {
      j = k;
    }
  }  //detecting separation space 10000us in that another array
  for (i = 1; i <= 6; i++)
  {
    ch[i] = (ch1[i + j]);
  }
}     //assign 6 channel values after separation space
