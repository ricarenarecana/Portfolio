// Stepper motor control for gripper arm using L298 driver on ESP32
// Adjust pin numbers and steps as needed

#include <Arduino.h>

// Define L298 stepper motor pins
#define IN1  14  // Connect to L298 IN1
#define IN2  27  // Connect to L298 IN2
#define IN3  26  // Connect to L298 IN3
#define IN4  25  // Connect to L298 IN4

// Steps per revolution for your stepper motor
#define STEPS_PER_REV 200

// Delay between steps (ms)
#define STEP_DELAY 15 // Increase for more torque

// Half-step sequence for 4-wire stepper
const int stepSequence[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

void setStepper(int s) {
  digitalWrite(IN1, stepSequence[s][0]);
  digitalWrite(IN2, stepSequence[s][1]);
  digitalWrite(IN3, stepSequence[s][2]);
  digitalWrite(IN4, stepSequence[s][3]);
}

void stepMotor(int steps, bool direction) {
  for (int i = 0; i < steps; i++) {
    int idx = direction ? i % 8 : (7 - (i % 8));
    setStepper(idx);
    delay(STEP_DELAY);
  }
}

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  Serial.begin(115200);
  Serial.println("Stepper motor gripper test ready.");
}

void loop() {
  Serial.println("Moving gripper arm UP...");
  stepMotor(200, true); // Move up
  delay(1000);
  Serial.println("Moving gripper arm DOWN...");
  stepMotor(200, false); // Move down
  delay(1000);
}
