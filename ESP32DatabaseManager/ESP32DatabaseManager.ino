#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include "credentials.h"
#include "data_structs.h"

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
    //Serial.print(".");
    delay(500);
  }
  
  digitalWrite(LED, LOW);

  //Serial.println();
  //Serial.println("Started");
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED) 
  {
    CheckMessage msg;
    
    // Check if there are incoming messages.
    if (Serial.available() >= sizeof(msg))
    {
      WiFiClient client;
      HTTPClient http;

      // Read the message.
      Serial.readBytes((char *) &msg, sizeof(msg));

      if (msg.type == CARD)
      {
        // Get the query string.
        char * studentCardQuery = generateStudentCardQuery(msg.number);
        
        // Send the http request.
        http.begin(client, studentCardQuery);

        // Free memory.
        free(studentCardQuery);
      }
      else if (msg.type == PIN)
      {
        // Get the query string.
        char * pinQuery = generatePinQuery(msg.number);

        // Send the http request.
        http.begin(client, pinQuery);

        // Free memory.
        free(pinQuery);
      }
      
      // Get response code.
      int httpCode = http.GET();
 
      if (httpCode > 0)
      {
        // Set the response code.
        msg.responseCode = (http.getString() == "OK") ? msg.responseCode = 1 : msg.responseCode = 2;

        Serial.write((uint8_t *) &msg, sizeof(msg));
      }
    }
  }

  delay(200);
}
