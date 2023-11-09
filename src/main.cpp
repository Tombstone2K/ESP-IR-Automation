// Imports
#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include <SPI.h>
#include "DHT.h"
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESPAsyncWebSrv.h>
#include <TimeLib.h>
#include <IRsend.h>
#include <Adafruit_Sensor.h>
#include <RTClib.h>
#include <ArduinoJSON.h>

#include "RtcSetup.h"
#include "RulesCustomClass.h"
#include "FileHandlingSD.h"
#include "WiFiCredentials.h"
#include "IrCodes.h"
#include "RequestHandlers.h"

// Variable definitions
#define DHTTYPE DHT11
uint8_t DHTPin = 4;
DHT dht(DHTPin, DHTTYPE);
float temperature;
float humid;

bool loadRulesAgain = false;

const uint16_t kIrLed = 15; // ESP8266 GPIO pin to use. Recommended: 4 (D2).
const char *temperatureRoute = "/temperature-humidity";

IRsend irsend(kIrLed); // Set the GPIO to be used to sending the message.

AsyncWebServer server(80);

// Setup function of Arduino
void setup()
{
  Serial.begin(115200);
  dht.begin();
  irsend.begin();

  // Init
  initializeWiFi();
  initializeSD();
  initializeDataTextFile();
  initializeLogsFile();

  // Set up the web server
  server.on(temperatureRoute, HTTP_GET, handleDataRetreival);
  server.on("/", HTTP_GET, handleMain);
  server.on("/logs", HTTP_GET, handleLogsPage);
  server.on("/testPing", HTTP_GET, handleTestPing);

  // Handle incoming Rules in form of JSON
  server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
                       {
    if (request->url() == "/json") {
      StaticJsonDocument<1024> doc;
      // Deserialize JSON content
      deserializeJson(doc, data);
      // Serial.println(doc["name"].as<String>());
      Serial.println(doc.as<String>());

      // Save JSON data to SD card
      File file = SD.open("/rules.json", FILE_WRITE);
      if (!file) {
        Serial.println("Failed to open file.");
        return;
      }
      
      serializeJson(doc, file);
      file.close();
      // Save Rules to FIle
      Serial.println("Data saved to SD card.");
      loadRulesAgain = true;
      request->send(200, "text/plain", "true");
    } });

  server.begin();
  Serial.println("Web server started");

  // Setup RTC and load Rules
  setupRTC(connectedToWiFi);
  loadRules("/rules.json");
}

// Loop function
void loop()
{

  // Handling alarms set for certain IR action (according to rules set)
  if (intFlag && rtc.alarmFired(1))
  {
    DateTime tempp = rtc.now();
    String dataMessage = "Alarm 1 fired" + String(tempp.hour()) + ":" + String(tempp.minute()) + ":" + String(tempp.second()) + "\n";
    appendFile(SD, "/logs.txt", dataMessage.c_str());
    decipherAndSendIR(rulesArray[AlarmTimerIndex].action);
    loadNextAlarmAction();
    Serial.println("ALARM INT");
    rtc.clearAlarm(1);
    intFlag = false;
  }

  // Handling regular temperature Data logging
  if (intFlag && rtc.alarmFired(2))
  {
    Serial.println("HANDLING NORMAL INT");

    rtc.clearAlarm(2);
    alarmInt = DateTime(alarmInt + TimeSpan(0, 0, 5, 0));
    setAlarmFunction();

    temperature = dht.readTemperature();
    humid = dht.readHumidity();

    // Log the temperature data to the SD card
    DateTime newUpdate = rtc.now();
    char formattedDate[11]; // "YYYY-MM-DD" + '\0';
    char formattedTime[9];  // "HH:MM:SS" + '\0'
    sprintf(formattedTime, "%02d:%02d:%02d", newUpdate.hour(), newUpdate.minute(), newUpdate.second());
    sprintf(formattedDate, "%04d-%02d-%02d", newUpdate.year(), newUpdate.month(), newUpdate.day());
    String dataMessage = String(formattedDate) + "," + String(formattedTime) + "," +
                         String(temperature) + "," + String(humid) + "\r\n";
    Serial.print("Save data: ");
    Serial.println(dataMessage);
    appendFile(SD, "/data.txt", dataMessage.c_str());

    intFlag = false;
  }

  // Check for time range rules
  if (millis() - lastLogTime >= 300000)
  {
    lastLogTime = millis();
    DateTime timeRightNow = rtc.now();
    char formattedTime[6]; // "HH:MM" + '\0'
    float tempRightNow = dht.readTemperature();
    sprintf(formattedTime, "%02d:%02d", timeRightNow.hour(), timeRightNow.minute());
    for (int i = 0; i < numRules; i++)
    {
      checkRulesAndTakeAction(i, String(formattedTime), tempRightNow);
      // Rules currentRule = rulesArray[i];
    }
  }

  // Load rules after changes received by ESP
  if (loadRulesAgain)
  {
    loadRules("/rules.json");
    loadRulesAgain = false;
  }
}
