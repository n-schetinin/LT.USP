#include <Wire.h>
#include <EEPROM.h>
 
void setup()
{
  byte error, addr;
  int devCount;
  Wire.begin();
 
  Serial.begin(9600);
  while (!Serial); 
  Serial.println("\n Scanning");

  Serial.println("Scanning...");
 
  devCount = 0;
  for(addr = 1; addr < 127; addr++ )
  {
    Wire.beginTransmission(addr);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("Device found - 0x");
      if (addr<16){
        Serial.print("0");
      }        
      Serial.println(addr,HEX);
      EEPROM.write(30, addr);
      Serial.println("EEPROM wrote");
      devCount++;
        
    }
    else if (error==4)
    {
      Serial.print("Unknown error - 0x");
      if (addr<16)
        Serial.print("0");
      Serial.println(addr,HEX);
    }    
  }
  if (devCount == 0){
    Serial.println("No devices found\n");
  }
}
 
 
void loop()
{

}
