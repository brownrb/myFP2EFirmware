// ----------------------------------------------------------------------------------------------
// TITLE: myFP2ESP FIRMWARE OFFICIAL RELEASE 131
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

#if defined(ACCESSPOINT) || defined(STATIONMODE)
char mySSID[64]     = "myfp2eap";             // you need to set this to your WiFi network SSID
char myPASSWORD[64] = "myfp2eap";             // and you need to set the correct password

IPAddress ESP32IPAddress;
String ServerLocalIP;
WiFiServer myserver(SERVERPORT);
WiFiClient myclient;                          // only one client supported, multiple connections denied
IPAddress myIP;
long rssi;
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

#include "temp.h"
TempProbe *myTempProbe;

#include "displays.h"
OLED_NON *myoled;

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

unsigned long ftargetPosition;              // target position
volatile bool halt_alert;

boolean displayfound;
byte    isMoving;                           // is the motor currently moving
char    ipStr[16];                          // shared between BT mode and other modes

int     packetsreceived;
int     packetssent;
bool    mdnsserverstate;                    // states for services, RUNNING | STOPPED
bool    ascomdiscoverystate;
bool    managementserverstate;
bool    tcpipserverstate;
bool    otaupdatestate;
bool    duckdnsstate;

SetupData *mySetupData;                       // focuser data

#if defined(ESP8266)
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif // if defined(esp8266)

#if defined(ESP8266)
#undef DEBUG_ESP_HTTP_SERVER
extern ESP8266WebServer mserver;
#else
extern WebServer mserver;
#endif // if defined(esp8266)

extern String MSpg;
extern void start_management(void);
extern void start_ascomremoteserver(void);
extern void checkASCOMALPACADiscovery(void);

#if defined(ESP8266)
extern ESP8266WebServer *ascomserver;
#else
extern WebServer *ascomserver;
#endif // if defined(esp8266)

extern void start_webserver(void);

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
// 17: INFRARED REMOTE CONTROLLER - CHANGE AT YOUR OWN PERIL
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
    
    if ( (isMoving == 1) && (lastcode == IR_HALT))
    {
      halt_alert = true;
    }
    else
    {
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
      } // switch(lastcode)
    } // if ( (isMoving == 1) && (lastcode == IR_HALT))
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
// 18: JOYSTICK - CHANGE AT YOUR OWN PERIL
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
// 19: PUSHBUTTONS - CHANGE AT YOUR OWN PERIL
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
// 20: mDNS SERVER - CHANGE AT YOUR OWN PERIL
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
// 21: WEBSERVER - CHANGE AT YOUR OWN PERIL
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
extern ESP8266WebServer *webserver;
#else
extern WebServer *webserver;
#endif // if defined(esp8266)

// ----------------------------------------------------------------------------------------------
// 22: OTAUPDATES - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#if defined(OTAUPDATES)
#include <ArduinoOTA.h>

void start_otaservice()
{
  DebugPrintln(STARTOTASERVICESTR);
  myoled->oledtextmsg(STARTOTASERVICESTR, -1, false, true);
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
// 23: DUCKDNS - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#ifdef USEDUCKDNS
#include <EasyDDNS.h>                           // https://github.com/ayushsharma82/EasyDDNS

void init_duckdns(void)
{
  DebugPrintln(SETUPDUCKDNSSTR);
  myoled->oledtextmsg(SETUPDUCKDNSSTR, -1, false, true);
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
// 24: FIRMWARE - CHANGE AT YOUR OWN PERIL
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

extern void stop_management(void);

void software_Reboot(int Reboot_delay)
{
  myoled->oledtextmsg(WIFIRESTARTSTR, -1, true, false);
  mySetupData->SaveNow();                               // save the focuser settings immediately

  // a reboot causes everything to reset, so code to stop services etc is not really needed
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
    delay(100);
    digitalWrite(INLEDPIN, 0);
    digitalWrite(OUTLEDPIN, 0);
#endif
  }
}

#ifdef READWIFICONFIG
bool readwificonfig( char* xSSID, char* xPASSWORD)
{
  const String filename = "/wificonfig.json";
  String SSID;
  String PASSWORD;
  boolean status = false;

  DebugPrintln(CHECKWIFICONFIGFILESTR);
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
    DebugPrintln(FILENOTFOUNDSTR);
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
      DebugPrintln(DESERIALIZEERRORSTR);
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
  //  Serial.begin(SERIALPORTSPEED);
  DebugPrintln(SERIALSTARTSTR);
  DebugPrintln(DEBUGONSTR);
#endif
  delay(100);                                   // go on after statement does appear

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

  
#ifdef OLED_MODE
  if (CheckOledConnected())
  {
    myoled = new OLED_MODE;                       // Start configured OLED display object
    DebugPrintln(F("init OLED OLED_MODE"));
  }
  else
  {
    myoled = new OLED_NON;
    DebugPrintln(F("init OLED OLED_NON"));
  }
#else
  myoled = new OLED_NON;
#endif

  HDebugPrint("Heap = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());

/*
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
  DebugPrint(F("Show hpsw messages="));
  DebugPrintln(mySetupData->get_showhpswmsg());
  DebugPrint(F("Forcedownload state="));
  DebugPrintln(mySetupData->get_forcedownload());
  DebugPrint(F("Display page option="));
  DebugPrintln(mySetupData->get_oledpageoption());
*/

  // attempt start of temperatrure probe
  // if the state is 0, then myTempProbe will still be Null
  if ( mySetupData->get_temperatureprobestate() == 1)
  {
    myTempProbe = new TempProbe;                       // call constructor
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
  myoled->oledtextmsg(STARTAPSTR, -1, true, true);
  DebugPrintln(STARTAPSTR);
  HDebugPrintln("setup(): accesspoint");
  HDebugPrint("Heap = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());
  HDebugPrintln();
  WiFi.mode(WIFI_AP);
  delay(200);
  WiFi.softAP(mySSID, myPASSWORD);
  delay(200);
  WiFi.config(ip, dns, gateway, subnet);
#endif // end ACCESSPOINT

  HDebugPrint("Heap = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());

  // this is setup as a station connecting to an existing wifi network
#ifdef STATIONMODE
  DebugPrintln(STARTSMSTR);
  myoled->oledtextmsg(STARTSMSTR, -1, false, true);
  WiFi.mode(WIFI_STA);

  if (staticip == STATICIPON)                   // if staticip then set this up before starting
  {
    DebugPrintln(SETSTATICIPSTR);
    myoled->oledtextmsg(SETSTATICIPSTR, -1, false, true);
    WiFi.config(ip, dns, gateway, subnet);
    delay(5);
  }

  // Log on to LAN
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
    myoled->oledtextmsg(ATTEMPTSSTR, attempts, false, true);
    if (attempts > 9)                          // if this attempt is 10 or more tries
    {
      DebugPrintln(APCONNECTFAILSTR);
      DebugPrintln(WIFIRESTARTSTR);
      myoled->oledtextmsg(APCONNECTFAILSTR + String(mySSID), -1, true, true);
      myoled->oledgraphicmsg(APSTARTFAILSTR + String(mySSID), -1, true);
      delay(2000);
      software_Reboot(2000);                    // GPIO0 must be HIGH and GPIO15 LOW when calling ESP.restart();
    }
  }
#endif // end STATIONMODE

  
  myoled->oledtextmsg(CONNECTEDSTR, -1, true, true);
  delay(100);                                   // keep delays small else issue with ASCOM

  tcpipserverstate = STOPPED;
  mdnsserverstate = STOPPED;
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
  myoled->oledtextmsg(STARTTCPSERVERSTR, -1, false, true);
  start_tcpipserver();
  DebugPrintln(GETLOCALIPSTR);
  ESP32IPAddress = WiFi.localIP();
  delay(100);                                   // keep delays small else issue with ASCOM
  DebugPrintln(TCPSERVERSTARTEDSTR);
  myoled->oledtextmsg(TCPSERVERSTARTEDSTR, -1, false, true);
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
  // it is Bluetooth or LOCALSERIAL so set some globals
  snprintf(ipStr, sizeof(ipStr), "%02i.%02i.%02i.%02i",  0, 0, 0, 0);
#endif // if defined(ACCESSPOINT) || defined(STATIONMODE)

  // assign to current working values
  //ftargetPosition = fcurrentPosition = mySetupData->get_fposition();
  ftargetPosition = mySetupData->get_fposition();

  DebugPrint(SETUPDRVBRDSTR);
  DebugPrintln(DRVBRD);
  myoled->oledtextmsg(SETUPDRVBRDSTR, DRVBRD, true, true);

  HDebugPrint("Heap = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());
  // Serial.println("setup(): driverboard");
  // ensure targetposition will be same as focuser position
  // otherwise after loading driverboard focuser will start moving immediately
  ftargetPosition = mySetupData->get_fposition();
  driverboard = new DriverBoard(DRVBRD, mySetupData->get_fposition() );
  // ensure driverboard position is same as setupData
  DebugPrintln(DRVBRDDONESTR);
  myoled->oledtextmsg(DRVBRDDONESTR, -1, false, true);
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
    myTempProbe->read_temp(1);
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

 
  if ( mySetupData->get_webserverstate() == 1)
  {
    DebugPrintln("setup(): web server");
    start_webserver();
  }

  delay(4000);
 
  if ( mySetupData->get_ascomserverstate() == 1)
  {
    DebugPrintln("setup(): ascom server");
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
  myoled->oledtextmsg(SETUPENDSTR, -1, false, true);

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
  static          StateMachineStates MainStateMachine = State_Idle;
  static uint32_t backlash_count = 0;
  static bool     DirOfTravel = (bool) mySetupData->get_focuserdirection();
  static uint32_t TimeStampDelayAfterMove = 0;
  static uint32_t TimeStampPark = millis();
  static bool     Parked = mySetupData->get_coilpower();
  static uint8_t  updatecount = 0;
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
    ESP_Communication();
  }
#endif // ifdef LOCALSERIAL

#ifdef OTAUPDATES
  if ( otaupdatestate == RUNNING )
  {
    ArduinoOTA.handle();                      // listen for OTA events
  }
#endif // ifdef OTAUPDATES

  if ( mySetupData->get_ascomserverstate() == 1)
  {
    ascomserver->handleClient();
    checkASCOMALPACADiscovery();
  }

  if ( mySetupData->get_webserverstate() == 1)
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

          myoled->update_oledtextdisplay();

        }
        else
        {
          oled = oled_off;
        }

        myoled->Update_Oled(oled, ConnectionStatus);


        if ( mySetupData->get_temperatureprobestate() == 1)
        {
          myTempProbe->update_temp();
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
          DebugPrint("bl: ");
          DebugPrint(bl);
          DebugPrint(" ");

          if (DirOfTravel == moving_out)
          {
            backlash_count = bl + sm - ((ftargetPosition + bl) % sm); // Trip to tuning point should be a fullstep position
          }
          else
          {
            backlash_count = bl + sm + ((ftargetPosition - bl) % sm); // Trip to tuning point should be a fullstep position
          }

          DebugPrint("backlash_count: ");
          DebugPrint(backlash_count);
          DebugPrint(" ");
        } // if (DirOfTravel != moving_main && backlash_count)
      } // if (mySetupData->get_focuserdirection() != DirOfTravel)

      // if target pos > current pos then steps = target pos - current pos
      // if target pos < current pos then steps = current pos - target pos
      steps = (ftargetPosition > driverboard->getposition()) ? ftargetPosition - driverboard->getposition() : driverboard->getposition() - ftargetPosition;
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
              myoled->oledtextmsg(HPCLOSEDFP0STR, -1, true, true);
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
              myoled->update_oledtext_position();
            }
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
          myoled->oledtextmsg(HPMOVETILLOPENSTR, -1, false, true);
        }
      }
      // HOME POSITION SWITCH IS CLOSED - Step out till switch opens then set position = 0
      stepstaken = 0;                                   // Count number of steps to prevent going too far
      DebugPrintln(HPMOVETILLOPENSTR);
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
          DebugPrintln(HPMOVEOUTERRORSTR);
          hpswstate = HPSWOPEN;
        }
        else
        {
          hpswstate = !(digitalRead(HPSWPIN));          // read state of HPSW
        }
      }
      DebugPrint(HPMOVEOUTSTEPSSTR);
      DebugPrintln(stepstaken);
      DebugPrintln(HPMOVEOUTFINISHEDSTR);
      ftargetPosition = 0;
      driverboard->setposition(0);
      mySetupData->set_fposition(0);

      if ( mySetupData->get_showhpswmsg() == 1)         // check if display home position switch messages is enabled
      {
        if (mySetupData->get_displayenabled() == 1)
        {
          myoled->oledtextmsg(HPMOVEOUTFINISHEDSTR, -1, true, true);
        }
      }
#endif // #ifdef HOMEPOSITIONSWITCH
      MainStateMachine = State_DelayAfterMove;
      TimeStampDelayAfterMove = millis();
      DebugPrintln(STATEDELAYAFTERMOVE);
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
        DebugPrint(">State_Idle ");
      }
      break;

    default:
      DebugPrintln("Error: wrong State => State_Idle");
      MainStateMachine = State_Idle;
      break;
  }

#ifdef TIMELOOP
  Serial.print("loop(): ");
  Serial.println(millis());
#endif
} // end Loop()
