// Minimal ESP-NOW pair test - Station side
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "../../firmware/shared/comm_defs.h"

uint8_t robotMac[6] = {0x24,0x6F,0x28,0x65,0x43,0x21}; // replace
unsigned long lastSend = 0;
uint32_t counter = 0;

void onRecv(const uint8_t *mac, const uint8_t *data, int len) {
  Serial.print("Station got "); Serial.print(len); Serial.println(" bytes");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(onRecv);
  esp_now_peer_info_t peer{}; memcpy(peer.peer_addr, robotMac,6); peer.channel=0; peer.encrypt=false;
  esp_now_add_peer(&peer);
}

void loop() {
  if (millis() - lastSend > 600) {
    StationReport rep{}; rep.ack=true; rep.limit[0]=(counter&1); rep.limit[1]=(counter&2); rep.limit[2]=(counter&4);
    esp_now_send(robotMac, (uint8_t*)&rep, sizeof(rep));
    Serial.print("Station sent mask "); Serial.println(counter & 0x7);
    counter++;
    lastSend = millis();
  }
}
