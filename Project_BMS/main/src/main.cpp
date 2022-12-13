#include <Arduino.h>
// #include <BQ76940.h>
// #include <UI.h>

// float cell_arr[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // BQ76940_init();
  //UI_Init();
  //UI_boot();

}

void loop() {
  // put your main code here, to run repeatedly:
  // for(int i = 0; i < NUMBER_OF_CELLS; i++)
  // {
  //   cell_arr[i] = BQ76940_getCellVoltage(VC1_HI+2*i);
  // }
  
  // Serial.println(BQ76940_getPackVoltage());
  // UI_displayCellInfo(cell_arr);
  // UI_displayBatInfo('V', BQ76940_getPackVoltage());
  // UI_displayBatInfo('C', BQ76940_getCurrent());
  // UI_checkBatLevel();
  // UI_displayBatLevel();
  Serial.println("Hello World");

}