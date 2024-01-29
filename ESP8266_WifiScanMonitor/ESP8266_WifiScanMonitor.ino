#include "ESP8266WiFi.h"
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

HTTPClient sender;
WiFiClient wifiClient;

#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);
String CurrInfo, CurrSsid, CurrTime, CurrBrand, CurrType;


void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  display.clearDisplay();
}

String Get(String url){
  if (sender.begin(wifiClient, url)) {

   // Serial.println(url);
    
    // HTTP-Code der Response speichern
    int httpCode = sender.GET();

    if (httpCode > 0) {
      
      // Anfrage wurde gesendet und Server hat geantwortet
      // Info: Der HTTP-Code für 'OK' ist 200
      if (httpCode == HTTP_CODE_OK) {

        // Hier wurden die Daten vom Server empfangen

        // String vom Webseiteninhalt speichern
        String payload = sender.getString();

        // Hier kann mit dem Wert weitergearbeitet werden
       // ist aber nicht unbedingt notwendig
       return payload;

      }
      
    }else{
      // Falls HTTP-Error
      Serial.printf("HTTP-Error: ", sender.errorToString(httpCode).c_str());
    }

    // Wenn alles abgeschlossen ist, wird die Verbindung wieder beendet
    sender.end();
  }
  
}
void GetTime(){

  CurrTime = Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=I");
  Serial.println(CurrTime);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(CurrBrand);
  display.println(CurrType);
  String empty = "";
  CurrSsid.replace(CurrBrand, empty);
  display.println(CurrSsid);
  display.println();
  display.setTextSize(1.5);
  display.println(CurrTime.substring(9,17));
  display.display();
}
void GetType(String ssid){
  CurrType = Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=GT");
  Serial.println();
  Serial.print("Type: " + CurrType);
} 

void WifiScan(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Scan");
  display.display();
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  int n = WiFi.scanNetworks();
  if (n != 0) 
  {
    for (int i = 0; i < n; ++i) 
    {      
      if(WiFi.SSID(i).indexOf("Airscent") >= 0)
      {
        CurrBrand = "Airscent";
        WifiConnect(String(WiFi.SSID(i)), "airscent");
      }
      if(WiFi.SSID(i).indexOf("Begeuren") >= 0)
      {
        CurrBrand = "Begeuren";
        WifiConnect(String(WiFi.SSID(i)), "begeuren");          
      }
      if(WiFi.SSID(i).indexOf("Amatrius") >= 0)
      {
        CurrBrand = "Amatrius";
        WifiConnect(String(WiFi.SSID(i)), "amatrius");          
      }
      delay(10);
    }
  }
}
void WifiConnect(String ssid, String pass){
  CurrSsid = ssid;
  Serial.print("connect to " + CurrSsid);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Found");
  display.display();
  
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print('.');
  }
  Serial.println();
  GetType(CurrSsid);
}


void DisplayTest(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Hello, world!");
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.println(3.141592);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("0x"); display.println(0xDEADBEEF, HEX);
  display.display();
  delay(2000);
}

void loop() 
{ 
  if (WiFi.status() != WL_CONNECTED)
  {
    WifiScan(); 
  }else{
    GetTime();
  }
  delay(1000);
}
