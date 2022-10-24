// servo variables

#include <Servo.h> // include the servo library to control the servos

Servo servo1;   // name each servo output for use with the servo library    
Servo servo2;      

// Each servo must be attached to a pin that has a PWM output
// on the arduino uno, nano and pro mini these pins are 3, 5, 6, 9, 10 and 11

const int servo1_pin = 9;  // identify the pins that each servo signal wire is connected to
const int servo2_pin = 10;

// Select Servo Direction, Rates and Sub-trim (the size of each array must match the number of servos)

boolean servo_dir[] = {0,1};     // Direction: 0 is normal, 1 is reverse
float servo_rates[] = {1,0.5};  // Rates: range 0 to 2 (1 = +-500us (NORMAL), 2 = +-1000us (MAX)): The amount of servo deflection in both directions
float servo_subtrim[] = {0.0,0.0};  // Subtrimrange -1 to +1 (-1 = 1000us, 0 = 1500us, 1 = 2000us): The neutral position of the servo
boolean servo_mix_on = true;

unsigned long now;                  // timing variables to update data at a regular interval                        
unsigned long rc_update;

// Receiver variables

const int channels = 6;                   // specify the number of receiver channels
float RC_in[channels];                    // an array to store the calibrated input from receiver 

void setup() {
    
    servo1.attach(servo1_pin, 500, 2500); // attach the servo library to each servo pin, and define min and max uS values
    servo2.attach(servo2_pin, 500, 2500);
    
    setup_pwmRead();                      
    Serial.begin(9600);
}

void loop() {  
    
    now = millis();
    
    if(RC_avail() || now - rc_update > 25){   // if RC data is available or 25ms has passed since last update (adjust to > frame rate of receiver)
      
      rc_update = now;                           
      
      print_RCpwm();                        // uncommment to print raw data from receiver to serial
      
      for (int i = 0; i<channels; i++){       // run through each RC channel
        int CH = i+1;
        
        RC_in[i] = RC_decode(CH);             // decode receiver channel and apply failsafe
        
        //print_decimal2percentage(RC_in[i]);   // uncomment to print calibrated receiver input (+-100%) to serial       
      }
      //Serial.println();                       // uncomment when printing calibrated receiver input to serial.

      
      
      int servo1_uS;      // variables to store the pulse widths to be sent to the servo
      int servo2_uS;      
      
      if (servo_mix_on == true){              // MIXING ON
        
        float mix1 = RC_in[1] - RC_in[2];     // Channel 2 (ELV) - Channel 3 (AIL)
        float mix2 = RC_in[1] + RC_in[2];     // Channel 2 (ELV) + Channel 3 (AIL)
  
        if(mix1 > 1) mix1 = 1;                // limit mixer output to +-1
        else if(mix1 < -1) mix1 = -1;
  
        if(mix2 > 1) mix2 = 1;                // limit mixer output to +-1
        else if(mix2 < -1) mix2 = -1;  
  
        // Calculate the pulse widths for the servos
      
        servo1_uS = calc_uS(mix1, 1);         // apply the servo rates, direction and sub_trim for servo 1, and convert to a RC pulsewidth (microseconds, uS)
        servo2_uS = calc_uS(mix2, 2);         // apply the servo rates, direction and sub_trim for servo 2, and convert to a RC pulsewidth (microseconds, uS)
            
      }
      else{                                   // MIXING OFF
        servo1_uS = calc_uS(RC_in[1],1);      // apply the servo rates, direction and sub_trim for servo 1, and convert to a RC pulsewidth (microseconds, uS)
        servo2_uS = calc_uS(RC_in[2],2);      // apply the servo rates, direction and sub_trim for servo 1, and convert to a RC pulsewidth (microseconds, uS)
      }

      servo1.writeMicroseconds(servo1_uS);   // write the pulsewidth to the servo.
      servo2.writeMicroseconds(servo2_uS);   // write the pulsewidth to the servo. 
    }
}

int calc_uS(float cmd, int servo){                                // cmd = commanded position +-100% 
                                                                  // servo = servo num (to apply correct direction, rates and trim)
  int i = servo-1;
  float dir;
  if(servo_dir[i] == 0) dir = -1; else dir = 1;                   // set the direction of servo travel
  
  cmd = 1500 + (cmd*servo_rates[i]*dir + servo_subtrim[i])*500;   // apply servo rates and sub trim, then convert to a uS value

  if(cmd > 2500) cmd = 2500;                                      // limit pulsewidth to the range 500 to 2500us
  else if(cmd < 500) cmd = 500;

  return cmd;
}
