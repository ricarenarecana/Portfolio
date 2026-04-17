/*
 * Motor + tachometer test (single-channel) for Robot ESP32.
 * Drives both sides forward/backward, prints tach counts.
 */
#include <Arduino.h>

const int PIN_IN1 = 25, PIN_IN2 = 26, PIN_ENA = 27; // left
const int PIN_IN3 = 32, PIN_IN4 = 33, PIN_ENB = 14; // right
const int PIN_TACH_L = 34;
const int PIN_TACH_R = 36;

volatile long tachL=0, tachR=0;
int pwm = 150;

void IRAM_ATTR tachL_ISR(){ tachL++; }
void IRAM_ATTR tachR_ISR(){ tachR++; }

void drive(int left, int right){
  left = constrain(left,-255,255); right = constrain(right,-255,255);
  digitalWrite(PIN_IN1, left>=0); digitalWrite(PIN_IN2, left<0); analogWrite(PIN_ENA, abs(left));
  digitalWrite(PIN_IN3, right>=0); digitalWrite(PIN_IN4, right<0); analogWrite(PIN_ENB, abs(right));
}

void setup(){
  Serial.begin(115200);
  pinMode(PIN_IN1,OUTPUT); pinMode(PIN_IN2,OUTPUT); pinMode(PIN_ENA,OUTPUT);
  pinMode(PIN_IN3,OUTPUT); pinMode(PIN_IN4,OUTPUT); pinMode(PIN_ENB,OUTPUT);
  pinMode(PIN_TACH_L,INPUT);
  pinMode(PIN_TACH_R,INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_TACH_L), tachL_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_TACH_R), tachR_ISR, RISING);
}

void loop(){
  drive(pwm,pwm);
  delay(2000);
  drive(0,0);
  Serial.printf("Fwd ticks L:%ld R:%ld\n", tachL, tachR);
  delay(800);

  drive(-pwm,-pwm);
  delay(2000);
  drive(0,0);
  Serial.printf("Rev ticks L:%ld R:%ld\n", tachL, tachR);
  delay(1000);
}
