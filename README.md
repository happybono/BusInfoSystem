# BusInfoSystem
Bus Information System for Gyeonggi Province, South Korea. NodeMCU (ESP8266) has been used as main microcontroller unit, estimated arrival time calculated based on the real time bus location data retrieves from the server once 20 minutes. Displays refined information on the OLED Screen. 

## Schematics
![GBusInfoSystem-Schematics](GBusInfoSystem-Schematics.png)

https://www.data.go.kr/tcs/dss/selectApiDataDetailView.do?publicDataPk=15058051
https://www.data.go.kr/tcs/dss/selectApiDataDetailView.do?publicDataPk=15058837

## What's New
<details>
<summary>Click to Expand</summary>

### v1.0
#### August 25, 2020  
Initial release.

#### August 26, 2020  
Added Open API document which provided by the Government of South Korea. </br>
Modified variables to consistent with bus route numbers.
</details>

## Specifications
### Connections
- 1 x Micro-USB

### Wireless
- IEEE 802.11 b/g/n Wi-Fi Technology

## Apparatus (Equipment)
### Platform
- ESP8266 NodeMCU

### 0.96" SSD1306 I2C OLED Display
- Soldering Required

### KOKIRI A-PACK FIXIE 5 (KP-LS50) Portable Battery
* USB Port : Power
* Micro-USB : Charging Port
* Dimension : 62.3 mm (W) × 112.0 mm (D) × 13.0 mm (H)
* Weight : 120 g
* Input : DC-5V / 2A
* Output : DC-5V / 2.1A
* Capacity : 5000 mAh

## Known Issues
Takes long time to load after the first boot. 
