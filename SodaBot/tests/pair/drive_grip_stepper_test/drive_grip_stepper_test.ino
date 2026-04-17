/*
 * Combined hardware test for Robot ESP32:
 * - DC motors (L298N) forward/backward with tach counts
 * - Gripper servo open/close
 * - Stepper lift up/down (4-wire driver pins)
 *
 * Pins match robot_main.ino (tach on front wheels).
 */
#include <Arduino.h>

// Motor pins (user mapping)
const int PIN_ENA = 23; // left PWM
const int PIN_IN1 = 22;
const int PIN_IN2 = 19;
const int PIN_ENB = 25; // right PWM
const int PIN_IN3 = 26;
const int PIN_IN4 = 27;
// Tach/enc pins (single-channel)
const int PIN_TACH_L = 17;
const int PIN_TACH_R = 35;
// Servo (gripper) - choose a free PWM pin; adjust if needed
const int PIN_SERVO = 18;
// Stepper 4-wire on L298N (your wiring):
// OUT1=Blue, OUT2=Red, OUT3=Green, OUT4=Black
// So map IN1->OUT1(Blue), IN2->OUT2(Red), IN3->OUT3(Green), IN4->OUT4(Black)
const int PIN_STEP_IN1 = 14; // Blue
const int PIN_STEP_IN2 = 12; // Red
const int PIN_STEP_IN3 = 13; // Green
const int PIN_STEP_IN4 = 33; // Black

volatile long tachL = 0, tachR = 0;

void IRAM_ATTR tachL_ISR(){ tachL++; }
void IRAM_ATTR tachR_ISR(){ tachR++; }

void drive(int left, int right){
  left = constrain(left,-255,255); right = constrain(right,-255,255);
  digitalWrite(PIN_IN1, left>=0); digitalWrite(PIN_IN2, left<0); analogWrite(PIN_ENA, abs(left));
  digitalWrite(PIN_IN3, right>=0); digitalWrite(PIN_IN4, right<0); analogWrite(PIN_ENB, abs(right));
}

// Simple servo drive without Servo/LEDC: send repeated pulses for stable move
void servoMoveUs(uint16_t us, uint16_t duration_ms=600){
  unsigned long start = millis();
  while (millis() - start < duration_ms){
    digitalWrite(PIN_SERVO, HIGH);
    delayMicroseconds(us);
    digitalWrite(PIN_SERVO, LOW);
    delayMicroseconds(20000 - us); // 20 ms frame
  }
}

// 4-step full-step sequence
const uint8_t seq[4][4] = {
  {1,0,1,0},
  {0,1,1,0},
  {0,1,0,1},
  {1,0,0,1}
};
const uint16_t STEPS_PER_REV = 200; // adjust to your motor

void stepperMoveRevs(float revs, bool up, uint16_t delayUs=2000){
  long steps = (long)(revs * STEPS_PER_REV);
  // Invert to match physical up/down if wiring makes "up" go downward
  int dir = up ? -1 : 1;
  int idx = 0;
  for (long s=0; s<steps; s++){
    idx = (idx + dir + 4) % 4;
    digitalWrite(PIN_STEP_IN1, seq[idx][0]);
    digitalWrite(PIN_STEP_IN2, seq[idx][1]);
    digitalWrite(PIN_STEP_IN3, seq[idx][2]);
    digitalWrite(PIN_STEP_IN4, seq[idx][3]);
    delayMicroseconds(delayUs);
  }
}

void stepperRelease(){
  digitalWrite(PIN_STEP_IN1, LOW);
  digitalWrite(PIN_STEP_IN2, LOW);
  digitalWrite(PIN_STEP_IN3, LOW);
  digitalWrite(PIN_STEP_IN4, LOW);
}

void setup(){
  Serial.begin(115200);
  // Motors
  pinMode(PIN_IN1,OUTPUT); pinMode(PIN_IN2,OUTPUT); pinMode(PIN_ENA,OUTPUT);
  pinMode(PIN_IN3,OUTPUT); pinMode(PIN_IN4,OUTPUT); pinMode(PIN_ENB,OUTPUT);
  // Tach
  pinMode(PIN_TACH_L,INPUT);
  pinMode(PIN_TACH_R,INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_TACH_L), tachL_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_TACH_R), tachR_ISR, RISING);
  // Servo
  pinMode(PIN_SERVO, OUTPUT);
  // Stepper coils
  pinMode(PIN_STEP_IN1, OUTPUT); pinMode(PIN_STEP_IN2, OUTPUT);
  pinMode(PIN_STEP_IN3, OUTPUT); pinMode(PIN_STEP_IN4, OUTPUT);
  Serial.println("Combined test ready");
}

void loop(){
  tachL = tachR = 0;
  Serial.println("Drive forward 1.5s");
  drive(150,150);
  delay(1500);
  drive(0,0);
  Serial.printf("Ticks fwd L:%ld R:%ld\n", tachL, tachR);
  delay(500);

  Serial.println("Drive reverse 1.5s");
  tachL = tachR = 0;
  drive(-150,-150);
  delay(1500);
  drive(0,0);
  Serial.printf("Ticks rev L:%ld R:%ld\n", tachL, tachR);
  delay(500);

  Serial.println("Servo open/close");
  servoMoveUs(600, 800);  // open (~10°)
  delay(200);
  servoMoveUs(950, 800);  // close (~40°)

  Serial.println("Stepper up then down");
  stepperMoveRevs(0.5, true, 4000);  // up half rev
  delay(400);
  stepperMoveRevs(0.5, false, 4000); // down half rev
  stepperRelease(); // de-energize

  Serial.println("Cycle done\n");
  delay(2500);
}
