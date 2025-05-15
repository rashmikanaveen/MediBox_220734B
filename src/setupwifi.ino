#include<WiFiManager.h>
#include<WiFi.h>
#include "config.h"


void setupWiFiWokwi(){
  WiFi.begin("Wokwi-GUEST", "");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wokwi...");
    print_line("Connecting to Wokwi...", 0, 0, 2);
  }
  Serial.println("Connected to Wokwi");
  Serial.println("IP Address: ");
  
}

void setupWiFi(){
  WiFiManager wifiManager;
  bool res=wifiManager.autoConnect("My Esp32","12345678");
  if(!res){
    Serial.println("Failed to connect");
    ESP.restart();
  }else{
    Serial.println("Connected to WiFi");
    print_line("Connected to WiFi", 0, 0, 2);

  }
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Connecting to WiFi...");
  print_line("Connecting to WiFi", 0, 0, 2);
  
}

