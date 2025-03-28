# Smart MediBox

This project, **Smart MediBox**, is part of the EN2853: Embedded Systems and Applications course. The Smart MediBox is designed to help users manage their medication schedules effectively by providing alarm notifications and temperature/humidity monitoring.

---

## Features

1. **Set Time Zone**: Configure the time zone dynamically based on the UTC offset.
2. **Set Alarms**: Set up to two alarms for medication reminders.
3. **View Active Alarms**: Display the currently active alarms on the OLED screen.
4. **Delete Alarms**: Remove specific alarms as needed.
5. **Alarm Notifications**: Ring a buzzer and blink an LED when an alarm is triggered.
6. **Snooze Functionality**: Snooze alarms for 5 minutes using a button.
7. **Temperature and Humidity Monitoring**: Display warnings if the temperature or humidity goes out of range.

---

## Platforms Used

### Wokwi
- The **Wokwi Simulator** was used to design and simulate the hardware components of the Smart MediBox.
- Wokwi provides a virtual environment to test the ESP32, OLED display, DHT22 sensor, buzzer, and push buttons without requiring physical hardware.
- [Visit Wokwi](https://wokwi.com/) to learn more.

### PlatformIO
- **PlatformIO** was used as the development environment for writing, building, and uploading the firmware to the ESP32.
- PlatformIO simplifies dependency management and provides advanced debugging tools.
- [Visit PlatformIO](https://platformio.org/) to learn more.

---

## Hardware Components

- **ESP32 DevKit v1**: Microcontroller for managing the system.
- **SSD1306 OLED Display**: Displays time, alarms, and notifications.
- **DHT22 Sensor**: Monitors temperature and humidity.
- **Buzzer**: Provides audible alarm notifications.
- **Push Buttons**: Used for menu navigation and alarm control.
- **LED**: Blinks during alarm notifications.

---

## Software Features

- **PlatformIO**: Used for development and deployment.
- **Libraries**:
  - Adafruit GFX Library
  - Adafruit SSD1306 Library
  - DHT Sensor Library for ESPx
- **NTP Synchronization**: Synchronizes time using an NTP server.

---

## Images

### MediBox Before Turning On
![MediBox Before Turning On](assets/MediBoxBeforeOn.png)

### MediBox in Action
![MediBox in Action - 1](assets/MediBox2.png)

![MediBox in Action - 2](assets/MediBox3.png)

---

## How It Works

1. **Set Time Zone**:
   - Use the menu to input the UTC offset for your location.
   - The system synchronizes time using an NTP server.

2. **Set Alarms**:
   - Navigate to the "Set Alarm" menu to configure up to two alarms.
   - Use the buttons to adjust the hour and minute.

3. **Alarm Notifications**:
   - When an alarm is triggered, the buzzer rings, and the LED blinks.
   - Use the "Cancel" button to stop the alarm or the "OK" button to snooze it for 5 minutes.

4. **Temperature and Humidity Monitoring**:
   - The system continuously monitors the environment.
   - Displays warnings if the temperature exceeds 32°C or drops below 24°C, or if the humidity goes out of the 65%-80% range.

---

## Circuit Diagram

The circuit diagram for the Smart MediBox is available in the Wokwi simulator.

---

## How to Run

1. Clone this repository to your local machine.
2. Open the project in PlatformIO.
3. Connect the hardware components as per the circuit diagram.
4. Build and upload the code to the ESP32.
5. Use the buttons to navigate the menu and set alarms.

---

## Acknowledgments

This project was developed as part of the **EN2853: Embedded Systems and Applications** course. Special thanks to:
- **Wokwi Simulator** for providing a platform to design and test the project virtually.
- **PlatformIO** for simplifying the development and deployment process.

---

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.