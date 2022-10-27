#include <Servo.h>
#define PWM_Pin 10
#define neutralPin 5
#define switchPos 3
#define switchNeg 4
#define upperLimit 2000
#define lowerLimit 1000
float pulseWidth = 1500;
Servo PWM_Out;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(PWM_Pin, OUTPUT);
  pinMode(neutralPin, INPUT);
  pinMode(switchPos, INPUT);
  pinMode(switchNeg, INPUT);
  PWM_Out.attach(PWM_Pin);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Pulse width in ms: ");
  Serial.println(pulseWidth/1000);

  //ADD OR SUBTRACT
  if (digitalRead(switchPos) == HIGH){
    pulseWidth += 100;
  }
  else if (digitalRead(switchNeg) == HIGH){
    pulseWidth -= 100;
  }
  else if(digitalRead(neutralPin) == HIGH){
    pulseWidth = 1500;
  }

  // CHECK LIMITS
  if(pulseWidth >= upperLimit){
    pulseWidth = upperLimit;
  }
  else if(pulseWidth <= lowerLimit){
    pulseWidth = lowerLimit;
  }

  //Generate PWM with servo library
  PWM_Out.writeMicroseconds(pulseWidth);
}
