#include "REPTestOS.h"

//global variable definitions
Util util;
OSStatus repOS;
testParams currentTest;
WiFiServer server(80);

REPTestOS::REPTestOS(){};  //constructor
REPTestOS::~REPTestOS(){}; //destructor

void REPTestOS::bootOS() //boot operating system function. To run in startup
{
  Wire.begin();              //Start wire
  setupMenu();               //Initialize menu system
  Serial.begin(9600);        // initialize serial communication at 9600 bits per second:
  Serial.println("Booting"); //small serial indicator during boot phase
  pnuma1.setup();            //setup function for pins for pnuma 1
  setupControls();           //setup function for this OS to configure pins
  //Start remaining items
  menuTimeRem.setTime(currentTest.timeRemaining); //initialize timeRemaining as 00:00:00
  menuTimeRem.setChanged(true);
  currentTest.minPressure = 0;
  repOS.safetyFreq = 50; //set update frequency to check for estop etc.
  //initWifi();            //run WiFi Connection attempts
}

void REPTestOS::runOS() //constantly runs in loop
{
  repOS.runTime = millis();
  taskManager.runLoop(); //handler for menu system task management
  if ((repOS.runTime - repOS.safetyFreqTimer) >= repOS.safetyFreq)
  {
    repOS.alarmStatus = systemCheck();
    repOS.safetyFreqTimer = repOS.runTime;
  }
  pnuma1.control(currentTest, repOS.alarmStatus);

  if (digitalRead(_startButtonPin))
    repOS.setRunning(true);

  //ArduinoOTA.handle();
}

bool REPTestOS::systemCheck()
{
  //this function should return true if all safety measures are good. Otherwise false, so machine does not run.
  //check key, eStop, pressure, and "Running Toggle"
  if (getKeyState() == 2)
  {
    if (!repOS.eStop())
    {
      if (repOS.currentPressure() >= currentTest.minPressure)
      {
        if (repOS.running)
        {
          return true;
        }
        //Serial.println("Running set false");
      }
      //Serial.println("Pressure reading to Low");
    }
    //Serial.println("E-STOP");
  }
  //Serial.println("key State not on");
  repOS.running = false;
  return false;
}

int REPTestOS::getKeyState()
{
  bool offPin = digitalRead(repOS.keyOffPin);
  bool onPin = digitalRead(repOS.keyOnPin);

  if (!offPin && !onPin)
  {
    return 1; //if neither pin is grounded, switch is neutral
  }
  else if (offPin && !onPin)
  {
    return 0; //if off pin is grounded, switch is OFF
  }
  else if (!offPin && onPin)
  {
    return 2; //if on pin is grounded, switch is on
  }
  else
  {
    return 0; //default to return 0 (off) for safety
  }

}; //function to check current key state.

void REPTestOS::setupControls()
{
  repOS.keyOffPin = _keyOffPin;           //set keyOffPin
  repOS.keyOnPin = _keyOnPin;             //set keyOnPin
  repOS.startButtonPin = _startButtonPin; //set startuButtonPin
  repOS.eStopPin = _eStopPin;             //set eStopPin
  repOS.eStopPin2 = _eStopPin2;           //set eStopPin
  repOS.pressurePin = _pressurePin;       //set pressurePin

  pinMode(repOS.keyOffPin, INPUT_PULLUP);
  pinMode(repOS.keyOnPin, INPUT_PULLUP);
  pinMode(repOS.startButtonPin, INPUT_PULLUP);
  pinMode(repOS.eStopPin, INPUT_PULLUP);
  pinMode(repOS.eStopPin2, INPUT_PULLUP);
  pinMode(repOS.pressurePin, INPUT); //analog input

  digitalWrite(repOS.keyOffPin, HIGH);
  digitalWrite(repOS.keyOnPin, HIGH);
  digitalWrite(repOS.startButtonPin, HIGH);
  digitalWrite(repOS.eStopPin, HIGH);
  digitalWrite(repOS.eStopPin2, HIGH);
}

void REPTestOS::initWifi()
{
  int timesToTry = 8;
  int attempts = 0;
  Serial.print("Connecting to: ");
  Serial.println(repOS.ssid);
  WiFi.begin(repOS.ssid, repOS.password);

  while (WiFi.status() != WL_CONNECTED)
  {
    while (attempts <= timesToTry)
    {
      delay(500);
      Serial.print(".");
      attempts++;
    }
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    // Serial.print("Attempts: ");
    // Serial.println(attempts);
    //server.begin();

    // Port defaults to 3232
    // ArduinoOTA.setPort(3232);

    // Hostname defaults to esp3232-[MAC]
    // ArduinoOTA.setHostname("myesp32");

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA
        .onStart([]()
                 {
                   String type;
                   if (ArduinoOTA.getCommand() == U_FLASH)
                     type = "sketch";
                   else // U_SPIFFS
                     type = "filesystem";

                   // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
                   Serial.println("Start updating " + type);
                 })
        .onEnd([]()
               { Serial.println("\nEnd"); })
        .onProgress([](unsigned int progress, unsigned int total)
                    { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
        .onError([](ota_error_t error)
                 {
                   Serial.printf("Error[%u]: ", error);
                   if (error == OTA_AUTH_ERROR)
                     Serial.println("Auth Failed");
                   else if (error == OTA_BEGIN_ERROR)
                     Serial.println("Begin Failed");
                   else if (error == OTA_CONNECT_ERROR)
                     Serial.println("Connect Failed");
                   else if (error == OTA_RECEIVE_ERROR)
                     Serial.println("Receive Failed");
                   else if (error == OTA_END_ERROR)
                     Serial.println("End Failed");
                 });

    ArduinoOTA.begin();
  }
  else
  {
    Serial.println("");
    Serial.println("WiFi connection Failed.");
    Serial.println("Returned WiFi ");
    Serial.println(WiFi.status());
  }
}

/*----------------------------------------------------------
------------------------------------------------------------
----------------- Menu System Handlers ---------------------
------------------------------------------------------------
----------------------------------------------------------*/

void CALLBACK_FUNCTION cycleFrequency(int id)
{
  int val = pnuma1.setCyclesPerSecond(int(menuTestSettingsCyclesPerSecond.getAsFloatingPointValue())); //update pnuma1 with value
  Serial.println(val);
}

void CALLBACK_FUNCTION pullControl(int id)
{
  ActuatorMode mode = pnuma1.setMode(menuTestSettingsPush.getBoolean(), menuTestSettingsPull.getBoolean()); //update pnuma1 with mode
  Serial.println(mode);
}

void CALLBACK_FUNCTION pushControl(int id)
{
  ActuatorMode mode = pnuma1.setMode(menuTestSettingsPush.getBoolean(), menuTestSettingsPull.getBoolean()); //update pnuma1 with mode
  Serial.println(mode);
}

void CALLBACK_FUNCTION resetTest(int id)
{
  repOS.setRunning(false);                    //stop test cycling
  menuTimeRem.setTime(currentTest.totalTime); //reset countdown
  menuTimeRem.setChanged(true);
  currentTest.timeRemaining = currentTest.totalTime;
  currentTest.cyclesRemaining = currentTest.totalCycles;
  menuTimeRem.setTime(currentTest.timeRemaining);
  pnuma1.setState(off);
  pnuma1.actuate();
}

void CALLBACK_FUNCTION maxCycles(int id)
{
  LargeFixedNumber *maxCyclesVal = menuTestSettingsMaxCycles.getLargeNumber(); //sets the max number of cycles in test structure
  currentTest.totalCycles = currentTest.cyclesRemaining = maxCyclesVal->getAsFloat();
  currentTest.setBySeconds(currentTest.totalCycles / pnuma1.getCyclesPerSecond());
  // Serial.println(currentTest.cyclesRemaining);
  menuTimeRem.setTime(currentTest.timeRemaining);
  currentTest.totalTime = currentTest.timeRemaining;
}

void CALLBACK_FUNCTION resetTotalCycles(int id)
{
  currentTest.cyclesExecuted = 0;
  menuTotalCycles.setCurrentValue(currentTest.cyclesExecuted);
  menuTotalCycles.setChanged(true);
}

/*----------------------------------------------------------
------------------------------------------------------------
---------------------- Functions ---------------------------
------------------------------------------------------------
----------------------------------------------------------*/

//use itoa(int, array, 10) to convert int to const char*
//from bool to const char* menuTotalCycles.setTextValue(util.returnCharPFromBool(menuTestSettingsPull.getBoolean()));
