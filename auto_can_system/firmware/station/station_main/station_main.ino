/*
 * auto_can_system - Station ESP32 (DevKit)
 * Reads three limit switches, displays all bin statuses on one OLED,
 * exchanges status with Robot via ESP-NOW, and serves a small status web page.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WebServer.h>
#include "../../shared/comm_defs.h"

// WiFi for status page
const char* WIFI_SSID = "Shimengba";
const char* WIFI_PASS = "1234567890qwertyuiopasdfghjklzxcvbnm";
WebServer server(80);
String stationIp = "offline";

// Pins
const int PIN_LIMIT[3] = {19, 23, 15}; // active LOW
const int PIN_LED_R = 2, PIN_LED_G = 12, PIN_LED_B = 0;

// Single OLED on I2C 21/22
Adafruit_SSD1306 oled(128,64,&Wire,-1);
const uint8_t OLED_ADDR = 0x3C; // set your module to 0x3C or 0x3D

// Comms
uint8_t robotMac[6] = {0x1C, 0xC3, 0xAB, 0xBA, 0x0B, 0xE0}; // robot STA MAC
RobotStatus lastRobot{};
StationReport report{};
unsigned long lastRx = 0, lastTx = 0;
unsigned long lastIpPrint = 0;
wl_status_t lastWifiStatus = WL_IDLE_STATUS;

void setLED(uint8_t r, uint8_t g, uint8_t b) {
  analogWrite(PIN_LED_R, r); analogWrite(PIN_LED_G, g); analogWrite(PIN_LED_B, b);
}

void onRobotMsg(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (len == sizeof(RobotStatus)) { memcpy(&lastRobot, data, sizeof(RobotStatus)); lastRx = millis(); }
}
void onSent(const esp_now_send_info_t *, esp_now_send_status_t) { }

void initOled(Adafruit_SSD1306 &o, uint8_t addr) {
  o.begin(SSD1306_SWITCHCAPVCC, addr);
  o.clearDisplay(); o.setTextSize(1); o.setTextColor(SSD1306_WHITE); o.display();
}

void drawStatus(bool linkOk) {
  oled.clearDisplay();
  oled.setTextSize(2);
  oled.setCursor(0,0);
  oled.println("Bins");
  oled.printf("1:%s\n", report.limit[0] ? "Full" : "Empty");
  oled.printf("2:%s\n", report.limit[1] ? "Full" : "Empty");
  oled.printf("3:%s\n", report.limit[2] ? "Full" : "Empty");
  oled.setTextSize(1);
  oled.printf("Link:%s ", linkOk ? "OK" : "LOST");
  oled.printf("St:%u Can:%u", lastRobot.state, lastRobot.can_index+1);
  oled.display();
}

String statusJson(){
  char buf[360];
  snprintf(buf,sizeof(buf),
    "{\"bins\":[%s,%s,%s],\"link\":%s,\"robot_state\":%u,\"robot_can\":%u,\"collected\":%u,"
    "\"robot_tof\":%u,\"robot_obstacle\":%s,\"ip\":\"%s\"}",
    report.limit[0]?"true":"false",
    report.limit[1]?"true":"false",
    report.limit[2]?"true":"false",
    ((millis()-lastRx)<2000)?"true":"false",
    lastRobot.state, lastRobot.can_index+1, lastRobot.collected_bits,
    lastRobot.tof_mm, lastRobot.obstacle?"true":"false",
    stationIp.c_str());
  return String(buf);
}

const char PAGE[] PROGMEM = R"HTML(
<!doctype html><html><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>SodaBot | Station</title>
<style>
:root{--bg:#060912;--panel:#0f162b;--border:#203463;--accent:#ff7bd8;--accent2:#6cf5ff;--text:#eef3ff;--ok:#18d47b;--warn:#f5b400;--err:#ff4f6d;}
*{box-sizing:border-box;font-family:'Inter',system-ui,-apple-system,sans-serif;}
body{margin:0;padding:24px;min-height:100vh;background:radial-gradient(circle at 15% 25%,rgba(255,123,216,0.18),transparent 28%),radial-gradient(circle at 80% 10%,rgba(108,245,255,0.18),transparent 28%),var(--bg);color:var(--text);}
.wrap{max-width:620px;margin:auto;}
.card{background:var(--panel);border:1px solid var(--border);border-radius:18px;padding:18px 20px;box-shadow:0 14px 42px rgba(0,0,0,.45);}
.title{display:flex;align-items:center;gap:10px;margin:0 0 12px;}
.pill{padding:6px 12px;border-radius:999px;font-weight:700;font-size:12px;border:1px solid var(--border);letter-spacing:0.5px;}
.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(150px,1fr));gap:10px;}
.item{background:rgba(255,255,255,0.04);border:1px solid var(--border);border-radius:12px;padding:10px;}
.label{font-size:12px;opacity:.8;text-transform:uppercase;letter-spacing:0.4px;}
.value{font-size:18px;font-weight:800;}
.badge{padding:4px 8px;border-radius:10px;font-weight:800;font-size:12px;color:#041016;}
.ok{background:var(--ok);} .warn{background:var(--warn);} .err{background:var(--err);}
.section{margin-top:14px;font-weight:700;opacity:.85;letter-spacing:0.6px;}
button{margin-top:14px;width:100%;padding:12px;border:none;border-radius:12px;background:linear-gradient(135deg,var(--accent),var(--accent2));color:#1a0a16;font-weight:800;font-size:15px;cursor:pointer;box-shadow:0 8px 20px rgba(0,0,0,.35);}
button:active{transform:translateY(1px);}
</style></head><body>
<div class="wrap">
  <div class="card">
    <div class="title">
      <span style="font-size:24px;">📦</span>
      <div>
        <div style="font-weight:900;font-size:20px;letter-spacing:0.6px;">SodaBot · Station</div>
        <div class="pill" id="link-pill">Link —</div>
        <div class="pill" id="ip-pill">IP —</div>
      </div>
    </div>
    <div class="section">Bins</div>
    <div class="grid" id="bins"></div>
    <div class="section">Robot</div>
    <div class="grid">
      <div class="item"><div class="label">Robot state</div><div class="value" id="state"></div></div>
      <div class="item"><div class="label">Target can</div><div class="value" id="can"></div></div>
      <div class="item"><div class="label">Collected bits</div><div class="value" id="col"></div></div>
      <div class="item"><div class="label">TOF (mm)</div><div class="value" id="tof"></div></div>
      <div class="item"><div class="label">Obstacle</div><div class="value"><span class="badge ok" id="obs">no</span></div></div>
    </div>
    <button onclick="refresh()">Refresh</button>
  </div>
</div>
<script>
function binBadge(full){return `<span class="badge ${full?'ok':'warn'}">${full?'Full':'Empty'}</span>`;}
async function refresh(){
  try{
    const r=await fetch('/status'); const j=await r.json();
    const link=document.getElementById('link-pill');
    link.textContent='Link '+(j.link?'OK':'Lost');
    link.className='pill '+(j.link?'ok':'err');
    const ip=document.getElementById('ip-pill');
    ip.textContent='IP '+j.ip;
    document.getElementById('bins').innerHTML=j.bins.map((b,i)=>`
      <div class="item"><div class="label">Bin ${i+1}</div><div class="value">${binBadge(b)}</div></div>`).join('');
    document.getElementById('state').textContent=j.robot_state;
    document.getElementById('can').textContent='#'+j.robot_can;
    document.getElementById('col').textContent=j.collected;
    document.getElementById('tof').textContent=j.robot_tof;
    const obs=document.getElementById('obs');
    obs.textContent=j.robot_obstacle?'YES':'no';
    obs.className='badge '+(j.robot_obstacle?'warn':'ok');
  }catch(e){
    document.getElementById('link-pill').textContent='Error';
    document.getElementById('ip-pill').textContent='Error';
  }
}
refresh(); setInterval(refresh,1500);
</script></body></html>
)HTML";

void setupWeb(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  unsigned long start=millis();
  while (WiFi.status()!=WL_CONNECTED && millis()-start<8000) {
    if ((millis()-start)%1000 < 50) Serial.printf("Connecting to %s...\n", WIFI_SSID);
    delay(200);
  }
  if (WiFi.status()==WL_CONNECTED) {
    stationIp = WiFi.localIP().toString();
    Serial.print("Station WiFi IP: "); Serial.println(stationIp);
    lastIpPrint = millis();
  } else {
    stationIp = "offline";
    Serial.printf("Station WiFi failed (status %d). Offline mode.\n", WiFi.status());
  }
  server.on("/status", [](){ server.send(200,"application/json",statusJson()); });
  server.on("/", [](){ server.send_P(200,"text/html",PAGE); });
  server.begin();
}

void setup() {
  Serial.begin(115200);
  for (int i=0;i<3;i++) pinMode(PIN_LIMIT[i], INPUT_PULLUP);
  pinMode(PIN_LED_R, OUTPUT); pinMode(PIN_LED_G, OUTPUT); pinMode(PIN_LED_B, OUTPUT);
  setLED(0,0,128);

  Wire.begin(21,22);
  initOled(oled, OLED_ADDR);

  WiFi.mode(WIFI_STA);
  if (esp_now_init()!=ESP_OK) { Serial.println("ESP-NOW init failed"); while(true) delay(1000); }
  esp_now_register_recv_cb(onRobotMsg);
  esp_now_register_send_cb(onSent);
  esp_now_peer_info_t peer{}; memcpy(peer.peer_addr, robotMac,6); peer.channel=0; peer.encrypt=false; esp_now_add_peer(&peer);

  report.ack = true;
  setupWeb();
}

void loop() {
  wl_status_t cur = WiFi.status();
  if (cur != lastWifiStatus) {
    Serial.printf("WiFi status changed: %d -> %d\n", lastWifiStatus, cur);
    lastWifiStatus = cur;
  }
  if (cur==WL_CONNECTED) {
    server.handleClient();
    if (millis() - lastIpPrint > 5000) {
      stationIp = WiFi.localIP().toString();
      Serial.print("Station WiFi IP: "); Serial.println(stationIp);
      lastIpPrint = millis();
    }
  } else if (cur==WL_DISCONNECTED || cur==WL_CONNECTION_LOST) {
    if (millis() - lastIpPrint > 5000) {
      Serial.println("WiFi disconnected, retrying...");
      WiFi.disconnect();
      WiFi.begin(WIFI_SSID, WIFI_PASS);
      lastIpPrint = millis();
    }
  }

  // switches
  for (int i=0;i<3;i++) report.limit[i] = (digitalRead(PIN_LIMIT[i])==LOW);

  bool linkOk = (millis() - lastRx) < 2000;
  uint8_t collected = lastRobot.collected_bits;
  if (!linkOk) setLED(255,0,0);
  else if (collected == 0b111) setLED(0,255,0);
  else setLED(255,255,0);

  drawStatus(linkOk);

  if (millis() - lastTx > 500) { esp_now_send(robotMac, (uint8_t*)&report, sizeof(report)); lastTx = millis(); }
  delay(50);
}
