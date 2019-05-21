#include <Arduino.h>
#include <ESP8266WiFi.h>

const char *ssid = "Famillia";
const char *password = "xx";

int ledPin = D3;
WiFiServer server(4545);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("");
  Serial.println(WiFi.localIP());
  WiFi.begin(ssid, password);

  Serial.println("Connecting.....");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi is connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("Server started");
}

void loop()
{
  // put your main code here, to run repeatedly:

  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }
  // Wait until the client sends some data
  while (!client.available())
  {
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  client.flush();

  // Match the request
  Serial.println("request: " + req);
  if (req == "GET / HTTP/1.1")
  {
    // Prepare the response
    String s = "HTTP/1.1 200 OK\r\n";
    s += "Content-Type: text/html\r\n\r\n";
    s += "<!DOCTYPE HTML>\r\n<html>\r\n";
    s += "<br><input type=\"button\" name=\"bl\" value=\"Turn LED ON \" onclick=\"location.href='/ON'\">";
    s += "<br><br><br>";
    s += "<br><input type=\"button\" name=\"bl\" value=\"Turn LED OFF\" onclick=\"location.href='/OFF'\">";
    s += "</html>\n";

    client.flush();

    // Send the response to the client
    client.print(s);
  }
  else if (req.indexOf("/OFF") != -1)
  { //checks if you clicked OFF
    digitalWrite(ledPin, LOW);
    Serial.println("clicked OFF");
  }
  else if (req.indexOf("/ON") != -1)
  { //checks if you clicked ON
    digitalWrite(ledPin, HIGH);
    Serial.println("clicked ON");
  }
  else
  {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  delay(1);
}