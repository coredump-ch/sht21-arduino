
#include <Wire.h>

const u16 POLYNOMIAL = 0x131; //P(x)=x^8+x^5+x^4+1 = 100110001

byte SHT2x_CheckCrc(byte data[], byte nbrOfBytes, byte checksum)
{
  byte crc = 0;
  byte byteCtr;
  //calculates 8-Bit checksum with given polynomial
  for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
  { crc ^= (data[byteCtr]);

    for (byte bit = 8; bit > 0; --bit)
    { if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
      else crc = (crc << 1);
    }
  }
  if (crc != checksum) return -1;
  else return 0;
}

long getTemp()
{
  
  Wire.beginTransmission(0x40); // transmit to device #112 (0x70)
  Wire.write(byte(0xe3)); // Trigger T measurement
  Wire.endTransmission();

  
  Wire.requestFrom(0x40, 3);    // request 3 bytes from slave device
  byte reading[] = {0,0,0};
  
  if (3 <= Wire.available()) { // if two bytes were received
    reading[0] = Wire.read();  // receive high byte (overwrites previous reading)
    reading[1] = Wire.read(); // receive low byte as lower 8 bits
    reading[2] = Wire.read(); // receive low byte as lower 8 bits

    if(SHT2x_CheckCrc(reading, 2, reading[2]) == 0){
      

      u16 ticks = reading[0] << 8;
      ticks |= reading[1];
      
      return ((21965 * long(ticks)) >> 13) - 46850;
      
      
    }
  }
  return -300;
}

long getHumidity()
{
  
  Wire.beginTransmission(0x40); // transmit to device #112 (0x70)
  Wire.write(byte(0xe5)); // Trigger RH measurement
  Wire.endTransmission();

  
  Wire.requestFrom(0x40, 3);    // request 3 bytes from slave device
  byte reading[] = {0,0,0};
  
  if (3 <= Wire.available()) { // if two bytes were received
    reading[0] = Wire.read();  // receive high byte (overwrites previous reading)
    reading[1] = Wire.read(); // receive low byte as lower 8 bits
    reading[2] = Wire.read(); // receive low byte as lower 8 bits

    if(SHT2x_CheckCrc(reading, 2, reading[2]) == 0){

      u16 ticks = reading[0] << 8;
      ticks |= reading[1];
      
      return (((15625 * long(ticks)) >> 13) - 6000);
      //return -6.0f + 125.0f * ticks*(1.0f/65536.0f);
      
    }
  }
  return -300;
}

void setup() {
  Wire.begin();                // join i2c bus (address optional for master)
  Serial.begin(9600);          // start serial communication at 9600bps
  Serial.println(sizeof(long));
}


void loop() {
  Serial.println(getHumidity());
  delay(1000);


}



