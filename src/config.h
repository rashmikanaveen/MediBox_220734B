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
char* SendTEmpAndHumidityData();
float retunLDRIntensity() ;
void setup_servo();
void run_servo();
void adjust_servo(float intensity, float temperature);
float getTemperature();
float getHumidity();

// Constants and macros
#define LED_HUMIDITY 25
#define LED_TEMP 26
#define BUZZER 2
#define LDR_AO 36
#define servo_pin 14

int ts=5;
// Global variable declarations (if needed)
extern int globalVariable;
bool alarm_enabled = true;

float theta_offset = 30.0;
float gammaa = 0.75;
float Tmed = 30.0;
int tu = 5; // default

#endif // CONFIG_H