#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define SDA_PIN 21
#define SCL_PIN 22

// Call buttons
#define CALL_F1_UP   4
#define CALL_F2_UP   5
#define CALL_F2_DOWN 18
#define CALL_F3_UP   19
#define CALL_F3_DOWN 23
#define CALL_F4_DOWN 25

// Cabin buttons
#define CABIN_F1 26
#define CABIN_F2 27
#define CABIN_F3 14
#define CABIN_F4 12

// Door and safety
#define OPEN_BTN      33
#define CLOSE_BTN     32
#define EMERGENCY_BTN 13
#define CONTINUE_BTN  15

bool lastState[50];  // global array to keep previous button states for debounce

void setup() {
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17 to UNO
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Elevator Ready");
  display.display();

  int buttons[] = {
    CALL_F1_UP, CALL_F2_UP, CALL_F2_DOWN, CALL_F3_UP, CALL_F3_DOWN, CALL_F4_DOWN,
    CABIN_F1, CABIN_F2, CABIN_F3, CABIN_F4,
    OPEN_BTN, CLOSE_BTN, EMERGENCY_BTN, CONTINUE_BTN
  };
  for (int pin : buttons) {
    pinMode(pin, INPUT_PULLUP);
    lastState[pin] = HIGH; // initialize lastState to HIGH (not pressed)
  }
}

void loop() {
  checkButton(CALL_F1_UP,   "F1");
  checkButton(CALL_F2_UP,   "F2");
  checkButton(CALL_F2_DOWN, "F2");
  checkButton(CALL_F3_UP,   "F3");
  checkButton(CALL_F3_DOWN, "F3");
  checkButton(CALL_F4_DOWN, "F4");

  checkButton(CABIN_F1, "F1");
  checkButton(CABIN_F2, "F2");
  checkButton(CABIN_F3, "F3");
  checkButton(CABIN_F4, "F4");

  checkButton(OPEN_BTN, "OPEN");
  checkButton(CLOSE_BTN, "CLOSE");
  checkButton(EMERGENCY_BTN, "STOP");
  checkButton(CONTINUE_BTN, "CONT");
}

void checkButton(int pin, const char* command) {
  static unsigned long lastPress[50] = {0};

  int idx = pin;
  bool currentState = digitalRead(pin);

  // Detect falling edge: HIGH -> LOW
  if (lastState[idx] == HIGH && currentState == LOW) {
    if (millis() - lastPress[idx] > 300) { // debounce time 300ms
      lastPress[idx] = millis();
      sendCommand(command);
    }
  }

  lastState[idx] = currentState;
}

void sendCommand(const char* cmd) {
  Serial2.println(cmd);
  Serial.print("Sent: ");
  Serial.println(cmd);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Command Sent:");
  display.println(cmd);
  display.display();
}
