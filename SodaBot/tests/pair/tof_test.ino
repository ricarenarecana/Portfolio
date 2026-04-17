/*
 * TOF sensor test (VL53L0X) on ESP32 I2C 21/22.
 * Requires Adafruit_VL53L0X library.
 */
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup(){
  Serial.begin(115200);
  Wire.begin(21,22);
  if(!lox.begin()){
    Serial.println("Failed to find VL53L0X"); while(true) delay(1000);
  }
  Serial.println("VL53L0X started");
}

void loop(){
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);
  if (measure.RangeStatus != 4) {
    Serial.printf("Distance: %d mm\n", measure.RangeMilliMeter);
  } else {
    Serial.println("Out of range");
  }
  delay(200);
}
