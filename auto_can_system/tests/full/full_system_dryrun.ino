/*
 * Dry-run whole system without motors (safe on desk).
 * Replays state machine timing and sends ESP-NOW packets.
 */
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "../../firmware/shared/comm_defs.h"

uint8_t stationMac[6] = {0x24,0x6F,0x28,0x12,0x34,0x56}; // replace
RobotState simState = STATE_INIT;
uint8_t canIndex = 0;
uint8_t collected = 0;
unsigned long stateStart = 0;

void onRecv(const uint8_t*, const uint8_t*, int) {}

void sendStatus() {
  RobotStatus rs{}; rs.state = simState; rs.can_index = canIndex; rs.collected_bits = collected; rs.tof_mm = 500; rs.obstacle=false;
  esp_now_send(stationMac, (uint8_t*)&rs, sizeof(rs));
}

void advance() {
  simState = (RobotState)((simState + 1) % STATE_DONE);
  if (simState == STATE_RETURN) collected |= (1 << canIndex);
  if (simState == STATE_DONE && canIndex < 2) { canIndex++; simState = STATE_NAV_SEG1; }
  stateStart = millis();
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(onRecv);
  esp_now_peer_info_t peer{}; memcpy(peer.peer_addr, stationMac,6); peer.channel=0; peer.encrypt=false;
  esp_now_add_peer(&peer);
  stateStart = millis();
}

void loop() {
  sendStatus();
  if (millis() - stateStart > 2000) advance();
  delay(200);
}
