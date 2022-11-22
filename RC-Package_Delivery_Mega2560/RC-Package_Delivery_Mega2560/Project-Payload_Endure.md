# **Payload Endurance Test**

The following documents the software and the performed tests in regards to the `Laerke` drone `package payload`.

## **Generating PWM signal**

---
Using the Arduino platform as a standin for the PixHawk flightcontroller, it was necessary to generate a PWM signal which matches the PWM-output of the PixHawk.
By measuring the output with an oscilloscope, it was possible to determine a PWM signal with a `period of 20ms` and a `frequency of 50Hz`. For this specific application, it was decided that a signal with a varying `pulse width` between 1.2ms and 1.8ms would be the most efficient implementation.

For the endurance test, it was decided upon to make use of the **Bit-Banging** method for pulse modulation, especially as this allows us full control of our frequency and duty-cycle. The Mega2560/Uno platforms have a standard PWM-frequency of ``490Hz/980Hz`` depending on the pin, so with the Bit-Banging method we can easily implement the 50Hz frequency.

The way bit-banging works, is by repeatedly turning the PWM-pin on and off with a delay inbetween on/off states. Below is the implementation used:

>```Arduino
>void setup(){
>    pinMode(PWM_Pin, OUTPUT);
>}
>
>void loop(){
>   digitalWrite(PWM_Pin, HIGH);
>   delayMicroseconds(pulseWidth);
>   digitalWrite(PWM_Pin, LOW);
>   delay(20-pulseWidth);
>}
>```

This produces a PWM-signal with a varying duty cycle between ``1.2ms`` and ``1.8ms``.

## **Connecting the PWM generated signal with the Payload**

---

In order to connect the generated PWM signal with the Payload, we had to create a test-program to be run on the Payload Arduino Nano device. This was needed, as the already implemented code did *not* seem to work as intended, as the servo was not responding to the PWM input, and the DC motor to drive the wrench too was unresponsive.
We took what we could from the `motor_control_based_on_timers_TEST_091122` [Link](https://github.com/Olliyard/Upteko/blob/master/RC-Package_Delivery_Mega2560/RC-Package_Delivery_Mega2560/reference_code/motor_control_based_on_timers_TEST_091122/motor_control_based_on_timers_TEST_091122.ino)

We ended up simply reading the pulse on the correct pin with the pulseIn function. This function is blocking, but this ended up not being a problem as we're currently only working with, and testing for a singular channel.
Again, we decided to use the **bit-banging** method to implement the correct PWM to drive the DC-motor. This was done, as the motor pin was not set on any of the pre-existing Nano PWM-pins.

[The following test program was completed](https://github.com/Olliyard/Upteko/blob/master/RC-Package_Delivery_Mega2560/RC-Package_Delivery_Mega2560/src/main.ino). Note, that this program, unlike the beforementioned timer based program, does currently **NOT** change the motors speed once the direction and PWM signal has been generated.


Tomorrow -> 16-11-2022 look into encoder values.

```C
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

// Servo Setup
#define pin_servo 10
#define pin_interrupt 2
Servo myservo; // create servo object to control a servo

// Control pin Setup
//  pin_in is connected to only micro switch but it is possible to wire this to the raspberry pi also
#define pin_in_control_n0 3
#define pin_in_control_n1 4
#define pin_in_control_n2 5
#define pin_in_control_n3 6

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

int in_encoder_byte = 0;

int servo_pos = CLOSE;
int enable = DISABLE;
int dir = CW;
int pwm = 0;

int old_servo_pos = CLOSE;
int old_enable = DISABLE;
int old_dir = CW;
int old_pwm = 0;

int motor_rev_for_4m = 32; // 127; // target is 31.8revs for 4m. We have 4 pulses on the encoder so 31.8*4=127.2

// Here it should go all the way in and stop just before there are no more rope on the dogleash
int motor_rev_in_target = -motor_rev_for_4m; // 3; // The motor overshoots, so -3 is a good first guess for in
// Make sure it goes all the way out, it is ok if it goes more out
int motor_rev_out_target = motor_rev_for_4m; // 10; // The motor overshoots, so +10 is a good first guess for out
int rev_at_startup = 0;
int rev_after_wind_in = 0;
int rev_after_release = 0;

unsigned long int a, b, c;
int x[15], ch1[15], ch[7], i;
int RC_high_time = micros();
int RC_PWM;
int RC_PWM_stored;

void setup()
{
  Serial.begin(115200);

  pinMode(pin_enable, OUTPUT);
  pinMode(pin_dir, OUTPUT);
  pinMode(pin_pwm, OUTPUT);
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

  pinMode(pin_interrupt, INPUT);

  pinMode(pin_in_control_n0, INPUT_PULLUP);
  pinMode(pin_in_control_n1, INPUT_PULLUP);
  pinMode(pin_in_control_n2, INPUT_PULLUP);
  pinMode(pin_in_control_n3, INPUT_PULLUP);

  pinMode(pin_servo, OUTPUT);
  myservo.attach(pin_servo);
  //attachInterrupt(digitalPinToInterrupt(pin_interrupt), read_me, CHANGE);

  //  delay(300);
  in_encoder_byte = read_encoder_byte();
  rev_at_startup = in_encoder_byte;
}

void loop()
{
  //read_rc();
  RC_PWM_stored = pulseIn(pin_interrupt, HIGH, 40000);
  Serial.println(RC_PWM_stored);
  // Read the nibble from the Pi or switch
  //in_control_nibble = read_nibble_control();
  
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
    break;
  case 13: // Wind rope in   1101
    Serial.print("in_encoder_byte: ");
    Serial.println(in_encoder_byte);
    // -39                -52              0                   -50                   10
    if (in_encoder_byte > (rev_at_startup + motor_rev_in_target + 15))
    { // Do it fast until there are 10 revs left then do it slow
      pwm = WIND_IN_FAST;
    }
    //      -39                -52              0                   -50                   10
    else if (in_encoder_byte > (rev_at_startup + motor_rev_in_target - 5))
    { // In the end, do it slow for 7 revs over max
      pwm = WIND_IN_SLOW;
    }
    else
    {
      pwm = 0;
    }
    dir = CCW;
    enable = ENABLE;
    servo_pos = CLOSE;
    // rev_after_wind_in = in_encoder_byte; // Set the rev_after_wind_in to keep track of where we are
    break;

  case 12: // Release rope   1100 // CORRECT!! 12 = 1100. DID THIS TO CONTROL WITH SWITCH, remember to revert!
    Serial.print("in_encoder_byte: ");
    Serial.println(in_encoder_byte);
    //  -40->-29           -52              0                   24
    if (in_encoder_byte < (rev_at_startup + 13))
    {                     // if we are after 12 and before 130
      pwm = RELEASE_FAST; // in the middle
      // Serial.println("fast");
    }
    //      -28->-17            -52              0                  No more rope          -16 ~4 revs ~0.5m
    else if (in_encoder_byte < (rev_at_startup + motor_rev_out_target))
    {                    // if we are after 12 and before 130
      pwm = RELEASE_MED; // at last
      // pwm = RELEASE_SLOW; //in the start
      // Serial.println("med");
    }
    //      -16+               -52               0                  No more rope
    else if (in_encoder_byte < (rev_at_startup + motor_rev_out_target + 7))
    { // motor_rev_out_target){ //if we are all the way out
      pwm = 0;
      // Serial.println("none");
    }
    servo_pos = OPEN;
    dir = CW;
    enable = ENABLE;
    // rev_after_release = in_encoder_byte;
    break;
  case 11: // Open servo     1011
    // Serial.println("Open servo");
    servo_pos = OPEN;
  default:

    break;
  }

  digitalWrite(pin_encoder_out_dir, dir); // change dir in encoder to match
  writePWM(pin_pwm, 100);                 // write pwm to motor
  digitalWrite(pin_dir, dir);
  myservo.write(servo_pos);
  digitalWrite(pin_enable, enable);

  delay(100);
}

void writePWM(int pin, int percent)
{
  int val = 16 * ((float)percent / 100);
  digitalWrite(pin, HIGH);
  delayMicroseconds(val / 1000); // 16/(percent/10)
  digitalWrite(pin, LOW);
  delayMicroseconds(16000 - (val / 1000));
}

int read_encoder_byte()
{
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

  if (in_encoder_b_sign)
  {
    // Serial.print("putting MINUS on in_byte bc pin is: ");
    // Serial.println(in_encoder_b_sign);
    return_result = -in_byte;
  }
  else
  {
    // Serial.print("putting PLUS on in_byte bc pin is: ");
    // Serial.println(in_encoder_b_sign);
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
```