#include "SSD1306.h"
SSD1306 display(0x3c, D3, D4);

void setup_oled(){
  display.init();
  display.clear();  
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0,13, "initialize OLED...");  
  display.display();
}

void wifi_oled(int cnt) {
  display.clear();  
  display.setFont(ArialMT_Plain_10);
  display.drawString(0,13, "waiting wifi..."); 
  display.drawString(0,24, String("ssid : ")+String(ssid));
  display.drawString(0,35, String("password : ")+ String(password)); 
  display.drawString(0,53, String(cnt));
  display.display();  
}

void nowifi_oled() {
  display.clear();  
  display.setFont(ArialMT_Plain_10);
  display.drawString(0,0, "NO WIFI: skip wifi..."); 
  display.display();  
}

void do_oled(int16_t x, int16_t y, String result) {
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0,0, "Bus No.");
  display.drawString(64,0, "Time Left");
  display.setFont(ArialMT_Plain_10);
  display.drawString(0,13, result_30_2);
  display.drawString(0,29, result_55);
  display.drawString(0,45, result_22);
  display.display();
  Serial.println(result);
}
