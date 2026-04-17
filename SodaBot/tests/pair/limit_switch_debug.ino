/*
 * Limit switch debug (Station pins 19,23,15).
 * Prints states to Serial so you can confirm wiring.
 */
#include <Arduino.h>

const int PIN_LIMIT[3] = {19,23,15};

void setup(){
  Serial.begin(115200);
  for(int i=0;i<3;i++) pinMode(PIN_LIMIT[i], INPUT_PULLUP);
  Serial.println("Limit switch debug: active LOW, press to see 0");
}

void loop(){
  bool l0 = digitalRead(PIN_LIMIT[0])==LOW;
  bool l1 = digitalRead(PIN_LIMIT[1])==LOW;
  bool l2 = digitalRead(PIN_LIMIT[2])==LOW;
  Serial.printf("Bin1:%d Bin2:%d Bin3:%d\n", l0,l1,l2);
  delay(300);
}
