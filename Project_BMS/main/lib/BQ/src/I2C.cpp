#include "I2C.h"

void I2C_init()
{
  Wire.begin();
  Wire.setClock(100000);
  Wire.beginTransmission(I2C_ADDR);
  Wire.endTransmission();
}

int I2C_write(uint8_t reg, uint8_t data)
{
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
  return 0;
}

uint8_t I2C_read(uint8_t reg, uint8_t bytesToRead)
{
  uint16_t data;
  byte MSB;
  byte LSB;
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDR, bytesToRead);
  if(bytesToRead == 1)
  {
    data = Wire.read();
  }
  else if(bytesToRead == 2)
  {
    MSB = Wire.read();
    LSB = Wire.read();
    data = (MSB << 8) | LSB;
  }
  return data;
}