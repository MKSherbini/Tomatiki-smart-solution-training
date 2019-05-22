#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"

const char *ssid = "Famillia";
const char *password = "xx";

unsigned long myChannelNumber = 775988;
const char *myWriteApiKey = "5QP41GS96CC21AIX";
const char *myReadApiKey = "UZ5CDSWN13R9NH47";

const char *server = "api.thingspeak.com";

WiFiClient client;

void setup()
{

  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("");

  ThingSpeak.begin(client);
  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi is connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  // put your main code here, to run repeatedly:

  if (client.connect(server, 80))
  {
    ThingSpeak.setField(1, analogRead(A0));
    ThingSpeak.setField(2, (long)millis());
    ThingSpeak.writeFields(myChannelNumber, myWriteApiKey);
    delay(20000);
  }
}