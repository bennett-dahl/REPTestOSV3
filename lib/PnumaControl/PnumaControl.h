#ifndef PNUMACONTROL_H
#define PNUMACONTROL_H

#include "Arduino.h"
#include "Structs.h"
#include "../Util/config.h"

enum ActuatorState
{
  off,
  on,
  in,
  out
}; //enum decleration of actuator state

enum ActuatorMode
{
  pushOnly,
  pullOnly,
  pushAndPull,
  none
}; //enum decleration of actuator mode

class PnumaControl
{
private:
  // bool push = false; //boolean for push enable
  // bool pull = false; //boolean for pull enable
  int pushPin; //arduino pin for push solenoid
  int pullPin; //arduino pin for pull solenoid

  //bool running = false; //trigger for actuation
  //int keyStatus;        //holds key position, 0 off (left), 1 Neutral (middle), 2 on (right)
  //bool eStop = false;   //eStop loop state

  int cyclesPerSecond;        //rate of actuation
  ActuatorState status = off; //used for tracking actuator state
  ActuatorMode mode = none;   //used for tracking actuation mode

  unsigned long baseClock;         //timer
  unsigned long lastActuation = 0; //time of last actuation
  unsigned long secondTimer = 0;

  int verifyStatus(); //functrion to check all hardware status, returns error code.

public:
  PnumaControl(int pushIn, int pullIn);
  ~PnumaControl();

  void setup();                                           //function to set pins of object on boot.
  void control(testParams &, bool systemCheck);           //primary control handler, must run in loop
  ActuatorMode setMode(bool pushEnable, bool pullEnable); //sets ActuatorMode based off of parameters
  ActuatorState setState(ActuatorState);                  //sets ActuatorState
  void setNextCycle();                                    //sets the state of the actuator's next movement based on push/pull enables and current state
  void actuate();                                         //handles actuator state logic to fire actuator as needed
  // bool setRunning(bool);                                  //updates private "running" value
  void pushToggle(bool);       //enables push toggle
  void pullToggle(bool);       //enables pull toggle
  int setCyclesPerSecond(int); //set then return cycles/second
  int getCyclesPerSecond();    //return cyclesPerSecond
};

extern PnumaControl pnuma1;

#endif