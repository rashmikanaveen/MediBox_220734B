
#include "config.h"





void check_temp()
{
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  if (data.temperature > 32)
  {

    print_line("Temperature is high!", 0, 40, 1);
    digitalWrite(LED_TEMP, HIGH);
  }
  else if (data.temperature < 24)
  {

    print_line("Temperature is low!", 0, 40, 1);
    digitalWrite(LED_TEMP, HIGH);
  }
  if (data.humidity > 80)
  {

    print_line("Humidity is high!", 0, 50, 1);
    digitalWrite(LED_HUMIDITY, HIGH);
  }
  else if (data.humidity < 65)
  {

    print_line("Humidity is low!", 0, 50, 1);
    digitalWrite(LED_HUMIDITY, HIGH);
  }
  else
  {
    digitalWrite(LED_TEMP, LOW);
    digitalWrite(LED_HUMIDITY, LOW);
  }
}