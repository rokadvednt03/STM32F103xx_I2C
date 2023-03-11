#include <Wire.h>
#define SLAVE_ADDR 0x67
uint8_t active_command = 0xff;

void setup() {
  // put your setup code here, to run once:
  Wire.begin(SLAVE_ADDR);
  Wire.onReceive(recieveevent);
  Wire.onRequest(requestEvent);
}


void recieveevent(int bytes) {
  active_command = Wire.read();    // read one character from the I2C 
    
}
uint8_t dd = 0x55 ;
void requestEvent()
{
  Wire.write(&dd,1);
}
void loop() {
  // put your main code here, to run repeatedly:
  
}
