// ----------------------------------------------------------------------------------------------
// TITLE: myFP2ESP FIRMWARE RELEASE 109
// ----------------------------------------------------------------------------------------------
// myFP2ESP - Firmware for ESP8266 and ESP32 myFocuserPro2 Controllers
// Supports driver boards DRV8825, ULN2003, L298N, L9110S, L293DMINI
// ESP8266 Supports OLED display, Temperature Probe
// ESP32 Supports OLED display, Temperature Probe, Push Buttons, Direction LED's. Infrared Remote
// Supports modes, ACCESSPOINT, STATIONMODE, BLUETOOTH, LOCALSERIAL, WEBSERVER, ASCOMREMOTE
// Remember to change your target CPU depending on board selection

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2019. All Rights Reserved.
// (c) Copyright Holger M, 2019. All Rights Reserved.
// (c) Copyright Pieter P - OTA code based on example from Pieter  P.

// ----------------------------------------------------------------------------------------------
// SPECIAL LICENSE
// ----------------------------------------------------------------------------------------------
// This code is released under license. If you copy or write new code based on the code in these files
// you must include to link to these files AND you must include references to the authors of this code.

// ----------------------------------------------------------------------------------------------
// CONTRIBUTIONS
// ----------------------------------------------------------------------------------------------
// It is costly to continue development and purchase boards and components. We need your help to
// continue development of this project. If you wish to make a contribution in thanks for this project,
// please use PayPal and send the amount to user rbb1brown@gmail.com (Robert Brown).
// All contributions are gratefully accepted.

// ----------------------------------------------------------------------------------------------
// TO PROGRAM THE FIRMWARE
// ----------------------------------------------------------------------------------------------
// 1. Set your DRVBRD [section 1] in myBoards.h so the correct driver board is used
// 2. Set your target CPU to match the correct CPU for your board
// 3. Set the correct hardware options [section 3] in this file to match your hardware
// 4. Compile and upload to your controller

// ----------------------------------------------------------------------------------------------
// PCB BOARDS
// ----------------------------------------------------------------------------------------------
// ESP8266
//  ULN2003       https://aisler.net/p/UAAKPUTS
//  DRV8825       https://aisler.net/p/EKDGHUYW
//  L293D Shield  https://www.ebay.com/itm/L293D-Motor-Drive-Shield-Wifi-Module-For-Arduino-NodeMcu-Lua-ESP8266-ESP-12E/292619874436
// ESP32
//  ULN2003       https://aisler.net/p/OTEGMJNE
//  DRV8825       https://aisler.net/p/TYQHHGAI
//
//  ESP32 R3WEMOS https://www.ebay.com/itm/R3-Wemos-UNO-D1-R32-ESP32-WIFI-Bluetooth-CH340-Devolopment-Board-For-Arduino/264166013552
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
// OneWire 2.3.5
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
// 2: FOR ESP8266 BOARDS USING DRV8825 SET DRV8825STEPMODE in myBoards.h
//    FOR R3 WEMOS ESP32 USING DRV8825 SET DRV8825STEPMODE in myBoards.h
// ----------------------------------------------------------------------------------------------
// Remember to set DRV8825TEPMODE to the correct value if using WEMOS or NODEMCUV1 in myBoards.h

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
//#define TEMPERATUREPROBE 1

// To enable the OLED DISPLAY uncomment one of the next lines, deselect OLED display by uncomment both lines
//#define OLEDTEXT 1
//#define OLEDGRAPHICS 2

// do NOT uncomment HOMEPOSITIONSWITCH if you do not have the switch fitted
// To enable the HOMEPOSITION SWITCH, uncomment the next line
//#define HOMEPOSITIONSWITCH 1

// To enable backlash in this firmware, uncomment the next line
//#define BACKLASH 1
//#define BACKLASH 2    // ALTERNATIVE BACKLASH ALGORITHM

// To enable In and Out Pushbuttons in this firmware, uncomment the next line [ESP32 only]
//#define INOUTPUSHBUTTONS 1

// To enable the 2-Axis Joystick in this firmware, uncomment the next line [ESP32 only]
#define JOYSTICK 1

// To enable In and Out LEDS in this firmware, uncomment the next line [ESP32 only]
//#define INOUTLEDPINS 1

// To enable the Infrared remote controller, uncomment the next line [ESP32 only]
//#define INFRAREDREMOTE

// To enable the start boot screen showing startup messages, uncomment the next line
//#define SHOWSTARTSCRN 1

// To display Spash screen graphic, uncomment the next line
//#define SPLASHSCREEN 1

// DO NOT CHANGE
#if (DRVBRD == WEMOSDRV8825 || DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2EDRV8825BIG \
  || DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N   || DRVBRD == PRO2EL293DMINI \
  || DSRVBRD == PRO2EL9110S || DRVBRD == PRO2EL293D   || DRVBRD == PRO2ESP32R3WEMOS )
// no support for pushbuttons, inout leds, irremote
#ifdef INOUTPUSHBUTTONS
#halt // ERROR - INOUTPUSHBUTTONS not supported for WEMOS or NODEMCUV1 ESP8266 chips
#endif
#ifdef INOUTLEDPINS
#halt // ERROR - INOUTLEDPINS not supported for WEMOS or NODEMCUV1 ESP8266 chips
#endif
#ifdef INFRAREDREMOTE
#halt // ERROR - INFRAREDREMOTE not supported for WEMOS or NODEMCUV1 ESP8266 chips
#endif
#ifdef JOYSTICK
#halt // ERROR - JOYSTICK not supported for WEMOS or NODEMCUV1 ESP8266 chips
#endif
#endif

#if defined(OLEDGRAPHICS)
#if defined(OLEDTEXT)
#halt // ERROR - you must have either OLEDGRAPHICS or OLEDTEXT defined, NOT BOTH
#endif
#endif

// DO NOT CHANGE
#if (DRVBRD == WEMOSDRV8825 || DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2EDRV8825BIG \
  || DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N   || DRVBRD == PRO2EL293DMINI \
  || DSRVBRD == PRO2EL9110S || DRVBRD == PRO2EL293D )
// no support for home position switch
#ifdef HOMEPOSITIONSWITCH
#halt // ERROR - Home Position Switch not supported for WEMOS or NODEMCUV1 ESP8266 chips
#endif
#endif // 

#ifdef JOYSTICK
#ifdef INOUTPUSHBUTTONS
#halt // ERROR - you cannot have INOUTPUSHBUTTONS and JOYSTICK enabled at the same time
#endif
#endif

// ----------------------------------------------------------------------------------------------
// 5: SPECIFY THE CONTROLLER MODE HERE - ONLY ONE OF THESE MUST BE DEFINED
// ----------------------------------------------------------------------------------------------

// to enable Bluetooth mode, uncomment the next line [ESP32 only]
//#define BLUETOOTHMODE 1

// to work as an access point, define accesspoint - cannot use DUCKDNS
//#define ACCESSPOINT 2

// to work as a station accessing a AP, define stationmode
#define STATIONMODE 3

// to work only via USB cable as Serial port, uncomment the next line
//#define LOCALSERIAL 4

// To enable OTA updates, uncomment the next line [can only be used with stationmode]
//#define OTAUPDATES 5

// to enable this focuser for ASCOM ALPACA REMOTE support, uncomment the next line
//#define ASCOMREMOTE 6

// to enable Webserver interface, uncomment the next line [recommed use Internet Explorer or Microsoft Edge Browser]
//#define WEBSERVER 7

// mdns support [myfp2eap.local:8080]
// to enable multicast DNS, uncomment the next line
#define MDNSSERVER 8

// DO NOT CHANGE
#ifdef MDNSSERVER
#if defined(ESP8266)
#ifndef WEBSERVER
#include <ESP8266WebServer.h>
#endif
#include <ESP8266mDNS.h>
#else
#ifndef WEBSERVER
#include <WebServer.h>
#endif
#include <ESPmDNS.h>
#endif
#endif // mDNS

// DO NOT CHANGE
#if defined(MDNSSERVER)
#if defined(BLUETOOTHMODE) || defined(LOCALSERIAL)
#halt // ERROR, mDNS only available with STATIONMODE or ACCESSPOINT
#endif
#endif // MDNSSERVER

#if defined(WEBSERVER)
#if !defined(ACCESSPOINT) && !defined(STATIONMODE)
#halt //ERROR you must use ACCESSPOINT or STATIONMODE with WEBSERVER
#endif
#if defined(LOCALSERIAL)
#halt //ERROR you cannot have both WEBSERVER and LOCALSERIAL enabled at the same time
#endif
#if defined(BLUETOOTH)
#halt //ERROR you cannot have both WEBSERVER and BLUETOOTH enabled at the same time
#endif
#endif

#if defined(ASCOMREMOTE)
#if !defined(ACCESSPOINT) && !defined(STATIONMODE)
#halt //ERROR you must use ACCESSPOINT or STATIONMODE with ASCOMREMOTE
#endif
#if defined(WEBSERVER)
#halt //ERROR you cannot have both ASCOMREMOTE and WEBSERVER enabled at the same time
#endif
#if defined(LOCALSERIAL)
#halt //ERROR you cannot have both ASCOMREMOTE and LOCALSERIAL enabled at the same time
#endif
#if defined(BLUETOOTH)
#halt //ERROR you cannot have both ASCOMREMOTE and BLUETOOTH enabled at the same time
#endif
#endif

#if defined(OTAUPDATES)
#if defined(BLUETOOTHMODE) || defined(LOCALSERIAL)
#halt //ERROR you cannot have both OTAUPDATES with either BLUETOOTHMODE or LOCALSERIAL enabled at the same time
#endif
#if defined(ACCESSPOINT)
#halt //ERROR you cannot use ACCESSPOINT with OTAUPDATES
#endif
#endif

#ifdef ACCESSPOINT
#ifdef STATIONMODE
#halt // ERROR - Cannot have both ACCESSPOINT and STATIONMODE defined at the same time
#endif
#endif

#if defined(ACCESSPOINT) || defined(STATIONMODE)
#if defined(BLUETOOTHMODE)
#halt //ERROR you cannot have BLUETOOTHMODE with either ACCESSPOINT and STATIONMODE
#endif
#endif

#if defined(STATIONMODE) || defined(ACCESSPOINT) || defined(BLUETOOTHMODE)
#if defined(LOCALSERIAL)
#halt // ERROR - Cannot have both STATIONMODE/ACCESSPOINT/BLUETOOTHMODE and LOCALSERIAL mode defined at the same time
#endif
#endif

#if (DRVBRD == WEMOSDRV8825 || DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2EDRV8825BIG \
  || DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N   || DRVBRD == PRO2EL293DMINI \
  || DSRVBRD == PRO2EL9110S || DRVBRD == PRO2EL293D   || DRVBRD == PRO2ESP32R3WEMOS )
// no support for bluetooth mode
#ifdef BLUETOOTHMODE
#halt // ERROR - BLUETOOTHMODE not supported for WEMOS or NODEMCUV1 ESP8266 chips
#endif
#endif

#if (DRVBRD == PRO2EL293DNEMA || DRVBRD == PRO2EL293D28BYJ48)
#ifdef LOCALSERIAL
#halt // ERROR - LOCALSERIAL not supported L293D Motor Shield [ESP8266] boards
#endif
#endif
#define MotorReleaseDelay 120*1000          // motor release power after 120s

// ----------------------------------------------------------------------------------------------
// 6: INCLUDES FOR WIFI
// ----------------------------------------------------------------------------------------------
#if defined(OTAUPDATES)
#include <ArduinoOTA.h>
#endif

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
#include "FocuserSetupData.h"

#ifdef ASCOMREMOTE
#if defined(ESP8266)
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif // if defined(esp8266)
#endif // ascomremote

#ifdef WEBSERVER
#if defined(ESP8266)
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif // if defined(esp8266)
#endif // webserver

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
char mySSID[64] = "myfp2eap";
char myPASSWORD[64] = "myfp2eap";
#endif

// ----------------------------------------------------------------------------------------------
// 8: OTA (OVER THE AIR UPDATING) SSID AND PASSWORD CONFIGURATION
// ----------------------------------------------------------------------------------------------
// You can change the values for OTANAME and OTAPassword if required
#if defined(OTAUPDATES)
const char *OTAName = "ESP8266";            // the username and password for the OTA service
const char *OTAPassword = "esp8266";
#include "otaupdate.h"                      // order of statements is important
#endif

// ----------------------------------------------------------------------------------------------
// 9: ASCOMREMOTE: DO NOT CHANGE
// ----------------------------------------------------------------------------------------------
#ifdef ASCOMREMOTE
#include "ascomserver.h"
#endif

// ----------------------------------------------------------------------------------------------
// 10: WEBSERVER: DO NOT CHANGE
// ----------------------------------------------------------------------------------------------
#ifdef WEBSERVER
#include "webserver.h"
#endif

// ----------------------------------------------------------------------------------------------
// 11: mDNS NAME: Name must be alphabetic chars only, lowercase
// ----------------------------------------------------------------------------------------------
#ifdef MDNSSERVER
char mDNSNAME[] = "myfp2eap";
int mdns_started;
#include "mdnsserver.h"
#endif

// ----------------------------------------------------------------------------------------------
// 12: DUCKDNS DOMAIN AND TOKEN CONFIGURATION
// ----------------------------------------------------------------------------------------------
// To use DucksDNS, uncomment the next line - can only be used together with STATIONMODE
//#define USEDUCKSDNS 1

// if using DuckDNS you need to set these next two parameters
// cannot use DuckDNS with ACCESSPOINT mode
#if defined(DUCKDNS)
const char* duckdnsdomain = "myfp2erobert.duckdns.org";
const char* duckdnstoken = "0a0379d5-3979-44ae-b1e2-6c371a4fe9bf";
#endif

// DO NOT CHANGE
#if defined(USEDUCKSDNS)
#include <EasyDDNS.h>                       // https://github.com/ayushsharma82/EasyDDNS
#endif

#if defined(ACCESSPOINT)
#if defined(USEDUCKSDNS)
#halt error- you cannot have ACCESSPOINT and STATIONMODE both defined - only one of then
#endif
#endif

// ----------------------------------------------------------------------------------------------
// 13: STATIC IP ADDRESS CONFIGURATION
// ----------------------------------------------------------------------------------------------
// must use static IP if using duckdns or as an Access Point
#ifndef STATICIPON
#define STATICIPON    1
#endif
#ifndef STATICIPOFF
#define STATICIPOFF   0
#endif
//int staticip = STATICIPON;      // IP address specified by controller - must be defined correctly
int staticip = STATICIPOFF;       // IP address is generated by network device and is dynamic and can change

#if defined(STATIONMODE)
// These need to reflect your current network settings - 192.168.x.21 - change x
// This has to be within the range for dynamic ip allocation in the router
// No effect if staticip = STATICIPOFF
IPAddress ip(192, 168, 2, 21);              // station static IP - you can change these values to change the IP
IPAddress dns(192, 168, 2, 1);              // just set it to the same IP as the gateway
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);
#endif

#if defined(ACCESSPOINT)
// By default the Access point should be 192.168.4.1 - DO NOT CHANGE
IPAddress ip(192, 168, 4, 1);               // AP static IP - you can change these values to change the IP
IPAddress dns(192, 168, 4, 1);              // just set it to the same IP as the gateway
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);
#endif

// ----------------------------------------------------------------------------------------------
// 14: FIRMWARE CODE START - INCLUDES AND LIBRARIES
// ----------------------------------------------------------------------------------------------
// Compile this with Arduino IDE 1.8.9 with ESP8266 Core library installed v2.5.2 [for ESP8266]
// Make sure target board is set to Node MCU 1.0 (ESP12-E Module) [for ESP8266]

// Project specific includes
#if defined(BLUETOOTHMODE) || defined(LOCALSERIAL)
#include "ESPQueue.h"                       //  By Steven de Salas
#endif

#include <Wire.h>                           // needed for I2C => OLED display

#if defined(TEMPERATUREPROBE)
#include <OneWire.h>                        // https://github.com/PaulStoffregen/OneWire
#include <DallasTemperature.h>              // https://github.com/milesburton/Arduino-Temperature-Control-Library
#include "temperature.h"
#endif

#if defined(OLEDGRAPHICS)
#include <SSD1306Wire.h>
#include "images.h"
SSD1306Wire *myoled;
#endif
#if defined(OLEDTEXT)
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 *myoled;
#endif

#if (DRVBRD == PRO2ESP32DRV8825 || DRVBRD == PRO2ESP32ULN2003   || DRVBRD == PRO2ESP32L298N \
  || DRVBRD == PRO2ESP32L293DMINI || DRVBRD == PRO2ESP32L9110S)
#ifdef INFRAREDREMOTE
#include "myIRremotemappings.h"
#include <IRremoteESP32.h>
#endif
#endif

// ----------------------------------------------------------------------------------------------
// 15: BLUETOOTH MODE - Do not change
// ----------------------------------------------------------------------------------------------
#if defined(BLUETOOTHMODE)
#include "BluetoothSerial.h"                // needed for Bluetooth comms
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

String btline;                              // buffer for serial data
String BLUETOOTHNAME = "MYFP3ESP32BT";      // default name for Bluetooth controller, this name you can change
BluetoothSerial SerialBT;                   // define BT adapter to use
#endif // BLUETOOTHMODE

// ----------------------------------------------------------------------------------------------
// 16: CONTROLLER FEATURES -- DO NOT CHANGE
// ----------------------------------------------------------------------------------------------

unsigned long Features = 0L;

void setFeatures()
{
#ifdef LCDDISPLAY
  Features = Features + ENABLEDLCD;
#endif
#ifdef OLEDDISPLAY
  Features = Features + ENABLEDOLED;
#endif
#ifdef TEMPERATUREPROBE
  Features = Features + ENABLEDTEMPPROBE;
#endif
#ifdef HOMEPOSITIONSWITCH
  Features = Features + ENABLEDHPSW;
#endif
#ifdef BLUETOOTHMODE
  Features = Features + ENABLEDBLUETOOTH;
#endif
#ifdef STEPPERPWRDETECT
  Features = Features + ENABLEDSTEPPERPWR;
#endif
#ifdef PUSHBUTTONS
  Features = Features + ENABLEDPUSHBUTTONS;
#endif
#ifdef ROTARYENCODER
  Features = Features + ENABLEDROTARYENCODER;
#endif
#ifdef INFRARED
  Features = Features + ENABLEDINFRARED;
#endif
#ifdef BACKLASH
  Features = Features + ENABLEDBACKLASH;
#endif
#ifdef TFTDISPLAY
  Features = Features + ENABLEDTFT;
#endif
#ifdef NOKIADISPLAY
  Features = Features + ENABLENOKIA;
#endif
#if (DRVBRD == DRV8825HW203KEYPAD)
  Features = Features + ENABLEKEYPAD;
#endif
#ifdef INOUTLEDS
  Features = Features + ENABLEDINOUTLEDS;
#endif
#ifdef BUZZER
  Features = Features + ENABLEDBUZZER;
#endif
#ifdef ACCESSPOINT
  Features = Features + ENABLEDACCESSPOINT;
#endif
#ifdef STATIONMODE
  Features = Features + ENABLEDSTATIONMODE;
#endif
#ifdef LOCALSERIAL
  Features = Features + ENABLEDLOCALSERIAL;
#endif
#ifdef OTAUPDATES
  Features = Features + ENABLEDOTAUPDATES;
#endif
#ifdef WEBSERVER
  Features = Features + ENABLEDWEBSERVER;
#endif
#ifdef ASCOMREMOTE
  Features = Features + ENABLEDASCOMREMOTE;
#endif
#ifdef STATICIP
  Features = Features + ENABLEDSTATICIP;
#endif
#ifdef MDNSSERVER
  Features = Features + ENABLEDMDNS;
#endif
#ifdef JOYSTICK
  Features = Features + ENABLEDJOYSTICK;
#endif
}

// ----------------------------------------------------------------------------------------------
// 17: GLOBAL DATA -- DO NOT CHANGE
// ----------------------------------------------------------------------------------------------

//  StateMachine definition
enum  StateMachineStates {  State_Idle, State_ApplyBacklash, State_ApplyBacklash2, State_Moving, \
                            State_FindHomePosition, State_SetHomePosition, State_DelayAfterMove, \
                            State_FinishedMove
                         };

#define move_in               0
#define move_out              1
#define move_main             move_in      //__needed for Backlash 2 
#define oled_off              0
#define oled_on               1
#define oled_stay             2

//           reversedirection
//__________________________________
//               0   |   1
//__________________________________
//move_out  1||  1   |   0
//move_in   0||  0   |   1

String programName;                                     // will become driverboard name
DriverBoard* driverboard;

char programVersion[] = "108";
char ProgramAuthor[]  = "(c) R BROWN 2019";

unsigned long fcurrentPosition;                         // current focuser position
unsigned long ftargetPosition;                          // target position
unsigned long tmppos;

byte tprobe1;                                           // indicate if there is a probe attached to myFocuserPro2
byte isMoving;                                          // is the motor currently moving
String ipStr;                                           // shared between BT mode and other modes
boolean displayfound;

#if defined(BLUETOOTHMODE)
Queue queue(QUEUELENGTH);                               // receive serial queue of commands
String line;                                            // buffer for serial data
#endif

#if defined(ACCESSPOINT) || defined(STATIONMODE) || defined(LOCALSERIAL) || defined(BLUETOOTH)
IPAddress ESP32IPAddress;
String ServerLocalIP;
WiFiServer myserver(SERVERPORT);
WiFiClient myclient;                                    // only one client supported, multiple connections denied
IPAddress myIP;
long rssi;
#endif

#if defined(INFRAREDREMOTE)
IRrecv irrecv(IRPIN);
decode_results results;
#endif

#ifdef HOMEPOSITIONSWITCH
volatile int hpswstate;
#endif

int packetsreceived;
int packetssent;

SetupData *mySetupData;

// ----------------------------------------------------------------------------------------------
// 18: CODE START - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#include "comms.h"
#if defined(OLEDTEXT) || defined(OLEDGRAPHICS)
#include "displays.h"
#endif
#ifdef INOUTPUSHBUTTONS
#include "pushbuttons.h"
#endif
#ifdef INFRAREDREMOTE
#include "infraredremote.h"
#endif
#ifdef JOYSTICK
#include "joystick.h"
#endif

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
  oledtextmsg("Controller reboot", -1, true, false);
#if defined(OLEDGRAPHICS)
  myoled->clear();
  myoled->setTextAlignment(TEXT_ALIGN_CENTER);
  myoled->setFont(ArialMT_Plain_24);
  myoled->drawString(64, 28, "REBOOT");                 // Print currentPosition
  myoled->display();
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

// STEPPER MOTOR ROUTINES
void steppermotormove(byte dir )                        // direction move_in, move_out ^ reverse direction
{
#if defined(INOUTLEDPINS)
  ( dir == move_in ) ? digitalWrite(INLEDPIN, 1) : digitalWrite(OUTLEDPIN, 1);
#endif
  driverboard->movemotor(dir);
#if defined(INOUTLEDPINS)
  ( dir == move_in ) ? digitalWrite(INLEDPIN, 0) : digitalWrite(OUTLEDPIN, 0);
#endif
}

#ifdef HOMEPOSITIONSWITCH
void IRAM_ATTR hpsw_isr()
{
  // internal pullup = 1 when switch is OPEN, hpswstate of 0
  hpswstate = !(digitalRead(HPSWPIN));
  // when switch is closed, level is 0 so hpswstate is 1
}
#endif

bool Read_WIFI_config_SPIFFS( char* xSSID, char* xPASSWORD)
{
  const String filename = "/wifi.json";
  String SSID;
  String PASSWORD;
  boolean status = false;

  DebugPrintln(F("check for Wifi setup data on SPIFFS"));
  File f = SPIFFS.open(filename, "r");                          // file open to read
  if (!f)
  {
    DebugPrintln(F("no SPIFFS Wifi Setupdata found => use default settings"));
  }
  else
  {
    String data = f.readString();                               // read content of the text file
    DebugPrint(F("SPIFFS Wifi Setupdata: "));
    DebugPrintln(data);                                         // ... and print on serial

    DynamicJsonDocument doc( (const size_t) (JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(2) + 120));  // allocate json buffer
    DeserializationError error = deserializeJson(doc, data);    // Parse JSON object
    if (error)
    {
      DebugPrintln("Deserialization failed! => use default settings");
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

void setup()
{
#if defined(DEBUG) || defined(LOCALSEARIAL)
  Serial.begin(SERIALPORTSPEED);
  DebugPrintln(serialstartstr);
#if defined(DEBUG)
  DebugPrintln(debugonstr);
#endif
#endif

  mySetupData = new SetupData();                        // instantiate object SetUpData with SPIFFS

#if defined(LOCALSERIAL)
  serialline = "";
  clearSerialPort();
#endif // if defined(LOCALSERIAL)

#if defined(BLUETOOTHMODE)                              // open Bluetooth port and set bluetooth device name if defined
  SerialBT.begin(BLUETOOTHNAME);                        // Bluetooth device name
  btline = "";
  clearbtPort();
  DebugPrintln(bluetoothstartstr);
#endif

#if defined(INOUTLEDPINS)                                  // Setup IN and OUT LEDS, use as controller power up indicator
  pinMode(INLEDPIN, OUTPUT);
  pinMode(OUTLEDPIN, OUTPUT);
  digitalWrite(INLEDPIN, 1);
  digitalWrite(OUTLEDPIN, 1);
#endif

#if defined(INOUTPUSHBUTTONS)                           // Setup IN and OUT Pushbuttons, active high when pressed
  pinMode(INPBPIN, INPUT);
  pinMode(OUTPBPIN, INPUT);
#endif

#ifdef JOYSTICK
  // no need to initialise pins as input as they will be treated as analog
  // JOYINOUTPIN and JOYSPEEDPIN
#endif

displayfound = false;
#if defined(OLEDGRAPHICS) || defined(OLEDTEXT)
  displayfound = Init_OLED();
#endif

  delay(100);                                           // keep delays small otherwise issue with ASCOM

  DebugPrint(F(" fposition : "));                       // Print Loaded Values from SPIFF
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

#if defined(TEMPERATUREPROBE)                           // start temp probe
  inittemp();
#endif // end TEMPERATUREPROBE

  //_____Start WIFI config________________________

#if defined(ACCESSPOINT) || defined(STATIONMODE)
  Read_WIFI_config_SPIFFS(mySSID, myPASSWORD); //__ Read mySSID,myPASSWORD from SPIFFS if exist, otherwise use defaults
#endif

#if defined(ACCESSPOINT)
  oledtextmsg(startapstr, -1, true, true);
  DebugPrintln(startapstr);
  WiFi.config(ip, dns, gateway, subnet);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(mySSID, myPASSWORD);
#endif // end ACCESSPOINT

#if defined(STATIONMODE)
  // this is setup as a station connecting to an existing wifi network
  DebugPrintln(startsmstr);
  oledtextmsg(startsmstr, -1, false, true);
  if (staticip == STATICIPON)                           // if staticip then set this up before starting
  {
    DebugPrintln(setstaticipstr);
    oledtextmsg(setstaticipstr, -1, false, true);
    WiFi.config(ip, dns, gateway, subnet);
    delay(5);
  }

  /* Log NodeMCU on to LAN. Provide IP Address over Serial port */

  WiFi.mode(WIFI_STA);
  WiFi.begin(mySSID, myPASSWORD);     // attempt to start the WiFi
  delay(1000);                                      // wait 500ms

  for (int attempts = 0; WiFi.status() != WL_CONNECTED; attempts++)
  {
    DebugPrint(attemptconnstr);
    DebugPrintln(mySSID);
    DebugPrint(attemptsstr);
    DebugPrint(attempts);
    delay(1000);            // wait 1s

    oled_draw_Wifi(attempts);
    oledtextmsg(attemptsstr, attempts, false, true);

#if defined(ESP32)
    if (attempts % 3 == 2)
    { // every 3 attempts new init for ESP32 => faster connection without reboot
      WiFi.mode(WIFI_STA);
      WiFi.begin(mySSID, myPASSWORD);
    }
#endif

    if (attempts > 9)                              // if this attempt is 10 or more tries
    {
      DebugPrintln(wifistartfailstr);
      DebugPrintln(wifirestartstr);
      oledtextmsg(didnotconnectstr, -1, true, false);
      oledtextmsg(mySSID, -1, false, true);
      oledgraphicmsg("Did not connect to AP", -1, true);
      delay(2000);
      software_Reboot(2000);                          // GPIO0 must be HIGH and GPIO15 LOW when calling ESP.restart();
    }
  }
#endif // end STATIONMODE

  oledtextmsg("Connected", -1, true, true);
  delay(100);                                           // keep delays small else issue with ASCOM

#if defined(ACCESSPOINT) || defined(STATIONMODE)
  // Starting TCP Server
  DebugPrintln(starttcpserverstr);
  oledtextmsg(starttcpserverstr, -1, false, true);

  myserver.begin();
  DebugPrintln(F("Get local IP address"));
  ESP32IPAddress = WiFi.localIP();
  delay(100);                                           // keep delays small else issue with ASCOM
  DebugPrintln(tcpserverstartedstr);
  oledtextmsg(tcpserverstartedstr, -1, false, true);

  // set packet counts to 0
  packetsreceived = 0;
  packetssent = 0;

  // connection established
  DebugPrint(ssidstr);
  DebugPrintln(mySSID);
  DebugPrint(ipaddressstr);
  DebugPrintln(WiFi.localIP());
  DebugPrint(tcpserverportstr);
  DebugPrintln(SERVERPORT);
  DebugPrintln(F(serverreadystr));
  myIP = WiFi.localIP();
  ipStr = String(myIP[0]) + "." + String(myIP[1]) + "." + String(myIP[2]) + "." + String(myIP[3]);
#else
  // it is Bluetooth or Local Serial so set some globals
  ipStr = "0.0.0.0";
#endif // if defined(ACCESSPOINT) || defined(STATIONMODE)

  // assign to current working values
  ftargetPosition = fcurrentPosition = mySetupData->get_fposition();

  DebugPrint(setupdrvbrdstr);
  DebugPrintln(DRVBRD);
  oledtextmsg(setupdrvbrdstr, DRVBRD, true, true);

  driverboard = new DriverBoard(DRVBRD);
  // setup firmware filename
  programName = programName + driverboard->getboardname();
  DebugPrintln(drvbrddonestr);
  oledtextmsg(drvbrddonestr, -1, false, true);
  delay(5);

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

  driverboard->setmotorspeed(mySetupData->get_motorSpeed());
  driverboard->setstepmode(mySetupData->get_stepmode());

  DebugPrintln(F("Check coilpower."));
  if (mySetupData->get_coilpower() == 0)
  {
    driverboard->releasemotor();
    DebugPrintln(F("Coil power released."));
  }

  delay(5);

#if defined(USEDUCKSDNS)
  DebugPrintln(setupduckdnsstr);
  oledtextmsg(setupduckdnsstr, -1, false, true);
  EasyDDNS.service("duckdns");                          // Enter your DDNS Service Name - "duckdns" / "noip"
  delay(5);
  EasyDDNS.client(duckdnsdomain, duckdnstoken);         // Enter ddns Domain & Token | Example - "esp.duckdns.org","1234567"
  delay(5);
  EasyDDNS.update(DUCKDNSREFRESHRATE);                               // Check for New Ip Every 60 Seconds.
  delay(5);
#endif // useduckdns

  // setup home position switch
#ifdef HOMEPOSITIONSWITCH
  pinMode(HPSWPIN, INPUT_PULLUP);
  // setup interrupt, falling, when switch is closed, pin goes low
  attachInterrupt(HPSWPIN, hpsw_isr, CHANGE);
  hpswstate = 0;
#endif

#if defined(INFRAREDREMOTE)
  irrecv.enableIRIn();                                  // Start the IR
#endif

  isMoving = 0;

#ifdef TEMPERATUREPROBE
  readtemp(1);
#endif

#ifdef OTAUPDATES
  start_otaservice();                        // Start the OTA service
#endif // if defined(OTAUPDATES)

#ifdef WEBSERVER
  start_webserver();
#endif

#ifdef ASCOMREMOTE
  start_ascomremoteserver();
#endif

#ifdef MDNSSERVER
  mdns_started = start_mdns_service();
#endif

  DebugPrint(currentposstr);
  DebugPrintln(fcurrentPosition);
  DebugPrint(targetposstr);
  DebugPrintln(ftargetPosition);
  DebugPrintln(setupendstr);
  oledtextmsg(setupendstr, -1, false, true);

#if defined(INOUTLEDPINS)
  digitalWrite(INLEDPIN, 0);
  digitalWrite(OUTLEDPIN, 0);
#endif
}

//_____________________ loop()___________________________________________

//void IRAM_ATTR loop() // ESP32
void loop()
{
  static StateMachineStates MainStateMachine = State_Idle;
  static byte DirOfTravel = mySetupData->get_focuserdirection();
  static byte backlash_count = 0;
  static byte m_bl;
  static byte backlash_enabled = 0;

  static unsigned long TimeStampDelayAfterMove = 0;
  static unsigned long TimeStampPark = millis();
  static byte Parked = false;
  static byte updatecount = 0;

#if defined(ACCESSPOINT) || defined(STATIONMODE)
  //#if !defined(WEBSERVER) && !defined(ASCOMREMOTE)
  static byte ConnectionStatus = 0;
#endif

#ifdef HOMEPOSITIONSWITCH
  static byte stepstaken       = 0;
#endif

#if defined(LOOPTIMETEST)
  DebugPrint(F("Loop Start ="));
  DebugPrintln(millis());
#endif

#if defined(ACCESSPOINT) || defined(STATIONMODE)
  if (ConnectionStatus < 2)
  {
    myclient = myserver.available();
    if (myclient)
    {
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
    }
  }
  else
  {
    // is data available from the client request
    if (myclient.connected())
    {
      if (myclient.available())
      {
        ESP_Communication(ESPDATA);                     // Wifi communication
      }
    }
    else
    {
      ConnectionStatus = 1;
    }
  }
#endif // #if defined(ACCESSPOINT) || defined(STATIONMODE)

#ifdef BLUETOOTHMODE
  if ( SerialBT.available() )
  {
    processbt();
  }
  // if there is a command from Bluetooth
  if ( queue.count() >= 1 )                             // check for serial command
  {
    ESP_Communication(BTDATA);
  }
#endif // ifdef Bluetoothmode

#ifdef LOCALSERIAL
  // if there is a command from Serial port
  if ( Serial.available() )
  {
    processserial();
  }
  if ( queue.count() >= 1 )                             // check for serial command
  {
    ESP_Communication(SERIALDATA);
  }
#endif // ifdef LOCALSERIAL

#ifdef OTAUPDATES
  ArduinoOTA.handle();                                  // listen for OTA events
#endif // ifdef OTAUPDATES

#ifdef ASCOMREMOTE
  ascomserver.handleClient();
#endif

#ifdef WEBSERVER
  webserver.handleClient();
#endif

#ifdef MDNSSERVER
  if (mdns_started == 1 )
  {
    mdnswebserver.handleClient();
  }
#endif

  //_____________________________MainMachine _____________________________

  switch (MainStateMachine)
  {
    //___Idle________________________________________________________________
    case State_Idle:
      if (fcurrentPosition != ftargetPosition)
      {
        //__init move _____________________________
        isMoving = 1;
        DebugPrint(F(STATEINITMOVE));
        DebugPrint(currentposstr);
        DebugPrintln(fcurrentPosition);
        DebugPrint(targetposstr);
        DebugPrintln(ftargetPosition);

        DirOfTravel = (ftargetPosition > fcurrentPosition) ? move_out : move_in;
        driverboard->enablemotor();
        if (mySetupData->get_focuserdirection() == DirOfTravel)
        {
          // move is in same direction, ignore backlash
          MainStateMachine = State_Moving;
          DebugPrintln(STATEMOVINGSTR);
        }
        else
        {
          // move is in opposite direction, check for backlash enabled
          // get backlash settings
          mySetupData->set_focuserdirection(DirOfTravel);
#if (BACKLASH == 1)   // go for standard backlash
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

          // if backlask was defined then follow the backlash rules
          // if backlash has been enabled then apply it
          if ( backlash_enabled == 1 )
          {
            // apply backlash
            // save new direction of travel
            // mySetupData->set_focuserdirection(DirOfTravel);
            //driverboard->setmotorspeed(BACKLASHSPEED);
            MainStateMachine = State_ApplyBacklash;
            DebugPrint(STATEAPPLYBACKLASH);
          }
          else
          {
            // do not apply backlash, go straight to moving
            MainStateMachine = State_Moving;
            DebugPrint(STATEMOVINGSTR);
          }

#elif (BACKLASH == 2)

          if ( DirOfTravel == move_main)
          {
            // do not apply backlash, go straight to moving
            MainStateMachine = State_Moving;
            DebugPrint(STATEMOVINGSTR);
          }
          else
          {
            unsigned long bl = mySetupData->get_backlashsteps_in();
            unsigned long sm = mySetupData->get_stepmode();

            if (DirOfTravel == move_out)
            {
              backlash_count = bl + sm - ((ftargetPosition + bl) % sm);   // Trip to tuning point should be a fullstep position
            }
            else
            {
              backlash_count = bl + sm - ((ftargetPosition - bl) % sm);   // Trip to tuning point should be a fullstep position
            }
            m_bl =  backlash_count;
            MainStateMachine = State_ApplyBacklash;
            DebugPrint(STATEAPPLYBACKLASH);
          }
#else
          // ignore backlash
          MainStateMachine = State_Moving;
          DebugPrint(STATEMOVINGSTR);;
#endif
        }
      }
      else
      {
        // focuser stationary. isMoving is 0
#ifdef INOUTPUSHBUTTONS
        update_pushbuttons();
#endif
#ifdef JOYSTICK
        update_joystick();
#endif
#ifdef INFRAREDREMOTE
        update_irremote();
#endif
        if (mySetupData->get_displayenabled() == 1)
        {
          Update_OledGraphics(oled_stay);
          Update_OledText();
        }
        else
        {
          Update_OledGraphics(oled_off);
        }

#ifdef TEMPERATUREPROBE
        Update_Temp();
#endif

        if (Parked == false)
        {
          if (TimeCheck(TimeStampPark, MotorReleaseDelay))   //Power off after MotorReleaseDelay
          {
            driverboard->releasemotor();
            DebugPrintln(F("Idle: release motor"));
            Parked = true;
          }
        }

        byte status = mySetupData->SaveConfiguration(fcurrentPosition, DirOfTravel); // save config if needed
        if ( status == true )
        {
          Update_OledGraphics(oled_off);                // Display off after config saved
          DebugPrint("new Config saved: ");
          DebugPrintln(status);
        }
      }
      break;

    //__ApplyBacklash ____________________________________________________________________
    case State_ApplyBacklash:
      if ( backlash_count )
      {
        steppermotormove(DirOfTravel);
        backlash_count--;
      }
      else
      {
        driverboard->setmotorspeed(mySetupData->get_motorSpeed());
        MainStateMachine = State_Moving;
        DebugPrint(STATEMOVINGSTR);
      }
      break;

    //__ Moving _________________________________________________________________________
    case State_Moving:
      if ( fcurrentPosition != ftargetPosition )        // must come first else cannot halt
      {
        (DirOfTravel == move_out ) ? fcurrentPosition++ : fcurrentPosition--;
        steppermotormove(DirOfTravel);

#ifdef HOMEPOSITIONSWITCH
        // if switch state = CLOSED and currentPosition != 0
        // need to back OUT a little till switch opens and then set position to 0
        if ( (hpswstate == HPSWCLOSED) && (fcurrentPosition != 0) )
        {
          isMoving = 1;
          fcurrentPosition = ftargetPosition = 0;
          MainStateMachine = State_SetHomePosition;
          DebugPrintln(F("HP closed, fcurrentPosition !=0"));
          DebugPrintln(F(STATESETHOMEPOSITION));
        }
        // else if switch state = CLOSED and Position = 0
        // need to back OUT a little till switch opens and then set position to 0
        else if ( (hpswstate == HPSWCLOSED) && (fcurrentPosition == 0) )
        {
          isMoving = 1;
          fcurrentPosition = ftargetPosition = 0;
          MainStateMachine = State_SetHomePosition;
          DebugPrintln(F("HP closed, fcurrentPosition=0"));
          DebugPrintln(F(STATESETHOMEPOSITION));
        }
        // else if switchstate = OPEN and Position = 0
        // need to move IN a little till switch CLOSES then
        else if ( (hpswstate == HPSWOPEN) && (fcurrentPosition == 0))
        {
          isMoving = 1;
          fcurrentPosition = ftargetPosition = 0;
          MainStateMachine = State_FindHomePosition;
          DebugPrintln(F("HP Open, fcurrentPosition=0"));
          DebugPrintln(F(STATEFINDHOMEPOSITION));
        }
#endif // HOMEPOSITIONSWITCH

        if ( mySetupData->get_displayenabled() == 1)
        {
          updatecount++;
          if ( updatecount > LCDUPDATEONMOVE )
          {
            updatecount++;
            if ( updatecount > LCDUPDATEONMOVE )
            {
              updatecount = 0;
              UpdatePositionOledText();
            }
          }
        }
      }
      else
      {
#if (BACKLASH == 2)
        if ( DirOfTravel != move_main)
        {
          DirOfTravel ^= 1;
          mySetupData->set_focuserdirection(DirOfTravel);
          backlash_count = m_bl;        // Backlash retour
          TimeStampDelayAfterMove = millis();
          MainStateMachine = State_ApplyBacklash2;
          DebugPrintln(F("State_Moving => State_ApplyBacklash2"));
        }
        else
        {
          MainStateMachine = State_DelayAfterMove;
          DebugPrintln(F("State_Moving => State_DelayAfterMove"));
        }
#else
        MainStateMachine = State_DelayAfterMove;
        DebugPrintln(F("State_Moving => State_DelayAfterMove"));
#endif
      }
      break;

    //__ FindHomePosition _________________________________________________________________________
    case State_FindHomePosition:                // move in till home position switch closes
#ifdef HOMEPOSITIONSWITCH
      driverboard->setmotorspeed(SLOW);
      stepstaken = 0;
      DebugPrintln(F("HP MoveIN till closed"));
      while ( hpswstate == HPSWOPEN )
      {
        // step IN till switch closes
        steppermotormove(DirOfTravel);
        stepstaken++;
        if ( stepstaken > HOMESTEPS )           // this prevents the endless loop if the hpsw is not connected or is faulty
        {
          DebugPrint(F("HP MoveIN ERROR: HOMESTEPS exceeded"));
          break;
        }
      }
      DebugPrint(F("HP MoveIN stepstaken="));
      DebugPrint(stepstaken);
      DebugPrint(F("HP MoveIN finished"));
      driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif  // HOMEPOSITIONSWITCH
      MainStateMachine = State_SetHomePosition;
      DebugPrint(F(STATESETHOMEPOSITION));
      break;

    //__ SetHomePosition _________________________________________________________________________
    case State_SetHomePosition:                 // move out till home position switch opens
#ifdef HOMEPOSITIONSWITCH
      driverboard->setmotorspeed(SLOW);
      stepstaken = 0;
      DebugPrintln(F("HP Move out till OPEN"));
      // if the previous moveIN failed at HOMESTEPS and HPSWITCH is still open then the
      // following while() code will drop through and have no effect and position = 0
      while ( hpswstate == HPSWCLOSED )
      {
        // step out till switch opens
        DirOfTravel = !DirOfTravel;
        steppermotormove(DirOfTravel);
        stepstaken++;
        if ( stepstaken > HOMESTEPS )           // this prevents the endless loop if the hpsw is not connected or is faulty
        {
          DebugPrintln(F("HP MoveOUT ERROR: HOMESTEPS exceeded#"));
          break;
        }
      }
      DebugPrint(F("HP MoveOUT stepstaken="));
      DebugPrintln(stepstaken);
      driverboard->setmotorspeed(mySetupData->get_motorSpeed());
      DebugPrintln(F("HP MoveOUT ended"));
#endif  // HOMEPOSITIONSWITCH
      MainStateMachine = State_DelayAfterMove;
      DebugPrintln(F(STATEDELAYAFTERMOVE));
      break;

    //__ ApplyBacklash2 ___________________________________________________________________
    case State_ApplyBacklash2:
      if (TimeCheck(TimeStampDelayAfterMove , 250))
      {
        if (backlash_count)
        {
          driverboard->movemotor(DirOfTravel);
          backlash_count--;
        }
        else
        {
          TimeStampDelayAfterMove = millis();
          MainStateMachine = State_DelayAfterMove;
          DebugPrintln(F(STATEDELAYAFTERMOVE));
        }
      }
      break;

    //__ DelayAfterMove _________________________________________________________________________
    case State_DelayAfterMove:
      // apply Delayaftermove, this MUST be done here in order to get accurate timing for DelayAfterMove
      if (TimeCheck(TimeStampDelayAfterMove , mySetupData->get_DelayAfterMove()))
      {
        //__FinishedMove
        Update_OledGraphics(oled_on);                   // display on after move
        TimeStampPark  = millis();                      // catch current time
        Parked = false;                                 // mark to park the motor in State_Idle
        isMoving = 0;
        MainStateMachine = State_Idle;
        DebugPrintln(F("State_DelayAfterMove => State_Idle"));
      }
      break;
      
    //__ Default _________________________________________________________________________
    default:
      DebugPrintln(F("Error: wrong State => State_Idle"));
      MainStateMachine = State_Idle;
      break;
  }

#if defined(LOOPTIMETEST)
  DebugPrint(F("- Loop End ="));
  DebugPrintln(millis());
#endif
} // end Loop()
