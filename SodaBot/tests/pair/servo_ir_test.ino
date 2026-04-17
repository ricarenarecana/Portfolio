/*
 * Gripper servo + dual IR sensor test (Robot ESP32).
 * IR sensors on pins 4 and 5 (active LOW). Servo on pin 13.
 */
#include <Arduino.h>

const int PIN_IR_L = 4, PIN_IR_R = 5;
const int PIN_SERVO = 13;

void setup(){
  Serial.begin(115200);
  pinMode(PIN_IR_L, INPUT_PULLUP);
  pinMode(PIN_IR_R, INPUT_PULLUP);
  ledcAttachPin(PIN_SERVO, 8);
  ledcSetup(8, 50, 16); // 50 Hz, 16-bit
  Serial.println("Servo/IR test ready");
}

void setServoUs(uint16_t us){
  uint32_t duty = (us * 65535UL * 50) / 1000000UL;
  ledcWrite(8, duty);
}

void loop(){
  bool l = digitalRead(PIN_IR_L)==LOW;
  bool r = digitalRead(PIN_IR_R)==LOW;
  Serial.printf("IR L:%d R:%d\n", l, r);
  setServoUs(1100); // open
  delay(800);
  setServoUs(1800); // close
  delay(1200);
}
