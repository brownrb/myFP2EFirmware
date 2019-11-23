#include <Wire.h>

void setup() 
{  
  Serial.begin (115200);
  Serial.println ("****** simple I2C bus scanner ********");

  Wire.begin();

  Serial.println("Scan at 100 kHz");
  scan(100000L);
  Serial.println("Scan at 400 kHz");
  scan(400000L);
}
  
void scan(unsigned long clock)
{
  Wire.setClock(clock);

  int count = 0;
  for (int i = 0; i < 128; i++)
  {
    Wire.beginTransmission(i);
    if(!Wire.endTransmission ())
    { 
      Serial.print ("address = 0x");
      Serial.println(i, HEX);
      count++;    
    }
  }
  Serial.print (count);
  Serial.println (" devices detected");   
}
  

void loop(){}