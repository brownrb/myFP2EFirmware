// myFP2ESP - Firmware for ESP8266 and ESP32 myFocuserPro2 Controllers
// Supports driver boards DRV8825, ULN2003, L298N, L9110S, L293DMINI
// ESP8266 Supports OLED display, Temperature Probe
// ESP32 Supports OLED display, Temperature Probe, Push Buttons, Direction LED's. Infrared Remote
// Remember to change your target CPU depending on board selection
//
// (c) Copyright Robert Brown 2014-2019. All Rights Reserved.
// (c) Copyright Holger M, 2019, who wrote large portions of code for state machine and esp comms
//
// CONTRIBUTIONS
// If you wish to make a small contribution in thanks for this project, please use PayPal and send the amount
// to user rbb1brown@gmail.com (Robert Brown). All contributions are gratefully accepted.
//
// 1. Set your DRVBRD [section 1] in this file so the correct driver board is used
// 2. Set your chipmodel in chipModels.h so that pins are mapped correctly
// 3. Set your target CPU to match the chipModel you defined
// 4. Set the correct hardware options [section 4] in this file to match your hardware
// 5. Compile and upload to your controller
//
// ----------------------------------------------------------------------------------------------------------
// PCB BOARDS
// ----------------------------------------------------------------------------------------------------------
// ESP8266
//    ULN2003    https://aisler.net/p/QVXMBSWW
//    DRV8825    https://aisler.net/p/QVXMBSWW
// ESP32
//    ULN2003
//    DRV8825
//
// ----------------------------------------------------------------------------------------------
// 1: SPECIFY DRIVER BOARD HERE
// ----------------------------------------------------------------------------------------------
// DRIVER BOARDS - Please specify your driver board here, only 1 can be defined, see DRVBRD line
#include "myBoardTypes.h"

//Set DRVBRD to the correct driver board above, ONLY ONE!!!!
#define DRVBRD PRO2EDRV8825
//#define DRVBRD PRO2EULN2003
//#define DRVBRD PRO2EL298N
//#define DRVBRD PRO2EL293DMINI
//#define DRVBRD PRO2EL9110S
//#define DRVBRD PRO2ESP32DRV8825
//#define DRVBRD PRO2ESP32ULN2003
//#define DRVBRD PRO2ESP32L298N
//#define DRVBRD PRO2ESP32L293DMINI
//#define DRVBRD PRO2ESP32L9110S

// FOR ESP8266 DRV8825 YOU MUST CHANGE DRV8825TEPMODE TO MATCH MS1/2/3 JUMPERS ON PCB
// YOU DO THIS IN myBoards.h file

// DO NOT CHANGE
#ifndef DRVBRD    // error checking, please do NOT change
#halt // ERROR you must have DRVBRD defined
#endif

// DO NOT CHANGE - Order is very important
#include "chipModels.h"             // include chip definitions
#include "hardwarePins.h"           // include pin mappings for temp probe etc
#include "myBoards.h"               // include mappings for driver boards and driver board routines

// ----------------------------------------------------------------------------------------------
// 2: SPECIFY STEPPER MOTOR HERE
// ----------------------------------------------------------------------------------------------
// ONLY NEEDED FOR L293D MOTOR SHIELD - ALL OTHER BOARDS PLEASE IGNORE

// ----------------------------------------------------------------------------------------------
// 3: SPECIFY ESP32/ESP8266 CHIP TYPE
// ----------------------------------------------------------------------------------------------
// Remember to set CHIPMODEL to the correct chip you using in chipModels.h

// For ESP8266, remember to set DRV8825TEPMODE to the correct value if using WEMOS or NODEMCUV1 in myBoards.h

// ----------------------------------------------------------------------------------------------------------
// 4: SPECIFY HARDWARE OPTIONS HERE
// ----------------------------------------------------------------------------------------------------------
// Caution: Do not enable a feature if you have not added the associated hardware circuits to support that feature
// Enable or disable the specific hardware below

// To enable temperature probe, uncomment the next line
#define TEMPERATUREPROBE 1

// To enable the OLED DISPLAY uncomment the next line
#define OLEDDISPLAY 1

// To enable backlash in this firmware, uncomment the next line
#define BACKLASH 1

// To enable In and Out Pushbuttons in this firmware, uncomment the next line [ESP32 only]
//#define INOUTPUSHBUTTONS 1

// To enable In and Out LEDS in this firmware, uncomment the next line [ESP32 only]
//#define INOUTLEDS 1

// To enable the Infrared remote controller, uncomment the next line [ESP32 only]
//#define INFRAREDREMOTE

// To enable the start boot screen showing startup messages, uncomment the next line
#define SHOWSTARTSCRN 1

// DO NOT CHANGE
#if (DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N \
  || DRVBRD == PRO2EL293DMINI || DSRVBRD == PRO2EL9110S)
// no support for pushbuttons, inout leds, irremote
#ifdef INOUTPUSHBUTTONS
#halt // ERROR - INOUTPUSHBUTTONS not supported for WEMOS or NODEMCUV1 ESP8266 chips
#endif
#ifdef INOUTLEDS
#halt // ERROR - INOUTLEDS not supported for WEMOS or NODEMCUV1 ESP8266 chips
#endif
#ifdef INFRAREDREMOTE
#halt // ERROR - INFRAREDREMOTE not supported for WEMOS or NODEMCUV1 ESP8266 chips
#endif
#endif // 

// ----------------------------------------------------------------------------------------------
// 5: SPECIFY THE TYPE OF OLED DISPLAY HERE
// ----------------------------------------------------------------------------------------------

//#define OLEDGRAPHICS 1
#define OLEDTEXT 2

// DO NOT CHANGE
#ifndef OLEDGRAPHICS
#ifndef OLEDTEXT
#halt //ERROR - you must have either OLEDGRAPHICS or OLEDTEXT defined
#endif
#endif

#ifdef OLEDGRAPHICS
#ifdef OLEDTEXT
#halt //ERROR - you must have either OLEDGRAPHICS or OLEDTEXT defined, not both
#endif
#endif

// ----------------------------------------------------------------------------------------------
// 6: SPECIFY THE CONTROLLER MODE HERE - ONLY ONE OF THESE MUST BE DEFINED
// ----------------------------------------------------------------------------------------------

// to enable Bluetooth mode, uncomment the next line [ESP32 only]
//#define BLUETOOTHMODE 1

// to work as an access point, define accesspoint - cannot use DUCKDNS
#define ACCESSPOINT 1

// to work as a station accessing a AP, define stationmode
//#define STATIONMODE 1

// DO NOT CHANGE
#ifdef ACCESSPOINT
#ifdef STATIONMODE
#halt // ERROR - Cannot have both ACCESSPOINT and STATIONMODE defined at the same time
#endif
#endif

#if (CHIPMODEL == WEMOS || CHIPMODEL == NODEMCUV1)
// no support for bluetooth mode
#ifdef BLUETOOTHMODE
#halt // ERROR - BLUETOOTHMODE not supported for WEMOS or NODEMCUV1 ESP8266 chips
#endif
#endif

// ----------------------------------------------------------------------------------------------
// 7. DEBUGGING                                       // do not change - leave this commented out
// ----------------------------------------------------------------------------------------------
//#define DEBUG     1

#ifdef DEBUG                                          //Macros are usually in all capital letters.
#define DebugPrint(...) Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
#define DebugPrintln(...) Serial.println(__VA_ARGS__) //DPRINTLN is a macro, debug print with new line
#else
#define DebugPrint(...)                               //now defines a blank line
#define DebugPrintln(...)                             //now defines a blank line
#endif

// ----------------------------------------------------------------------------------------------
// 8. INCLUDES FOR WIFI
// ----------------------------------------------------------------------------------------------
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include "FocuserSetupData.h"
// WIFI STUFF + SPIFFS => differed includes on differend boards
#if defined(ESP8266)                        // this "define(ESP8266)" comes from Arduino IDE automatic 
#include <ESP8266WiFi.h>
#include <FS.h>                             // Include the SPIFFS library  
#else
#include <WiFi.h>
#include "SPIFFS.h"
#endif

#include <WiFiServer.h>

// ----------------------------------------------------------------------------------------------------------
// 9. WIFI NETWORK SSID AND PASSWORD CONFIGURATION
// ----------------------------------------------------------------------------------------------------------
// 1. For access point mode this is the network you connect to
// 2. For station mode, change these to match your network details
const char* mySSID = "myfp2eap";
const char* myPASSWORD = "myfp2eap";

// ----------------------------------------------------------------------------------------------------------
// 10. DUCKDNS DOMAIN AND TOKEN CONFIGURATION
// ----------------------------------------------------------------------------------------------
// To use DucksDNS, uncomment the next line - can only be used together with STATIONMODE
//#define USEDUCKSDNS 1

// if using DuckDNS you need to set these next two parameters, and enable the define statement for USEDUCKSDNS
// cannot use DuckDNS with ACCESSPOINT mode
#ifdef DUCKDNS
const char* duckdnsdomain = "myfp2erobert.duckdns.org";
const char* duckdnstoken = "0a0379d5-3979-44ae-b1e2-6c371a4fe9bf";
#endif

// DO NOT CHANGE
#ifdef USEDUCKSDNS
#include <EasyDDNS.h>             // https://github.com/ayushsharma82/EasyDDNS
#endif

#ifdef ACCESSPOINT
#ifdef USEDUCKSDNS
#halt error- you cannot have ACCESSPOINT and STATIONMODE both defined - only one of then
#endif
#endif

// ----------------------------------------------------------------------------------------------------------
// 11. STATIC IP ADDRESS CONFIGURATION
// ----------------------------------------------------------------------------------------------
// must use static IP if using duckdns or as an Access Point
#define STATICIPON    1
#define STATICIPOFF   0
int staticip = STATICIPON;                    // if set to STATICIPON then the IP address must be defined

#ifdef STATIONMODE
// These need to reflect your current network settings - 192.168.x.21 - change x
// This has to be within the range for dynamic ip allocation in the router
// No effect if staticip = STATICIPOFF
IPAddress ip(192, 168, 2, 21);                // station static IP - you can change these values to change the IP
IPAddress dns(192, 168, 2, 1);                // just set it to the same IP as the gateway
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);
#endif

#ifdef ACCESSPOINT
// By default the Access point should be 192.168.4.1 - DO NOT CHANGE
IPAddress ip(192, 168, 4, 1);                 // AP static IP - you can change these values to change the IP
IPAddress dns(192, 168, 4, 1);                // just set it to the same IP as the gateway
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);
#endif

// ----------------------------------------------------------------------------------------------------------
// 12. FIRMWARE CODE START - INCLUDES AND LIBRARIES
// ----------------------------------------------------------------------------------------------------------
// Compile this with Arduino IDE 1.8.9 with ESP8266 Core library installed v2.5.2 [for ESP8266]
// Make sure target board is set to Node MCU 1.0 (ESP12-E Module) [for ESP8266]

// Project specific includes
#ifdef BLUETOOTHMODE
#include "ESPQueue.h"                       //  By Steven de Salas
#endif

#include <Wire.h>                           // needed for I2C

#ifdef TEMPERATUREPROBE
#include <OneWire.h>                        // https://github.com/PaulStoffregen/OneWire
#include <DallasTemperature.h>              // https://github.com/milesburton/Arduino-Temperature-Control-Library
#endif
#ifdef OLEDDISPLAY
#ifdef OLEDGRAPHICS
#include "SSD1306Wire.h"                    // TODO Holger need to put url of library here
#include "images.h"                         // TODO Holger need to provide file
#endif
#ifdef OLEDTEXT
#include <mySSD1306Ascii.h>
#include <mySSD1306AsciiWire.h>
#endif
#endif
#if (CHIPMODEL == ESP32VROOM)
#ifdef INFRAREDREMOTE
#include <IRremoteESP32.h>
#endif
#endif

// ----------------------------------------------------------------------------------------------------------
// 13. BLUETOOTH MODE - Do not change
// ----------------------------------------------------------------------------------------------------------
#ifdef BLUETOOTHMODE
#include "BluetoothSerial.h"                // needed for Bluetooth comms
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
#endif

#ifdef BLUETOOTHMODE
String btline                               // buffer for serial data
String BLUETOOTHNAME = "MYFP3ESP32BT";      // default name for Bluetooth controller, this name you can change
BluetoothSerial SerialBT;                   // define BT adapter to use
#endif // BLUETOOTHMODE

// ----------------------------------------------------------------------------------------------------------
// 14. GLOBAL DEFINES YOU CAN CHANGE - THESE CAN ONLY BE CONFIGURED AT COMPILE TIME
// ----------------------------------------------------------------------------------------------------------
// CHANGE AT YOUR PERIL

#define SERVERPORT          2020
#define TEMPREFRESHRATE     2000L         // refresh rate between temperature conversions unless an update is requested via serial command
#define SERIALPORTSPEED     115200        // 9600, 14400, 19200, 28800, 38400, 57600, 115200

#define EEPROMSIZE          512           // use 512 locations 
#define EEPROMWRITEINTERVAL 15000         // interval in milliseconds to wait after a move before writing settings to EEPROM, 15s
#define VALIDDATAFLAG       99            // 01100011 valid eeprom data flag
#define LCDUPDATEONMOVE     15            // defines how many steps before refreshing position when moving if lcdupdateonmove is 1
#define TEMP_PRECISION      10            // Set the default DS18B20 precision to 0.25 of a degree 9=0.5, 10=0.25, 11=0.125, 12=0.0625
#define MOTORSPEEDCHANGETHRESHOLD   200
#define MOVINGIN            0             // moving focuser inwards towards 0 or home
#define MOVINGOUT           1             // moving focuser outwars towards maxstep
#define FOCUSERUPPERLIMIT   2000000000L   // arbitary focuser limit up to 2000000000
#define FOCUSERLOWERLIMIT   1024L         // lowest value that maxsteps can be
#define DEFAULTSTEPSIZE     50.0          // This is the default setting for the step size in microns
#define LCDPAGETIMEMIN      2             // 2s minimum lcd page display time
#define LCDPAGETIMEMAX      10            // 10s maximum lcd page display time

#define ESPDATA             0
#define BTDATA              1
#define QUEUELENGTH         20            // number of commands that can be saved in the serial queue

// ----------------------------------------------------------------------------------------------------------
// 15. GLOBAL DATA -- DO NOT CHANGE
// ----------------------------------------------------------------------------------------------------------

//  StateMachine definition
#define State_Idle            0
#define State_InitMove        1
#define State_ApplyBacklash   2
#define State_Moving          3
#define State_DelayAfterMove  4
#define State_FinishedMove    5
#define move_in               0
#define move_out              1
//           reversedirection
//__________________________________
//               0   |   1
//__________________________________
//move_out  1||  1   |   0
//move_in   0||  0   |   1

#ifdef DRVBRD
#if( DRVBRD == PRO2EDRV8825)
char programName[]  = "myFP2E.DRV8825";
#endif
#if( DRVBRD == PRO2ESP32DRV8825)
char programName[]  = "myFP2ESP32.DRV8825";
#endif
#if( DRVBRD == PRO2EULN2003)
char programName[]  = "myFP2E.ULN2003";
#endif
#if( DRVBRD == PRO2ESP32ULN2003)
char programName[]  = "myFP2ESP32.ULN2003";
#endif
#if( DRVBRD == PRO2EL298N)
char programName[]  = "myFP2E.L298N";
#endif
#if( DRVBRD == PRO2ESP32L298N)
char programName[]  = "myFP2ESP32.L298N";
#endif
#if( DRVBRD == PRO2EL293DMINI)
char programName[]  = "myFP2E.L293DMINI";
#endif
#if( DRVBRD == PRO2ESP32L293DMINI)
char programName[]  = "myFP2ESP32.L293DMINI";
#endif
#if( DRVBRD == PRO2EL9110S)
char programName[]  = "myFP2E.L9110S";
#endif
#if( DRVBRD == PRO2ESP32L9110S)
char programName[]  = "myFP2ESP32.L9110S";
#endif
DriverBoard* driverboard;
#endif

char programVersion[] = "228";
char ProgramAuthor[]  = "(c) R BROWN 2018";
char ontxt[]          = "ON ";
char offtxt[]         = "OFF";
char coilpwrtxt[]     = "Coil power  =";
char revdirtxt[]      = "Reverse Dir =";

unsigned long fcurrentPosition;          // current focuser position
unsigned long ftargetPosition;           // target position
unsigned long tmppos;

byte tprobe1;                           // indicate if there is a probe attached to myFocuserPro2
byte isMoving;                          // is the motor currently moving
byte motorspeedchangethresholdsteps;    // step number where when pos close to target motor speed changes
byte motorspeedchangethresholdenabled;  // used to enable/disable motorspeedchange when close to target position

#ifdef BLUETOOTHMODE
Queue queue(QUEUELENGTH);               // receive serial queue of commands
String line;                            // buffer for serial data
#endif

#ifdef OLEDDISPLAY
#ifdef OLEDGRAPHICS
SSD1306* myoled;
#endif
#ifdef OLEDTEXT
SSD1306AsciiWire* myoled;
#endif
#endif

#ifdef TEMPERATUREPROBE
OneWire oneWirech1(TEMPPIN);            // setup temperature probe
DallasTemperature sensor1(&oneWirech1);
DeviceAddress tpAddress;                // holds address of the temperature probe
#endif

#ifndef BLUETOOTHMODE                   // WiFi stuff
IPAddress ESP32IPAddress;
String ServerLocalIP;
WiFiServer myserver(SERVERPORT);
int status = WL_IDLE_STATUS;
boolean ClientConnected = 0;
boolean LastClientConnected = 0;
WiFiClient myclient;                    // only one client supported, multiple connections denied
IPAddress myIP;
String ipStr;
long rssi;
#endif // ifndef BLUETOOTHMODE

#ifdef INFRAREDREMOTE
IRrecv irrecv(IRPIN);
decode_results results;
#endif

int packetsreceived;
int packetssent;

SetupData *mySetupData;

// ----------------------------------------------------------------------------------------------------------
// 16. CODE START - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------------------

void software_Reboot()
{
#ifdef OLEDDISPLAY
  myoled->clear();
  myoled->print("Controller reboot");
#endif
  if ( myclient.connected() )
    myclient.stop();
  delay(1000);
  ESP.restart();
}

// STEPPER MOTOR ROUTINES
void setstepperspeed( byte spd )
{
  driverboard->setmotorspeed(spd);
}

void setsteppermode(byte smode)
{
  driverboard->setstepmode(smode);
}

void enablesteppermotor(void)
{
  driverboard->enablemotor();
}

void releasesteppermotor(void)
{
  driverboard->releasemotor();
}

void steppermotormove(byte dir )           // direction move_in, move_out ^ reverse direction
{
  driverboard->movemotor(dir);
}

// TEMPERATURE PROBE ROUTINES
#ifdef TEMPERATUREPROBE
float readtemp(byte new_measurement)
{
  static float lasttemp = 20.0;                 // start temp value
  if (!new_measurement)
    return lasttemp;                            // return latest measurement

  float result = sensor1.getTempCByIndex(0);    // get channel 1 temperature, always in celsius
  DebugPrint(F("Temperature = "));
  DebugPrintln(result);
  if (result > -40.0 && result < 80.0)
    lasttemp = result;
  else
    result = lasttemp;
  return result;
}

void settempprobeprecision(byte precision)
{
  sensor1.setResolution(tpAddress, precision); // set probe resolution, tpAddress must be global
}

// find the address of the DS18B20 sensor probe
byte findds18b20address()
{
  // look for probes, search the wire for address
  DebugPrintln(F("Searching for temperature probe"));
  if (sensor1.getAddress(tpAddress, 0))
  {
    DebugPrint(F("Temperature probe address found"));
    tprobe1 = 1;
  }
  else
  {
    DebugPrintln(F("Temperature probe NOT found"));
    tprobe1 = 0;
  }
  return tprobe1;
}

void Update_Temp(void)
{
  static byte tcchanged = mySetupData->get_tempcompenabled();  // keeps track if tempcompenabled changes

  if (tprobe1 == 1)
  {
    static unsigned long lasttempconversion = 0;
    static byte requesttempflag = 0;              // start with request
    unsigned long tempnow = millis();

    // see if the temperature needs updating - done automatically every 1.5s
    if (((tempnow - lasttempconversion) > TEMPREFRESHRATE) || (tempnow < lasttempconversion))
    {
      static float tempval;
      static float starttemp;                     // start temperature to use when temperature compensation is enabled

      if ( tcchanged != mySetupData->get_tempcompenabled() )
      {
        tcchanged = mySetupData->get_tempcompenabled();
        if ( tcchanged == 1 )
        {
          starttemp = readtemp(1);
        }
      }

      lasttempconversion = tempnow;               // update time stamp

      if (requesttempflag)
      {
        tempval = readtemp(1);
      }
      else
      {
        sensor1.requestTemperatures();
      }

      requesttempflag ^= 1; // toggle flag

      if (mySetupData->get_tempcompenabled() == 1)     // check for temperature compensation
      {
        if ((abs)(starttemp - tempval) >= 1)          // calculate if temp has moved by more than 1 degree
        {
          unsigned long newPos;
          byte temperaturedirection;                  // did temperature fall (1) or rise (0)?
          temperaturedirection = (tempval < starttemp) ? 1 : 0;
          if (mySetupData->get_tcdirection() == 0)     // check if tc direction for compensation is inwards
          {
            // temperature compensation direction is in, if a fall then move in else move out
            if ( temperaturedirection == 1 )          // check if temperature is falling
            { // then move inwards
              newPos = ftargetPosition - mySetupData->get_tempcoefficient();
            }
            else
            { // else move outwards
              newPos = ftargetPosition + mySetupData->get_tempcoefficient();
            }
          }
          else
          {
            // temperature compensation direction is out, if a fall then move out else move in
            if ( temperaturedirection == 1 )
            {
              newPos = ftargetPosition + mySetupData->get_tempcoefficient();
            }
            else
            {
              newPos = ftargetPosition - mySetupData->get_tempcoefficient();
            }
          }
          newPos = (newPos > mySetupData->get_maxstep()) ? mySetupData->get_maxstep() : newPos;
          // newPos should be checked for < 0 but cannot due to unsigned
          // newPos = (newPos < 0 ) ? 0 : newPos;
          ftargetPosition = newPos;
          starttemp = tempval;                        // save this current temp point for future reference
        } // end of check for tempchange >=1
      } // end of check for tempcomp enabled
    } // end of check for temperature needs updating
  } // end of if tprobe
}
#endif // TEMPERATUREPROBE

// OLED DISPLAY ROUTINES
#ifdef OLEDDISPLAY
#ifdef OLEDGRAPHICS
// TODO Holger to fill in routines for graphics display
// called by update_oledgraphics()

void Update_OledGraphics(void)
{
  // TODO Holger
}
#endif // oledgraphics

#ifdef OLEDTEXT
void displaylcdpage0(void)      // displaylcd screen
{
  char tempString[20];
  myoled->home();
  myoled->print("Current Pos = ");
  myoled->print(fcurrentPosition);
  myoled->clearToEOL();

  myoled->println();
  myoled->print("Target Pos  = ");
  myoled->print(ftargetPosition);
  myoled->clearToEOL();
  myoled->println();

  myoled->print(coilpwrtxt);
  if ( mySetupData->get_coilpower() == 1 )
  {
    myoled->print(ontxt);
  }
  else
  {
    myoled->print(offtxt);
  }
  myoled->clearToEOL();
  myoled->println();

  myoled->print(revdirtxt);
  if ( mySetupData->get_reversedirection() == 1 )
  {
    myoled->print(ontxt);
  }
  else
  {
    myoled->print(offtxt);
  }
  myoled->clearToEOL();
  myoled->println();

  // stepmode setting
  myoled->print("Step Mode   = ");
  myoled->print(mySetupData->get_stepmode());
  myoled->clearToEOL();
  myoled->println();

  //Temperature
  myoled->print("Temperature = ");
#ifdef TEMPERATUREPROBE
  myoled->print(String(readtemp(0)));
#else
  myoled->print("20.0");
#endif
  myoled->print(" c");
  myoled->clearToEOL();
  myoled->println();

  //Motor Speed
  myoled->print("Motor Speed = ");
  switch ( mySetupData->get_motorSpeed() )
  {
    case SLOW:
      myoled->print("Slow");
      break;
    case MED:
      myoled->print("Med");
      break;
    case FAST:
      myoled->print("Fast");
      break;
  }
  myoled->clearToEOL();
  myoled->println();

  //MaxSteps
  myoled->print("MaxSteps    = ");
  ltoa(mySetupData->get_maxstep(), tempString, 10);
  myoled->print(tempString);
  myoled->clearToEOL();
  myoled->println();
}

void displaylcdpage1(void)
{
  // temperature compensation
  myoled->print("TComp Steps = ");
  myoled->print(mySetupData->get_tempcoefficient());
  myoled->clearToEOL();
  myoled->println();

  myoled->print("TComp State = ");
  if ( mySetupData->get_tempcompenabled() == 0 )
  {
    myoled->print("Off");
  }
  else
  {
    myoled->print("On");
  }
  myoled->clearToEOL();
  myoled->println();

  myoled->print("TComp Dir   = ");
  if ( mySetupData->get_tcdirection() == 0 )
  {
    myoled->print("In");
  }
  else
  {
    myoled->print("Out");
  }
  myoled->clearToEOL();
  myoled->println();

  myoled->print("Backlash In = ");
  if ( mySetupData->get_backlash_in_enabled() == 0 )
  {
    myoled->print("Off");
  }
  else
  {
    myoled->print("On");
  }
  myoled->clearToEOL();
  myoled->println();

  myoled->print("Backlash Out = ");
  if ( mySetupData->get_backlash_out_enabled() == 0 )
  {
    myoled->print("Off");
  }
  else
  {
    myoled->print("On");
  }
  myoled->clearToEOL();
  myoled->println();

  myoled->print("Backlash In#= ");
  myoled->print(mySetupData->get_backlashsteps_in());
  myoled->clearToEOL();
  myoled->println();

  myoled->print("Backlash Ou#= ");
  myoled->print(mySetupData->get_backlashsteps_out());
  myoled->clearToEOL();
  myoled->println();
}

void displaylcdpage2(void)
{
#ifndef BLUETOOTHMODE
  myoled->print("SSID = ");
  myoled->print(mySSID);
  myoled->clearToEOL();
  myoled->println();

  myoled->print("IP   = ");
  myoled->print(ipStr);
  myoled->clearToEOL();
  myoled->println();
#else
  myoled->print("Bluetooth Mode");
  myoled->clearToEOL();
  myoled->println();
#endif
}

void UpdatePositionOledText(void)
{
  myoled->setCursor(0, 0);
  myoled->print("Current Pos = ");
  myoled->print(fcurrentPosition);
  myoled->clearToEOL();
  myoled->println();

  myoled->print("Target Pos  = ");
  myoled->print(ftargetPosition);
  myoled->clearToEOL();
  myoled->println();
}

void Update_OledText(void)
{
  static unsigned long currentMillis;
  static unsigned long olddisplaytimestampNotMoving = millis();
  static byte displaypage = 0;

  currentMillis = millis();                       // see if the display needs updating
  if (((currentMillis - olddisplaytimestampNotMoving) > ((int)mySetupData->get_lcdpagetime() * 1000)) || (currentMillis < olddisplaytimestampNotMoving))
  {
    olddisplaytimestampNotMoving = currentMillis; // update the timestamp
    myoled->clear();                              // clrscr OLED
    switch (displaypage)
    {
      case 0:   displaylcdpage0();
        break;
      case 1:   displaylcdpage1();
        break;
      case 2:   displaylcdpage2();
        break;
      default:  displaylcdpage0();
        break;
    }
    displaypage++;
    displaypage = (displaypage > 2) ? 0 : displaypage;
  }
}
#endif // oledtext
#endif // OLEDDISPLAY

//____________ SendPaket Macro ______________________
/*
  #define SendPaket(string_) \
  myclient.print(string_); \
  DebugPrint(F("Send: ")); \
  DebugPrintln(string_);   \
  packetssent++;
*/

void SendPaket(String str)
{
  DebugPrint(F("Send: "));
  DebugPrintln(str);
#ifndef BLUETOOTHMODE
  // for Accesspoint or Station mode
  myclient.print(str);
  packetssent++;
#else
  // for bluetooth
  SerialBT.print(str);
#endif
}

void ESP_Communication( byte mode )
{
  byte cmdval;
  String receiveString = "";
  String WorkString = "";
  long paramval = 0;
  String replystr = "";

  switch ( mode )
  {
    case ESPDATA:
      // for Accesspoint or Station mode
      packetsreceived++;
      receiveString = myclient.readStringUntil('#'); // read until terminator    break;
#ifdef BLUETOOTHMODE
    case BTDATA:
      // for bluetooth
      receiveString = " : " + queue.pop();
      break;
#endif
  }

  receiveString += '#';                          // put back terminator
  String cmdstr = receiveString.substring(1, 3);
  cmdval = cmdstr.toInt();                       // convert command to an integer
  DebugPrint(F("- receive string="));
  DebugPrintln(receiveString);
  DebugPrint(F("- cmdstr="));
  DebugPrintln(cmdstr);
  DebugPrint(F("- cmdval="));
  DebugPrintln(cmdval);
  DebugPrint(F("- WorkString="));
  DebugPrintln(WorkString);
  switch (cmdval)
  {
    // all the get go first followed by set
    case 0: // get focuser position
      SendPaket('P' + String(fcurrentPosition) + '#');
      break;
    case 1: // ismoving
      SendPaket('I' + String(isMoving) + '#');
      break;
    case 2: // get controller status
      SendPaket("EOK#");
      break;
    case 3: // get firmware version
      SendPaket('F' + String(programVersion) + '#');
      break;
    case 4: // get firmware name
      SendPaket('F' + String(programName) + '#');
      break;
    case 6: // get temperature
      SendPaket('Z' + String(readtemp(0), 3) + '#');
      break;
    case 8: // get maxStep
      SendPaket('M' + String(mySetupData->get_maxstep()) + '#');
      break;
    case 10: // get maxIncrement
      SendPaket('Y' + String(mySetupData->get_maxstep()) + '#');
      break;
    case 11: // get coilpower
      SendPaket('O' + String(mySetupData->get_coilpower()) + '#');
      break;
    case 13: // get reverse direction setting, 00 off, 01 on
      SendPaket('R' + String(mySetupData->get_reversedirection()) + '#');
      break;
    case 21: // get temp probe resolution
      SendPaket('Q' + String(mySetupData->get_tempprecision()) + '#');
      break;
    case 24: // get status of temperature compensation (enabled | disabled)
      SendPaket('1' + String(mySetupData->get_tempcompenabled()) + '#');
      break;
    case 25: // get IF temperature compensation is available
#ifdef TEMPERATUREPROBE
      SendPaket("A1#"); // this focuser supports temperature compensation
#else
      SendPaket("A0#");
#endif
      break;
    case 26: // get temperature coefficient steps/degree
      SendPaket('B' + String(mySetupData->get_tempcoefficient()) + '#');
      break;
    case 29: // get stepmode
      SendPaket('S' + String(mySetupData->get_stepmode()) + '#');
      break;
    case 32: // get if stepsize is enabled
      SendPaket('U' + String(mySetupData->get_stepsizeenabled()) + '#');
      break;
    case 33: // get stepsize
      SendPaket('T' + String(mySetupData->get_stepsize()) + '#');
      break;
    case 34: // get the time that an LCD screen is displayed for
      SendPaket('X' + String(mySetupData->get_lcdpagetime()) + '#');
      break;
    case 37: // get displaystatus
      SendPaket('D' + String(mySetupData->get_displayenabled()) + '#');
      break;
    case 38: // :38#   Dxx#      Get Temperature mode 1=Celsius, 0=Fahrenheight
      SendPaket('b' + String(mySetupData->get_tempmode()) + '#');
      break;
    case 39: // get the new motor position (target) XXXXXX
      SendPaket('N' + String(ftargetPosition) + '#');
      break;
    case 43: // get motorspeed
      SendPaket('C' + String(mySetupData->get_motorSpeed()) + '#');
      break;
    case 45: // get motorspeedchange threshold value
      SendPaket('G' + String(motorspeedchangethresholdsteps) + '#');
      break;
    case 47: // get motorspeedchange enabled? on/off
      SendPaket('J' + String(motorspeedchangethresholdenabled) + '#');
      break;
    case 49: // aXXXXX
      SendPaket("ab552efd25e454b36b35795029f3a9ba7#");
      break;
    case 51: // return ESP8266Wifi Controller IP Address
      SendPaket('d' + ipStr + '#');
      break;
    case 52: // return ESP32 Controller number of TCP packets sent
      SendPaket('e' + String(packetssent) + '#');
      break;
    case 53: // return ESP32 Controller number of TCP packets received
      SendPaket('f' + String(packetsreceived) + '#');
      break;
    case 54: // gstr#  return ESP32 Controller SSID
      SendPaket('g' + String(mySSID));
      break;
    case 62: // get update of position on lcd when moving (00=disable, 01=enable)
      SendPaket('L' + String(mySetupData->get_lcdupdateonmove()) + '#');
      break;
    case 63: // get status of home position switch (0=off, 1=closed, position 0)
      SendPaket("H0#");
      break;
    case 66: // Get jogging state enabled/disabled
      SendPaket("K0#");
      break;
    case 68: // Get jogging direction, 0=IN, 1=OUT
      SendPaket("V0#");
      break;
    case 72: // get DelayAfterMove
      SendPaket('3' + String(mySetupData->get_DelayAfterMove()) + '#');
      break;
    case 74: // get backlash in enabled status
      SendPaket((mySetupData->get_backlash_in_enabled() == 0) ? "40#" : "41#");
      break;
    case 76: // get backlash OUT enabled status
      SendPaket((mySetupData->get_backlash_out_enabled() == 0) ? "50#" : "51#");
      break;
    case 78: // return number of backlash steps IN
      SendPaket('6' + String(mySetupData->get_backlashsteps_in()) + '#');
      break;
    case 80: // return number of backlash steps OUT
      SendPaket('7' + String(mySetupData->get_backlashsteps_out()) + '#');
      break;
    case 83: // get if there is a temperature probe
      SendPaket('c' + String(tprobe1) + '#');
      break;
    case 87: // get tc direction
      SendPaket('k' + String(mySetupData->get_tcdirection()) + '#');
      break;
    case 89:
      SendPaket("91#");
      break;
    // only the set commands are listed here as they do not require a response
    case 28:              // :28#       None    home the motor to position 0
      ftargetPosition = 0; // if this is a home then set target to 0
      break;
    case 5: // :05xxxxxx# None    Set new target position to xxxxxx (and focuser initiates immediate move to xxxxxx)
      // only if not already moving
      if ( isMoving == 0 )
      {
        WorkString = receiveString.substring(3, receiveString.length() - 1);
        ftargetPosition = (unsigned long)WorkString.toInt();
        if (ftargetPosition > mySetupData->get_maxstep())
          ftargetPosition = mySetupData->get_maxstep();
#ifdef OLEDDISPLAY
#ifdef OLEDGRAPHICS
        // TODO Holger
#endif
#ifdef OLEDTEXT
        UpdatePositionOledText();
#endif
#endif // oleddisplay
      }
      break;
    case 7: // set maxsteps
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      tmppos = (unsigned long)WorkString.toInt();
      delay(5);
      // check to make sure not above largest value for maxstep
      tmppos = (tmppos > FOCUSERUPPERLIMIT) ? FOCUSERUPPERLIMIT : tmppos;
      // check if below lowest set value for maxstep
      tmppos = (tmppos < FOCUSERLOWERLIMIT) ? FOCUSERLOWERLIMIT : tmppos;
      // check to make sure its not less than current focuser position
      tmppos = (tmppos < fcurrentPosition) ? fcurrentPosition : tmppos;
      mySetupData->set_maxstep(tmppos);
      break;
    case 12: // set coil power
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = (byte)WorkString.toInt() & 1;
      ( paramval == 1 ) ? enablesteppermotor() : releasesteppermotor();
      mySetupData->set_coilpower(paramval);
      break;
    case 14: // set reverse direction
      if ( isMoving == 0 )
      {
        WorkString = receiveString.substring(3, receiveString.length() - 1);
        mySetupData->set_reversedirection((byte)WorkString.toInt() & 1);
      }
      break;
    case 15: // set motorspeed
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = (byte)WorkString.toInt() & 3;
      mySetupData->set_motorSpeed((byte) paramval);
      setstepperspeed((byte) paramval);
      break;
    case 16: // set display to celsius
      mySetupData->set_tempmode(1); // temperature display mode, Celcius=1, Fahrenheit=0
      break;
    case 17: // set display to fahrenheit
      mySetupData->set_tempmode(0); // temperature display mode, Celcius=1, Fahrenheit=0
      break;
    case 18:
      // :180#    None    set the return of user specified stepsize to be OFF - default
      // :181#    None    set the return of user specified stepsize to be ON - reports what user specified as stepsize
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      mySetupData->set_stepsizeenabled((byte)WorkString.toInt() & 1);
      break;
    case 19: // :19xxxx#  None   set the step size value - double type, eg 2.1
      {
        WorkString = receiveString.substring(3, receiveString.length() - 1);
        float tempstepsize = (float)WorkString.toFloat();
        if (tempstepsize < 0)
          tempstepsize = 0; // set default maximum stepsize
        mySetupData->set_stepsize(tempstepsize);
      }
      break;
    case 20: // set the temperature resolution setting for the DS18B20 temperature probe
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = WorkString.toInt();
      mySetupData->set_tempprecision((byte) paramval);
#ifdef TEMPERATUREPROBE
      settempprobeprecision((byte) paramval);
#endif
      break;
    case 22: // set the temperature compensation value to xxx
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = WorkString.toInt();
      mySetupData->set_tempcoefficient((byte)paramval);
      break;
    case 23: // set the temperature compensation ON (1) or OFF (0)
#ifdef TEMPERATUREPROBE
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = WorkString.toInt();
      mySetupData->set_tempcompenabled((byte)paramval & 1);
#endif
      break;
    case 27: // stop a move - like a Halt
      ftargetPosition = fcurrentPosition;
      break;
    case 30: // set step mode
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = WorkString.toInt();
#if (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N || DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2EL9110S)
      paramval = (byte)(paramval & 3);      // STEP1 - STEP2
#endif
#if (DRVBRD == PRO2ESP32ULN2003 || DRVBRD == PRO2ESP32L298N || DRVBRD == PRO2ESP32L293DMINI || DRVBRD == PRO2ESP32L9110S)
      paramval = (byte)(paramval & 3);      // STEP1 - STEP2
#endif
#if (DRVBRD == PRO2EDRV8825)
      paramval = DRV8825TEPMODE;            // stepmopde set by jumpers
#endif
#if (DRVBRD == PRO2ESP32DRV8825)
      paramval = (paramval < STEP1 ) ? STEP1 : paramval;
      paramval = (paramval > STEP32 ) ? STEP32 : paramval;
#endif
      mySetupData->set_stepmode((byte)paramval);
      setsteppermode((byte) paramval);
      break;
    case 31: // set focuser position
      if ( isMoving == 0 )
      {
        WorkString = receiveString.substring(3, receiveString.length() - 1);
        {
          long tpos = (long)WorkString.toInt();
          tpos = (tpos < 0) ? 0 : tpos;
          tmppos = ((unsigned long) tpos > mySetupData->get_maxstep()) ? mySetupData->get_maxstep() : (unsigned long) tpos;
          fcurrentPosition = ftargetPosition = tmppos;
          mySetupData->set_fposition(tmppos);
        }
      }
      break;
    case 35: // set length of time an LCD page is displayed for in seconds
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = WorkString.toInt();
      paramval = (paramval < LCDPAGETIMEMIN ) ? LCDPAGETIMEMIN : paramval;
      paramval = (paramval > LCDPAGETIMEMAX ) ? LCDPAGETIMEMAX : paramval;
      mySetupData->set_lcdpagetime((byte)paramval);
      break;
    case 36:
      // :360#    None    Disable Display
      // :361#    None    Enable Display
#ifdef OLEDDISPLAY
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      mySetupData->set_displayenabled((byte)(WorkString.toInt() & 1));
      if (mySetupData->get_displayenabled() == 1)
      {
#ifdef OLEDGRAPHICS
        // TODO Holger
#endif
#ifdef OLEDTEXT
        myoled->Display_On();
#endif
      }
      else
      {
#ifdef OLEDGRAPHICS
        // TODO Holger
#endif
#ifdef OLEDTEXT
        myoled->Display_Off();
#endif
      }
#endif
      break;
    case 40: // reset Arduino myFocuserPro2E controller
      software_Reboot();
      //server.restart();
      break;
    case 44: // set motorspeed threshold when moving - switches to slowspeed when nearing destination
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      motorspeedchangethresholdsteps  = (byte) (WorkString.toInt() & 255);
      break;
    case 46: // enable/Disable motorspeed change when moving
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      delay(5);
      motorspeedchangethresholdenabled = (byte) (WorkString.toInt() & 1);
      break;
    case 48: // save settings to EEPROM
      mySetupData->set_fposition(fcurrentPosition); // need to forth save setting????????
      break;
    case 61: // set update of position on lcd when moving (00=disable, 01=enable)
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      mySetupData->set_lcdupdateonmove((byte)WorkString.toInt() & 1);
      break;
    case 64: // move a specified number of steps
      if ( isMoving == 0 )
      {
        WorkString = receiveString.substring(3, receiveString.length() - 1);
        tmppos = (unsigned long)WorkString.toInt() + fcurrentPosition;
        ftargetPosition = ( tmppos > mySetupData->get_maxstep()) ? mySetupData->get_maxstep() : tmppos;
      }
      break;
    case 65: // set jogging state enable/disable
      // ignore
      break;
    case 67: // set jogging direction, 0=IN, 1=OUT
      // ignore
      break;
    case 42: // reset focuser defaults
      if ( isMoving == 0 )
      {
        mySetupData->SetFocuserDefaults();
        ftargetPosition = fcurrentPosition = mySetupData->get_fposition();
      }
      break;
    case 71: // set DelayAfterMove in milliseconds
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      mySetupData->set_DelayAfterMove((byte)WorkString.toInt());
      break;
    case 73: // Disable/enable backlash IN (going to lower focuser position)
#ifdef BACKLASH
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = WorkString.toInt();
      mySetupData->set_backlash_in_enabled((byte)(paramval & 1));
#endif
      break;
    case 75: // Disable/enable backlash OUT (going to lower focuser position)
#ifdef BACKLASH
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = (byte)WorkString.toInt();
      mySetupData->set_backlash_out_enabled((byte)(paramval & 1));
#endif
      break;
    case 77: // set backlash in steps
#ifdef BACKLASH
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      mySetupData->set_backlashsteps_in((byte)WorkString.toInt());
#endif
      break;
    case 79: // set backlash OUT steps
#ifdef BACKLASH
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      mySetupData->set_backlashsteps_out((byte)WorkString.toInt());
#endif
      break;
    case 88: // set tc direction
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = WorkString.toInt();
      mySetupData->set_tcdirection((byte)paramval & 1);
      break;
  }
}

// Push button code
#ifdef INOUTPUSHBUTTONS
void update_pushbuttons(void)
{
  long newpos;
  // as PB use their own input pins, using if else will only read INPB if both PB are pushed
  if ( digitalRead(INPB == 0 ))       // is pushbutton pressed?
  {
    delay(20);                        // software debounce delay
    if ( digitalRead(INPB == 0 ))     // if still pressed
    {
      newpos = ftargetPosition - 1;
      newpos = (newpos < 0 ) ? 0 : newpos;
      ftargetPosition = newpos;
    }
  }
  else if ( digitalRead(OUTPB == 0 ))
  {
    delay(20);
    if ( digitalRead(OUTPB == 0 ))
    {
      newpos = ftargetPosition + 1;
      // an unsigned long range is 0 to 4,294,967,295
      // when an unsigned long decrements from 0-1 it goes to largest +ve value, ie 4,294,967,295
      // which would in likely be much much greater than maxstep
      newpos = (newpos > (long) mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : newpos;
      ftargetPosition = newpos;
    }
  }
}
#endif

#ifdef INFRAREDREMOTE
void update_irremote()
{
  // check IR
  if ( isMoving == 0 )                  // do not update if focuser is already moving - just wait
  {
    if (irrecv.decode(&results))
    {
      int adjpos = 0;
      static long lastcode;
      if ( results.value == 4294967295 )
      {
        results.value = lastcode;       // repeat last code
      }
      else
      {
        lastcode = results.value;
      }
      switch ( lastcode )
      {
        case 16753245:                  // CH- IN -1 SLOW
          adjpos = -1;
          mySetupData->set_motorSpeed(SLOW);
          break;
        case 16769565:                  // CH+ OUT +1 SLOW
          adjpos = 1;
          mySetupData->set_motorSpeed(SLOW);
          break;
        case 16720605:                  // |<< IN -10 MEDIUM
          adjpos = -10;
          mySetupData->set_motorSpeed(MED);
          break;
        case 16761405:                  // >>| OUT +10 MEDIUM
          adjpos = 10;
          mySetupData->set_motorSpeed(MED);
          break;
        case 16769055:                  // '-' IN -50 FAST
          adjpos = -50;
          mySetupData->set_motorSpeed(FAST);
          break;
        case 16748655:                  // 'EQ' OUT +50 FAST
          adjpos = 50;
          mySetupData->set_motorSpeed(FAST);
          break;
        case 16738455 :                 // 0 RESET POSITION TO 0
          adjpos = 0;
          ftargetPosition = 0;
          fcurrentPosition = 0;
          break;
      }
      setstepperspeed(mySetupData->get_motorSpeed());      // set the correct delay based on motorSpeed
      irrecv.resume();                                    // Receive the next value
      long newPos = fcurrentPosition + adjpos;            // adjust the target position
      newPos = (newPos < 0 ) ? 0 : newPos;
      newPos = (newPos > (long) mySetupData->get_maxstep() ) ? (long) mySetupData->get_maxstep() : newPos;
      ftargetPosition = newPos;
    }
  }
}
#endif

void setup()
{
#if defined DEBUG || defined LOCALSERIAL    // Open serial port if debugging or open serial port if LOCALSERIAL
  Serial.begin(SERIALPORTSPEED);
#endif

  mySetupData = new SetupData(Mode_SPIFFS); //instantiate object SetUpData with SPIFFS file instead of using EEPROM, init SPIFFS

#ifdef BLUETOOTHMODE                        // open Bluetooth port and set bluetooth device name if defined
  SerialBT.begin(BLUETOOTHNAME);            // Bluetooth device name
  btline = "";
  clearbtPort();
#endif

#ifdef INOUTLEDS                            // Setup IN and OUT LEDS, use as controller power up indicator
  pinMode(INLED, OUTPUT);
  pinMode(OUTLED, OUTPUT);
  digitalWrite(INLED, 1);
  digitalWrite(OUTLED, 1);
#endif

#ifdef INOUTPUSHBUTTONS                     // Setup IN and OUT Pushbuttons
  pinMode(INPB, INPUT);
  pinMode(OUTPB, INPUT);
#endif

#ifdef OLEDDISPLAY
#ifdef OLEDGRAPHICS
  // TODO Holger to check for graphics OLED
  Wire.begin();
  myoled = new SSD1306Wire(OLED_ADDR , I2CDATAPIN, I2CCLOCKPIN);
  myoled->init();
  myoled->flipScreenVertically();
  myoled->setFont(ArialMT_Plain_10);
  myoled->setTextAlignment(TEXT_ALIGN_LEFT);
  myoled.clear();
#endif
#ifdef OLEDTEXT
  Wire.begin(I2CDATAPIN, I2CCLOCKPIN);
  Wire.setClock(400000L);
  // Setup the OLED, screen size is 128 x 64, so using characters at 6x8 this gives 21chars across and 8 lines down
  myoled->begin(&Adafruit128x64, OLED_ADDR);
  myoled->setFont(Adafruit5x7);               // Set font
  myoled->clear();                            // clrscr OLED
#endif
#ifdef SHOWSTARTSCRN
  myoled->println(programName);               // print startup screen
  myoled->println(programVersion);
  myoled->println(ProgramAuthor);
#endif
#endif
  delay(250);                                 // keep delays small otherwise issue with ASCOM

  DebugPrint(F("fposition : "));              // Print Loaded Values from SPIFF
  DebugPrintln(mySetupData->get_fposition());
  DebugPrint(F(" focuserdirection : "));
  DebugPrintln(mySetupData->get_focuserdirection());
  DebugPrint(F(" maxstep : "));
  DebugPrintln(mySetupData->get_maxstep());
  DebugPrint(F(" stepsize : "));;
  DebugPrintln(mySetupData->get_stepsize());;
  DebugPrint(F(" DelayAfterMove : "));
  DebugPrintln(mySetupData->get_DelayAfterMove());
  DebugPrint(F(" backlashsteps_in : "));
  DebugPrintln(mySetupData->get_backlashsteps_in());
  DebugPrint(F(" backlashsteps_out : "));
  DebugPrintln(mySetupData->get_backlashsteps_out());
  DebugPrint(F(" tempcoefficient : "));
  DebugPrintln(mySetupData->get_tempcoefficient());
  DebugPrint(F(" tempprecision : "));
  DebugPrintln(mySetupData->get_tempprecision());
  DebugPrint(F(" stepomode : "));
  DebugPrintln(mySetupData->get_stepmode());
  DebugPrint(F(" coilpower : "));
  DebugPrintln(mySetupData->get_coilpower());
  DebugPrint(F(" reversedirection : "));
  DebugPrintln(mySetupData->get_reversedirection());
  DebugPrint(F(" stepsizeenabled : "));
  DebugPrintln(mySetupData->get_stepsizeenabled());
  DebugPrint(F(" tempmode : "));
  DebugPrintln(mySetupData->get_tempmode());
  DebugPrint(F(" lcdupdateonmove : "));
  DebugPrintln(mySetupData->get_lcdupdateonmove());
  DebugPrint(F(" lcdpagedisplaytime : "));
  DebugPrintln(mySetupData->get_lcdpagetime());
  DebugPrint(F(" tempcompenabled : "));
  DebugPrintln(mySetupData->get_tempcompenabled());
  DebugPrint(F(" tcdirection : "));
  DebugPrintln(mySetupData->get_tcdirection());
  DebugPrint(F(" motorSpeed : "));
  DebugPrintln(mySetupData->get_motorSpeed());
  DebugPrint(F(" displayenabled : "));
  DebugPrintln(mySetupData->get_displayenabled());

#ifdef TEMPERATUREPROBE                       // start temp probe
  pinMode(TEMPPIN, INPUT);                    // Configure GPIO pin for temperature probe
  DebugPrintln(F("Start temperature sensor"));
#ifdef OLEDDISPLAY
  myoled->clear();
  myoled->println(F("Check for temp probe"));
#endif
  sensor1.begin();                            // start the temperature sensor1
  DebugPrintln(F("Get number of temperature sensors"));
  tprobe1 = sensor1.getDeviceCount();         // should return 1 if probe connected
  DebugPrint(F("Sensors found: "));
  DebugPrintln(tprobe1);
  DebugPrintln(F("Find temperature probe address"));
  if (findds18b20address() == 1)
  {
    settempprobeprecision(mySetupData->get_tempprecision()); // set probe resolution
    DebugPrint(F("- Sensors found: "));
    DebugPrintln(tprobe1);
    DebugPrint(F("- Set temperature resolution to "));
    switch (mySetupData->get_tempprecision())
    {
      case 9: DebugPrintln(F("0.5"));
        break;
      case 10: DebugPrint(F("0.25"));
        break;
      case 11: DebugPrintln(F("0.125"));
        break;
      case 12: DebugPrintln(F("0.0625"));
        break;
      default:
        DebugPrintln(F("Unknown"));
        break;
    }
    sensor1.requestTemperatures();
    // readtemp(1); - moved to end of setup to avoid 600ms delay
  }
  else
  {
    DebugPrintln(F("Temperature probe address not found"));
#ifdef OLEDDISPLAY
    myoled->println("TempProbe not found");
#endif
  }
#endif // end TEMPERATUREPROBE

  // this is setup as an access point - your computer connects to this, cannot use DUCKDNS
#ifdef ACCESSPOINTMODE
#ifdef OLEDDISPLAY
  myoled->clear();
  myoled->println("Setup Access Point");
#endif
  WiFi.config(ip, dns, gateway, subnet);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(mySSID, myPASSWORD);
#endif // end ACCESSPOINTMODE

  // this is setup as a station connecting to an existing wifi network
#ifdef STATIONMODE
  if (staticip == STATICIPON)                       // if staticip then set this up before starting
  {
    DebugPrintln(F("Static IP defined. Setting up static ip now"));
#ifdef OLEDDISPLAY
    myoled->println(F("Setup Static IP"));
#endif
    WiFi.config(ip, dns, gateway, subnet);
    delay(5);
  }

  /* Log NodeMCU on to LAN. Provide IP Address over Serial port */
  int attempts = 0;                                 // holds the number of attempts/tries
  delay(5);
  WiFi.mode(WIFI_STA);
#ifdef OLEDDISPLAY
  myoled->println(F("Setup Station Mode"));
#endif
  status = WiFi.begin(mySSID, myPASSWORD);          // attempt to start the WiFi
  delay(1000);                                      // wait 500ms
  while (WiFi.status() != WL_CONNECTED)
  {
    DebugPrint(F("Attempting to connect to SSID : "));
    DebugPrintln(mySSID);
    DebugPrint(F("Attempting : "));
    DebugPrint(attempts);
    DebugPrintln(F(" to start WiFi"));
    delay(1000);                                    // wait 1s
    attempts++;                                     // add 1 to attempt counter to start WiFi
#ifdef OLEDDISPLAY
    myoled->clear();
    myoled->print(F("Connect attempt "));
    myoled->print(attempts);
#endif
    if (attempts > 10)                              // if this attempt is 11 or more tries
    {
      DebugPrint(F("Attempt to start Wifi failed after "));
      DebugPrint(attempts);
      DebugPrintln(F(" attempts"));
      DebugPrintln(F("Will attempt to restart the ESP module."));
#ifdef OLEDDISPLAY
      myoled->clear();
      myoled->print(F("Did not connect to AP"));
      delay(2000);
#endif
      ESP.restart();                                // GPIO0 must be HIGH and GPIO15 LOW when calling ESP.restart();
    }
  }
#endif // end STATIONMODE

#ifdef OLEDDISPLAY
  myoled->clear();
  myoled->println(F("Connected"));
  delay(100);                                       // keep delays small else issue with ASCOM
#endif

#ifndef BLUETOOTHMODE
  // Starting TCP Server
  myserver.begin();
  ESP32IPAddress = WiFi.localIP();
  delay(100);                                       // keep delays small else issue with ASCOM
#ifdef OLEDDISPLAY
  myoled->println(F("Start Server"));
#endif // end BLUETOOTHMODE

  // set packet counts to 0
  packetsreceived = 0;
  packetssent = 0;

  // connection established
  DebugPrint(F("SSID "));
  DebugPrintln(mySSID);
  DebugPrint(F("IP address : "));
  DebugPrintln(WiFi.localIP());
  DebugPrint(F("Starting TCP server on port : "));
  DebugPrintln(SERVERPORT);
  DebugPrintln(F("Server Ready"));
  myIP = WiFi.localIP();
  ipStr = String(myIP[0]) + "." + String(myIP[1]) + "." + String(myIP[2]) + "." + String(myIP[3]);
#else
  // it is Bluetooth so set some globals
  ipStr = "0.0.0.0";
#endif // ifndef BLUETOOTHMODE

  // assign to current working values
  ftargetPosition = fcurrentPosition = mySetupData->get_fposition();

#ifdef OLEDDISPLAY
  myoled->clear();
  myoled->print("Setup drvbrd: ");
  myoled->println(DRVBRD);
#endif
#if( DRVBRD == PRO2EDRV8825)
  driverboard = new DriverBoard(PRO2EDRV8825, String(programName), mySetupData->get_stepmode(), mySetupData->get_motorSpeed());
#endif
#if( DRVBRD == PRO2ESP32DRV8825)
  driverboard = new DriverBoard(PRO2ESP32DRV8825, String(programName), mySetupData->get_stepmode(), mySetupData->get_motorSpeed());
#endif
#if( DRVBRD == PRO2EULN2003)
  driverboard = new DriverBoard(PRO2EULN2003, String(programName), mySetupData->get_stepmode(), mySetupData->get_motorSpeed(), IN1ULN, IN3ULN, IN4ULN, IN2ULN);
#endif
#if( DRVBRD == PRO2ESP32ULN2003)
  driverboard = new DriverBoard(PRO2ESP32ULN2003, String(programName), mySetupData->get_stepmode(), mySetupData->get_motorSpeed(), IN1ULN, IN3ULN, IN4ULN, IN2ULN);
#endif
#if( DRVBRD == PRO2EL298N)
  driverboard = new DriverBoard(PRO2EL298N, String(programName), mySetupData->get_stepmode(), mySetupData->get_motorSpeed(), IN1L298N, IN2L298N, IN3L298N, IN4L298N);
#endif
#if( DRVBRD == PRO2ESP32L298N)
  driverboard = new DriverBoard(PRO2ESP32L298N, String(programName), mySetupData->get_stepmode(), mySetupData->get_motorSpeed(), IN1L298N, IN2L298N, IN3L298N, IN4L298N);
#endif
#if( DRVBRD == PRO2EL293DMINI)
  driverboard = new DriverBoard(PRO2EL293DMINI, String(programName), mySetupData->get_stepmode(), mySetupData->get_motorSpeed(), IN1L293DMINI, IN2L293DMINI, IN3L293DMINI, IN4L293DMINI);
#endif
#if( DRVBRD == PRO2ESP32L293DMINI)
  driverboard = new DriverBoard(PRO2ESP32L293DMINI, String(programName), mySetupData->get_stepmode(), mySetupData->get_motorSpeed(), IN1L293DMINI, IN2L293DMINI, IN3L293DMINI, IN4L293DMINI);
#endif
#if( DRVBRD == PRO2EL9110S)
  driverboard = new DriverBoard(PRO2EL9110S, String(programName), mySetupData->get_stepmode(), mySetupData->get_motorSpeed(), IN1L9110S, IN2L9110S, IN3L9110S, IN4L9110S);
#endif
#if( DRVBRD == PRO2ESP32L9110S)
  driverboard = new DriverBoard(PRO2ESP32L9110S, String(programName), mySetupData->get_stepmode(), mySetupData->get_motorSpeed(), IN1L9110S, IN2L9110S, IN3L9110S, IN4L9110S);
#endif

  delay(5);

#ifdef OLEDDISPLAY
  myoled->println("Driver board done");
#endif

  // range check focuser variables
  mySetupData->set_coilpower((mySetupData->get_coilpower() >= 1) ?  1 : 0);
  mySetupData->set_reversedirection((mySetupData->get_reversedirection() >= 1) ?  1 : 0);
  mySetupData->set_lcdpagetime((mySetupData->get_lcdpagetime() < 2) ? mySetupData->get_lcdpagetime() : 2);
  mySetupData->set_lcdpagetime((mySetupData->get_lcdpagetime() > 10) ? 10 : mySetupData->get_lcdpagetime());
  mySetupData->set_displayenabled((mySetupData->get_displayenabled() & 1));
  mySetupData->set_maxstep((mySetupData->get_maxstep() < FOCUSERLOWERLIMIT) ? FOCUSERLOWERLIMIT : mySetupData->get_maxstep());
  //mySetupData->set_fposition((mySetupData->get_fposition() < 0 ) ? 0 : mySetupData->get_fposition());
  //mySetupData->set_fposition((mySetupData->get_fposition() > mySetupData->get_maxstep()) ? mySetupData->get_maxstep() : mySetupData->get_fposition());
  mySetupData->set_stepsize((float)(mySetupData->get_stepsize() < 0.0 ) ? 0 : mySetupData->get_stepsize());
  mySetupData->set_stepsize((float)(mySetupData->get_stepsize() > DEFAULTSTEPSIZE ) ? DEFAULTSTEPSIZE : mySetupData->get_stepsize());

  if (mySetupData->get_coilpower() == 0)
  {
    driverboard->releasemotor();
  }

  delay(5);

#ifdef USEDUCKSDNS
#ifdef OLEDDISPLAY
  myoled->println("Setup DuckDNS");
  delay(1000);
#endif
  EasyDDNS.service("duckdns");                      // Enter your DDNS Service Name - "duckdns" / "noip"
  delay(5);
  EasyDDNS.client(duckdnsdomain, duckdnstoken);     // Enter ddns Domain & Token | Example - "esp.duckdns.org","1234567"
  delay(5);
  EasyDDNS.update(60000);                           // Check for New Ip Every 60 Seconds.
  delay(5);
#endif

#ifdef INFRAREDREMOTE
  irrecv.enableIRIn();                              // Start the IR
#endif

#ifdef INOUTLEDS
  digitalWrite(INLED, 0);
  digitalWrite(OUTLED, 0);
#endif

#ifdef OLEDDISPLAY
  myoled->clear();
  myoled->println("End setup()");
  delay(5);
#endif

  motorspeedchangethresholdsteps = MOTORSPEEDCHANGETHRESHOLD;
  motorspeedchangethresholdenabled = 0;
  isMoving = 0;
  readtemp(1);
}

//_____________________ loop()___________________________________________

void loop()
{
  static byte MainStateMachine = State_Idle;
  static byte DirOfTravel = mySetupData->get_focuserdirection();
  static byte ConnectionStatus = 0;
  static byte backlash_count = 0;
  static byte backlash_enabled = 0;
  static byte updatecount = 0;

  if (ConnectionStatus < 2)
  {
    myclient = myserver.available();
    if (myclient)
    {
      delay(100);
      DebugPrintln(F("tcp client has connected"));
      if (myclient.connected())
        ConnectionStatus = 2;
    }
    else
    {
      if (ConnectionStatus)
      {
        DebugPrintln(F("tcp client has disconnected"));
        myclient.stop();
        ConnectionStatus = 0;
      }
      delay(100);
    }
  }
  else
  {
    // is data available from the client request
    if (myclient.connected())
    {
      if (myclient.available())
        ESP_Communication(ESPDATA);
    }
    else
      ConnectionStatus = 1;
  }

#ifdef BLUETOOTHMODE
  // if there is a command from Bluetooth
  if ( queue.count() >= 1 )                 // check for serial command
  {
    ESP_Communication(BTDATA);
  }
#endif // end Bluetoothmode

#ifdef PUSHBUTTONS
  update_pushbuttons();
#endif

#ifdef INFRAREDREMOTE
  update_irremote();
#endif

  switch (MainStateMachine)
  {
    case State_Idle:
      if (fcurrentPosition != ftargetPosition)
      {
        enablesteppermotor();
        MainStateMachine = State_InitMove;
        DebugPrint(F("Idle => InitMove Target "));
        DebugPrint(ftargetPosition);
        DebugPrint(F(" Current "));
        DebugPrintln(fcurrentPosition);
      }
      else
      {
#ifdef OLEDDISPLAY
#ifdef OLEDGRAPHICS
        if ( mySetupData->get_displayenabled() == 1)
        {
          oled_draw_main(oled_stay);
        }
#endif
#ifdef OLEDTEXT
        if ( mySetupData->get_displayenabled() == 1)
        {
          Update_OledText();
        }
#endif
#endif
#ifdef TEMPERATUREPROBE
        Update_Temp();
#endif
        byte status = mySetupData->SaveConfiguration(fcurrentPosition, DirOfTravel); // save config if needed
        if ( status == true )
        {
#ifdef OLEDDISPLAY
#ifdef OLEDGRAPHICS
          oled_draw_main(oled_off);           // Display off after config saved
#endif
#endif
          DebugPrint("new Config saved: ");
          DebugPrintln(status);
        }
        break;
      }

    case State_InitMove:
      isMoving = 1;
      DirOfTravel = (ftargetPosition > fcurrentPosition) ? move_out : move_in;
      //driverboard.setDirOfTravel(DirOfTravel ^ mySetupData->get_reversedirection());    // Dir and Enable motor driver
      enablesteppermotor();
      if (mySetupData->get_focuserdirection() == DirOfTravel)
      {
        // move is in same direction, ignore backlash
        MainStateMachine = State_Moving;
        DebugPrintln(F("=> State_Moving"));
      }
      else
      {
        // move is in opposite direction, check for backlash enabled
        // get backlash settings
        if ( DirOfTravel == move_in)
        {
          backlash_count = mySetupData->get_backlashsteps_in();
          backlash_enabled = mySetupData->get_backlash_in_enabled();
        }
        else
        {
          backlash_count = mySetupData->get_backlashsteps_out();
          backlash_enabled = mySetupData->get_backlash_out_enabled();
        }
        // backlash needs to be applied, so get backlash values and states
        if ( backlash_enabled == 1 )
        {
          // apply backlash
          // save new direction of travel
          mySetupData->set_focuserdirection(DirOfTravel);
          setstepperspeed(BACKLASHSPEED);
          MainStateMachine = State_ApplyBacklash;
          DebugPrint(F("Idle => State_ApplyBacklash"));
        }
        else
        {
          // do not apply backlash, go straight to moving
          MainStateMachine = State_Moving;
          DebugPrint(F("Idle => State_Moving"));
        }
      }
      break;

    case State_ApplyBacklash:
      if ( backlash_count )
      {
        steppermotormove(DirOfTravel);
        backlash_count--;
      }
      else
      {
        setstepperspeed(mySetupData->get_motorSpeed());
        MainStateMachine = State_Moving;
        DebugPrintln(F("=> State_Moving"));
      }
      break;

    case State_Moving:
      if ( fcurrentPosition != ftargetPosition )      // must come first else cannot halt
      {
        if (DirOfTravel == move_out )
          fcurrentPosition++;
        else
          fcurrentPosition--;
        steppermotormove(DirOfTravel);
#ifdef OLEDDISPLAY
        if ( mySetupData->get_displayenabled() == 1)
        {
          if ( mySetupData->get_lcdupdateonmove() == 1 )
          {
            updatecount++;
            if ( updatecount > LCDUPDATEONMOVE )
            {
              delay(5);
              updatecount = 0;
#ifdef OLEDGRAPHICS
              // TODO Holger - updates position and target values on OLED when moving
              // UpdatePositionOledGraphics();  // ????
#endif
#ifdef OLEDTEXT
              UpdatePositionOledText();
#endif
            }
          }
        }
#endif
      }
      else
      {
        MainStateMachine = State_DelayAfterMove;
        DebugPrintln(F("=> State_DelayAfterMove"));
      }
      break;

    case State_DelayAfterMove:
      // apply Delayaftermove, this MUST be done here in order to get accurate timing for DelayAfterMove
      delay(mySetupData->get_DelayAfterMove());
      MainStateMachine = State_FinishedMove;
      DebugPrintln(F("=> State_FinishedMove"));
      break;

    case State_FinishedMove:
      isMoving = 0;
      if ( mySetupData->get_coilpower() == 0 )
        releasesteppermotor();
      MainStateMachine = State_Idle;
      DebugPrintln(F("=> State_Idle"));
      break;

    default:
      MainStateMachine = State_Idle;
      break;
  }
} // end Loop()

#ifdef BLUETOOTHMODE
// btSerialEvent checks for bluetooth data
void btserialEvent()
{
  // : starts the command, # ends the command, do not store these in the command buffer
  // read the command until the terminating # character
  while (btSerial.available() )
  {
    char inChar = btSerial.read();
    switch ( inChar )
    {
      case ':' :     // start
        btline = "";
        break;
      case '\r' :
      case '\n' :
        // ignore
        break;
      case '#' :     // eoc
        queue.push(btline);
        break;
      default :      // anything else
        btline = btline + inChar;
        break;
    }
  }
}
#endif
