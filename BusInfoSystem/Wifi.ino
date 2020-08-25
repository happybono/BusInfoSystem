#include <ESP8266WiFi.h>

boolean connect_ap(char* ssid, char* password) {
  int count = 60;                                 // 최대 60 회 연결 시도 중 wifi 연결하면 성공, 아니면 실패
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    wifi_oled(count);
    if (!count--) {
      Serial.print("\nNO WIFI");
      return(false);
    }
  }
  previousMillis = millis();
  Serial.print("\n Got WiFi, IP address: ");
  Serial.println(WiFi.localIP()); 
  Serial.write(Serial.read());
  return(true);
}
