#include <ESP32Servo.h>
#include <math.h>
#include "config.h"

Servo myservo;

 

void setup_servo(){
  ledcSetup(SERVO_CHANNEL, 50, 16);    // 50Hz PWM, 16-bit resolution
  ledcAttachPin(SERVO_PIN, SERVO_CHANNEL);
}

void run_servo() {
  for (int pos = 0; pos <= 180; pos++) {
    int us = map(pos, 0, 180, 500, 2500);
    uint32_t duty = (us * 65535) / 20000;  // 20ms period (50Hz)
    ledcWrite(SERVO_CHANNEL, duty);
    delay(15);
  }
  for (int pos = 180; pos >= 0; pos--) {
    int us = map(pos, 0, 180, 500, 2500);
    uint32_t duty = (us * 65535) / 20000;
    ledcWrite(SERVO_CHANNEL, duty);
    delay(15);
  }
}


void adjust_servo(float intensity, float temperature) {
  //Serial.print("Intensity: ");
    //Serial.print(intensity);
    //Serial.print(" Temperature: ");
    //Serial.println(temperature);
    //Serial.print("ts: ");
    //Serial.print(ts);
    //Serial.print(" tu: ");
    //Serial.print(tu);
    //Serial.print(" θ_offset: ");
    //Serial.print(theta_offset);
    //Serial.print(" γ: ");
    //Serial.print(gammaa);
    //Serial.print(" Tmed: ");
    //Serial.print(Tmed);
    // θ = θoffset + (180 − θoffset) × I × γ × ln(ts/tu) × T/Tmed
    float ln_ratio = log(ts / tu); // natural log
    float angle = theta_offset + (180.0 - theta_offset) * intensity * gammaa * ln_ratio * (temperature / Tmed);

    // Clamp angle between theta_offset and 180
    if (angle < theta_offset) angle = theta_offset;
    if (angle > 180.0) angle = 180.0;

    int us = map((int)angle, 0, 180, 500, 2500);
    uint32_t duty = (us * 65535) / 20000;  // 20ms period (50Hz)
    ledcWrite(SERVO_CHANNEL, duty);
}