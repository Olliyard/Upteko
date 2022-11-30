#ifndef _UI
#define _UI

#include "epd2in66.h"
#include "imagedata.h"
#include "epdif.h"
#include "epdpaint.h"
#include "fonts.h"
#include "imagedata.h"

#define COLORED 0
#define UNCOLORED 1
#define MAX_CELLS 12
#define BOOT 1

Epd epd;
UBYTE image[44992];           // Image size in Bytes = (width in pixels*height in pixels*bit depth)/8 (8-bit depth)
Paint paint(image, 152, 296); // Image size in pixels = (width 152, height 296)

// Initializes the UI
// Sets up the display and paints the background
void UI_Init()
{
  if (epd.Init() != 0)
  {
    Serial.print("e-Paper init failed...");
    return;
  }

  if (epd.Init_Partial() != 0)
  {
    Serial.print("e-Paper init failed...");
    return;
  }
  epd.Clear();
  paint.SetRotate(ROTATE_0);
}

// Displays the boot screen
void UI_boot()
{
  epd.DisplayFrame(upteko_frame_1);
  delay(200);
  // epd.Clear();
  epd.DisplayFrame(upteko_frame_2);
  delay(200);
  // epd.Clear();
  epd.DisplayFrame(upteko_frame_3);
  delay(200);
  epd.Clear();
}

// Takes a float array of cell voltages and displays them on the screen
void UI_displayCellInfo(float cell_arr[])
{
  char *buf = (char *)malloc(12); // Allocate memory for the char array

  for (int i = 0; i < MAX_CELLS; i++)
  {
    if (cell_arr[i] < 10)
    { // If cell voltage is less than 10, add a 0 to the front
      snprintf(buf, 12, "C0%d:0%.2fV", i + 1, cell_arr[i]);
      if (9 <= i)
      { // If cell number is 10 or greater, shift text over
        snprintf(buf, 12, "C%d:0%.2fV", i + 1, cell_arr[i]);
      }
    }
    else
    { // If cell voltage is greater than 10, don't add a 0 to the front
      snprintf(buf, 12, "C0%d:%.2fV", i + 1, cell_arr[i]);
      if (9 <= i)
      { // If cell number is 10 or greater, shift text over
        snprintf(buf, 12, "C%d:%.2fV", i + 1, cell_arr[i]);
      }
    }
    if (i < 6)
    {
      paint.DrawStringAt(0, 20 + (i * 10), buf, &Font12, COLORED);
    }
    else if (i >= 6)
    {
      paint.DrawStringAt((paint.GetWidth() / 2) + 4, 20 + ((i - 6) * 10), buf, &Font12, COLORED);
    }
  }
  free(buf); // Free memory
}

// Takes a float and displays it on the screen
void UI_displayBatInfo(char type, float data)
{
  int x = 0, y = 0;
  char vbuf[24] = {}; // Allocate memory for the char array
  switch (type)
  {
  case 'V':
  case 'v':
  case '0': // Voltage
    x = 5;
    y = 120;
    snprintf(vbuf, sizeof(vbuf), "Volts:%.2fV", data);
    break;
  case 'C':
  case 'c':
  case '1': // Current
    x = 15;
    y = 140;
    snprintf(vbuf, sizeof(vbuf), "Amps:%.2fA", data);
    break;
  case 'H':
  case 'h':
  case '2': // State of Health
    x = 25;
    y = 160;
    snprintf(vbuf, sizeof(vbuf), "SoH:%d%c", (int)data, '%');
    break;
  case 'S':
  case 's':
  case '3': // State of Charge
    x = 25;
    y = 180;
    snprintf(vbuf, sizeof(vbuf), "SoC:%d%c", (int)data, '%');
    break;
  default:
    Serial.println("Invalid type");
    return;
  }

  paint.DrawStringAt(x, y, vbuf, &Font16, COLORED);
}


// Displays the battery level
void UI_displayBatLevel(int charge_left)
{
  int x = 15;
  for (int i = 0; i < charge_left; i++)
  {
    paint.DrawFilledRectangle(x, 205, x + 10, 255, COLORED);
    x += 15;
  }
}


// Checks the battery level and updates the UI accordingly
void UI_checkBatLevel(uint16_t charge, uint16_t health)
{
  float bat_div = 12.5;

  // Draw battery -- Later, implement so this changes depending on SoC.
  paint.DrawRectangle(10, 200, 135, 260, COLORED);
  paint.DrawFilledRectangle(135, 215, 145, 245, COLORED);

  if (charge >= 100 - bat_div)
  {
    UI_displayBatLevel(8);
  }
  else if (charge <= 100 - bat_div && charge > 100 - (2 * bat_div))
  {
    UI_displayBatLevel(7);
  }
  else if (charge <= 100 - (2 * bat_div) && charge > 100 - (3 * bat_div))
  {
    UI_displayBatLevel(6);
  }
  else if (charge <= 100 - (3 * bat_div) && charge > 100 - (4 * bat_div))
  {
    UI_displayBatLevel(5);
  }
  else if (charge <= 100 - (4 * bat_div) && charge > 100 - (5 * bat_div))
  {
    UI_displayBatLevel(4);
  }
  else if (charge <= 100 - (5 * bat_div) && charge > 100 - (6 * bat_div))
  {
    UI_displayBatLevel(3);
  }
  else if (charge <= 100 - (6 * bat_div) && charge > 100 - (7 * bat_div))
  {
    UI_displayBatLevel(2);
  }
  else if (charge <= 100 - (7 * bat_div) && charge > 100 - (8 * bat_div))
  {
    UI_displayBatLevel(1);
  }
  else
  {
    UI_displayBatLevel(0);
  }
}

#endif