#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>
#include <WiFi.h>

#include<WiFiUdp.h>
#include<NTPClient.h>
#include<PubSubClient.h>
#include "config.h"


// Define the OLED display
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


#define LED_1 15

#define PB_CANCEL 34
#define PB_OK 32
#define PB_UP 33
#define PB_DOWN 35

#define DHTPIN 12






// declare objects
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", 19800, 60000);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHTesp dhtSensor;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Global variables
int days;
int hours ;
int minutes;
int seconds ;

unsigned long timeNow = 0;
unsigned long timeLast = 0;
unsigned long lastSample = 0;
unsigned long lastTempHumPublish = 0; 






int current_mode = 0;
int max_modes = 6;

String modes[6] = {"1-Set Time",
                   "2-Set Alarm1",
                   "3-Set Alarm2",
                   alarm_enabled ? "4-Disable Alarm" : "4-Enable Alarm",
                   "5-View Alarms",
                   "6-Delete Alarm"};







void setup()

{

  pinMode(BUZZER, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(PB_CANCEL, INPUT);
  pinMode(PB_OK, INPUT);
  pinMode(PB_UP, INPUT);
  pinMode(PB_DOWN, INPUT);
  pinMode(LED_HUMIDITY, OUTPUT);
  pinMode(LED_TEMP, OUTPUT);

  //ledcSetup(0, 2000, 8);
  //ledcAttachPin(BUZZER, 0);

  dhtSensor.setup(DHTPIN, DHTesp::DHT22);

  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.display();
  // delay(2000);

  setupWiFiWokwi();
  setupMQTT();
  setup_servo();


  display.clearDisplay();
  print_line("Connected to WiFi", 0, 0, 2);
  timeClient.begin();

  

  display.clearDisplay();

  print_line("Welcome to Medibox!", 10, 20, 2);
  // delay(2000);
  display.clearDisplay();
  //run_servo();

}


void loop()
{
  
  // Serial.println(wait_for_button_press());

  if(!mqttClient.connected()){
    connectToBroker();
  }
  mqttClient.loop();

  
  // Publish the JSON string to the MQTT topic
  //mqttClient.publish("rashmikanaveen-temp-humidity", SendTEmpAndHumidityData());

  
  
  // Publish the LDR value to the MQTT topic
  unsigned long now = millis();
  float intensity=retunLDRIntensity();

  if ((now - lastSample) >= ts * 1000) {
    lastSample = now;

    mqttClient.publish("rashmikanaveen-ldr-value", String(intensity).c_str());//ldr to dashboard
  }
  
  if (now - lastTempHumPublish >= 5000) {
    lastTempHumPublish = now;
    mqttClient.publish("rashmikanaveen-temp-humidity", SendTEmpAndHumidityData());
  }

  

  update_time_with_check_alarm();
  if (digitalRead(PB_OK) == LOW)
  {
    delay(200);
    go_to_menu();
  }
  check_temp();
  adjust_servo( intensity, getTemperature());
  
  
}


void start_ring_alarm_in_task(int alarm_index) {
  int *param = new int(alarm_index);
  xTaskCreate(
    [](void *param) {
      int idx = *((int*)param);
      delete (int*)param;
      ring_alarm(idx);
      vTaskDelete(NULL);
    },
    "RingAlarmTask",
    4096,
    param,
    1,
    NULL
  );
}

// Then update your update_time_with_check_alarm() like this:
void update_time_with_check_alarm()
{
  update_time();

  if (alarm_enabled == true)
  {
    for (int i = 0; i < n_alarm; i++)
    {
      if (alarm_triggered[i] == false && alarm_hours[i] == hours && alarm_minutes[i] == minutes)
      {
        start_ring_alarm_in_task(i); // <-- Use the task starter here!
        alarm_triggered[i] = true;
      }
      else{
        alarm_on = false;
      }
    }
  }
}

void update_time()
{
  timeClient.update();
  //Serial.print("Current time: ");
  //Serial.println(timeClient.getFormattedTime());
  display.clearDisplay();
  hours=timeClient.getHours();
  minutes=timeClient.getMinutes();
  seconds=timeClient.getSeconds();
  

  print_line(timeClient.getFormattedTime(), 0, 0, 2);
}





void print_line(String text, int column, int row, int text_size)
{
  // display.clearDisplay();
  display.setTextSize(text_size);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(column, row);
  display.println(text);
  display.display();
}



void ring_alarm(int alarm_index)
{
  display.clearDisplay();
  print_line("Medicine time!", 0, 0, 2);
  mqttClient.publish("rashmikanaveen-alarm-state", "on");
  Serial.println("Alarm triggered!");

  digitalWrite(LED_1, HIGH);

  bool alarm_stopped = false;

  // Ring the buzzer and blink the LED
  while (!alarm_stopped)
  {
    
    for (int i = 0; i < n_notes; i++)
    {
      if (digitalRead(PB_CANCEL) == LOW)
      { // Stop the alarm
        delay(200);
        alarm_stopped = true;
        break;
      }
      else if (digitalRead(PB_OK) == LOW)
      { // Snooze the alarm
        delay(200);
        alarm_hours[alarm_index] = hours;
        alarm_minutes[alarm_index] = (minutes + 5) % 60; // Add 5 minutes for snooze
        if (minutes + 5 >= 60)
        {
          alarm_hours[alarm_index] = (hours + 1) % 24;
        }
        alarm_stopped = true;
        display.clearDisplay();
        print_line("Snoozed for 5 min!", 0, 0, 2);
        delay(1000);
        break;
      }
      //play_buzzer();
    }
  }

  digitalWrite(LED_1, LOW);
  noTone(BUZZER);
  display.clearDisplay();
}



/*

void ring_alarm(int alarm_index)
{
  alarm_on = true;
  Serial.println(alarm_on);
  display.clearDisplay();
  print_line("Medicine time!", 0, 0, 2);
  Serial.println("Alarm triggered!");

  digitalWrite(LED_1, HIGH);

  bool alarm_stopped = false;

  // Ring the buzzer and blink the LED
  while (!alarm_stopped)
  {
    
    for (int i = 0; i < n_notes; i++)
    {
      mqttClient.publish("rashmikanaveen-alarm-state", "on");
      if (digitalRead(PB_CANCEL) == LOW)
      { // Stop the alarm
        delay(200);
        alarm_stopped = true;
        break;
      }
      else if (digitalRead(PB_OK) == LOW)
      { // Snooze the alarm
        delay(200);
        alarm_hours[alarm_index] = hours;
        alarm_minutes[alarm_index] = (minutes + 5) % 60; // Add 5 minutes for snooze
        if (minutes + 5 >= 60)
        {
          alarm_hours[alarm_index] = (hours + 1) % 24;
        }
        alarm_stopped = true;
        display.clearDisplay();
        print_line("Snoozed for 5 min!", 0, 0, 2);
        delay(1000);
        break;
      }
      
      tone(BUZZER, notes[i]);
      digitalWrite(LED_1, HIGH);
      delay(250);
      noTone(BUZZER);
      digitalWrite(LED_1, LOW);
      delay(250);
    }
  }

  digitalWrite(LED_1, LOW);
  display.clearDisplay();
  Serial.println("Alarm stopped!");
}
*/

int wait_for_button_press()
{
  while (true)
  {
    if (digitalRead(PB_OK) == LOW)
    {
      delay(200);
      return PB_OK;
    }
    else if (digitalRead(PB_UP) == LOW)
    {
      delay(200);
      return PB_UP;
    }
    else if (digitalRead(PB_DOWN) == LOW)
    {
      delay(200);
      return PB_DOWN;
    }
    else if (digitalRead(PB_CANCEL) == LOW)
    {
      delay(200);
      return PB_CANCEL;
    }
  }
  update_time();
}


void go_to_menu()
{
  while (digitalRead(PB_CANCEL) == HIGH)
  {
    display.clearDisplay();
    print_line(modes[current_mode], 0, 0, 2);
    int pressed = wait_for_button_press();
    if (pressed == PB_UP)
    {
      current_mode++;
      current_mode = current_mode % max_modes;
    }
    else if (pressed == PB_DOWN)
    {
      current_mode--;
      if (current_mode < 0)
      {
        current_mode = max_modes - 1;
      }
    }
    else if (pressed == PB_OK)
    {
      delay(200);
      // Serial.println(current_mode);
      run_mode(current_mode);
    }
    else if (pressed == PB_CANCEL)
    {
      delay(200);
      break;
    }
  }
}

void set_time()
{
  int temp_hour = hours;
  int temp_minute = minutes;
  // int temp_second = seconds;

  while (true)
  {
    display.clearDisplay();
    print_line("Enter hour :" + String(temp_hour), 0, 0, 2);
    int pressed = wait_for_button_press();
    if (pressed == PB_UP)
    {
      temp_hour++;
      temp_hour = temp_hour % 24;
    }
    else if (pressed == PB_DOWN)
    {
      temp_hour--;
      if (temp_hour < 0)
      {
        temp_hour = 23;
      }
    }
    else if (pressed == PB_OK)
    {
      delay(200);
      hours = temp_hour;
      break;
    }
    else if (pressed == PB_CANCEL)
    {
      delay(200);
      break;
    }
  }

  // change minutes
  while (true)
  {
    display.clearDisplay();
    print_line("Enter minute :" + String(temp_minute), 0, 0, 2);
    int pressed = wait_for_button_press();
    if (pressed == PB_UP)
    {
      temp_minute++;
      temp_minute = temp_minute % 60;
    }
    else if (pressed == PB_DOWN)
    {
      temp_minute--;
      if (temp_minute < 0)
      {
        temp_minute = 59;
      }
    }
    else if (pressed == PB_OK)
    {
      delay(200);
      minutes = temp_minute;
      break;
    }
    else if (pressed == PB_CANCEL)
    {
      delay(200);
      break;
    }
  }
  display.clearDisplay();
  print_line("Time set!", 0, 0, 2);
  delay(1000);
}






void set_alarm(int alarm)
{

  Serial.println("Alarm " + String(alarm));

  int temp_hour = alarm_hours[alarm];
  int temp_minute = alarm_minutes[alarm];

  while (true)
  {
    display.clearDisplay();
    print_line("Enter hour :" + String(temp_hour), 0, 0, 2);
    int pressed = wait_for_button_press();
    if (pressed == PB_UP)
    {
      temp_hour++;
      temp_hour = temp_hour % 24;
    }
    else if (pressed == PB_DOWN)
    {
      temp_hour--;
      if (temp_hour < 0)
      {
        temp_hour = 23;
      }
    }
    else if (pressed == PB_OK)
    {
      delay(200);
      alarm_hours[alarm] = temp_hour;
      break;
    }
    else if (pressed == PB_CANCEL)
    {
      delay(200);
      break;
    }
  }

  // change minutes
  while (true)
  {
    display.clearDisplay();
    print_line("Enter minute :" + String(temp_minute), 0, 0, 2);
    int pressed = wait_for_button_press();
    if (pressed == PB_UP)
    {
      temp_minute++;
      temp_minute = temp_minute % 60;
    }
    else if (pressed == PB_DOWN)
    {
      temp_minute--;
      if (temp_minute < 0)
      {
        temp_minute = 59;
      }
    }
    else if (pressed == PB_OK)
    {
      delay(200);
      alarm_minutes[alarm] = temp_minute;
      break;
    }
    else if (pressed == PB_CANCEL)
    {
      delay(200);
      break;
    }
  }
  display.clearDisplay();
  print_line("Alarm is set!", 0, 0, 2);
  delay(1000);
}





void run_mode(int mode)
{
  if (mode == 0)
  {
    set_time();
  }
  else if (mode == 1 || mode == 2)
  {
    set_alarm(mode - 1);
  }

  else if (mode == 3)
  {
    display.clearDisplay();
    alarm_enabled = !alarm_enabled;
    print_line(alarm_enabled ? "Alarm Enabled!" : "Alarm Disabled!", 0, 0, 2);
    delay(1000);
    
  }
  else if (mode == 4)
  {
    view_active_alarms();
  }
  else if (mode == 5)
  {
    delete_alarm();
  }
}






// my implementtaions
void view_active_alarms()
{
  display.clearDisplay();
  while (digitalRead(PB_CANCEL) == HIGH)
  {
    for (int i = 0; i < n_alarm; i++)
    {
      if (alarm_hours[i] == -1 && alarm_minutes[i] == -1)
      {
        String alarmText = "Alarm " + String(i + 1) + ":is not set";
        print_line(alarmText, 0, i * 10, 1); // Display each alarm on a new line
      }
      else
      {
        String alarmText = "Alarm " + String(i + 1) + ": " + String(alarm_hours[i]) + ":" + String(alarm_minutes[i]);
        print_line(alarmText, 0, i * 10, 1); // Display each alarm on a new line
      }
    }
  }

  display.clearDisplay();
}






void delete_alarm()
{
  int alarm_to_delete = 0;

  while (true)
  {
    display.clearDisplay();
    print_line("Delete Alarm: " + String(alarm_to_delete + 1), 0, 0, 2);
    int pressed = wait_for_button_press();
    if (pressed == PB_UP)
    {
      alarm_to_delete++;
      alarm_to_delete = alarm_to_delete % n_alarm;
    }
    else if (pressed == PB_DOWN)
    {
      alarm_to_delete--;
      if (alarm_to_delete < 0)
      {
        alarm_to_delete = n_alarm - 1;
      }
    }
    else if (pressed == PB_OK)
    {
      delay(200);
      alarm_hours[alarm_to_delete] = -1; // Mark the alarm as deleted
      alarm_minutes[alarm_to_delete] = -1;
      alarm_triggered[alarm_to_delete] = false;
      display.clearDisplay();
      print_line("Alarm Deleted!", 0, 0, 2);
      delay(1000);
      break;
    }
    else if (pressed == PB_CANCEL)
    {
      delay(200);
      break;
    }
  }
}
