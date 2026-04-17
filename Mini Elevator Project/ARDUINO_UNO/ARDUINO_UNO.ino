#include <Servo.h>
#include <SoftwareSerial.h>

// Motor driver pins (L298N)
#define ENA 10
#define IN1 9
#define IN2 8

// Limit switches for floors
#define LIMIT_F1 2
#define LIMIT_F2 3
#define LIMIT_F3 4
#define LIMIT_F4 5

// Continuous rotation servo for door
Servo doorServo;
#define SERVO_PIN 6

// Serial comm to ESP32
SoftwareSerial mySerial(11, 12); // RX, TX

int targetFloor = 0;
int currentFloor = 0;
bool emergencyStop = false;

unsigned long doorOpenTime = 3000; // door stays open 3 seconds

void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(LIMIT_F1, INPUT_PULLUP);
  pinMode(LIMIT_F2, INPUT_PULLUP);
  pinMode(LIMIT_F3, INPUT_PULLUP);
  pinMode(LIMIT_F4, INPUT_PULLUP);

  doorServo.attach(SERVO_PIN);
  stopDoor();

  Serial.println("Elevator Ready");
}

void loop() {
  // Update current floor reading
  if (digitalRead(LIMIT_F1) == LOW) currentFloor = 1;
  if (digitalRead(LIMIT_F2) == LOW) currentFloor = 2;
  if (digitalRead(LIMIT_F3) == LOW) currentFloor = 3;
  if (digitalRead(LIMIT_F4) == LOW) currentFloor = 4;

  if (mySerial.available()) {
    String cmd = mySerial.readStringUntil('\n');
    cmd.trim();
    Serial.println("CMD: " + cmd);

    if (cmd.startsWith("F")) { // Floor command
      targetFloor = cmd.substring(1).toInt();
      moveToFloor(targetFloor);
    }
    else if (cmd == "OPEN") {
      openDoor();
    }
    else if (cmd == "CLOSE") {
      closeDoor();
    }
    else if (cmd == "STOP") {
      emergencyStop = true;
      stopMotor();
    }
    else if (cmd == "CONT") {
      emergencyStop = false;
      moveToFloor(targetFloor);
    }
  }
}

void moveToFloor(int floor) {
  if (emergencyStop) return;
  if (floor == currentFloor) {
    Serial.println("Already at floor");
    return;
  }

  if (floor > currentFloor) { // Move up
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else { // Move down
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
  analogWrite(ENA, 200);

  // Run until target floor limit switch triggers
  while (true) {
    if (emergencyStop) {
      stopMotor();
      return;
    }
    if (floor == 1 && digitalRead(LIMIT_F1) == LOW) break;
    if (floor == 2 && digitalRead(LIMIT_F2) == LOW) break;
    if (floor == 3 && digitalRead(LIMIT_F3) == LOW) break;
    if (floor == 4 && digitalRead(LIMIT_F4) == LOW) break;
  }

  stopMotor();
  currentFloor = floor;
  openDoor();
  delay(doorOpenTime);
  closeDoor();
}

void stopMotor() {
  analogWrite(ENA, 0);
}

void openDoor() {
  Serial.println("Opening door...");
  doorServo.write(0); // rotate one way
  delay(1000);
  stopDoor();
}

void closeDoor() {
  Serial.println("Closing door...");
  doorServo.write(180); // rotate other way
  delay(1000);
  stopDoor();
}

void stopDoor() {
  doorServo.write(90); // stop servo
}
