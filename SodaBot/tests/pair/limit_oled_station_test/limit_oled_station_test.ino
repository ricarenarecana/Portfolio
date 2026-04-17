/*
 * Station-side test: read 3 limit switches and show on ONE OLED.
 * Pins: limits 19,23,15 (active low). I2C 21/22. OLED addr 0x3C (or 0x3D).
 */
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int PIN_LIMIT[3] = {19,23,15};
Adafruit_SSD1306 oled(128,64,&Wire,-1);
const uint8_t OLED_ADDR = 0x3C;

void setup(){
  Serial.begin(115200);
  for(int i=0;i<3;i++) pinMode(PIN_LIMIT[i], INPUT_PULLUP);
  Wire.begin(21,22);
  oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  oled.clearDisplay(); oled.setTextSize(2); oled.setTextColor(SSD1306_WHITE); oled.display();
}

void loop(){
  bool l0 = digitalRead(PIN_LIMIT[0])==LOW;
  bool l1 = digitalRead(PIN_LIMIT[1])==LOW;
  bool l2 = digitalRead(PIN_LIMIT[2])==LOW;

  oled.clearDisplay();
  oled.setCursor(0,0);
  oled.println("Bins:");
  oled.setTextSize(2);
  oled.printf("1:%s\n", l0 ? "Full" : "Empty");
  oled.printf("2:%s\n", l1 ? "Full" : "Empty");
  oled.printf("3:%s\n", l2 ? "Full" : "Empty");
  oled.display();

  Serial.printf("Limits: %d %d %d\n", l0,l1,l2);
  delay(300);
}
