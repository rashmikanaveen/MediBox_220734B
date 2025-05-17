#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"





// mqtt 
void setupMQTT(){
  mqttClient.setServer("broker.hivemq.com", 1883);
  mqttClient.setCallback(receviveCallback);
  
}


void connectToBroker(){
  while(!mqttClient.connected()){
    Serial.println("Connecting to MQTT Broker...");
    if(mqttClient.connect("ESP32-rashmikanaveen")){
      Serial.println("Connected to MQTT Broker");
      mqttClient.subscribe("rashmikanaveen");
      mqttClient.subscribe("rashmikanaveen-ts");
      mqttClient.subscribe("rashmikanaveen-tu");
      mqttClient.subscribe("rashmikanaveen-θ_offset");
      mqttClient.subscribe("rashmikanaveen-γ");
      mqttClient.subscribe("rashmikanaveen-T_med");
      mqttClient.subscribe("rashmikanaveen-alarm-on-off");
    }else{
      Serial.print("Failed to connect, rc=");
      Serial.print(mqttClient.state());
      delay(500);
    }
  }
}

void receviveCallback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  char payloadCharAr[length + 1]; // Add space for null terminator
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    payloadCharAr[i] = (char)payload[i];
  }
  payloadCharAr[length] = '\0'; // Null-terminate the string
  Serial.println();
  
  if (strcmp(topic, "rashmikanaveen-ts") == 0) {
    int value = atoi(payloadCharAr);
    ts=value;
  }
  if (strcmp(topic, "rashmikanaveen-tu") == 0) {
    int value = atoi(payloadCharAr);
    tu=value;
  }


  if (strcmp(topic, "rashmikanaveen-θ_offset") == 0) {
    int value = atoi(payloadCharAr);
    theta_offset=value;
  }
  if (strcmp(topic, "rashmikanaveen-γ") == 0) {
    int value = atoi(payloadCharAr);
    gammaa=value;
  }
  if (strcmp(topic, "rashmikanaveen-T_med") == 0) {
    int value = atoi(payloadCharAr);
    Tmed=value;
  }
  if (strcmp(topic, "rashmikanaveen-T_med") == 0) {
    alarm_enabled=!alarm_enabled;
  }

  
}
