// CONFIG IS ACCESSPOINT AND MANAGEMENT SERVER
// Target is ESP8266, Node MCU 12E

// Test Procedure
// Open TCPIP app, connect to focuser, set position at 35000. use preset0=35000, preset1=4500, move from preset1 to preset0
// Test to see if crash when moving - if crash then increase MSPEED value for that driver board and retest

// Incompatibility discovered:
// An interrupt occuring whilst during SPIFFS access has a HIGH likelyhood of generating an exception causing a reboot
// Implications: Cannot load web pages using webserserver/management server/ ascom remote aplaca server WHEN focuser is MOVING
// If one disables interrupts during SPIFFS acccess then that causes significant pauses in motor rotation

// Driver Boards
// DRV8825 DONE
// ULN2003 Driver Board DONE

// Changes
// Fix for print(F()) errors
// Fix for OLEDTEXT compile error
// Fix for Focuser position not being updated on display when moving
// Fixes for HalfStepperESP32 library code - must reinstall

// TODO
// Open TCPIP app, connect to focuser, set position at 35000. use preset0=35000, preset1=4500, move from preset1 to preset0
// Test these driver boards and update driver board code to work
// L298N Driver Board
// L293DNEMA and L293D28BYJ48 Driver Board
// L293DMINI Driver Board
// L9110S Driver Board
// Check IRREMOTE on ESP32 boards, try IRREMOTE test program - created new library to reduce footprint 60K -> 4K

// ISSUE 1: OLED
// I have commented out display.h and display.cpp just to get a compile
// If OLEDGRAPHIC is defined then it will not compile

// ----------------------------------------------------------------------------------------------
// TITLE: myFP2ESP FIRMWARE OFFICIAL RELEASE 130
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
char mySSID[64]     = "myfp2eap";           // you need to set this to your WiFi network SSID
char myPASSWORD[64] = "myfp2eap";           // and you need to set the correct password

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
//byte    tprobe1;                            // indicate if there is a probe attached to myFocuserPro2
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
      newpos = mySetupData->get_fposition() + adjpos;
      newpos = (newpos < 0 ) ? 0 : newpos;
      ftargetPosition = newpos;
    }
    else if ( adjpos > 0)
    {
      newpos = mySetupData->get_fposition() + adjpos;
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
extern ESP8266WebServer *webserver;
#else
extern WebServer *webserver;
#endif // if defined(esp8266)










/*

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
    DebugPrintln(READPAGESTR);
    WSpg = file.readString();                           // read contents into string
    file.close();

    DebugPrintln(PROCESSPAGESTARTSTR);
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
    DebugPrintln(PROCESSPAGEENDSTR);
  }
  else
  {
    TRACE();
    DebugPrintln(FSFILENOTFOUNDSTR);
    DebugPrintln(BUILDDEFAULTPAGESTR);
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
    DebugPrintln(READPAGESTR);
    WSpg = file.readString();                           // read contents into string
    file.close();

    DebugPrintln(PROCESSPAGESTARTSTR);
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
    WSpg.replace("%CPO%", String(driverboard->getposition()));
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
    DebugPrintln(PROCESSPAGEENDSTR);
  }
  else
  {
    // could not read preset file from SPIFFS
    TRACE();
    DebugPrintln(FSFILENOTFOUNDSTR);
    DebugPrintln(BUILDDEFAULTPAGESTR);
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
    DebugPrintln(READPAGESTR);
    WSpg = file.readString();                           // read contents into string
    file.close();

    DebugPrintln(PROCESSPAGESTARTSTR);
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
    WSpg.replace("%CPO%", String(driverboard->getposition()));
    WSpg.replace("%TPO%", String(ftargetPosition));
    WSpg.replace("%MOV%", String(isMoving));
    DebugPrintln(PROCESSPAGEENDSTR);
  }
  else
  {
    // could not read move file from SPIFFS
    TRACE();
    DebugPrintln(FSFILENOTFOUNDSTR);
    DebugPrintln(BUILDDEFAULTPAGESTR);
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
    long newtemp = (long) driverboard->getposition() + temp;
    newtemp = ( newtemp < 0 ) ? 0 : newtemp;
    newtemp = ( newtemp > (long)mySetupData->get_maxstep()) ? mySetupData->get_maxstep() : newtemp;
    ftargetPosition = (unsigned long) newtemp;
    DebugPrint("Move = "); DebugPrintln(fmv_str);
    DebugPrint(CURRENTPOSSTR);
    DebugPrintln(driverboard->getposition());
    DebugPrint(TARGETPOSSTR);
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
    DebugPrintln(READPAGESTR);
    WSpg = file.readString();                           // read contents into string
    file.close();

    DebugPrintln(PROCESSPAGESTARTSTR);
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
      WSpg.replace("%CPO%", String(driverboard->getposition()));
    }
    WSpg.replace("%TPO%", String(ftargetPosition));
    WSpg.replace("%MAX%", String(mySetupData->get_maxstep()));
    WSpg.replace("%MOV%", String(isMoving));
    if ( mySetupData->get_temperatureprobestate() == 1 )
    {
      if ( mySetupData->get_tempmode() == 1)
      {
        String tpstr = String(myTempProbe->read_temp(1), 2) + " c";
        WSpg.replace("%TEM%", tpstr);
      }
      else
      {
        float ft = myTempProbe->read_temp(1);
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
    DebugPrintln(PROCESSPAGEENDSTR);
  }
  else
  {
    // could not read index file from SPIFFS
    TRACE();
    DebugPrintln(FSFILENOTFOUNDSTR);
    DebugPrintln(BUILDDEFAULTPAGESTR);
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
  webserver->send(NORMALWEBPAGE, PLAINTEXTPAGETYPE, String(driverboard->getposition())); // Send position value only to client ajax request
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
      driverboard->setposition(ftargetPosition);
      mySetupData->set_fposition(ftargetPosition);
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
    if ( temp < (long) driverboard->getposition() )               // if maxstep is less than focuser position
    {
      temp = (long) driverboard->getposition() + 1;
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
  DebugPrintln(SENDPAGESTR);
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
    DebugPrintln(FSNOTSTARTEDSTR);
    DebugPrintln(SERVERSTATESTOPSTR);
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
  DebugPrintln(SERVERSTATESTARTSTR);
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
    DebugPrintln(SERVERSTATESTOPSTR);
  }
  else
  {
    DebugPrintln(SERVERNOTRUNNINGSTR);
  }
  delay(10);                                            // small pause so background tasks can run
}
// WEBSERVER END -------------------------------------------------------------


*/















// ----------------------------------------------------------------------------------------------
// 25: OTAUPDATES - CHANGE AT YOUR OWN PERIL
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
// 26: DUCKDNS - CHANGE AT YOUR OWN PERIL
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

extern void stop_management(void);

void software_Reboot(int Reboot_delay)
{
  myoled->oledtextmsg(WIFIRESTARTSTR, -1, true, false);
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
//    init_temp();                                        // start temp probe
    myTempProbe = new TempProbe;    
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
  WiFi.config(ip, dns, gateway, subnet);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(mySSID, myPASSWORD);
#endif // end ACCESSPOINT
  HDebugPrint("Heap = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());

  // this is setup as a station connecting to an existing wifi network
#ifdef STATIONMODE
  DebugPrintln(STARTSMSTR);
  myoled->oledtextmsg(STARTSMSTR, -1, false, true);
  if (staticip == STATICIPON)                   // if staticip then set this up before starting
  {
    DebugPrintln(SETSTATICIPSTR);
    myoled->oledtextmsg(SETSTATICIPSTR, -1, false, true);
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

  DebugPrint("WifiHostname: ");
  DebugPrintln(WiFi.getHostname());

  myoled->oledtextmsg(CONNECTEDSTR, -1, true, true);
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
  // it is Bluetooth so set some globals
  ipStr = ip_zero;      // "0.0.0.0"
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
