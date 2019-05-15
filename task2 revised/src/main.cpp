#include <Arduino.h>
#include <fs.h>

#define READING_TIME_INTERVAL_MS (100)

char filename[] = "test.txt";
u16 sensorValue;
File myDataFile;
unsigned long read_timer = 0;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPIFFS.begin();
}

void loop()
{
  // put your main code here, to run repeatedly:

  if (millis() - read_timer > READING_TIME_INTERVAL_MS)
  {
    myDataFile = SPIFFS.open(filename, "a+");
    if (!myDataFile)
    {
      Serial.println("file open failed"); // Check for errors
      return;
    }
    sensorValue = analogRead(A0);
    myDataFile.println(sensorValue); // Write some data to it
    myDataFile.close();              // Close the file
    read_timer = millis();
  }
}