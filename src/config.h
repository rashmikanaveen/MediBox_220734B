#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>


// Use the global WiFi client from main.ino
extern WiFiClient espClient;
extern PubSubClient mqttClient;

// Use the global DHT sensor and display objects from main.ino
extern DHTesp dhtSensor;


// Function declarations
void setupWiFi();
void setupWiFiWokwi();
void print_line(String text, int column, int row, int text_size);
void setupMQTT();
void connectToBroker();
void receviveCallback(char* topic, byte* payload, unsigned int length);
void check_temp();

// Constants and macros
#define LED_HUMIDITY 25
#define LED_TEMP 26

// Global variable declarations (if needed)
extern int globalVariable;

#endif // CONFIG_H