# 📷 Arducam Mini 2MP Plus with ESP32 – Step-by-Step Guide

This project demonstrates how to set up and use the **Arducam Mini 2MP Plus** (OV2640) with an **ESP32** microcontroller. It covers wiring, software setup, capturing images, and using the camera data for applications like Object Detection.

---

## 🧰 What You Need

- [ ] Arducam Mini 2MP Plus (OV2640)
- [ ] ESP32 Dev Board
- [ ] Jumper wires
- [ ] Breadboard (optional)
- [ ] USB cable
- [ ] Arduino IDE or PlatformIO

---

## 🔌 1. Wiring the Arducam to ESP32

| Arducam Pin | ESP32 Pin (Example) |
|-------------|---------------------|
| VCC         | 3.3V                |
| GND         | GND                 |
| SCL         | GPIO 22             |
| SDA         | GPIO 21             |
| MISO        | GPIO 19             |
| MOSI        | GPIO 23             |
| SCK         | GPIO 18             |
| CS          | GPIO 5              |

> ⚠️ **Note:** You can change GPIO pins in the code if needed.

---

## 🔧 2 Install ESP32 Board in Arduino IDE

To add ESP32 support to your Arduino IDE:

1. Open Arduino IDE
2. Go to **File > Preferences**
3. In the “Additional Boards Manager URLs” field, add:

https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

4. Click **OK**
5. Go to **Tools > Board > Boards Manager**
6. Search for “esp32” and click **Install**

> 📝 Now you’re ready to select your ESP32 board

---

## 🔧 3. Configure Arduino IDE

1. Open Arduino IDE
2. Select your board: `ESP32 Dev Module` (or appropriate model)
3. Choose the correct port via **Tools > Port**
   
---

## 🖥️ 4. Install ArduCAM Libraries

Install the following libraries via the Arduino Library Manager or manually:

- `ArduCAM`

1. Open Arduino IDE
2. Go to **Sketch > Include library > Library manager
3. Search for “Arducam” and click **Install** at ArduCAM by Arducam

🔗 Official repo: [https://github.com/ArduCAM/Arduino](https://github.com/ArduCAM/Arduino)
🔗 Arducam Mini 2MP Plus: [https://www.arducam.com/arducam-2mp-spi-camera-b0067-arduino.html](https://www.arducam.com/arducam-2mp-spi-camera-b0067-arduino.html)

---

##📝 5. Modify memorysaver.h According to Your Hardware
Edit the memorysaver.h file to enable the correct camera module. For the Arducam Mini 2MP Plus, make sure only the following line is uncommented:

```cpp
#ifndef _MEMORYSAVER_
#define _MEMORYSAVER_
//Only when using raspberry,enable it
//#define RASPBERRY_PI
//There are two steps you need to modify in this file before normal compilation
//Only ArduCAM Shield series platform need to select camera module, ArduCAM-Mini series platform doesn't
//Step 1: select the hardware platform, only one at a time
//#define OV2640_MINI_2MP
//#define OV3640_MINI_3MP
// #define OV5642_MINI_5MP
//#define OV5642_MINI_5MP_BIT_ROTATION_FIXED
#define OV2640_MINI_2MP_PLUS
//#define OV5642_MINI_5MP_PLUS
//#define OV5640_MINI_5MP_PLUS
```
## 🧪 6. Upload and Test Example Code

```cpp
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

```
⚠️ If you encounter errors, make sure you have completed all the previous setup steps correctly.
---

Reference
https://github.com/ArduCAM/Arduino/blob/master/ArduCAM/examples/RaspberryPi/arducam_ov2640_capture.cpp
https://blog.arducam.com/downloads/shields/ArduCAM_Camera_Shield_Software_Application_Note.pdf


