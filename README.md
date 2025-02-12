# BusInfoSystem
Bus Information System for Gyeonggi province and Incheon metropolitan city, South Korea. NodeMCU (ESP8266) has been used as the main microcontroller unit, estimated arrival time calculated based on the real-time bus location data retrieved from the public data server once every 20 seconds. Displays refined information on the OLED Screen. 

<div align="center">
<img alt="GitHub Last Commit" src="https://img.shields.io/github/last-commit/happybono/BusInfoSystem"> 
<img alt="GitHub Repo Size" src="https://img.shields.io/github/repo-size/happybono/BusInfoSystem">
<img alt="GitHub Repo Languages" src="https://img.shields.io/github/languages/count/happybono/BusInfoSystem">
<img alt="GitHub Top Languages" src="https://img.shields.io/github/languages/top/HappyBono/BusInfoSystem">
</div>

## What's New
<details>
<summary>Click to Expand</summary>

### v1.0
#### August 25, 2020  
Initial release.

#### August 26, 2020  
Added Open API document which the Government of South Korea provides. </br>
Modified variables to be consistent with bus route numbers.

#### August 29, 2020
Bug Fixed : When there are "no buses in service," the variable "rcvbuf" is not cleared.

### v2.0
#### May 12, 2021
Now supports modified API and renewaled endpoint URL.

#### July 1, 2023
Improvements were made to serial logs to display more clearly.

### v3.0
#### January 12, 2025
Now supports modified API and renewaled endpoint URL. </br>
Bugs fixed.

#### February 9, 2025
Enhanced the system to retrieve bus route numbers from the API dynamically. </br>
Improved the Gyeonggi bus information system system to display an error message if the server fails to respond due to an error. </br>
Bugs fixed.

### v4.0
#### February 9, 2025
Now supports Incheon bus information system. </br>
Improved the Incheon bus information system to display an error message if the server fails to respond due to an error. </br>
If the second bus with the same route number (has not departed/is not scheduled) or the first bus to arrive is less than or equal to 7 minutes, the estimated arrival time and the number of remaining stops for the first bus will display on the OLED Screen. </br>
</details>

## Project Setup
1. Please download and install the necessary drivers and libraries.
2. Replace `[Read API Key issued from the data.go.kr]` with your actual API key.
3. Replace `[Your Wi-Fi SSID]` and `[Your Wi-Fi Password]` with your Wi-Fi credentials.
4. Upload the code to your ESP8266 module.

## Function Descriptions & Usage

<details>
<summary>Click to Expand</summary></br>
  
### `setup`
Initializes the serial communication, OLED display, and Wi-Fi connection.

#### Example
```c++
void setup() {
  Serial.begin(9600);
  setup_oled();
  wifi_ready = connect_ap(ssid, password);
  if (!wifi_ready){
    nowifi_oled();
  }
}
```
</br>

### `loop`
Main loop that checks for client data and updates the bus arrival times at set intervals.

#### Example
```c++
void loop() {
  while (client.available()) {
    char c = client.read();
    if (c != NULL) {
      rcvbuf += c;
      Serial.print(c);
    }
    yield();
  }

  if (millis() - previousMillis > BISUPD_INTERVAL) {
    resultBus1 = gBusParseArrivalTime(FormatBusString(ExtractBusNum()));
    do_oled(0, 11, resultBus1);
    do_oled(0, 22, resultBus2);
    do_oled(0, 33, resultBus3);
    requestLocker2 = true;
  }
  // Additional conditions...
}
```
</br>

### `gBusRequestArrivalTime`
Sends a GET request to the API to get the arrival time of a specific bus at a particular station.

#### Parameters
`routeId (String)` : The route ID of the bus. </br>
`bstopId (String)` : The station (bus stop) ID.

#### Example
```c++
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
    // Additional code...
  } else {
    Serial.println("Connection failed");
  }
}
```
</br>

### `iBusRequestArrivalTime`
Sends a GET request to the API to get the arrival time of a specific bus at a particular station.

#### Parameters
`routeId (String)` : The route ID of the bus. </br>
`bstopId (String)` : The station (bus stop) ID.

#### Example
```c++
void iBusRequestArrivalTime(String routeId, String bstopId) {
  String str = "GET /6280000/busArrivalService/getBusArrivalList?serviceKey=" + iServiceKey + "&pageNo=1&numOfRows=10&routeId=";
  str.concat(routeId);
  str.concat("&bstopId=");
  str.concat(bstopId);
  str.concat(" HTTP/1.1\r\nHost:apis.data.go.kr\r\nConnection: close\r\n\r\n");

  if (client.connect(iHost, httpPort)) {
    Serial.println("connected");
    Serial.print(str);
    client.print(str);
    client.println();
    // Additional code...
  } else {
    Serial.println("Connection Failed");
  }
}
```
</br>

### `gBusParseArrivalTime`
Parses the arrival time of a general bus from the received buffer string.

#### Parameters
`busNum (String)` : The bus number.

#### Example
```c++
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
      return busNum + " : Error Code ( " + errCode + " )";
    }
  }

  int strLength = strlen("<predictTimeX>");
  endIndex = rcvbuf.indexOf("<", startIndex + strLength);
  String predictTime1 = rcvbuf.substring(startIndex + strLength, endIndex);

  startIndex = rcvbuf.indexOf("<predictTime2>") + strlen("<predictTime2>");
  endIndex = rcvbuf.indexOf("<", startIndex);
  String predictTime2 = rcvbuf.substring(startIndex, endIndex);

  if (predictTime1 == "" && predictTime2 == "") {
    return busNum + " : No buses in service";
  }

  if (predictTime2 == "" || predictTime1.toInt() <= 7) {
    startIndex = rcvbuf.indexOf("<locationNo1>") + strlen("<locationNo1>");
    endIndex = rcvbuf.indexOf("<", startIndex);
    String predictStop1 = rcvbuf.substring(startIndex, endIndex);
    rcvbuf = "";
    return busNum + " : " + (predictTime1 == "1" ? "ARRIV." : predictTime1 + " mins") + (predictStop1 != "" ? "  ( " + predictStop1 + (predictStop1 == "1" ? " stop )" : " stops )") : "");
  } else {
    rcvbuf = "";
    return busNum + " : " + (predictTime1 == "1" ? "ARRIV." : predictTime1 + " mins") + (predictTime2 != "" ? "  ,  " + (predictTime2 == "1" ? "ARRIV." : predictTime2 + " mins") : "");
  }
}
```
</br>

### `iBusParseArrivalTime`
Parses the arrival time of a general bus from the received buffer string.

#### Parameters
`busNum (String)` : The bus number.

#### Example
```c++
String iBusParseArrivalTime(String busNum) {
  previousMillis = millis();
  int startIndex = rcvbuf.indexOf("<ARRIVALESTIMATETIME>");
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
      return busNum + " : Error Code ( " + errCode + " )";
    }
  }

  startIndex += strlen("<ARRIVALESTIMATETIME>");
  endIndex = rcvbuf.indexOf("<", startIndex);
  String predictTime1 = rcvbuf.substring(startIndex, endIndex);

  startIndex = rcvbuf.indexOf("<REST_STOP_COUNT>") + strlen("<REST_STOP_COUNT>");
  endIndex = rcvbuf.indexOf("<", startIndex);
  String predictStop1 = rcvbuf.substring(startIndex, endIndex);

  String predictTime2;
  int nextStartIndex = rcvbuf.indexOf("<ARRIVALESTIMATETIME>", startIndex);
  if (nextStartIndex != -1) {
    nextStartIndex += strlen("<ARRIVALESTIMATETIME>");
    predictTime2 = rcvbuf.substring(nextStartIndex, rcvbuf.indexOf("<", nextStartIndex));
  }
  rcvbuf = "";

  String strDisp;
  strDisp.concat(busNum + " : ");
  int timeStr = predictTime1.toInt() / 60;
  strDisp.concat(timeStr > 1 ? String(timeStr) + " mins" : "ARRIV.");
  if (predictTime2 != "") {
    strDisp.concat(String(predictTime2.toInt() / 60) + " mins");
  } else {
    if (predictStop1 != "") {
      strDisp.concat("  ( " + predictStop1 + (predictStop1 == "1" ? " stop )" : " stops )"));
    }
  }

  return strDisp;
}
```
</br>

### `FormatBusString`
Formats the bus number string to a consistent length for display purposes.

#### Parameters
`busStr (String)` : The bus number string to align center consistently.

#### Example
```c++
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
```
</br>

### `ExtractBusNum`
Extracts the bus number from the received buffer string.

#### Example
```c++
String ExtractBusNum() {
  int startIndex = rcvbuf.indexOf("<routeName>");
  if (startIndex == -1) {
    return "";
  } else {
    startIndex += strlen("<routeName>");
    int endIndex = rcvbuf.indexOf("<", startIndex);
    return rcvbuf.substring(startIndex, endIndex);
  }
}
```
</details>

## Specifications
### Scenarios
- Estimated arrival time calculated based on the real-time bus location data retrieved from the server once every 20 seconds. Displays refined information on the OLED Screen.

### Connections
- 1 x Micro-USB

### Wireless
- IEEE 802.11 b/g/n Wi-Fi Technology

## Apparatus (Equipment)
### Platform
- ESP8266 NodeMCU

### 0.96" SSD1306 I2C OLED Display 
* Soldering required
* D3 : Data, D4 : Clock

### KOKIRI A-PACK FIXIE 5 (KP-LS50) Portable Battery
* USB Port : Power
* Micro-USB : Charging Port
* Dimension : 62.3 mm (W) × 112.0 mm (D) × 13.0 mm (H)
* Weight : 120 g
* Input : DC-5V / 2A
* Output : DC-5V / 2.1A
* Capacity : 5000 mAh

## Schematics
![BusInfoSystem-Schematics-Dark](BusInfoSystem-Schematics-Dark.png#gh-dark-mode-only)
![BusInfoSystem-Schematics-Light](BusInfoSystem-Schematics-Light.png#gh-light-mode-only)
</br></br>
![BIS_SCHEMDiag-Dark](BIS_SCHEMDiag-Dark.png#gh-dark-mode-only)
![BIS_SCHEMDiag-Light](BIS_SCHEMDiag-Light.png#gh-light-mode-only)
</br></br>

## Data Usage
**1,126 bytes per API Call.** <br>
1,126 bytes (1.09 KB) x 3 = **3,378 bytes. (3 bus routes have been used in this project.)**
 
86,400 ÷ 20 = **4,320 times API calls in a day.** </br>
4,320 * 3,378 = **14,592,960 bytes**. </br>
Approximately uses **13.92 MB per day.** (**418 MB per month.**)

## Known Issues
* The Incheon City Bus Arrival Information API only gives details on one arriving bus at a time, so it's impossible to show the expected arrival times for multiple buses on the OLED screen. </br>
* Since the Incheon City Bus Arrival Information API doesn't provide bus route numbers in its responses, you'll need to hard-code the bus route numbers in the code. </br>
* In cases where the Wi-Fi or server connection might not be stable, there may be instances when the OLED screen displays the message `No buses in service.` even if a server-related error code is not shown.

## APIs Used
### Gyeonggi-Do (Gyeonggi province) bus route inquiry API.
https://www.data.go.kr/tcs/dss/selectApiDataDetailView.do?publicDataPk=15080666
- Provides the route ID, route type, and operating area of the route number.
- Provides a list of stops via which the corresponding line stops, the name of the stop, whether the center lane is located, the turnaround point, and coordinate values.

### Gyeonggi-Do (Gyeonggi province) bus arrival information inquiry API.
https://www.data.go.kr/tcs/dss/selectApiDataDetailView.do?publicDataPk=15080346
- Provides location information, estimated arrival time, vacant seats, and low-floor bus information of the first and second scheduled buses for a specific route stopping at the corresponding stop.

### Incheon metropolitan city bus route inquiry API.
https://www.data.go.kr/tcs/dss/selectApiDataDetailView.do?publicDataPk=15058487
- Provides the route ID, route type, and operating area of the route number.
- Provides a list of stops via which the corresponding line stops, the name of the stop, whether the center lane is located, the turnaround point, and coordinate values.

### Incheon metropolitan city bus arrival information inquiry API.
https://www.data.go.kr/tcs/dss/selectApiDataDetailView.do?publicDataPk=15059084
- Provides location information, estimated arrival time, vacant seats, and low-floor bus information of the first and second scheduled buses for a specific route stopping at the corresponding stop.


