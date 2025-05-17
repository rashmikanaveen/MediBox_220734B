#include <ESP32Servo.h>
#include <math.h>
#include "config.h"

Servo myservo;

void setup_servo() {
  myservo.attach(servo_pin);  // Attach servo to GPIO 27
}

void run_servo() {
  for (int pos = 0; pos <= 180; pos += 1) {
    myservo.write(pos);
    delay(15);
  }
  for (int pos = 180; pos >= 0; pos -= 1) {
    myservo.write(pos);
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
    //Serial.print("Angle: ");
    //Serial.println(int(angle));
    // Clamp angle between theta_offset and 180
    if (angle < theta_offset) angle = theta_offset;
    if (angle > 180.0) angle = 180.0;

    myservo.write((int)angle);
}