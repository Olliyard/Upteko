void setup() {
    setup_pwmRead();                      
    Serial.begin(9600);
}

void loop() {  
    
    // Print RC receiver frame length and frame rate
 
    if (PWM_read(1)){                                      // if a new pulse is detected on channel 1
      Serial.print(PWM_period(),0);Serial.print("uS ");     
      Serial.print(PWM_freq());Serial.println("Hz");
    }

}
