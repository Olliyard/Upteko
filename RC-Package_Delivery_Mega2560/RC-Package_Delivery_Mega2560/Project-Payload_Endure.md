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
>   delayMicroseconds(lowerLimit);
>   digitalWrite(PWM_Pin, LOW);
>   delayMicroseconds(2000);  
>   digitalWrite(PWM_Pin, HIGH);
>   delayMicroseconds(2000);
>   digitalWrite(PWM_Pin, LOW);
>   delay(20-(lowerLimit/1000)-4);
>}
>```