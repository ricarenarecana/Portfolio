// Minimal ESP-NOW pair test - Robot side
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "../../firmware/shared/comm_defs.h"

uint8_t stationMac[6] = {0x24,0x6F,0x28,0x12,0x34,0x56}; // replace
unsigned long lastSend = 0;
uint32_t counter = 0;

void onRecv(const uint8_t *mac, const uint8_t *data, int len) {
  Serial.print("Robot got "); Serial.print(len); Serial.println(" bytes");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(onRecv);
  esp_now_peer_info_t peer{}; memcpy(peer.peer_addr, stationMac,6); peer.channel=0; peer.encrypt=false;
  esp_now_add_peer(&peer);
}

void loop() {
  if (millis() - lastSend > 500) {
    RobotStatus rs{}; rs.state = STATE_INIT; rs.can_index = 0; rs.collected_bits = counter & 0x07;
    esp_now_send(stationMac, (uint8_t*)&rs, sizeof(rs));
    Serial.print("Robot sent count "); Serial.println(counter++);
    lastSend = millis();
  }
}
