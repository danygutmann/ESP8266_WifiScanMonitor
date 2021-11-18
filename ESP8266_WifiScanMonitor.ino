#include "ESP8266WiFi.h"
#include <ESP8266HTTPClient.h>

HTTPClient sender;
WiFiClient wifiClient;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Setup done");
}

String Get(String url)
{
  if (sender.begin(wifiClient, url)) {

    Serial.println(url);
    
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
        Serial.println(payload);
      }
      
    }else{
      // Falls HTTP-Error
      Serial.printf("HTTP-Error: ", sender.errorToString(httpCode).c_str());
    }

    // Wenn alles abgeschlossen ist, wird die Verbindung wieder beendet
    sender.end();
  }
  
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
          Serial.println("FOUND " + WiFi.SSID(i));
          WiFi.begin(WiFi.SSID(i), "airscent"); 
          while (WiFi.status() != WL_CONNECTED) 
          {
            delay(1000);
            Serial.print('.');
          }
        }
        delay(10);
      }
    }
  
  }else{
    // get data 
    Get("http://192.168.4.1/CMD/?CMD=SendCmd&SUBCMD=I");
    
  }
  delay(1000);
}
