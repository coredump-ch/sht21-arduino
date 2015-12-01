/**
 * Sensirion SHT21 humidity and temperature arduino example application.
 * 
 * This is an example app that prints the relative humidity as well as the temperature 
 * every second to the serial port.
 * 
 * Some Parts of this code are based on the SHT21 humidity and temperature sensor
 * driver for Linux by Urs Fleisch.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include <Wire.h>

const byte TEMPERATURE = 0xe3;
const byte HUMIDITY = 0xe5;

const byte INVALID_DATA = -300;

void setup() {
  Wire.begin();                // join i2c bus as master
  Serial.begin(9600);          // start serial communication at 9600bps

}

void loop() {
  Serial.println("Humidity is: " + getMeasurement(HUMIDITY));
  Serial.println("Temperature is: " + getMeasurement(TEMPERATURE));
  delay(1000);
}

long getMeasurement(byte type)
{
  
  Wire.beginTransmission(0x40);   // transmit to device 0x40 (SHT21)
  Wire.write(type);               // Trigger measurement based on the given type
  Wire.endTransmission();

  
  Wire.requestFrom(0x40, 3);      // request 3 bytes from slave device
 
  if (3 == Wire.available()) {    // if three  bytes were received
    
    byte reading[] = {0,0,0};  
    reading[0] = Wire.read();     // First part of the measurement
    reading[1] = Wire.read();     // Second part of the measurement
    reading[2] = Wire.read();     // CRC checksum

    if(isValidateCRC(reading, 2, reading[2])){

      // Put the effective measured value together
      u16 ticks = reading[0] << 8;
      ticks |= reading[1];

      // Calculate the 
      switch (type) {
        case TEMPERATURE:
          // Temparature in milli celsius
          return ((21965 * long(ticks)) >> 13) - 46850;
         
         case HUMIDITY:
           // one-thousandths of a percent relative humidity
          return (((15625 * long(ticks)) >> 13) - 6000);   
      }    
    }
  }
  return INVALID_DATA;
}

byte isValidateCRC(byte data[], byte nbrOfBytes, byte checksum)
{
  byte crc = 0;
  byte byteCtr;
  
  for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr) {
    crc ^= (data[byteCtr]);
    for (byte bit = 8; bit > 0; --bit) {
      if (crc & 0x80)
        crc = (crc << 1) ^ 0x131;
      else
        crc = (crc << 1);
    }
  }
  return crc == checksum;
}
