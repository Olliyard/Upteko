#include "Arduino.h"
#include <Servo.h>
#define PWM_Pin 10
#define neutralPin 5
#define switchPos 3
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
  // put your main code here, to run repeatedly:
  // Serial.print("Pulse width in ms: ");
  // Serial.println(pulseWidth/1000);

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

  //PWM for S1
  // digitalWrite(PWM_Pin, HIGH);
  // delayMicroseconds(pulseWidth);
  // digitalWrite(PWM_Pin, LOW);
  // delay(20-(pulseWidth/1000));
  
  //PWM for S2
  // digitalWrite(PWM_Pin, HIGH);
  // delayMicroseconds(2000);
  // digitalWrite(PWM_Pin, LOW);
  // delay(20-2);

  //Combine the two:
  //S1
  digitalWrite(PWM_Pin, HIGH);
  delayMicroseconds(lowerLimit);
  digitalWrite(PWM_Pin, LOW);
  delayMicroseconds(2000);  // low
  //S2
  digitalWrite(PWM_Pin, HIGH);
  delayMicroseconds(2000);  // high
  digitalWrite(PWM_Pin, LOW);
  delay(20-(lowerLimit/1000)-4);  //The total sum of all delays [pulsewidth (expressed in miliseconds), 2000us low signal and 2000us S2 high]

}
