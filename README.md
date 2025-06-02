# 📷 Arducam Mini 2MP Plus with ESP32 – Step-by-Step Guide

This project demonstrates how to set up and use the **Arducam Mini 2MP Plus** (OV2640) with an **ESP32** microcontroller. It covers wiring, software setup, capturing images, and using the camera data for applications like Object Detection or OCR.

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
| RESET       | GPIO 15             |
| PWDN        | GND                 |

> ⚠️ **Note:** You can change GPIO pins in the code if needed.

---

## 🖥️ 2. Install Arduino Libraries

Install the following libraries via the Arduino Library Manager or manually:

- `ArduCAM`
- `Wire`
- `SPI`

🔗 Official repo: [https://github.com/ArduCAM/Arduino](https://github.com/ArduCAM/Arduino)
🔗 Arducam Mini 2MP Plus: [https://www.arducam.com/arducam-2mp-spi-camera-b0067-arduino.html](https://www.arducam.com/arducam-2mp-spi-camera-b0067-arduino.html)

---

## 🔧 3. Configure Arduino IDE

1. Open Arduino IDE
2. Install **ESP32** board support via **Boards Manager**
3. Select your board: `ESP32 Dev Module`
4. Choose the correct port via **Tools > Port**

---

## 🧪 4. Upload and Test Example Code

```cpp
#include <Wire.h>
#include <SPI.h>
#include <ArduCAM.h>
#include "memorysaver.h"

#define CS_PIN 5

ArduCAM myCAM(OV2640, CS_PIN);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  SPI.begin();
  myCAM.initCAM();
  Serial.println("Camera Initialized.");
}

void loop() {
  myCAM.captureImage();
  Serial.println("Image Captured!");
  delay(3000);
}
