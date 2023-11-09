#include "RTCSetup.h"
#include <Wire.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

RTC_DS3231 rtc;
const int SQW_INTERRUPT_PIN = 26;
bool intFlag = false;
DateTime alarmInt;
unsigned long lastLogTime = 0; // 300000;

// Setup the RTC module
void setupRTC(bool connectedToWiFi)
{
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC!");
    Serial.flush();
    abort();
  }

  if (rtc.lostPower())
  {
    Serial.println("RTC lost power, set the time!");
  }

  // we don't need the 32K Pin, so disable it
  rtc.disable32K();

  if (connectedToWiFi)
  {
    // Calibrate the time according to NTP server
    WiFiUDP ntpUDP;
    NTPClient timeClient(ntpUDP);
    timeClient.begin();
    timeClient.setTimeOffset(19800);
    // Adding offset for Indian Standard Time (IST)
    timeClient.update();
    time_t epochTime = timeClient.getEpochTime();
    if (epochTime > 1577838812 && epochTime < 1893458012)
    {
      rtc.adjust(DateTime(epochTime));
    }
  }

  // Making it so, that the alarm will trigger an interrupt
  pinMode(SQW_INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SQW_INTERRUPT_PIN), handleInterrupt, FALLING);

  // set alarm 1, 2 flag to false (so alarm 1, 2 didn't happen so far)
  // if not done, this easily leads to problems, as both register aren't reset on reboot/recompile
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  // stop oscillating signals at SQW Pin
  // otherwise setAlarm1 will fail
  rtc.writeSqwPinMode(DS3231_OFF);

  // turn off alarm 2 (in case it isn't off already)
  // again, this isn't done at reboot, so a previously set alarm could easily go overlooked
  // rtc.disableAlarm(1);
  uint8_t secondss; // = (now.second()!=0) ? (60-now.second()) : (0);
  uint8_t minutess; // = ((now.minute()%5) !=0) ? ((((now.minute() + 4) / 5) * 5)-now.minute()) : (now.minute());
  DateTime now = rtc.now();
  if (now.second() == 0)
  {
    secondss = 0;
    minutess = (5 - (now.minute() % 5));
  }
  else
  {
    secondss = (60 - now.second());
    minutess = (4 - (now.minute() % 5));
  }
  lastLogTime = 240000 - (minutess * 60 + secondss) * 1000;

  alarmInt = DateTime(now + TimeSpan(0, 0, minutess, secondss)); // total seconds or // days,hrs,mins,secs

  setAlarmFunction();
}

void handleInterrupt()
{
  intFlag = true;
}

void setAlarmFunction()
{
  if (!rtc.setAlarm2(alarmInt, DS3231_A2_Hour))
  {
    Serial.println("Error, alarm wasn't set!");
  }
  else
  {
    Serial.print("Alarm will occur at: ");
  }

  Serial.print(alarmInt.year(), DEC);
  Serial.print('/');
  Serial.print(alarmInt.month(), DEC);
  Serial.print('/');
  Serial.print(alarmInt.day(), DEC);
  Serial.print(' ');
  Serial.print(alarmInt.hour(), DEC);
  Serial.print(':');
  Serial.print(alarmInt.minute(), DEC);
  Serial.print(':');
  Serial.print(alarmInt.second(), DEC);
  Serial.println();
}
