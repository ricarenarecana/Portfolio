/*
 * Stepper (L298N 4-wire) + SG996R gripper servo test for ESP32.
 * - L298N ENA/ENB tied HIGH (5V). Drive coils via IN1..IN4 only.
 * - Starts assuming lift is DOWN; first move is UP, then DOWN.
 * - Serial @115200 live tuning:
 *     up 1.0      -> set UP turns
 *     down 0.6    -> set DOWN turns
 *     both 0.8    -> set both
 *     speed 800   -> step delay microseconds (lower = faster)
 *     once        -> run one cycle immediately
 */
#include <Arduino.h>
#include <ESP32Servo.h>

// --- L298N stepper pins (coil order): OUT1=IN1, OUT2=IN2, OUT3=IN3, OUT4=IN4
// Color hint if wired like earlier tests: Blue=IN1(14), Red=IN2(27), Green=IN3(26), Black=IN4(25)
const int PIN_IN1 = 14;
const int PIN_IN2 = 12;
const int PIN_IN3 = 13;
const int PIN_IN4 = 33;

// --- Servo (gripper) pin matches robot_main.ino
const int PIN_SERVO = 18;

// --- Tuning (modifiable via Serial commands) ---
const int STEPS_PER_REV = 200;   // adjust if motor/gearbox differs
volatile float TURNS_UP = 0.50f;
volatile float TURNS_DOWN = 0.50f;
volatile uint16_t STEP_DELAY_US = 2000; // microseconds between steps (more torque)

// Servo angles
const float SERVO_OPEN_DEG = 10.0f;  // home/open
const float SERVO_CLOSE_DEG = 40.0f; // closed
Servo gripper;

// Full-step (two-phase on) for maximum torque with L298N
const uint8_t SEQ[4][4] = {
  {1,0,1,0},
  {0,1,1,0},
  {0,1,0,1},
  {1,0,0,1}
};

// --- Helpers ---
void setServoDeg(float deg) {
  deg = constrain(deg, 0.0f, 180.0f);
  gripper.write(deg);
}

void setStepPhase(uint8_t idx) {
  digitalWrite(PIN_IN1, SEQ[idx][0]);
  digitalWrite(PIN_IN2, SEQ[idx][1]);
  digitalWrite(PIN_IN3, SEQ[idx][2]);
  digitalWrite(PIN_IN4, SEQ[idx][3]);
}

void stepperTurns(float turns, bool up) {
  if (turns <= 0.0f) return;
  long steps = (long)(turns * STEPS_PER_REV); // full steps
  int dir = up ? 1 : -1;
  int idx = 0;
  for (long i = 0; i < steps; i++) {
    idx = (idx + dir + 4) % 4;
    setStepPhase(idx);
    delayMicroseconds(STEP_DELAY_US);
  }
}

void allCoilsOff() {
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, LOW);
}

void printStatus() {
  Serial.printf("UP turns: %.3f | DOWN turns: %.3f | step delay: %u us\n",
                TURNS_UP, TURNS_DOWN, STEP_DELAY_US);
}

void handleSerial() {
  if (!Serial.available()) return;
  String line = Serial.readStringUntil('\n');
  line.trim();
  if (line.length() == 0) return;

  if (line.startsWith("up")) {
    TURNS_UP = line.substring(2).toFloat();
    Serial.print("Set UP turns -> "); Serial.println(TURNS_UP, 3);
  } else if (line.startsWith("down")) {
    TURNS_DOWN = line.substring(4).toFloat();
    Serial.print("Set DOWN turns -> "); Serial.println(TURNS_DOWN, 3);
  } else if (line.startsWith("both")) {
    float v = line.substring(4).toFloat();
    TURNS_UP = TURNS_DOWN = v;
    Serial.print("Set BOTH turns -> "); Serial.println(v, 3);
  } else if (line.startsWith("speed")) {
    int v = line.substring(5).toInt();
    if (v > 0) { STEP_DELAY_US = (uint16_t)v; Serial.print("Step delay us -> "); Serial.println(STEP_DELAY_US); }
  } else if (line == "once") {
    Serial.println("Manual cycle requested.");
    // fall through
  } else {
    Serial.println("Cmds: up <t>, down <t>, both <t>, speed <us>, once");
  }
  printStatus();
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);

  // Servo at 50 Hz, pulse range 500-2400 us to cover 0..180 deg
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  gripper.setPeriodHertz(50);
  gripper.attach(PIN_SERVO, 500, 2400);
  setServoDeg(SERVO_OPEN_DEG); // home/open

  Serial.println("\nL298N Stepper + SG996R Servo test (starts DOWN, first move is UP).");
  Serial.println("Serial @115200 -> cmds: up <t>, down <t>, both <t>, speed <us>, once");
  printStatus();
  delay(800);
}

void loop() {
  handleSerial();

  Serial.printf("Stepper UP %.3f turns...\n", TURNS_UP);
  stepperTurns(TURNS_UP, true);
  delay(150);

  Serial.println("Close gripper (40 deg)");
  setServoDeg(SERVO_CLOSE_DEG);
  delay(600);

  Serial.println("Open gripper (10 deg)");
  setServoDeg(SERVO_OPEN_DEG);
  delay(300);

  Serial.printf("Stepper DOWN %.3f turns...\n", TURNS_DOWN);
  stepperTurns(TURNS_DOWN, false);
  allCoilsOff(); // de-energize between cycles since ENA/ENB are tied high
  delay(1000);
}
