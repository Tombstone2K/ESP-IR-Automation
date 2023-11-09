#ifndef RTC_SETUP_H
#define RTC_SETUP_H

#include <RTClib.h>

extern RTC_DS3231 rtc;
extern DateTime alarmInt;
extern const int SQW_INTERRUPT_PIN;
extern bool intFlag;
extern unsigned long lastLogTime;

void setupRTC(bool connectedToWiFi);
void handleInterrupt();
void setAlarmFunction();

#endif
