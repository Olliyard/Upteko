// Allows floats to be output with print statements (snprintf included)
asm(".global _printf_float");

#include <SPI.h>
#include <time.h>
#include <stdlib.h>
#include "epd2in66.h"
#include "imagedata.h"
#include "epdpaint.h"

#define COLORED     0
#define UNCOLORED   1
#define MAX_CELLS   12

void batLevel(int charge);
void UI_Init();
void displayCellInfo(char buf, float cell_arr);

Epd epd;
UBYTE image[44992];  //Image size in Bytes = (width in pixels*height in pixels*bit depth)/8 (8-bit depth)
Paint paint(image, 152, 296);


char c1[12], c2[12], c3[12], c4[12], c5[12], c6[12], c7[12], c8[12], c9[12], c10[12], c11[12], c12[12];
char voltage[24], current[24], SoH[9], SoC[9];
float r = 0;
uint16_t bat_Charge = 100, bat_Health = 100;
float bat_div = 12.5;

float cell_values[12];
char* cell_buf[12];

void setup() {
  Serial.begin(9600);
  UI_Init();
  for(int i = 0; i<=12; i++){
    cell_values[i] = i;
  }
}

void loop() {
  paint.Clear(UNCOLORED);

  // ---------- CELL INFO ---------- //
  displayCellInfo(cell_buf, *cell_values);

  // ---------- BATTERY INFO ---------- //
  // snprintf(voltage, sizeof(voltage), "Volts:0%.2fV", r);
  // snprintf(current, sizeof(current), "Amps:0%.2fA", r);
  // snprintf(SoH, sizeof(SoH), "SoH:0%d%c", bat_Health, '%');
  // snprintf(SoC, sizeof(SoC), "SoC:0%d%c", bat_Charge, '%');


  // snprintf(voltage, sizeof(voltage), "Volts:%.2fV", r);
  // snprintf(current, sizeof(current), "Amps:%.2fA", r);
  // snprintf(SoH, sizeof(SoH), "SoH:%d%c", bat_Health, '%');
  // snprintf(SoC, sizeof(SoC), "SoC:%d%c", bat_Charge, '%');
  // paint.DrawStringAt(5, 100, "Battery Stats", &Font16, COLORED);
  // paint.DrawStringAt(5, 120, voltage, &Font16, COLORED);
  // paint.DrawStringAt(15, 140, current, &Font16, COLORED);
  // paint.DrawStringAt(25, 160, SoH, &Font16, COLORED);
  // paint.DrawStringAt(25, 180, SoC, &Font16, COLORED);
  // paint.DrawStringAt(0, 280, "Refresh rate: 1.2ms", &Font12, COLORED);

  //Draw battery -- Later, implement so this changes depending on SoC.
  paint.DrawRectangle(10, 200, 135, 260, COLORED);
  paint.DrawFilledRectangle(135, 215, 145, 245, COLORED);
  
  if(bat_Charge >=100-bat_div){
    batLevel(8);
  }
  else if(bat_Charge <= 100-bat_div && bat_Charge > 100-(2*bat_div)){
    batLevel(7);
  }
  else if(bat_Charge <= 100-(2*bat_div) && bat_Charge > 100-(3*bat_div)){
    batLevel(6);
  }
  else if(bat_Charge <= 100-(3*bat_div) && bat_Charge > 100-(4*bat_div)){
    batLevel(5);
  }
  else if(bat_Charge <= 100-(4*bat_div) && bat_Charge > 100-(5*bat_div)){
    batLevel(4);
  }
  else if(bat_Charge <= 100-(5*bat_div) && bat_Charge > 100-(6*bat_div)){
    batLevel(3);
  }
  else if(bat_Charge <= 100-(6*bat_div) && bat_Charge > 100-(7*bat_div)){
    batLevel(2);
  }
  else if(bat_Charge <= 100-(7*bat_div) && bat_Charge > 100-(8*bat_div)){
    batLevel(1);
  }
  else{
    batLevel(0);
  }
  

  //Display information
  epd.DisplayFrame(paint.GetImage());
  //delay(500);

  r++;
  bat_Charge--;
}


void batLevel(int charge_left){
  int x = 15;
  for(int i = 0; i < charge_left; i++){
    paint.DrawFilledRectangle(x, 205, x+10, 255, COLORED);
    x += 15;
  }
}


void UI_Init(){
  if (epd.Init() != 0) {
    Serial.print("e-Paper init failed...");
    return;
  }

  if (epd.Init_Partial() != 0) {
    Serial.print("e-Paper init failed...");
    return;
  }
  epd.Clear();
  paint.SetRotate(ROTATE_0);
  epd.DisplayFrame(upteko_frame_1);
  delay(200);
  //epd.Clear();
  epd.DisplayFrame(upteko_frame_2);
  delay(200);
  //epd.Clear();
  epd.DisplayFrame(upteko_frame_3);
  delay(200);
  epd.Clear();
}


void displayCellInfo(char buf[], float cell_arr[]){
for(float i = 0; i<MAX_CELLS; i+=1)
  if(cell_arr[i] < 10){
    snprintf(buf[i], sizeof(buf[i]), "C0%d:0%.2fV", i, cell_arr[i]);
    if (i >= 10){
      snprintf(buf[i], sizeof(buf[i]), "C%d:0%.2fV", i, cell_arr[i]); 
    }

  }
  else (cell_arr[i] > 10){
    snprintf(buf[i], sizeof(buf[i]), "C0%d:%.2fV", i, cell_arr[i]);
    if (i >= 10){
      snprintf(buf[i], sizeof(buf[i]), "C%d:%.2fV", i, cell_arr[i]); 
    }
  }

  //Create title and box for voltages
  paint.DrawStringAt(5, 0, "Cell Voltages", &Font16, COLORED);
  paint.DrawHorizontalLine(0, 15, paint.GetWidth(), COLORED);
  paint.DrawHorizontalLine(0, 85, paint.GetWidth(), COLORED);
  paint.DrawVerticalLine((paint.GetWidth()/2)-4, 16, 85-16, COLORED);

  //C1 + C7
  paint.DrawStringAt(0, 20, buf[0], &Font12, COLORED);
  paint.DrawStringAt((paint.GetWidth()/2)+4, 20, buf[6], &Font12, COLORED);
  //C2 + C8
  paint.DrawStringAt(0, 30, buf[1], &Font12, COLORED);
  paint.DrawStringAt((paint.GetWidth()/2)+4, 30, buf[7], &Font12, COLORED);
  //C3 + C9
  paint.DrawStringAt(0, 40, buf[2], &Font12, COLORED);
  paint.DrawStringAt((paint.GetWidth()/2)+4, 40, buf[8], &Font12, COLORED);
  //C4 + C10
  paint.DrawStringAt(0, 50, buf[3], &Font12, COLORED);
  paint.DrawStringAt((paint.GetWidth()/2)+4, 50, buf[9], &Font12, COLORED);
  //C5 + C11
  paint.DrawStringAt(0, 60, buf[4], &Font12, COLORED);
  paint.DrawStringAt((paint.GetWidth()/2)+4, 60, buf[10], &Font12, COLORED);
  //C6 + C12
  paint.DrawStringAt(0, 70, buf[5], &Font12, COLORED);
  paint.DrawStringAt((paint.GetWidth()/2)+4, 70, buf[11], &Font12, COLORED);
}

void setCellInfo(){

}





