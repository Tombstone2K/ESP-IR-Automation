#include "RulesCustomClass.h"
#include "FS.h"
#include "SD.h"
#include <RTCLib.h>

#include "IrCodes.h"
#include "RtcSetup.h"
#include "FileHandlingSD.h"

// Rules Schema
/**
 * Rules(String startTime, String endTime, float temperatureThreshold, bool greaterThanTemperature, bool actionTaken, const String &action, int ruleType)
 *
 * startTime - contains the startTime for time range based rules OR action trigger time for Time based alarm rules
 * endTime - contains the endTime for time range based rules - irrelevant for alarm based rules
 * temperatureThreshold - contains the temperature threshold, crossing which action to be triggered
 * greaterThanThreshold - bool variable to store whether action to be taken if temperature is greater than or lesser than threshold
 * actionTaken - bool variable to store whether action has been taken in stipulated time range to prevent multiple triggers in same time window
 * action - contains action to be taken - in the form of two alphabets followed by digits
 *        - the alphabets indicate the device/appliance
 *        -  the digits represent the AC temperature setting OR Ceiling Fan Speed setting
 * ruleType - integer indicating the rule Type
 *          - Type 1 & 2 indicates a time range based rule with temperature threshold
 *          - Type 1 has startTime and endTime on the same day
 *          - Type 2 has startTime and endTime on different days
 *          - Type 3 is a time based alarm trigger rule
 *
 */

// Constructor
Rules::Rules(String startTime, String endTime, float temperatureThreshold, bool greaterThanTemperature, bool actionTaken, const String &action, int ruleType)
    : startTime(startTime), endTime(endTime), temperatureThreshold(temperatureThreshold), greaterThanTemperature(greaterThanTemperature), actionTaken(actionTaken), action(action), ruleType(ruleType) {}

// currently restricted to 4 rules only
// can easily be scaled up if neccesary
const int numRules = 4;
int AlarmTimerIndex;

bool checkTempRule(float currentTemp, bool greaterThanTemperature, float temperatureThreshold);
bool compareTime2(String sTime, String currentTime, String eTime);
bool compareTime1(String sTime, String currentTime, String eTime);
void loadInitAlarmAction();
void loadNextAlarmAction();
void decipherAndSendIR(int index);

// Definition and initialization of rulesArray
Rules rulesArray[4] = {
    Rules("04:00", "06:00", 25.5, true, false, "AA0", 1),
    Rules("23:00", "02:00", 38.0, false, false, "AA0", 2),
    Rules("03:31", "03:32", 38.0, false, false, "AA0", 3),
    Rules("04:22", "4:23", 38.0, false, false, "AA0", 3),

};

// Serialize the instance to JSON
void Rules::serialize(JsonObject &json) const
{
  json["startTime"] = startTime;
  json["endTime"] = endTime;
  json["temperatureThreshold"] = temperatureThreshold;
  json["greaterThanTemperature"] = greaterThanTemperature;
  json["actionTaken"] = actionTaken;
  json["action"] = action;
  json["ruleType"] = ruleType;
}

// Deserialize JSON to create an instance
Rules Rules::deserialize(const JsonObject &json)
{
  return Rules(json["startTime"], json["endTime"], json["temperatureThreshold"], json["greaterThanTemperature"], json["actionTaken"], json["action"], json["ruleType"]);
}

// Save Rules to SD card file
void saveRules(const char *filename)
{
  File file = SD.open(filename, FILE_WRITE); // Open the file on the SD card
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }

  DynamicJsonDocument doc(1024); // Create a JSON document

  // Create a JSON array
  JsonArray jsonArray = doc.to<JsonArray>();

  // Serialize each Rules instance and add it to the array
  for (int i = 0; i < numRules; i++)
  {
    JsonObject json = jsonArray.createNestedObject();
    rulesArray[i].serialize(json);
  }

  // Serialize the JSON document to the file
  if (serializeJson(doc, file) == 0)
  {
    Serial.println("Failed to write to file");
  }

  // Close the file
  file.close();
}

// Load Rules from SD card file
void loadRules(const char *filename)
{
  File file = SD.open(filename, FILE_READ); // Open the file on the SD card
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }

  DynamicJsonDocument doc(1024); // Create a JSON document

  // Deserialize the JSON document from the file
  DeserializationError error = deserializeJson(doc, file);
  if (error)
  {
    Serial.println("Failed to read file");
    file.close();
    return;
  }

  // Close the file
  file.close();

  // Read the JSON array and deserialize into Rules instances
  JsonArray jsonArray = doc.as<JsonArray>();
  if (jsonArray.size() != numRules)
  {
    Serial.println("Invalid number of rules in file");
    return;
  }

  // Deserialize each JSON object in the array into a Rules instance
  for (int i = 0; i < numRules; i++)
  {
    JsonObject json = jsonArray[i].as<JsonObject>();
    rulesArray[i] = Rules::deserialize(json);
  }

  loadInitAlarmAction();
}

// Check time range based rules and trigger action if condition met
void checkRulesAndTakeAction(int index, String currentTime, float currentTemp)
{
  Rules currentRule = rulesArray[index];
  switch (currentRule.ruleType)
  {
  case 1:
    if (compareTime1(currentRule.startTime, currentTime, currentRule.endTime))
    {
      // Serial.println("HEYY");
      if ((currentRule.actionTaken == false) &&
          checkTempRule(currentTemp, currentRule.greaterThanTemperature, currentRule.temperatureThreshold))
      {

        decipherAndSendIR(currentRule.action);
        DateTime tempp = rtc.now();
        String dataMessage = "RULE TYPE 1 " + String(tempp.hour()) + ":" + String(tempp.minute()) + ":" + String(tempp.second()) + "\n";
        appendFile(SD, "/logs.txt", dataMessage.c_str());
        // Log rule firing into SD card file
        rulesArray[index].actionTaken = true;
      }
    }
    else
    {
      rulesArray[index].actionTaken = false;
    }
    break;
  case 2:
    if (compareTime2(currentRule.startTime, currentTime, currentRule.endTime))
    {
      if ((currentRule.actionTaken == false) &&
          checkTempRule(currentTemp, currentRule.greaterThanTemperature, currentRule.temperatureThreshold))
      {
        // Take ACtion
        decipherAndSendIR(currentRule.action);
        DateTime tempp = rtc.now();
        String dataMessage = "RULE TYPE 2 " + String(tempp.hour()) + ":" + String(tempp.minute()) + ":" + String(tempp.second()) + "\n";
        appendFile(SD, "/logs.txt", dataMessage.c_str());
        // Log rule firing into SD card file
        rulesArray[index].actionTaken = true;
      }
    }
    else
    {
      rulesArray[index].actionTaken = false;
    }
    break;
  case 3:
    break;
  default:
    break;
  }
}

// Check whether time lies between start time and end time where both fall on same days
bool compareTime1(String sTime, String currentTime, String eTime)
{

  if ((sTime.compareTo(currentTime) <= 0) && (currentTime.compareTo(eTime) <= 0))
  {
    return true;
  }
  else
  {
    return false;
  }
}

// Check whether time lies between start time and end time where both fall on different days
bool compareTime2(String sTime, String currentTime, String eTime)
{

  if ((sTime.compareTo(currentTime) <= 0) || (currentTime.compareTo(eTime) <= 0))
  {
    return true;
  }
  else
  {
    return false;
  }
}

// Check whether current temperature meets the condition (aka crosses the threshold)
bool checkTempRule(float currentTemp, bool greaterThanTemperature, float temperatureThreshold)
{
  if (greaterThanTemperature)
  {
    return (currentTemp > temperatureThreshold);
  }
  else
  {
    return (currentTemp <= temperatureThreshold);
  }
}

// load the alarm based rules during initialization
void loadInitAlarmAction()
{
  int i = 0;
  while (i < numRules)
  {
    if (rulesArray[i].ruleType == 3)
    {
      String time1 = rulesArray[i].startTime;
      AlarmTimerIndex = i;
      int hour1 = time1.substring(0, 2).toInt();
      int minute1 = time1.substring(3).toInt();
      DateTime temp(2023, 7, 4, hour1, minute1, 30);
      if (!rtc.setAlarm1(temp, DS3231_A1_Hour))
      {
        Serial.println("Error, alarm wasn't set!");
      }
      else
      {

        DateTime tempp = rtc.now();
        String dataMessage = "Init Alarm Set successfully " + String(hour1) + ":" + String(minute1) + "\n";
        appendFile(SD, "/logs.txt", dataMessage.c_str());

        Serial.print(dataMessage);
      }
      return;
    }
    i++;
  }
  rtc.disableAlarm(1);
}

// load the subsequent alarm based rules
void loadNextAlarmAction()
{
  int temp = (AlarmTimerIndex + 1) % numRules;
  int index = temp;
  do
  {
    if (rulesArray[index].ruleType == 3)
    {
      String time1 = rulesArray[index].startTime;
      AlarmTimerIndex = index;
      int hour1 = time1.substring(0, 2).toInt();
      int minute1 = time1.substring(3).toInt();
      DateTime temp(2023, 7, 4, hour1, minute1, 30);
      if (!rtc.setAlarm1(temp, DS3231_A1_Hour))
      {
        Serial.println("Error, alarm wasn't set!");
      }
      else
      {

        DateTime tempp = rtc.now();
        String dataMessage = "Next Alarm Set successfully for " + String(hour1) + ":" + String(minute1) + "\n";
        appendFile(SD, "/logs.txt", dataMessage.c_str());

        Serial.print(dataMessage);
      }
      return;
    }
    index = (index + 1) % numRules;
  } while (index != temp);
}

// decipher the action to be taken based on the rule and send appropriate IR signal
void decipherAndSendIR(String actionToBeTaken)
{
  // String actionToBeTaken = rulesArray[index].action;
  String device = actionToBeTaken.substring(0, 2);
  int numberComponent = actionToBeTaken.substring(2).toInt();
  Serial.println("Sending IR signal");
  appendFile(SD, "/logs.txt", "Sending IR signal\n");
  if (device == "SF")
  {
    switch (numberComponent)
    {
    case 0:
      irsend.sendRaw(SF0, RAW_DATA_LEN_FAN, 36);
      break;
    case 1:
      irsend.sendRaw(SF1, RAW_DATA_LEN_FAN, 36);
      break;
    case 2:
      irsend.sendRaw(SF2, RAW_DATA_LEN_FAN, 36);
      break;
    case 3:
      irsend.sendRaw(SF3, RAW_DATA_LEN_FAN, 36);
      break;
    case 4:
      irsend.sendRaw(SF4, RAW_DATA_LEN_FAN, 36);
      break;
    case 5:
      irsend.sendRaw(SF5, RAW_DATA_LEN_FAN, 36);
      break;
    default:
      break;
    }
  }
  else if (device == "AC")
  {
    switch (numberComponent)
    {
    case 0:
      irsend.sendRaw(AC0, RAW_DATA_LEN, 36);
      break;
    case 23:
      irsend.sendRaw(AC23, RAW_DATA_LEN, 36);
      break;
    case 24:
      irsend.sendRaw(AC24, RAW_DATA_LEN, 36);
      break;
    case 25:
      irsend.sendRaw(AC25, RAW_DATA_LEN, 36);
      break;
    case 26:
      irsend.sendRaw(AC26, RAW_DATA_LEN, 36);
      break;
    case 27:
      irsend.sendRaw(AC27, RAW_DATA_LEN, 36);
      break;
    case 28:
      irsend.sendRaw(AC28, RAW_DATA_LEN, 36);
      break;
    case 29:
      irsend.sendRaw(AC29, RAW_DATA_LEN, 36);
      break;
    default:
      break;
    }
  }
}