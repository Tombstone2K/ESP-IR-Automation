#ifndef RulesCustomClass_H
#define RulesCustomClass_H

#include <Arduino.h>
#include <ArduinoJSON.h>
#include <TimeLib.h>


class Rules {
public:
  String startTime;
  String endTime;
  float temperatureThreshold;
  bool greaterThanTemperature;
  bool actionTaken;
  String action;
  /**
   * SF - Superfan
   * AC - Air Conditioner
   * eg - SF5 -> superfan on 5
   * eg - AC26 -> ac on temperature 26 auto
  */
  int ruleType;
  /**
   * Rule Type 1 - between time and temperature condition
   * Rule Type 2 - between time on different days and temp condition
   * Rule Type 3 - straight timer condiditon
  */
  

  // Constructor
  Rules(String startTime, String endTime, float temperatureThreshold, bool greaterThanTemperature,  bool actionTaken, const String& action, int ruleType);

  // Serialize the instance to JSON
  void serialize(JsonObject& json) const;

  // Deserialize JSON to create an instance
  static Rules deserialize(const JsonObject& json);
};

void checkRulesAndTakeAction(int index, String currentTime, float currentTemp);
void saveRules(const char* filename);
void loadRules(const char* filename);
void loadInitAlarmAction();
void loadNextAlarmAction();
void decipherAndSendIR(String actionToBeTaken);

extern const int numRules;
extern int AlarmTimerIndex;
extern Rules rulesArray[4];

#endif
