//API Key
#define gServiceKey String ("[Read API Key issued from the data.go.kr]")

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
WiFiClient client;

char* ssid = "[Your Wi-Fi SSID]";
char* password = "[Your Wi-Fi Password]";
 
const int GBISUPD_INTERVAL = 20000;
const char* gHost = "apis.data.go.kr";
 
const int httpPort = 80;

int cmdSize = 0;
String busstopID = "224000481"; // Siheung City Hall
 
String rcvbuf;
long currentMillis;
long previousMillis = 0;
boolean wifi_ready;
boolean requestLocker = false;
boolean requestLocker1 = false;
boolean requestLocker2 = true;
String resultBus1, resultBus2, resultBus3;
 
void setup() {
  Serial.begin(9600);
  setup_oled();
  wifi_ready = connect_ap(ssid, password);
  if (!wifi_ready){
    nowifi_oled();
  }
}
 
 
void loop() {
  while (client.available())
  {
    char c = client.read();
    if (c != NULL) {
      //if (rcvbuf.length() > 1300)
      //rcvbuf = "";
      rcvbuf += c;
      //Serial.write(c);
      Serial.print(c);
    }
    yield();
  }
 
  if (millis() - previousMillis > GBISUPD_INTERVAL) {
    resultBus1 = gBusParseArrivalTime(FormatBusString("30-2"));
    // resultBus1 = gBusParseArrivalTime(FormatBusString(ExtractBusNum()));
    do_oled(0, 11, resultBus1);
    do_oled(0, 22, resultBus2);
    do_oled(0, 33, resultBus3);
    requestLocker2 = true;
  }

  else if (millis() - previousMillis > GBISUPD_INTERVAL - 4000 && requestLocker)
  {
    resultBus2 = gBusParseArrivalTime(FormatBusString("55"));
    //resultBus2 = gBusParseArrivalTime(FormatBusString(ExtractBusNum()));

    // 30-2
    gBusRequestArrivalTime("224000014", "224000718");
    rcvbuf = "";
    requestLocker = false;
  }

  else if (millis() - previousMillis > GBISUPD_INTERVAL - 8000 && requestLocker1)
  {
    resultBus3 = gBusParseArrivalTime(FormatBusString("22"));
    //resultBus3 = gBusParseArrivalTime(FormatBusString(ExtractBusNum()));

    // 55
    gBusRequestArrivalTime("216000011", "224000718");
    rcvbuf = "";
    requestLocker1 = false;
    requestLocker = true;
  }

  else if (millis() - previousMillis > GBISUPD_INTERVAL - 12000 && requestLocker2)
  {
    // 22
    gBusRequestArrivalTime("216000004", "224000718");
    rcvbuf = "";
    requestLocker2 = false;
    requestLocker1 = true;
  }
}
 

void gBusRequestArrivalTime(String routeId, String stationId) {
  String str = "GET /6410000/busarrivalservice/v2/getBusArrivalItemv2?serviceKey=" + gServiceKey + "&routeId=";
  str.concat(routeId);
  str.concat("&stationId=");
  str.concat(stationId);
  str.concat("&format=xml");
  str.concat(" HTTP/1.1\r\nHost:apis.data.go.kr\r\nConnection: close\r\n\r\n");

  if (client.connect(gHost, httpPort)) {
    Serial.println("gBusRequest(routeId = " + routeId + ", stationId = " + stationId + " ) -- Connected");
    Serial.println(str);
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
    Serial.println("gBusRequest(routeId = " + routeId + ", stationId = " + stationId + ") -- Connection failed ");
    return;
  }
}

String FormatBusString(String busStr) {
  switch (busStr.length()) {
    case 0:
      return "    " + busStr + "    ";
    case 1:
      return "   " + busStr + "   ";
    case 2:
      return "  " + busStr + "  ";
    case 3:
      return " " + busStr + " ";
    case 4:
      if (busStr.indexOf("-") == -1) {
        return busStr;
      } else {
        return " " + busStr;
      }
    default:
      return busStr;
  }
}
String gBusParseArrivalTime(String busNum) {
  previousMillis = millis();
  int startIndex = rcvbuf.indexOf("<predictTime1>");
  int endIndex;
  String errCode;

  if (startIndex == -1) {
    startIndex = rcvbuf.indexOf("<returnReasonCode>");
    if (startIndex == -1) {
      rcvbuf = "";
      return busNum + " : No buses in service";
    } else {
      startIndex += strlen("<returnReasonCode>");
      endIndex = rcvbuf.indexOf("<", startIndex);

      errCode = rcvbuf.substring(startIndex, endIndex);
      rcvbuf = "";
      return busNum + " : Error Code   ( " + errCode + " )";  // Corrected line
    }
  }


  int strLength = strlen("<predictTimeX>");
  endIndex = rcvbuf.indexOf("<", startIndex + strLength);
  String predictTime1 = rcvbuf.substring(startIndex + strLength, endIndex);

  startIndex = rcvbuf.indexOf("<predictTime2>") + strlen("<predictTime2>");
  endIndex = rcvbuf.indexOf("<", startIndex);
  String predictTime2 = rcvbuf.substring(startIndex, endIndex);

  if (predictTime1 == "" && predictTime2 == "")
  {
    return busNum + " : No buses in service";
  }

  if (predictTime2 == "" || predictTime1.toInt() <= 7) {
    startIndex = rcvbuf.indexOf("<locationNo1>") + strlen("<locationNo1>");
    endIndex = rcvbuf.indexOf("<", startIndex);
    String predictStop1 = rcvbuf.substring(startIndex, endIndex);
    Serial.println("gBusParse(" + busNum + " , P1 = " + predictTime1 + " , rcvbuf = " + String(rcvbuf.length()) + ")");
    rcvbuf = "";
    return busNum + " : " + (predictTime1 == "1" ? "ARRIV." : predictTime1 + " mins") + (predictStop1 != "" ? "  ( " + predictStop1 + (predictStop1 == "1" ? " stop )" : " stops )") : "");

  } else {
    Serial.println("gBusParse(" + busNum + " , P1 = " + predictTime1 + " , P2 = " + predictTime2 + " , rcvbuf = " + String(rcvbuf.length()) + ")");
    rcvbuf = "";
    return busNum + " : " + (predictTime1 == "1" ? "ARRIV." : predictTime1 + " mins") + (predictTime2 != "" ? "  ,  " + (predictTime2 == "1" ? "ARRIV." : predictTime2 + " mins") : "");
  }
}
