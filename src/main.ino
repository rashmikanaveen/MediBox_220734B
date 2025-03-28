#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>
#include <WiFi.h>

// Define the OLED display
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define BUZZER 5
#define LED_1 15

#define PB_CANCEL 34
#define PB_OK 32
#define PB_UP 33
#define PB_DOWN 35

#define DHTPIN 12

#define NTP_SERVER "pool.ntp.org"
#define UTC_OFFSET 19800 // Sri Lanka is UTC+5:30, which is 19800 seconds
#define UTC_OFFSET_DST 0

// declare objects
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHTesp dhtSensor;

// Global variables
int days = 0;
int hours = 0;
int minutes = 0;
int seconds = 0;

unsigned long timeNow = 0;
unsigned long timeLast = 0;

bool alarm_enabled = true;
int n_alarm = 2;
int alarm_hours[2] = {0, 23};
int alarm_minutes[2] = {23, 59};
bool alarm_triggered[2] = {false, false};

int n_notes = 8;
int C = 262;
int D = 294;
int E = 330;
int F = 349;
int G = 392;
int A = 440;
int B = 494;
int C_H = 523;

int notes[8] = {C, D, E, F, G, A, B, C_H};

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
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    display.clearDisplay();
    print_line("Connecting to WiFi", 0, 0, 2);
  }

  display.clearDisplay();
  print_line("Connected to WiFi", 0, 0, 2);

  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);

  display.clearDisplay();

  print_line("Welcome to Medibox!", 10, 20, 2);
  // delay(2000);
  display.clearDisplay();
}

void loop()
{
  // Serial.println(wait_for_button_press());

  update_time_with_check_alarm();
  if (digitalRead(PB_OK) == LOW)
  {
    delay(200);
    go_to_menu();
  }
  check_temp();
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

void print_time_now()
{
  display.clearDisplay();
  print_line(String(days), 0, 0, 2);
  print_line(":", 20, 0, 2);
  print_line(String(hours), 30, 0, 2);
  print_line(":", 50, 0, 2);
  print_line(String(minutes), 60, 0, 2);
  print_line(":", 80, 0, 2);
  print_line(String(seconds), 90, 0, 2);
}

void update_time()
{
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  days = timeinfo.tm_mday;
  hours = timeinfo.tm_hour;
  minutes = timeinfo.tm_min;
  seconds = timeinfo.tm_sec;
}

void ring_alarm(int alarm_index)
{
  display.clearDisplay();
  print_line("Medicine time!", 0, 0, 2);

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
}

void update_time_with_check_alarm()
{
  update_time();
  print_time_now();

  if (alarm_enabled == true)
  {
    for (int i = 0; i < n_alarm; i++)
    {
      if (alarm_triggered[i] == false && alarm_hours[i] == hours && alarm_minutes[i] == minutes)
      {
        ring_alarm(i); // Pass the alarm index
        alarm_triggered[i] = true;
      }
    }
  }
}

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

void check_temp()
{
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  if (data.temperature > 32)
  {

    print_line("Temperature is high!", 0, 40, 1);
  }
  else if (data.temperature < 24)
  {

    print_line("Temperature is low!", 0, 40, 1);
  }
  if (data.humidity > 80)
  {

    print_line("Humidity is high!", 0, 50, 1);
  }
  else if (data.humidity < 65)
  {

    print_line("Humidity is low!", 0, 50, 1);
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