/*
 * SodaBot - Robot ESP32
 * Short scripted pickup/drop for 3 cans in 2.5 ft x 6 ft area.
 * Stepper home = UP. Stops before lift/grip. 90° turns only.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <WebServer.h>
#include <Wire.h>
#include <ESP32Servo.h>
#include <Adafruit_VL53L0X.h>
#include "../../shared/comm_defs.h"

// WiFi / web
const char* WIFI_SSID = "Shimengba";
const char* WIFI_PASS = "1234567890qwertyuiopasdfghjklzxcvbnm";
WebServer server(80);
const bool ALLOW_STANDALONE = true;

// Pins
const int PIN_ENA = 23, PIN_IN1 = 16, PIN_IN2 = 19;
const int PIN_IN3 = 26, PIN_IN4 = 27, PIN_ENB = 25;
const int PIN_TACH_L = 17, PIN_TACH_R = 35;
const int PIN_IR_L = 4, PIN_IR_R = 5;
const int PIN_LED_R = 2, PIN_LED_G = 32, PIN_LED_B = 15;
const int PIN_SERVO = 18;
const int PIN_STP_IN1 = 14, PIN_STP_IN2 = 12, PIN_STP_IN3 = 13, PIN_STP_IN4 = 33;
const int PIN_I2C_SDA = 21, PIN_I2C_SCL = 22; // VL53L0X I2C lines

// Speeds
const int PWM_MIN = 90;
const int PWM_MAX = 255;
// Slightly reduced speeds for finer control (less overshoot)
const int PWM_CRUISE = 220;
const int PWM_TURN = 220;

// Lift
const uint16_t STEP_DELAY_US = 2000;
const uint16_t STEPS_PER_REV = 200;
const float LIFT_UP_TURNS = 0.60f;  // a bit more travel to ensure clear lift
const float LIFT_DN_TURNS = 0.35f;  // slightly longer dip
const uint16_t SERVO_OPEN_DEG = 10;
const uint16_t SERVO_CLOSE_DEG = 40;
const uint16_t GRIP_MS = 600;

// Geometry
const float WHEEL_DIAMETER_MM = 65.0f;
const float ENCODER_SLOTS = 20.0f;
const float MM_PER_TICK = (PI * WHEEL_DIAMETER_MM) / ENCODER_SLOTS;
// Add ~12% to distance counts so each commanded segment runs slightly longer
const float COUNTS_PER_CM = (10.0f / MM_PER_TICK) * 1.12f;
// Slightly longer turn count to improve final realignment at the station
long COUNTS_PER_TURN = 540;
const unsigned long TURN_TIMEOUT_MS = 1400;

// Safety
const uint16_t TOF_STOP_MM = 250;
const uint16_t TOF_RESUME_MM = 320;
const uint16_t TOF_FAKE_MM = 500;

// ESP-NOW peer MACs
uint8_t stationMac[6] = {0x1C, 0xC3, 0xAB, 0xBA, 0x18, 0x38};

// State
volatile long tachL = 0, tachR = 0;
RobotState state = STATE_INIT;
uint8_t canIndex = 0;
uint8_t collectedBits = 0;
bool obstacleStop = false;
uint16_t lastTof = TOF_FAKE_MM;
bool irLState = false, irRState = false;
unsigned long motionStartMs = 0;

Servo gripper;
Adafruit_VL53L0X lox;
bool tofReady = false;

// Command queue
enum CmdType : uint8_t { CMD_FWD, CMD_REV, CMD_TURN_L, CMD_TURN_R, CMD_PICK, CMD_DROP, CMD_WAIT_LIMIT, CMD_WAIT_IR };
struct Cmd { CmdType t; int16_t v; };
const char* cmdNameEnum(CmdType t){
  switch(t){
    case CMD_FWD: return "FWD";
    case CMD_REV: return "REV";
    case CMD_TURN_L: return "TURN_L";
    case CMD_TURN_R: return "TURN_R";
    case CMD_PICK: return "PICK";
    case CMD_DROP: return "DROP";
    case CMD_WAIT_LIMIT: return "WAIT_LIMIT";
    case CMD_WAIT_IR: return "WAIT_IR";
    default: return "?";
  }
}

const Cmd PATH_CMDS[] = {
  // Round 1
  {CMD_FWD,20}, {CMD_TURN_L,0}, {CMD_FWD,12}, {CMD_TURN_R,0}, {CMD_FWD,4},
  {CMD_WAIT_IR,0}, {CMD_PICK,0},
  {CMD_REV,4}, {CMD_TURN_R,0}, {CMD_FWD,12}, {CMD_TURN_L,0}, {CMD_REV,20},
  {CMD_DROP,0}, {CMD_WAIT_LIMIT,0}, {CMD_TURN_R,0}, {CMD_FWD,10}, {CMD_TURN_L,0},
  // Round 2
  {CMD_FWD,60}, {CMD_TURN_L,0}, {CMD_FWD,40}, {CMD_TURN_R,0}, {CMD_FWD,5},
  {CMD_WAIT_IR,0}, {CMD_PICK,0},
  {CMD_REV,5}, {CMD_TURN_R,0}, {CMD_FWD,40}, {CMD_TURN_L,0}, {CMD_REV,60},
  {CMD_DROP,0}, {CMD_WAIT_LIMIT,1}, {CMD_TURN_R,0}, {CMD_FWD,10}, {CMD_TURN_L,0},
  // Round 3
  {CMD_FWD,110}, {CMD_TURN_L,0}, {CMD_FWD,25},
  {CMD_WAIT_IR,0}, {CMD_PICK,0},
  {CMD_REV,25}, {CMD_TURN_R,0}, {CMD_FWD,25}, {CMD_TURN_R,0}, {CMD_FWD,110},
  {CMD_DROP,0}, {CMD_WAIT_LIMIT,2}, {CMD_TURN_L,0}
};
const size_t PATH_LEN = sizeof(PATH_CMDS)/sizeof(PATH_CMDS[0]);
size_t cmdIdx = 0;
bool cmdActive = false;
StationReport latestStation{}; // declare early for use in runCommand

const char* stateName(uint8_t s){
  switch(s){
    case STATE_INIT: return "INIT";
    case STATE_NAV_SEG1: return "NAV";
    case STATE_DONE: return "DONE";
    case STATE_OBSTACLE_HALT: return "HALT";
    default: return "?";
  }
}

// Stepper seq
const uint8_t STP_SEQ[4][4] = {{1,0,1,0},{0,1,1,0},{0,1,0,1},{1,0,0,1}};

// Helpers
void setLED(uint8_t r,uint8_t g,uint8_t b){ analogWrite(PIN_LED_R,r); analogWrite(PIN_LED_G,g); analogWrite(PIN_LED_B,b); }

void updateLED() {
  uint8_t r=0,g=0,b=40; // default idle blue
  if (obstacleStop || state==STATE_OBSTACLE_HALT) { r=255; g=0; b=0; }
  else if (cmdIdx >= PATH_LEN) { r=0; g=255; b=0; } // done
  else {
    CmdType cur = (cmdIdx<PATH_LEN)? PATH_CMDS[cmdIdx].t : CMD_WAIT_LIMIT;
    switch(cur){
      case CMD_FWD: case CMD_REV: r=0; g=180; b=255; break;           // moving
      case CMD_TURN_L: case CMD_TURN_R: r=200; g=0; b=255; break;     // turning
      case CMD_WAIT_IR: r=0; g=100; b=255; break;                     // searching can
      case CMD_PICK: r=255; g=200; b=0; break;                        // gripping
      case CMD_DROP: r=255; g=255; b=160; break;                      // dropping
      case CMD_WAIT_LIMIT: r=255; g=128; b=0; break;                  // waiting bin
      default: break;
    }
  }
  setLED(r,g,b);
}

void drivePwm(int left,int right){
  left = constrain(left,-PWM_MAX,PWM_MAX);
  right= constrain(right,-PWM_MAX,PWM_MAX);
  left = -left; // invert left wiring
  digitalWrite(PIN_IN1,left>=0); digitalWrite(PIN_IN2,left<0); analogWrite(PIN_ENA,abs(left));
  digitalWrite(PIN_IN3,right>=0); digitalWrite(PIN_IN4,right<0); analogWrite(PIN_ENB,abs(right));
}
void stopMotors(){ drivePwm(0,0); }

bool turnRight90(){
  long delta=(abs(tachL)+abs(tachR))/2;
  drivePwm(PWM_TURN,-PWM_TURN);
  if(delta>=COUNTS_PER_TURN) { stopMotors(); return true; }
  if(motionStartMs && (millis()-motionStartMs > TURN_TIMEOUT_MS)) { stopMotors(); return true; }
  return false;
}
bool turnLeft90(){
  long delta=(abs(tachL)+abs(tachR))/2;
  drivePwm(-PWM_TURN,PWM_TURN);
  if(delta>=COUNTS_PER_TURN) { stopMotors(); return true; }
  if(motionStartMs && (millis()-motionStartMs > TURN_TIMEOUT_MS)) { stopMotors(); return true; }
  return false;
}

void stepperStepIdx(uint8_t idx){
  digitalWrite(PIN_STP_IN1,STP_SEQ[idx][0]);
  digitalWrite(PIN_STP_IN2,STP_SEQ[idx][1]);
  digitalWrite(PIN_STP_IN3,STP_SEQ[idx][2]);
  digitalWrite(PIN_STP_IN4,STP_SEQ[idx][3]);
}
void stepperTurns(float turns,bool up){
  if(turns<=0)return;
  long steps=(long)(turns*STEPS_PER_REV);
  int dir= up ? -1 : 1; // inverted to match wiring
  int idx=0;
  for(long i=0;i<steps;i++){
    idx=(idx+dir+4)%4;
    stepperStepIdx(idx);
    delayMicroseconds(STEP_DELAY_US);
  }
}
void stepperRelease(){
  digitalWrite(PIN_STP_IN1,LOW); digitalWrite(PIN_STP_IN2,LOW);
  digitalWrite(PIN_STP_IN3,LOW); digitalWrite(PIN_STP_IN4,LOW);
}

void resetEnc(){ tachL=tachR=0; }

bool driveDistanceCm(long cmTarget,bool forward=true,unsigned long timeoutMs=7000){
  long targetCounts=(long)(cmTarget*COUNTS_PER_CM);
  long avg=(abs(tachL)+abs(tachR))/2;
  long error=tachL-tachR;
  int corr=constrain(error*0.6,-50,50);
  int base=PWM_CRUISE;
  int left=forward?(base-corr):-(base-corr);
  int right=forward?(base+corr):-(base+corr);
  drivePwm(left,right);
  if(cmdActive && ((millis()-motionStartMs)%250<5)) Serial.printf("DRV %s %ldcm avg=%ld L=%d R=%d\n",forward?"FWD":"REV",cmTarget,avg,left,right);
  if(avg>=targetCounts){ stopMotors(); return true; }
  if(timeoutMs && (millis()-motionStartMs>timeoutMs)){ stopMotors(); return true; }
  return false;
}

bool irBoth(){ return digitalRead(PIN_IR_L)==LOW && digitalRead(PIN_IR_R)==LOW; }

uint16_t readTofMm(){
  if(!tofReady) return TOF_FAKE_MM;
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure,false);
  if(measure.RangeStatus != 4) return measure.RangeMilliMeter;
  return TOF_FAKE_MM;
}

bool runCommand(const Cmd& c){
  switch(c.t){
    case CMD_FWD:
      if(!cmdActive){ resetEnc(); motionStartMs=millis(); cmdActive=true; Serial.printf("CMD start %s %dcm\n",cmdNameEnum(c.t),c.v); }
      if(driveDistanceCm(c.v,true)){ cmdActive=false; return true; }
      return false;
    case CMD_REV:
      if(!cmdActive){ resetEnc(); motionStartMs=millis(); cmdActive=true; Serial.printf("CMD start %s %dcm\n",cmdNameEnum(c.t),c.v); }
      if(driveDistanceCm(c.v,false)){ cmdActive=false; return true; }
      return false;
    case CMD_TURN_L:
      if(!cmdActive){ resetEnc(); motionStartMs=millis(); cmdActive=true; Serial.println("CMD start TURN_L"); }
      if(turnLeft90()){ stopMotors(); Serial.println("TURN L 90"); cmdActive=false; return true; }
      return false;
    case CMD_TURN_R:
      if(!cmdActive){ resetEnc(); motionStartMs=millis(); cmdActive=true; Serial.println("CMD start TURN_R"); }
      if(turnRight90()){ stopMotors(); Serial.println("TURN R 90"); cmdActive=false; return true; }
      return false;
    case CMD_WAIT_IR:
      if(!cmdActive){ cmdActive=true; Serial.println("CMD start WAIT_IR"); }
      stopMotors();
      return irBoth();
    case CMD_PICK:
      Serial.println("CMD start PICK");
      stopMotors(); delay(80);
      stepperTurns(LIFT_DN_TURNS,false);
      delay(120); // settle
      gripper.write(SERVO_CLOSE_DEG);
      delay(GRIP_MS);
      delay(120); // hold before lifting
      stepperTurns(LIFT_UP_TURNS,true);
      Serial.println("PICK complete");
      return true;
    case CMD_DROP:
      Serial.println("CMD start DROP");
      stopMotors(); delay(80);
      stepperTurns(LIFT_DN_TURNS,false); // lower before release
      delay(120);
      gripper.write(SERVO_OPEN_DEG);
      delay(200);
      stepperTurns(LIFT_UP_TURNS,true); // always return to home up (hold coils for torque)
      Serial.println("DROP complete");
      return true;
    case CMD_WAIT_LIMIT:
      if(!cmdActive){ cmdActive=true; Serial.printf("CMD start WAIT_LIMIT bin %d\n",c.v); }
      return latestStation.limit[c.v];
  }
  return true;
}

// ESP-NOW
unsigned long lastStatusSent=0;
void onDataRecv(const esp_now_recv_info_t*, const uint8_t* data, int len){
  if(len==sizeof(StationReport)) memcpy(&latestStation,data,sizeof(StationReport));
}
void onDataSent(const esp_now_send_info_t*, esp_now_send_status_t){}

void sendStatus(){
  RobotStatus rs{}; rs.state=state; rs.can_index=canIndex; rs.collected_bits=collectedBits; rs.tof_mm=lastTof; rs.obstacle=obstacleStop;
  esp_now_send(stationMac,(uint8_t*)&rs,sizeof(rs));
}

// Web status
String statusJson(){
  char buf[320];
  snprintf(buf,sizeof(buf),
    "{\"state\":%u,\"state_name\":\"%s\",\"cmd\":%u,\"cmd_name\":\"%s\",\"idx\":%u,"
    "\"irL\":%s,\"irR\":%s,\"obstacle\":%s,\"tof\":%u,\"collected\":%u}",
    state, stateName(state),
    (cmdIdx<PATH_LEN)?PATH_CMDS[cmdIdx].t:255,
    (cmdIdx<PATH_LEN)?cmdNameEnum(PATH_CMDS[cmdIdx].t):"?",
    (unsigned)cmdIdx,
    irLState?"true":"false", irRState?"true":"false",
    obstacleStop?"true":"false", lastTof, collectedBits);
  return String(buf);
}
const char PAGE[] PROGMEM = "<!doctype html><html><body style='font-family:sans-serif;background:#0b1a2a;color:#e7f2ff;padding:16px'>"
"<h2>SodaBot Robot</h2><div id=s>Loading...</div><script>"
"async function r(){try{const j=await (await fetch('/status')).json();"
"document.getElementById('s').innerHTML=`State ${j.state_name}<br>Cmd ${j.cmd_name} (idx ${j.idx})<br>IR L:${j.irL} R:${j.irR}<br>TOF:${j.tof} mm<br>Obstacle:${j.obstacle}<br>Collected bits:${j.collected}`;}catch(e){s.innerText=e;}}"
"r();setInterval(r,1500);</script></body></html>";

void setupWeb(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID,WIFI_PASS);
  unsigned long start=millis();
  while(WiFi.status()!=WL_CONNECTED && millis()-start<6000) delay(200);
  server.on("/status",[]{server.send(200,"application/json",statusJson());});
  server.on("/",[]{server.send_P(200,"text/html",PAGE);});
  server.begin();
}

// Setup
void setup(){
  Serial.begin(115200);
  pinMode(PIN_IN1,OUTPUT); pinMode(PIN_IN2,OUTPUT); pinMode(PIN_ENA,OUTPUT);
  pinMode(PIN_IN3,OUTPUT); pinMode(PIN_IN4,OUTPUT); pinMode(PIN_ENB,OUTPUT);
  pinMode(PIN_IR_L,INPUT_PULLUP); pinMode(PIN_IR_R,INPUT_PULLUP);
  pinMode(PIN_LED_R,OUTPUT); pinMode(PIN_LED_G,OUTPUT); pinMode(PIN_LED_B,OUTPUT);
  pinMode(PIN_STP_IN1,OUTPUT); pinMode(PIN_STP_IN2,OUTPUT); pinMode(PIN_STP_IN3,OUTPUT); pinMode(PIN_STP_IN4,OUTPUT);
  pinMode(PIN_TACH_L,INPUT); pinMode(PIN_TACH_R,INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_TACH_L),[]{tachL++;},RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_TACH_R),[]{tachR++;},RISING);

  // TOF init (explicit I2C pins)
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  Wire.setClock(400000);
  if(lox.begin(0x29, false, &Wire)){
    tofReady = true;
    Serial.println("VL53L0X ready on I2C (SDA=21, SCL=22)");
  } else {
    Serial.println("VL53L0X not found (check power/SDA=21/SCL=22/XSHUT high) - using fake distance");
    tofReady = false;
  }

  gripper.attach(PIN_SERVO,500,2400);
  gripper.write(SERVO_OPEN_DEG);
  // ensure lift starts UP (home)
  stepperTurns(LIFT_UP_TURNS,true);

  WiFi.mode(WIFI_STA);
  if(esp_now_init()!=ESP_OK){ Serial.println("ESP-NOW init fail"); while(true) delay(1000); }
  esp_now_register_recv_cb(onDataRecv);
  esp_now_register_send_cb(onDataSent);
  esp_now_peer_info_t peer{}; memcpy(peer.peer_addr,stationMac,6); peer.channel=0; peer.encrypt=false; esp_now_add_peer(&peer);

  setupWeb();
  setLED(0,0,255);
}

// Loop
void loop(){
  if(WiFi.status()==WL_CONNECTED) server.handleClient();

  lastTof = readTofMm();
  irLState = digitalRead(PIN_IR_L)==LOW;
  irRState = digitalRead(PIN_IR_R)==LOW;

  if(lastTof < TOF_STOP_MM){ obstacleStop=true; state=STATE_OBSTACLE_HALT; }
  else if(obstacleStop && lastTof > TOF_RESUME_MM){ obstacleStop=false; state=STATE_NAV_SEG1; }

  if(ALLOW_STANDALONE && !latestStation.ack){ latestStation.ack=true; latestStation.limit[0]=latestStation.limit[1]=latestStation.limit[2]=true; }

  switch(state){
    case STATE_INIT:
      resetEnc(); stopMotors(); setLED(0,0,255);
      if(latestStation.ack){ state=STATE_NAV_SEG1; cmdIdx=0; cmdActive=false; motionStartMs=millis(); }
      break;
    case STATE_NAV_SEG1:
      if(cmdIdx<PATH_LEN){
        if(runCommand(PATH_CMDS[cmdIdx])){
          Serial.printf("CMD %u done (%s)\n",(unsigned)cmdIdx,cmdNameEnum(PATH_CMDS[cmdIdx].t));
          if(PATH_CMDS[cmdIdx].t==CMD_WAIT_LIMIT && canIndex<3) { collectedBits |= (1<<canIndex); canIndex++; }
          cmdIdx++; cmdActive=false; resetEnc();
        }
      }else{ state=STATE_DONE; }
      break;
    case STATE_DONE:
      stopMotors(); setLED(0,255,0);
      break;
    case STATE_OBSTACLE_HALT:
      stopMotors(); setLED(255,0,0);
      break;
    default: break;
  }

  if(millis()-lastStatusSent>500){ sendStatus(); lastStatusSent=millis(); }
  updateLED();
}
