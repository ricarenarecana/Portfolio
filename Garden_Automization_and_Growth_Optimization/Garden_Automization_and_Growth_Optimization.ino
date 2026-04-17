#include <WiFi.h>
#include <WebServer.h>
#include <VL53L0X.h>         // ToF
#include <SoftwareSerial.h>  // SMS
#include "website2.h"  // Website Template
#include <limits.h>          // Used for INT_MAX

const int SERIAL_BAUD_RATE = 115200;

const int TOF_OFFSET = 80;  // ToF sensor is not accurate. Offset the value by X
int tofDistance = INT_MAX;  // Set intially to highest possible int

const int GSM_BAUD_RATE = 9600;
const int GSM_TX_PIN = 33;
const int GSM_RX_PIN = 32;
const int GSM_SMS_COOLDOWN = 60 * 1000;  // Milliseconds
unsigned long lastSmsSendTime = millis() - GSM_SMS_COOLDOWN;

const int WATER_PUMP_PIN = 25;
int waterPumpState = 0;
const int GROW_LIGHT_PIN = 26;
int growLightState = 0;

const int MOISTURE_SENSOR_0_PIN = 34;
const int MOISTURE_SENSOR_1_PIN = 35;
const int MAX_MOISTURE_VAL = 100;
const float MOISTURE_MULTIPLIER = 1.25;
float moisture0Value = 0;
float moisture1Value = 0;

// LDR Sensor Configuration
const int LDR_SENSOR_PIN = 36;  // ADC pin for LDR sensor
const int MIN_LIGHT_THRESHOLD = 30;  // Threshold to trigger grow light (0-100)
const int MAX_LIGHT_VAL = 100;
float ldrValue = 0;
bool autoLightControl = true;  // Flag to enable/disable automatic light control

// Moisture threshold for automatic watering
const int MIN_MOISTURE_THRESHOLD = 20;  // If moisture falls below this, activate pump
bool autoWaterControl = true;  // Flag to enable/disable automatic water control

// Data history for graphs
const int HISTORY_SIZE = 20;
float moisture0History[HISTORY_SIZE];
float moisture1History[HISTORY_SIZE];
float ldrHistory[HISTORY_SIZE];
float tofHistory[HISTORY_SIZE];
int historyIndex = 0;
unsigned long lastHistoryUpdate = 0;
const unsigned long HISTORY_UPDATE_INTERVAL = 5000; // Update history every 5 seconds

// WiFi Configuration
const char* ssid = "HONOR Pad X8a";  // WiFi SSID
const char* password = "10052115";  // WiFi Password

VL53L0X tofSensor;                                 // ToF Sensor
SoftwareSerial simSerial(GSM_TX_PIN, GSM_RX_PIN);  // Initialize Sim Serial
WebServer server(80);                              // Run server on port 80, which is default port router

void handleRouteRoot() {
  String s = websiteHtml;  //Read HTML contents
  server.send(200, "text/html", s);
}

void handleRouteSetPinState() {
  String pinName = server.arg("name");
  String pinState = server.arg("state");
  Serial.print("Pin name: ");
  Serial.println(pinName);
  Serial.print("Pin state: ");
  Serial.println(pinState);

  const bool state = pinState == "ON" ? 1 : 0;
  if (pinName == "waterPump") {
    waterPumpState = state;
    digitalWrite(WATER_PUMP_PIN, waterPumpState);
    Serial.print("Water pump state set to: ");
    Serial.println(waterPumpState);
  } else if (pinName == "growLight") {
    growLightState = state;
    digitalWrite(GROW_LIGHT_PIN, growLightState);
    Serial.print("Grow light state set to: ");
    Serial.println(growLightState);
  } else if (pinName == "autoLight") {
    autoLightControl = state;
    if (!autoLightControl) {
      // When turning off auto mode, don't change light state
      // It will stay as manually set
      Serial.println("Auto light control disabled");
    } else {
      Serial.println("Auto light control enabled");
      // Immediately apply the correct state based on current light level
      handleAutoLightControl();
    }
  } else if (pinName == "autoWater") {
    autoWaterControl = state;
    if (!autoWaterControl) {
      Serial.println("Auto water control disabled");
    } else {
      Serial.println("Auto water control enabled");
      // Immediately apply the correct state based on current moisture levels
      handleAutoWaterControl();
    }
  }

  server.send(200, "text/plain", "ok");
}

void handleRouteGetStates() {
  const String sTofValue = "\"tofValue\":" + String(tofDistance);
  const String sMoisture0Value = "\"moisture0Value\":" + String(moisture0Value);
  const String sMoisture1Value = "\"moisture1Value\":" + String(moisture1Value);
  const String sLdrValue = "\"ldrValue\":" + String(ldrValue);
  const String sWaterPump = "\"waterPump\":" + String(waterPumpState);
  const String sGrowLight = "\"growLight\":" + String(growLightState);
  const String sAutoLight = "\"autoLight\":" + String(autoLightControl);
  const String sAutoWater = "\"autoWater\":" + String(autoWaterControl);

  server.send(200, "application/json", "{" + sTofValue + "," + 
                                     sMoisture0Value + "," + 
                                     sMoisture1Value + "," + 
                                     sLdrValue + "," + 
                                     sWaterPump + "," + 
                                     sGrowLight + "," + 
                                     sAutoLight + "," + 
                                     sAutoWater + "}");
}

void handleRouteGetHistory() {
  String response = "{\"moisture0\": [";
  for (int i = 0; i < HISTORY_SIZE; i++) {
    int idx = (historyIndex + i) % HISTORY_SIZE;
    response += String(moisture0History[idx]);
    if (i < HISTORY_SIZE - 1) response += ",";
  }
  response += "],\"moisture1\": [";
  for (int i = 0; i < HISTORY_SIZE; i++) {
    int idx = (historyIndex + i) % HISTORY_SIZE;
    response += String(moisture1History[idx]);
    if (i < HISTORY_SIZE - 1) response += ",";
  }
  response += "],\"light\": [";
  for (int i = 0; i < HISTORY_SIZE; i++) {
    int idx = (historyIndex + i) % HISTORY_SIZE;
    response += String(ldrHistory[idx]);
    if (i < HISTORY_SIZE - 1) response += ",";
  }
  response += "],\"height\": [";
  for (int i = 0; i < HISTORY_SIZE; i++) {
    int idx = (historyIndex + i) % HISTORY_SIZE;
    // Convert to centimeters for better display and only if there's a valid reading
    float heightValue = tofHistory[idx] < 10000 ? tofHistory[idx] / 10.0 : 0;
    response += String(heightValue);
    if (i < HISTORY_SIZE - 1) response += ",";
  }
  response += "]}";
  
  server.send(200, "application/json", response);
}

void handleRouteSendSmsMessage() {
  String phoneNumber = server.arg("phoneNumber");
  String message = server.arg("message");
  Serial.print("Phone Number: ");
  Serial.println(phoneNumber);
  Serial.print("Message: ");
  Serial.println(message);

  sendSmsMessage(phoneNumber, message);
  server.send(200, "text/plain", "ok");
}

void sendSmsMessage(String phoneNumber, String message) {
  Serial.println("Sending Message");
  simSerial.println("AT+CMGF=1");
  delay(200);
  simSerial.println("AT+CMGS=\"" + phoneNumber + "\"\r");
  delay(200);
  simSerial.println(message);
  delay(200);
  simSerial.println((char)26);
}

int readMoisture0Sensor() {
  int sensorValue = analogRead(MOISTURE_SENSOR_0_PIN);
  int outputValue = map(sensorValue, 0, 4095, MAX_MOISTURE_VAL * MOISTURE_MULTIPLIER, 0);
  if (outputValue > MAX_MOISTURE_VAL) {
    outputValue = MAX_MOISTURE_VAL;
  } else if (outputValue < 0) {
    outputValue = 0;
  }
  return outputValue;
}

int readMoisture1Sensor() {
  int sensorValue = analogRead(MOISTURE_SENSOR_1_PIN);
  int outputValue = map(sensorValue, 0, 4095, MAX_MOISTURE_VAL * MOISTURE_MULTIPLIER, 0);
  if (outputValue > MAX_MOISTURE_VAL) {
    outputValue = MAX_MOISTURE_VAL;
  } else if (outputValue < 0) {
    outputValue = 0;
  }
  return outputValue;
}

int readLdrSensor() {
  int sensorValue = analogRead(LDR_SENSOR_PIN);
  // LDR typically has higher resistance in darkness, so invert the mapping
  // to make 100% represent max light and 0% represent darkness
  int outputValue = map(sensorValue, 0, 4095, MAX_LIGHT_VAL, 0);
  if (outputValue > MAX_LIGHT_VAL) {
    outputValue = MAX_LIGHT_VAL;
  } else if (outputValue < 0) {
    outputValue = 0;
  }
  return outputValue;
}

void updateHistory() {
  unsigned long currentTime = millis();
  if (currentTime - lastHistoryUpdate >= HISTORY_UPDATE_INTERVAL) {
    lastHistoryUpdate = currentTime;
    
    // Store current readings in the history arrays
    moisture0History[historyIndex] = moisture0Value;
    moisture1History[historyIndex] = moisture1Value;
    ldrHistory[historyIndex] = ldrValue;
    tofHistory[historyIndex] = tofDistance;
    
    // Move to the next position in the circular buffer
    historyIndex = (historyIndex + 1) % HISTORY_SIZE;
  }
}

void handleAutomatedSmsMessage() {
  unsigned long currentTime = millis();
  if (tofDistance < 10000 && currentTime >= GSM_SMS_COOLDOWN + lastSmsSendTime) {
    lastSmsSendTime = currentTime;
    Serial.println("SEND MESSAGE");
    
    // Format a comprehensive message with all sensor data and actuator states
    String smsMessage = "Garden Status Alert:\n";
    smsMessage += "The plant is within the desired harvest range: " + (tofDistance < 500 ? String(tofDistance) + "mm (Needs trimming!)" : "Normal") + "\n";
    smsMessage += "Moisture Sensor 1: " + String(moisture0Value) + "%\n";
    smsMessage += "Moisture Sensor 2: " + String(moisture1Value) + "%\n";
    smsMessage += "Light Level: " + String(ldrValue) + "%\n";
    smsMessage += "Grow Light: " + String(growLightState ? "ON" : "OFF") + "\n";
    smsMessage += "Water Pump: " + String(waterPumpState ? "ON" : "OFF");
    
    sendSmsMessage("+639208991085", smsMessage);
  }
}

void handleAutoLightControl() {
  if (autoLightControl) {
    // If light level is below threshold, turn on the grow light
    if (ldrValue < MIN_LIGHT_THRESHOLD) {
      if (growLightState == 0) {
        growLightState = 1;
        digitalWrite(GROW_LIGHT_PIN, growLightState);
        Serial.println("Auto: Turning ON grow light due to low light level");
      }
    } else {
      // Light level is above threshold, turn off the grow light
      if (growLightState == 1) {
        growLightState = 0;
        digitalWrite(GROW_LIGHT_PIN, growLightState);
        Serial.println("Auto: Turning OFF grow light due to sufficient light level");
      }
    }
  }
}

void handleAutoWaterControl() {
  if (autoWaterControl) {
    // Calculate average of both moisture sensors
    float avgMoisture = (moisture0Value + moisture1Value) / 2.0;
    
    // If average moisture level is below threshold, turn on the water pump
    if (avgMoisture < MIN_MOISTURE_THRESHOLD) {
      if (waterPumpState == 0) {
        waterPumpState = 1;
        digitalWrite(WATER_PUMP_PIN, waterPumpState);
        Serial.println("Auto: Turning ON water pump due to low moisture level");
      }
    } else {
      // Moisture level is above threshold, turn off the water pump
      if (waterPumpState == 1) {
        waterPumpState = 0;
        digitalWrite(WATER_PUMP_PIN, waterPumpState);
        Serial.println("Auto: Turning OFF water pump due to sufficient moisture level");
      }
    }
  }
}

void setup(void) {
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(GROW_LIGHT_PIN, OUTPUT);
  pinMode(MOISTURE_SENSOR_0_PIN, INPUT);
  pinMode(MOISTURE_SENSOR_1_PIN, INPUT);
  pinMode(LDR_SENSOR_PIN, INPUT);  // Setup LDR sensor pin
  
  // Set default states
  digitalWrite(WATER_PUMP_PIN, waterPumpState);
  digitalWrite(GROW_LIGHT_PIN, growLightState);

  Serial.begin(SERIAL_BAUD_RATE);
  simSerial.begin(GSM_BAUD_RATE);

  Wire.begin();
  WiFi.begin(ssid, password);

  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if (!tofSensor.init()) {
    Serial.println("Failed to detect VL53L0X!");
  }

  tofSensor.setTimeout(500);
  tofSensor.startContinuous();

  // Initialize history arrays
  for (int i = 0; i < HISTORY_SIZE; i++) {
    moisture0History[i] = 0;
    moisture1History[i] = 0;
    ldrHistory[i] = 0;
    tofHistory[i] = 0;
  }

  IPAddress myIP = WiFi.localIP();
  Serial.println();
  Serial.print("Connected to WiFi! IP address: ");
  Serial.println(myIP);

  // Create routes
  server.on("/", handleRouteRoot);
  server.on("/setPinState", handleRouteSetPinState);
  server.on("/sendSmsMessage", handleRouteSendSmsMessage);
  server.on("/getStates", handleRouteGetStates);
  server.on("/getHistory", handleRouteGetHistory);

  server.begin();
  Serial.println("HTTP server started");

  // Initial readings
  moisture0Value = readMoisture0Sensor();
  moisture1Value = readMoisture1Sensor();
  ldrValue = readLdrSensor();
  tofDistance = tofSensor.readRangeContinuousMillimeters() - TOF_OFFSET;

  // Initialize history with current values
  for (int i = 0; i < HISTORY_SIZE; i++) {
    moisture0History[i] = moisture0Value;
    moisture1History[i] = moisture1Value;
    ldrHistory[i] = ldrValue;
    tofHistory[i] = tofDistance;
  }

  Serial.println("Garden Automation System initialized!");
  Serial.print("Initial Moisture Sensor 1: ");
  Serial.println(moisture0Value);
  Serial.print("Initial Moisture Sensor 2: ");
  Serial.println(moisture1Value);
  Serial.print("Initial Light Level: ");
  Serial.println(ldrValue);
}

int lastWiFiStatus = 0;
const int MAX_SENSOR_BUFFER = 20;
int sensorBufferCounter = 0;
void loop(void) {
  server.handleClient();

  if (sensorBufferCounter >= MAX_SENSOR_BUFFER) {
    tofDistance = tofSensor.readRangeContinuousMillimeters() - TOF_OFFSET;
    moisture0Value = readMoisture0Sensor();
    moisture1Value = readMoisture1Sensor();
    ldrValue = readLdrSensor();  // Read LDR sensor value
    
    updateHistory();  // Update sensor history for graphs
    handleAutomatedSmsMessage();
    handleAutoLightControl();    // Handle automatic light control
    handleAutoWaterControl();    // Handle automatic water control
    
    sensorBufferCounter = 0;
  } else {
    sensorBufferCounter++;
  }
}