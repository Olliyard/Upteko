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