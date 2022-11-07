/**
 *  @filename   :   imagedata.cpp
 *  @brief      :   data file for epd demo
 *
 *  Copyright (C) Waveshare     July 17 2020
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
#include "imagedata.h"
#include <avr/pgmspace.h>

const unsigned char IMAGE_DATA[] PROGMEM = { /* 0X01,0X01,0XB4,0X00,0X40,0X00, */
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x00, 
0x7F, 0xC0, 0x00, 0x3F, 0xCF, 0xFF, 0x3F, 0xCF, 
0xFF, 0x3F, 0xCC, 0x03, 0x37, 0xCC, 0x03, 0x33, 
0xCF, 0xFF, 0x33, 0xCF, 0xFF, 0x33, 0xC0, 0x7F, 
0x33, 0xC0, 0x3F, 0x33, 0xC0, 0x3F, 0x33, 0xF0, 
0x3F, 0x33, 0xF0, 0x3F, 0x33, 0xFC, 0x3F, 0x33, 
0xFC, 0x00, 0x33, 0xFF, 0x00, 0x73, 0xFF, 0xFF, 
0xF3, 0xFF, 0xFF, 0xF3, 0xFC, 0x00, 0x03, 0xFE, 
0x00, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };