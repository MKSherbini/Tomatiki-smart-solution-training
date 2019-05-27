#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "ThingSpeak.h"
#include <time.h>
#include <Crypto.h>
#include "fs_database.h"

const char *ssid = "Famillia";
const char *password = "khaledsa14";

unsigned long myChannelNumber = 775988;
const char *myWriteApiKey = "5QP41GS96CC21AIX";
const char *myReadApiKey = "UZ5CDSWN13R9NH47";
const char *server = "api.thingspeak.com";
const char *fingerprint PROGMEM = "F9 C2 65 6C F9 EF 7F 66 8B F7 35 FE 15 EA 82 9F 5F 55 54 3E";

WiFiClientSecure client;

int caesar_key;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  fs_database_init();
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
  client.setFingerprint(fingerprint);
}
void makehttpsRequestReadFeeds();
String caesar_encrypt(String n);
 unsigned int FNVHash(int n);
void loop()
{
  // put your main code here, to run repeatedly:
  makehttpsRequestReadFeeds();

  if (client.connect(server, 443))
  {
    Serial.println("ssl success");
    // ThingSpeak.setField(1, analogRead(A0));
    // ThingSpeak.setField(2, String((long)millis()));
    ThingSpeak.setField(1, caesar_encrypt(String(analogRead(A0))));
    ThingSpeak.setField(2, caesar_encrypt(String(millis())));
    ThingSpeak.writeFields(myChannelNumber, myWriteApiKey);
     delay(20000);
  }else{
     Serial.println("ssl failed");
     delay(100);
  }
  add_record_I(FNVHash(millis()), analogRead(A0), millis());
  delay(20000);
}
 unsigned int FNVHash(int n)
{
  String str(n);
  const unsigned int fnv_prime = 107;
  unsigned int hash = 0;
  unsigned int i = 0;
  unsigned int len = str.length();

  for (i = 0; i < len; i++)
  {
    hash *= fnv_prime;
    hash ^= (str[i]);
  }

  return hash;
}
String caesar_encrypt(String n)
{
  String s(n);
  for (size_t i = 0; i < s.length(); i++)
  {
    s[i] = ((s[i] - caesar_key - 48) % 10 + 10) % 10 + 48;
  }
  Serial.println(s);
  return s;
}
String caesar_decrypt(String n)
{
  String s(n);
  for (size_t i = 0; i < s.length(); i++)
  {
    s[i] = ((s[i] - caesar_key - 48) % 10 + 10) % 10 + 48;
  }
  return s;
}

void makehttpsRequestReadFeeds()
{
  // close any connection before send a new request to allow client make connection to server
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 443))
  {
    // Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("GET /channels/775988/feeds.json HTTP/1.1");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Host: ");
    client.println(server);
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();

    unsigned long timeout = millis();
    while (client.available() == 0)
    {
      if (millis() - timeout > 5000)
      {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    // while(client.available())Serial.write(client.read());
    // Check HTTP status
    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0)
    {
      Serial.print(F("Unexpected response: "));
      Serial.println(status);
      return;
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders))
    {
      Serial.println(F("Invalid response"));
      return;
    }

    //parse json
    const size_t bufferSize = 2 * JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(2) + 4 * JSON_OBJECT_SIZE(1) + 3 * JSON_OBJECT_SIZE(2) + 3 * JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 2 * JSON_OBJECT_SIZE(7) + 2 * JSON_OBJECT_SIZE(8) + 720;
    // long bufferSize= client.readStringUntil('\n').toInt();
    // bufferSize=bufferSize *JSON_OBJECT_SIZE(1)*1000U+bufferSize *JSON_ARRAY_SIZE(1)*1000U;
    client.readStringUntil('\n');
    Serial.println(bufferSize);
    DynamicJsonDocument jsonBuffer(bufferSize * 1000);

    // FIND FIELDS IN JSON TREE
    DeserializationError error = deserializeJson(jsonBuffer, client);

    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      Serial.println(jsonBuffer.as<String>());
      return;
    }
    else
    {
      Serial.println(F("deserializeJson() success: "));
    }

    const JsonArray &list = jsonBuffer["feeds"];
    caesar_key = list[0]["field1"];
    for (size_t i = 1; i < jsonBuffer["channel"]["last_entry_id"]; i++)
    {
      Serial.println(caesar_decrypt(list[i]["field1"].as<String>()));
      Serial.println(caesar_decrypt(list[i]["field2"].as<String>()));
    }
  }
  else
  {
    // if no connction was made:
    Serial.println("connection failed");
    return;
  }
}