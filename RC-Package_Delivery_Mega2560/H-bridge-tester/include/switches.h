// Defines the pins for the switches and the functions to read them
#define neutralPin 3
#define switchPos 5
#define switchNeg 4

// Define return values for the switches
#define SWITCH_NEUTRAL 1
#define SWITCH_POS 2
#define SWITCH_NEG 3

uint8_t pressed_switch = 0;

// Functions
void switches_setup();
void read_switches();

// Setup the switches
void switches_setup()
{
    pinMode(neutralPin, INPUT);
    pinMode(switchPos, INPUT);
    pinMode(switchNeg, INPUT);
}

// Read the switches and return the state
void read_switches()
{
    if (digitalRead(switchPos) == HIGH)
        pressed_switch = SWITCH_POS;

    else if (digitalRead(switchNeg) == HIGH)
        pressed_switch = SWITCH_NEG;

    else if (digitalRead(neutralPin) == HIGH)
        pressed_switch = SWITCH_NEUTRAL;
}