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

Epd epd;
UBYTE image[44992];  //Image size in Bytes = (width in pixels*height in pixels*bit depth)/8 (8-bit depth)
Paint paint(image, 152, 296);
float r = 0;

void setup() {
  //srand(time(0));
  Serial.begin(9600);
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
}

void loop() {
  paint.Clear(UNCOLORED);
  int offset = (paint.GetWidth()/2)+4;
  //Create string arrays and fill them with i float value.
  char c1[12], c2[12], c3[12], c4[12], c5[12], c6[12], c7[12], c8[12], c9[12], c10[12], c11[12], c12[12];
  char voltage[24], current[24], SoH[24], SoC[24];
    
  if(r < 10){
    snprintf(c1, sizeof(c1), "C01:0%.2fV", r);
    snprintf(c2, sizeof(c2), "C02:0%.2fV", r);
    snprintf(c3, sizeof(c3), "C03:0%.2fV", r);
    snprintf(c4, sizeof(c4), "C04:0%.2fV", r);
    snprintf(c5, sizeof(c5), "C05:0%.2fV", r);
    snprintf(c6, sizeof(c6), "C06:0%.2fV", r);
    snprintf(c7, sizeof(c7), "C07:0%.2fV", r);
    snprintf(c8, sizeof(c8), "C08:0%.2fV", r);
    snprintf(c9, sizeof(c9), "C09:0%.2fV", r);
    snprintf(c10, sizeof(c10), "C10:0%.2fV", r);
    snprintf(c11, sizeof(c11), "C11:0%.2fV", r);
    snprintf(c12, sizeof(c12), "C12:0%.2fV", r);
    snprintf(voltage, sizeof(voltage), "Voltage:0%.2fV", r);
    snprintf(current, sizeof(current), "Current:0%.2fA", r);
  }
  else{
    snprintf(c1, sizeof(c1), "C01:%.2fV", r);      
    snprintf(c2, sizeof(c2), "C02:%.2fV", r);
    snprintf(c3, sizeof(c3), "C03:%.2fV", r);
    snprintf(c4, sizeof(c4), "C04:%.2fV", r);
    snprintf(c5, sizeof(c5), "C05:%.2fV", r);
    snprintf(c6, sizeof(c6), "C06:%.2fV", r);
    snprintf(c7, sizeof(c7), "C07:%.2fV", r);
    snprintf(c8, sizeof(c8), "C08:%.2fV", r);
    snprintf(c9, sizeof(c9), "C09:%.2fV", r);
    snprintf(c10, sizeof(c10), "C10:%.2fV", r);
    snprintf(c11, sizeof(c11), "C11:%.2fV", r);
    snprintf(c12, sizeof(c12), "C12:%.2fV", r);
    snprintf(voltage, sizeof(voltage), "Voltage:%.2fV", r);
    snprintf(current, sizeof(current), "Current:%.2fA", r);
  }
  // ---------- CELL INFO ---------- //
  //Create title and box for voltages
  paint.DrawStringAt(5, 0, "Cell Voltages", &Font16, COLORED);
  paint.DrawHorizontalLine(0, 15, paint.GetWidth(), COLORED);
  paint.DrawHorizontalLine(0, 85, paint.GetWidth(), COLORED);
  paint.DrawVerticalLine(offset-4, 16, 85-16, COLORED);
  //C1 + C7
  paint.DrawStringAt(0, 20, c1, &Font12, COLORED);
  paint.DrawStringAt(offset, 20, c7, &Font12, COLORED);
  //C2 + C8
  paint.DrawStringAt(0, 30, c2, &Font12, COLORED);
  paint.DrawStringAt(offset, 30, c8, &Font12, COLORED);
  //C3 + C9
  paint.DrawStringAt(0, 40, c3, &Font12, COLORED);
  paint.DrawStringAt(offset, 40, c9, &Font12, COLORED);
  //C4 + C10
  paint.DrawStringAt(0, 50, c4, &Font12, COLORED);
  paint.DrawStringAt(offset, 50, c10, &Font12, COLORED);
  //C5 + C11
  paint.DrawStringAt(0, 60, c5, &Font12, COLORED);
  paint.DrawStringAt(offset, 60, c11, &Font12, COLORED);
  //C6 + C12
  paint.DrawStringAt(0, 70, c6, &Font12, COLORED);
  paint.DrawStringAt(offset, 70, c12, &Font12, COLORED);
  // ---------- BATTERY INFO ---------- //
  paint.DrawStringAt(5, 100, "Battery Stats", &Font16, COLORED);
  paint.DrawStringAt(0, 120, voltage, &Font16, COLORED);
  paint.DrawStringAt(0, 140, current, &Font16, COLORED);
  paint.DrawStringAt(0, 160, "    SoH:100%", &Font16, COLORED);
  paint.DrawStringAt(0, 180, "    SoC:100%", &Font16, COLORED);
  //Draw battery -- Later, implement so this changes depending on SoC.
  paint.DrawRectangle(10, 200, 135, 260, COLORED);
  paint.DrawFilledRectangle(135, 215, 145, 245, COLORED);
  paint.DrawFilledRectangle(15, 205, 25, 255, COLORED);
  paint.DrawFilledRectangle(30, 205, 40, 255, COLORED);
  paint.DrawFilledRectangle(45, 205, 55, 255, COLORED);
  paint.DrawFilledRectangle(60, 205, 70, 255, COLORED);
  paint.DrawFilledRectangle(75, 205, 85, 255, COLORED);
  paint.DrawFilledRectangle(90, 205, 100, 255, COLORED);
  paint.DrawFilledRectangle(105, 205, 115, 255, COLORED);
  paint.DrawFilledRectangle(120, 205, 130, 255, COLORED);
  //Display information
  epd.DisplayFrame_part(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
  delay(500);

  r++;
}
