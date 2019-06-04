#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// IoT platform Credentials
String apiKey = "5QP41GS96CC21AIX";
const char *logServer = "api.thingspeak.com";

// Internet router credentials
const char *ssidAP = "ESP AP";
const char *passwordAP = "esp8266";

const char *ssidHome = "Redmi Note 5";
const char *passwordHome = "iamiron4";

ESP8266WebServer server(80);

void setupStMode(String t, String v);
void setupAccessPoint();
void setupServer();

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  setupAccessPoint();
}
// Handling the / root web page from my server
void handle_index()
{
  server.send(200, "text/plain", "Get the f**k out from my server!");
}
// Handling the /feed page from my server
void handle_feed()
{
  String t = server.arg("time");
  String h = server.arg("sensor");

  server.send(200, "text/plain", "This is response to client");
  Serial.println("Feed : time= "+String(t)+", sensor= "+String(h));
  setupStMode(t, h);
}

void setupAccessPoint()
{
  Serial.println("** SETUP ACCESS POINT **");
  Serial.println("- disconnect from any other modes");
  WiFi.disconnect();
  Serial.println("- start ap with SID: " + String(ssidAP));
  WiFi.softAP(ssidAP);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("- AP IP address is :");
  Serial.println(myIP);
  setupServer();
}

void setupServer()
{
  Serial.println("** SETUP SERVER **");
  Serial.println("- starting server :");
  server.on("/", handle_index);
  server.on("/feed", handle_feed);
  server.begin();
  Serial.println("- server up");

};

void setupStMode(String t, String v)
{
  Serial.println("** SETUP STATION MODE **");
  Serial.println("- disconnect from any other modes");
  WiFi.disconnect();
  Serial.println();
  Serial.println("- connecting to Home Router SID: " + String(ssidHome));
  WiFi.begin(ssidHome, passwordHome);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("- succesfully connected");
  Serial.println("- starting client");

  WiFiClient client;

  Serial.println("- connecting to Database server: " + String(logServer));
  if (client.connect(logServer, 80))
  {
    Serial.println("- succesfully connected");
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(v);
    Serial.println("- sending data...");
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
  }
  else
  {
    Serial.println("- unsuccesfully connect");
  }
  client.stop();
  Serial.println("- stopping the client");
  /** If ESP does not respond just reset after each request sending 
  Serial.println("- trying reset");
  ESP.reset();
  **/
}

void loop()
{
  server.handleClient();
}