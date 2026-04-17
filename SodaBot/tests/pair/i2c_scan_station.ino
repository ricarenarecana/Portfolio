/*
 * I2C scanner for Station ESP32 (SDA 21, SCL 22).
 * Lists all devices to verify OLED addresses.
 */
#include <Arduino.h>
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  while(!Serial) {}
  Wire.begin(21,22);
  Serial.println("I2C scan start");
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    uint8_t err = Wire.endTransmission();
    if (err == 0) {
      Serial.printf("Found 0x%02X\n", addr);
    }
  }
  Serial.println("Done.");
}

void loop() {}
