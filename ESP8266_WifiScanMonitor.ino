#include "ESP8266WiFi.h"
#include <ESP8266HTTPClient.h>

HTTPClient sender;
WiFiClient wifiClient;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Start Scan");
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
  Serial.println(Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=I"));
}
void GetInfo(String ssid){
  Serial.println();
  Serial.print(Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=GT"));
  Serial.print(" ");
  Serial.println(ssid);
} 
void Connect(String ssid, String pass){
  Serial.print("connect to " + ssid);
  WiFi.begin(ssid, pass); 
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print('.');
  }
  Serial.println();
  GetInfo(ssid);
}

void loop() 
{
  if (WiFi.status() != WL_CONNECTED)
  {
    // no wifi connection
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    int n = WiFi.scanNetworks();
    if (n != 0) 
    {
      for (int i = 0; i < n; ++i) 
      {      
        if(WiFi.SSID(i).indexOf("Airscent") >= 0)
        {
          // connect
          Connect(String(WiFi.SSID(i)), "airscent");
//          Serial.println("FOUND " + WiFi.SSID(i));
//          WiFi.begin(WiFi.SSID(i), "airscent"); 
//          while (WiFi.status() != WL_CONNECTED) 
//          {
//            delay(1000);
//            Serial.print('.');
//          }
//          GetInfo(WiFi.SSID(i));
        }
        if(WiFi.SSID(i).indexOf("Begeuren") >= 0)
        {
          // connect
          Connect(String(WiFi.SSID(i)), "begeuren");
          
//          Serial.println("FOUND " + WiFi.SSID(i));
//          WiFi.begin(WiFi.SSID(i), "begeuren"); 
//          while (WiFi.status() != WL_CONNECTED) 
//          {
//            delay(1000);
//            Serial.print('.');
//          }
//          GetInfo(WiFi.SSID(i));
        }
        
        
        delay(10);
      }
    }
  
  }else{
    GetTime();
    
  }
  delay(1000);
}
