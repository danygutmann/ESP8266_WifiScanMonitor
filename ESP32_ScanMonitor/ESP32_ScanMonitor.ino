#include "WiFi.h"


void setup() {

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Setup done");
}


String WifiScan() {
  String erg = "WifiScan:\n";
  int n = WiFi.scanNetworks();
  if (n == 0) {
    erg += "no networks found\n";
  } else {
    erg += String(n);
    erg += " networks found:\n";

    Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.printf("%2d", i + 1);
      Serial.print(" | ");
      Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
      Serial.print(" | ");
      Serial.printf("%4ld", WiFi.RSSI(i));
      Serial.print(" | ");
      Serial.printf("%2ld", WiFi.channel(i));
      Serial.print(" | ");
      switch (WiFi.encryptionType(i)) {
        case WIFI_AUTH_OPEN:            Serial.print("open"); break;
        case WIFI_AUTH_WEP:             Serial.print("WEP"); break;
        case WIFI_AUTH_WPA_PSK:         Serial.print("WPA"); break;
        case WIFI_AUTH_WPA2_PSK:        Serial.print("WPA2"); break;
        case WIFI_AUTH_WPA_WPA2_PSK:    Serial.print("WPA+WPA2"); break;
        case WIFI_AUTH_WPA2_ENTERPRISE: Serial.print("WPA2-EAP"); break;
        case WIFI_AUTH_WPA3_PSK:        Serial.print("WPA3"); break;
        case WIFI_AUTH_WPA2_WPA3_PSK:   Serial.print("WPA2+WPA3"); break;
        case WIFI_AUTH_WAPI_PSK:        Serial.print("WAPI"); break;
        default:                        Serial.print("unknown");
      }
      Serial.println();
      delay(10);
    }
  }
  WiFi.scanDelete();
  return erg;
}
String WifiConnect(String ssid, String pass){

  WiFi.begin(ssid.c_str(), pass.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Connected to ");

  return "connected";
}
String WifiInfo() {
  return "";
}



String AsScan() {
  String erg = "";
  int n = WiFi.scanNetworks();
  if (n == 0) {
    erg += "no networks found\n";
    return erg;
  } else {
    for (int i = 0; i < n; ++i) {
      erg += String(WiFi.SSID(i).c_str());
      erg += "\n";
      delay(10);
    }
  }
  WiFi.scanDelete();
  return erg;
  }

String AsConnect(String ssid){
  // clean ssid
  ssid = ssid.substring(10);
  Serial.println("ssid: " + ssid);
  String erg = "";

  // find out password
  String pass = "";
  if (ssid.indexOf("Airscent") > -1) pass = "airscent";
  if (ssid.indexOf("Begeuren") > -1) pass = "begeuren";
  Serial.println("pass: " + pass);

  return WifiConnect(ssid, pass);
}


String GetType() {
  String erg = "GetType:";
  return erg;}



void loop() {
  if (Serial.available() > 0) {
    String str = Serial.readString();
    str.trim();    
    Serial.println(str);


    String erg = "";
    if (str == "WifiScan") erg = WifiScan();
    else if (str == "AsScan") erg = AsScan();
    else if (str.indexOf("AsConnect") > -1) erg = AsConnect(str);

    else erg = "error (" + str + ")";
    Serial.println(erg);
  }
}
