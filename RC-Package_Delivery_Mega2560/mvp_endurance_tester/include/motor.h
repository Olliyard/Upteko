#include <SoftPWM.h>
#include <SoftPWM_timer.h>

// MOTOR STATES
#define MOTOR_STOP 0
#define MOTOR_HOLD 1
#define MOTOR_WIND_IN 2
#define MOTOR_WIND_OUT 3

// MOTOR PINS
#define ENABLE_PIN 12
#define DIR_PIN 11
#define PWM_PIN 13

// ENCODER
#define ENCODER_DIR 1
#define ENCODER_P 9
#define ENCODER_B1 19
#define ENCODER_B2 18
#define ENCODER_B3 17
#define ENCODER_B4 16
#define ENCODER_B5 15
#define ENCODER_B6 14
#define ENCODER_B7 7
#define ENCODER_B8 8

// CONTROLLER
#define CONTROL_N0 3
#define CONTROL_N1 4
#define CONTROL_N2 5
#define CONTROL_N3 6

void __motor_stop();
void __motor_hold();
void __motor_wind_in();
void __motor_wind_out();


void motor_setup()
{
    // SoftPWM
    SoftPWMBegin();
    SoftPWMSet(PWM_PIN, 0);

    // Set pin mode
    pinMode(ENABLE_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
    pinMode(PWM_PIN, OUTPUT);

    // Set pin mode encoder
    pinMode(ENCODER_DIR, OUTPUT);
    pinMode(ENCODER_P, INPUT_PULLUP);
    pinMode(ENCODER_B1, INPUT_PULLUP);
    pinMode(ENCODER_B2, INPUT_PULLUP);
    pinMode(ENCODER_B3, INPUT_PULLUP);
    pinMode(ENCODER_B4, INPUT_PULLUP);
    pinMode(ENCODER_B5, INPUT_PULLUP);
    pinMode(ENCODER_B6, INPUT_PULLUP);
    pinMode(ENCODER_B7, INPUT_PULLUP);
    pinMode(ENCODER_B8, INPUT_PULLUP);

    // Set pin mode controller
    pinMode(CONTROL_N0, INPUT_PULLUP);
    pinMode(CONTROL_N1, INPUT_PULLUP);
    pinMode(CONTROL_N2, INPUT_PULLUP);
    pinMode(CONTROL_N3, INPUT_PULLUP);
}

uint8_t read_encoder()
{
    uint8_t result = 0;

    // Read encoder
    uint8_t b_p = digitalRead(ENCODER_P);
    uint8_t b1 = digitalRead(ENCODER_B1);
    uint8_t b2 = digitalRead(ENCODER_B2);
    uint8_t b3 = digitalRead(ENCODER_B3);
    uint8_t b4 = digitalRead(ENCODER_B4);
    uint8_t b5 = digitalRead(ENCODER_B5);
    uint8_t b6 = digitalRead(ENCODER_B6);
    uint8_t b7 = digitalRead(ENCODER_B7);
    uint8_t b8 = digitalRead(ENCODER_B8);

    // Write bits
    bitWrite(result, 0, b1);
    bitWrite(result, 1, b2);
    bitWrite(result, 2, b3);
    bitWrite(result, 3, b4);
    bitWrite(result, 4, b5);
    bitWrite(result, 5, b6);
    bitWrite(result, 6, b7);
    bitWrite(result, 7, b8);

    // Check polarity
    if (b_p == 1)
        result = -result;

    // Return result
    return result;
}

uint8_t read_nibble()
{
    uint8_t result = 0;

    // Read controller
    uint8_t n0 = digitalRead(CONTROL_N0);
    uint8_t n1 = digitalRead(CONTROL_N1);
    uint8_t n2 = digitalRead(CONTROL_N2);
    uint8_t n3 = digitalRead(CONTROL_N3);

    // Write bits
    bitWrite(result, 0, n0);
    bitWrite(result, 1, n1);
    bitWrite(result, 2, n2);
    bitWrite(result, 3, n3);

    // Return result
    return result;
}

void motor_control(uint8_t state)
{
    if (state == MOTOR_STOP)
    {
        __motor_stop();
    }

    else if (state == MOTOR_HOLD)
    {
        __motor_hold();
    }

    else if (state == MOTOR_WIND_IN)
    {
        __motor_wind_in();
    }

    else if (state == MOTOR_WIND_OUT)
    {
        __motor_wind_out();
    }
}

void __motor_stop()
{
    // Set enable pin
    digitalWrite(ENABLE_PIN, LOW);

    // Set direction pin
    digitalWrite(DIR_PIN, LOW);

    // Set pwm pin
    SoftPWMSet(PWM_PIN, 0);
}

void __motor_hold()
{
    // Set enable pin
    digitalWrite(ENABLE_PIN, HIGH);

    // Set direction pin
    digitalWrite(DIR_PIN, LOW);

    // Set pwm pin
    SoftPWMSet(PWM_PIN, 5);
}

void __motor_wind_in()
{
    // Set enable pin
    digitalWrite(ENABLE_PIN, HIGH);

    // Set direction pin
    digitalWrite(DIR_PIN, LOW);

    // Set pwm pin
    SoftPWMSet(PWM_PIN, 255);
}

void __motor_wind_out()
{
    // Set enable pin
    digitalWrite(ENABLE_PIN, HIGH);

    // Set direction pin
    digitalWrite(DIR_PIN, HIGH);

    // Set pwm pin
    SoftPWMSet(PWM_PIN, 255);
}