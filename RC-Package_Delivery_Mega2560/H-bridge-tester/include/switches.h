// Defines the pins for the switches and the functions to read them
#define neutralPin 3
#define switchPos 5
#define switchNeg 4

// Define return values for the switches
#define SWITCH_NEUTRAL 0
#define SWITCH_POS 1
#define SWITCH_NEG 2

// Functions
void switches_setup();
uint8_t read_switches();

// Setup the switches
void switches_setup()
{
    pinMode(neutralPin, INPUT);
    pinMode(switchPos, INPUT);
    pinMode(switchNeg, INPUT);
}

// Read the switches and return the state
uint8_t read_switches()
{
    if (digitalRead(switchPos) == HIGH)
        return SWITCH_POS;
    else if (digitalRead(switchNeg) == HIGH)
        return SWITCH_NEG;
    else if (digitalRead(neutralPin) == HIGH)
        return SWITCH_NEUTRAL;
    else
        return SWITCH_NEUTRAL;
}