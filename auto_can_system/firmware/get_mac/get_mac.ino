#include <WiFi.h>

void setup(){
  Serial.begin(115200);
  delay(200);
  WiFi.mode(WIFI_STA);
  delay(100);
  Serial.print("STA MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.print("AP  MAC: ");
  Serial.println(WiFi.softAPmacAddress());
}

void loop(){}
