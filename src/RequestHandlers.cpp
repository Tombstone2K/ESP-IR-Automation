// RequestHandlers.cpp

#include "RequestHandlers.h"
#include "SD.h"
#include <IRsend.h>
#include "IrCodes.h"
#include <ArduinoJSON.h>

String handleUpdate(AsyncWebServerRequest *request);

// Handle all incoming server requests

// Serves the temperature data
void handleMain(AsyncWebServerRequest *request)
{
  Serial.println("IN MAIN PAGE");
  Serial.println("TRYING TO SEND DATA");
  request->send(SD, "/data.txt", "text/plain");
}

// Serves the file containing the temperature data
void handleDataRetreival(AsyncWebServerRequest *request)
{
  Serial.println("TRYING TO SEND DATA");
  request->send(SD, "/data.txt", "text/csv");
}

// Serves the Log Page
void handleLogsPage(AsyncWebServerRequest *request)
{
  Serial.println("IN LOGS PAGE");
  request->send(SD, "/logs.txt", "text/plain");
}

// Serves content in second.html
void handleSec(AsyncWebServerRequest *request)
{
  Serial.println("IN SECOND PAGE");
  request->send(SD, "/second.html", "text/html");
}

// Function used by the companion app to verify that ESP is online
void handleTestPing(AsyncWebServerRequest *request)
{
  Serial.println("IN TEST PING");
  String response = handleUpdate(request);
  request->send(200, "text/plain", response);
}

// function to trigger IR signals based on button pressed in companion app
String handleUpdate(AsyncWebServerRequest *request)
{
  if (request->hasParam("output") && request->hasParam("state"))
  {
    String output = request->getParam("output")->value();
    String state = request->getParam("state")->value();

    // Process the values and generate a response
    String response = "Output: " + output + ", State: " + state;
    if (output == "1" && state == "2")
    {
      irsend.sendRaw(SF2, RAW_DATA_LEN_FAN, 36);
      return "SENT IR CODE FOR FAN";
    }
    if (output == "1" && state == "3")
    {
      irsend.sendRaw(SF3, RAW_DATA_LEN_FAN, 36);
      return "SENT IR CODE FOR FAN";
    }
    if (output == "2" && state == "25")
    {
      irsend.sendRaw(AC25, RAW_DATA_LEN, 36);
      return "SENT IR CODE FOR AC";
    }
    if (output == "2" && state == "26")
    {
      irsend.sendRaw(AC26, RAW_DATA_LEN, 36);
      return "SENT IR CODE FOR AC";
    }

    return response;
  }
  else
  {
    // Handle the case when query parameters are missing
    return "Invalid request: Missing query parameters";
  }
}

// Handle a file upload via Web interface or companion app
void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
  static File file;

  if (!index)
  {
    // Create or overwrite the file on the SD card
    if (SD.exists(filename))
    {
      SD.remove(filename);
    }

    // Create a new file on the SD card
    file = SD.open(filename, FILE_WRITE);
    // file = SD.open(filename, FILE_WRITE);
  }

  if (file)
  {
    // Write the received data to the file
    file.write(data, len);

    if (final)
    {
      // File upload completed
      file.close();
      request->send(200);
    }
  }
  else
  {
    // Failed to open the file
    request->send(500);
  }
}

// Method to handle the JSON data sent over web request
void handleJSON(AsyncWebServerRequest *request)
{
  // Print the request method (GET, POST, etc.)
  Serial.print("Request Method: ");
  Serial.println(request->methodToString());

  // Print the request URL
  Serial.println("Request URL: " + request->url());

  // Print the request headers
  Serial.println("Request Headers:");
  int headerCount = request->headers();
  for (int i = 0; i < headerCount; i++)
  {
    AsyncWebHeader *header = request->getHeader(i);
    Serial.println(header->name() + ": " + header->value());
  }

  // Print the request parameters (URL parameters)
  Serial.println("Request Parameters:");
  int paramCount = request->params();
  for (int i = 0; i < paramCount; i++)
  {
    AsyncWebParameter *param = request->getParam(i);
    Serial.println(param->name() + ": " + param->value());
  }

  // Print the request body
  String requestBody = request->arg("plain");
  Serial.println("Request Body:");
  Serial.println(requestBody);

  // Handle the rest of the request processing...
}
