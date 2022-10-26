int PWM_Pin = 10;
int switchPos = 2;
int switchNeutral = 3;
int switchNeg = 4;
int pulseWidth = 1500;

void setup() {
  // put your setup code here, to run once:
  digitalWrite(switchNeutral, HIGH);


}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(switchPos) == HIGH){
    pulseWidth += 10;
  }

  else if (digitalRead(switchNeg) == HIGH){
    pulseWidth -= 10;
  }

  else {
    pulseWidth = 1500;
  }

  if(pulseWidth > 2000){
    pulseWidth = 2000;
  }

  else if(pulseWidth < 1000){
    pulseWidth = 1000;
  }



}
