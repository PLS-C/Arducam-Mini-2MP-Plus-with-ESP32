#include <WiFi.h>
#include <Wire.h>
#include <WebServer.h>
#include <ArduCAM.h>
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

// WiFi config
const char *ssid = "PLS_wifi";
const char *password = "55378826";

WebServer server(80);
ArduCAM myCAM(OV2640, CS);


void start_capture() {
  myCAM.flush_fifo();
  myCAM.clear_fifo_flag();
  myCAM.start_capture();
}

void camCapture(ArduCAM myCAM) {
  WiFiClient client = server.client();
  myCAM.CS_LOW();
  size_t len = myCAM.read_fifo_length();
  myCAM.CS_HIGH();
  Serial.println(len);
  if (len == 0 || len == 0x7FFFFF || len > 0x5FFFF) {
    Serial.println("Invalid image size");
    return;
  }

  myCAM.CS_LOW();
  myCAM.set_fifo_burst();

  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: image/jpeg\r\n";
  response += "Content-Length: " + String(len) + "\r\n\r\n";
  server.sendContent(response);

  uint8_t buf[128];
  while (len) {
    uint8_t buf[128];
    uint32_t bytesToRead = (len < sizeof(buf)) ? len : sizeof(buf);
    for (uint32_t i = 0; i < bytesToRead; i++) {
      buf[i] = SPI.transfer(0x00);
    }


    if (!client.connected()) break;
    client.write(buf, bytesToRead);
    len -= bytesToRead;
  }

  myCAM.CS_HIGH();
}

void serverCapture() {
  start_capture();
  while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));
  camCapture(myCAM);
}

void serverStream() {
  WiFiClient client = server.client();
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  server.sendContent(response);

  while (client.connected()) {
    start_capture();
    while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));

    size_t len = myCAM.read_fifo_length();
    if (len >= 0x07FFFF || len == 0) continue;

    myCAM.CS_LOW();
    myCAM.set_fifo_burst();
    unsigned long start_time = millis();
    server.sendContent("--frame\r\nContent-Type: image/jpeg\r\n\r\n");

    uint8_t buf[128];
    while (len) {
      size_t bytesToRead = (len < sizeof(buf)) ? len : sizeof(buf);
      for (size_t i = 0; i < bytesToRead; i++) {
        buf[i] = SPI.transfer(0x00);
      }
      if (!client.connected()) break;
      client.write(buf, bytesToRead);
      len -= bytesToRead;

    }
    unsigned long End_time=millis();
    unsigned long frame_time = End_time-start_time;
    Serial.print("frame_time: ");
    Serial.print(frame_time);
    Serial.print("ms, FPS:");
    float FPS = 1000.0/frame_time;
    Serial.print(String(FPS, 2));
    Serial.println("");
    myCAM.CS_HIGH();
    delay(100);
  }
}

void handleNotFound() {
  server.send(200, "text/plain", "ESP32 ArduCAM Web Server\n/capture or /stream");
}

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
  //SPI.setFrequency(1000000); //1 MHz

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
  //myCAM.OV2640_set_JPEG_size(OV2640_160x120); //min size
  //myCAM.OV2640_set_JPEG_size(OV2640_1600x1200); //max size
  myCAM.OV2640_set_JPEG_size(OV2640_640x480);
  myCAM.clear_fifo_flag();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());

  server.on("/capture", HTTP_GET, serverCapture);
  server.on("/stream", HTTP_GET, serverStream);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}