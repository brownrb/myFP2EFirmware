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
// 1. Set your DRVBRD [section 1] in myBoards.h so the correct driver board is used
// 2. Set your target CPU to match the correct CPU for your board
// 3. Set the correct hardware options [section 3] in this file to match your hardware
// 4. Compile and upload to your controller
//
// ----------------------------------------------------------------------------------------------
// PCB BOARDS
// ----------------------------------------------------------------------------------------------
// ESP8266
//    ULN2003   https://aisler.net/p/UAAKPUTS
//    DRV8825   https://aisler.net/p/EKDGHUYW
// ESP32
//    ULN2003   https://aisler.net/p/OTEGMJNE
//    DRV8825   https://aisler.net/p/TYQHHGAI
//
// ----------------------------------------------------------------------------------------------
// COMPILE ENVIRONMENT : Tested with
// Arduino IDE 1.8.9
// ESP8266 Driver Board 2.4.0
// Libraries
// Arduino JSON 6.11.2
// myOLED as in myFP2ELibs
// IRRemoteESP32 2.0.1 as in myFP2ELibs
// HalfStepperESP32 as in myFP2ELibs
// Dallas Temperature 3.80
// Wire [as installed with Arduino 1.8.9
// OneWire 2.3.3
// EasyDDNS 1.5.2
// Notes:
// You may need to turn 12V off to reprogram chip. Speed is 115200. Sometimes you might need to
// remove the chip from PCB before re-programming new firmware. Remember to remove WIFI library
// as instructed in PDF when programming ESP32.
// ----------------------------------------------------------------------------------------------
// ESP8266 ISSUES
// One chip I have boots fine.
// Another chip will not boot properly from 12V only. I need to plug in USB cable, press reset
// then turn on 12V for it to boot correctly. ESP8266 Arduino lib 2.2.0 does not work with this
// chip either.
// TODO: Look at what need to be when chip starts up, do we need a delay reset circuit?

// ----------------------------------------------------------------------------------------------
// 1: SPECIFY DRIVER BOARD in myBoards.h
// ----------------------------------------------------------------------------------------------
// Please specify your driver board in myBoards.h, only 1 can be defined, see DRVBRD line

#include "generalDefinitions.h"
#include "myBoards.h"

// ----------------------------------------------------------------------------------------------
// 2: FOR ESP8266 BOARDS USNG DRV8825 SET DRV8825STEPMODE in myBoards.h
// ----------------------------------------------------------------------------------------------
// For ESP8266, remember to set DRV8825TEPMODE to the correct value if using WEMOS or NODEMCUV1 in myBoards.h

// ----------------------------------------------------------------------------------------------
// 3: FOR ULN2003, L293D, L293DMINI, L298N, L9110S specify STEPSPERREVOLUTION in myBoards.h
// ----------------------------------------------------------------------------------------------
// For these driver boards you MUST set STEPSPERREVOLUTION to the correct value in myBoards.h

// ----------------------------------------------------------------------------------------------
// 4: SPECIFY HARDWARE OPTIONS HERE
// ----------------------------------------------------------------------------------------------
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
#if (DRVBRD == WEMOSDRV8825 || DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2EDRV8825BIG \
  || DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N   || DRVBRD == PRO2EL293DMINI \
  || DSRVBRD == PRO2EL9110S || DRVBRD == PRO2EL293D )
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

enum OLED_Type {NO_OLED, TEXT_OLED, GRAPHIC_OLED};

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

#if (DRVBRD == WEMOSDRV8825 || DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2EDRV8825BIG \
  || DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N   || DRVBRD == PRO2EL293DMINI \
  || DSRVBRD == PRO2EL9110S || DRVBRD == PRO2EL293D )
// no support for bluetooth mode
#ifdef BLUETOOTHMODE
#halt // ERROR - BLUETOOTHMODE not supported for WEMOS or NODEMCUV1 ESP8266 chips
#endif
#endif

// ----------------------------------------------------------------------------------------------
// 7: INCLUDES FOR WIFI
// ----------------------------------------------------------------------------------------------
#include <WiFiServer.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

#if defined(ESP8266)                        // this "define(ESP8266)" comes from Arduino IDE automatic 
#include <ESP8266WiFi.h>
#include <FS.h>                             // Include the SPIFFS library  
#else
#include <WiFi.h>
#include "SPIFFS.h"
#endif
#include <SPI.h>
#include "FocuserSetupData.h"
// WIFI STUFF + SPIFFS => differed includes on different boards

// ----------------------------------------------------------------------------------------------
// 8: WIFI NETWORK SSID AND PASSWORD CONFIGURATION
// ----------------------------------------------------------------------------------------------
// 1. For access point mode this is the network you connect to
// 2. For station mode, change these to match your network details
const char* mySSID = "myfp2eap";
const char* myPASSWORD = "myfp2eap";

// ----------------------------------------------------------------------------------------------
// 9: DUCKDNS DOMAIN AND TOKEN CONFIGURATION
// ----------------------------------------------------------------------------------------------
// To use DucksDNS, uncomment the next line - can only be used together with STATIONMODE
//#define USEDUCKSDNS 1

// if using DuckDNS you need to set these next two parameters
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

// ----------------------------------------------------------------------------------------------
// 10: STATIC IP ADDRESS CONFIGURATION
// ----------------------------------------------------------------------------------------------
// must use static IP if using duckdns or as an Access Point
#define STATICIPON    1
#define STATICIPOFF   0
//int staticip = STATICIPON;                    // if set to STATICIPON then the IP address must be defined
int staticip = STATICIPOFF;                    // if set to STATICIPON then the IP address must be defined

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

// ----------------------------------------------------------------------------------------------
// 11: FIRMWARE CODE START - INCLUDES AND LIBRARIES
// ----------------------------------------------------------------------------------------------
// Compile this with Arduino IDE 1.8.9 with ESP8266 Core library installed v2.5.2 [for ESP8266]
// Make sure target board is set to Node MCU 1.0 (ESP12-E Module) [for ESP8266]

// Project specific includes
#ifdef BLUETOOTHMODE
#include "ESPQueue.h"                       //  By Steven de Salas
#endif

#include <Wire.h>                           // needed for I2C => OLED display

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
#if (DRVBRD == PRO2ESP32DRV8825 || DRVBRD == PRO2ESP32ULN2003   || DRVBRD == PRO2ESP32L298N \
  || DRVBRD == PRO2ESP32L293DMINI || DRVBRD == PRO2ESP32L9110S)
#ifdef INFRAREDREMOTE
#include <IRremoteESP32.h>
#endif
#endif

// ----------------------------------------------------------------------------------------------
// 12: BLUETOOTH MODE - Do not change
// ----------------------------------------------------------------------------------------------
#ifdef BLUETOOTHMODE
#include "BluetoothSerial.h"                // needed for Bluetooth comms
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
#endif

#ifdef BLUETOOTHMODE
String btline;                              // buffer for serial data
String BLUETOOTHNAME = "MYFP3ESP32BT";      // default name for Bluetooth controller, this name you can change
BluetoothSerial SerialBT;                   // define BT adapter to use
#endif // BLUETOOTHMODE

// ----------------------------------------------------------------------------------------------
// 13: GLOBAL DATA -- DO NOT CHANGE
// ----------------------------------------------------------------------------------------------

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

String programName = "myFP2E.";
DriverBoard* driverboard;

char programVersion[] = "100";
char ProgramAuthor[]  = "(c) R BROWN 2019";
char ontxt[]          = "ON ";
char offtxt[]         = "OFF";
char coilpwrtxt[]     = "Coil power  =";
char revdirtxt[]      = "Reverse Dir =";

unsigned long fcurrentPosition;         // current focuser position
unsigned long ftargetPosition;          // target position
unsigned long tmppos;

byte tprobe1;                           // indicate if there is a probe attached to myFocuserPro2
byte isMoving;                          // is the motor currently moving
byte motorspeedchangethresholdsteps;    // step number where when pos close to target motor speed changes
byte motorspeedchangethresholdenabled;  // used to enable/disable motorspeedchange when close to target position
String ipStr;                           // shared between BT mode and other modes

#ifdef BLUETOOTHMODE
Queue queue(QUEUELENGTH);               // receive serial queue of commands
String line;                            // buffer for serial data
#endif // bluetoothmode

#ifdef OLEDDISPLAY
#ifdef OLEDGRAPHICS
//SSD1306* myoled;
SSD1306Wire *myoled;
#endif
#ifdef OLEDTEXT
SSD1306AsciiWire* myoled;
#endif
#endif // oleddisplay

#ifdef TEMPERATUREPROBE
OneWire oneWirech1(TEMPPIN);            // setup temperature probe
DallasTemperature sensor1(&oneWirech1);
DeviceAddress tpAddress;                // holds address of the temperature probe
#endif // temperatureprobe

#ifndef BLUETOOTHMODE                   // WiFi stuff
IPAddress ESP32IPAddress;
String ServerLocalIP;
WiFiServer myserver(SERVERPORT);
WiFiClient myclient;                    // only one client supported, multiple connections denied
IPAddress myIP;
long rssi;
#endif // ifndef BLUETOOTHMODE

#ifdef INFRAREDREMOTE
IRrecv irrecv(IRPIN);
decode_results results;
#endif // infraredremote

int packetsreceived;
int packetssent;

SetupData *mySetupData;

// ----------------------------------------------------------------------------------------------
// 14: CODE START - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------

void software_Reboot()
{
#ifdef OLEDDISPLAY
#ifdef OLEDTEXT
  myoled->clear();
  myoled->print("Controller reboot");
#endif
#endif
#ifndef BLUETOOTHMODE
  if ( myclient.connected() )
  {
    myclient.stop();
  }
#endif
  delay(1000);
  ESP.restart();
}

// STEPPER MOTOR ROUTINES
void steppermotormove(byte dir )           // direction move_in, move_out ^ reverse direction
{
#ifdef INOUTLEDS
  ( dir == move_in ) ? digitalWrite(INLED, 1) : digitalWrite(OUTLED, 1);
#endif
  driverboard->movemotor(dir);
#ifdef INOUTLEDS
  ( dir == move_in ) ? digitalWrite(INLED, 0) : digitalWrite(OUTLED, 0);
#endif
}

// TEMPERATURE PROBE ROUTINES
#ifdef TEMPERATUREPROBE
float readtemp(byte new_measurement)
{
  static float lasttemp = 20.0;                 // start temp value
  if (!new_measurement)
  {
    return lasttemp;                            // return latest measurement
  }

  float result = sensor1.getTempCByIndex(0);    // get channel 1 temperature, always in celsius
  DebugPrint(F("Temperature = "));
  DebugPrintln(result);
  if (result > -40.0 && result < 80.0)
  {
    lasttemp = result;
  }
  else
  {
    result = lasttemp;
  }
  return result;
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

// COMMS ROUTINES
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
#ifndef BLUETOOTHMODE
      receiveString = myclient.readStringUntil('#'); // read until terminator    break;
#endif
#ifdef BLUETOOTHMODE
    case BTDATA:
      // for bluetooth
      receiveString = ':' + queue.pop();
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
      SendPaket('F' + programName + '\r' + '\n' + String(programVersion) + '#');
      break;
    case 6: // get temperature
#ifdef TEMPERATUREPROBE
      SendPaket('Z' + String(readtemp(0), 3) + '#');
#else
      SendPaket("Z20.00#");
#endif
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
      SendPaket('g' + String(mySSID) + '#');
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
      ( paramval == 1 ) ? driverboard->enablemotor() : driverboard->releasemotor();
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
      driverboard->setmotorspeed((byte) paramval);
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
        tempstepsize = (tempstepsize < 0) ? 0 : tempstepsize;     // set default maximum stepsize
        mySetupData->set_stepsize(tempstepsize);
      }
      break;
    case 20: // set the temperature resolution setting for the DS18B20 temperature probe
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = WorkString.toInt();
      mySetupData->set_tempprecision((byte) paramval);
#ifdef TEMPERATUREPROBE
      sensor1.setResolution(tpAddress, (byte) paramval);
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
#if (DRVBRD == WEMOSDRV8825 || DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2EDRV8825BIG)
      paramval = DRV8825TEPMODE;            // stepmopde set by jumpers
#endif
#if (DRVBRD == PRO2ESP32DRV8825)
      paramval = (paramval < STEP1 ) ? STEP1 : paramval;
      paramval = (paramval > STEP32 ) ? STEP32 : paramval;
#endif
#if (DRVBRD == PRO2EL293DNEMA || DRVBRD == PRO2EL293D28BYJ48)
      paramval = STEP1;
#endif
      mySetupData->set_stepmode((byte)paramval);
      driverboard->setstepmode((byte) paramval);
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
#endif // ifdef OLEDDISPLAY
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
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = WorkString.toInt();
      mySetupData->set_backlash_in_enabled((byte)(paramval & 1));
      break;
    case 75: // Disable/enable backlash OUT (going to lower focuser position)
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = (byte)WorkString.toInt();
      mySetupData->set_backlash_out_enabled((byte)(paramval & 1));
      break;
    case 77: // set backlash in steps
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      mySetupData->set_backlashsteps_in((byte)WorkString.toInt());
      break;
    case 79: // set backlash OUT steps
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      mySetupData->set_backlashsteps_out((byte)WorkString.toInt());
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
  // PB are active high - pins float low if unconnected
  if ( digitalRead(INPB) == 1 )       // is pushbutton pressed?
  {
    newpos = ftargetPosition - 1;
    newpos = (newpos < 0 ) ? 0 : newpos;
    ftargetPosition = newpos;
  }
  if ( digitalRead(OUTPB) == 1 )
  {
    newpos = ftargetPosition + 1;
    // an unsigned long range is 0 to 4,294,967,295
    // when an unsigned long decrements from 0-1 it goes to largest +ve value, ie 4,294,967,295
    // which would in likely be much much greater than maxstep
    newpos = (newpos > (long) mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : newpos;
    ftargetPosition = newpos;
  }
}
#endif

#ifdef INFRAREDREMOTE
void updateirremote()
{
  // check IR
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
    driverboard->setmotorspeed(mySetupData->get_motorSpeed());      // set the correct delay based on motorSpeed
    irrecv.resume();                                    // Receive the next value
    long newPos = fcurrentPosition + adjpos;            // adjust the target position
    newPos = (newPos < 0 ) ? 0 : newPos;
    newPos = (newPos > (long) mySetupData->get_maxstep() ) ? (long) mySetupData->get_maxstep() : newPos;
    ftargetPosition = newPos;
  }
}
#endif

#ifdef OLEDDISPLAY
#ifdef OLEDGRAPHICS
void oled_draw_Wifi(int j)
{
  myoled->clear();
  myoled->setTextAlignment(TEXT_ALIGN_CENTER);
  myoled->setFont(ArialMT_Plain_10);
  myoled->drawString(64, 0, "SSID: " + mySSID);
  myoled->drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits); // draw wifi logo

  for (int i = 1; i < 10; i++)
  {
    myoled->drawXbm(12 * i, 56, 8, 8, (i == j) ? activeSymbol : inactiveSymbol);
  }

  myoled->display();
}

void oled_draw_main_update(void)
{
  myoled->clear();
  myoled->setTextAlignment(TEXT_ALIGN_CENTER);
  myoled->setFont(ArialMT_Plain_10);
  myoled->drawString(64, 0, driverboard->getboardname());
  myoled->drawString(64, 12, "IP= " + ipStr);

  myoled->setTextAlignment(TEXT_ALIGN_LEFT);
  myoled->drawString(54, 54, "TEMP:" + String(readtemp(0), 2) + " C");
  myoled->drawString(0, 54, "BL:" + String(mySetupData->get_backlashsteps_out()));
  //myoled->drawString(24, 54, "SM:" + String(driverboard->getstepmode()));

  myoled->setTextAlignment(TEXT_ALIGN_CENTER);
  myoled->setFont(ArialMT_Plain_24);
  myoled->drawString(64, 28, String(fcurrentPosition, DEC) + ":" +  String(fcurrentPosition % driverboard->getstepmode(), DEC)); // Print currentPosition

  myoled->display();
}

void oled_draw_main(byte new_status)
{
  static byte current_status = oled_on;

  switch (new_status)
  {
    case oled_off:
      current_status = new_status;
      myoled->clear();
      myoled->display();
      break;
    case oled_stay:
      if (current_status == oled_on)
        oled_draw_main_update();
      break;
    case oled_on:
    default:
      oled_draw_main_update();
      current_status = new_status;
      break;
  }
}
#endif  // ifdef OLEDGRAPHICS
#endif  // ifdef OLEDDISPLAY

void setup()
{
#if (defined DEBUG)
  Serial.begin(SERIALPORTSPEED);
  DebugPrintln(F("Serial started. Debug on."));
#endif

  mySetupData = new SetupData(Mode_SPIFFS); // instantiate object SetUpData with SPIFFS file instead of using EEPROM, init SPIFFS

#ifdef BLUETOOTHMODE                        // open Bluetooth port and set bluetooth device name if defined
  SerialBT.begin(BLUETOOTHNAME);            // Bluetooth device name
  btline = "";
  clearbtPort();
  DebugPrintln(F("Bluetooth started."));
#endif

#ifdef INOUTLEDS                            // Setup IN and OUT LEDS, use as controller power up indicator
  pinMode(INLED, OUTPUT);
  pinMode(OUTLED, OUTPUT);
  digitalWrite(INLED, 1);
  digitalWrite(OUTLED, 1);
#endif

#ifdef INOUTPUSHBUTTONS                     // Setup IN and OUT Pushbuttons, active high when pressed
  pinMode(INPB, INPUT);
  pinMode(OUTPB, INPUT);
#endif

#ifdef OLEDDISPLAY
#ifdef OLEDGRAPHICS
  Wire.begin();
  Wire.beginTransmission(OLED_ADDR);        //check if OLED display is present
  if (Wire.endTransmission() == 0)
  {
    Serial.print(F("I2C device found at address "));
    Serial.println(OLED_ADDR, HEX);
  }
  else
  {
    Serial.println(F("no I2C device found"));
  }
  myoled = new SSD1306Wire(OLED_ADDR , I2CDATAPIN, I2CCLKPIN);
  myoled->init();
  myoled->flipScreenVertically();
  myoled->setFont(ArialMT_Plain_10);
  myoled->setTextAlignment(TEXT_ALIGN_LEFT);
  myoled->clear();

  myoled->drawString(0, 0, "myFocuserPro2 v:" + String(programVersion));
  myoled->drawString(0, 12, ProgramAuthor);
  myoled->display();

#endif // oledgraphics
#ifdef OLEDTEXT
  //Wire.begin(I2CDATAPIN, I2CCLKPIN);
  Wire.begin();
  myoled = new SSD1306AsciiWire();
  delay(5);
  // Setup the OLED
  myoled->begin(&Adafruit128x64, OLED_ADDR);
  delay(5);
  myoled->set400kHz();
  myoled->setFont(Adafruit5x7);
  myoled->clear();                                 // clrscr OLED
  myoled->Display_Normal();                        // black on white
  delay(5);
  myoled->Display_On();                            // display ON
  myoled->Display_Rotate(0);                       // portrait, not rotated
  myoled->Display_Bright();
  delay(5);
#ifdef SHOWSTARTSCRN
  myoled->println(programName);               // print startup screen
  myoled->println(programVersion);
  delay(5);
  myoled->println(ProgramAuthor);
#endif // showstartscreen
#endif // oledtext
#endif // oleddisplay

  delay(250);                                 // keep delays small otherwise issue with ASCOM

  DebugPrint(F(" fposition : "));             // Print Loaded Values from SPIFF
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
  DebugPrint(F(" stepmode : "));
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
  DebugPrintln(F("Start Tsensor"));
#ifdef OLEDDISPLAY
#ifdef OLEDTEXT
  myoled->clear();
  myoled->println(F("Check for Tprobe"));
#endif
#endif
  sensor1.begin();                            // start the temperature sensor1
  DebugPrintln(F("Get # of Tsensors"));
  tprobe1 = sensor1.getDeviceCount();         // should return 1 if probe connected
  DebugPrint(F("Sensors : "));
  DebugPrintln(tprobe1);
  DebugPrintln(F("Find Tprobe address"));
  if (sensor1.getAddress(tpAddress, 0) == true)
  {
    sensor1.setResolution(tpAddress, mySetupData->get_tempprecision());    // set probe resolution
    DebugPrint(F("- Sensors found: "));
    DebugPrintln(tprobe1);
    DebugPrint(F("- Set Tprecision to "));
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
  }
  else
  {
    DebugPrintln(F("Temperature probe address not found"));
#ifdef OLEDDISPLAY
#ifdef OLEDTEXT
    myoled->println("TempProbe not found");
#endif
#endif
  }
#endif // end TEMPERATUREPROBE

#ifdef ACCESSPOINT
#ifdef OLEDDISPLAY
#ifdef OLEDTEXT
  myoled->clear();
  myoled->println("Start Access Point");
#endif
#endif
  DebugPrintln(F("Start Access point"));
  WiFi.config(ip, dns, gateway, subnet);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(mySSID, myPASSWORD);
#endif // end ACCESSPOINT

  // this is setup as a station connecting to an existing wifi network
#ifdef STATIONMODE
  DebugPrintln(F("Start Station mode"));
  if (staticip == STATICIPON)                       // if staticip then set this up before starting
  {
    DebugPrintln(F("Static IP defined. Setting up static ip now"));
#ifdef OLEDDISPLAY
#ifdef OLEDTEXT
    myoled->println(F("Setup Static IP"));
#endif
#endif
    WiFi.config(ip, dns, gateway, subnet);
    delay(5);
  }

  /* Log NodeMCU on to LAN. Provide IP Address over Serial port */

  WiFi.mode(WIFI_STA);
#ifdef OLEDDISPLAY
#ifdef OLEDTEXT
  myoled->println(F("Setup Station Mode"));
#endif
#endif

  char xSSID[mySSID.length() + 1];
  char xPASSWORD[myPASSWORD.length() + 1];

  mySSID.toCharArray(xSSID, mySSID.length() + 1);
  myPASSWORD.toCharArray(xPASSWORD, myPASSWORD.length() + 1);

  byte status = WiFi.begin(xSSID, xPASSWORD);     // attempt to start the WiFi
  delay(1000);                                      // wait 500ms

  int attempts = 0;                                 // holds the number of attempts/tries
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
#ifdef OLEDTEXT
    myoled->clear();
    myoled->print(F("Connect attempt "));
    myoled->print(attempts);
#endif
#ifdef OLEDGRAPHICS
    oled_draw_Wifi(attempts);
#endif
#endif

    if (attempts > 10)                              // if this attempt is 11 or more tries
    {
      DebugPrint(F("Attempt to start Wifi failed after "));
      DebugPrint(attempts);
      DebugPrintln(F(" attempts"));
      DebugPrintln(F("Will attempt to restart the ESP module."));
#ifdef OLEDDISPLAY
#ifdef OLEDTEXT
      myoled->clear();
      myoled->print(F("Did not connect to AP"));
#endif
#ifdef OLEDGRAPHICS
      myoled->clear();
      myoled->drawString(0, 0, F("Did not connect to AP"));
#endif
#endif
      delay(2000);
      ESP.restart();                                // GPIO0 must be HIGH and GPIO15 LOW when calling ESP.restart();
    }
  }
#endif // end STATIONMODE

#ifdef OLEDDISPLAY
#ifdef OLEDTEXT
  myoled->clear();
  myoled->println(F("Connected"));
  delay(100);                                       // keep delays small else issue with ASCOM
#endif
#endif

#ifndef BLUETOOTHMODE
  // Starting TCP Server
  DebugPrintln(F("Start TCP Server"));
#ifdef OLEDDISPLAY
#ifdef OLEDTEXT
  myoled->println(F("Start TCP Server"));
#endif
#endif

  myserver.begin();
  DebugPrintln(F("Get local IP address"));
  ESP32IPAddress = WiFi.localIP();
  delay(100);                                       // keep delays small else issue with ASCOM
  DebugPrintln(F("TCP Server started"));

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
#ifdef OLEDTEXT
  myoled->clear();
  myoled->print("Setup drvbrd: ");
  myoled->println(DRVBRD);
#endif
#endif

  driverboard = new DriverBoard(DRVBRD);
  // setup firmware filename
  programName = programName + driverboard->getboardname();
  delay(5);

#ifdef OLEDDISPLAY
#ifdef OLEDTEXT
  myoled->println("Driver board done");
#endif
#endif

  // range check focuser variables
  mySetupData->set_coilpower((mySetupData->get_coilpower() >= 1) ?  1 : 0);
  mySetupData->set_reversedirection((mySetupData->get_reversedirection() >= 1) ?  1 : 0);
  mySetupData->set_lcdpagetime((mySetupData->get_lcdpagetime() < LCDPAGETIMEMIN) ? mySetupData->get_lcdpagetime() : LCDPAGETIMEMIN);
  mySetupData->set_lcdpagetime((mySetupData->get_lcdpagetime() > LCDPAGETIMEMAX) ? LCDPAGETIMEMAX : mySetupData->get_lcdpagetime());
  mySetupData->set_maxstep((mySetupData->get_maxstep() < FOCUSERLOWERLIMIT) ? FOCUSERLOWERLIMIT : mySetupData->get_maxstep());
  //mySetupData->set_fposition((mySetupData->get_fposition() < 0 ) ? 0 : mySetupData->get_fposition());
  //mySetupData->set_fposition((mySetupData->get_fposition() > mySetupData->get_maxstep()) ? mySetupData->get_maxstep() : mySetupData->get_fposition());
  mySetupData->set_stepsize((float)(mySetupData->get_stepsize() < 0.0 ) ? 0 : mySetupData->get_stepsize());
  mySetupData->set_stepsize((float)(mySetupData->get_stepsize() > DEFAULTSTEPSIZE ) ? DEFAULTSTEPSIZE : mySetupData->get_stepsize());

  // restore motorspeed
  driverboard->setmotorspeed(mySetupData->get_motorSpeed());
  // restore stepmode
  driverboard->setstepmode(mySetupData->get_stepmode());

  DebugPrintln(F("Check coilpower."));

  if (mySetupData->get_coilpower() == 0)
  {
    driverboard->releasemotor();
    DebugPrintln(F("Coil power released."));
  }

  delay(5);

#ifdef USEDUCKSDNS
#ifdef OLEDDISPLAY
#ifdef OLEDTEXT
  myoled->println("Setup DuckDNS");
#endif
#endif
  EasyDDNS.service("duckdns");                      // Enter your DDNS Service Name - "duckdns" / "noip"
  delay(5);
  EasyDDNS.client(duckdnsdomain, duckdnstoken);     // Enter ddns Domain & Token | Example - "esp.duckdns.org","1234567"
  delay(5);
  EasyDDNS.update(60000);                           // Check for New Ip Every 60 Seconds.
  delay(5);
#endif // useduckdns

#ifdef INFRAREDREMOTE
  irrecv.enableIRIn();                              // Start the IR
#endif

  DebugPrintln(F("Set motorspeedchange."));
  motorspeedchangethresholdsteps = MOTORSPEEDCHANGETHRESHOLD;
  motorspeedchangethresholdenabled = 0;
  isMoving = 0;

#ifdef TEMPERATUREPROBE
  readtemp(1);
#endif

#ifdef INOUTLEDS
  digitalWrite(INLED, 0);
  digitalWrite(OUTLED, 0);
#endif

  // TODO there is a bug which is not saving displayenabled state - seems to alwaus powerup disabled
  mySetupData->set_displayenabled(1);

  DebugPrint(F("Target  ="));
  DebugPrint(ftargetPosition);
  DebugPrint(F(", Current = "));
  DebugPrintln(fcurrentPosition);
  DebugPrint(F("Displayenabled : "));
  DebugPrintln(mySetupData->get_displayenabled());
  DebugPrintln(F("Setup end."));
#ifdef OLEDDISPLAY
#ifdef OLEDTEXT
  myoled->println("Setup end.");
#endif
#endif
}

//_____________________ loop()___________________________________________

void loop()
{
  static byte MainStateMachine = State_Idle;
  static byte DirOfTravel = mySetupData->get_focuserdirection();
  static byte ConnectionStatus = 0;
  static byte backlash_count = 0;
  static byte backlash_enabled = 0;
#ifdef OLEDDISPLAY
  static byte updatecount = 0;
#endif

#ifdef LOOPTIMETEST
  DebugPrint(F("- Loop Start ="));
  DebugPrintln(millis());
#endif

#ifndef BLUETOOTHMODE
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
      {
        ESP_Communication(ESPDATA);
      }
    }
    else
    {
      ConnectionStatus = 1;
    }
  }
#endif // ifndef BLUETOOTHMODE
#ifdef BLUETOOTHMODE
  if ( SerialBT.available() )
  {
    processbt();
  }
  // if there is a command from Bluetooth
  if ( queue.count() >= 1 )                 // check for serial command
  {
    ESP_Communication(BTDATA);
  }
#endif // ifdef Bluetoothmode



  //_____________________________MainMachine _____________________________

  switch (MainStateMachine)
  {
    case State_Idle:
      if (fcurrentPosition != ftargetPosition)
      {
        driverboard->enablemotor();
        MainStateMachine = State_InitMove;
        DebugPrint(F("Idle => InitMove Target "));
        DebugPrint(ftargetPosition);
        DebugPrint(F(" Current "));
        DebugPrintln(fcurrentPosition);
      }
      else
      {
        // focuser stationary. isMoving is 0
#ifdef INOUTPUSHBUTTONS
        updatepushbuttons();
#endif
#ifdef INFRAREDREMOTE
        updateirremote();
#endif
#ifdef OLEDDISPLAY
#ifdef OLEDGRAPHICS
        if ( mySetupData->get_displayenabled() == 1)
        {
          oled_draw_main(oled_stay);
        }
#endif // oledgraphics
#ifdef OLEDTEXT
        if ( mySetupData->get_displayenabled() == 1)
        {
          Update_OledText();
        }
#endif // OLEDTEXT
#endif // OLEDDISPLAY

#ifdef TEMPERATUREPROBE
        Update_Temp();
#endif // temperatureprobe
        byte status = mySetupData->SaveConfiguration(fcurrentPosition, DirOfTravel); // save config if needed
        if ( status == true )
        {
#ifdef OLEDDISPLAY
#ifdef OLEDGRAPHICS
          //          oled_draw_main(oled_off);           // Display off after config saved     ???????????????????????????????????????????
#endif // oledgraphics
#endif // oleddisplay
          DebugPrint("new Config saved: ");
          DebugPrintln(status);
        }
      }
      break;

    case State_InitMove:
      isMoving = 1;
      DirOfTravel = (ftargetPosition > fcurrentPosition) ? move_out : move_in;
      driverboard->enablemotor();
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
#ifdef BACKLASH
        // if backlask was defined then follow the backlash rules
        // if backlash has been enabled then apply it
        if ( backlash_enabled == 1 )
        {
          // apply backlash
          // save new direction of travel
          mySetupData->set_focuserdirection(DirOfTravel);
          //driverboard->setmotorspeed(BACKLASHSPEED);
          MainStateMachine = State_ApplyBacklash;
          DebugPrint(F("Idle => State_ApplyBacklash"));
        }
        else
        {
          // do not apply backlash, go straight to moving
          MainStateMachine = State_Moving;
          DebugPrint(F("Idle => State_Moving"));
        }
#else
        // ignore backlash
        MainStateMachine = State_Moving;
        DebugPrint(F("Idle => State_Moving"));
#endif
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
        //driverboard->setmotorspeed(mySetupData->get_motorSpeed());
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
        driverboard->releasemotor();
      MainStateMachine = State_Idle;
      DebugPrintln(F("=> State_Idle"));
      break;

    default:
      MainStateMachine = State_Idle;
      break;
  }

#ifdef LOOPTIMETEST
  DebugPrint(F("- Loop End ="));
  DebugPrintln(millis());
#endif
} // end Loop()

#ifdef BLUETOOTHMODE
void clearbtPort()
{
  while (SerialBT.available())
  {
    SerialBT.read();
  }
}

void processbt()
{
  // SerialBT.read() only returns a single char so build a command line one char at a time
  // : starts the command, # ends the command, do not store these in the command buffer
  // read the command until the terminating # character
  while (SerialBT.available() )
  {
    char inChar = SerialBT.read();
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
