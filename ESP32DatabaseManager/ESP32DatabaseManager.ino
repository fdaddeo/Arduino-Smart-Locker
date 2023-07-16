#include <WiFi.h>
#include <HTTPClient.h>

#include "credentials.h"

#define LED 2

//String msg = "<C751AB79";
// String msg = "$123456";

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
    delay(500);
  }
  
  digitalWrite(LED, LOW);
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED) 
  {    
    // Check if there are incoming messages.
    if (Serial.available() > 0)
    {
      HTTPClient http;

      String msg = Serial.readString();

      if (msg[0] == '<')
      {
        // Card readed
        char card[8];

        for (size_t i = 0; i < 8; ++i)
        {
          card[i] = msg[i+1];
        }

        char * studentCardQuery = generateStudentCardQuery(card);
        
        http.begin(studentCardQuery);
      }
      else if (msg[0] == '$')
      {
        // Pin readed.
        char pin[6];

        for (size_t i = 0; i < 6; ++i)
        {
          pin[i] = msg[i+1];
        }

        char * studentPinQuery = generatePinQuery(pin);
        http.begin(studentPinQuery);
      }
      
      // Get response code.
      int httpCode = http.GET();
 
      if (httpCode > 0)
      {
        // Set the response code.
        String responseCode;
        String response = http.getString();

        if (response == "OK")
        {
          responseCode = "1\n";
        }
        else if (response == "BOOKED")
        {
          responseCode = "2\n";
        }
        else if (response == "OCCUPIED")
        {
          responseCode = "3\n";
        }
        else if (response == "RES_DONE")
        {
          responseCode = "4\n";
        }
        else if (response == "UNAUTHORIZED")
        {
          responseCode = "5\n";
        }

        //Serial.println(responseCode);
        Serial.write(responseCode.c_str());
      }
    }
  }

  delay(200);
}
