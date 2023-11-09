// WiFiCredentials.cpp

#include "WiFiCredentials.h"
#include <WiFi.h>
#include "SD.h"
#include <FileHandlingSD.h>

const char *ssid = "WIFI-SSID";
const char *password = "WIFI-PASSWORD";
bool connectedToWiFi;

// Connect to Wi-Fi
void initializeWiFi()
{
  Serial.print("Connecting to ");
  Serial.println(ssid);
  uint8_t numberOfTries = 0;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if (++numberOfTries > 24)
    {
      break;
    }
  }
  Serial.println("");

  // Abort if number of tries exceeds 24
  if (++numberOfTries > 24)
  {
    Serial.println("Not able to connect to Wi-Fi");
    appendFile(SD, "/logs.txt", "Not Connected to Wifi \n");
    connectedToWiFi = false;
  }
  else
  {
    Serial.println("WiFi connected.");
    Serial.println(WiFi.localIP());
    connectedToWiFi = true;
  }
}