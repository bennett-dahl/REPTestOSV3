#ifndef STRUCTS_H
#define STRUCTS_H

#include "REPTestOS_menus_menu.h"

struct testParams
{
  long totalCycles = 0;
  long cyclesRemaining = 0;
  long cyclesExecuted = 0;
  int minPressure;

  int daysRemaining;

  TimeStorage timeRemaining;
  TimeStorage totalTime;

  void decSecond()
  {
    if (timeRemaining.seconds > 0)
    {
      timeRemaining.seconds--;
    }
    else if (timeRemaining.minutes > 0)
    {
      timeRemaining.minutes--;
      timeRemaining.seconds = 59;
    }
    else if (timeRemaining.hours > 0)
    {
      timeRemaining.hours--;
      timeRemaining.minutes = 59;
      timeRemaining.seconds = 59;
    }
    else if (daysRemaining > 0)
    {
      daysRemaining--;
      timeRemaining.hours = 24;
      timeRemaining.minutes = 59;
      timeRemaining.seconds = 59;
    }
  }

  long getSeconds()
  {
    long seconds;
    seconds += this->timeRemaining.seconds;
    seconds += this->timeRemaining.minutes * 60;
    seconds += this->timeRemaining.hours * 60 * 60;
    seconds += this->daysRemaining * 24 * 60 * 60;
    return seconds;
  }

  void setBySeconds(long seconds)
  {
    Serial.println(seconds);
    long minutes = seconds / 60;
    long hours = minutes / 60;
    long days = hours / 24;
    this->daysRemaining = int(days);
    this->timeRemaining.hours = int(hours % 24);
    this->timeRemaining.minutes = int(minutes % 60);
    this->timeRemaining.seconds = int(seconds % 60);
    Serial.println(this->daysRemaining);
    Serial.println(this->timeRemaining.hours);
    Serial.println(this->timeRemaining.minutes);
    Serial.println(this->timeRemaining.seconds);
  }
};

struct OSStatus
{
  unsigned long runTime = millis();
  bool running = false;
  bool lowPressure = false;
  int keyStatus = 0;
  unsigned long safetyFreqTimer = 0;
  int safetyFreq = 0;
  bool alarmStatus = false;
  const char *ssid = "CBCI-CE6A-2.4_2GEXT";
  const char *password = "flower2351earth";

  int keyOffPin;
  int keyOnPin;
  int startButtonPin;
  int eStopPin;
  int eStopPin2;
  int pressurePin;

  bool eStop()
  {
    if (digitalRead(eStopPin) || digitalRead(eStopPin2))
    {
      return true;
    }
    return false;
  }

  int currentPressure()
  {
    return analogRead(pressurePin);
  }

  bool setRunning(bool runSetStatus)
  {
    running = runSetStatus;
    return running;
  }
};

#endif
