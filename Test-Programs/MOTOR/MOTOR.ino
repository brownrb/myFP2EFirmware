// ----------------------------------------------------------------------------------------------
// TITLE: myFP2ESP FIRMWARE OFFICIAL RELEASE 119
// ----------------------------------------------------------------------------------------------
// myFP2ESP - Firmware for ESP8266 and ESP32 myFocuserPro2 Controllers
// Supports driver boards DRV8825, ULN2003, L298N, L9110S, L293DMINI
// ESP8266 Supports OLED display, Temperature Probe
// ESP32 Supports OLED display, Temperature Probe, Push Buttons, Direction LED's, Infrared Remote, Bluetooth
// Supports modes, ACCESSPOINT, STATIONMODE, BLUETOOTH, LOCALSERIAL, WEBSERVER, ASCOMREMOTE
// Remember to change your target CPU depending on board selection

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2020. All Rights Reserved.
// (c) Copyright Holger M, 2019-2020. All Rights Reserved.
// (c) Copyright Pieter P - OTA code and SPIFFs file handling/upload based on examples

// ----------------------------------------------------------------------------------------------
// SPECIAL LICENSE
// ----------------------------------------------------------------------------------------------
// This code is released under license. If you copy or write new code based on the code in these files
// you MUST include to link to these files AND you MUST include references to the authors of this code.

// ----------------------------------------------------------------------------------------------
// CONTRIBUTIONS
// ----------------------------------------------------------------------------------------------
// It is costly to continue development and purchase boards and components. We need your help to
// continue development of this project. Please make a contribution in thanks for this project,
// and use PayPal to send the amount to user rbb1brown@gmail.com (Robert Brown).
// All contributions are gratefully accepted.

#include "generalDefinitions.h"
#include "myBoards.h"
#include <ArduinoJson.h>


//  StateMachine definition
enum StateMachineStates
{
  State_Idle,
  State_InitMove,
  State_ApplyBacklash,
  State_ApplyBacklash2,
  State_Moving,
  State_DelayAfterMove,
  State_FinishedMove,
  State_SetHomePosition,
  State_FindHomePosition
};

//           reversedirection
//__________________________________
//               0   |   1
//__________________________________
//moving_out  1||  1   |   0
//moving_in   0||  0   |   1

const char* programName = DRVBRD_ID;
DriverBoard* driverboard;

unsigned long fcurrentPosition;             // current focuser position
unsigned long ftargetPosition;              // target position
unsigned long tmppos;
boolean  displayfound;

byte   tprobe1;                               // indicate if there is a probe attached to myFocuserPro2
bool   isMoving;                              // is the motor currently moving




// ----------------------------------------------------------------------------------------------
// 30: FIRMWARE - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
byte TimeCheck(unsigned long x, unsigned long Delay)
{
  unsigned long y = x + Delay;
  unsigned long z = millis();                           // pick current time

  if ((x > y) && (x < z))
    return 0;                                           // overflow y
  if ((x < y) && ( x > z))
    return 1;                                           // overflow z

  return (y < z);                                       // no or (z and y) overflow
}


//_______________________________________________ setup()

void setup()
{

  Serial.begin(SERIALPORTSPEED);
  DebugPrintln(SERIALSTARTSTR);
  DebugPrintln(DEBUGONSTR);

  driverboard = new DriverBoard(DRVBRD);
  driverboard->setmotorspeed(FAST);  // restore motorspeed
  driverboard->setstepmode(STEP8);

  isMoving = 0;
}

//_____________________ loop()___________________________________________

//void IRAM_ATTR loop() // ESP32
void loop()
{
  static StateMachineStates MainStateMachine = State_Idle;
  static int count = 0;
  static bool DirOfTravel = moving_in;
  static unsigned long TimeStampDelayAfterMove = 0;
  static unsigned long TimeStampPark = millis();


  //_____________________________MainMachine _____________________________


//    case State_InitMove:
  switch (MainStateMachine)
  {

    case State_Idle:
      isMoving = 1;
      DirOfTravel = (DirOfTravel == moving_in) ? moving_out : moving_in;
      
      driverboard->inittravel(DirOfTravel, 200 * 8);  // one turn in Step8 mode
      MainStateMachine = State_Moving; 
      break;


    case State_Moving:
      if (driverboard->movemotor() == false)
      {
        TimeStampDelayAfterMove = millis();
        MainStateMachine = State_DelayAfterMove;
        DebugPrintln(F(STATEDELAYAFTERMOVE));
      }
    break;


    case State_DelayAfterMove:
      // apply Delayaftermove, this MUST be done here in order to get accurate timing for DelayAfterMove
      if (TimeCheck(TimeStampDelayAfterMove , 1000))
      {
        TimeStampPark  = millis();                      // catch current time
        MainStateMachine = State_FinishedMove;
        DebugPrintln(F(STATEFINISHEDMOVE));
      }
      break;

    case State_FinishedMove:
      isMoving = 0;
      // coil power is turned off after MotorReleaseDelay expired and Parked==true, see State_Idle

      MainStateMachine = State_Idle;
      DebugPrintln(F(STATEIDLE));
      break;

    default:
      DebugPrintln(F("Error: wrong State => State_Idle"));
      MainStateMachine = State_Idle;
      break;
  }
} // end Loop()
