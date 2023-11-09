#include "FileHandlingSD.h"
#include "SD.h"

// File Handling Functions

// Init SD card
void initializeSD()
{
  if (!SD.begin(5))
  {
    Serial.println("SD card initialization failed!");
    return;
  }
}

// Init Data file to log temperature
void initializeDataTextFile()
{
  File file = SD.open("/data.txt");
  if (!file)
  {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/data.txt", "Date,Hour,Temperature,Humidity\r\n");
  }
  else
  {
    Serial.println("data.txt already exists");
  }
  file.close();
}

// Init Log File
void initializeLogsFile()
{
  File file = SD.open("/logs.txt");
  if (!file)
  {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/logs.txt", "LOGS :) \r\n");
  }
  else
  {
    Serial.println("logs.txt already exists");
  }
  file.close();
}

// Helper function to write to file
void writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    Serial.println("File written");
  }
  else
  {
    Serial.println("Write failed");
  }
  file.close();
}

// Helper function to append to file
// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file)
  {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message))
  {
    Serial.println("Message appendedd");
  }
  else
  {
    Serial.println("Append failed");
  }
  file.close();
}