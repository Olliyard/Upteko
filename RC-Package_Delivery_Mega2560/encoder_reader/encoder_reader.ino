#include <Arduino.h>
#define readA bitRead(PIND, 2) // faster than digitalRead()

const byte encoderPinA = 2; // outputA digital pin2

volatile long count = 0;
long protectedCount = 0;
long previousCount = 0;
int revs = 0;
int previousRevs = 0;
int dir_in = 0;

const byte pin0 = 4;
const byte pin1 = 5;
const byte pin2 = 6;
const byte pin3 = 7;
const byte pin4 = 8;
const byte pin5 = 9;
const byte pin6 = 10;
const byte pin7 = 11;
const byte pin_sign = 12; // todo, attach this wire 12->8

const byte pin_dir_in = 14;

int dir = 0; // 1 is CCW, 0 is CW

void setup()
{
  // Serial.begin (115200);//115200

  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);

  pinMode(pin0, OUTPUT);
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
  pinMode(pin5, OUTPUT);
  pinMode(pin6, OUTPUT);
  pinMode(pin7, OUTPUT);
  pinMode(pin_sign, OUTPUT);

  pinMode(pin_dir_in, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(encoderPinA), isrA, RISING);

  dir_in = digitalRead(pin_dir_in);
}

void loop()
{
  noInterrupts();
  protectedCount = count;
  interrupts();
  dir_in = digitalRead(pin_dir_in);
  // Serial.print("dir: ");
  // Serial.println(dir_in);

  if (protectedCount >= (10752))
  {
    count = 0;
    revs += 1;
  }
  else if (protectedCount <= (-10752))
  {
    count = 0;
    revs -= 1;
  }

  if (previousCount != protectedCount)
  {
    // Serial.print("protectedCount: ");
    // Serial.println(protectedCount);
  }
  if (previousRevs != revs)
  {
    // Serial.print("revs: ");
    // Serial.println(revs);
    sendRevsInForloop(revs);
  }

  previousCount = protectedCount;
  previousRevs = revs;

  // delay(15);
}

void sendRevsInForloop(int value)
{
  if (value < 0)
  {
    value = ~value + 1;
    digitalWrite(pin_sign, HIGH);
    // Serial.print(1);
  }
  else
  {
    digitalWrite(pin_sign, LOW);
    // Serial.print(0);
  }
  // Serial.print("Sending this rev: ");
  // Serial.println(value);

  byte pin_inv_i = 12;
  for (byte i = 4, mask = 128; i < 12; i++, mask = mask >> 1)
  {
    pin_inv_i--;
    if (value & mask)
    {
      // bit "i" is on
      digitalWrite(pin_inv_i, HIGH);
    }
    else
    {
      // bit "i" is off
      digitalWrite(pin_inv_i, LOW);
    }
  }
}

void isrA()
{
  //  count++;
  if (dir_in)
  {
    count++;
  }
  else
  {
    count--;
  }
}
