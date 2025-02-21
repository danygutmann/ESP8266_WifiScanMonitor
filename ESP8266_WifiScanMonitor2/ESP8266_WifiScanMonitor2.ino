#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "RTClib.h"

RTC_DS3231 rtc;

HTTPClient sender;
WiFiClient wifiClient;
String strErg = "";
boolean debug = false;
String last_SSID = "";
String rtc_date = "unknown";
String rtc_time = "unknown";
String rtc_timestamp = "unknown";
String rtc_SetTime = "unknown";

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char* ssid     = "K2-NET";
const char* password = "Dikt81mp!";

void setup() {
  Serial.begin(115200);
  while (!Serial) {
      delay(10);
  }

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("");
  Serial.println("hello");
  Serial.println("firmware from " + String(__DATE__) + " " + String(__TIME__));

  delay(10);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  Serial.println("wait for command");
}

// helpers
String PadLeft(String in, int l = 2){
  String pre = "";
  for (int i=in.length(); i < l; i++){
    pre += "0";
    }
  return pre + in;
}

// rtc 
void RtcGetTime() {
  DateTime now = rtc.now();
  rtc_time = PadLeft(String(now.hour(), DEC)) + ":" + PadLeft(String(now.minute(), DEC)) + ":" + PadLeft(String(now.second(), DEC));
  rtc_date = PadLeft(String(now.day(), DEC)) + ":" + PadLeft(String(now.month(), DEC)) + ":" + String(now.year(), DEC);
  rtc_timestamp = rtc_date + "|" + rtc_time;
  rtc_SetTime = RtcTimeString();
  }
void RtcPrintTimestamp() {
  Serial.println();
  RtcTimeString();
  Serial.println();
  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  }
String RtcTimeString() {

  DateTime now = rtc.now();
  String erg = "&DATE=";
  if (now.month() == 1) erg += "Jan";
  if (now.month() == 2) erg += "Feb";
  if (now.month() == 3) erg += "Mar";
  if (now.month() == 4) erg += "Apr";
  if (now.month() == 5) erg += "May";
  if (now.month() == 6) erg += "Jun";
  if (now.month() == 7) erg += "Jul";
  if (now.month() == 8) erg += "Ago";
  if (now.month() == 9) erg += "Sep";
  if (now.month() == 10) erg += "Oct";
  if (now.month() == 11) erg += "Nov";
  if (now.month() == 12) erg += "Dec";
  erg += "%20";

  erg += PadLeft(String(now.day())) + "%20";
  erg += String(now.year()) + "&TIME=";
  erg += PadLeft(String(now.hour()-1)) + ":";
  erg += PadLeft(String(now.minute())) + ":";
  erg += PadLeft(String(now.second()+1));

  //Serial.println(erg);
  return erg;
}

// core fuctions
String WifiScan() {
  ErgNew("WifiScan;");
  int scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);
  
  if (scanResult == 0) {
    ErgAdd("NoNetworksFound;");
  } else if (scanResult > 0) {
    String ssid;
    int32_t rssi;
    uint8_t encryptionType;
    uint8_t *bssid;
    int32_t channel;
    bool hidden;

    for (int8_t i = 0; i < scanResult; i++) {
      WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden);
      const bss_info *bssInfo = WiFi.getScanInfoByIndex(i);
      if (bssInfo) {
        ErgAdd(ssid+";");
      }
    }
  }
  ErgAdd("WifiScanDone;");
  return ErgGet(); 
  }
String WifiConnect(String ssid, String pass) {
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
  }
  return JsonErg("WifiConnect",ssid); 
  }
String WifiDisconnect() {
  return JsonErg("WifiDisconnect",String(WiFi.isConnected()));
 }
String WifiStatus(){
  return JsonErg("WifiStatus",String(WiFi.isConnected()));
  }
String WifiConnectFirst() {
  int scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);
  if (scanResult == 0) {
    return JsonErg("WifiConnectFirst","NoNetworkFound");
  } else if (scanResult > 0) {
    String ssid = "nothing found";
    int32_t rssi;
    uint8_t encryptionType;
    uint8_t *bssid;
    int32_t channel;
    bool hidden;
    for (int8_t i = 0; i < scanResult; i++) {
      WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden);
      const bss_info *bssInfo = WiFi.getScanInfoByIndex(i);
      if (bssInfo) {
          if (ssid.indexOf("Airscent") > -1){
            last_SSID = String(ssid);
            WifiConnect(ssid, "airscent");
            return JsonErg("WifiConnectFirst",String(ssid));
          }
          if (ssid.indexOf("Begeuren") > -1){
            last_SSID = String(ssid);
            WifiConnect(ssid, "begeuren");
            return JsonErg("WifiConnectFirst",String(ssid));
          }
        }
    }
  }
  return JsonErg("WifiConnectFirst","NoValidSsidFound");
  }

String Get(String url) {
  if (sender.begin(wifiClient, url)) {
    int httpCode = sender.GET();
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        String payload = sender.getString();
       return payload;
      }
    }else{
      Serial.printf("HTTP-Error: ", sender.errorToString(httpCode).c_str());
    }
    sender.end();
  }
  return "error";
}




String ConnectGetSerial() {
  WifiConnectFirst();
  return Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=GS");
}
void ErgNew(String message) {
  strErg = message;
  if (debug) Serial.println(message);
}
void ErgAdd(String message) {
  strErg = strErg + message;
  if (debug) Serial.println(message);
}
String ErgGet() {
  return strErg;
}
String JsonErg(String cmd, String res) {
  return "{\"command\": \"" + cmd + "\",\"result\": \"" + res + "\"}";
}
String JsonArr(String cmd, char* arr[]) {
  String erg = "{\"command\": \"+cmd+""\",\"result\": [";
  int items = 0;
  for (int i = 0; i < items ; i++) {
    erg += "\"" + String(arr[i]) + "\",";
    }
  erg += "]}";
  return erg;
}


// wrappers
String GetType(){
  return JsonErg("GetType",Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=GT"));
} 
String GetInfo(){
  RtcPrintTimestamp();
  return JsonErg("GetInfo",Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=I"));
}
String GetTime(){
  return JsonErg("GetTime",Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=I"));
}
String ResetAlias(){
  return JsonErg("ResetAlias",Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=SetAlias"));
}
String GetSerial(){
  return JsonErg("GetSerial",Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=GS"));
}
String DeleteAllLines(){
  return JsonErg("DeleteAllLines",Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=E"));
}
String ReadLine(int linenumber){
  int number = 10*linenumber;
  String erg = JsonErg("ReadLine",Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=R&LINES=1&DATA="+String(number)));
  return erg;
}
String WriteLine(int linenumber, String data){
  //Serial.print("Write Line "+String(linenumber)+": ");
  int lineraw = linenumber-1;
  int number = lineraw*10;
  String Address = "x";
  if (number > 100 ) Address = "0" + String(number); 
  if (number > 10 ) Address = "00" + String(number); 
  if (number > 1 ) Address = "000"; 
  String erg = JsonErg("WriteLine",Address+data);
  return erg;
}

// Procedures
String TestProduction() {
  RtcGetTime();
  Serial.println("PRODUCTION TEST");
  Serial.println("Date: " + rtc_date);
  Serial.println("Time: " + rtc_time);
  Serial.println("Pc: ESPHelper " + String(ESP.getChipId()));
  Serial.println("");
  Serial.println("FIND wifi device:");
  WifiConnectFirst();
  Serial.println("   " + last_SSID);

  Serial.println("GET Type:");
  String thetype = "   " + Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=GT");
  Serial.println(thetype);
  String basetype = "unknown";
  if(thetype.indexOf("Aromare") > 0) basetype = "Aromare";
  if(thetype.indexOf("Quad") > 0)    basetype = "Quad";
  if(thetype.indexOf("Venturi") > 0) basetype = "Venturi";
  Serial.println("   Basetype " + basetype);

  Serial.println("GET Serial:");
  String serial = Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=GS");
  Serial.println("   " + serial);
  Serial.println("SERIAL=" + serial);

  RtcGetTime();
  Serial.println("SET new DateTime:");
  Serial.println("   " + Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=DT" + rtc_SetTime));

  Serial.println("CHECK Time:");
  RtcGetTime();
  String info = Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=I");
  Serial.println("   " + rtc_date + " " + rtc_time + " (PC)");
  Serial.println("   " + info + " (device)");
  if(info.indexOf(".20|") > 0) Serial.println("    Error! wrong Date/Time (.20|)");
  if(info.indexOf(".2000") > 0) Serial.println("    Error! wrong Date/Time (.2000)");

  Serial.println("DELETE all lines:");
  Serial.println("   " + Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=E"));

  Serial.println("SET default lines for " + basetype + ":");
  if (basetype == "Quad") {
    Serial.println("   " + Get("http://192.168.4.1/CMD/?CMD=SendData&SUBCMD=P&LINES=12&DATA=000254000001023059001001100100"));
    Serial.println("   " + Get("http://192.168.4.1/CMD/?CMD=SendData&SUBCMD=P&LINES=12&DATA=000254000001023059002001100090"));
    Serial.println("   " + Get("http://192.168.4.1/CMD/?CMD=SendData&SUBCMD=P&LINES=12&DATA=000254000001023059003001100080"));
    Serial.println("   " + Get("http://192.168.4.1/CMD/?CMD=SendData&SUBCMD=P&LINES=12&DATA=000254000001023059004001100070"));
  }
  if (basetype == "Aromare") {
    Serial.println("   " + Get("http://192.168.4.1/CMD/?CMD=SendData&SUBCMD=P&LINES=12&DATA=000254000001023059001001100100"));
  }
  if (basetype == "Venturi") {
    Serial.println("   " + Get("http://192.168.4.1/CMD/?CMD=SendData&SUBCMD=P&LINES=12&DATA=000254000001023059200001001100"));
  }

  Serial.println("READ out lines:");
  for (int i = 0; i < 20; i++) 
  {
    int number = 10*i;
    String tmp = Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=R&LINES=1&DATA="+String(number));
    if (tmp.startsWith("254")){
      Serial.println("   " + tmp);
    }else{
      break;
    }
     
  }

  Serial.println("GET Info:");
  Serial.println("   " + Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=I"));
  
  return "EOF";
}

// loop
String ExecuteCmd(String str, String source) {

  // WIFI
  if (str == "WifiScan") return  WifiScan();
  if (str == "WifiConnectFirst") return  WifiConnectFirst();
  if (str == "WifiDisconnect") return WifiDisconnect();
  if (str == "ConnectGetSerial") return ConnectGetSerial();

  // DEVICE
  if (str == "GetInfo") return GetInfo();
  if (str == "GetTime") return GetTime();
  if (str == "GetType") return GetType();
  if (str == "GetSerial") return GetSerial();
  if (str == "ResetAlias") return ResetAlias();

  // Tests
  if (str == "TestProduction") return TestProduction();


  //  Set RTC
  if (str.indexOf("SetRtc") > -1) {
    String newtime = str.substring(6);
    Serial.println("set RTC to " + newtime);
    rtc.adjust(DateTime(newtime.toInt()+7200)); // +7200 Timezone
    RtcGetTime();
    Serial.println("new Time: " + rtc_timestamp);
   }

  // send as Relay
  if (str.indexOf("Send") > -1) {
    Serial.print("");
    Serial.print("");
    String url = "http://192.168.4.1/" + str.substring(4);
    //Serial.println("Send: " + url);
    String answer = Get(url);
    Serial.println("Answer: " + answer);
    }

  return "Error;"+ str;
}
void loop() {
  // serial
  if (Serial.available() > 0) {
    String str = Serial.readString();
    str.trim();   
    Serial.println(ExecuteCmd(str, "serial;"));
  }
}