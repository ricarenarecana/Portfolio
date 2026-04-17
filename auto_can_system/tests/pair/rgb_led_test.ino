/*
 * RGB status LED test (Robot or Station).
 * Pins: R=2, G=12, B=0
 */
#include <Arduino.h>
const int PIN_R=2, PIN_G=12, PIN_B=0;

void setup(){
  pinMode(PIN_R,OUTPUT); pinMode(PIN_G,OUTPUT); pinMode(PIN_B,OUTPUT);
  Serial.begin(115200);
}

void setRGB(uint8_t r,uint8_t g,uint8_t b){
  analogWrite(PIN_R,r); analogWrite(PIN_G,g); analogWrite(PIN_B,b);
}

void loop(){
  setRGB(255,0,0); Serial.println("Red"); delay(700);
  setRGB(0,255,0); Serial.println("Green"); delay(700);
  setRGB(0,0,255); Serial.println("Blue"); delay(700);
  setRGB(255,255,0); Serial.println("Yellow"); delay(700);
  setRGB(0,255,255); Serial.println("Cyan"); delay(700);
  setRGB(255,0,255); Serial.println("Magenta"); delay(700);
  setRGB(255,255,255); Serial.println("White"); delay(700);
  setRGB(0,0,0); Serial.println("Off"); delay(700);
}
