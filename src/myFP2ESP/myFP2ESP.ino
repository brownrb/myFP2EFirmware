// COMPILE ISSUE
// If OLEDTEXT and OLEDGRAPHIC is undefined then will not compile
// IF OLEDTEXT or OLEDGRAPHIC is defined then it will not compile

// TODO
// Use preset0=35000, preset1=4500, move from preset1 to preset0
// TESTING DRVBRD PRO2EL293DNEMA again after recent changes to SPIFFS and DRVBRD code
// PRO2EDRV8825BIG  DOES NOT WORK AT ALL
// PRO2EL293D28BYJ48
// PRO2EL298N
// PRO2EL293DMINI
// PRO2EL9110S
// Check IRREMOTE on ESP32 boards, try IRREMOTE test program - created new library to reduce footprint 60K -> 4K

// COMPLETED
// PRO2ESP32DRV8825 DONE 4000 NEMA
// PRO2EL298N DONE 16500  NEMA
// PRO2EULN2003 DONE 17500 28BYJ48
// PRO2EL293DNEMA DONE 10500 NEMA
// PRO2EL293D28BYJ48 TESTING 17500 28BYJ48

// ISSUE 1: TODO
// ListDir in FocuserSetupData DOES NOT COMPILE ON ESP8266

// ISSUE 1: TODO
// OLED - I have commented most out just to get a compile


// ----------------------------------------------------------------------------------------------
// TITLE: myFP2ESP FIRMWARE OFFICIAL RELEASE 128
// ----------------------------------------------------------------------------------------------
// myFP2ESP - Firmware for ESP8266 and ESP32 myFocuserPro2 WiFi Controllers
// Supports Driver boards DRV8825, ULN2003, L298N, L9110S, L293DMINI, L293D
// ESP8266  OLED display, Temperature Probe
// ESP32    OLED display, Temperature Probe, Push Buttons, In/Out LED's,
//          Infrared Remote, Bluetooth
// Supports modes, ACCESSPOINT, STATIONMODE, BLUETOOTH, LOCALSERIAL, WEBSERVER,
//          ASCOMREMOTE
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
// This code is released under license. If you copy or write new code based on
// the code in these files. you MUST include to link to these files AND you MUST
// include references to the authors of this code.

// ----------------------------------------------------------------------------------------------
// CONTRIBUTIONS
// ----------------------------------------------------------------------------------------------
// It is costly to continue development and purchase boards and components.
// Your support is needed to continue development of this project. Please
// contribute to this project, and use PayPal to send your donation to user
// rbb1brown@gmail.com (Robert Brown). All contributions are gratefully accepted.

// ----------------------------------------------------------------------------------------------
// TO PROGRAM THE FIRMWARE
// ----------------------------------------------------------------------------------------------
// 1. Set your DRVBRD in myBoards.h so the correct driver board is used
// 2. Set the correct hardware options/controller modes in focuserconfig.h
//    to match your hardware
// 3. Set your target CPU to match the correct CPU for your board
// 4. Compile and upload to your controller

// ----------------------------------------------------------------------------------------------
// PCB BOARDS
// ----------------------------------------------------------------------------------------------
// The PCB's are available as GERBER files.
// https://sourceforge.net/projects/myfocuserpro2-esp32/files/PCB%20Gerber%20Files/
//
// L293D Shield  https://www.ebay.com/itm/L293D-Motor-Drive-Shield-Wifi-Module-For-Arduino-NodeMcu-Lua-ESP8266-ESP-12E/292619874436
// ESP32 R3WEMOS https://www.ebay.com/itm/R3-Wemos-UNO-D1-R32-ESP32-WIFI-Bluetooth-CH340-Devolopment-Board-For-Arduino/264166013552
// ----------------------------------------------------------------------------------------------
// COMPILE ENVIRONMENT : Tested with
// Arduino IDE 1.8.13
// ESP8266 Core 2.7.4
// ESP32 Core 1.0.4
// Libraries
// Arduino JSON 6.15.2
// myOLED as in myFP2ELibs
// IRRemoteESP32 as in myFP2ELibs
// HalfStepperESP32 as in myFP2ELibs
// myDallas Temperature 3.7.3A as in myFP2ELibs
// Wire [as installed with Arduino 1.8.13
// OneWire 2.3.5
// EasyDDNS 1.5.9
// Notes:
// You may need to turn 12V off to reprogram chip. Speed is 115200. Sometimes
// you might need to remove the chip from PCB before re-programming new firmware.
// Remember to remove WIFI library as instructed in PDF when programming ESP32
// or ESP8266.
// ----------------------------------------------------------------------------------------------
// ESP8266 ISSUES
// One chip I have boots fine.
// Another chip will not boot properly from 12V only. I need to plug in USB
// cable, press reset then turn on 12V for it to boot correctly. ESP8266
// Arduino lib 2.2.0 does not work with this chip either.

// ----------------------------------------------------------------------------------------------
// 1: SPECIFY DRIVER BOARD in myBoards.h
// ----------------------------------------------------------------------------------------------
// Please specify your driver board in myBoards.h, only 1 can be defined, see DRVBRD line

#include "generalDefinitions.h"
#include "myBoards.h"

// ----------------------------------------------------------------------------------------------
// 2: FOR ALL ESP8266 BOARDS AND R3-WEMOS-ESP32 USING DRV8825 SET DRV8825STEPMODE in myBoards.h
// ----------------------------------------------------------------------------------------------
// SET DRV8825STEPMODE in myBoards.h to the correct value

// ----------------------------------------------------------------------------------------------
// 3: FOR ULN2003, L293D, L293DMINI, L298N, L9110S specify STEPSPERREVOLUTION in myBoards.h
// ----------------------------------------------------------------------------------------------
// You MUST set STEPSPERREVOLUTION to the correct value in myBoards.h

// ----------------------------------------------------------------------------------------------
// 4: SPECIFY HARDWARE OPTIONS IN focuserconfig.h
// ----------------------------------------------------------------------------------------------
// Specify your controller options in focuserconfig.h, such as OLEDTEXT

// ----------------------------------------------------------------------------------------------
// 5: SPECIFY THE CONTROLLER MODE IN focuserconfig.h
// ----------------------------------------------------------------------------------------------
// Please specify your controller mode in focuserconfig.h, such as ACCESSPOINT
// and MANAGEMENT

// ----------------------------------------------------------------------------------------------
// 6: INCLUDES
// ----------------------------------------------------------------------------------------------
#include "focuserconfig.h"

#undef DEBUG_ESP_HTTP_SERVER                // needed sometimes to prevent serial output from WiFiServerlibrary

#include <WiFiServer.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

#if defined(ESP8266)                        // this "define(ESP8266)" comes from Arduino IDE
#undef DEBUG_ESP_HTTP_SERVER                // prevent messages from WiFiServer 
#include <ESP8266WiFi.h>
#include <FS.h>                             // include the SPIFFS library  
#else                                       // otherwise assume ESP32
#include <WiFi.h>
#include "SPIFFS.h"
#endif
#include <SPI.h>
#include "FocuserSetupData.h"

// ----------------------------------------------------------------------------------------------
// 7: WIFI NETWORK SSID AND PASSWORD CONFIGURATION
// ----------------------------------------------------------------------------------------------
// 1. For access point mode this is the network you connect to
// 2. For station mode, change these to match your network details
#ifdef ACCESSPOINT                          // your computer connects to this accesspoint
char mySSID[64] = "myfp2eap";
char myPASSWORD[64] = "myfp2eap";
#endif
#ifdef STATIONMODE                          // the controller connects to your network
char mySSID[64] = "myfp2eap";               // you need to set this to your WiFi network SSID
char myPASSWORD[64] = "myfp2eap";           // and you need to set the correct password
#endif

// ----------------------------------------------------------------------------------------------
// 8: OTAUPDATES (OVER THE AIR UPDATE) SSID AND PASSWORD CONFIGURATION
// ----------------------------------------------------------------------------------------------
// You can change the values for OTANAME and OTAPassword if required
#ifdef OTAUPDATES
#include <ArduinoOTA.h>

const char *OTAName = "ESP8266";            // the username and password for the OTA service
const char *OTAPassword = "esp8266";
#endif // #ifdef OTAUPDATES

// ----------------------------------------------------------------------------------------------
// 9: BLUETOOTH MODE NAME:
// ----------------------------------------------------------------------------------------------
#ifdef BLUETOOTHMODE
String BLUETOOTHNAME = "MYFP3ESP32BT";      // default name for Bluetooth controller, this name you can change
#endif // #ifdef BLUETOOTHMODE

// ----------------------------------------------------------------------------------------------
// 10: mDNS NAME: Name must be alphabetic chars only, lowercase
// ----------------------------------------------------------------------------------------------
#ifdef MDNSSERVER
char mDNSNAME[] = "myfp2eap";               // mDNS name will be myfp2eap.local
#endif // #ifdef MDNSSERVER

// ----------------------------------------------------------------------------------------------
// 11: DUCKDNS DOMAIN AND TOKEN CONFIGURATION
// ----------------------------------------------------------------------------------------------
// if using DuckDNS you need to set these next two parameters, duckdnsdomain
// and duckdnstoken, BUT you cannot use DuckDNS with ACCESSPOINT, BLUETOOTHMODE
// or LOCALSERIAL mode
#ifdef USEDUCKDNS
const char* duckdnsdomain = "myfp2erobert.duckdns.org";
const char* duckdnstoken = "0a0379d5-3979-44ae-b1e2-6c371a4fe9bf";
#endif // #ifdef USEDUCKDNS

// ----------------------------------------------------------------------------------------------
// 12: STATIC IP ADDRESS CONFIGURATION
// ----------------------------------------------------------------------------------------------
// must use static IP if using duckdns or as an Access Point
#ifndef STATICIPON
#define STATICIPON    1
#endif
#ifndef STATICIPOFF
#define STATICIPOFF   0
#endif
//int staticip = STATICIPON;                  // IP address specified by controller - must be defined correctly
int staticip = STATICIPOFF;                   // IP address is generated by network device and is dynamic and can change

#ifdef ACCESSPOINT
// By default the Access point should be 192.168.4.1 - DO NOT CHANGE
IPAddress ip(192, 168, 4, 1);                 // AP static IP - you can change these values to change the IP
IPAddress dns(192, 168, 4, 1);                // just set it to the same IP as the gateway
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);
#endif // #ifdef ACCESSPOINT

#ifdef STATIONMODE
// These need to reflect your current network settings - 192.168.x.21 - change x
// This has to be within the range for dynamic ip allocation in the router
// No effect if staticip = STATICIPOFF
IPAddress ip(192, 168, 2, 21);                // station static IP - you can change these values to change the IP
IPAddress dns(192, 168, 2, 1);                // just set it to the same IP as the gateway
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);
#endif // #ifdef STATIONMODE

// ----------------------------------------------------------------------------------------------
// 13: BLUETOOTH MODE - Do not change
// ----------------------------------------------------------------------------------------------
#ifdef BLUETOOTHMODE
#include "BluetoothSerial.h"                  // needed for Bluetooth comms
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;                     // define BT adapter to use
#endif // BLUETOOTHMODE

// ----------------------------------------------------------------------------------------------
// 14: FIRMWARE CODE START - INCLUDES AND LIBRARIES
// ----------------------------------------------------------------------------------------------

// Project specific includes - DO NOT CHANGE
#if defined(BLUETOOTHMODE) || defined(LOCALSERIAL)
#include "ESPQueue.h"                         // by Steven de Salas
Queue queue(QUEUELENGTH);                     // receive serial queue of commands
#ifdef LOCALSERIAL
String serialline;                            // buffer for serial data
#endif
#ifdef BLUETOOTHMODE
String btline;                                // buffer for serial data
#endif
#endif // #if defined(BLUETOOTHMODE) || defined(LOCALSERIAL)

#if defined(ACCESSPOINT) || defined(STATIONMODE)
IPAddress ESP32IPAddress;
String ServerLocalIP;
WiFiServer myserver(SERVERPORT);
WiFiClient myclient;                          // only one client supported, multiple connections denied
IPAddress myIP;
long rssi;
#endif // #if defined(ACCESSPOINT) || defined(STATIONMODE)

#include <OneWire.h>                          // https://github.com/PaulStoffregen/OneWire
#include <myDallasTemperature.h>
OneWire oneWirech1(TEMPPIN);                  // setup temperature probe
DallasTemperature sensor1(&oneWirech1);
DeviceAddress tpAddress;                      // holds address of the temperature probe

#include "displays.h"
#if defined(OLEDTEXT) || defined(OLEDGRAPHICS)
#ifdef OLEDTEXT
OLED_TEXT *myoled;
#elif OLEDGRAPHICS
OLED_GRAPHIC *myoled;
#else
OLED_NON *myoled;
#endif
#endif // #if defined(OLEDTEXT) || defined(OLEDGRAPHICS)

// ----------------------------------------------------------------------------------------------
// 15: GLOBAL DATA -- DO NOT CHANGE
// ----------------------------------------------------------------------------------------------

//           reversedirection
//__________________________________
//               0   |   1
//__________________________________
//moving_out  1||  1   |   0
//moving_in   0||  0   |   1

DriverBoard* driverboard;

unsigned long fcurrentPosition;             // current focuser position
unsigned long ftargetPosition;              // target position
volatile bool halt_alert;

boolean displayfound;
byte    tprobe1;                            // indicate if there is a probe attached to myFocuserPro2
byte    isMoving;                           // is the motor currently moving
char    ipStr[16];                          // shared between BT mode and other modes
const char ip_zero[] = "0.0.0.0";

int  packetsreceived;
int  packetssent;
bool mdnsserverstate;                       // states for services, RUNNING | STOPPED
bool webserverstate;
bool ascomserverstate;
bool ascomdiscoverystate;
bool managementserverstate;
bool tcpipserverstate;
bool otaupdatestate;
bool duckdnsstate;

SetupData *mySetupData;                           // focuser data

// ----------------------------------------------------------------------------------------------
// 16: FIRMWARE CODE START - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
// Compile this with Arduino IDE 1.8.13
// ESP8266 Core library installed v2.7.4
// ESP32 Core library installed v1.0.4
// Make sure Arduino IDE-Tools-Board is set to
//   Node MCU 1.0 (ESP12-E Module) [for ESP8266] or WEMOS ????
//   ESP32 Dev Module or WEMOS ????

#include "comms.h"                                // do not change or move

// ----------------------------------------------------------------------------------------------
// 17: TEMPERATURE - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
void init_temp(void)
{
  // start temp probe
  pinMode(TEMPPIN, INPUT);                // Configure GPIO pin for temperature probe
  DebugPrintln(CHECKFORTPROBESTR);
  sensor1.begin();                        // start the temperature sensor1
  DebugPrintln(GETTEMPPROBESSTR);
  tprobe1 = sensor1.getDeviceCount();     // should return 1 if probe connected
  DebugPrint(TPROBESTR);
  DebugPrintln(tprobe1);
  if (sensor1.getAddress(tpAddress, 0) == true)   // get the address so we can set the probe resolution
  {
    tprobe1 = 1;                                  // address was found so there was a probe
    sensor1.setResolution(tpAddress, mySetupData->get_tempprecision());    // set probe resolution
    DebugPrint(SETTPROBERESSTR);
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
    sensor1.requestTemperatures();                // request the sensor to begin a temperature reading
  }
  else
  {
    DebugPrintln(TPROBENOTFOUNDSTR);
  }
}

void temp_setresolution(byte rval)
{
  sensor1.setResolution(tpAddress, rval);         // set the probe resolution (9=0.25, 12=0.00125)
}

float read_temp(byte new_measurement)
{
  static float lasttemp = 20.0;                   // keep track of previous temperature value
  if (!new_measurement)
  {
    return lasttemp;                              // return previous measurement
  }

  float result = sensor1.getTempCByIndex(0);      // get temperature, always in celsius
  DebugPrint(F(TEMPSTR));
  DebugPrintln(result);
  if (result > -40.0 && result < 80.0)            // avoid erronous readings
  {
    lasttemp = result;
  }
  else
  {
    result = lasttemp;
  }
  return result;
}

void update_temp(void)
{
  static byte tcchanged = mySetupData->get_tempcompenabled();  // keeps track if tempcompenabled changes

  if (tprobe1 == 1)
  {
    static unsigned long lasttempconversion = 0;
    static byte requesttempflag = 0;                  // start with request
    unsigned long tempnow = millis();

    // see if the temperature needs updating - done automatically every 1.5s
    if (TimeCheck(lasttempconversion, TEMPREFRESHRATE))   // see if the temperature needs updating
    {
      static float tempval;
      static float starttemp;                         // start temperature to use when temperature compensation is enabled

      if ( tcchanged != mySetupData->get_tempcompenabled() )
      {
        tcchanged = mySetupData->get_tempcompenabled();
        if ( tcchanged == 1 )
        {
          starttemp = read_temp(1);
        }
      }

      lasttempconversion = tempnow;                   // update time stamp

      if (requesttempflag)
      {
        tempval = read_temp(1);
      }
      else
      {
        sensor1.requestTemperatures();
      }

      requesttempflag ^= 1; // toggle flag

      if (mySetupData->get_tempcompenabled() == 1)    // check for temperature compensation
      {
        if ((abs)(starttemp - tempval) >= 1)          // calculate if temp has moved by more than 1 degree
        {
          unsigned long newPos;
          byte temperaturedirection;                  // did temperature fall (1) or rise (0)?
          temperaturedirection = (tempval < starttemp) ? 1 : 0;
          if (mySetupData->get_tcdirection() == 0)    // check if tc direction for compensation is inwards
          {
            // temperature compensation direction is in, if a fall then move in else move out
            if ( temperaturedirection == 1 )          // check if temperature is falling
            {
              newPos = ftargetPosition - mySetupData->get_tempcoefficient();    // then move inwards
            }
            else
            {
              newPos = ftargetPosition + mySetupData->get_tempcoefficient();    // else move outwards
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

// ----------------------------------------------------------------------------------------------
// 18: INFRARED REMOTE CONTROLLER - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#ifdef INFRAREDREMOTE

#include <myfp2eIRremoteESP8266.h>                    // use cut down version to save spave
#include <myfp2eIRrecv.h>                             // unable to turn off all options by using a define
#include <myfp2eIRutils.h>
#include "irremotemappings.h"
const uint16_t RECV_PIN = IRPIN;
IRrecv irrecv(RECV_PIN);
decode_results results;

void update_irremote()
{
  // check IR
  if (irrecv.decode(&results))
  {
    int adjpos = 0;
    static long lastcode;
    if ( results.value == KEY_REPEAT )
    {
      results.value = lastcode;                   // repeat last code
    }
    else
    {
      lastcode = results.value;
    }
    switch ( lastcode )
    {
      case IR_SLOW:
        mySetupData->set_motorSpeed(SLOW);
        break;
      case IR_MEDIUM:
        mySetupData->set_motorSpeed(MED);
        break;
      case IR_FAST:
        mySetupData->set_motorSpeed(FAST);
        break;
      case IR_IN1:
        adjpos = -1;
        break;
      case IR_OUT1:
        adjpos = 1;
        break;
      case IR_IN10:
        adjpos = -10;
        break;
      case IR_OUT10:
        adjpos = 10;
        break;
      case IR_IN50:
        adjpos = -50;
        break;
      case IR_OUT50:
        adjpos = 50;
        break;
      case IR_IN100:
        adjpos = -100;
        break;
      case IR_OUT100:
        adjpos = 100;
        break;
      case IR_SETPOSZERO:                         // 0 RESET POSITION TO 0
        adjpos = 0;
        ftargetPosition = 0;
        driverboard->setposition(0);
        mySetupData->set_fposition(0);
        break;
      case IR_PRESET0:
        ftargetPosition = mySetupData->get_focuserpreset(0);
        break;
      case IR_PRESET1:
        ftargetPosition = mySetupData->get_focuserpreset(1);
        break;
      case IR_PRESET2:
        ftargetPosition = mySetupData->get_focuserpreset(2);
        break;
      case IR_PRESET3:
        ftargetPosition = mySetupData->get_focuserpreset(3);
        break;
      case IR_PRESET4:
        ftargetPosition = mySetupData->get_focuserpreset(4);
        break;
      case IR_PRESET5:
        ftargetPosition = mySetupData->get_focuserpreset(5);
        break;
      case IR_PRESET6:
        ftargetPosition = mySetupData->get_focuserpreset(6);
        break;
      case IR_PRESET7:
        ftargetPosition = mySetupData->get_focuserpreset(7);
        break;
      case IR_PRESET8:
        ftargetPosition = mySetupData->get_focuserpreset(8);
        break;
      case IR_PRESET9:
        ftargetPosition = mySetupData->get_focuserpreset(9);
        break;
    }
    irrecv.resume();                              // Receive the next value
    long newpos;
    if ( adjpos < 0 )
    {
      newpos = (long)fcurrentPosition + adjpos;
      newpos = (newpos < 0 ) ? 0 : newpos;
      ftargetPosition = newpos;
    }
    else if ( adjpos > 0)
    {
      newpos = fcurrentPosition + adjpos;
      newpos = (newpos > mySetupData->get_maxstep()) ? mySetupData->get_maxstep() : newpos;
      ftargetPosition = newpos;
    }
  }
}

void init_irremote(void)
{
  irrecv.enableIRIn();                            // Start the IR
}
#endif // #ifdef INFRAREDREMOTE

// ----------------------------------------------------------------------------------------------
// 19: JOYSTICK - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#if defined(JOYSTICK1) || defined(JOYSTICK2)
#include "joystick.h"

#ifdef JOYSTICK2
volatile int joy2swstate;
#endif

// 2-AXIS Analog Thumb Joystick for Arduino
#ifdef JOYSTICK1
void update_joystick1(void)
{
  int joyval;

  DebugPrintln(UPDATEJOYSTICKSTR);
  joyval = analogRead(JOYINOUTPIN);
  DebugPrint(JOYSTICKVALSTR);
  DebugPrintln(joyval);
  if ( joyval < (JZEROPOINT - JTHRESHOLD) )
  {
    ftargetPosition--;                            // move IN
    DebugPrint(JOYSTICKXINVALSTR);
    DebugPrint(joyval);
  }
  else if ( joyval > (JZEROPOINT + JTHRESHOLD) )
  {
    ftargetPosition++;                            // move OUT
    if ( ftargetPosition > mySetupData->get_maxstep())
    {
      ftargetPosition = mySetupData->get_maxstep();
    }
    joyval = joyval - (JZEROPOINT + JTHRESHOLD);
    if ( joyval < 0 )
    {
      joyval = JZEROPOINT - joyval;
    }
    DebugPrint(JOYSTICKXOUTVALSTR);
    DebugPrint(joyval);
  }
}

void init_joystick1(void)
{
  // perform any inititalisations necessary
  // for future use
  pinMode(JOYINOUTPIN, INPUT);
  pinMode(JOYOTHERPIN, INPUT);
}
#endif // #ifdef JOYSTICK1

// Keyes KY-023 PS2 style 2-Axis Joystick
#ifdef JOYSTICK2
void IRAM_ATTR joystick2sw_isr()
{
  joy2swstate = !(digitalRead(JOYOTHERPIN));      // joy2swstate will be 1 when switch is pressed
}

void update_joystick2(void)
{
  int joyval;

  joyval = analogRead(JOYINOUTPIN);               // range is 0 - 4095, midpoint is 2047
  DebugPrint(JOYSTICKVALSTR);
  DebugPrintln(joyval);
  if ( joyval < (JZEROPOINT - JTHRESHOLD) )
  {
    ftargetPosition--;                            // move IN
    DebugPrint(JOYSTICKXINVALSTR);
    DebugPrint(joyval);
  }
  else if ( joyval > (JZEROPOINT + JTHRESHOLD) )
  {
    ftargetPosition++;                            // move OUT
    if ( ftargetPosition > mySetupData->get_maxstep())
    {
      ftargetPosition = mySetupData->get_maxstep();
    }
    joyval = joyval - (JZEROPOINT + JTHRESHOLD);
    if ( joyval < 0 )
    {
      joyval = JZEROPOINT - joyval;
    }
    DebugPrint(JOYSTICKXOUTVALSTR);
    DebugPrint(joyval);
  }

  if ( joy2swstate == 1)                          // switch is pressed
  {
    // user defined code here
    // could be a halt
    // could be a home
    // could be a preset
    // insert code here

    joy2swstate = 0;                              // finally reset joystick switch state
  }
}

void init_joystick2(void)
{
  pinMode(JOYINOUTPIN, INPUT);
  pinMode(JOYOTHERPIN, INPUT_PULLUP);
  // setup interrupt, falling, when switch is pressed, pin falls from high to low
  attachInterrupt(JOYOTHERPIN, joystick2sw_isr, FALLING);
  joy2swstate = 0;
}
#endif // #ifdef JOYSTICK2

#endif // #if defined(JOYSTICK1) || defined(JOYSTICK2)

// ----------------------------------------------------------------------------------------------
// 20: PUSHBUTTONS - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#ifdef PUSHBUTTONS
void init_pushbuttons(void)
{
  pinMode(INPBPIN, INPUT);
  pinMode(OUTPBPIN, INPUT);
}

void update_pushbuttons(void)
{
  long newpos;
  // PB are active high - pins float low if unconnected
  if ( digitalRead(INPBPIN) == 1 )                // is pushbutton pressed?
  {
    newpos = ftargetPosition - 1;
    newpos = (newpos < 0 ) ? 0 : newpos;
    ftargetPosition = newpos;
  }
  if ( digitalRead(OUTPBPIN) == 1 )
  {
    newpos = ftargetPosition + 1;
    // an unsigned long range is 0 to 4,294,967,295
    // when an unsigned long decrements from 0-1 it goes to largest +ve value, ie 4,294,967,295
    // which would in likely be much much greater than maxstep
    newpos = (newpos > (long) mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : newpos;
    ftargetPosition = newpos;
  }
}
#endif // PUSHBUTTONS

// ----------------------------------------------------------------------------------------------
// 21: mDNS SERVER - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#ifdef MDNSSERVER

#if defined(ESP8266)
#include <ESP8266mDNS.h>
#else
#include <ESPmDNS.h>
#endif

// MDNS service. find the device using dnsname.local
void start_mdns_service(void)
{
  // Set up mDNS responder:
  // the fully-qualified domain name is "mDNSNAME.local"
#if defined(ESP8266)
  if (!MDNS.begin(mDNSNAME, WiFi.localIP()))      // ESP8266 supports additional parameter for IP
#else
  if (!MDNS.begin(mDNSNAME))                      // ESP32 does not support IPaddress parameter
#endif
  {
    DebugPrintln(MDNSSTARTFAILSTR);
    mdnsserverstate = STOPPED;
  }
  else
  {
    DebugPrintln(MDNSSTARTEDSTR);
    // Add service to MDNS-SD, MDNS.addService(service, proto, port)
    MDNS.addService("http", "tcp", MDNSSERVERPORT);
    mdnsserverstate = RUNNING;
  }
  delay(10);                      // small pause so background tasks can run
}

void stop_mdns_service(void)
{
  DebugPrintln(STOPMDNSSERVERSTR);
  if ( mdnsserverstate == RUNNING )
  {
#if defined(ESP8266)
    // ignore
    // esp8266 library has no end() function to release mdns
#else
    MDNS.end();
#endif
    mdnsserverstate = STOPPED;
  }
  else
  {
    DebugPrintln(SERVERNOTRUNNINGSTR);
  }
  delay(10);                      // small pause so background tasks can run
}
#endif // #ifdef MDNSSERVER

// ----------------------------------------------------------------------------------------------
// 22: MANAGEMENT INTERFACE - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#ifdef MANAGEMENT

#if defined(ESP8266)
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif // if defined(esp8266)

#if defined(ESP8266)
#undef DEBUG_ESP_HTTP_SERVER
ESP8266WebServer mserver(MSSERVERPORT);
#else
WebServer mserver(MSSERVERPORT);
#endif // if defined(esp8266)

String MSpg;
File   fsUploadFile;

boolean ishexdigit( char c )
{
  if ( (c >= '0') && (c <= '9') )                 // is a digit
  {
    return true;
  }
  if ( (c >= 'a') && (c <= 'f') )                 // is a-f
  {
    return true;
  }
  if ( (c >= 'A') && (c <= 'F') )                 // is A-F
  {
    return true;
  }
  return false;
}

// convert the file extension to the MIME type
String MANAGEMENT_getcontenttype(String filename)
{
  String retval = "text/plain";
  if (filename.endsWith(".html"))
  {
    retval = "text/html";
  }
  else if (filename.endsWith(".css"))
  {
    retval = "text/css";
  }
  else if (filename.endsWith(".js"))
  {
    retval = "application/javascript";
  }
  else if (filename.endsWith(".ico"))
  {
    retval = "image/x-icon";
  }
  //retval = "application/octet-stream";
  return retval;
}

void MANAGEMENT_sendmyheader(void)
{
  //mserver.sendHeader(F(CACHECONTROLSTR), F(NOCACHENOSTORESTR));
  //mserver.sendHeader(F(PRAGMASTR), F(NOCACHESTR));
  //mserver.sendHeader(F(EXPIRESSTR), "-1");
  //mserver.setContentLength(CONTENT_LENGTH_UNKNOWN);
  //mserver.send(NORMALWEBPAGE, F(TEXTPAGETYPE), "");
  mserver.client().println("HTTP/1.1 200 OK");
  mserver.client().println("Content-type:text/html");
  //mserver.client().println("Connection: close");       // only valid on http/1.0
  mserver.client().println();
}

void MANAGEMENT_sendmycontent()
{
  mserver.client().print(MSpg);
}

// send the requested file to the client (if it exists)
bool MANAGEMENT_handlefileread(String path)
{
  DebugPrintln("handleFileRead: " + path);
  if (path.endsWith("/"))
  {
    path += "index.html";                               // if a folder is requested, send the index file
  }
  String contentType = MANAGEMENT_getcontenttype(path); // get the MIME type
  if ( SPIFFS.exists(path) )                            // if the file exists
  {
    File file = SPIFFS.open(path, "r");                 // open it
    if ( mySetupData->get_forcedownload() == 1)         // should the file be downloaded or displayed?
    {
      if ( path.indexOf(".html") == -1)
      {
        // if not an html file, force download : html files will be displayed in browser
        mserver.sendHeader("Content-Type", "application/octet-stream");
        mserver.sendHeader("Content-Disposition", "attachment");
      }
    }
    mserver.streamFile(file, contentType);              // and send it to the client
    file.close();                                       // then finish by closing the file
    return true;
  }
  else
  {
    TRACE();
    DebugPrintln(F(FILENOTFOUNDSTR));
    return false;                                       // if the file doesn't exist, return false
  }
}

void MANAGEMENT_checkreboot(void)
{
  String msg = mserver.arg("srestart");                 // if reboot controller
  if ( msg != "" )
  {
    String WaitPage = "<html><meta http-equiv=refresh content=\"" + String(MSREBOOTPAGEDELAY) + "\"><head><title>Management Server></title></head><body><p>Please wait, controller rebooting.</p></body></html>";
    mserver.send(NORMALWEBPAGE, TEXTPAGETYPE, WaitPage );
    WaitPage = "";
    delay(1000);                                        // wait for page to be sent
    software_Reboot(REBOOTDELAY);
  }
}

void MANAGEMENT_displaydeletepage()
{
  // spiffs was started earlier when server was started so assume it has started
  if ( SPIFFS.exists("/msdelete.html") )                // check for the webpage
  {
    File file = SPIFFS.open("/msdelete.html", "r");     // open it
    DebugPrintln(F(READPAGESTR));
    MSpg = file.readString();                           // read contents into string
    file.close();
    DebugPrintln(F(PROCESSPAGESTARTSTR));

    // Web page colors
    String bcol = mySetupData->get_wp_backcolor();
    MSpg.replace("%BKC%", bcol);
    String txtcol = mySetupData->get_wp_textcolor();
    MSpg.replace("%TXC%", txtcol);
    String ticol = mySetupData->get_wp_titlecolor();
    MSpg.replace("%TIC%", ticol);
    String hcol = mySetupData->get_wp_headercolor();
    MSpg.replace("%HEC%", hcol);
    DebugPrintln(F(PROCESSPAGEENDSTR));
  }
  else
  {
    TRACE();
    DebugPrintln(F(FSFILENOTFOUNDSTR));
    MSpg = FILENOTFOUNDSTR;
  }
  mserver.send(NORMALWEBPAGE, F(TEXTPAGETYPE), MSpg);
}

void MANAGEMENT_handledeletefile()
{
  String msg;
  String df = mserver.arg("fname");                     // check server arguments, df has filename
  if ( df != "" )                                       // check for file in spiffs
  {
    // spiffs was started earlier when server was started so assume it has started
    //df = "/" + df;
    if ( df[0] != '/')
    {
      df = '/' + df;
    }
    if ( SPIFFS.exists(df))
    {
      if ( SPIFFS.remove(df))
        msg = "The file is deleted: " + df;
      mserver.send(NORMALWEBPAGE, PLAINTEXTPAGETYPE, msg);
    }
    else
    {
      msg = String(FILENOTFOUNDSTR) + df;               // file does not exist
      mserver.send(NOTFOUNDWEBPAGE, PLAINTEXTPAGETYPE, msg);
    }
  }
  else
  {
    msg = "File field was empty";
    mserver.send(BADREQUESTWEBPAGE, PLAINTEXTPAGETYPE, msg);
  }
}

void MANAGEMENT_listFSfiles(void)
{
  // spiffs was started earlier when server was started so assume it has started
  // example code taken from FSBrowser
  String path = "/";
  DebugPrintln("MANAGEMENT_listFSfiles: " + path);
#if defined(ESP8266)
  String output = "{[";
  Dir dir = SPIFFS.openDir("/");
  while (dir.next())
  {
    output += "{" + dir.fileName() + "}, ";
  }
  output += "]}";
  mserver.send(NORMALWEBPAGE, String(JSONTEXTPAGETYPE), output);
#else // ESP32
  File root = SPIFFS.open(path);
  path = String();

  String output = "{[";
  if (root.isDirectory())
  {
    File file = root.openNextFile();
    while (file)
    {
      if (output != "[")
      {
        output += ',';
      }
      output += "{\"type\":\"";
      output += (file.isDirectory()) ? "dir" : "file";
      output += "\",\"name\":\"";
      output += String(file.name()).substring(1);
      output += "\"}";
      file = root.openNextFile();
    }
  }
  output += "]}";
  mserver.send(NORMALWEBPAGE, String(JSONTEXTPAGETYPE), output);
#endif
}

void MANAGEMENT_buildnotfound(void)
{
  // spiffs was started earlier when server was started so assume it has started
  if ( SPIFFS.exists("/msnotfound.html"))               // load page from fs - wsnotfound.html
  {
    // open file for read
    File file = SPIFFS.open("/msnotfound.html", "r");
    // read contents into string
    DebugPrintln(F(READPAGESTR));
    MSpg = file.readString();
    file.close();
    DebugPrintln(F(PROCESSPAGESTARTSTR));
    // process for dynamic data
    // Web page colors
    String bcol = mySetupData->get_wp_backcolor();
    MSpg.replace("%BKC%", bcol);
    String txtcol = mySetupData->get_wp_textcolor();
    MSpg.replace("%TXC%", txtcol);
    String ticol = mySetupData->get_wp_titlecolor();
    MSpg.replace("%TIC%", ticol);
    String hcol = mySetupData->get_wp_headercolor();
    MSpg.replace("%HEC%", hcol);
    MSpg.replace("%VER%", String(programVersion));
    MSpg.replace("%NAM%", String(DRVBRD_ID));
    // add code to handle reboot controller
    MSpg.replace("%BT%", String(CREBOOTSTR));
    MSpg.replace("%HEA%", String(ESP.getFreeHeap()));
    DebugPrintln(F(PROCESSPAGEENDSTR));
  }
  else
  {
    TRACE();
    DebugPrintln(F(FSFILENOTFOUNDSTR));
    MSpg = MANAGEMENTNOTFOUNDSTR;
  }
  delay(10);                                            // small pause so background tasks can run
}

void MANAGEMENT_handlenotfound(void)
{
  MANAGEMENT_checkreboot();                             // if reboot controller;
  MANAGEMENT_buildnotfound();
  mserver.send(NOTFOUNDWEBPAGE, TEXTPAGETYPE, MSpg);
  MSpg = "";
}

void MANAGEMENT_buildupload(void)
{
  // spiffs was started earlier when server was started so assume it has started
  if ( SPIFFS.exists("/msupload.html"))                 // load page from fs - wsupload.html
  {
    File file = SPIFFS.open("/msupload.html", "r");     // open file for read
    DebugPrintln(F(READPAGESTR));
    MSpg = file.readString();                           // read contents into string
    file.close();
    DebugPrintln(F(PROCESSPAGESTARTSTR));
    // process for dynamic data
    String bcol = mySetupData->get_wp_backcolor();
    MSpg.replace("%BKC%", bcol);
    String txtcol = mySetupData->get_wp_textcolor();
    MSpg.replace("%TXC%", txtcol);
    String ticol = mySetupData->get_wp_titlecolor();
    MSpg.replace("%TIC%", ticol);
    String hcol = mySetupData->get_wp_headercolor();
    MSpg.replace("%HEC%", hcol);
    MSpg.replace("%VER%", String(programVersion));
    MSpg.replace("%NAM%", String(DRVBRD_ID));
    // add code to handle reboot controller
    MSpg.replace("%BT%", String(CREBOOTSTR));
    DebugPrintln(F(PROCESSPAGEENDSTR));
  }
  else
  {
    TRACE();
    DebugPrintln(F(FSFILENOTFOUNDSTR));
    MSpg = MANAGEMENTNOTFOUNDSTR;
  }
  delay(10);                                            // small pause so background tasks can run
}

void MANAGEMENT_displayfileupload(void)
{
  MANAGEMENT_buildupload();
  mserver.send(NORMALWEBPAGE, TEXTPAGETYPE, MSpg);
  MSpg = "";
}

void MANAGEMENT_handlefileupload(void)
{
  HTTPUpload& upload = mserver.upload();
  if (upload.status == UPLOAD_FILE_START)
  {
    String filename = upload.filename;
    if (!filename.startsWith("/"))
    {
      filename = "/" + filename;
    }
    DebugPrint("handleFileUpload Name: ");
    DebugPrintln(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    if (fsUploadFile)
    {
      fsUploadFile.write(upload.buf, upload.currentSize);
    }
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    if (fsUploadFile)
    {
      // If the file was successfully created
      fsUploadFile.close();
      DebugPrint("handleFileUpload Size: ");
      DebugPrintln(upload.totalSize);
      mserver.sendHeader("Location", "/mssuccess.html");
      mserver.send(301);
    }
    else
    {
      mserver.send(INTERNALSERVERERROR, String(PLAINTEXTPAGETYPE), String(CANNOTCREATEFILESTR));
    }
  }
}

void MANAGEMENT_buildadminpg5(void)
{
#ifdef TIMEMSBUILDPG5
  Serial.print("ms_buildpg5: ");
  Serial.println(millis());
#endif
  if ( SPIFFS.exists("/msindex5.html"))                 // constructs admin page 5 of management server
  {
    DebugPrintln(F(FILEFOUNDSTR));
    File file = SPIFFS.open("/msindex5.html", "r");     // open file for read
    DebugPrintln(F(READPAGESTR));
    MSpg = file.readString();                           // read contents into string
    file.close();
    DebugPrintln(F(PROCESSPAGESTARTSTR));
    // process for dynamic data
    String bcol = mySetupData->get_wp_backcolor();
    MSpg.replace("%BKC%", bcol);
    String txtcol = mySetupData->get_wp_textcolor();
    MSpg.replace("%TXC%", txtcol);
    String ticol = mySetupData->get_wp_titlecolor();
    MSpg.replace("%TIC%", ticol);
    String hcol = mySetupData->get_wp_headercolor();
    MSpg.replace("%HEC%", hcol);
    MSpg.replace("%VER%", String(programVersion));
    MSpg.replace("%NAM%", String(DRVBRD_ID));

    // Display image for color picker

    MSpg.replace("%BT%", String(CREBOOTSTR));           // add code to handle reboot controller

    // display heap memory for tracking memory loss?
    // only esp32?
    MSpg.replace("%HEA%", String(ESP.getFreeHeap()));
    DebugPrintln(F(PROCESSPAGEENDSTR));
  }
  else
  {
    // could not read file
    TRACE();
    DebugPrintln(F(BUILDDEFAULTPAGESTR));
    MSpg = MANAGEMENTNOTFOUNDSTR;
  }
#ifdef TIMEMSBUILDPG5
  Serial.print("ms_buildpg5: ");
  Serial.println(millis());
#endif
  delay(10);                                            // small pause so background tasks can run
}

void MANAGEMENT_handleadminpg5(void)
{
#ifdef TIMEMSHANDLEPG5
  Serial.print("ms_handlepg5: ");
  Serial.println(millis());
#endif
  MANAGEMENT_checkreboot();                             // if reboot controller;

  // there are no parameters to check

  MANAGEMENT_sendadminpg5();
#ifdef TIMEMSHANDLEPG5
  Serial.print("ms_handlepg5: ");
  Serial.println(millis());
#endif
}

void MANAGEMENT_buildadminpg4(void)
{
#ifdef TIMEMSBUILDPG4
  Serial.print("ms_buildpg4: ");
  Serial.println(millis());
#endif
  if ( SPIFFS.exists("/msindex4.html"))                 // constructs admin page 4 of management server
  {
    DebugPrintln(F(FILEFOUNDSTR));
    File file = SPIFFS.open("/msindex4.html", "r");     // open file for read
    DebugPrintln(F(READPAGESTR));
    MSpg = file.readString();                           // read contents into string
    file.close();
    DebugPrintln(F(PROCESSPAGESTARTSTR));
    // process for dynamic data
    String bcol = mySetupData->get_wp_backcolor();
    MSpg.replace("%BKC%", bcol);
    String txtcol = mySetupData->get_wp_textcolor();
    MSpg.replace("%TXC%", txtcol);
    String ticol = mySetupData->get_wp_titlecolor();
    MSpg.replace("%TIC%", ticol);
    String hcol = mySetupData->get_wp_headercolor();
    MSpg.replace("%HEC%", hcol);
    MSpg.replace("%VER%", String(programVersion));
    MSpg.replace("%NAM%", String(DRVBRD_ID));

    // Background color %BC% default #333333
    // Main Page Title Color %MP% default #8e44ad
    // Header Group Color %HC% default #3399ff
    // Text Color %TC% default #5d6d7e
    String bcolor = "<form action=\"/msindex4\" method=\"post\"><input type=\"text\" name=\"bc\" size=\"6\" value=\"" + String(mySetupData->get_wp_backcolor()) + "\"><input type=\"submit\" name=\"bc\" value=\"SET\"></form>";
    MSpg.replace("%BC%", String(bcolor));
    String ticolor = "<form action=\"/msindex4\" method=\"post\"><input type=\"text\" name=\"ti\" size=\"6\" value=\"" + String(mySetupData->get_wp_titlecolor()) + "\"><input type=\"submit\" name=\"ti\" value=\"SET\"></form>";
    MSpg.replace("%TI%", String(ticolor));
    String hcolor = "<form action=\"/msindex4\" method=\"post\"><input type=\"text\" name=\"hc\" size=\"6\" value=\"" + String(mySetupData->get_wp_headercolor()) + "\"><input type=\"submit\" name=\"hc\" value=\"SET\"></form>";
    MSpg.replace("%HC%", String(hcolor));
    String tcolor = "<form action=\"/msindex4\" method=\"post\"><input type=\"text\" name=\"tc\" size=\"6\" value=\"" + String(mySetupData->get_wp_textcolor()) + "\"><input type=\"submit\" name=\"tc\" value=\"SET\"></form>";
    MSpg.replace("%TC%", String(tcolor));

    MSpg.replace("%BT%", String(CREBOOTSTR));           // add code to handle reboot controller

    TRACE();
    DebugPrintln(F(PROCESSPAGEENDSTR));

    // display heap memory for tracking memory loss?
    // only esp32?
    MSpg.replace("%HEA%", String(ESP.getFreeHeap()));
    DebugPrintln(F(PROCESSPAGEENDSTR));
  }
  else
  {
    // could not read index file from SPIFFS
    TRACE();
    DebugPrintln(F(BUILDDEFAULTPAGESTR));
    MSpg = MANAGEMENTNOTFOUNDSTR;
  }
#ifdef TIMEMSBUILDPG4
  Serial.print("ms_buildpg4: ");
  Serial.println(millis());
#endif
  delay(10);                                            // small pause so background tasks can run
}

void MANAGEMENT_handleadminpg4(void)
{
#ifdef TIMEMSHANDLEPG4
  Serial.print("ms_handlepg4: ");
  Serial.println(millis());
#endif
  // code here to handle a put request
  String msg;

  MANAGEMENT_checkreboot();                             // if reboot controller;

  // Handle update of webpage colors
  // Main Page Title Color <span id="TI">%MP%</span></p> #8e44ad
  // <p>Background Color <span id="BC">%BC%</span></p> #333333
  // <p>Header Group Color ><span id="HC">%HC%</span></p> #3399ff
  // <p>Text Color ><span id="TC">%TC%</span></p> #5d6d7e
  // get background color
  msg = mserver.arg("bc");
  if ( msg != "" )
  {
    boolean flag = true;
    String str = msg;
    int len = str.length();
    for ( int i = 0; i < len; i++ )
    {
      char ch = str[i];
      if ( ishexdigit(ch) == false )
      {
        flag = false;
      }
    }
    if ( flag == false )
    {
      DebugPrintln(F(BACKCOLORINVALIDSTR));
    }
    else
    {
      mySetupData->set_wp_backcolor(str);               // set the new background color
    }
  }

  // get text color
  msg = mserver.arg("tc");
  if ( msg != "" )
  {
    boolean flag = true;
    String str = msg;
    int len = str.length();
    for ( int i = 0; i < len; i++ )
    {
      char ch = str[i];
      if ( ishexdigit(ch) == false )
      {
        flag = false;
      }
    }
    if ( flag == false )
    {
      DebugPrintln(F(TEXTCOLORINVALIDSTR));
    }
    else
    {
      DebugPrint(F(NEWTEXTCOLORSTR));
      DebugPrintln(str);
      mySetupData->set_wp_textcolor(str);               // set the new text color
    }
  }

  // get header color
  msg = mserver.arg("hc");
  if ( msg != "" )
  {
    boolean flag = true;
    String str = msg;
    int len = str.length();
    for ( int i = 0; i < len; i++ )
    {
      char ch = str[i];
      if ( ishexdigit(ch) == false )
      {
        flag = false;
      }
    }
    if ( flag == false )
    {
      DebugPrintln(F(HEADERCOLORINVALIDSTR));
    }
    else
    {
      DebugPrint(F(NEWHEADERCOLORSTR));
      DebugPrintln(str);
      mySetupData->set_wp_headercolor(str);             // set the new header color
    }
  }

  // get page title color
  msg = mserver.arg("ti");
  if ( msg != "" )
  {
    boolean flag = true;
    String str = msg;
    int len = str.length();
    for ( int i = 0; i < len; i++ )
    {
      char ch = str[i];
      if ( ishexdigit(ch) == false )
      {
        flag = false;
      }
    }
    if ( flag == false )
    {
      DebugPrintln(F(TITLECOLORINVALIDSTR));
    }
    else
    {
      DebugPrint(F(NEWTITLECOLORSTR));
      DebugPrintln(str);
      mySetupData->set_wp_titlecolor(str);              // set the new header color
    }
  }
  MANAGEMENT_sendadminpg4();
#ifdef TIMEMSHANDLEPG4
  Serial.print("ms_handlepg4: ");
  Serial.println(millis());
#endif
}

void MANAGEMENT_buildadminpg3(void)
{
#ifdef TIMEMSBUILDPG3
  Serial.print("ms_buildpg3: ");
  Serial.println(millis());
#endif
  // spiffs was started earlier when server was started so assume it has started
  if ( SPIFFS.exists("/msindex3.html"))                 // constructs admin page 3 of management server
  {
    DebugPrintln(F(FILEFOUNDSTR));
    File file = SPIFFS.open("/msindex3.html", "r");     // open file for read
    DebugPrintln(F(READPAGESTR));
    MSpg = file.readString();                           // read contents into string
    file.close();
    DebugPrintln(F(PROCESSPAGESTARTSTR));
    // process for dynamic data
    String bcol = mySetupData->get_wp_backcolor();
    MSpg.replace("%BKC%", bcol);
    String txtcol = mySetupData->get_wp_textcolor();
    MSpg.replace("%TXC%", txtcol);
    String ticol = mySetupData->get_wp_titlecolor();
    MSpg.replace("%TIC%", ticol);
    String hcol = mySetupData->get_wp_headercolor();
    MSpg.replace("%HEC%", hcol);
    MSpg.replace("%VER%", String(programVersion));
    MSpg.replace("%NAM%", String(DRVBRD_ID));

    if ( mySetupData->get_backlash_in_enabled() )
    {
      MSpg.replace("%BIE%", String(DISABLEBKINSTR));
      MSpg.replace("%STI%", String(ENABLEDSTR));
    }
    else
    {
      MSpg.replace("%BIE%", String(ENABLEBKINSTR));
      MSpg.replace("%STI%", String(NOTENABLEDSTR));
    }
    if ( mySetupData->get_backlash_out_enabled() )
    {
      MSpg.replace("%BOE%", String(DISABLEBKOUTSTR));
      MSpg.replace("%STO%", String(ENABLEDSTR));
    }
    else
    {
      MSpg.replace("%BOE%", String(ENABLEBKOUTSTR));
      MSpg.replace("%STO%", String(NOTENABLEDSTR));
    }
    MSpg.replace("%BIS%", "<form action=\"/msindex3\" method =\"post\">BL-In &nbsp;Steps: <input type=\"text\" name=\"bis\" size=\"6\" value=" + String(mySetupData->get_backlashsteps_in()) + "> <input type=\"submit\" name=\"setbis\" value=\"Set\"></form>");
    MSpg.replace("%BOS%", "<form action=\"/msindex3\" method =\"post\">BL-Out Steps: <input type=\"text\" name=\"bos\" size=\"6\" value=" + String(mySetupData->get_backlashsteps_out()) + "> <input type=\"submit\" name=\"setbos\" value=\"Set\"></form>");

    MSpg.replace("%BT%", String(CREBOOTSTR));           // add code to handle reboot controller

    DebugPrintln(F(PROCESSPAGEENDSTR));

    // display heap memory for tracking memory loss?
    // only esp32?
    MSpg.replace("%HEA%", String(ESP.getFreeHeap()));
    DebugPrintln(F(PROCESSPAGEENDSTR));
  }
  else
  {
    // could not read file
    TRACE();
    DebugPrintln(F(BUILDDEFAULTPAGESTR));
    MSpg = MANAGEMENTNOTFOUNDSTR;
  }
#ifdef TIMEMSBUILDPG3
  Serial.print("ms_buildpg3: ");
  Serial.println(millis());
#endif
  delay(10);                                            // small pause so background tasks can run
}

void MANAGEMENT_handleadminpg3(void)
{
#ifdef TIMEMSHANDLEPG3
  Serial.print("ms_handlepg3: ");
  Serial.println(millis());
#endif
  // code here to handle a put request
  String msg;

  MANAGEMENT_checkreboot();                             // if reboot controller;

  // handle backlash
  // backlash in enable/disable, enin, diin
  msg = mserver.arg("enin");
  if ( msg != "" )
  {
    DebugPrintln(F("adminpg3: enin: "));
    mySetupData->set_backlash_in_enabled(1);
  }
  msg = mserver.arg("diin");
  if ( msg != "" )
  {
    DebugPrintln(F("adminpg3: diin: "));
    mySetupData->set_backlash_in_enabled(0);
  }

  // backlash out enable/disable, enou, diou
  msg = mserver.arg("enou");
  if ( msg != "" )
  {
    DebugPrintln(F("adminpg3: enou: "));
    mySetupData->set_backlash_out_enabled(1);
  }
  msg = mserver.arg("diou");
  if ( msg != "" )
  {
    DebugPrintln(F("adminpg3: diou: "));
    mySetupData->set_backlash_out_enabled(0);
  }

  // backlash in steps, setbis, bis,
  msg = mserver.arg("setbis");
  if ( msg != "" )
  {
    DebugPrintln(F("adminpg3: setbis: "));
    String st = mserver.arg("bis");
    DebugPrint(F("adminpg3: bis: "));
    DebugPrintln(st);
    byte steps = st.toInt();                            // no need to test for <0 and > 255 as it is a byte value
    mySetupData->set_backlashsteps_in(steps);
  }

  // backlash out steps, setbos, bos
  msg = mserver.arg("setbos");
  if ( msg != "" )
  {
    DebugPrintln(F("adminpg3: setbos: "));
    String st = mserver.arg("bos");
    DebugPrint(F("adminpg3: bos: "));
    DebugPrintln(st);
    byte steps = st.toInt();
    mySetupData->set_backlashsteps_out(steps);
  }

  MANAGEMENT_sendadminpg3();
#ifdef TIMEMSHANDLEPG3
  Serial.print("ms_handlepg3: ");
  Serial.println(millis());
#endif
}

void MANAGEMENT_buildadminpg2(void)
{
#ifdef TIMEMSBUILDPG2
  Serial.print("ms_buildpg2: ");
  Serial.println(millis());
#endif
  // spiffs was started earlier when server was started so assume it has started
  DebugPrintln(F("management: FS mounted"));            // constructs admin page 2 of management server
  if ( SPIFFS.exists("/msindex2.html"))
  {
    DebugPrintln(F(FILEFOUNDSTR));
    File file = SPIFFS.open("/msindex2.html", "r");     // open file for read
    DebugPrintln(F(READPAGESTR));
    MSpg = file.readString();                           // read contents into string
    file.close();

    DebugPrintln(F(PROCESSPAGESTARTSTR));
    // process for dynamic data
    String bcol = mySetupData->get_wp_backcolor();
    MSpg.replace("%BKC%", bcol);
    String txtcol = mySetupData->get_wp_textcolor();
    MSpg.replace("%TXC%", txtcol);
    String ticol = mySetupData->get_wp_titlecolor();
    MSpg.replace("%TIC%", ticol);
    String hcol = mySetupData->get_wp_headercolor();
    MSpg.replace("%HEC%", hcol);
    MSpg.replace("%VER%", String(programVersion));
    MSpg.replace("%NAM%", String(DRVBRD_ID));

    // tcp/ip server
#if defined(ACCESSPOINT) || defined(STATIONMODE)
    if ( tcpipserverstate == RUNNING )
    {
      MSpg.replace("%TBT%", String(STOPTSSTR));
      MSpg.replace("%TST%", SERVERSTATERUNSTR);
    }
    else
    {
      MSpg.replace("%TBT%", String(STARTTSSTR));
      MSpg.replace("%TST%", SERVERSTATESTOPSTR);
    }
#if defined(ESP8266)
    // esp8266 cannot change port of server
    String portstr = "Port: " + String(mySetupData->get_tcpipport());
    MSpg.replace("%TPO%", portstr );
#else
    // esp32
    MSpg.replace("%TPO%", "<form action=\"/msindex2\" method =\"post\">Port: <input type=\"text\" name=\"tp\" size=\"6\" value=" + String(mySetupData->get_tcpipport()) + "> <input type=\"submit\" name=\"settsport\" value=\"Set\"></form>");
#endif // #if defined(ESP8266)
#else
    MSpg.replace("%TPO%", "Port: " + String(mySetupData->get_tcpipport()));
    MSpg.replace("%TBT%", "Not defined");
#endif // #if defined(ACCESSPOINT) || defined(STATIONMODE)

    // Webserver status %WST%
    if ( webserverstate == RUNNING )
    {
      MSpg.replace("%WBT%", String(STOPWSSTR));
      MSpg.replace("%WST%", String(SERVERSTATERUNSTR));
    }
    else
    {
      MSpg.replace("%WBT%", String(STARTWSSTR));
      MSpg.replace("%WST%", String(SERVERSTATESTOPSTR));
    }
    // Webserver Port number %WPO%, %WBT%refresh Rate %WRA%
    MSpg.replace("%WPO%", "<form action=\"/msindex2\" method =\"post\">Port: <input type=\"text\" name=\"wp\" size=\"6\" value=" + String(mySetupData->get_webserverport()) + "> <input type=\"submit\" name=\"setwsport\" value=\"Set\"></form>");
    MSpg.replace("%WRA%", "<form action=\"/msindex2\" method =\"post\">Refresh Rate: <input type=\"text\" name=\"wr\" size=\"6\" value=" + String(mySetupData->get_webpagerefreshrate()) + "> <input type=\"submit\" name=\"setwsrate\" value=\"Set\"></form>");

    // ascom server start/stop service, Status %AST%, Port %APO%, Button %ABT%
    if ( ascomserverstate == RUNNING )
    {
      MSpg.replace("%AST%", String(STOPASSTR));
      MSpg.replace("%ABT%", String(SERVERSTATERUNSTR));
    }
    else
    {
      MSpg.replace("%AST%", String(STARTASSTR));
      MSpg.replace("%ABT%", String(SERVERSTATESTOPSTR));
    }
    MSpg.replace("%APO%", "<form action=\"/\" method =\"post\">Port: <input type=\"text\" name=\"ap\" size=\"8\" value=" + String(mySetupData->get_ascomalpacaport()) + "> <input type=\"submit\" name=\"setasport\" value=\"Set\"></form>");

    // TEMPERATURE PROBE ENABLE/DISABLE, State %TPE%, Button %TPO%
    if ( mySetupData->get_temperatureprobestate() == 1 )
    {
      MSpg.replace("%TPP%", String(DISABLETEMPSTR));    // button
      MSpg.replace("%TPE%", String(ENABLEDSTR));        // state
    }
    else
    {
      MSpg.replace("%TPP%", String(ENABLETEMPSTR));
      MSpg.replace("%TPE%", String(NOTENABLEDSTR));
    }
    // Temperature Mode %TEM%
    // Celcius=1, Fahrenheit=0
    if ( mySetupData->get_tempmode() == 1 )
    {
      // celsius - Change to Fahrenheit
      MSpg.replace("%TEM%", String(DISPLAYFSTR));
    }
    else
    {
      // Fahrenheit - change to celsius
      MSpg.replace("%TEM%", String(DISPLAYCSTR));
    }

    // INOUT LEDS ENABLE/DISABLE, State %INL%, Button %INO%
    if ( mySetupData->get_inoutledstate() == 1)
    {
      MSpg.replace("%INO%", String(DISABLELEDSTR));     // button
      MSpg.replace("%INL%", String(ENABLEDSTR));        // state
    }
    else
    {
      MSpg.replace("%INO%", String(ENABLELEDSTR));
      MSpg.replace("%INL%", String(NOTENABLEDSTR));
    }

    // add code to handle reboot controller %BT%
    MSpg.replace("%BT%", String(CREBOOTSTR));

    // display heap memory for tracking memory loss, %HEA%
    // only esp32?
    MSpg.replace("%HEA%", String(ESP.getFreeHeap()));
    DebugPrintln(F(PROCESSPAGEENDSTR));
  }
  else
  {
    // could not read file
    TRACE();
    DebugPrintln(F(BUILDDEFAULTPAGESTR));
    MSpg = MANAGEMENTNOTFOUNDSTR;
  }
#ifdef TIMEMSBUILDPG2
  Serial.print("ms_buildpg2: ");
  Serial.println(millis());
#endif
  delay(10);                                            // small pause so background tasks can run
}

void MANAGEMENT_handleadminpg2(void)
{
#ifdef TIMEMSHANDLEPG2
  Serial.print("ms_handlepg2: ");
  Serial.println(millis());
#endif
  // code here to handle a put request
  String msg;

  MANAGEMENT_checkreboot();                             // if reboot controller;

  // TCP/IP server START STOP
  msg = mserver.arg("startts");
  if ( msg != "" )
  {
    DebugPrintln(F("adminpg2: startts: "));
#if defined(ACCESSPOINT) || defined(STATIONMODE)
    start_tcpipserver();
#endif
  }
  msg = mserver.arg("stopts");
  if ( msg != "" )
  {
    DebugPrintln(F("adminpg2: stopts: "));
#if defined(ACCESSPOINT) || defined(STATIONMODE)
    stop_tcpipserver();
#endif
  }
  // tcpip server change port
  msg = mserver.arg("settsport");
  if ( msg != "" )
  {
    DebugPrint("set tcpip server port: ");
    DebugPrintln(msg);
    String tp = mserver.arg("tp");                      // process new webserver port number
    if ( tp != "" )
    {
      unsigned long newport = 0;
      DebugPrint("tp:");
      DebugPrintln(tp);
      newport = tp.toInt();
      if ( tcpipserverstate == STOPPED )
      {
        unsigned long currentport = mySetupData->get_tcpipport();
        if ( newport == currentport)
        {
          // port is the same so do not bother to change it
          DebugPrintln("tp err: new Port = current port");
        }
        else
        {
          if ( newport == MSSERVERPORT )                              // if same as management server
          {
            DebugPrintln("wp err: new Port = MSSERVERPORT");
          }
          else if ( newport == mySetupData->get_ascomalpacaport() )   // if same as ASCOM REMOTE server
          {
            DebugPrintln("wp err: new Port = ALPACAPORT");
          }
          else if ( newport == mySetupData->get_mdnsport() )          // if same as mDNS server
          {
            DebugPrintln("wp err: new Port = MDNSSERVERPORT");
          }
          else if ( newport == mySetupData->get_webserverport() )     // if same as mDNS server
          {
            DebugPrintln("wp err: new Port = WEBSERVERPORT");
          }
          else
          {
            DebugPrintln("New tcpipserver port = " + String(newport));
            mySetupData->set_tcpipport(newport);                      // assign new port and save it
          }
        }
      }
      else
      {
        DebugPrintln("Attempt to change tcpipserver port when tcpipserver running");
      }
    }
  }

  // webserver START STOP service
  msg = mserver.arg("startws");
  if ( msg != "" )
  {
    DebugPrintln(F("adminpg2: startws: "));
    if ( webserverstate == STOPPED)
    {
      start_webserver();
      mySetupData->set_webserverstate(1);
    }
  }
  msg = mserver.arg("stopws");
  if ( msg != "" )
  {
    DebugPrintln(F("adminpg2: stopws: "));
    if ( webserverstate == RUNNING )
    {
      stop_webserver();
      mySetupData->set_webserverstate(0);
    }
  }
  // webserver change port - we should be able to change port if not running or enabled or not enabled
  msg = mserver.arg("setwsport");
  if ( msg != "" )
  {
    DebugPrint("set web server port: ");
    DebugPrintln(msg);
    String wp = mserver.arg("wp");                                    // process new webserver port number
    if ( wp != "" )
    {
      unsigned long newport = 0;
      DebugPrint("wp:");
      DebugPrintln(wp);
      newport = wp.toInt();
      if ( webserverstate == STOPPED )
      {
        unsigned long currentport = mySetupData->get_webserverport();
        if ( newport == currentport)
        {
          DebugPrintln("wp err: new Port = current port");            // port is the same so do not bother to change it
        }
        else
        {
          if ( newport == MSSERVERPORT )                              // if same as management server
          {
            DebugPrintln("wp err: new Port = MSSERVERPORT");
          }
          else if ( newport == mySetupData->get_ascomalpacaport() )   // if same as ASCOM REMOTE server
          {
            DebugPrintln("wp err: new Port = ALPACAPORT");
          }
          else if ( newport == mySetupData->get_mdnsport() )          // if same as mDNS server
          {
            DebugPrintln("wp err: new Port = MDNSSERVERPORT");
          }
          else if ( newport == mySetupData->get_tcpipport() )         // if same as tcpip server
          {
            DebugPrintln("wp err: new Port = SERVERPORT");
          }
          else
          {
            DebugPrintln("New webserver port = " + String(newport));
            mySetupData->set_webserverport(newport);                  // assign new port and save it
          }
        }
      }
      else
      {
        DebugPrintln("Attempt to change webserver port when webserver running");
      }
    }
  }
  // web page refresh rate - should be able to change at any time
  msg = mserver.arg("setwsrate");
  if ( msg != "" )
  {
    DebugPrint("set web server page refresh rate: ");
    DebugPrintln(msg);
    String wr = mserver.arg("wr");                      // process new webserver page refresh rate
    if ( wr != "" )
    {
      int newrate = 0;
      DebugPrint("wr:");
      DebugPrintln(wr);
      newrate = wr.toInt();
      int currentrate = mySetupData->get_webpagerefreshrate();
      if ( newrate == currentrate)
      {
        // port is the same so do not bother to change it
        DebugPrintln("wr err: new page refresh rate = current page refresh rate");
      }
      else
      {
        if ( newrate < MINREFRESHPAGERATE )
        {
          DebugPrintln("wr err: Page refresh rate too low");
          newrate = MINREFRESHPAGERATE;
        }
        else if ( newrate > MAXREFRESHPAGERATE )
        {
          DebugPrintln("wr err: Page refresh rate too high");
          newrate = MAXREFRESHPAGERATE;
        }
        DebugPrintln("New page refresh rate = " + String(newrate));
        mySetupData->set_webpagerefreshrate(newrate);                // assign new refresh rate and save it
      }
    }
  }

  // ascomserver start/stop service
  msg = mserver.arg("startas");
  if ( msg != "" )
  {
    DebugPrintln(F("adminpg2: startas: "));
    if ( ascomserverstate == STOPPED )
    {
      start_ascomremoteserver();
      mySetupData->set_ascomserverstate(1);
    }
  }
  msg = mserver.arg("stopas");
  if ( msg != "" )
  {
    DebugPrintln(F("adminpg2: stopas: "));
    if ( ascomserverstate == RUNNING)
    {
      stop_ascomremoteserver();
      mySetupData->set_ascomserverstate(0);
    }
  }
  // ascom server port
  msg = mserver.arg("setasport");
  if ( msg != "" )
  {
    DebugPrint("set ascom server port: ");
    DebugPrintln(msg);
    String ap = mserver.arg("ap");                                    // process new ascomalpaca port number
    if ( ap != "" )
    {
      unsigned long newport = 0;
      DebugPrint("ap:");
      DebugPrintln(ap);
      newport = ap.toInt();
      if ( ascomserverstate == STOPPED )
      {
        unsigned long currentport = mySetupData->get_ascomalpacaport();
        if ( newport == currentport)
        {
          // port is the same so do not bother to change it
          DebugPrintln("ap error: new Port = current port");
        }
        else
        {
          if ( newport == MSSERVERPORT )                              // if same as management server
          {
            DebugPrintln("wp err: new Port = MSSERVERPORT");
          }
          else if ( newport == mySetupData->get_webserverport() )     // if same as webserver
          {
            DebugPrintln("wp err: new Port = ALPACAPORT");
          }
          else if ( newport == mySetupData->get_mdnsport() )          // if same as mDNS server
          {
            DebugPrintln("wp err: new Port = MDNSSERVERPORT");
          }
          else if ( newport == mySetupData->get_tcpipport() )          // if same as tcpip server
          {
            DebugPrintln("wp err: new Port = SERVERPORT");
          }
          else
          {
            DebugPrintln("New ascomalpaca port = " + String(newport));
            mySetupData->set_ascomalpacaport(newport);                  // assign new port and save it
          }
        }
      }
      else
      {
        DebugPrintln("Attempt to change ascomalpaca port when ascomserver running");
      }
    }
  }

  // Temperature Probe ENABLE/DISABLE, starttp, stoptp
  msg = mserver.arg("starttp");
  if ( msg != "" )
  {
    // check if already enabled
    if (mySetupData->get_temperatureprobestate() == 1)
    {
      // nothing to do, ignore
    }
    else
    {
      mySetupData->set_temperatureprobestate(1);
      init_temp();                                                    // we need to reinitialise it
    }
  }
  msg = mserver.arg("stoptp");
  if ( msg != "" )
  {
    if ( mySetupData->get_temperatureprobestate() == 1 )
    {
      // there is no destructor call
      mySetupData->set_temperatureprobestate(0);
    }
    else
    {
      // do nothing, already disabled
    }
  }
  // Temperature probe celsius/farentheit
  msg = mserver.arg("tm");
  if ( msg != "" )
  {
    DebugPrint("Set temp mode: ");
    DebugPrintln(msg);
    if ( msg == "cel" )
    {
      mySetupData->set_tempmode(1);
    }
    else if ( msg == "fah" )
    {
      mySetupData->set_tempmode(0);
    }
  }

  // LEDS ENABLE/DISABLE startle, stople
  msg = mserver.arg("startle");
  if ( msg != "" )
  {
    // if disabled then enable
    if ( mySetupData->get_inoutledstate() == 0)
    {
      mySetupData->set_inoutledstate(1);
      // reinitialise pins
#if (DRVBRD == PRO2ESP32ULN2003 || DRVBRD == PRO2ESP32L298N || DRVBRD == PRO2ESP32L293DMINI || DRVBRD == PRO2ESP32L9110S) || (DRVBRD == PRO2ESP32DRV8825 )
      init_leds();
#endif
    }
  }
  msg = mserver.arg("stople");
  if ( msg != "" )
  {
    // if enabled then disable
    if ( mySetupData->get_inoutledstate() == 1)
    {
      mySetupData->set_inoutledstate(0);
    }
  }
  MANAGEMENT_sendadminpg2();
#ifdef TIMEMSHANDLEPG2
  Serial.print("ms_handlepg2: ");
  Serial.println(millis());
#endif
}

void MANAGEMENT_buildadminpg1(void)
{
#ifdef TIMEMSBUILDPG1
  Serial.print("ms_buildpg1: ");
  Serial.println(millis());
#endif
  // spiffs was started earlier when server was started so assume it has started
  if ( SPIFFS.exists("/msindex1.html"))                 // constructs home page of management server
  {
    DebugPrintln(F(FILEFOUNDSTR));
    File file = SPIFFS.open("/msindex1.html", "r");     // open file for read
    DebugPrintln(F(READPAGESTR));
    MSpg = file.readString();                           // read contents into string
    file.close();

    DebugPrintln(F(PROCESSPAGESTARTSTR));
    // process for dynamic data
    String bcol = mySetupData->get_wp_backcolor();
    MSpg.replace("%BKC%", bcol);
    String txtcol = mySetupData->get_wp_textcolor();
    MSpg.replace("%TXC%", txtcol);
    String ticol = mySetupData->get_wp_titlecolor();
    MSpg.replace("%TIC%", ticol);
    String hcol = mySetupData->get_wp_headercolor();
    MSpg.replace("%HEC%", hcol);
    MSpg.replace("%VER%", String(programVersion));
    MSpg.replace("%NAM%", String(DRVBRD_ID));
#ifdef BLUETOOTHMODE
    MSpg.replace("%MOD%", "BLUETOOTH : " + String(BLUETOOTHNAME));
#endif
#ifdef ACCESSPOINT
    MSpg.replace("%MOD%", "ACCESSPOINT");
#endif
#ifdef STATIONMODE
    MSpg.replace("%MOD%", "STATIONMODE");
#endif
#ifdef LOCALSERIAL
    MSpg.replace("%MOD%", "LOCALSERIAL");
#endif

#ifdef MDNSSERVER
    if ( mdnsserverstate == RUNNING)
    {
      MSpg.replace("%MST%", "RUNNING");
    }
    else
    {
      MSpg.replace("%MST%", "STOPPED");
    }
    MSpg.replace("%MPO%", "<form action=\"/\" method =\"post\">Port: <input type=\"text\" name=\"mdnsp\" size=\"8\" value=" + String(mySetupData->get_mdnsport()) + "> <input type=\"submit\" name=\"setmdnsport\" value=\"Set\"></form>");
    if ( mdnsserverstate == RUNNING)
    {
      MSpg.replace("%MBT%", "STOP");
    }
    else
    {
      MSpg.replace("%MBT%", "START");
    }
#else
    MSpg.replace("%MST%", "Not defined");
    MSpg.replace("%MPO%", "Port: " + String(mySetupData->get_mdnsport()));
    MSpg.replace("%MBT%", " ");
#endif

#ifdef OTAUPDATES
    if ( otaupdatestate == RUNNING )
    {
      MSpg.replace("%OST%", "RUNNING");
    }
    else
    {
      MSpg.replace("%OST%", "STOPPED");
    }
#else
    MSpg.replace("%OST%", "Not defined");
#endif

#ifdef USEDUCKDNS
    if ( duckdnsstate == RUNNING )
    {
      MSpg.replace("%DST%", "RUNNING");
    }
    else
    {
      MSpg.replace("%DST%", "STOPPED");
    }
#else
    MSpg.replace("%DST%", "Not defined");
#endif

    // staticip %IPS%
    if ( staticip == STATICIPON )
    {
      MSpg.replace("%IPS%", "ON");
    }
    else
    {
      MSpg.replace("%IPS%", "OFF");
    }

    // display %OLE%
#if defined(OLEDTEXT) || defined(OLEDGRAPHICS)
    if ( mySetupData->get_displayenabled() == 1 )
    {
      MSpg.replace("%OLE%", String(DISPLAYONSTR));      // checked already
    }
    else
    {
      MSpg.replace("%OLE%", String(DISPLAYOFFSTR));     // not checked
    }
#else
    MSpg.replace("%OLE%", "<b>Display:</b> not defined");
#endif

    // startscreen %SS%
    if ( mySetupData->get_showstartscreen() == 1 )
    {
      // checked already
      MSpg.replace("%SS%", String(STARTSCREENONSTR));
    }
    else
    {
      // not checked
      MSpg.replace("%SS%", String(STARTSCREENOFFSTR));
    }

    // FORCEMANAGEMENTDOWNLOAD %MDL%
    if ( mySetupData->get_forcedownload() == 1 )
    {
      // checked already
      MSpg.replace("%MDL%", String(STARTFMDLONSTR));
    }
    else
    {
      // not checked
      MSpg.replace("%MDL%", String(STARTFMDLOFFSTR));
    }

    // SHOWHPSWMESSAGES %HPM%
    if ( mySetupData->get_showhpswmsg() == 1 )
    {
      // checked already
      MSpg.replace("%HPM%", String(STARTHPSWMONSTR));
    }
    else
    {
      // not checked
      MSpg.replace("%HPM%", String(STARTHPSWMOFFSTR));
    }

    MSpg.replace("%BT%", String(CREBOOTSTR));           // add code to handle reboot controller

    // display heap memory for tracking memory loss?
    // only esp32?
    MSpg.replace("%HEA%", String(ESP.getFreeHeap()));
    DebugPrintln(F(PROCESSPAGEENDSTR));
  }
  else
  {
    // could not read file
    TRACE();
    DebugPrintln(F(BUILDDEFAULTPAGESTR));
    MSpg = MANAGEMENTNOTFOUNDSTR;
  }
#ifdef TIMEMSBUILDPG1
  Serial.print("ms_buildpg1: ");
  Serial.println(millis());
#endif
  delay(10);                                            // small pause so background tasks can run
}

void MANAGEMENT_handleadminpg1(void)
{
#ifdef TIMEMSHANDLEPG1
  Serial.print("ms_handlepg1: ");
  Serial.println(millis());
#endif
  // code here to handle a put request
  String msg;

  MANAGEMENT_checkreboot();                              // if reboot controller;

  msg = mserver.arg("reboot");
  if ( msg != "" )
  {
    DebugPrintln(F("MANAGEMENT_handleadminpg1: reboot controller: "));
    // services are stopped in the reboot() code

    String WaitPage = "<html><meta http-equiv=refresh content=\"" + String(MSREBOOTPAGEDELAY) + "\"><head><title>Management Server></title></head><body><p>Please wait, controller rebooting.</p></body></html>";
    mserver.send(NORMALWEBPAGE, TEXTPAGETYPE, WaitPage );
    software_Reboot(REBOOTDELAY);
  }

  // mdns server
  msg = mserver.arg("startmdns");
  if ( msg != "" )
  {
    DebugPrintln(F("MANAGEMENT_handleadminpg1: startmdns: "));
#ifdef MDNSSERVER
    start_mdns_service();
#endif
  }
  msg = mserver.arg("stopmdns");
  if ( msg != "" )
  {
    DebugPrintln(F("MANAGEMENT_handleadminpg1: stopmdns: "));
#ifdef MDNSSERVER
    stop_mdns_service();
#endif
  }
  // mdns port
  msg = mserver.arg("setmdnsport");
  if ( msg != "" )
  {
    DebugPrint("set web server port: ");
    DebugPrintln(msg);
    String mp = mserver.arg("mdnsp");                                 // process new webserver port number
    if ( mp != "" )
    {
      unsigned long newport = 0;
      DebugPrint("mp:");
      DebugPrintln(mp);
      newport = mp.toInt();
      if ( mdnsserverstate == STOPPED )
      {
        unsigned long currentport = mySetupData->get_mdnsport();
        if ( newport == currentport)
        {
          // port is the same so do not bother to change it
          DebugPrintln("mp err: new Port = current port");
        }
        else
        {
          if ( newport == MSSERVERPORT )                              // if same as management server
          {
            DebugPrintln("mp err: new Port = MSSERVERPORT");
          }
          else if ( newport == mySetupData->get_ascomalpacaport() )   // if same as ASCOM REMOTE server
          {
            DebugPrintln("mp err: new Port = ALPACAPORT");
          }
          else if ( newport == mySetupData->get_webserverport() )     // if same as web server
          {
            DebugPrintln("mp err: new Port = WEBSERVERPORT");
          }
          else if ( newport == mySetupData->get_tcpipport() )        // if same as tcpip server
          {
            DebugPrintln("wp err: new Port = SERVERPORT");
          }
          else
          {
            DebugPrintln("New webserver port = " + String(newport));
            mySetupData->set_mdnsport(newport);                       // assign new port and save it
          }
        }
      }
      else
      {
        DebugPrintln("Attempt to change mdnsserver port when mdnsserver running");
      }
    }
  }

  // if update display state
  msg = mserver.arg("di");
  if ( msg != "" )
  {
    DebugPrint("Set display state: ");
    DebugPrintln(msg);
    if ( msg == "don" )
    {
      mySetupData->set_displayenabled(1);
#ifdef OLEDTEXT
      myoled->Display_On();
#endif
    }
    else
    {
      mySetupData->set_displayenabled(0);
#ifdef OLEDTEXT
      myoled->Display_Off();
#endif
    }
  }

  // if update start screen
  msg = mserver.arg("ss");
  if ( msg != "" )
  {
    DebugPrint("Set start screen state: ");
    DebugPrintln(msg);
    if ( msg == "sson" )
    {
      mySetupData->set_showstartscreen(1);
    }
    else
    {
      mySetupData->set_showstartscreen(0);
    }
  }

  // if update home position switch messages
  msg = mserver.arg("hp");
  if ( msg != "" )
  {
    DebugPrint("Set hpswmsg state: ");
    DebugPrintln(msg);
    if ( msg == "hpon" )
    {
      mySetupData->set_showhpswmsg(1);
    }
    else
    {
      mySetupData->set_showhpswmsg(0);
    }
  }

  // if update force management server download
  msg = mserver.arg("fd");
  if ( msg != "" )
  {
    DebugPrint("Set ms_force download state: ");
    DebugPrintln(msg);
    if ( msg == "fdon" )
    {
      mySetupData->set_forcedownload(1);
    }
    else
    {
      mySetupData->set_forcedownload(0);
    }
  }

  // OTA, DuckDNS, StaticIP are status only so no need to check or update here

  MANAGEMENT_sendadminpg1();
#ifdef TIMEMSHANDLEPG1
  Serial.print("ms_handlepg1: ");
  Serial.println(millis());
#endif
}

void MANAGEMENT_sendadminpg5(void)
{
#ifdef TIMEMSSENDPG5
  Serial.print("ms_sendpg5: ");
  Serial.println(millis());
#endif
  MANAGEMENT_buildadminpg5();
  DebugPrintln("root() - send admin pg5");
  MANAGEMENT_sendmyheader();
  MANAGEMENT_sendmycontent();
  MSpg = "";
#ifdef TIMEMSSENDPG5
  Serial.print("ms_sendpg5: ");
  Serial.println(millis());
#endif
  delay(10);
}

void MANAGEMENT_sendadminpg4(void)
{
#ifdef TIMEMSSENDPG4
  Serial.print("ms_sendpg4: ");
  Serial.println(millis());
#endif
  MANAGEMENT_buildadminpg4();
  DebugPrintln("root() - send admin pg4");
  MANAGEMENT_sendmyheader();
  MANAGEMENT_sendmycontent();
  MSpg = "";
#ifdef TIMEMSSENDPG4
  Serial.print("ms_sendpg4: ");
  Serial.println(millis());
#endif
  delay(10);
}

void MANAGEMENT_sendadminpg3(void)
{
#ifdef TIMEMSSENDPG3
  Serial.print("ms_sendpg3: ");
  Serial.println(millis());
#endif
  MANAGEMENT_buildadminpg3();
  DebugPrintln("root() - send admin pg3");
  MANAGEMENT_sendmyheader();
  MANAGEMENT_sendmycontent();
  MSpg = "";
#ifdef TIMEMSSENDPG3
  Serial.print("ms_sendpg3: ");
  Serial.println(millis());
#endif
  delay(10);
}

void MANAGEMENT_sendadminpg2(void)
{
#ifdef TIMEMSSENDPG2
  Serial.print("ms_sendpg2: ");
  Serial.println(millis());
#endif
  MANAGEMENT_buildadminpg2();
  DebugPrintln("root() - send admin pg2");
  MANAGEMENT_sendmyheader();
  MANAGEMENT_sendmycontent();
  MSpg = "";
#ifdef TIMEMSSENDPG2
  Serial.print("ms_sendpg2: ");
  Serial.println(millis());
#endif
  delay(10);
}

void MANAGEMENT_sendadminpg1(void)
{
#ifdef TIMEMSSENDPG1
  Serial.print("ms_sendpg1: ");
  Serial.println(millis());
#endif
  MANAGEMENT_buildadminpg1();
  DebugPrintln("root() - send admin pg1");
  MANAGEMENT_sendmyheader();
  MANAGEMENT_sendmycontent();
  MSpg = "";
#ifdef TIMEMSSENDPG1
  Serial.print("ms_sendpg1: ");
  Serial.println(millis());
#endif
  delay(10);
}

void start_management(void)
{
  if ( !SPIFFS.begin() )
  {
    TRACE();
    DebugPrintln(F(FSNOTSTARTEDSTR));
    DebugPrintln(F(SERVERSTATESTOPSTR));
    managementserverstate = STOPPED;
    return;
  }
  MSpg.reserve(MAXMANAGEMENTPAGESIZE);
  mserver.on("/",         HTTP_GET,  MANAGEMENT_sendadminpg1);
  mserver.on("/",         HTTP_POST, MANAGEMENT_handleadminpg1);
  mserver.on("/msindex1", HTTP_GET,  MANAGEMENT_sendadminpg1);
  mserver.on("/msindex1", HTTP_POST, MANAGEMENT_handleadminpg1);
  mserver.on("/msindex2", HTTP_GET,  MANAGEMENT_sendadminpg2);
  mserver.on("/msindex2", HTTP_POST, MANAGEMENT_handleadminpg2);
  mserver.on("/msindex3", HTTP_GET,  MANAGEMENT_sendadminpg3);
  mserver.on("/msindex3", HTTP_POST, MANAGEMENT_handleadminpg3);
  mserver.on("/msindex4", HTTP_GET,  MANAGEMENT_sendadminpg4);            // web colors
  mserver.on("/msindex4", HTTP_POST, MANAGEMENT_handleadminpg4);
  mserver.on("/color",    HTTP_GET,  MANAGEMENT_sendadminpg5);
  mserver.on("/color",    HTTP_POST, MANAGEMENT_handleadminpg5);          // color picker
  mserver.on("/delete",   HTTP_GET,  MANAGEMENT_displaydeletepage);
  mserver.on("/delete",   HTTP_POST, MANAGEMENT_handledeletefile);
  mserver.on("/list",     HTTP_GET,  MANAGEMENT_listFSfiles);
  mserver.on("/upload",   HTTP_GET,  MANAGEMENT_displayfileupload);
  mserver.on("/upload",   HTTP_POST, []() {
    mserver.send(NORMALWEBPAGE);
  }, MANAGEMENT_handlefileupload );
  mserver.onNotFound([]() {                             // if the client requests any URI
    if (!MANAGEMENT_handlefileread(mserver.uri()))      // send file if it exists
    {
      MANAGEMENT_handlenotfound();                      // otherwise, respond with a 404 (Not Found) error
    }
  });
  mserver.begin();
  managementserverstate = RUNNING;
  TRACE();
  DebugPrintln(F(SERVERSTATESTARTSTR));
  delay(10);                                            // small pause so background tasks can run
}

void stop_management(void)
{
  if ( managementserverstate == RUNNING )
  {
    mserver.stop();
    managementserverstate = STOPPED;
    TRACE();
    DebugPrintln(F(SERVERSTATESTOPSTR));
  }
  else
  {
    DebugPrintln(F(SERVERNOTRUNNINGSTR));
  }
}
#endif // #ifdef MANAGEMENT

// ----------------------------------------------------------------------------------------------
// 23: WEBSERVER - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#if defined(ESP8266)
#undef DEBUG_ESP_HTTP_SERVER
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif // if defined(esp8266)

#include "webserver.h"
#if defined(ESP8266)
#undef DEBUG_ESP_HTTP_SERVER
ESP8266WebServer *webserver;
#else
WebServer *webserver;
#endif // if defined(esp8266)
String WSpg;

void WEBSERVER_sendACAOheader(void)
{
  webserver->sendHeader("Access-Control-Allow-Origin", "*");
}

void WEBSERVER_sendmyheader(void)
{
  //webserver->sendHeader(F(CACHECONTROLSTR), F(NOCACHENOSTORESTR));
  //webserver->sendHeader(F(PRAGMASTR), F(NOCACHESTR));
  //webserver->sendHeader(F(EXPIRESSTR), "-1");
  //webserver->setContentLength(CONTENT_LENGTH_UNKNOWN);
  //webserver->send(NORMALWEBPAGE, F(TEXTPAGETYPE), "");
  webserver->client().println("HTTP/1.1 200 OK");
  webserver->client().println("Content-type:text/html");
  //webserver->client().println("Connection: close");       // only valid on http/1.0
  webserver->client().println();
}

void WEBSERVER_sendmycontent()
{
  webserver->client().print(WSpg);
}

void WEBSERVER_buildnotfound(void)
{
  // spiffs was started earlier when server was started so assume it has started
  if ( SPIFFS.exists("/wsnotfound.html"))               // load page from fs - wsnotfound.html
  {
    File file = SPIFFS.open("/wsnotfound.html", "r");   // open file for read
    DebugPrintln(F(READPAGESTR));
    WSpg = file.readString();                           // read contents into string
    file.close();

    DebugPrintln(F(PROCESSPAGESTARTSTR));
    // process for dynamic data
    WSpg.replace("%IP%", ipStr);
    WSpg.replace("%POR%", String(mySetupData->get_webserverport()));
    WSpg.replace("%VER%", String(programVersion));
    WSpg.replace("%NAM%", String(DRVBRD_ID));
    String bcol = mySetupData->get_wp_backcolor();
    WSpg.replace("%BKC%", bcol);
    String txtcol = mySetupData->get_wp_textcolor();
    WSpg.replace("%TXC%", txtcol);
    String ticol = mySetupData->get_wp_titlecolor();
    WSpg.replace("%TIC%", ticol);
    String hcol = mySetupData->get_wp_headercolor();
    WSpg.replace("%HEC%", hcol);
    DebugPrintln(F(PROCESSPAGEENDSTR));
  }
  else
  {
    TRACE();
    DebugPrintln(F(FSFILENOTFOUNDSTR));
    DebugPrintln(F(BUILDDEFAULTPAGESTR));
    WSpg = WEBSERVERNOTFOUNDSTR;
  }
  delay(10);                                            // small pause so background tasks can run
}

void WEBSERVER_handlenotfound(void)
{
  WEBSERVER_buildnotfound();
  webserver->send(NOTFOUNDWEBPAGE, TEXTPAGETYPE, WSpg);
  WSpg = "";
}

void WEBSERVER_buildpresets(void)
{
#ifdef TIMEWSBUILDPRESETS
  Serial.print("ws_buildpresets: ");
  Serial.println(millis());
#endif
  // spiffs was started earlier when server was started so assume it has started
  if ( SPIFFS.exists("/wspresets.html"))                // load page from fs - wspresets.html
  {
    File file = SPIFFS.open("/wspresets.html", "r");    // open file for read
    DebugPrintln(F(READPAGESTR));
    WSpg = file.readString();                           // read contents into string
    file.close();

    DebugPrintln(F(PROCESSPAGESTARTSTR));
    // process for dynamic data
    String bcol = mySetupData->get_wp_backcolor();
    WSpg.replace("%BKC%", bcol);
    String txtcol = mySetupData->get_wp_textcolor();
    WSpg.replace("%TXC%", txtcol);
    String ticol = mySetupData->get_wp_titlecolor();
    WSpg.replace("%TIC%", ticol);
    String hcol = mySetupData->get_wp_headercolor();
    WSpg.replace("%HEC%", hcol);
    WSpg.replace("%RAT%", String(mySetupData->get_webpagerefreshrate()));
    WSpg.replace("%IP%", ipStr);
    WSpg.replace("%POR%", String(mySetupData->get_webserverport()));
    WSpg.replace("%VER%", String(programVersion));
    WSpg.replace("%NAM%", String(DRVBRD_ID));
    WSpg.replace("%CPO%", String(fcurrentPosition));
    WSpg.replace("%TPO%", String(ftargetPosition));
    WSpg.replace("%MOV%", String(isMoving));

    WSpg.replace("%WSP0%", String(mySetupData->get_focuserpreset(0)));
    WSpg.replace("%WSP1%", String(mySetupData->get_focuserpreset(1)));
    WSpg.replace("%WSP2%", String(mySetupData->get_focuserpreset(2)));
    WSpg.replace("%WSP3%", String(mySetupData->get_focuserpreset(3)));
    WSpg.replace("%WSP4%", String(mySetupData->get_focuserpreset(4)));
    WSpg.replace("%WSP5%", String(mySetupData->get_focuserpreset(5)));
    WSpg.replace("%WSP6%", String(mySetupData->get_focuserpreset(6)));
    WSpg.replace("%WSP7%", String(mySetupData->get_focuserpreset(7)));
    WSpg.replace("%WSP8%", String(mySetupData->get_focuserpreset(8)));
    WSpg.replace("%WSP9%", String(mySetupData->get_focuserpreset(9)));
    DebugPrintln(F(PROCESSPAGEENDSTR));
  }
  else
  {
    // could not read preset file from SPIFFS
    TRACE();
    DebugPrintln(F(FSFILENOTFOUNDSTR));
    DebugPrintln(F(BUILDDEFAULTPAGESTR));
    WSpg = WEBSERVERNOTFOUNDSTR;
  }
#ifdef TIMEWSBUILDPRESETS
  Serial.print("ws_buildpresets: ");
  Serial.println(millis());
#endif
  delay(10);                                            // small pause so background tasks can run
}

void WEBSERVER_handlepresets(void)
{
#ifdef TIMEWSHANDLEPRESETS
  Serial.print("ws_handlepresets: ");
  Serial.println(millis());
#endif
  // if the root page was a HALT request via Submit button
  String halt_str = webserver->arg("ha");
  if ( halt_str != "" )
  {
    TRACE();
    DebugPrintln(halt_str);
    halt_alert = true;
    //ftargetPosition = fcurrentPosition;
  }

  // if set focuser preset 0
  String fp_str = webserver->arg("setp0");
  if ( fp_str != "" )
  {
    DebugPrint("setp0:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p0");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(0, (unsigned long)temp);
    }
  }

  // if goto focuser preset 0
  fp_str = webserver->arg("gop0");
  if ( fp_str != "" )
  {
    DebugPrint("gop0:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p0");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(0, (unsigned long) temp);
      ftargetPosition = temp;
    }
  }

  // if set focuser preset 1
  fp_str = webserver->arg("setp1");

  if ( fp_str != "" )
  {
    DebugPrint("setp1:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p1");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(1, (unsigned long) temp);
    }
  }

  // if goto focuser preset 1
  fp_str = webserver->arg("gop1");
  if ( fp_str != "" )
  {
    DebugPrint("gop1:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p1");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(1, (unsigned long) temp);
      ftargetPosition = temp;
    }
  }

  // if set focuser preset 2
  fp_str = webserver->arg("setp2");
  if ( fp_str != "" )
  {
    DebugPrint("setp2:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p2");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(2, (unsigned long) temp);
    }
  }

  // if goto focuser preset 2
  fp_str = webserver->arg("gop2");
  if ( fp_str != "" )
  {
    DebugPrint("gop2:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p2");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(2, (unsigned long) temp);
      ftargetPosition = temp;
    }
  }

  // if set focuser preset 3
  fp_str = webserver->arg("setp3");
  if ( fp_str != "" )
  {
    DebugPrint("setp3:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p3");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(3, (unsigned long) temp);
    }
  }

  // if goto focuser preset 3
  fp_str = webserver->arg("gop3");
  if ( fp_str != "" )
  {
    DebugPrint("gop3:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p3");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(3, (unsigned long) temp);
      ftargetPosition = temp;
    }
  }

  // if set focuser preset 4
  fp_str = webserver->arg("setp4");
  if ( fp_str != "" )
  {
    DebugPrint("setp4:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p4");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(4, (unsigned long) temp);
    }
  }

  // if goto focuser preset 4
  fp_str = webserver->arg("gop4");
  if ( fp_str != "" )
  {
    DebugPrint("gop4:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p4");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(4, (unsigned long) temp);
      ftargetPosition = temp;
    }
  }

  // if set focuser preset 5
  fp_str = webserver->arg("setp5");
  if ( fp_str != "" )
  {
    DebugPrint("setp5:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p5");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(5, (unsigned long) temp);
    }
  }

  // if goto focuser preset 5
  fp_str = webserver->arg("gop5");
  if ( fp_str != "" )
  {
    DebugPrint("gop5:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p5");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(5, (unsigned long) temp);
      ftargetPosition = temp;
    }
  }

  // if set focuser preset 6
  fp_str = webserver->arg("setp6");
  if ( fp_str != "" )
  {
    DebugPrint("setp6:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p6");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(6, (unsigned long) temp);
    }
  }

  // if goto focuser preset 6
  fp_str = webserver->arg("gop6");
  if ( fp_str != "" )
  {
    DebugPrint("gop6:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p6");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(6, (unsigned long) temp);
      ftargetPosition = temp;
    }
  }

  // if set focuser preset 7
  fp_str = webserver->arg("setp7");
  if ( fp_str != "" )
  {
    DebugPrint("setp0:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p7");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(7, (unsigned long) temp);
    }
  }

  // if goto focuser preset 7
  fp_str = webserver->arg("gop7");
  if ( fp_str != "" )
  {
    DebugPrint("gop0:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p7");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(7, (unsigned long) temp);
      ftargetPosition = temp;
    }
  }

  // if set focuser preset 8
  fp_str = webserver->arg("setp8");
  if ( fp_str != "" )
  {
    DebugPrint("setp8:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p8");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(8, (unsigned long) temp);
    }
  }

  // if goto focuser preset 8
  fp_str = webserver->arg("gop8");
  if ( fp_str != "" )
  {
    DebugPrint("gop8:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p8");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(8, (unsigned long) temp);
      ftargetPosition = temp;
    }
  }

  // if set focuser preset 9
  fp_str = webserver->arg("setp9");
  if ( fp_str != "" )
  {
    DebugPrint("setp9:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p9");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(9, (unsigned long) temp);
    }
  }

  // if goto focuser preset 9
  fp_str = webserver->arg("gop9");
  if ( fp_str != "" )
  {
    DebugPrint("gop9:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("p9");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = ( temp < 0 ) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      mySetupData->set_focuserpreset(9, (unsigned long) temp);
      ftargetPosition = temp;
    }
  }

  WEBSERVER_sendpresets();
#ifdef TIMEWSHANDLEPRESETS
  Serial.print("ws_handlepresets: ");
  Serial.println(millis());
#endif
  delay(10);                                            // small pause so background ESP8266 tasks can run
}

void WEBSERVER_sendpresets(void)
{
#ifdef TIMEWSSENDPRESETS
  Serial.print("ws:sendpresets: ");
  Serial.println(millis());
#endif
  WEBSERVER_buildpresets();
  // send the presetspage to a connected client
  DebugPrintln(SENDPAGESTR);
  WEBSERVER_sendmyheader();
  WEBSERVER_sendmycontent();
  WSpg = "";
#ifdef TIMEWSSENDPRESETS
  Serial.print("ws:sendpresets: ");
  Serial.println(millis());
#endif
  delay(10);                                            // small pause so background tasks can run
}

void WEBSERVER_buildmove(void)
{
  // construct the movepage now
#ifdef TIMEWSBUILDMOVE
  Serial.print("ws_buildmove: ");
  Serial.println(millis());
#endif
  // spiffs was started earlier when server was started so assume it has started
  if ( SPIFFS.exists("/wsmove.html"))                   // load page from fs - wsmove.html
  {
    File file = SPIFFS.open("/wsmove.html", "r");       // open file for read
    DebugPrintln(F(READPAGESTR));
    WSpg = file.readString();                           // read contents into string
    file.close();

    DebugPrintln(F(PROCESSPAGESTARTSTR));
    // process for dynamic data
    String bcol = mySetupData->get_wp_backcolor();
    WSpg.replace("%BKC%", bcol);
    String txtcol = mySetupData->get_wp_textcolor();
    WSpg.replace("%TXC%", txtcol);
    String ticol = mySetupData->get_wp_titlecolor();
    WSpg.replace("%TIC%", ticol);
    String hcol = mySetupData->get_wp_headercolor();
    WSpg.replace("%HEC%", hcol);
    WSpg.replace("%RAT%", String(mySetupData->get_webpagerefreshrate()));
    WSpg.replace("%IP%", ipStr);
    WSpg.replace("%POR%", String(mySetupData->get_webserverport()));
    WSpg.replace("%VER%", String(programVersion));
    WSpg.replace("%NAM%", String(DRVBRD_ID));
    WSpg.replace("%CPO%", String(fcurrentPosition));
    WSpg.replace("%TPO%", String(ftargetPosition));
    WSpg.replace("%MOV%", String(isMoving));
    DebugPrintln(F(PROCESSPAGEENDSTR));
  }
  else
  {
    // could not read move file from SPIFFS
    TRACE();
    DebugPrintln(F(FSFILENOTFOUNDSTR));
    DebugPrintln(F(BUILDDEFAULTPAGESTR));
    WSpg = WEBSERVERNOTFOUNDSTR;
  }
#ifdef TIMEWSBUILDMOVE
  Serial.print("ws_buildmove: ");
  Serial.println(millis());
#endif
  delay(10);                                            // small pause so background tasks can run
}

void WEBSERVER_sendmove(void)
{
#ifdef TIMEWSSENDMOVE
  Serial.print("ws:sendmove: ");
  Serial.println(millis());
#endif
  WEBSERVER_buildmove();
  DebugPrintln(SENDPAGESTR);
  WEBSERVER_sendmyheader();
  WEBSERVER_sendmycontent();
  WSpg = "";
#ifdef TIMEWSSENDMOVE
  Serial.print("ws:sendmove: ");
  Serial.println(millis());
#endif
  delay(10);                                            // small pause so background ESP8266 tasks can run
}

// handles move page of webserver
// this is called whenever a client requests move
void WEBSERVER_handlemove()
{
#ifdef TIMEWSMOVEHANDLE
  Serial.print("ws_handlemove: ");
  Serial.println(millis());
#endif
  // if the root page was a HALT request via Submit button
  String halt_str = webserver->arg("ha");
  if ( halt_str != "" )
  {
    TRACE();
    DebugPrintln(halt_str);
    halt_alert = true;
    //ftargetPosition = fcurrentPosition;
  }

  // if move
  String fmv_str = webserver->arg("mv");
  if ( fmv_str != "" )
  {
    long temp = 0;
    TRACE();
    DebugPrintln(fmv_str);
    temp = fmv_str.toInt();
    long newtemp = (long) fcurrentPosition + temp;
    newtemp = ( newtemp < 0 ) ? 0 : newtemp;
    newtemp = ( newtemp > (long)mySetupData->get_maxstep()) ? mySetupData->get_maxstep() : newtemp;
    ftargetPosition = (unsigned long) newtemp;
    DebugPrint("Move = "); DebugPrintln(fmv_str);
    DebugPrint(F(CURRENTPOSSTR));
    DebugPrintln(fcurrentPosition);
    DebugPrint(F(TARGETPOSSTR));
    DebugPrintln(ftargetPosition);
  }

  WEBSERVER_sendmove();
#ifdef TIMEWSMOVEHANDLE
  Serial.print("ws_handlemove: ");
  Serial.println(millis());
#endif
  delay(10);                                            // small pause so background ESP8266 tasks can run
}

void WEBSERVER_buildhome(void)
{
  // construct the homepage now
#ifdef TIMEWSROOTBUILD
  Serial.print("ws_buildroot: ");
  Serial.println(millis());
#endif
  // spiffs was started earlier when server was started so assume it has started
  if ( SPIFFS.exists("/wsindex.html"))                  // load page from fs - wsindex.html
  {
    File file = SPIFFS.open("/wsindex.html", "r");      // open file for read
    DebugPrintln(F(READPAGESTR));
    WSpg = file.readString();                           // read contents into string
    file.close();

    DebugPrintln(F(PROCESSPAGESTARTSTR));
    // process for dynamic data
    String bcol = mySetupData->get_wp_backcolor();
    WSpg.replace("%BKC%", bcol);
    String txtcol = mySetupData->get_wp_textcolor();
    WSpg.replace("%TXC%", txtcol);
    String ticol = mySetupData->get_wp_titlecolor();
    WSpg.replace("%TIC%", ticol);
    String hcol = mySetupData->get_wp_headercolor();
    WSpg.replace("%HEC%", hcol);

    WSpg.replace("%RAT%", String(mySetupData->get_webpagerefreshrate()));
    WSpg.replace("%IP%", ipStr);
    WSpg.replace("%POR%", String(mySetupData->get_webserverport()));
    WSpg.replace("%VER%", String(programVersion));
    WSpg.replace("%NAM%", String(DRVBRD_ID));
    // if this is a GOTO command then make this target else make current
    String fp_str = webserver->arg("gotopos");
    if ( fp_str != "" )
    {
      WSpg.replace("%CPO%", String(ftargetPosition));
    }
    else
    {
      WSpg.replace("%CPO%", String(fcurrentPosition));
    }
    WSpg.replace("%TPO%", String(ftargetPosition));
    WSpg.replace("%MAX%", String(mySetupData->get_maxstep()));
    WSpg.replace("%MOV%", String(isMoving));
    if ( mySetupData->get_temperatureprobestate() == 1 )
    {
      if ( mySetupData->get_tempmode() == 1)
      {
        String tpstr = String(read_temp(1), 2) + " c";
        WSpg.replace("%TEM%", tpstr);
      }
      else
      {
        float ft = read_temp(1);
        ft = (ft * 1.8) + 32;
        String tpstr = String(ft, 2) + " f";
        WSpg.replace("%TEM%", tpstr);
      }
    }
    else
    {
      if ( mySetupData->get_tempmode() == 1)
      {
        WSpg.replace("%TEM%", "20.00 c");
      }
      else
      {
        WSpg.replace("%TEM%", "68.00 f");
      }
    }
    WSpg.replace("%TPR%", String(mySetupData->get_tempprecision()));
    String smbuffer = String(mySetupData->get_stepmode());
    switch ( mySetupData->get_stepmode() )
    {
      case 1:
        smbuffer = WS_SM1CHECKED;
        smbuffer = smbuffer + WS_SM2UNCHECKED;
        smbuffer = smbuffer + WS_SM4UNCHECKED;
        smbuffer = smbuffer + WS_SM8UNCHECKED;
        smbuffer = smbuffer + WS_SM16UNCHECKED;
        smbuffer = smbuffer + WS_SM32UNCHECKED;
        break;
      case 2 :
        smbuffer = WS_SM1UNCHECKED;
        smbuffer = smbuffer + WS_SM2CHECKED;
        smbuffer = smbuffer + WS_SM4UNCHECKED;
        smbuffer = smbuffer + WS_SM8UNCHECKED;
        smbuffer = smbuffer + WS_SM16UNCHECKED;
        smbuffer = smbuffer + WS_SM32UNCHECKED;
        break;
      case 4 :
        smbuffer = WS_SM1UNCHECKED;
        smbuffer = smbuffer + WS_SM2UNCHECKED;
        smbuffer = smbuffer + WS_SM4CHECKED;
        smbuffer = smbuffer + WS_SM8UNCHECKED;
        smbuffer = smbuffer + WS_SM16UNCHECKED;
        smbuffer = smbuffer + WS_SM32UNCHECKED;
        break;
      case 8 :
        smbuffer = WS_SM1UNCHECKED;
        smbuffer = smbuffer + WS_SM2UNCHECKED;
        smbuffer = smbuffer + WS_SM4UNCHECKED;
        smbuffer = smbuffer + WS_SM8CHECKED;
        smbuffer = smbuffer + WS_SM16UNCHECKED;
        smbuffer = smbuffer + WS_SM32UNCHECKED;
        break;
      case 16 :
        smbuffer = WS_SM1UNCHECKED;
        smbuffer = smbuffer + WS_SM2UNCHECKED;
        smbuffer = smbuffer + WS_SM4UNCHECKED;
        smbuffer = smbuffer + WS_SM8UNCHECKED;
        smbuffer = smbuffer + WS_SM16CHECKED;
        smbuffer = smbuffer + WS_SM32UNCHECKED;
        break;
      case 32 :
        smbuffer = WS_SM1UNCHECKED;
        smbuffer = smbuffer + WS_SM2UNCHECKED;
        smbuffer = smbuffer + WS_SM4UNCHECKED;
        smbuffer = smbuffer + WS_SM8UNCHECKED;
        smbuffer = smbuffer + WS_SM16UNCHECKED;
        smbuffer = smbuffer + WS_SM32CHECKED;
        break;
      default :
        smbuffer = WS_SM1CHECKED;
        smbuffer = smbuffer + WS_SM2UNCHECKED;
        smbuffer = smbuffer + WS_SM4UNCHECKED;
        smbuffer = smbuffer + WS_SM8UNCHECKED;
        smbuffer = smbuffer + WS_SM16UNCHECKED;
        smbuffer = smbuffer + WS_SM32UNCHECKED;
        break;
    }
    WSpg.replace("%SMB%", smbuffer);
    String msbuffer = String(mySetupData->get_motorSpeed());
    switch ( mySetupData->get_motorSpeed() )
    {
      case 0:
        msbuffer = WS_MSSLOWCHECKED;
        msbuffer = msbuffer + WS_MSMEDUNCHECKED;
        msbuffer = msbuffer + WS_MSFASTUNCHECKED;
        break;
      case 1:
        msbuffer = WS_MSSLOWUNCHECKED;
        msbuffer = msbuffer + WS_MSMEDCHECKED;
        msbuffer = msbuffer + WS_MSFASTUNCHECKED;
        break;
      case 2:
        msbuffer = WS_MSSLOWUNCHECKED;
        msbuffer = msbuffer + WS_MSMEDUNCHECKED;
        msbuffer = msbuffer + WS_MSFASTCHECKED;
        break;
      default:
        msbuffer = WS_MSSLOWUNCHECKED;
        msbuffer = msbuffer + WS_MSMEDUNCHECKED;
        msbuffer = msbuffer + WS_MSFASTCHECKED;
        break;
    }
    WSpg.replace("%MSB%", msbuffer);
    String cpbuffer;
    if ( !mySetupData->get_coilpower() )
    {
      cpbuffer = "<input type=\"checkbox\" name=\"cp\" value=\"cp\" > ";
    }
    else
    {
      cpbuffer = "<input type=\"checkbox\" name=\"cp\" value=\"cp\" Checked> ";
    }
    WSpg.replace("%CPB%", cpbuffer);
    String rdbuffer;
    if ( !mySetupData->get_reversedirection() )
    {
      rdbuffer = "<input type=\"checkbox\" name=\"rd\" value=\"rd\" > ";
    }
    else
    {
      rdbuffer = "<input type=\"checkbox\" name=\"rd\" value=\"rd\" Checked> ";
    }
    WSpg.replace("%RDB%", rdbuffer);
    // display
#if defined(OLEDTEXT) || defined(OLEDGRAPHICS)
    if ( mySetupData->get_displayenabled() == 1 )
    {
      WSpg.replace("%OLE%", String(DISPLAYONSTR));      // checked already
    }
    else
    {
      WSpg.replace("%OLE%", String(DISPLAYOFFSTR));
    }
#else
    WSpg.replace("%OLE%", "<b>OLED:</b> Display not defined");
#endif
    DebugPrintln(F(PROCESSPAGEENDSTR));
  }
  else
  {
    // could not read index file from SPIFFS
    TRACE();
    DebugPrintln(F(FSFILENOTFOUNDSTR));
    DebugPrintln(F(BUILDDEFAULTPAGESTR));
    WSpg = WEBSERVERNOTFOUNDSTR;
  }
#ifdef TIMEWSROOTBUILD
  Serial.print("ws_buildroot: ");
  Serial.println(millis());
#endif
  delay(10);                                            // small pause so background tasks can run
}

void WEBSERVER_handleposition()
{
  webserver->send(NORMALWEBPAGE, PLAINTEXTPAGETYPE, String(fcurrentPosition)); // Send position value only to client ajax request
}

void WEBSERVER_handleismoving()
{
  webserver->send(NORMALWEBPAGE, PLAINTEXTPAGETYPE, String(isMoving));          // Send isMoving value only to client ajax request
}

void WEBSERVER_handletargetposition()
{
  webserver->send(NORMALWEBPAGE, PLAINTEXTPAGETYPE, String(ftargetPosition)); //Send targetPosition value only to client ajax request
}

// handles root page of webserver
// this is called whenever a client requests home page of sebserver
void WEBSERVER_handleroot()
{
#ifdef TIMEWSROOTHANDLE
  Serial.print("ws_handleroot: ");
  Serial.println(millis());
#endif
  // if the root page was a HALT request via Submit button
  String halt_str = webserver->arg("ha");
  if ( halt_str != "" )
  {
    DebugPrint("root() -halt:");
    DebugPrintln(halt_str);
    halt_alert = true;;
    //ftargetPosition = fcurrentPosition;
  }

  // if set focuser position
  String fp_str;
  fp_str = webserver->arg("setpos");
  if ( fp_str != "" )
  {
    DebugPrint("setpos:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("fp");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      ftargetPosition = ( temp > (long)mySetupData->get_maxstep()) ? mySetupData->get_maxstep() : (unsigned long)temp;
      fcurrentPosition = ftargetPosition;
    }
  }

  // if goto focuser position
  fp_str = webserver->arg("gotopos");
  if ( fp_str != "" )
  {
    DebugPrint("gotopos:");
    DebugPrintln(fp_str);
    String fp = webserver->arg("fp");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = (temp < 0) ? 0 : temp;
      ftargetPosition = ( temp > (long)mySetupData->get_maxstep()) ? mySetupData->get_maxstep() : (unsigned long)temp;
    }
  }

  // if update of maxsteps
  String fmax_str = webserver->arg("fm");
  if ( fmax_str != "" )
  {
    long temp = 0;
    DebugPrint("root() -maxsteps:");
    DebugPrintln(fmax_str);
    temp = fmax_str.toInt();
    if ( temp < (long) fcurrentPosition )               // if maxstep is less than focuser position
    {
      temp = (long) fcurrentPosition + 1;
    }
    if ( temp < FOCUSERLOWERLIMIT )                     // do not set it less then 1024
    {
      temp = FOCUSERLOWERLIMIT;
    }
    if ( temp > (long) mySetupData->get_maxstep() )     // if higher than max value
    {
      temp = (long) mySetupData->get_maxstep();
    }
    mySetupData->set_maxstep((unsigned long)temp);
  }

  // if update motorspeed
  String fms_str = webserver->arg("ms");
  if ( fms_str != "" )
  {
    int temp1 = 0;
    DebugPrint("root() -motorspeed:");
    DebugPrintln(fms_str);
    temp1 = fms_str.toInt();
    if ( temp1 < SLOW )
    {
      temp1 = SLOW;
    }
    if ( temp1 > FAST )
    {
      temp1 = FAST;
    }
    mySetupData->set_motorSpeed(temp1);
  }

  // if update coilpower
  String fcp_str = webserver->arg("cp");
  if ( fcp_str != "" )
  {
    DebugPrint("root() -coil power:");
    DebugPrintln(fcp_str);
    if ( fcp_str == "cp" )
    {
      mySetupData->set_coilpower(1);
    }
    else
    {
      mySetupData->set_coilpower(0);
    }
  }

  // if update reversedirection
  String frd_str = webserver->arg("rd");
  if ( frd_str != "" )
  {
    DebugPrint("root() -reverse direction:");
    DebugPrintln(frd_str);
    if ( frd_str == "rd" )
    {
      mySetupData->set_reversedirection(1);
    }
    else
    {
      mySetupData->set_reversedirection(0);
    }
  }

  // if update stepmode
  // (1=Full, 2=Half, 4=1/4, 8=1/8, 16=1/16, 32=1/32, 64=1/64, 128=1/128)
  String fsm_str = webserver->arg("sm");
  if ( fsm_str != "" )
  {
    int temp1 = 0;
    DebugPrint("root() -stepmode:");
    DebugPrintln(fsm_str);
    temp1 = fsm_str.toInt();
    if ( temp1 < STEP1 )
    {
      temp1 = STEP1;
    }
    if ( temp1 > STEP32 )
    {
      temp1 = STEP32;
    }
    mySetupData->set_stepmode(temp1);
  }

  // if update temperature resolution
  String tres_str = webserver->arg("tr");
  if ( tres_str != "" )
  {
    int temp = 0;
    DebugPrint("root() -temperature resolution:");
    DebugPrintln(tres_str);
    temp = tres_str.toInt();
    if ( temp < 9 )
    {
      // error, do not change
      temp = 9;
    }
    if ( temp > 12 )
    {
      temp = 12;
    }
    mySetupData->set_tempprecision(temp);
  }

  // if update display state
  String d_str = webserver->arg("di");
  if ( d_str != "" )
  {
    DebugPrint("root() -set display state: ");
    DebugPrintln(d_str);
    if ( d_str == "don" )
    {
      mySetupData->set_displayenabled(1);
#ifdef OLEDTEXT
      myoled->Display_On();
#endif
    }
    else
    {
      mySetupData->set_displayenabled(0);
#ifdef OLEDTEXT
      myoled->Display_Off();
#endif
    }
  }
  WEBSERVER_sendroot();
#ifdef TIMEWSROOTHANDLE
  Serial.print("ws_handleroot: ");
  Serial.println(millis());
#endif
}

void WEBSERVER_sendroot(void)
{
#ifdef TIMEWSROOTSEND
  Serial.print("ws_sendroot: ");
  Serial.println(millis());
#endif
  WEBSERVER_buildhome();
  DebugPrintln(F(SENDPAGESTR));
  WEBSERVER_sendmyheader();
  WEBSERVER_sendmycontent();
  WSpg = "";
#ifdef TIMEWSROOTSEND
  Serial.print("ws_sendroot: ");
  Serial.println(millis());
#endif
  delay(10);                                            // small pause so background ESP8266 tasks can run
}

void start_webserver(void)
{
  if ( !SPIFFS.begin() )
  {
    TRACE();
    DebugPrintln(F(FSNOTSTARTEDSTR));
    DebugPrintln(F(SERVERSTATESTOPSTR));
    webserverstate = STOPPED;
    return;
  }
  WSpg.reserve(MAXWEBPAGESIZE);
  DebugPrintln(F(STARTWEBSERVERSTR));
  HDebugPrint("Heap before start_webserver = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());
  HDebugPrintf("%u\n", ESP.getFreeHeap());
#if defined(ESP8266)
  webserver = new ESP8266WebServer(mySetupData->get_webserverport());
#else
  webserver = new WebServer(mySetupData->get_webserverport());
#endif // if defined(esp8266) 
  webserver->on("/", HTTP_GET,  WEBSERVER_sendroot);
  webserver->on("/", HTTP_POST, WEBSERVER_handleroot);
  webserver->on("/move",    HTTP_GET,   WEBSERVER_sendmove);
  webserver->on("/move",    HTTP_POST,  WEBSERVER_handlemove);
  webserver->on("/presets", HTTP_GET,   WEBSERVER_sendpresets);
  webserver->on("/presets", HTTP_POST,  WEBSERVER_handlepresets);
  webserver->on("/position",    WEBSERVER_handleposition);
  webserver->on("/ismoving",    WEBSERVER_handleismoving);
  webserver->on("/target",      WEBSERVER_handletargetposition);
  webserver->onNotFound(WEBSERVER_handlenotfound);
  webserver->begin();
  webserverstate = RUNNING;
  DebugPrintln(F(SERVERSTATESTARTSTR));
  HDebugPrint("Heap after  start_webserver = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());
  delay(10);                                            // small pause so background tasks can run
}

void stop_webserver(void)
{
  if ( webserverstate == RUNNING )
  {
    webserver->close();
    delete webserver;                                   // free the webserver pointer and associated memory/code
    webserverstate = STOPPED;
    TRACE();
    DebugPrintln(F(SERVERSTATESTOPSTR));
  }
  else
  {
    DebugPrintln(F(SERVERNOTRUNNINGSTR));
  }
  delay(10);                                            // small pause so background tasks can run
}
// WEBSERVER END -------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// 24: ASCOMSERVER - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#if defined(ESP8266)
#include <ESP8266WebServer.h>
#else
#include <webserver.h>
#endif // if defined(esp8266)

#include "ascomserver.h"
// Implement ASCOM ALPACA DISCOVERY PROTOCOL
#include <WiFiUdp.h>
WiFiUDP ASCOMDISCOVERYUdp;
char packetBuffer[255];                                 // buffer to hold incoming UDP packet

String ASpg;                                            // url:/setup/v1/focuser/0/setup
unsigned int ASCOMClientID;
unsigned int ASCOMClientTransactionID;
unsigned int ASCOMServerTransactionID = 0;
int          ASCOMErrorNumber = 0;
String       ASCOMErrorMessage = "";
long         ASCOMpos = 0L;
byte         ASCOMTempCompState = 0;
byte         ASCOMConnectedState = 0;
WiFiClient   ascomclient;
#if defined(ESP8266)
ESP8266WebServer *ascomserver;
#else
WebServer *ascomserver;
#endif // if defined(esp8266)
void ASCOM_sendmyheader(void)
{
  //ascomserver->sendHeader(F(CACHECONTROLSTR), F(NOCACHENOSTORESTR));
  //ascomserver->sendHeader(F(PRAGMASTR), F(NOCACHESTR));
  //ascomserver->sendHeader(F(EXPIRESSTR), "-1");
  //ascomserver->setContentLength(CONTENT_LENGTH_UNKNOWN);
  //ascomserver->send(NORMALWEBPAGE, F(TEXTPAGETYPE), "");
  ascomserver->client().println("HTTP/1.1 200 OK");
  ascomserver->client().println("Content-type:text/html");
  //ascomserver->client().println("Connection: close");       // only valid on http/1.0
  ascomserver->client().println();
}

void ASCOM_sendmycontent()
{
  ascomserver->client().print(ASpg);
}

// ASCOM ALPCACA REMOTE DISCOVERY
void checkASCOMALPACADiscovery()
{
  // (c) Daniel VanNoord
  // https://github.com/DanielVanNoord/AlpacaDiscoveryTests/blob/master/Alpaca8266/Alpaca8266.ino
  // if there's data available, read a packet
  int packetSize = ASCOMDISCOVERYUdp.parsePacket();
  if (packetSize)
  {
    DebugPrint("ASCOM ALPACA Discovery: Rcd packet size: ");
    DebugPrintln(packetSize);
    DebugPrint("From ");
    IPAddress remoteIp = ASCOMDISCOVERYUdp.remoteIP();
    DebugPrint(remoteIp);
    DebugPrint(", on port ");
    DebugPrintln(ASCOMDISCOVERYUdp.remotePort());

    // read the packet into packetBufffer
    int len = ASCOMDISCOVERYUdp.read(packetBuffer, 255);
    if (len > 0)
    {
      packetBuffer[len] = 0;                          // Ensure that it is null terminated
    }
    DebugPrint("Contents: ");
    DebugPrintln(packetBuffer);

    if (len < 16)                                     // No undersized packets allowed
    {
      DebugPrintln(F("Packet is undersized"));
      return;
    }

    if (strncmp("alpacadiscovery1", packetBuffer, 16) != 0)    // 0-14 "alpacadiscovery", 15 ASCII Version number of 1
    {
      DebugPrintln(F("Packet is not correct format"));
      return;
    }

    String strresponse = "{\"alpacaport\":" + String(mySetupData->get_ascomalpacaport()) + "}";
    uint8_t response[36] = { 0 };
    len = strresponse.length();
    DebugPrintln("Response : " + strresponse);
    // copy to response
    for ( int i = 0; i < len; i++ )
    {
      response[i] = (uint8_t) strresponse[i];
    }

    ASCOMDISCOVERYUdp.beginPacket(ASCOMDISCOVERYUdp.remoteIP(), ASCOMDISCOVERYUdp.remotePort());
#if defined(ESP8266)
    ASCOMDISCOVERYUdp.write(response, len);
#else
    ASCOMDISCOVERYUdp.write(response, len);
#endif
    ASCOMDISCOVERYUdp.endPacket();
  }
}

// Construct pages for /setup

// constructs ascom setup server page /setup/v1/focuser/0/setup
void ASCOM_Create_Setup_Focuser_HomePage()
{
#ifdef TIMEASCOMBUILDSETUP
  Serial.print("ascombuildsetup() : ");
  Serial.println(millis());
#endif
  // Convert IP address to a string;
  // already in ipStr
  // convert current values of focuserposition and focusermaxsteps to string types
  String fpbuffer = String(fcurrentPosition);
  String mxbuffer = String(mySetupData->get_maxstep());
  String smbuffer = String(mySetupData->get_stepmode());
  int    eflag    = 0;

  switch ( mySetupData->get_stepmode() )
  {
    case 1:
      smbuffer = AS_SM1CHECKED;
      smbuffer = smbuffer + AS_SM2UNCHECKED;
      smbuffer = smbuffer + AS_SM4UNCHECKED;
      smbuffer = smbuffer + AS_SM8UNCHECKED;
      smbuffer = smbuffer + AS_SM16UNCHECKED;
      smbuffer = smbuffer + AS_SM32UNCHECKED;
      break;
    case 2 :
      smbuffer = AS_SM1UNCHECKED;
      smbuffer = smbuffer + AS_SM2CHECKED;
      smbuffer = smbuffer + AS_SM4UNCHECKED;
      smbuffer = smbuffer + AS_SM8UNCHECKED;
      smbuffer = smbuffer + AS_SM16UNCHECKED;
      smbuffer = smbuffer + AS_SM32UNCHECKED;
      break;
    case 4 :
      smbuffer = AS_SM1UNCHECKED;
      smbuffer = smbuffer + AS_SM2UNCHECKED;
      smbuffer = smbuffer + AS_SM4CHECKED;
      smbuffer = smbuffer + AS_SM8UNCHECKED;
      smbuffer = smbuffer + AS_SM16UNCHECKED;
      smbuffer = smbuffer + AS_SM32UNCHECKED;
      break;
    case 8 :
      smbuffer = AS_SM1UNCHECKED;
      smbuffer = smbuffer + AS_SM2UNCHECKED;
      smbuffer = smbuffer + AS_SM4UNCHECKED;
      smbuffer = smbuffer + AS_SM8CHECKED;
      smbuffer = smbuffer + AS_SM16UNCHECKED;
      smbuffer = smbuffer + AS_SM32UNCHECKED;
      break;
    case 16 :
      smbuffer = AS_SM1UNCHECKED;
      smbuffer = smbuffer + AS_SM2UNCHECKED;
      smbuffer = smbuffer + AS_SM4UNCHECKED;
      smbuffer = smbuffer + AS_SM8UNCHECKED;
      smbuffer = smbuffer + AS_SM16CHECKED;
      smbuffer = smbuffer + AS_SM32UNCHECKED;
      break;
    case 32 :
      smbuffer = AS_SM1UNCHECKED;
      smbuffer = smbuffer + AS_SM2UNCHECKED;
      smbuffer = smbuffer + AS_SM4UNCHECKED;
      smbuffer = smbuffer + AS_SM8UNCHECKED;
      smbuffer = smbuffer + AS_SM16UNCHECKED;
      smbuffer = smbuffer + AS_SM32CHECKED;
      break;
    default :
      smbuffer = AS_SM1CHECKED;
      smbuffer = smbuffer + AS_SM2UNCHECKED;
      smbuffer = smbuffer + AS_SM4UNCHECKED;
      smbuffer = smbuffer + AS_SM8UNCHECKED;
      smbuffer = smbuffer + AS_SM16UNCHECKED;
      smbuffer = smbuffer + AS_SM32UNCHECKED;
      break;
  }

  String msbuffer = String(mySetupData->get_motorSpeed());
  switch ( mySetupData->get_motorSpeed() )
  {
    case 0:
      msbuffer = AS_MSSLOWCHECKED;
      msbuffer = msbuffer + AS_MSMEDUNCHECKED;
      msbuffer = msbuffer + AS_MSFASTUNCHECKED;
      break;
    case 1:
      msbuffer = AS_MSSLOWUNCHECKED;
      msbuffer = msbuffer + AS_MSMEDCHECKED;
      msbuffer = msbuffer + AS_MSFASTUNCHECKED;
      break;
    case 2:
      msbuffer = AS_MSSLOWUNCHECKED;
      msbuffer = msbuffer + AS_MSMEDUNCHECKED;
      msbuffer = msbuffer + AS_MSFASTCHECKED;
      break;
    default:
      msbuffer = AS_MSSLOWUNCHECKED;
      msbuffer = msbuffer + AS_MSMEDUNCHECKED;
      msbuffer = msbuffer + AS_MSFASTCHECKED;
      break;
  }

  String cpbuffer;
  if ( !mySetupData->get_coilpower() )
  {
    cpbuffer = "<input type=\"checkbox\" name=\"cp\" value=\"cp\" > ";
  }
  else
  {
    cpbuffer = "<input type=\"checkbox\" name=\"cp\" value=\"cp\" Checked> ";
  }

  String rdbuffer;
  if ( !mySetupData->get_reversedirection() )
  {
    rdbuffer = "<input type=\"checkbox\" name=\"rd\" value=\"rd\" > ";
  }
  else
  {
    rdbuffer = "<input type=\"checkbox\" name=\"rd\" value=\"rd\" Checked> ";
  }

  // construct setup page of ascom server
  // header
  if ( SPIFFS.begin())
  {
    if ( SPIFFS.exists("/assetup.html"))
    {
      File file = SPIFFS.open("/assetup.html", "r");    // open file for read
      DebugPrintln(F(READPAGESTR));                     // read contents into string
      ASpg = file.readString();
      file.close();

      DebugPrintln(F(PROCESSPAGESTARTSTR));
      // process for dynamic data
      String bcol = mySetupData->get_wp_backcolor();
      ASpg.replace("%BKC%", bcol);
      String txtcol = mySetupData->get_wp_textcolor();
      ASpg.replace("%TXC%", txtcol);
      String ticol = mySetupData->get_wp_titlecolor();
      ASpg.replace("%TIC%", ticol);
      String hcol = mySetupData->get_wp_headercolor();
      ASpg.replace("%HEC%", hcol);
      ASpg.replace("%PRN%", String(DRVBRD_ID));
      ASpg.replace("%IPS%", ipStr);
      ASpg.replace("%ALP%", String(mySetupData->get_ascomalpacaport()));
      ASpg.replace("%PRV%", String(programVersion));
      ASpg.replace("%FPB%", fpbuffer);
      ASpg.replace("%MXB%", mxbuffer);
      ASpg.replace("%CPB%", cpbuffer);
      ASpg.replace("%RDB%", rdbuffer);
      ASpg.replace("%SMB%", smbuffer);
      ASpg.replace("%MSB%", msbuffer);
      DebugPrintln(F(PROCESSPAGEENDSTR));
      eflag = 0;
    }
    else
    {
      eflag = 1;                                        // assetup.html not found
    }
  }
  else
  {
    // spiffs not started
    eflag = 1;
  }
  if ( eflag == 1 )
  {
    // SPIFFS FILE NOT FOUND
    ASpg = "<head>" + String(AS_PAGETITLE) + "</head><body>";
    ASpg = ASpg + String(AS_TITLE);

    ASpg = ASpg + String(AS_COPYRIGHT);
    ASpg = ASpg + "<p>Driverboard = myFP2ESP." + DRVBRD_ID + "<br>";
    ASpg = ASpg + "<myFP2ESP." + DRVBRD_ID + "</h3>IP Address: " + ipStr + ", Firmware Version=" + String(programVersion) + "</br>";

    // position. set position
    ASpg = ASpg + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><br><b>Focuser Position</b> <input type=\"text\" name=\"fp\" size =\"15\" value=" + fpbuffer + "> ";
    ASpg = ASpg + "<input type=\"submit\" name=\"setpos\" value=\"Set Pos\"> </form></p>";

    // maxstep
    ASpg = ASpg + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>MaxSteps</b> <input type=\"text\" name=\"fm\" size =\"15\" value=" + mxbuffer + "> ";
    ASpg = ASpg + "<input type=\"submit\" value=\"Submit\"></form>";

    // coilpower
    ASpg = ASpg + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>Coil Power </b>" + cpbuffer;
    ASpg = ASpg + "<input type=\"hidden\" name=\"cp\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

    // reverse direction
    ASpg = ASpg + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>Reverse Direction </b>" + rdbuffer;
    ASpg = ASpg + "<input type=\"hidden\" name=\"rd\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

    // stepmode
    ASpg = ASpg + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>Step Mode </b>" + smbuffer + " ";
    ASpg = ASpg + "<input type=\"hidden\" name=\"sm\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

    // motor speed
    ASpg = ASpg + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>Motor Speed: </b>" + msbuffer + " ";
    ASpg = ASpg + "<input type=\"hidden\" name=\"ms\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

    ASpg = ASpg + "</body></html>\r\n";
  }
#ifdef TIMEASCOMBUILDSETUP
  Serial.print("ascombuildsetup() : ");
  Serial.println(millis());
#endif
}

// generic ASCOM send reply
void ASCOM_sendreply( int replycode, String contenttype, String jsonstr)
{
  DebugPrint("ASCOM_sendreply: replycode:");
  DebugPrint(replycode);
  DebugPrint(" , content-type:");
  DebugPrint(contenttype);
  DebugPrint(", \njson:");
  DebugPrintln(jsonstr);
  // ascomserver.send builds the http header, jsonstr will be in the body
  ascomserver->send(replycode, contenttype, jsonstr );
}

void ASCOM_getURLParameters()
{
  String str;
  // get server args, translate server args to lowercase, they can be mixed case
  DebugPrintln("ASCOM_getURLParameters START");
  DebugPrint("Number of args:");
  DebugPrintln(ascomserver->args());
  for (int i = 0; i < ascomserver->args(); i++)
  {
    if ( i >= ASCOMMAXIMUMARGS )
    {
      break;
    }
    str = ascomserver->argName(i);
    str.toLowerCase();
    DebugPrint("Parameter Found: ");
    DebugPrintln(str);
    if ( str.equals("clientid") )
    {
      ASCOMClientID = (unsigned int) ascomserver->arg(i).toInt();
      DebugPrint("clientID:");
      DebugPrintln(ASCOMClientID);
    }
    if ( str.equals("clienttransactionid") )
    {
      ASCOMClientTransactionID = (unsigned int) ascomserver->arg(i).toInt();
      DebugPrint("clienttransactionid:");
      DebugPrintln(ASCOMClientTransactionID);
    }
    if ( str.equals("tempcomp") )
    {
      String strtmp = ascomserver->arg(i);
      strtmp.toLowerCase();
      if ( strtmp.equals("true") )
      {
        ASCOMTempCompState = 1;
      }
      else
      {
        ASCOMTempCompState = 0;
      }
      DebugPrint("ASCOMTempCompState:");
      DebugPrintln(ASCOMTempCompState);
    }
    if ( str.equals("position") )
    {
      String str1 = ascomserver->arg(i);
      DebugPrint("ASCOMpos RAW:");
      DebugPrintln(str1);
      ASCOMpos = ascomserver->arg(i).toInt();           // this returns a long data type
      DebugPrint("ASCOMpos:");
      DebugPrintln(ASCOMpos);
    }
    if ( str.equals("connected") )
    {
      String strtmp = ascomserver->arg(i);
      strtmp.toLowerCase();
      DebugPrint("conneded RAW:");
      DebugPrintln(str);
      if ( strtmp.equals("true") )
      {
        ASCOMConnectedState = 1;
      }
      else
      {
        ASCOMConnectedState = 0;
      }
      DebugPrint("ASCOMConnectedState:");
      DebugPrintln(ASCOMConnectedState);
    }
  }
  DebugPrintln("ASCOM_getURLParameters END");
}

String ASCOM_addclientinfo(String str )
{
  String str1 = str;
  // add clientid
  str1 = str1 +  "\"ClientID\":" + String(ASCOMClientID) + ",";
  // add clienttransactionid
  str1 = str1 + "\"ClientTransactionID\":" + String(ASCOMClientTransactionID) + ",";
  // add ServerTransactionID
  str1 = str1 + "\"ServerTransactionID\":" + String(ASCOMServerTransactionID) + ",";
  // add errornumber
  str1 = str1 + "\"ErrorNumber\":\"" + String(ASCOMErrorNumber) + "\",";
  // add errormessage
  if ( ASCOMErrorMessage == "" )
  {
    str1 = str1 + "\"ErrorMessage\":\"\"}";
  }
  else
  {
    str1 = str1 + "\"ErrorMessage\":\"" + ASCOMErrorMessage + "\"}";
  }
  return str1;
}

// ---------------------------------------------------------------------------
// Setup functions
// ---------------------------------------------------------------------------
void ASCOM_handle_setup()
{
#ifdef TIMEASCOMHANDLESETUP
  Serial.print("ascomhandlesetup() : ");
  Serial.println(millis());
#endif
  // url /setup
  // The web page must describe the overall device, including name, manufacturer and version number.
  // content-type: text/html
  String ASpg;
  // spiffs was started earlier when server was started so assume it has started
  if ( SPIFFS.exists("/ashomepage.html"))               // read ashomepage.html from FS
  {
    File file = SPIFFS.open("/ashomepage.html", "r");   // open file for read
    DebugPrintln(F(READPAGESTR));
    ASpg = file.readString();                           // read contents into string
    file.close();

    DebugPrintln(F(PROCESSPAGESTARTSTR));
    // process for dynamic data
    String bcol = mySetupData->get_wp_backcolor();
    ASpg.replace("%BKC%", bcol);
    String txtcol = mySetupData->get_wp_textcolor();
    ASpg.replace("%TXC%", txtcol);
    String ticol = mySetupData->get_wp_titlecolor();
    ASpg.replace("%TIC%", ticol);
    String hcol = mySetupData->get_wp_headercolor();
    ASpg.replace("%HEC%", hcol);
    ASpg.replace("%IPS%", ipStr);
    ASpg.replace("%ALP%", String(mySetupData->get_ascomalpacaport()));
    ASpg.replace("%PRV%", String(programVersion));
    ASpg.replace("%PRN%", String(DRVBRD_ID));
    DebugPrintln(F(PROCESSPAGEENDSTR));
  }
  else
  {
    DebugPrintln(F("ascomserver: Error file ashomepage.html not found"));
    DebugPrintln("ascomserver: build_default_homepage");
    ASpg = ASCOMSERVERNOTFOUNDSTR;
  }
  ASCOMServerTransactionID++;
  ASCOM_sendmyheader();
  ASCOM_sendmycontent();
  ASpg = "";
#ifdef TIMEASCOMHANDLESETUP
  Serial.print("ascomhandlesetup() : ");
  Serial.println(millis());
#endif
  delay(10);                                            // small pause so background tasks can run
}

void ASCOM_handle_focuser_setup()
{
#ifdef TIMEASCOMHANDLEFOCUSERSETUP
  Serial.print("ascomhandlefocusersetup() : ");
  Serial.println(millis());
#endif
  // url /setup/v1/focuser/0/setup
  // Configuration web page for the specified device
  // content-type: text/html

  // if set focuser position
  String fpos_str = ascomserver->arg("setpos");
  if ( fpos_str != "" )
  {
    DebugPrint("setpos:");
    DebugPrintln(fpos_str);
    String fp = ascomserver->arg("fp");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = ( temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      fcurrentPosition = ftargetPosition = (unsigned long) temp;
    }
  }

  // if update of maxsteps
  String fmax_str = ascomserver->arg("fm");
  if ( fmax_str != "" )
  {
    long temp = 0;
    DebugPrint("root() -maxsteps:");
    DebugPrintln(fmax_str);
    temp = fmax_str.toInt();
    if ( temp < (long) fcurrentPosition )               // if maxstep is less than focuser position
    {
      temp = (long) fcurrentPosition + 1;
    }
    if ( temp < FOCUSERLOWERLIMIT )                     // do not set it less then 1024
    {
      temp = FOCUSERLOWERLIMIT;
    }
    if ( temp > (long) mySetupData->get_maxstep() )     // if higher than max value
    {
      temp = (long) mySetupData->get_maxstep();
    }
    mySetupData->set_maxstep((unsigned long) temp);
  }

  // if update motorspeed
  String fms_str = ascomserver->arg("ms");
  if ( fms_str != "" )
  {
    int temp1 = 0;
    DebugPrint("root() -motorspeed:");
    DebugPrintln(fms_str);
    temp1 = fms_str.toInt();
    if ( temp1 < SLOW )
    {
      temp1 = SLOW;
    }
    if ( temp1 > FAST )
    {
      temp1 = FAST;
    }
    mySetupData->set_motorSpeed(temp1);
  }

  // if update coilpower
  String fcp_str = ascomserver->arg("cp");
  if ( fcp_str != "" )
  {
    DebugPrint("root() -coil power:");
    DebugPrintln(fcp_str);
    if ( fcp_str == "cp" )
    {
      mySetupData->set_coilpower(1);
    }
    else
    {
      mySetupData->set_coilpower(0);
    }
  }

  // if update reversedirection
  String frd_str = ascomserver->arg("rd");
  if ( frd_str != "" )
  {
    DebugPrint("root() -reverse direction:");
    DebugPrintln(frd_str);
    if ( frd_str == "rd" )
    {
      mySetupData->set_reversedirection(1);
    }
    else
    {
      mySetupData->set_reversedirection(0);
    }
  }

  // if update stepmode
  // (1=Full, 2=Half, 4=1/4, 8=1/8, 16=1/16, 32=1/32, 64=1/64, 128=1/128)
  String fsm_str = ascomserver->arg("sm");
  if ( fsm_str != "" )
  {
    int temp1 = 0;
    DebugPrint("root() -stepmode:");
    DebugPrintln(fsm_str);
    temp1 = fsm_str.toInt();
    if ( temp1 < STEP1 )
    {
      temp1 = STEP1;
    }
    if ( temp1 > STEP32 )
    {
      temp1 = STEP32;
    }
    mySetupData->set_stepmode(temp1);
  }

  DebugPrintln( "root() -build homepage");

  ASCOM_Create_Setup_Focuser_HomePage();                // construct the homepage now

  // send the homepage to a connected client
  ASCOMServerTransactionID++;
  DebugPrintln("root() - send homepage");
  ASCOM_sendmyheader();
  ASCOM_sendmycontent();
  ASpg = "";
#ifdef TIMEASCOMHANDLEFOCUSERSETUP
  Serial.print("ascomhandlefocusersetup() : ");
  Serial.println(millis());
#endif
  delay(10);                                            // small pause so background ESP8266 tasks can run
}

// ---------------------------------------------------------------------------
// Management API functions
// ---------------------------------------------------------------------------
void ASCOM_handleapiversions()
{
#ifdef TIMEASCOMHANDLEAPIVER
  Serial.print("ascomhandleapiver() : ");
  Serial.println(millis());
#endif
  // url /management/apiversions
  // Returns an integer array of supported Alpaca API version numbers.
  // { "Value": [1,2,3,4],"ClientTransactionID": 9876,"ServerTransactionID": 54321}
  String jsonretstr = "";
  DebugPrintln("ASCOM_handleapiversions:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":[1]," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
#ifdef TIMEASCOMHANDLEAPIVER
  Serial.print("ascomhandleapiver() : ");
  Serial.println(millis());
#endif
}

void ASCOM_handleapidescription()
{
#ifdef TIMEASCOMHANDLEAPICON
  Serial.print("ascomhandleapicon() : ");
  Serial.println(millis());
#endif
  // url /management/v1/description
  // Returns cross-cutting information that applies to all devices available at this URL:Port.
  // content-type: application/json
  // { "Value": { "ServerName": "Random Alpaca Device", "Manufacturer": "The Briliant Company",
  //   "ManufacturerVersion": "v1.0.0", "Location": "Horsham, UK" },
  //   "ClientTransactionID": 9876, "ServerTransactionID": 54321 }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handleapidescription:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":" + String(ASCOMMANAGEMENTINFO) + "," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
#ifdef TIMEASCOMHANDLEAPICON
  Serial.print("ascomhandleapicon() : ");
  Serial.println(millis());
#endif
}

void ASCOM_handleapiconfigureddevices()
{
  // url /management/v1/configureddevices
  // Returns an array of device description objects, providing unique information for each served device, enabling them to be accessed through the Alpaca Device API.
  // content-type: application/json
  // { "Value": [{"DeviceName": "Super focuser 1","DeviceType": "Focuser","DeviceNumber": 0,"UniqueID": "277C652F-2AA9-4E86-A6A6-9230C42876FA"}],"ClientTransactionID": 9876,"ServerTransactionID": 54321}
  String jsonretstr = "";
  DebugPrintln("ASCOM_handleapiconfigureddevices:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":[{\"DeviceName\":" + String(ASCOMNAME) + ",\"DeviceType\":\"focuser\",\"DeviceNumber\":0,\"UniqueID\":\"" + String(ASCOMGUID) + "\"}]," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

// ---------------------------------------------------------------------------
// ASCOM ALPACA API
// ---------------------------------------------------------------------------
void ASCOM_handleinterfaceversionget()
{
  // curl -X GET "/api/v1/focuser/0/interfaceversion?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {"Value": 0,  "ErrorNumber": 0,  "ErrorMessage": "string"}
  String jsonretstr = "";
  DebugPrintln("ASCOM_handleinterfaceversionget:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":3," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handleconnectedput()
{
  // PUT "/api/v1/focuser/0/connected" -H  "accept: application/json" -H  "Content-Type: application/x-www-form-urlencoded" -d "Connected=true&ClientID=1&ClientTransactionID=2"
  // response { "ErrorNumber": 0, "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handleconnectedput:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"ErrorNumber\":0,";
  jsonretstr = jsonretstr + "\"ErrorMessage\":\"\"}";
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handleconnectedget()
{
  // GET "/api/v1/focuser/0/connected?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": true, "ErrorNumber": 0, "ErrorMessage": "string"}

  // Should we just return the value of ASCOMConnectedState?
  String jsonretstr = "";
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":1," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handlenameget()
{
  // curl -X GET "/api/v1/focuser/0/name?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": "string",  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handlenameget:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":" + String(ASCOMNAME) + "," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handledescriptionget()
{
  // GET "/api/v1/focuser/0/description?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": "string",  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handledescriptionget:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":" + String(ASCOMDESCRIPTION) + "," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handledriverinfoget()
{
  // curl -X GET "/api/v1/focuser/0/driverinfo?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": "string",  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handledescriptionget:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":" + String(ASCOMDRIVERINFO) + "," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handledriverversionget()
{
  // curl -X GET "/api/v1/focuser/0/driverversion?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": "string",  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handledriverversionget");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":\"" + String(programVersion) + "\"," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handleabsoluteget()
{
  // curl -X GET "/api/v1/focuser/0/absolute?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": true,  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handleabsoluteget");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  // should this be 1? - yes
  jsonretstr = "{\"Value\":1," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handlemaxstepget()
{
  // curl -X GET "/api/v1/focuser/0/maxstep?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": 0,  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handlemaxstepget");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":" + String(mySetupData->get_maxstep()) + "," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handlemaxincrementget()
{
  // curl -X GET "/api/v1/focuser/0/maxincrement?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": 0,  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handlemaxincrementget");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":" + String(mySetupData->get_maxstep()) + "," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handletemperatureget()
{
  // curl -X GET "/api/v1/focuser/0/temperature?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": 1.100000023841858,  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handletemperatureget");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  if ( mySetupData->get_temperatureprobestate() == 1 )
  {
    jsonretstr = "{\"Value\":" + String(read_temp(0)) + "," + ASCOM_addclientinfo( jsonretstr );
  }
  else
  {
    jsonretstr = "{\"Value\":20.0," + ASCOM_addclientinfo( jsonretstr );
  }
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void  ASCOM_handlepositionget()
{
  // curl -X GET "/api/v1/focuser/0/position?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": 0,  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handlepositionget");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":" + String(fcurrentPosition) + "," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void  ASCOM_handlehaltput()
{
  // curl -X PUT "/api/v1/focuser/0/halt" -H  "accept: application/json" -H  "Content-Type: application/x-www-form-urlencoded" -d "ClientID=22&ClientTransactionID=33"
  // { "ErrorNumber": 0, "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handlehaltput");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  halt_alert = true;;
  //ftargetPosition = fcurrentPosition;
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{" + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handleismovingget()
{
  // curl -X GET "/api/v1/focuser/0/ismoving?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": true,  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handleismovingget:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  if ( isMoving == 1 )
  {
    jsonretstr = "{\"Value\":1,"  + ASCOM_addclientinfo( jsonretstr );
  }
  else
  {
    jsonretstr = "{\"Value\":0," + ASCOM_addclientinfo( jsonretstr );
  }
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handlestepsizeget()
{
  // curl -X GET "/api/v1/focuser/0/stepsize?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": 1.100000023841858,  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handlestepsizeget:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":" + String(mySetupData->get_stepsize()) + "," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handletempcompget()
{
  // curl -X GET "/api/v1/focuser/0/tempcomp?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": true,  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handletempcompget:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // The state of temperature compensation mode (if available), else always False.
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  if ( mySetupData->get_tempcompenabled() == 0 )
  {
    jsonretstr = "{\"Value\":0," + ASCOM_addclientinfo( jsonretstr );
  }
  else
  {
    jsonretstr = "{\"Value\":1," + ASCOM_addclientinfo( jsonretstr );
  }
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handletempcompput()
{
  // curl -X PUT "/api/v1/focuser/0/tempcomp" -H  "accept: application/json" -H  "Content-Type: application/x-www-form-urlencoded" -d "TempComp=true&Client=1&ClientTransactionIDForm=12"
  // {  "ErrorNumber": 0,  "ErrorMessage": "string" }
  // look for parameter tempcomp=true or tempcomp=false
  String jsonretstr = "";
  DebugPrintln("ASCOM_handletempcompput:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  if ( mySetupData->get_temperatureprobestate() == 1)
  {
    if ( ASCOMTempCompState == 1 )
    {
      // turn on temperature compensation
      mySetupData->set_tempcompenabled(1);
    }
    else
    {
      // turn off temperature compensation
      mySetupData->set_tempcompenabled(0);
    }
    jsonretstr = "{" + ASCOM_addclientinfo( jsonretstr );
    // sendreply builds http header, sets content type, and then sends jsonretstr
    ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
  }
  else
  {
    ASCOMErrorNumber = ASCOMNOTIMPLEMENTED;
    ASCOMErrorMessage = ASCOMERRORNOTIMPLEMENTED;
    jsonretstr = "{" + ASCOM_addclientinfo( jsonretstr );
    // sendreply builds http header, sets content type, and then sends jsonretstr
    ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
  }
}

void ASCOM_handletempcompavailableget()
{
  // curl -X GET "/api/v1/focuser/0/tempcompavailable?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": true,  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handletempcompavailableget:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  if ( mySetupData->get_temperatureprobestate() == 1 )
  {
    jsonretstr = "{\"Value\":1," + ASCOM_addclientinfo( jsonretstr );
  }
  else
  {
    jsonretstr = "{\"Value\":0," + ASCOM_addclientinfo( jsonretstr );
  }
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handlemoveput()
{
  // curl -X PUT "/api/v1/focuser/0/move" -H  "accept: application/json" -H  "Content-Type: application/x-www-form-urlencoded" -d "Position=1000&ClientID=22&ClientTransactionID=33"
  // {  "ErrorNumber": 0,  "ErrorMessage": "string" }
  // extract new value
  String jsonretstr = "";
  DebugPrintln("ASCOM_handlemoveput:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();         // get clientID and clienttransactionID

  // destination is in ASCOMpos
  // this is interfaceversion = 3, so moves are allowed when temperature compensation is on
  unsigned long newpos;
  DebugPrint("ASCOMpos: ");
  DebugPrintln(ASCOMpos);
  if ( ASCOMpos <= 0 )
  {
    newpos = 0;
    DebugPrint("new position: ");
    DebugPrintln(newpos);
    ftargetPosition = newpos;
    jsonretstr = "{" + ASCOM_addclientinfo( jsonretstr );
    ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
  }
  else
  {
    newpos = (unsigned long) ASCOMpos;
    if (newpos > mySetupData->get_maxstep() )
    {
      newpos = mySetupData->get_maxstep();
      ftargetPosition = newpos;
      DebugPrint("new position: ");
      DebugPrintln(newpos);
      jsonretstr = "{" + ASCOM_addclientinfo( jsonretstr );
      ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
    }
    else
    {
      ftargetPosition = newpos;
      DebugPrint("new position: ");
      DebugPrintln(newpos);
      jsonretstr = "{" + ASCOM_addclientinfo( jsonretstr );
      ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
    }
  }
}

void ASCOM_handlesupportedactionsget()
{
  // curl -X GET "/api/v1/focuser/0/supportedactions?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": [    "string"  ],  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handlesupportedactionsget:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  // get clientID and clienttransactionID
  ASCOM_getURLParameters();
  jsonretstr = "{\"Value\": [\"isMoving\",\"MaxStep\",\"Temperature\",\"Position\",\"Absolute\",\"MaxIncrement\",\"StepSize\",\"TempComp\",\"TempCompAvailable\", ]," + ASCOM_addclientinfo( jsonretstr );
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handleNotFound()
{
  String message = "Not Found: ";
  String jsonretstr = "";
  DebugPrintln("ASCOM_handleNotFound:");
  message += "URI: ";
  message += ascomserver->uri();
  message += "\nMethod: ";
  if ( ascomserver->method() == HTTP_GET )
  {
    message += "GET";
  }
  else if ( ascomserver->method() == HTTP_POST )
  {
    message += "POST";
  }
  else if ( ascomserver->method() == HTTP_PUT )
  {
    message += "PUT";
  }
  else if ( ascomserver->method() == HTTP_DELETE )
  {
    message += "DELETE";
  }
  else
  {
    message += "UNKNOWN_METHOD: " + ascomserver->method();
  }
  message += "\nArguments: ";
  message += ascomserver->args();
  message += "\n";
  for (uint8_t i = 0; i < ascomserver->args(); i++)
  {
    message += " " + ascomserver->argName(i) + ": " + ascomserver->arg(i) + "\n";
  }
  DebugPrint("Error: ");
  DebugPrintln(message);
  ASCOMErrorNumber  = ASCOMNOTIMPLEMENTED;
  ASCOMErrorMessage = ASCOMERRORNOTIMPLEMENTED;
  ASCOMServerTransactionID++;
  jsonretstr = "{" + ASCOM_addclientinfo( jsonretstr );
  ASCOM_sendreply( BADREQUESTWEBPAGE, JSONPAGETYPE, jsonretstr);
  delay(10);                                            // small pause so background tasks can run
}

void ASCOM_handleRoot()
{
  String ASpg;
  // spiffs was started earlier when server was started so assume it has started
  if ( SPIFFS.exists("/ashomepage.html"))               // read ashomepage.html from FS
  {
    DebugPrintln("ascomserver: ashomepage.html found");
    File file = SPIFFS.open("/ashomepage.html", "r");   // open file for read
    DebugPrintln("ascomserver: read page into string");
    ASpg = file.readString();                           // read contents into string
    file.close();

    DebugPrintln("ascomserver: processing page start");
    // process for dynamic data
    String bcol = mySetupData->get_wp_backcolor();
    MSpg.replace("%BKC%", bcol);
    String txtcol = mySetupData->get_wp_textcolor();
    MSpg.replace("%TXC%", txtcol);
    String ticol = mySetupData->get_wp_titlecolor();
    MSpg.replace("%TIC%", ticol);
    String hcol = mySetupData->get_wp_headercolor();
    MSpg.replace("%HEC%", hcol);
    ASpg.replace("%IPS%", ipStr);
    ASpg.replace("%ALP%", String(mySetupData->get_ascomalpacaport()));
    ASpg.replace("%PRV%", String(programVersion));
    ASpg.replace("%PRN%", String(DRVBRD_ID));
    DebugPrintln("ascomserver: processing page done");
  }
  else
  {
    DebugPrintln(F("ascomserver: Error occurred finding SPIFFS file ashomepage.html"));
    DebugPrintln("ascomserver: build_default_homepage");
    ASpg = ASCOMSERVERNOTFOUNDSTR;
  }
  ASCOMServerTransactionID++;
  ASCOM_sendreply( NORMALWEBPAGE, TEXTPAGETYPE, ASpg);
  delay(10);                                            // small pause so background tasks can run
}

void start_ascomremoteserver(void)
{
  if ( !SPIFFS.begin() )
  {
    TRACE();
    DebugPrintln(F(FSNOTSTARTEDSTR));
    DebugPrintln(F(SERVERSTATESTOPSTR));
    ascomserverstate = STOPPED;
    return;
  }
  ASpg.reserve(MAXASCOMPAGESIZE);
  HDebugPrint("Heap before start_ascomremoteserver = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());
  DebugPrintln("start ascom server");

#if defined(ESP8266)
  ascomserver = new ESP8266WebServer(mySetupData->get_ascomalpacaport());
#else
  ascomserver = new WebServer(mySetupData->get_ascomalpacaport());
#endif // if defined(esp8266) 

  if ( ascomdiscoverystate == STOPPED )
  {
    ASCOMDISCOVERYUdp.begin(ASCOMDISCOVERYPORT);
    ascomdiscoverystate = RUNNING;
  }
  ascomserver->on("/", ASCOM_handleRoot);               // handle root access
  ascomserver->onNotFound(ASCOM_handleNotFound);        // handle url not found 404

  ascomserver->on("/management/apiversions",              ASCOM_handleapiversions);
  ascomserver->on("/management/v1/description",           ASCOM_handleapidescription);
  ascomserver->on("/management/v1/configureddevices",     ASCOM_handleapiconfigureddevices);

  ascomserver->on("/setup",                               ASCOM_handle_setup);
  ascomserver->on("/setup/v1/focuser/0/setup",            ASCOM_handle_focuser_setup);

  ascomserver->on("/api/v1/focuser/0/connected",          HTTP_PUT, ASCOM_handleconnectedput);
  ascomserver->on("/api/v1/focuser/0/interfaceversion",   HTTP_GET, ASCOM_handleinterfaceversionget);
  ascomserver->on("/api/v1/focuser/0/name",               HTTP_GET, ASCOM_handlenameget);
  ascomserver->on("/api/v1/focuser/0/description",        HTTP_GET, ASCOM_handledescriptionget);
  ascomserver->on("/api/v1/focuser/0/driverinfo",         HTTP_GET, ASCOM_handledriverinfoget);
  ascomserver->on("/api/v1/focuser/0/driverversion",      HTTP_GET, ASCOM_handledriverversionget);
  ascomserver->on("/api/v1/focuser/0/absolute",           HTTP_GET, ASCOM_handleabsoluteget);
  ascomserver->on("/api/v1/focuser/0/maxstep",            HTTP_GET, ASCOM_handlemaxstepget);
  ascomserver->on("/api/v1/focuser/0/maxincrement",       HTTP_GET, ASCOM_handlemaxincrementget);
  ascomserver->on("/api/v1/focuser/0/temperature",        HTTP_GET, ASCOM_handletemperatureget);
  ascomserver->on("/api/v1/focuser/0/position",           HTTP_GET, ASCOM_handlepositionget);
  ascomserver->on("/api/v1/focuser/0/halt",               HTTP_PUT, ASCOM_handlehaltput);
  ascomserver->on("/api/v1/focuser/0/ismoving",           HTTP_GET, ASCOM_handleismovingget);
  ascomserver->on("/api/v1/focuser/0/stepsize",           HTTP_GET, ASCOM_handlestepsizeget);
  ascomserver->on("/api/v1/focuser/0/connected",          HTTP_GET, ASCOM_handleconnectedget);
  ascomserver->on("/api/v1/focuser/0/tempcomp",           HTTP_GET, ASCOM_handletempcompget);
  ascomserver->on("/api/v1/focuser/0/tempcomp",           HTTP_PUT, ASCOM_handletempcompput);
  ascomserver->on("/api/v1/focuser/0/tempcompavailable",  HTTP_GET, ASCOM_handletempcompavailableget);
  ascomserver->on("/api/v1/focuser/0/move",               HTTP_PUT, ASCOM_handlemoveput);
  ascomserver->on("/api/v1/focuser/0/supportedactions",   HTTP_GET, ASCOM_handlesupportedactionsget);
  ascomserver->begin();
  ascomserverstate = RUNNING;
  DebugPrintln(F("start ascom server: RUNNING"));
  HDebugPrint("Heap after  start_ascomremoteserver = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());
  delay(10);                                            // small pause so background tasks can run
}

void stop_ascomremoteserver(void)
{
  if ( ascomserverstate == RUNNING )
  {
    DebugPrintln("stop ascom server");
    ascomserver->close();
    delete ascomserver;                                 // free the ascomserver pointer and associated memory/code
    ascomserverstate = STOPPED;
    ASCOMDISCOVERYUdp.stop();                           // stop discovery service
  }
  else
  {
    DebugPrintln(F(SERVERNOTRUNNINGSTR));
  }

  if ( ascomdiscoverystate == STOPPED )
  {
    ASCOMDISCOVERYUdp.stop();
    ascomdiscoverystate = STOPPED;
  }
  else
  {
    DebugPrintln(F(SERVERNOTRUNNINGSTR));
  }
  delay(10);                                            // small pause so background tasks can run
}
// ASCOM REMOTE END ----------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// 25: OTAUPDATES - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#if defined(OTAUPDATES)
#include <ArduinoOTA.h>

void start_otaservice()
{
  DebugPrintln(STARTOTASERVICESTR);
  oledtextmsg(STARTOTASERVICESTR, -1, false, true);
  ArduinoOTA.setHostname(OTAName);                      // Start the OTA service
  ArduinoOTA.setPassword(OTAPassword);

  ArduinoOTA.onStart([]()
  {
    DebugPrintln(STARTSTR);
  });
  ArduinoOTA.onEnd([]()
  {
    DebugPrintln(ENDSTR);
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
    DebugPrint(PROGRESSSTR);
    DebugPrintln((progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error)
  {
    DebugPrint(ERRORSTR);
    DebugPrintln(error);
    if (error == OTA_AUTH_ERROR)
    {
      DebugPrintln(F("Auth Failed"));
    }
    else if (error == OTA_BEGIN_ERROR)
    {
      DebugPrintln(F("Begin Failed"));
    }
    else if (error == OTA_CONNECT_ERROR)
    {
      DebugPrintln(F("Connect Failed"));
    }
    else if (error == OTA_RECEIVE_ERROR)
    {
      DebugPrintln(F("Receive Failed"));
    }
    else if (error == OTA_END_ERROR)
    {
      DebugPrintln(F("End Failed"));
    }
  });
  ArduinoOTA.begin();
  DebugPrintln(READYSTR);
  otaupdatestate = RUNNING;
}
#endif // #if defined(OTAUPDATES)

// ----------------------------------------------------------------------------------------------
// 26: DUCKDNS - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#ifdef USEDUCKDNS
#include <EasyDDNS.h>                           // https://github.com/ayushsharma82/EasyDDNS

void init_duckdns(void)
{
  DebugPrintln(SETUPDUCKDNSSTR);
  oledtextmsg(SETUPDUCKDNSSTR, -1, false, true);
  EasyDDNS.service("duckdns");                  // Enter your DDNS Service Name - "duckdns" / "noip"
  delay(5);
  EasyDDNS.client(duckdnsdomain, duckdnstoken); // Enter ddns Domain & Token | Example - "esp.duckdns.org","1234567"
  delay(5);
  EasyDDNS.update(DUCKDNS_REFRESHRATE);         // Check for New Ip Every 60 Seconds.
  delay(5);
  duckdnsstate = RUNNING;
}
#endif // #ifdef USEDUCKSDNS

// ----------------------------------------------------------------------------------------------
// 27: FIRMWARE - CHANGE AT YOUR OWN PERIL
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

void software_Reboot(int Reboot_delay)
{
  oledtextmsg(WIFIRESTARTSTR, -1, true, false);
#ifdef MDNSSERVER
  stop_mdns_service();
#endif
  mySetupData->SaveNow();                               // save the focuser settings immediately
#ifdef WEBSERVER
  stop_webserver();
#endif
#ifdef ASCOMREMOTE
  stop_ascomremoteserver();
#endif
#ifdef MANAGEMENT
  stop_management();
#endif

#if defined(ACCESSPOINT) || defined(STATIONMODE)
  if ( myclient.connected() )
  {
    myclient.stop();
  }
#endif
  delay(Reboot_delay);
  ESP.restart();
}

// move motor without updating position, used by sethomeposition
void steppermotormove(byte dir )                // direction moving_in, moving_out ^ reverse direction
{
  if ( mySetupData->get_inoutledstate() == 1)
  {
#if (DRVBRD == PRO2ESP32ULN2003 || DRVBRD == PRO2ESP32L298N || DRVBRD == PRO2ESP32L293DMINI || DRVBRD == PRO2ESP32L9110S) || (DRVBRD == PRO2ESP32DRV8825 )
    ( dir == moving_in ) ? digitalWrite(INLEDPIN, 1) : digitalWrite(OUTLEDPIN, 1);
#endif
  }
  driverboard->movemotor(dir, false);
  if ( mySetupData->get_inoutledstate() == 1)
  {
#if (DRVBRD == PRO2ESP32ULN2003 || DRVBRD == PRO2ESP32L298N || DRVBRD == PRO2ESP32L293DMINI || DRVBRD == PRO2ESP32L9110S) || (DRVBRD == PRO2ESP32DRV8825 )
    ( dir == moving_in ) ? digitalWrite(INLEDPIN, 0) : digitalWrite(OUTLEDPIN, 0);
#endif
  }
}

void init_leds()
{
  if ( mySetupData->get_inoutledstate() == 1)
  {
#if (DRVBRD == PRO2ESP32ULN2003 || DRVBRD == PRO2ESP32L298N || DRVBRD == PRO2ESP32L293DMINI || DRVBRD == PRO2ESP32L9110S) || (DRVBRD == PRO2ESP32DRV8825 )
    pinMode(INLEDPIN, OUTPUT);
    pinMode(OUTLEDPIN, OUTPUT);
    digitalWrite(INLEDPIN, 1);
    digitalWrite(OUTLEDPIN, 1);
#endif
  }
}

void oledtextmsg(String str, int val, boolean clrscr, boolean nl)
{
#ifdef OLEDTEXT
  if (displayfound == true)
  {
    if ( clrscr == true)                      // clear the screen?
    {
      myoled->clear();
      myoled->setCursor(0, 0);
    }
    if ( nl == true )                         // need to print a new line?
    {
      if ( val != -1)                         // need to print a value?
      {
        myoled->print(str);
        myoled->println(val);
      }
      else
      {
        myoled->println(str);
      }
    }
    else
    {
      myoled->print(str);
      if ( val != -1 )
      {
        myoled->print(val);
      }
    }
  }
#endif // OLEDTEXT
}

#ifdef READWIFICONFIG
bool readwificonfig( char* xSSID, char* xPASSWORD)
{
  const String filename = "/wificonfig.json";
  String SSID;
  String PASSWORD;
  boolean status = false;

  DebugPrintln(F(CHECKWIFICONFIGFILESTR));
  // SPIFFS may have failed to start
  if ( !SPIFFS.begin() )
  {
    TRACE();
    DebugPrintln(F("Failed to read wificonfig.jsn"));
    return status;
  }
  File f = SPIFFS.open(filename, "r");                  // file open to read
  if (!f)
  {
    TRACE();
    DebugPrintln(F(FILENOTFOUNDSTR));
  }
  else
  {
    String data = f.readString();                       // read content of the text file
    DebugPrint(F("Wifi Config data: "));
    DebugPrintln(data);                                 // ... and print on serial
    f.close();

    DynamicJsonDocument doc( (const size_t) (JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(2) + 120));  // allocate json buffer
    DeserializationError error = deserializeJson(doc, data);    // Parse JSON object
    if (error)
    {
      TRACE();
      DebugPrintln(F(DESERIALIZEERRORSTR));
    }
    else
    {
      // Decode JSON/Extract values
      SSID     =  doc["mySSID"].as<char*>();
      PASSWORD =  doc["myPASSWORD"].as<char*>();

      SSID.toCharArray(xSSID, SSID.length() + 1);
      PASSWORD.toCharArray(xPASSWORD, PASSWORD.length() + 1);

      status = true;
    }
  }
  return status;
}
#endif

#if defined(ACCESSPOINT) || defined(STATIONMODE)
void start_tcpipserver()
{
#if defined(ESP8266)
  myserver.begin();                       // esp8266 cannot define a port when starting
#else
  myserver.begin(mySetupData->get_tcpipport());
#endif
  tcpipserverstate = RUNNING;
}

void stop_tcpipserver()
{
  myserver.stop();
  tcpipserverstate = STOPPED;
}
#endif

//_______________________________________________ setup()

void setup()
{
  Serial.begin(SERIALPORTSPEED);
#if defined(DEBUG)
  Serial.begin(SERIALPORTSPEED);
  DebugPrintln(SERIALSTARTSTR);
  DebugPrintln(DEBUGONSTR);
#endif
  delay(100);                                   // otherwise this serial statement does not appear

#ifdef TIMESETUP
  Serial.print("setup(): ");
  Serial.println(millis());
#endif

  HDebugPrint("Heap = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());
  HDebugPrintln("setup(): mySetupData()");
  mySetupData = new SetupData();                // instantiate object SetUpData with SPIFFS file
  HDebugPrint("Heap = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());

#ifdef LOCALSERIAL
  Serial.begin(SERIALPORTSPEED);
  serialline = "";
  clearSerialPort();
#endif // if defined(LOCALSERIAL)

#ifdef BLUETOOTHMODE                            // open Bluetooth port, set bluetooth device name
  SerialBT.begin(BLUETOOTHNAME);                // Bluetooth device name
  btline = "";
  clearbtPort();
  DebugPrintln(BLUETOOTHSTARTSTR);
#endif

  // Setup LEDS, use as controller power up indicator
  if ( mySetupData->get_inoutledstate() == 1)
  {
    init_leds();
  }

#ifdef PUSHBUTTONS                              // Setup Pushbuttons, active high when pressed
  init_pushbuttons();
#endif

  HDebugPrint("Heap = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());
  HDebugPrintln("setup(): oledtextdisplay()");
  displayfound = false;
#ifdef OLEDTEXT
  myoled = new OLED_TEXT;
#elif OLEDGRAPHICS
  myoled = new OLED_GRAPHIC();
#else
  //myoled = new OLED_NON;                       // create Object for non OLED
#endif
  HDebugPrint("Heap = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());

  DebugPrint(F("fposition= "));                 // Print Loaded Values from SPIFF
  DebugPrintln(mySetupData->get_fposition());
  DebugPrint(F("focuserdirection= "));
  DebugPrintln(mySetupData->get_focuserdirection());
  DebugPrint(F("maxstep= "));
  DebugPrintln(mySetupData->get_maxstep());
  DebugPrint(F("stepsize= "));;
  DebugPrintln(mySetupData->get_stepsize());;
  DebugPrint(F("DelayAfterMove= "));
  DebugPrintln(mySetupData->get_DelayAfterMove());
  DebugPrint(F("backlashsteps_in= "));
  DebugPrintln(mySetupData->get_backlashsteps_in());
  DebugPrint(F("backlashsteps_out= "));
  DebugPrintln(mySetupData->get_backlashsteps_out());
  DebugPrint(F("tempcoefficient= "));
  DebugPrintln(mySetupData->get_tempcoefficient());
  DebugPrint(F("tempprecision= "));
  DebugPrintln(mySetupData->get_tempprecision());
  DebugPrint(F("stepmode = "));
  DebugPrintln(mySetupData->get_stepmode());
  DebugPrint(F("coilpower= "));
  DebugPrintln(mySetupData->get_coilpower());
  DebugPrint(F("reversedirection= "));
  DebugPrintln(mySetupData->get_reversedirection());
  DebugPrint(F("stepsizeenabled= "));
  DebugPrintln(mySetupData->get_stepsizeenabled());
  DebugPrint(F("tempmode= "));
  DebugPrintln(mySetupData->get_tempmode());
  DebugPrint(F("lcdupdateonmove= "));
  DebugPrintln(mySetupData->get_lcdupdateonmove());
  DebugPrint(F("lcdpagedisplaytime= "));
  DebugPrintln(mySetupData->get_lcdpagetime());
  DebugPrint(F("tempcompenabled= "));
  DebugPrintln(mySetupData->get_tempcompenabled());
  DebugPrint(F("tcdirection= "));
  DebugPrintln(mySetupData->get_tcdirection());
  DebugPrint(F("motorSpeed= "));
  DebugPrintln(mySetupData->get_motorSpeed());
  DebugPrint(F("displayenabled= "));
  DebugPrintln(mySetupData->get_displayenabled());
  DebugPrint(F("webserverport= "));
  DebugPrintln(mySetupData->get_webserverport());
  DebugPrint(F("ascomalpacaserverport= "));
  DebugPrintln(mySetupData->get_ascomalpacaport());
  DebugPrint(F("webserver page refresh rate= "));
  DebugPrintln(mySetupData->get_webpagerefreshrate());
  DebugPrint(F("mdnsserverport= "));
  DebugPrintln(mySetupData->get_mdnsport());
  DebugPrint(F("tcpipserverport= "));
  DebugPrintln(mySetupData->get_tcpipport());
  DebugPrint(F("showstartscreen= "));
  DebugPrintln(mySetupData->get_showstartscreen());
  DebugPrint(F("Webpg backgnd color="));
  DebugPrintln(mySetupData->get_wp_backcolor());
  DebugPrint(F("Webpg txt color="));
  DebugPrintln(mySetupData->get_wp_textcolor());
  DebugPrint(F("Webpg header color="));
  DebugPrintln(mySetupData->get_wp_headercolor());
  DebugPrint(F("Webpg title color="));
  DebugPrintln(mySetupData->get_wp_titlecolor());
  DebugPrint(F("Ascom remote server state ="));
  DebugPrintln(mySetupData->get_ascomserverstate());
  DebugPrint(F("Webserver state="));
  DebugPrintln(mySetupData->get_webserverstate());
  DebugPrint(F("temperature probe state="));
  DebugPrintln(mySetupData->get_temperatureprobestate());
  DebugPrint(F("In/Out LED's state="));
  DebugPrintln(mySetupData->get_inoutledstate());

  if ( mySetupData->get_temperatureprobestate() == 1)
  {
    init_temp();                                        // start temp probe
  }

#ifdef READWIFICONFIG
#if defined(ACCESSPOINT) || defined(STATIONMODE)
  readwificonfig(mySSID, myPASSWORD);                   // read mySSID,myPASSWORD from FS if exist, otherwise use defaults
#endif
#endif

  HDebugPrint("Heap = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());
  HDebugPrintln("setup(): accesspoint");
#ifdef ACCESSPOINT
  oledtextmsg(STARTAPSTR, -1, true, true);
  DebugPrintln(STARTAPSTR);
  WiFi.config(ip, dns, gateway, subnet);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(mySSID, myPASSWORD);
#endif // end ACCESSPOINT
  HDebugPrint("Heap = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());

  // this is setup as a station connecting to an existing wifi network
#ifdef STATIONMODE
  DebugPrintln(STARTSMSTR);
  oledtextmsg(STARTSMSTR, -1, false, true);
  if (staticip == STATICIPON)                   // if staticip then set this up before starting
  {
    DebugPrintln(SETSTATICIPSTR);
    oledtextmsg(SETSTATICIPSTR, -1, false, true);
    WiFi.config(ip, dns, gateway, subnet);
    delay(5);
  }

  // Log on to LAN
  WiFi.mode(WIFI_STA);
  byte status = WiFi.begin(mySSID, myPASSWORD); // attempt to start the WiFi
  DebugPrint(WIFIBEGINSTATUSSTR);
  DebugPrintln(String(status));
  delay(1000);                                  // wait 500ms

  for (int attempts = 0; WiFi.status() != WL_CONNECTED; attempts++)
  {
    DebugPrint(ATTEMPTCONNSTR);
    DebugPrintln(mySSID);
    DebugPrint(ATTEMPTSSTR);
    DebugPrint(attempts);
    delay(1000);                                // wait 1s

    myoled->oled_draw_Wifi(attempts);
    oledtextmsg(ATTEMPTSSTR, attempts, false, true);
    if (attempts > 9)                          // if this attempt is 10 or more tries
    {
      DebugPrintln(APCONNECTFAILSTR);
      DebugPrintln(WIFIRESTARTSTR);
      oledtextmsg(APCONNECTFAILSTR + String(mySSID), -1, true, true);
      myoled->oledgraphicmsg(APSTARTFAILSTR + String(mySSID), -1, true);
      delay(2000);
      software_Reboot(2000);                    // GPIO0 must be HIGH and GPIO15 LOW when calling ESP.restart();
    }
  }
#endif // end STATIONMODE

  oledtextmsg(CONNECTEDSTR, -1, true, true);
  delay(100);                                   // keep delays small else issue with ASCOM

  tcpipserverstate = STOPPED;
  mdnsserverstate = STOPPED;
  webserverstate = STOPPED;
  ascomserverstate = STOPPED;
  ascomdiscoverystate = STOPPED;
  managementserverstate = STOPPED;
  otaupdatestate = STOPPED;
  duckdnsstate = STOPPED;

  HDebugPrint("Heap = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());
  HDebugPrintln("setup(): tcpip server");
#if defined(ACCESSPOINT) || defined(STATIONMODE)
  // Starting TCP Server
  DebugPrintln(STARTTCPSERVERSTR);
  oledtextmsg(STARTTCPSERVERSTR, -1, false, true);
  start_tcpipserver();
  DebugPrintln(GETLOCALIPSTR);
  ESP32IPAddress = WiFi.localIP();
  delay(100);                                   // keep delays small else issue with ASCOM
  DebugPrintln(TCPSERVERSTARTEDSTR);
  oledtextmsg(TCPSERVERSTARTEDSTR, -1, false, true);
  HDebugPrint("Heap = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());

  // set packet counts to 0
  packetsreceived = 0;
  packetssent = 0;

  // connection established
  DebugPrint(SSIDSTR);
  DebugPrintln(mySSID);
  DebugPrint(IPADDRESSSTR);
  DebugPrintln(WiFi.localIP());
  DebugPrint(PORTSTR);
  DebugPrintln(SERVERPORT);
  DebugPrintln(SERVERREADYSTR);
  myIP = WiFi.localIP();
  snprintf(ipStr, sizeof(ipStr), "%i.%i.%i.%i",  myIP[0], myIP[1], myIP[2], myIP[3]);
#else
  // it is Bluetooth so set some globals
  ipStr = ip_zero;      // "0.0.0.0"
#endif // if defined(ACCESSPOINT) || defined(STATIONMODE)

  // assign to current working values
  ftargetPosition = fcurrentPosition = mySetupData->get_fposition();

  DebugPrint(SETUPDRVBRDSTR);
  DebugPrintln(DRVBRD);
  oledtextmsg(SETUPDRVBRDSTR, DRVBRD, true, true);

  HDebugPrint("Heap = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());
  // Serial.println("setup(): driverboard");
  // ensure targetposition will be same as focuser position
  // otherwise after loading driverboard focuser will start moving immediately
  ftargetPosition = mySetupData->get_fposition();
  driverboard = new DriverBoard(DRVBRD, mySetupData->get_fposition() );
  // ensure driverboard position is same as setupData
  DebugPrintln(DRVBRDDONESTR);
  oledtextmsg(DRVBRDDONESTR, -1, false, true);
  delay(5);
  HDebugPrint("Heap = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());

  // range check focuser variables
  mySetupData->set_stepmode((mySetupData->get_stepmode() < 1 ) ? 1 : mySetupData->get_stepmode());
  mySetupData->set_coilpower((mySetupData->get_coilpower() >= 1) ?  1 : 0);
  mySetupData->set_reversedirection((mySetupData->get_reversedirection() >= 1) ?  1 : 0);
  mySetupData->set_lcdpagetime((mySetupData->get_lcdpagetime() < LCDPAGETIMEMIN) ? mySetupData->get_lcdpagetime() : LCDPAGETIMEMIN);
  mySetupData->set_lcdpagetime((mySetupData->get_lcdpagetime() > LCDPAGETIMEMAX) ? LCDPAGETIMEMAX : mySetupData->get_lcdpagetime());
  mySetupData->set_maxstep((mySetupData->get_maxstep() < FOCUSERLOWERLIMIT) ? FOCUSERLOWERLIMIT : mySetupData->get_maxstep());
  mySetupData->set_stepsize((float)(mySetupData->get_stepsize() < 0.0 ) ? 0 : mySetupData->get_stepsize());
  mySetupData->set_stepsize((float)(mySetupData->get_stepsize() > MAXIMUMSTEPSIZE ) ? MAXIMUMSTEPSIZE : mySetupData->get_stepsize());

  driverboard->setstepmode(mySetupData->get_stepmode());      // restore stepmode

  // set focuser position in DriverBoard
  driverboard->setposition(mySetupData->get_fposition());

  // set coilpower
  DebugPrintln(CHECKCPWRSTR);
  if (mySetupData->get_coilpower() == 0)
  {
    driverboard->releasemotor();
    DebugPrintln(CPWRRELEASEDSTR);
  }

  delay(5);

  // setup home position switch input pin
#ifdef HOMEPOSITIONSWITCH
  pinMode(HPSWPIN, INPUT_PULLUP);
#endif

  // Setup infra red remote
#ifdef INFRAREDREMOTE
  init_irremote();
#endif

  // setup joystick
#ifdef JOYSTICK1
  init_joystick1();
#endif

#ifdef JOYSTICK2
  init_joystick2();
#endif

  isMoving = 0;

  if ( mySetupData->get_temperatureprobestate() == 1)
  {
    read_temp(1);
  }

#ifdef OTAUPDATES
  start_otaservice();                       // Start the OTA service
#endif // if defined(OTAUPDATES)

  HDebugPrint("Heap = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());
  HDebugPrintln("setup(): management server");
#ifdef MANAGEMENT
  start_management();
#endif
  HDebugPrint("Heap = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());

  DebugPrintln("setup(): web server");
  if ( mySetupData->get_webserverstate() == 1)
  {
    start_webserver();
  }

  DebugPrintln("setup(): ascom server");
  if ( mySetupData->get_ascomserverstate() == 1)
  {
    start_ascomremoteserver();
  }

#ifdef MDNSSERVER
  start_mdns_service();
#endif

  // setup duckdns
#ifdef USEDUCKDNS
  init_duckdns();
#endif

  DebugPrint(CURRENTPOSSTR);
  DebugPrintln(driverboard->getposition());
  DebugPrint(TARGETPOSSTR);
  DebugPrintln(ftargetPosition);
  DebugPrintln(SETUPENDSTR);
  oledtextmsg(SETUPENDSTR, -1, false, true);

  if ( mySetupData->get_inoutledstate() == 1)
  {
#if (DRVBRD == PRO2ESP32ULN2003 || DRVBRD == PRO2ESP32L298N || DRVBRD == PRO2ESP32L293DMINI || DRVBRD == PRO2ESP32L9110S) || (DRVBRD == PRO2ESP32DRV8825 )
    digitalWrite(INLEDPIN, 0);
    digitalWrite(OUTLEDPIN, 0);
#endif
  }

  halt_alert = false;
#ifdef TIMESETUP
  Serial.print("setup(): ");
  Serial.println(millis());
#endif
}

//_____________________ loop()___________________________________________

extern volatile uint32_t stepcount;     // number of steps to go in timer interrupt service routine
extern volatile bool timerSemaphore;

void loop()
{
  static StateMachineStates MainStateMachine = State_Idle;
  static uint32_t backlash_count = 0;
  static bool DirOfTravel = (bool) mySetupData->get_focuserdirection();
  static uint32_t TimeStampDelayAfterMove = 0;
  static uint32_t TimeStampPark = millis();
  static bool Parked = mySetupData->get_coilpower();
  static uint8_t updatecount = 0;
  static uint32_t steps = 0;

  static connection_status ConnectionStatus = disconnected;
  static oled_state oled = oled_on;

#ifdef HOMEPOSITIONSWITCH
  int stepstaken = 0;
  bool hpswstate = false;
#endif

#ifdef TIMELOOP
  Serial.print("loop(): ");
  Serial.println(millis());
#endif

#if defined(ACCESSPOINT) || defined(STATIONMODE)
  if (ConnectionStatus == disconnected)
  {
    myclient = myserver.available();
    if (myclient)
    {
      DebugPrintln(TCPCLIENTCONNECTSTR);
      if (myclient.connected())
      {
        ConnectionStatus = connected;
      }
    }
  }
  else
  {
    // is data available from the client request
    if (myclient.connected())
    {
      if (myclient.available())
      {
        ESP_Communication(); // Wifi communication
      }
    }
    else
    {
      DebugPrintln(TCPCLIENTDISCONNECTSTR);
      myclient.stop();
      ConnectionStatus = disconnected;
      oled = oled_on;
    }
  }
#endif // defined(ACCESSPOINT) || defined(STATIONMODE)

#ifdef BLUETOOTHMODE
  if ( SerialBT.available() )
  {
    processbt();
  }
  // if there is a command from Bluetooth
  if ( queue.count() >= 1 )                 // check for serial command
  {
    ESP_Communication();
  }
#endif // ifdef Bluetoothmode

#ifdef LOCALSERIAL
  // if there is a command from Serial port
  if ( Serial.available() )
  {
    processserial();
  }
  if ( queue.count() >= 1 )                 // check for serial command
  {
    halt_alert = ESP_Communication();
  }
#endif // ifdef LOCALSERIAL

#ifdef OTAUPDATES
  if ( otaupdatestate == RUNNING )
  {
    ArduinoOTA.handle();                      // listen for OTA events
  }
#endif // ifdef OTAUPDATES

  if ( ascomserverstate == RUNNING)
  {
    ascomserver->handleClient();
    checkASCOMALPACADiscovery();
  }

  if ( webserverstate == RUNNING )
  {
    webserver->handleClient();
  }

#ifdef MANAGEMENT
  if ( managementserverstate == RUNNING )
  {
    mserver.handleClient();
  }
#endif

  //_____________________________MainMachine _____________________________

  switch (MainStateMachine)
  {
    case State_Idle:
      if (driverboard->getposition() != ftargetPosition)
      {
        isMoving = 1;
        driverboard->enablemotor();
        MainStateMachine = State_InitMove;
        DebugPrint(STATEINITMOVE);
        DebugPrint(CURRENTPOSSTR);
        DebugPrintln(driverboard->getposition());
        DebugPrint(TARGETPOSSTR);
        DebugPrintln(ftargetPosition);
      }
      else
      {
        // focuser stationary. isMoving is 0
        if (mySetupData->SaveConfiguration(driverboard->getposition(), DirOfTravel)) // save config if needed
        {
          oled = oled_off;
          DebugPrint(CONFIGSAVEDSTR);
        }

#ifdef PUSHBUTTONS
        update_pushbuttons();
#endif
#ifdef JOYSTICK1
        update_joystick1();
#endif
#ifdef JOYSTICK2
        update_joystick2();
#endif
#ifdef INFRAREDREMOTE
        update_irremote();
#endif
        if (mySetupData->get_displayenabled() == 1)
        {
#if defined(OLEDTEXT)
          myoled->update_oledtextdisplay();
#endif
        }
        else
        {
          oled = oled_off;
        }
#if defined(OLEDGRAPHICS)
        myoled->Update_Oled(oled, ConnectionStatus);
#endif

        if ( mySetupData->get_temperatureprobestate() == 1)
        {
          update_temp();
        }

        if (Parked == false)
        {
          if (TimeCheck(TimeStampPark, MotorReleaseDelay))   //Power off after MotorReleaseDelay
          {
            // need to obey rule - can only release motor if coil power is disabled
            if ( mySetupData->get_coilpower() == 0 )
            {
              driverboard->releasemotor();
              DebugPrintln(RELEASEMOTORSTR);
            }
            Parked = true;
          } // if (TimeCheck(TimeStampPark, MotorReleaseDelay))
        } // if (Parked == false)
      } // if (driverboard->getposition() != ftargetPosition)
      break;

    case State_InitMove:
      isMoving = 1;
      backlash_count = 0;
      DirOfTravel = (ftargetPosition > driverboard->getposition()) ? moving_out : moving_in;
      driverboard->enablemotor();
      if (mySetupData->get_focuserdirection() != DirOfTravel)
      {
        mySetupData->set_focuserdirection(DirOfTravel);
        // move is in opposite direction, check for backlash enabled
        // get backlash settings
        if ( DirOfTravel == moving_in)
        {
          if (mySetupData->get_backlash_in_enabled())
          {
            backlash_count = mySetupData->get_backlashsteps_in();
          }
        }
        else
        {
          if (mySetupData->get_backlash_out_enabled())
          {
            backlash_count = mySetupData->get_backlashsteps_out();
          }
        } // if ( DirOfTravel == moving_in)

        if (DirOfTravel != moving_main && backlash_count)
        {
          uint32_t sm = mySetupData->get_stepmode();
          uint32_t bl = backlash_count * sm;
          DebugPrint(F("bl: "));
          DebugPrint(bl);
          DebugPrint(F(" "));

          if (DirOfTravel == moving_out)
          {
            backlash_count = bl + sm - ((ftargetPosition + bl) % sm); // Trip to tuning point should be a fullstep position
          }
          else
          {
            backlash_count = bl + sm + ((ftargetPosition - bl) % sm); // Trip to tuning point should be a fullstep position
          }

          DebugPrint(F("backlash_count: "));
          DebugPrint(backlash_count);
          DebugPrint(F(" "));
        } // if (DirOfTravel != moving_main && backlash_count)
      } // if (mySetupData->get_focuserdirection() != DirOfTravel)

      steps = (driverboard->getposition() > ftargetPosition) ? driverboard->getposition() - ftargetPosition : driverboard->getposition() - fcurrentPosition;
      DebugPrint(STATEMOVINGSTR);
      DebugPrint(steps);
      HDebugPrint("heap before move : ");
      HDebugPrintf("%u\n", ESP.getFreeHeap());
      driverboard->initmove(DirOfTravel, steps + backlash_count, mySetupData->get_motorSpeed(), mySetupData->get_inoutledstate());
      MainStateMachine = State_Moving;
      break;

    //_______________________________State_Moving

    case State_Moving:
      //Serial.println("S_M");
      if ( timerSemaphore == true )
      {
        // move has completed, the driverboard keeps track of focuser position
        DebugPrintln("Move completed");
        DebugPrintln("Going to State_DelayAfterMove");
        MainStateMachine = State_DelayAfterMove;
        DebugPrintln(STATEDELAYAFTERMOVE);
      }
      else
      {
        // timer semaphore is false. still moving, we need to check for halt and hpsw closure
        if ( halt_alert )
        {
          DebugPrintln("halt_alert");
          halt_alert = false;                             // reset alert flag
          ftargetPosition = driverboard->getposition();
          mySetupData->set_fposition(driverboard->getposition());
          int haltsteps = driverboard->halt();            // halt returns stepcount
          // we no longer need to keep track of steps here or halt because driverboard updates position on every move
          DebugPrintln("Going to State_DelayAfterMove");
          MainStateMachine = State_DelayAfterMove;
          DebugPrintln(STATEDELAYAFTERMOVE);
        } // if ( halt_alert )
        if (HPS_alert)                                    // check if home position sensor activated?
        {
          if (driverboard->getposition() > 0)
          {
            DebugPrintln(HPCLOSEDFPNOT0STR);
          }
          else
          {
            DebugPrintln(HPCLOSEDFP0STR);
          } // if (driverboard->getposition() > 0)
          DebugPrintln(STATESETHOMEPOSITION);
          ftargetPosition = 0;
          driverboard->setposition(0);
          mySetupData->set_fposition(0);

          if ( mySetupData->get_showhpswmsg() == 1)     // check if display home position messages is enabled
          {
            if (mySetupData->get_displayenabled() == 1)
            {
              oledtextmsg(HPCLOSEDFP0STR, -1, true, true);
            }
          }
          // we should jump to
          MainStateMachine = State_SetHomePosition;
        } // if (HPS_alert)

        // if the update position on display when moving is enabled, then update the display
        if ( mySetupData->get_lcdupdateonmove() == 1)
        {
          // update position counter on display if there is an enabled display
          if (mySetupData->get_displayenabled() == 1)
          {
            updatecount++;
            if ( updatecount > LCDUPDATEONMOVE )        // only update every 15th move to avoid overhead
            {
              updatecount = 0;
#if defined(OLEDGRAPHICS)
              update_oledtext_position();
#endif
            }  // !!!!!!!!!!!!!!!!!!!  no support for  Graphic OLED Mode !!!!!!!!!!!!!!!!!!!!!!!
          } // if (mySetupData->get_displayenabled() == 1)
        } // if ( get_lcdupdateonmove() == 1)
      }
      break;

    case State_SetHomePosition:                         // move out till home position switch opens
      DebugPrintln("State_SetHomePosition");
#ifdef HOMEPOSITIONSWITCH
      // check if display home position switch messages is enabled
      if ( mySetupData->get_showhpswmsg() == 1)
      {
        if (mySetupData->get_displayenabled() == 1)
        {
          oledtextmsg(HPMOVETILLOPENSTR, -1, false, true);
        }
      }
      // HOME POSITION SWITCH IS CLOSED - Step out till switch opens then set position = 0
      stepstaken = 0;                                   // Count number of steps to prevent going too far
      DebugPrintln(F(HPMOVETILLOPENSTR));
      DirOfTravel = !DirOfTravel;                       // We were going in, now we need to reverse and go out
      hpswstate = HPSWCLOSED;                           // We know we got here because switch was closed
      while ( hpswstate == HPSWCLOSED )
      {
        if ( mySetupData->get_reversedirection() == 0 )
        {
          steppermotormove(DirOfTravel);                // take 1 step
        }
        else
        {
          steppermotormove(!DirOfTravel);
        }
        stepstaken++;                                   // increment steps taken
        if ( stepstaken > HOMESTEPS )                   // this prevents the endless loop if the hpsw is not connected or is faulty
        {
          DebugPrintln(F(HPMOVEOUTERRORSTR));
          hpswstate = HPSWOPEN;
        }
        else
        {
          hpswstate = !(digitalRead(HPSWPIN));          // read state of HPSW
        }
      }
      DebugPrint(F(HPMOVEOUTSTEPSSTR));
      DebugPrintln(stepstaken);
      DebugPrintln(F(HPMOVEOUTFINISHEDSTR));
      ftargetPosition = 0;
      driverboard->setposition(0);
      mySetupData->set_fposition(0);

      if ( mySetupData->get_showhpswmsg() == 1)         // check if display home position switch messages is enabled
      {
        if (mySetupData->get_displayenabled() == 1)
        {
          oledtextmsg(HPMOVEOUTFINISHEDSTR, -1, true, true);
        }
      }
#endif // #ifdef HOMEPOSITIONSWITCH
      MainStateMachine = State_DelayAfterMove;
      TimeStampDelayAfterMove = millis();
      DebugPrintln(F(STATEDELAYAFTERMOVE));
      break;

    //_______________________________State_DelayAfterMove

    case State_DelayAfterMove:
      HDebugPrint("Heap after move = ");
      HDebugPrintf("%u\n", ESP.getFreeHeap());
      // apply Delayaftermove, this MUST be done here in order to get accurate timing for DelayAfterMove
      if (TimeCheck(TimeStampDelayAfterMove , mySetupData->get_DelayAfterMove()))
      {
        oled = oled_on;
        isMoving = 0;
        TimeStampPark  = millis();                      // catch current time
        Parked = false;                                 // mark to park the motor in State_Idle
        MainStateMachine = State_Idle;
        DebugPrint(F(">State_Idle "));
      }
      break;

    default:
      DebugPrintln(F("Error: wrong State => State_Idle"));
      MainStateMachine = State_Idle;
      break;
  }

#ifdef TIMELOOP
  Serial.print("loop(): ");
  Serial.println(millis());
#endif
} // end Loop()
