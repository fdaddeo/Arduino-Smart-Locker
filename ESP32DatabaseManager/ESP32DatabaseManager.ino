#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

#include "credentials.h"

#define LED 2


void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  IPAddress ip(192, 168, 1, 200);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns1(8, 8, 8, 8);
  IPAddress dns2(8, 8, 4, 4);

  WiFi.config(ip, gateway, subnet, dns1, dns2);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  
  digitalWrite(LED, LOW);

  Serial.println();
  Serial.println("Started");
}

void loop()
{
  WiFiClientSecure * client = new WiFiClientSecure();

  if (WiFi.status() == WL_CONNECTED) 
  {
    HTTPClient http;
    client->setInsecure();
 
    http.begin(*client, serverName);
    int httpCode = http.GET();
 
    if (httpCode > 0)
    {
      String payload = http.getString();
      Serial.println(payload);
    }
  }

  delay(10000);
}


