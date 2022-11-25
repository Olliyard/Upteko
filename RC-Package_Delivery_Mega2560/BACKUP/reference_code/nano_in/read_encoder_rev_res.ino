#define readA bitRead(PIND, 2)//faster than digitalRead()
#define readB bitRead(PIND, 3)//faster than digitalRead()

const byte encoderPinA = 2;//outputA digital pin2
const byte encoderPinB = 3;//outoutB digital pin3
volatile long count = 0;
long protectedCount = 0;
long previousCount = 0;
int revs = 0;
int previousRevs = 0;

int aState = 0;
int aLastState = 0;
int bState = 0;
int bLastState = 0;

int pin0 = 4;
int pin1 = 5;
int pin2 = 6;
int pin3 = 7;
int pin4 = 8;
int pin5 = 9;
int pin6 = 10;
int pin7 = 11;
int pin_sign = 12; //todo, attach this wire 12->8

void setup() {
  //Serial.begin (230400);//115200

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
  
  attachInterrupt(digitalPinToInterrupt(encoderPinA), isrA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), isrB, CHANGE);
}

void loop() {
  noInterrupts();
  protectedCount = count;
  interrupts();

  if(protectedCount >= 10752){
    count = 0;
    revs += 1;
  }
  else if(protectedCount <= -10752){
    count = 0;
    revs -= 1;
  }
 
  if(previousRevs != revs) {
    sendRevsInForloop(revs);
    //Serial.println(revs);
  } 
  previousCount = protectedCount;
  previousRevs = revs;

}

void sendRevsInForloop(int value) {
  if(value < 0){
    value = ~value + 1;
    digitalWrite(pin_sign, HIGH);
  }
  else{
    digitalWrite(pin_sign, LOW);
  }

  int pin_inv_i = 12;
  for(int i = 4, mask = 128; i < 12; i++, mask = mask >> 1) {
    pin_inv_i --;
    if (value & mask) {
      // bit "i" is on
      digitalWrite(pin_inv_i, HIGH);
    }
    else {
      // bit "i" is off
      digitalWrite(pin_inv_i, LOW);
    }
  }
}
void isrA() {
  if (readA != aLastState){
    if(bState != readA){
      count ++;
    }
    else{
      count --;
    }
  }
  aLastState = readA;
}

void isrB() {
  bState = readB;
}
