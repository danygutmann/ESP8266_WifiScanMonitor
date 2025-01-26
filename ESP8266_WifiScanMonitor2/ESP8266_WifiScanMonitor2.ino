#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "RTClib.h"


RTC_DS3231 rtc;

HTTPClient sender;
WiFiClient wifiClient;
String strErg = "";
boolean debug = false;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


const char* ssid     = "K2-NET";
const char* password = "Dikt81mp!";

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("");
  Serial.println("hello");
  Serial.println("firmware from " + String(__DATE__) + " " + String(__TIME__));

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  //WiFi.begin(ssid, password);
  //while (WiFi.status() != WL_CONNECTED) {
    //delay(200);
    //Serial.print(".");
  //}
  //Serial.println("Verbunden!");
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
  String erg = "-> ";
  if (now.month() == 1) erg += "Jan ";
  if (now.month() == 2) erg += "Feb ";
  if (now.month() == 3) erg += "Mar ";
  if (now.month() == 4) erg += "Apr ";
  if (now.month() == 5) erg += "May ";
  if (now.month() == 6) erg += "Jun ";
  if (now.month() == 7) erg += "Jul ";
  if (now.month() == 8) erg += "Ago ";
  if (now.month() == 9) erg += "Sep ";
  if (now.month() == 10) erg += "Oct ";
  if (now.month() == 11) erg += "Nov ";
  if (now.month() == 12) erg += "Dec ";

  erg += String(now.day()) + " ";
  erg += String(now.year()) + ";";
  erg += String(now.hour()) + ":";
  erg += String(now.minute()) + ":";
  erg += String(now.second()) + ":";

  Serial.println(erg);
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
String WifiStatus() {
  return JsonErg("WifiStatus",String(WiFi.isConnected()));
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

void SessionNew() {

}
String SessionGet() {
  return "";
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
            WifiConnect(ssid, "airscent");
            return JsonErg("WifiConnectFirst",String(ssid));
          }
          if (ssid.indexOf("Begeuren") > -1){
            WifiConnect(ssid, "begeuren");
            return JsonErg("WifiConnectFirst",String(ssid));
          }
        }
    }
  }
  return JsonErg("WifiConnectFirst","NoValidSsidFound");
}
String GetType(){
  return JsonErg("GetType",Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=GT"));
} 
String GetInfo(){
  RtcPrintTimestamp();
  return JsonErg("GetInfo",Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=I"));
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


String ExecuteCmd(String str, String source) {

  // WIFI
  if (str == "WifiScan") return  WifiScan();
  if (str == "WifiConnectFirst") return  WifiConnectFirst();
  if (str == "WifiDisconnect") return WifiDisconnect();

  // DEVICE
  if (str == "GetInfo") return GetInfo();
  if (str == "GetType") return GetType();
  if (str == "GetSerial") return GetSerial();

  if (str == "c") {
    Serial.println("cmd c");
    Serial.println(WifiConnectFirst());
    Serial.println(GetSerial());
    Serial.println(GetType());
    Serial.println(GetInfo());
    //DeleteAllLines();
    //Serial.println(WriteLine(1,"001002003004005006007008009010"));
    //Serial.println(WriteLine(2,"011012013014015016017018019020"));
    //Serial.println(ReadLine(1));
    //Serial.println(ReadLine(2));
    
    return "cmd c done";
    }


  if (str.indexOf("SetTime") > -1) {
    String time = str.substring(7);
    Serial.println("set time to " + time);
   }
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

  // wifi


  // local ports




}