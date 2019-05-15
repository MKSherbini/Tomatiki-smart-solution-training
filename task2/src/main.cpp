#include <Arduino.h>
#include <string>
#include <EEPROM.h>

#define NO_OF_SENSORS 1 //only 1 here :V
typedef struct data
{
  u16 sensor_readings[NO_OF_SENSORS];
} Data_t;

#define NO_OF_READINGS (100)
#define READING_SIZE (sizeof(Data_t))
#define NO_OF_READINGS_XS (NO_OF_READINGS * READING_SIZE)
#define EEPROM_CACHE_SIZE (NO_OF_READINGS_XS)
#define READING_TIME_INTERVAL_MS (100)
#define READING_COMMIT_INTERVAL_MS (READING_TIME_INTERVAL_MS * NO_OF_READINGS)

Data_t reading;
int reading_idx;
unsigned long commit_timer = 0;
unsigned long read_timer = 0;
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  EEPROM.begin(EEPROM_CACHE_SIZE);
  Serial.println();

  // Serial.println("writing");
  // for (size_t i = 0; i < NO_OF_READINGS; i++)
  // {
  //   reading.sensor_readings[0] = 0xffff;
  //   EEPROM.put(reading_idx++, reading);
  //   EEPROM.commit();
  //   Serial.println(String(reading.sensor_readings[0]) + " ");
  // }
  // EEPROM.commit();

  Serial.println("reading");
  reading_idx = 0;
  for (size_t i = 0; i < NO_OF_READINGS; i++)
  {
    reading = EEPROM.get(reading_idx, reading);
    Serial.print(String(reading_idx) + " Reading: ");
    for (uint i = 0; i < NO_OF_SENSORS; i++)
    {
      Serial.print(String(reading.sensor_readings[i]) + " ");
    }
    reading_idx = (reading_idx + READING_SIZE) % NO_OF_READINGS_XS;
    Serial.println();
  }
  Serial.println("done");
  reading_idx = 0;

  // EEPROM.get(reading_idx, reading);
  // EEPROM.put(reading_idx, reading);
  // EEPROM.commit();
  // EEPROM.end();
}

void loop()
{
  // put your main code here, to run repeatedly:

  if (millis() - read_timer > READING_TIME_INTERVAL_MS)
  {
    for (uint i = 0; i < NO_OF_SENSORS; i++)
    {
      reading.sensor_readings[i] = analogRead(A0 + i);
    }
    EEPROM.put(reading_idx, reading);
    Serial.print(String(reading_idx) + " Reading: ");
    for (uint i = 0; i < NO_OF_SENSORS; i++)
    {
      Serial.print(String(reading.sensor_readings[i]) + " ");
    }
    Serial.println();
    reading_idx = (reading_idx + READING_SIZE) % NO_OF_READINGS_XS;
    read_timer = millis();
  }

  if (millis() - commit_timer > READING_COMMIT_INTERVAL_MS)
  {
    // EEPROM.end();
    EEPROM.commit();
    Serial.println("commited");
    commit_timer = millis();
  }
}