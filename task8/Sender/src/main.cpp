#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// AP Wi-Fi credentials
const char *ssid = "ESP AP";
const char *password = "esp8266";

// Local ESP web-server address
String serverHost = "http://192.168.4.1/feed";
String data;
// DEEP_SLEEP Timeout interval
int sleepInterval = 5;
// DEEP_SLEEP Timeout interval when connecting to AP fails
int failConnectRetryInterval = 2;
int counter = 0;

float s;
float t;
// Static network configuration
IPAddress ip(192, 168, 4, 4);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiClient client;

void hibernate(int);
void sendHttpRequest();
void buildDataStream();
void readSensor();

void setup()
{
  ESP.eraseConfig();
  WiFi.persistent(false);
  Serial.begin(115200);
  Serial.println();
  Serial.println("**************************");
  Serial.println("**************************");
  Serial.println("******** BEGIN ***********");

  delay(500);
  Serial.println("- set ESP STA mode");
  WiFi.mode(WIFI_STA);
  Serial.println("- connecting to wifi");
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED)
  {
    if (counter > 20)
    {
      Serial.println("- can't connect, going to sleep");
      hibernate(failConnectRetryInterval);
    }
    delay(500);
    Serial.print(".");
    counter++;
  }

  Serial.println("- wifi connected");
  Serial.println("- read sensor");
  readSensor();
  Serial.println("- build DATA stream string");
  buildDataStream();
  Serial.println("- send GET request");
  sendHttpRequest();
  Serial.println();
  Serial.println("- got back to sleep");
  Serial.println("**************************");
  Serial.println("**************************");
  hibernate(sleepInterval);
}

void sendHttpRequest()
{
  HTTPClient http;
  http.begin(serverHost); //todo remove deprecated
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.POST(data);
  http.writeToStream(&Serial);
  http.end();
}

void readSensor()
{
  delay(200);
  s = analogRead(A0);
  t = millis();

  Serial.println("- time read : " + String(t));
  Serial.println("- sensor read : " + String(s));
}

void buildDataStream()
{
  data = "time=";
  data += String(t);
  data += "&sensor=";
  data += String(s);
  Serial.println("- data stream: " + data);
}

void hibernate(int pInterval)
{
  WiFi.disconnect();
  ESP.deepSleep(10 * 600000 * pInterval, WAKE_RFCAL);
  delay(100);
}

void loop() {}