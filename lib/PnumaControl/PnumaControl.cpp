#include "PnumaControl.h"
#include "REPTestOS_menus_menu.h"

// #define E_STOP digitalRead(13)

PnumaControl pnuma1(_pnuma1PushPin, _pnuma1PullPin);

PnumaControl::PnumaControl(int pushIn, int pullIn)
{
    pushPin = pushIn; //constructor requires push and pull actuator pin #'s, set to private
    pullPin = pullIn;
}

PnumaControl::~PnumaControl()
{
}

void PnumaControl::setup()
{
    pinMode(pushPin, OUTPUT);
    pinMode(pullPin, OUTPUT);
    digitalWrite(pushPin, LOW);
    digitalWrite(pullPin, LOW);
}
void PnumaControl::control(testParams &testConditions, bool systemCheck)
{
    //check all hardware status, if error, exit with code, otherwise execute loop.

    if (systemCheck)
    {
        if (testConditions.cyclesRemaining > 0)
        {
            baseClock = millis();                                //update clock
            unsigned long timeDelta = baseClock - lastActuation; //find time difference between last actuation

            unsigned int freq = 1000 / cyclesPerSecond; //compare to desired frequency
            if (timeDelta > freq)
            {
                Serial.println("we're firing");
                setNextCycle();                   //update state based off of mode
                actuate();                        //fire updated actuation state
                lastActuation = millis();         //reset timer
                testConditions.cyclesRemaining--; //decrement cycles remaining.
                testConditions.cyclesExecuted++;  //increment cycles executed
                menuTotalCycles.setCurrentValue(testConditions.cyclesExecuted);
            }
        }
        if (testConditions.cyclesRemaining == 0)
        {
        }

        baseClock = millis(); //update clock
        unsigned int secondTimeDelta = baseClock - secondTimer;
        if (secondTimeDelta > 1000)
        {
            testConditions.decSecond();
            menuTimeRem.setTime(testConditions.timeRemaining);
            menuTimeRem.setChanged(true);
            secondTimer = millis(); //reset second timer
        }
    }
    // else if (!running)
    // {
    //     //handle machine in not running test mode
    // }
    else
    {
        this->status = off;
        this->actuate();
    }
};

/* Error Codes
    1 -- running set to false
*/
int PnumaControl::verifyStatus()
{
    //do a thing
    return 0;
}

ActuatorMode PnumaControl::setMode(bool pushEnable, bool pullEnable)
{
    if (pushEnable && pullEnable)
    {
        mode = pushAndPull;
        return mode;
    }
    else if (!pushEnable && !pullEnable)
    {
        mode = none;
        return mode;
    }
    else if (!pushEnable && pullEnable)
    {
        mode = pullOnly;
        return mode;
    }
    else if (pushEnable && !pullEnable)
    {
        mode = pushOnly;
        return mode;
    }
    else
    {
        mode = none;
        return mode;
    }
}

// bool PnumaControl::setRunning(bool run)
// {
//     running = run;
//     return running;
// }
void PnumaControl::setNextCycle()
{
    switch (mode)
    {
    case pushOnly:
        switch (status)
        {
        case off:
            status = out;
            break;
        case on:
            status = out;
            break;
        case in:
            status = out;
            break;
        case out:
            status = off;
            break;
        default:
            break;
        }
        break;
    case pullOnly:
        switch (status)
        {
        case off:
            status = in;
            break;
        case on:
            status = in;
            break;
        case in:
            status = off;
            break;
        case out:
            status = in;
            break;
        default:
            break;
        }
        break;
    case pushAndPull:
        switch (status)
        {
        case off:
            status = out;
            break;
        case on:
            status = out;
            break;
        case in:
            status = out;
            break;
        case out:
            status = in;
            break;
        default:
            break;
        }
        break;
    case none:
        switch (status)
        {
        case off:
            status = off;
            break;
        case on:
            status = off;
            break;
        case in:
            status = off;
            break;
        case out:
            status = off;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

ActuatorState PnumaControl::setState(ActuatorState setValue)
{
    status = setValue;
    return status;
}

int PnumaControl::setCyclesPerSecond(int setCyclesVal)
{
    cyclesPerSecond = setCyclesVal;
    return cyclesPerSecond;
}

int PnumaControl::getCyclesPerSecond()
{
    return cyclesPerSecond;
}
void PnumaControl::actuate()
{
    switch (status)
    {
    case off:
        digitalWrite(pushPin, LOW);
        digitalWrite(pullPin, LOW);
        break;
    case on:
        digitalWrite(pushPin, HIGH);
        digitalWrite(pullPin, HIGH);
        break;
    case in:
        digitalWrite(pushPin, LOW);
        digitalWrite(pullPin, HIGH);
        break;
    case out:
        digitalWrite(pushPin, HIGH);
        digitalWrite(pullPin, LOW);
        break;
    default:
        break;
    }
}

void PnumaControl::pushToggle(bool toggleValue)
{
    if (toggleValue)
    {
        digitalWrite(pushPin, HIGH);
    }
    else
    {
        digitalWrite(pushPin, LOW);
    }
}

void PnumaControl::pullToggle(bool toggleValue)
{
    if (toggleValue)
    {
        digitalWrite(pullPin, HIGH);
    }
    else
    {
        digitalWrite(pullPin, LOW);
    }
}