#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

HTTPClient sender;
WiFiClient wifiClient;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

void WifiConnect(String ssid, String pass) {
  Serial.print("Connect to " + ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print('.');
  }

  Serial.println(" Connected.");
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
void GetType(){
  Serial.print("Get Type: ");
  Serial.println(Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=GT"));
} 
void GetInfo(){
  Serial.print("Get Info: ");
  Serial.println(Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=I"));
}
void DeleteAllLines(){
  Serial.print("Delete all lines: ");
  Serial.println(Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=E"));
}
void ReadLine(int linenumber){
  Serial.print("Read Line "+String(linenumber)+": ");
  int number = 10*linenumber;
  Serial.println(Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=R&LINES=1&DATA="+String(number)));
}
void WriteLine(int linenumber, String data){
  Serial.print("Write Line "+String(linenumber)+": ");
  int lineraw = linenumber-1;
  int number = 10*lineraw;
  String Address = "";
  if (number > 100 ) Address = "0" + String(number); 
  if (number > 10 ) Address = "000"; 
  Serial.println(Get("http://192.168.4.1/CMD/?CMD=SendData&SUBCMD=P&LINES=12&DATA="+Address+data));
}

void loop() {
  if (Serial.available() > 0) {
    String str = Serial.readString();
    str.trim();    
    //Serial.println("I received: " + str);

    if (str == "c") {
      Serial.print("WiFi scan. ");
      int scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);
      if (scanResult == 0) {
        Serial.println("No networks found");
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
            if (ssid.indexOf("Airscent") > -1){
              WifiConnect(ssid, "airscent");
              GetType();
              
              GetInfo();
              delay(1000); 
              GetInfo();
              delay(1000); 
              GetInfo();
              WriteLine(1,"001002003004005006007008009010");
              WriteLine(2,"011012013014015016017018019020");
              ReadLine(1);
              ReadLine(2);
              DeleteAllLines();
            }
          }
        }

      }
    }
    if (str == "GetInfo") GetInfo();
    if (str == "GetType") GetType();

    if (str.indexOf("SetTime") > -1) {
      String time = str.substring(7);
      Serial.println("set time to " + time);
    }
    if (str.indexOf("Send") > -1) {
      String url = "http://192.168.4.1/" + str.substring(4);
      //Serial.println("Send: " + url);
      String answer = Get(url);
      Serial.println("Answer: " + answer);
    }





  }




}