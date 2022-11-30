#ifndef _I2C
#define _I2C
#include <Wire.h>

#define I2C_ADDR 0x18

void I2C_init();
int I2C_write(uint8_t reg, uint8_t data);
uint8_t I2C_read(uint8_t reg, uint8_t bytesToRead);

#endif