#include <WiFi.h>
#include <ArduCAM.h>
#include <Wire.h>
#include <SPI.h>
#include "memorysaver.h"
#ifndef READ_FIFO
#define READ_FIFO 0x3C
#endif
// Select camera module
#if !(defined (OV2640_MINI_2MP_PLUS))
  #error Please enable OV2640_MINI_2MP in memorysaver.h
#endif
#define CS 5  // Example CS pin (GPIO5), update if using a different one

ArduCAM myCAM(OV2640, CS);


void setup() {
  uint8_t vid, pid;
  uint8_t temp;

  //Wire.begin();
  Wire.begin(21, 22); // or your SDA, SCL pins
  Serial.begin(115200);
  delay(1000);

  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  //SPI.begin();
  SPI.begin(18, 19, 23, 5); //SPI.begin(SCK, MISO, MOSI, CS)
  SPI.setFrequency(8000000); //8 MHz

  //myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  myCAM.write_reg(ARDUCHIP_MODE, 0x00); // Set to MCU mode
  
  // Check camera module
  myCAM.wrSensorReg8_8(0xff, 0x01);
  myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
  if ((vid != 0x26) || (pid != 0x41 && pid != 0x42)) {
    Serial.println("Can't find OV2640 module!");
  } else {
    Serial.println("OV2640 detected.");
  }
  //check SPI register test
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  uint8_t testVal = myCAM.read_reg(ARDUCHIP_TEST1);
  Serial.print("TEST REG VALUE: 0x"); Serial.println(testVal, HEX);

  myCAM.set_format(JPEG);
  myCAM.InitCAM();
  delay(1000);
  myCAM.OV2640_set_JPEG_size(OV2640_160x120); //min size
  myCAM.clear_fifo_flag();

}

void loop() {
}