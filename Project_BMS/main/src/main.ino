#include "Display.h"
#include "BQ76940.h"
#include "BQ" // Include the BQ76940 library

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  I2C_init();
  bq76940_init();
  UI_Init();
  //UI_boot();

}

void loop() {
  // put your main code here, to run repeatedly:
}