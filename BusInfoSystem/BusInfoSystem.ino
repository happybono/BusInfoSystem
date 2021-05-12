#define serviceKey String ("[GBIS Read API Key]")
 
//API Key
 
char* ssid = "[Wi-Fi SSID]";
char* password = "[Wi-Fi Password]";
 
 
const int GBISUPD_INTERVAL = 20000;
const char* host = "apis.data.go.kr";
 
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
 
void requestArrivalTime(String routeId, String stationId) {
  String str = "GET /6410000/busarrivalservice/getBusArrivalItem?serviceKey=" + serviceKey + "&routeId=";
  str.concat(routeId);
  str.concat("&stationId=");
  str.concat(stationId);
  str.concat(" HTTP/1.1\rnHost:apis.data.go.kr\r\nConnection: close\r\n\r\n");
 
  if(client.connect(host, httpPort)){
  Serial.println("connected");
  Serial.print(str);
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
    Serial.println("Connection Failed: ");
    return;
  }
}
 
 
String parseArrivalTime(String busNum)
{
  previousMillis = millis();
  int startIndex = rcvbuf.indexOf("<predictTime1>");
  if(startIndex == -1){ 
    rcvbuf = "";
    return busNum + " :     no bus";
  }
 
  int strLength = strlen("<predictTime1>");
  int endIndex = rcvbuf.indexOf("<", startIndex + strLength);
  String predictTime1 = rcvbuf.substring(startIndex+strLength,endIndex);
  startIndex = rcvbuf.indexOf("<predictTime2>");
  strLength = strlen("<predictTime2>");
  endIndex = rcvbuf.indexOf("<", startIndex + strLength);
  String predictTime2 = rcvbuf.substring(startIndex+strLength,endIndex);
 
  if(predictTime2.equals("") && predictTime1 == "1"){
    return busNum + " : " + predictTime1 + "min";
  }
  else if(predictTime2.equals("") && predictTime1 != "1"){
  return busNum + " : " + predictTime1 + "mins";
  }
  
  Serial.println("===========");
  Serial.println(predictTime1);
  Serial.println(predictTime2);
  Serial.println("===========");
  rcvbuf = "";
   
  if (predictTime1 != "1" && predictTime2 != "1") { 
  return busNum + " :     " + predictTime1 + "mins, " + predictTime2 + "mins";
  }
  else if (predictTime1 == "1" && predictTime2 != "1"){
  return busNum + " :     " + predictTime1 + "min, " + predictTime2 + "mins";
  }
  else if (predictTime1 != "1" && predictTime2 == "1"){
  return busNum + " :     " + predictTime1 + "mins, " + predictTime2 + "min";
  }  
}
