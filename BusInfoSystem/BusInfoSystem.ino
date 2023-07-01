#define serviceKey String ("[Read API Key issued from the data.go.kr]")
 
//API Key
 
char* ssid = "[Wi-Fi SSID]";
char* password = "[Wi-Fi Password]";
 
 
const int GBISUPD_INTERVAL = 20000;
const char* gHost = "apis.data.go.kr";
const int httpPort = 80;
 
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
WiFiClient client;
 
int cmdSize = 0;
String busstopID = "224000481"; // Siheung City Hall
 
String rcvbuf;
long currentMillis;
long previousMillis = 0;
boolean wifi_ready;
boolean requestLocker = false;
boolean requestLocker1 = false;
boolean requestLocker2 = true;
String result_26_1;
String result_63;
String result_11_3;
 
void setup() {
  Serial.begin(9600);
  setup_oled();
  wifi_ready = connect_ap(ssid, password);
  if (!wifi_ready){
    nowifi_oled();
  }
}
 
 
void loop(){
  while(client.available())
  {
    char c = client.read();
    if(c != NULL){
      if(rcvbuf.length() > 1300)
        rcvbuf = "";
      rcvbuf += c;
      //Serial.write(c);
      Serial.print(c);
    }
  }
 
  if(millis() - previousMillis > GBISUPD_INTERVAL){
    result_26_1 = parseArrivalTime("26-1");
    do_oled(0, 11, result_26_1);
    do_oled(0, 22, result_63);
    do_oled(0, 33, result_11_3);
    requestLocker2 = true;
  }
 
  else if(millis() - previousMillis > GBISUPD_INTERVAL - 4000 && requestLocker)
  {
    result_63 = parseArrivalTime("63");
     
    // 26-1
    requestArrivalTime("224000027", "224000481");  
    requestLocker = false;
  }
   
  else if(millis() - previousMillis > GBISUPD_INTERVAL - 8000 && requestLocker1)
  {
    result_11_3 = parseArrivalTime("11-3");
 
    // 63
    requestArrivalTime("224000010", "224000481");  
    requestLocker1 = false;
    requestLocker = true;
  }
 
  else if(millis() - previousMillis > GBISUPD_INTERVAL - 12000 && requestLocker2)
  {
 
    // 11-3
    requestArrivalTime("213000014", "224000481");
    requestLocker2 = false;
    requestLocker1 = true;
  }
 }
 
void RequestArrivalTime(String routeId, String stationId) {
  String str = "GET /6410000/busarrivalservice/getBusArrivalItem?serviceKey=" + gServiceKey + "&routeId=";
  str.concat(routeId);
  str.concat("&stationId=");
  str.concat(stationId);
  str.concat(" HTTP/1.1\r\nHost:apis.data.go.kr\r\nConnection: close\r\n\r\n");

  Serial.println("gBus: Attempt connection");
  Serial.println(str);
   
  if (client.connect(gHost, httpPort)) {
    Serial.println("success");
    client.print(str);
    client.println();
    cmdSize = str.length();

    client.println("AT+CIPSTART=\"TCP\",\"apis.data.go.kr\",80");
    delay(500);
    client.print("AT+CIPSEND=");
    delay(500);
    client.println(cmdSize);
    delay(500);
    client.println(str);
  }
  else {
    Serial.println("fail");
    return;
  }
}

String ParseArrivalTime(String busNum) {
  Serial.println("gBus: Parse " + busNum);
  Serial.println(rcvbuf);

  previousMillis = millis();
  int startIndex = rcvbuf.indexOf("<predictTime1>");

  if (startIndex == -1) {
    Serial.println("no buses in service.");
    rcvbuf = "";
    return busNum + " : No buses in service";
  }
  
  int strLength = strlen("<predictTimeX>");
  int endIndex = rcvbuf.indexOf("<", startIndex + strLength);
  String predictTime1 = rcvbuf.substring(startIndex + strLength, endIndex);

  startIndex = rcvbuf.indexOf("<predictTime2>") + strlen("<predictTime2>");
  endIndex = rcvbuf.indexOf("<", startIndex);
  String predictTime2 = rcvbuf.substring(startIndex, endIndex);

  if (predictTime2 == "" || predictTime1.toInt() <= 7) {
    startIndex = rcvbuf.indexOf("<locationNo1>") + strlen("<locationNo1>");
    endIndex = rcvbuf.indexOf("<", startIndex);
    String predictStop1 = rcvbuf.substring(startIndex, endIndex);

    return busNum + " : " + (predictTime1 == "1" ? "ARRIV." : predictTime1 + " mins") + (predictStop1 != "" ? " ( " + predictStop1 + (predictStop1 == "1" ? " stop )" : " stops )") : "");
  } else {
    return busNum + " : " + (predictTime1 == "1" ? "ARRIV." : predictTime1 + " mins") + (predictTime2 != "" ? " , " + (predictTime2 == "1" ? "ARRIV." : predictTime2 + " mins") : "");
  }
}
