#include <Arduino.h>
#include <Servo.h>
#define PWM_Pin 11
#define neutralPin 3
#define switchPos 5
#define switchNeg 4
#define upperLimit 1800
#define lowerLimit 1200
float pulseWidth = 1500;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(PWM_Pin, OUTPUT);
  pinMode(neutralPin, INPUT);
  pinMode(switchPos, INPUT);
  pinMode(switchNeg, INPUT);
}

void loop() {
  //ADD OR SUBTRACT
  if (digitalRead(switchPos) == HIGH){
    pulseWidth = upperLimit;
  }
  else if (digitalRead(switchNeg) == HIGH){
    pulseWidth = lowerLimit;
  }
  else if(digitalRead(neutralPin) == HIGH){
    pulseWidth = 1500;
  }

  digitalWrite(PWM_Pin, HIGH);
  delayMicroseconds(pulseWidth);
  digitalWrite(PWM_Pin, LOW);
  delay(20-(pulseWidth/1000));  // low

}
