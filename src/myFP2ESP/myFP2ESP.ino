// myFP2ESP - Firmware for ESP8266 and ESP32 myFocuserPro2 Controllers
// Supports driver boards DRV8825, ULN2003, L298N, L9110S, L293DMINI
// ESP8266 Supports OLED display, Temperature Probe
// ESP32 Supports OLED display, Temperature Probe, Push Buttons, Direction LED's. Infrared Remote
// Remember to change your target CPU depending on board selection
//
// (c) Copyright Robert Brown 2014-2019. All Rights Reserved.
// (c) Copyright Holger M, 2019, who wrote large portions of code for state machine and esp comms
// (c) Copyright Pieter P - OTA code based on example from Pieter  P.
//
// CONTRIBUTIONS
// If you wish to make a contribution in thanks for this project, please use PayPal and send the amount
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
#define TEMPERATUREPROBE 1

// To enable the OLED DISPLAY uncomment one of the next lines, deselect OLED display by uncomment both lines
#define OLEDTEXT 1
//#define OLEDGRAPHICS 2

// To enable backlash in this firmware, uncomment the next line
//#define BACKLASH 1
//#define BACKLASH 2    // alternative BACKLASH

// To enable In and Out Pushbuttons in this firmware, uncomment the next line [ESP32 only]
//#define INOUTPUSHBUTTONS 1

// To enable In and Out LEDS in this firmware, uncomment the next line [ESP32 only]
//#define INOUTLEDPINS 1

// To enable the Infrared remote controller, uncomment the next line [ESP32 only]
//#define INFRAREDREMOTE

// To enable the start boot screen showing startup messages, uncomment the next line
#define SHOWSTARTSCRN 1

// To display Spash screen graphic, uncomment the next line
#define SPLASHSCREEN 1

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
#endif // 

#if defined(OLEDGRAPHICS)
#if defined(OLEDTEXT)
#halt //ERROR - you must have either OLEDGRAPHICS or OLEDTEXT defined, NOT BOTH
#endif
#endif

// ----------------------------------------------------------------------------------------------
// 5: SPECIFY THE CONTROLLER MODE HERE - ONLY ONE OF THESE MUST BE DEFINED
// ----------------------------------------------------------------------------------------------

// to enable Bluetooth mode, uncomment the next line [ESP32 only]
//#define BLUETOOTHMODE 1

// to work as an access point, define accesspoint - cannot use DUCKDNS
#define ACCESSPOINT 2

// to work as a station accessing a AP, define stationmode
//#define STATIONMODE 3

// to work only via USB cable as Serial port, uncomment the next line
//#define LOCALSERIAL 4

// To enable OTA updates, uncomment the next line [can only be used with stationmode]
//#define OTAUPDATES 5

// to enable this focuser for ASCOM ALPACA REMOTE support, uncomment the next line
//#define ASCOMREMOTE 6

// to enable Webserver interface, uncomment the next line [recommed use Internet Explorer or Microsoft Edge Browser]
#define WEBSERVER 7

// DO NOT CHANGE
#if !defined(ACCESSPOINT) && !defined(STATIONMODE) && !defined(BLUETOOTHMODE) && !defined(LOCALSERIAL) && !defined(WEBSERVER) && !defined(ASCOMREMOTE)
#halt //ERROR you must use ACCESSPOINT or STATIONMODE or BLUETOOTHMODE or LOCALSERIAL or WEBSERVER or ASCOMREMOTE
#endif

#if defined(WEBSERVER)
#if !defined(ACCESSPOINT) && !defined(STATIONMODE)
#halt //ERROR you must use ACCESSPOINT or STATIONMODE with WEBSERVER
#endif
#endif

#if defined(ASCOMREMOTE)
#if !defined(ACCESSPOINT) && !defined(STATIONMODE)
#halt //ERROR you must use ACCESSPOINT or STATIONMODE with ASCOMREMOTE
#endif
#endif

#if defined(ASCOMREMOTE)
#if defined(WEBSERVER)
#halt //ERROR you cannot have both ASCOMREMOTE and WEBSERVER enabled at the same time
#endif
#endif

#if defined(ASCOMREMOTE)
#if defined(LOCALSERIAL)
#halt //ERROR you cannot have both ASCOMREMOTE and LOCALSERIAL enabled at the same time
#endif
#endif

#if defined(ASCOMREMOTE)
#if defined(BLUETOOTH)
#halt //ERROR you cannot have both ASCOMREMOTE and BLUETOOTH enabled at the same time
#endif
#endif

#if defined(WEBSERVER)
#if defined(LOCALSERIAL)
#halt //ERROR you cannot have both WEBSERVER and LOCALSERIAL enabled at the same time
#endif
#endif

#if defined(WEBSERVER)
#if defined(BLUETOOTH)
#halt //ERROR you cannot have both WEBSERVER and BLUETOOTH enabled at the same time
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
#define MotorReleaseDelay 120*1000     // motor release power after 120s

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
char mySSID[64] = "myfp2eap";
char myPASSWORD[64] = "myfp2eap";

// ----------------------------------------------------------------------------------------------
// 8: OTA (OVER THE AIR UPDATING) SSID AND PASSWORD CONFIGURATION
// ----------------------------------------------------------------------------------------------
// You can change the values for OTANAME and OTAPassword if required
#if defined(OTAUPDATES)
const char *OTAName = "ESP8266";            // the username and password for the OTA service
const char *OTAPassword = "esp8266";
#endif

// ----------------------------------------------------------------------------------------------
// 9: ASCOMREMOTE: DO NOT CHANGE
// ----------------------------------------------------------------------------------------------
#ifdef ASCOMREMOTE
unsigned int ASCOMClientID;
unsigned int ASCOMClientTransactionID;
unsigned int ASCOMServerTransactionID = 0;
int ASCOMErrorNumber = 0;
String ASCOMErrorMessage = "";
long ASCOMpos = 0L;
byte ASCOMTempCompState = 0;
byte ASCOMConnectedState = 0;
WiFiClient ascomclient;
#if defined(ESP8266)
ESP8266WebServer ascomserver(ALPACAPORT);
#else
WebServer ascomserver(ALPACAPORT);
#endif // if defined(esp8266)
#endif // ifdef ASCOMREMOTE

// ----------------------------------------------------------------------------------------------
// 10: WEBSERVER: DO NOT CHANGE
// ----------------------------------------------------------------------------------------------
#ifdef WEBSERVER
//WiFiClient webserverclient;
String NotFoundPage;
#if defined(ESP8266)
ESP8266WebServer webserver(WEBSERVERPORT);
#else
WebServer webserver(WEBSERVERPORT);
#endif // if defined(esp8266)
#endif

// ----------------------------------------------------------------------------------------------
// 11: DUCKDNS DOMAIN AND TOKEN CONFIGURATION
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
// 12: STATIC IP ADDRESS CONFIGURATION
// ----------------------------------------------------------------------------------------------
// must use static IP if using duckdns or as an Access Point
#define STATICIPON    1
#define STATICIPOFF   0
//int staticip = STATICIPON;                // if set to STATICIPON then the IP address must be defined
int staticip = STATICIPOFF;                 // if set to STATICIPON then the IP address must be defined

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
// 13: FIRMWARE CODE START - INCLUDES AND LIBRARIES
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
// 14: BLUETOOTH MODE - Do not change
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
// 15: CONTROLLER FEATURES -- DO NOT CHANGE
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
}

// ----------------------------------------------------------------------------------------------
// 16: GLOBAL DATA -- DO NOT CHANGE
// ----------------------------------------------------------------------------------------------

//  StateMachine definition
enum  StateMachineStates {State_Idle, State_ApplyBacklash, State_ApplyBacklash2, State_Moving, State_DelayAfterMove, State_FinishedMove};

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

char programVersion[] = "105";
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

#if defined(LOCALSERIAL)
Queue queue(QUEUELENGTH);                               // receive serial queue of commands
String serialline;                                      // buffer for serial data
#endif // localserial

#if defined(TEMPERATUREPROBE)
OneWire oneWirech1(TEMPPIN);                            // setup temperature probe
DallasTemperature sensor1(&oneWirech1);
DeviceAddress tpAddress;                                // holds address of the temperature probe
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

int packetsreceived;
int packetssent;

SetupData *mySetupData;

// ----------------------------------------------------------------------------------------------
// 17: CODE START - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------

#ifdef WEBSERVER
void setNotFoundPage()
{
  NotFoundPage = "<html><head></head>";
  NotFoundPage = NotFoundPage + String(WS_PAGETITLE) + "</head>";
  NotFoundPage = NotFoundPage + "<body>" + String(WS_TITLE);
  NotFoundPage = NotFoundPage + String(WS_COPYRIGHT);
  NotFoundPage = NotFoundPage + "<p>IP Address: " + ipStr + ": Port: " + String(WEBSERVERPORT) + ", Firmware Version=" + String(programVersion) + "</p>";
  NotFoundPage = NotFoundPage + "<p>Driverboard = myFP2ESP." + programName + "</p>";
  NotFoundPage = NotFoundPage + "<p>The requested URL was not found</p>";
  NotFoundPage = NotFoundPage + "</body></html>";
  delay(10);                     // small pause so background tasks can run
}

void WEBSERVER_handleNotFound()
{
  delay(10);                     // small pause so background tasks can run
  webserver.send(NOTFOUNDWEBPAGE, TEXTPAGETYPE, NotFoundPage);
}

// constructs home page of web server
String SetHomePage()
{
  String HomePage;
  
  // convert current values of focuserposition and focusermaxsteps to string types
  String fpbuffer = String(fcurrentPosition);
  String mxbuffer = String(mySetupData->get_maxstep());
  String smbuffer = String(mySetupData->get_stepmode());
  String imbuffer = String(isMoving);

  HomePage = "";

  volatile byte sm = mySetupData->get_stepmode();
  switch ( sm )
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

  String msbuffer = "";
  volatile byte mspd = driverboard->getmotorspeed();
  switch ( mspd )
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

#ifdef TEMPERATUREPROBE
  String tmbuffer = String(readtemp(0));
#else
  String tmbuffer = "20.0";
#endif
  String trbuffer = String(mySetupData->get_tempprecision());

  String stbuffer;    // stepstomove
  stbuffer = "<input type=\"radio\" name=\"mv\" value=\"-500\" > -500 ";
  stbuffer = stbuffer + "<input type=\"radio\" name=\"mv\" value=\"-100\" > -100 ";
  stbuffer = stbuffer + "<input type=\"radio\" name=\"mv\" value=\"-10\" > -10 ";
  stbuffer = stbuffer + "<input type=\"radio\" name=\"mv\" value=\"-1\" > -1 ";
  stbuffer = stbuffer + "<input type=\"radio\" name=\"mv\" value=\"0\" Checked > 0 ";
  stbuffer = stbuffer + "<input type=\"radio\" name=\"mv\" value=\"1\" > 1 ";
  stbuffer = stbuffer + "<input type=\"radio\" name=\"mv\" value=\"10\" > 10 ";
  stbuffer = stbuffer + "<input type=\"radio\" name=\"mv\" value=\"100\" > 100 ";
  stbuffer = stbuffer + "<input type=\"radio\" name=\"mv\" value=\"500\" > 500 ";

  // construct home page of webserver
  // header
  HomePage = "<head><meta http-equiv=\"refresh\" content=\"" + String(WS_REFRESHRATE) + "\"; url=http://" + ipStr +">";
  // TODO Add serverport
  
  HomePage = HomePage + String(WS_PAGETITLE) + "</head><body>";
  HomePage = HomePage + String(WS_TITLE) + String(WS_COPYRIGHT);
  HomePage = HomePage + "<p>Driverboard = myFP2ESP." + programName + "<br>";
  HomePage = HomePage + "<myFP2ESP." + programName + "</h3>IP Address: " + ipStr + ", Firmware Version=" + String(programVersion) + "</br>";

  // position. set position. goto position
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><br><b>Focuser Position</b> <input type=\"text\" name=\"fp\" size =\"15\" value=" + fpbuffer + "> ";
  HomePage = HomePage + "<input type=\"submit\" name=\"setpos\" value=\"Set Pos\"> <input type=\"submit\" name=\"gotopos\" value=\"Goto Pos\">"; \
  HomePage = HomePage + " (Target = " + String(ftargetPosition) + ")</form></p>";

  // maxstep
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><b>MaxSteps</b> <input type=\"text\" name=\"fm\" size =\"15\" value=" + mxbuffer + "> ";
  HomePage = HomePage + "<input type=\"submit\" value=\"Submit\"></form>";

  // move buttons
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><b>Move: </b>" + stbuffer;
  HomePage = HomePage + "<input type=\"hidden\" name=\"mv\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

  // Halt button and isMoving
  HomePage = HomePage + "<form action=\"/\" method=\"post\">";
  HomePage = HomePage + "IsMoving = " + imbuffer + " ";
  HomePage = HomePage + "<input type=\"hidden\" name=\"ha\" value=\"true\"><input type=\"submit\" value=\"HALT\"></form>";

  // temperature and temperature resolution
  HomePage = HomePage + "<p><b>Temperature</b> = " + tmbuffer + "</p>";
  HomePage = HomePage + "<form action=\"/\" method=\"post\">";
  HomePage = HomePage + "<b>Temperature Resolution </b><input type=\"text\" name=\"tr\" size =\"3\" value=" + trbuffer + "> ";
  HomePage = HomePage + "<input type=\"submit\" value=\"Submit\"></form>";

  // coilpower
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><b>Coil Power </b>" + cpbuffer ;
  HomePage = HomePage + "<input type=\"hidden\" name=\"cp\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

  // reverse direction
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><b>Reverse Direction </b>" + rdbuffer ;
  HomePage = HomePage + "<input type=\"hidden\" name=\"rd\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

  // stepmode
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><b>Step Mode </b>" + smbuffer + " ";
  HomePage = HomePage + "<input type=\"hidden\" name=\"sm\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

  // motor speed
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><b>Motor Speed: </b>" + msbuffer + " ";
  HomePage = HomePage + "<input type=\"hidden\" name=\"ms\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

  // add HOME button
  HomePage = HomePage + "<p><form action=\"/\" method=\"GET\"><input type=\"submit\" value=\"REFRESH\"></form></p>";
  HomePage = HomePage + "</body></html>\r\n";
  return HomePage;
}

// handles root page of webserver
// this is called whenever a client requests home page of sebserver
void WEBSERVER_handleRoot()
{
  // if the root page was a HALT request via Submit button

  String halt_str = webserver.arg("ha");
  if ( halt_str != "" )
  {
    DebugPrint( "ws: halt:" );
    DebugPrintln(halt_str);
    ftargetPosition = fcurrentPosition;
  }

  // if set focuser position
  String fpos_str = webserver.arg("setpos");
  if ( fpos_str != "" )
  {
    DebugPrint( "ws: setpos:" );
    DebugPrintln(fpos_str);
    String fp = webserver.arg("fp");
    if ( fp != "" )
    {
      unsigned long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      if ( temp > mySetupData->get_maxstep() )  // if greater than maxStep then set to maxStep
      {
        temp = mySetupData->get_maxstep();
      }
      fcurrentPosition = ftargetPosition = temp;
    }
  }

  // if goto focuser position
  fpos_str = webserver.arg("gotopos");
  if ( fpos_str != "" )
  {
    DebugPrint( "ws: gotopos:" );
    DebugPrintln(fpos_str);
    String fp = webserver.arg("fp");
    if ( fp != "" )
    {
      unsigned long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      if ( temp > mySetupData->get_maxstep() )  // if greater than maxStep then set to maxStep
      {
        temp = mySetupData->get_maxstep();
      }
      ftargetPosition = temp;
    }
  }

  delay(10);                     // small pause so background ESP8266 tasks can run

  // if update of maxsteps
  String fmax_str = webserver.arg("fm");
  if ( fmax_str != "" )
  {
    unsigned long temp = 0;
    DebugPrint( "ws: maxsteps:" );
    DebugPrintln(fmax_str);
    temp = fmax_str.toInt();
    if ( temp < fcurrentPosition )                    // if maxstep is less than focuser position
    {
      temp = fcurrentPosition + 1;
    }
    if ( temp < FOCUSERLOWERLIMIT )                   // do not set it less then 1024
    {
      temp = FOCUSERLOWERLIMIT;
    }
    if ( temp > mySetupData->get_maxstep() )          // if higher than max value
    {
      temp = mySetupData->get_maxstep();
    }
    mySetupData->set_maxstep(temp);
  }

  delay(10);                                          // small pause so background ESP8266 tasks can run

  // if update motorspeed
  String fms_str = webserver.arg("ms");
  if ( fms_str != "" )
  {
    int temp1 = 0;
    DebugPrint( "ws: -motorspeed:" );
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
    driverboard->setmotorspeed(temp1);
  }

  delay(10);                     // small pause so background ESP8266 tasks can run

  // if update coilpower
  String fcp_str = webserver.arg("cp");
  if ( fcp_str != "" )
  {
    DebugPrint( "ws: coil power:" );
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

  delay(10);                     // small pause so background ESP8266 tasks can run

  // if update reversedirection
  String frd_str = webserver.arg("rd");
  if ( frd_str != "" )
  {
    DebugPrint( "ws: reverse direction:" );
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

  delay(10);                     // small pause so background ESP8266 tasks can run

  // if update stepmode
  // (1=Full, 2=Half, 4=1/4, 8=1/8, 16=1/16, 32=1/32, 64=1/64, 128=1/128)
  String fsm_str = webserver.arg("sm");
  if ( fsm_str != "" )
  {
    int temp1 = 0;
    DebugPrint( "ws: stepmode:" );
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

  delay(10);                     // small pause so background ESP8266 tasks can run

  // if update temperature resolution
  String tres_str = webserver.arg("tr");
  if ( tres_str != "" )
  {
    int temp = 0;
    DebugPrint( "ws: temperature resolution:" );
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

  delay(10);                     // small pause so background ESP8266 tasks can run

  // if move
  String fmv_str = webserver.arg("mv");
  if ( fmv_str != "" )
  {
    unsigned long temp = 0;
    DebugPrint("ws: move:" );
    DebugPrintln(fmv_str);
    temp = fmv_str.toInt();
    ftargetPosition = fcurrentPosition + temp;
    DebugPrint("Move = "); DebugPrintln(fmv_str);
    DebugPrint("Current = "); DebugPrint(fcurrentPosition);
    DebugPrint(", Target = "); DebugPrintln(ftargetPosition);
    if ( ftargetPosition > mySetupData->get_maxstep() )
    {
      ftargetPosition = mySetupData->get_maxstep();
    }
  }

  DebugPrintln( "ws: build homepage" );

  delay(10);                     // small pause so background ESP8266 tasks can run

  // construct the homepage now
  String htmlpage = SetHomePage();

  delay(10);                     // small pause so background ESP8266 tasks can run

  // send the homepage to a connected client
  DebugPrintln("ws: send homepage");
  webserver.send(NORMALWEBPAGE, TEXTPAGETYPE, htmlpage );
}
#endif // ifdef WEBSERVER
// WEBSERVER END ------------------------------------------------------------------------------------

// ASCOM REMOTE START -------------------------------------------------------------------------------
// The Alpaca device must return the clienttransactionid number, or zero if no value was supplied by the client
// The server transaction id should be returned by the Alpaca device with every response
// URLs are case sensitive and all elements must be in lower case. This means that both the device type and
// command name must always be in lower case. Parameter names are not case sensitive, so clients and drivers
// should be prepared for parameter names to be supplied and returned with any casing. Parameter values can
// be in mixed case as required.
// For GET operations, parameters should be placed in the URL query string
// For PUT operations they should be placed in the body of the message.
// If the transaction completes successfully, the ErrorMessage field will be an empty string and the
// ErrorNumber field will be zero.

#ifdef ASCOMREMOTE
// generic ASCOM send reply
void ASCOM_sendreply( int replycode, String contenttype, String jsonstr)
{
  DebugPrint("ASCOM_sendreply: replycode:");
  DebugPrint(replycode);
  DebugPrint(" , content-type:");
  DebugPrint(contenttype);
  DebugPrint(", \njson:");
  DebugPrintln(jsonstr);
  // ascomserver.send builds the http header, jsonstr will be in the body?
  ascomserver.send(replycode, contenttype, jsonstr );
}

void ASCOM_getURLParameters()
{
  String str;
  // get server args, translate server args to lowercase, they can be mixed case
  DebugPrintln("ASCOM_getURLParameters START");
  DebugPrint("Number of args:");
  DebugPrintln(ascomserver.args());
  for (int i = 0; i < ascomserver.args(); i++)
  {
    if ( i >= ASCOMMAXIMUMARGS )
    {
      break;
    }
    str = ascomserver.argName(i);
    str.toLowerCase();
    DebugPrint("Paramter Found: ");
    DebugPrintln(str);
    if ( str.equals("clientid") )
    {
      ASCOMClientID = (unsigned int) ascomserver.arg(i).toInt();
      DebugPrint("clientID:");
      DebugPrintln(ASCOMClientID);
    }
    if ( str.equals("clienttransactionid") )
    {
      ASCOMClientTransactionID = (unsigned int) ascomserver.arg(i).toInt();
      DebugPrint("clienttransactionid:");
      DebugPrintln(ASCOMClientTransactionID);
    }
    if ( str.equals("tempcomp") )
    {
      String strtmp = ascomserver.arg(i);
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
      String str = ascomserver.arg(i);
      DebugPrint("ASCOMpos RAW:");
      DebugPrintln(str);
      ASCOMpos = ascomserver.arg(i).toInt();      // this returns a long data type
      DebugPrint("ASCOMpos:");
      DebugPrintln(ASCOMpos);
    }
    if ( str.equals("connected") )
    {
      String strtmp = ascomserver.arg(i);
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
    // todo
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
  //jsonretstr = jsonretstr + "\"ErrorMessage\":\"\"}";
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
  jsonretstr = "{\"Value\":\"myFP2ESPASCOMR\"," + ASCOM_addclientinfo( jsonretstr );
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
  jsonretstr = "{\"Value\":\"ASCOM driver for myFP2ESP controllers\"," + ASCOM_addclientinfo( jsonretstr );
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
  jsonretstr = "{\"Value\":\"myFP2ESP ASCOM Driver (c) R. Brown. 2019\"," + ASCOM_addclientinfo( jsonretstr );
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
  jsonretstr = "{\"Value\":\"104\"," + ASCOM_addclientinfo( jsonretstr );
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
#ifdef TEMPERATUREPROBE
  jsonretstr = "{\"Value\":" + String(readtemp(0)) + "," + ASCOM_addclientinfo( jsonretstr );
#else
  jsonretstr = "{\"Value\":20.0," + ASCOM_addclientinfo( jsonretstr );
#endif
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
  ftargetPosition = fcurrentPosition;
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
#ifdef TEMPERATUREPROBE
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
#else
  ASCOMErrorNumber = ASCOMNOTIMPLEMENTED;
  ASCOMErrorMessage = ASCOMERRORNOTIMPLEMENTED;
  jsonretstr = "{" + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
#endif
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
#ifdef TEMPERATUREPROBE
  jsonretstr = "{\"Value\":1," + ASCOM_addclientinfo( jsonretstr );
#else
  jsonretstr = "{\"Value\":0," + ASCOM_addclientinfo( jsonretstr );
#endif
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void  ASCOM_handlemoveput()
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
  message += ascomserver.uri();
  message += "\nMethod: ";
  if ( ascomserver.method() == HTTP_GET )
  {
    message += "GET";
  }
  else if ( ascomserver.method() == HTTP_POST )
  {
    message += "POST";
  }
  else if ( ascomserver.method() == HTTP_PUT )
  {
    message += "PUT";
  }
  else if ( ascomserver.method() == HTTP_DELETE )
  {
    message += "DELETE";
  }
  else
  {
    message += "UNKNOWN_METHOD: " + ascomserver.method();
  }
  message += "\nArguments: ";
  message += ascomserver.args();
  message += "\n";
  for (uint8_t i = 0; i < ascomserver.args(); i++)
  {
    message += " " + ascomserver.argName(i) + ": " + ascomserver.arg(i) + "\n";
  }
  DebugPrint("Error: ");
  DebugPrintln(message);
  ASCOMErrorNumber  = ASCOMNOTIMPLEMENTED;
  ASCOMErrorMessage = ASCOMERRORNOTIMPLEMENTED;
  ASCOMServerTransactionID++;
  jsonretstr = "{" + ASCOM_addclientinfo( jsonretstr );
  ASCOM_sendreply( BADREQUESTWEBPAGE, JSONPAGETYPE, jsonretstr);
  delay(10);                     // small pause so background tasks can run
}

void ASCOM_handleRoot()
{
  String HomePage;
  HomePage = "<html><head><meta http-equiv=\"refresh\" content=\"10\"></head>";
  HomePage = HomePage + "<title>myFP2ESP ASCOM REMOTE</title></head>";
  HomePage = HomePage + "<body><h3>myFP2ESP ASCOM REMOTE</h3>";
  HomePage = HomePage + "<p>(c) R. Brown, Holger M, 2019. All rights reserved.</p>";
  HomePage = HomePage + "<p>IP Address: " + ipStr + ": Port: " + String(ALPACAPORT) + ", Firmware Version=" + String(programVersion) + "</p>";
  HomePage = HomePage + "<p>Driverboard = myFP2ESP." + driverboard->getboardname() + "</p>";
  HomePage = HomePage + "</body></html>";
  ASCOMServerTransactionID++;
  ASCOM_sendreply( NORMALWEBPAGE, TEXTPAGETYPE, HomePage);
  delay(10);                     // small pause so background tasks can run
}
#endif
// ASCOM REMOTE END ---------------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------------------------
// TEMPERATURE PROBE ROUTINES
// ----------------------------------------------------------------------------------------------
#if defined(TEMPERATUREPROBE)
float readtemp(byte new_measurement)
{
  static float lasttemp = 20.0;                         // start temp value
  if (!new_measurement)
  {
    return lasttemp;                                    // return latest measurement
  }

  float result = sensor1.getTempCByIndex(0);            // get channel 1 temperature, always in celsius
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
    static byte requesttempflag = 0;                    // start with request
    unsigned long tempnow = millis();

    // see if the temperature needs updating - done automatically every 1.5s
    if (TimeCheck(lasttempconversion , TEMPREFRESHRATE))
    {
      static float tempval;
      static float starttemp;                           // start temperature to use when temperature compensation is enabled

      if ( tcchanged != mySetupData->get_tempcompenabled() )
      {
        tcchanged = mySetupData->get_tempcompenabled();
        if ( tcchanged == 1 )
        {
          starttemp = readtemp(1);
        }
      }

      lasttempconversion = tempnow;                     // update time stamp

      if (requesttempflag)
      {
        tempval = readtemp(1);
      }
      else
      {
        sensor1.requestTemperatures();
      }

      requesttempflag ^= 1; // toggle flag

      if (mySetupData->get_tempcompenabled() == 1)      // check for temperature compensation
      {
        if ((abs)(starttemp - tempval) >= 1)            // calculate if temp has moved by more than 1 degree
        {
          unsigned long newPos;
          byte temperaturedirection;                    // did temperature fall (1) or rise (0)?
          temperaturedirection = (tempval < starttemp) ? 1 : 0;
          if (mySetupData->get_tcdirection() == 0)      // check if tc direction for compensation is inwards
          {
            // temperature compensation direction is in, if a fall then move in else move out
            if ( temperaturedirection == 1 )            // check if temperature is falling
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
          starttemp = tempval;                          // save this current temp point for future reference
        } // end of check for tempchange >=1
      } // end of check for tempcomp enabled
    } // end of check for temperature needs updating
  } // end of if tprobe
}
#endif // TEMPERATUREPROBE 

// ----------------------------------------------------------------------------------------------
// Section OLED GRAPHICS
// ----------------------------------------------------------------------------------------------

void Update_OledGraphics(byte new_status)
{
#if defined(OLEDGRAPHICS)
  static byte current_status = oled_on;

  if ( displayfound)
  {
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
#endif
}

#if defined(OLEDGRAPHICS)

void oledgraphicmsg(String str, int val, boolean clrscr)
{
  static byte linecount = 0;

  myoled->setTextAlignment(TEXT_ALIGN_LEFT);
  myoled->setFont(ArialMT_Plain_10);

  if (displayfound)
  {
    if (clrscr == true)
    {
      myoled->clear();
      linecount = 0;
    }
    if (val != -1)
    {
      str += String(val);
    }
    myoled->drawString(0, linecount * 12, str);
    myoled->display();
    linecount++;
  }
}

void oled_draw_Wifi(int j)
{
  if ( displayfound == true)
  {
    myoled->clear();
    myoled->setTextAlignment(TEXT_ALIGN_CENTER);
    myoled->setFont(ArialMT_Plain_10);
    myoled->drawString(64, 0, "SSID: " + String(mySSID));
    myoled->drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits); // draw wifi logo

    for (int i = 1; i < 10; i++)
      myoled->drawXbm(12 * i, 56, 8, 8, (i == j) ? activeSymbol : inactiveSymbol);

    myoled->display();
  }
}

void oled_draw_main_update(void)
{
  if (displayfound == true)
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

    char dir = (mySetupData->get_focuserdirection() == move_in ) ? '<' : '>';
    myoled->drawString(64, 28, String(fcurrentPosition, DEC) + ":" +  String(fcurrentPosition % driverboard->getstepmode(), DEC) + ' ' + dir); // Print currentPosition
    myoled->display();
  }
}

boolean Init_OLED(void)
{
  Wire.begin();
  Wire.beginTransmission(OLED_ADDR);                    //check if OLED display is present
  if (Wire.endTransmission() != 0)
  {
    DebugPrintln(F("no I2C device found"));
    DebugPrint(F("I2C device found at address "));
    DebugPrintln(OLED_ADDR, HEX);
    displayfound = false;
  }
  else
  {
    DebugPrint(F("I2C device found at address "));
    DebugPrintln(OLED_ADDR, HEX);
    displayfound = true;
    myoled = new SSD1306Wire(OLED_ADDR , I2CDATAPIN, I2CCLKPIN);
    myoled->init();
    myoled->flipScreenVertically();
    myoled->setFont(ArialMT_Plain_10);
    myoled->setTextAlignment(TEXT_ALIGN_LEFT);
    myoled->clear();
#if defined(SHOWSTARTSCRN)
    myoled->drawString(0, 0, "myFocuserPro2 v:" + String(programVersion));
    myoled->drawString(0, 12, ProgramAuthor);
#endif
    myoled->display();
  }
  return displayfound;
}

#else
void oledgraphicmsg(String str, int val, boolean clrscr) {}
void oled_draw_Wifi(int j) {}
#endif // if defined(OLEDGRAPHICS)

// ----------------------------------------------------------------------------------------------
// Section OLED TEXT
// ----------------------------------------------------------------------------------------------
void oledtextmsg(String str, int val, boolean clrscr, boolean nl)
{
  if ( displayfound == false)
  {
    return;
  }
#if defined(OLEDTEXT)
  if ( clrscr == true)                                  // clear the screen?
  {
    myoled->clearDisplay();
    myoled->setCursor(0, 0);
  }
  if ( nl == true )                                     // need to print a new line?
  {
    if ( val != -1)                                     // need to print a value?
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
  myoled->display();
#endif // if defined(OLEDTEXT)
}

void Update_OledText(void)
{
  if ( displayfound == false)
  {
    return;
  }
#if defined(OLEDTEXT)
  static unsigned long currentMillis;
  static unsigned long olddisplaytimestampNotMoving = millis();
  static byte displaypage = 0;

  currentMillis = millis();                             // see if the display needs updating
  if (((currentMillis - olddisplaytimestampNotMoving) > ((int)mySetupData->get_lcdpagetime() * 1000)) || (currentMillis < olddisplaytimestampNotMoving))
  {
    olddisplaytimestampNotMoving = currentMillis;       // update the timestamp
    myoled->clearDisplay();                             // clrscr OLED
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
#endif // if defined(OLEDTEXT)
}

void UpdatePositionOledText(void)
{
  if ( displayfound == false)
  {
    return;
  }
#if defined(OLEDTEXT)
  myoled->setCursor(0, 0);
  myoled->print(currentposstr);
  myoled->println(fcurrentPosition);

  myoled->print(targetposstr);
  myoled->println(ftargetPosition);
  myoled->display();
#endif // if defined(OLEDTEXT)
}

void displaylcdpage0(void)      // displaylcd screen
{
  if ( displayfound == false)
  {
    return;
  }
#if defined(OLEDTEXT)
  char tempString[20];
  myoled->setCursor(0, 0);
  myoled->print(currentposstr);
  myoled->println(fcurrentPosition);
  myoled->print(targetposstr);
  myoled->println(ftargetPosition);

  myoled->print(coilpwrstr);
  if ( mySetupData->get_coilpower() == 1 )
  {
    myoled->println(onstr);
  }
  else
  {
    myoled->println(offstr);
  }

  myoled->print(revdirstr);
  if ( mySetupData->get_reversedirection() == 1 )
  {
    myoled->println(onstr);
  }
  else
  {
    myoled->println(offstr);
  }

  // stepmode setting
  myoled->print(stepmodestr);
  myoled->println(mySetupData->get_stepmode());

  //Temperature
  myoled->print(tempstr);
#if defined(TEMPERATUREPROBE)
  myoled->print(String(readtemp(0)));
#else
  myoled->print("20.0");
#endif
  myoled->println(" c");

  //Motor Speed
  myoled->print(motorspeedstr);
  switch ( mySetupData->get_motorSpeed() )
  {
    case SLOW:
      myoled->print(slowstr);
      break;
    case MED:
      myoled->print(medstr);
      break;
    case FAST:
      myoled->print(faststr);
      break;
  }

  //MaxSteps
  myoled->print(maxstepsstr);
  ltoa(mySetupData->get_maxstep(), tempString, 10);
  myoled->println(tempString);
  myoled->display();
#endif // if defined(OLEDTEXT)
}

void displaylcdpage1(void)
{
  if ( displayfound == false)
  {
    return;
  }
#if defined(OLEDTEXT)
  myoled->setCursor(0, 0);
  // temperature compensation
  myoled->print(tcompstepsstr);
  myoled->println(mySetupData->get_tempcoefficient());

  myoled->print(tcompstatestr);
  if ( mySetupData->get_tempcompenabled() == 0 )
  {
    myoled->println(offstr);
  }
  else
  {
    myoled->println(onstr);
  }

  myoled->print(tcompdirstr);
  if ( mySetupData->get_tcdirection() == 0 )
  {
    myoled->println(instr);
  }
  else
  {
    myoled->println(outstr);
  }

  myoled->print(backlashinstr);
  if ( mySetupData->get_backlash_in_enabled() == 0 )
  {
    myoled->println(offstr);
  }
  else
  {
    myoled->println(onstr);
  }

  myoled->print(backlashoutstr);
  if ( mySetupData->get_backlash_out_enabled() == 0 )
  {
    myoled->println(offstr);
  }
  else
  {
    myoled->println(onstr);
  }

  myoled->print(backlashinstepsstr);
  myoled->println(mySetupData->get_backlashsteps_in());

  myoled->print(backlashoutstepsstr);
  myoled->println(mySetupData->get_backlashsteps_out());
  myoled->display();
#endif // if defined(OLEDTEXT)
}

void displaylcdpage2(void)
{
  if ( displayfound == false)
  {
    return;
  }
#if defined(OLEDTEXT)
#if defined(ACCESSPOINT) || defined(STATIONMODE)
  myoled->setCursor(0, 0);
#if defined(ACCESSPOINT)
  myoled->println(accesspointstr);
#endif
#if defined(STATIONMODE)
  myoled->println(stationmodestr);
#endif
  myoled->print(ssidstr);
  myoled->print(mySSID);
  myoled->println();

  myoled->print(ipaddressstr);
  myoled->print(ipStr);
  myoled->println();
#endif // if defined(ACCESSPOINT) || defined(STATIONMODE)

#if defined(WEBSERVER)
  myoled->setCursor(0, 0);
  myoled->println(webserverstr);
#if defined(ACCESSPOINT)
  myoled->println(accesspointstr);
#endif
#if defined(STATIONMODE)
  myoled->println(stationmodestr);
#endif
  myoled->print(ipaddressstr);
  myoled->print(ipStr);
  myoled->print(STARTSTR);
  myoled->println(SERVERPORT);
#endif // webserver
#if defined(ASCOMREMOTE)
  myoled->setCursor(0, 0);
  myoled->println(ascomremotestr);
  myoled->print(ipaddressstr);
  myoled->print(ipStr);
  myoled->print(STARTSTR);
  myoled->println(ALPACAPORT);
#endif

#if defined(BLUETOOTHMODE)
  myoled->setCursor(0, 0);
  myoled->print(bluetoothstr);
  myoled->println();
#endif
#if defined(LOCALSERIAL)
  myoled->setCursor(0, 0);
  myoled->println(localserialstr);
#endif
  myoled->display();
#endif // if defined(OLEDTEXT)
}

#if defined(OLEDTEXT)
boolean Init_OLED(void)
{
#if defined(ESP8266)
#if (DRVBRD == PRO2EL293DNEMA) || (DRVBRD == PRO2EL293D28BYJ48)
  DebugPrintln("Setup esp8266 PRO2EL293D SHIELD I2C");
  Wire.begin(I2CDATAPIN, I2CCLKPIN);              // l293d esp8266 shield
  DebugPrintln("Setup PRO2EL293DNEMA / PRO2EL293D28BYJ48 I2C");
#else
  DebugPrintln("Setup esp8266 I2C");
  Wire.begin();
#endif
#else
  DebugPrintln("Setup esp32 I2C");
  Wire.begin(I2CDATAPIN, I2CCLKPIN);      // esp32
#endif
  Wire.beginTransmission(OLED_ADDR);                    //check if OLED display is present
  if (Wire.endTransmission() != 0)
  {
    DebugPrintln(F("no I2C device found"));
    displayfound = false;
  }
  else
  {
    DebugPrint(F("I2C device found at address "));
    DebugPrintln(OLED_ADDR, HEX);
    displayfound = true;
    myoled = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
    if (!myoled->begin(SSD1306_SWITCHCAPVCC, OLED_ADDR))
    {
      // Address 0x3C for 128x64
      DebugPrintln(F("SSD1306 allocation failed"));
      for (;;)
      {
        ;                                               // Don't proceed, loop forever=
      }
    }
#if defined(SPLASHSCRN)
    myoled->display();                                  // display splash screen
    delay(500);
#endif
    myoled->clearDisplay();
    myoled->setTextSize(1);                             // Normal 1:1 pixel scale
    myoled->setTextColor(WHITE);                        // Draw white text
    myoled->setCursor(0, 0);                            // Start at top-left corner
    myoled->dim(false);
    delay(5);
#if defined(SHOWSTARTSCRN)
    myoled->println(programName);                       // print startup screen
    myoled->println(programVersion);
    myoled->println(programName);                       // print startup screen
    myoled->println(programVersion);;
    myoled->println(ProgramAuthor);
    myoled->display();
#endif // showstartscreen
  }
  return displayfound;
}
#endif // oledtext

// ----------------------------------------------------------------------------------------------
// Section COMMUNICATIONS
// ----------------------------------------------------------------------------------------------
#if defined(ACCESSPOINT) || defined(STATIONMODE) || defined(LOCALSERIAL) || defined(BLUETOOTHMODE)
void SendPaket(String str)
{
  DebugPrint(sendstr);
  DebugPrintln(str);
#if defined(ACCESSPOINT) || defined(STATIONMODE)
  // for Accesspoint or Station mode
  myclient.print(str);
  packetssent++;
#endif // if defined(ACCESSPOINT) || defined(STATIONMODE)
#if defined(BLUETOOTHMODE)  // for bluetooth
  SerialBT.print(str);
#endif
#if defined(LOCALSERIAL)
  Serial.print(str);
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
#if defined(ACCESSPOINT) || defined(STATIONMODE)
      receiveString = myclient.readStringUntil(EOFSTR);    // read until terminator
#endif
      break;
#if defined(BLUETOOTHMODE)
    case BTDATA:
      receiveString = STARTSTR + queue.pop();
      break;
#endif
#if defined(LOCALSERIAL)
    case SERIALDATA:                                    // for Serial
      receiveString = STARTSTR + queue.pop();
      break;
#endif
  }

  receiveString += EOFSTR;                                 // put back terminator
  String cmdstr = receiveString.substring(1, 3);
  cmdval = cmdstr.toInt();                              // convert command to an integer
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
      SendPaket('P' + String(fcurrentPosition) + EOFSTR);
      break;
    case 1: // ismoving
      SendPaket('I' + String(isMoving) + EOFSTR);
      break;
    case 2: // get controller status
      SendPaket("EOK#");
      break;
    case 3: // get firmware version
      SendPaket('F' + String(programVersion) + EOFSTR);
      break;
    case 4: // get firmware name
      SendPaket('F' + programName + '\r' + '\n' + String(programVersion) + EOFSTR);
      break;
    case 6: // get temperature
#if defined(TEMPERATUREPROBE)
      SendPaket('Z' + String(readtemp(0), 3) + EOFSTR);
#else
      SendPaket("Z20.00#");
#endif
      break;
    case 8: // get maxStep
      SendPaket('M' + String(mySetupData->get_maxstep()) + EOFSTR);
      break;
    case 10: // get maxIncrement
      SendPaket('Y' + String(mySetupData->get_maxstep()) + EOFSTR);
      break;
    case 11: // get coilpower
      SendPaket('O' + String(mySetupData->get_coilpower()) + EOFSTR);
      break;
    case 13: // get reverse direction setting, 00 off, 01 on
      SendPaket('R' + String(mySetupData->get_reversedirection()) + EOFSTR);
      break;
    case 21: // get temp probe resolution
      SendPaket('Q' + String(mySetupData->get_tempprecision()) + EOFSTR);
      break;
    case 24: // get status of temperature compensation (enabled | disabled)
      SendPaket('1' + String(mySetupData->get_tempcompenabled()) + EOFSTR);
      break;
    case 25: // get IF temperature compensation is available
#if defined(TEMPERATUREPROBE)
      SendPaket("A1#"); // this focuser supports temperature compensation
#else
      SendPaket("A0#");
#endif
      break;
    case 26: // get temperature coefficient steps/degree
      SendPaket('B' + String(mySetupData->get_tempcoefficient()) + EOFSTR);
      break;
    case 29: // get stepmode
      SendPaket('S' + String(mySetupData->get_stepmode()) + EOFSTR);
      break;
    case 32: // get if stepsize is enabled
      SendPaket('U' + String(mySetupData->get_stepsizeenabled()) + EOFSTR);
      break;
    case 33: // get stepsize
      SendPaket('T' + String(mySetupData->get_stepsize()) + EOFSTR);
      break;
    case 34: // get the time that an LCD screen is displayed for
      SendPaket('X' + String(mySetupData->get_lcdpagetime()) + EOFSTR);
      break;
    case 37: // get displaystatus
      SendPaket('D' + String(mySetupData->get_displayenabled()) + EOFSTR);
      break;
    case 38: // :38#   Dxx#      Get Temperature mode 1=Celsius, 0=Fahrenheight
      SendPaket('b' + String(mySetupData->get_tempmode()) + EOFSTR);
      break;
    case 39: // get the new motor position (target) XXXXXX
      SendPaket('N' + String(ftargetPosition) + EOFSTR);
      break;
    case 43: // get motorspeed
      SendPaket('C' + String(mySetupData->get_motorSpeed()) + EOFSTR);
      break;
    case 49: // aXXXXX
      SendPaket("ab552efd25e454b36b35795029f3a9ba7#");
      break;
    case 51: // return ESP8266Wifi Controller IP Address
      SendPaket('d' + ipStr + EOFSTR);
      break;
    case 52: // return ESP32 Controller number of TCP packets sent
      SendPaket('e' + String(packetssent) + EOFSTR);
      break;
    case 53: // return ESP32 Controller number of TCP packets received
      SendPaket('f' + String(packetsreceived) + EOFSTR);
      break;
    case 54: // return ESP32 Controller SSID
#ifdef LOCALSERIAL
      SendPaket("gSERIAL#");
#endif
#ifdef BLUETOOTH
      SendPaket("gBLUETOOTH#");
#endif
#if !defined(LOCALSERIAL) && !defined(BLUETOOTHMODE)
      SendPaket('g' + String(mySSID) + EOFSTR);
#endif
      break;
    case 55: // get motorspeed delay for current speed setting
      SendPaket('0' + String(driverboard->getstepdelay()) + EOFSTR);
      break;
    case 58: // get controller features
      SendPaket('m' + String(Features) + EOFSTR);
      break;
    case 62: // get update of position on lcd when moving (00=disable, 01=enable)
      SendPaket('L' + String(mySetupData->get_lcdupdateonmove()) + EOFSTR);
      break;
    case 72: // get DelayAfterMove
      SendPaket('3' + String(mySetupData->get_DelayAfterMove()) + EOFSTR);
      break;
    case 74: // get backlash in enabled status
      SendPaket((mySetupData->get_backlash_in_enabled() == 0) ? "40#" : "41#");
      break;
    case 76: // get backlash OUT enabled status
      SendPaket((mySetupData->get_backlash_out_enabled() == 0) ? "50#" : "51#");
      break;
    case 78: // return number of backlash steps IN
      SendPaket('6' + String(mySetupData->get_backlashsteps_in()) + EOFSTR);
      break;
    case 80: // return number of backlash steps OUT
      SendPaket('7' + String(mySetupData->get_backlashsteps_out()) + EOFSTR);
      break;
    case 83: // get if there is a temperature probe
      SendPaket('c' + String(tprobe1) + EOFSTR);
      break;
    case 87: // get tc direction
      SendPaket('k' + String(mySetupData->get_tcdirection()) + EOFSTR);
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
        ftargetPosition = (ftargetPosition > mySetupData->get_maxstep()) ? mySetupData->get_maxstep() : ftargetPosition;
        UpdatePositionOledText();
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
        tempstepsize = (tempstepsize < MINIMUMSTEPSIZE ) ? MINIMUMSTEPSIZE : tempstepsize;
        tempstepsize = (tempstepsize > MAXIMUMSTEPSIZE ) ? MAXIMUMSTEPSIZE : tempstepsize;
        mySetupData->set_stepsize(tempstepsize);
      }
      break;
    case 20: // set the temperature resolution setting for the DS18B20 temperature probe
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = WorkString.toInt();
      mySetupData->set_tempprecision((byte) paramval);
#if defined(TEMPERATUREPROBE)
      sensor1.setResolution(tpAddress, (byte) paramval);
#endif
      break;
    case 22: // set the temperature compensation value to xxx
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = WorkString.toInt();
      mySetupData->set_tempcoefficient((byte)paramval);
      break;
    case 23: // set the temperature compensation ON (1) or OFF (0)
#if defined(TEMPERATUREPROBE)
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
      // this is the proper way to do this
      driverboard->setstepmode((byte) paramval);
      mySetupData->set_stepmode(driverboard->getstepmode());
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
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      mySetupData->set_displayenabled((byte)(WorkString.toInt() & 1));
      if (mySetupData->get_displayenabled() == 1)
      {
#if defined(OLEDTEXT)
        if ( displayfound == true )
        {
          myoled->dim(false);
        }
#endif
      }
      else
      {
#if defined(OLEDTEXT)
        if ( displayfound == true )
        {
          // clear display buffer
          // set dim
          myoled->clearDisplay();
          myoled->dim(true);
        }
#endif
      }
      break;
    case 40: // reset Arduino myFocuserPro2E controller
      software_Reboot(2000);      // reboot with 2s delay
      break;
    case 42: // reset focuser defaults
      if ( isMoving == 0 )
      {
        mySetupData->SetFocuserDefaults();
        ftargetPosition = fcurrentPosition = mySetupData->get_fposition();
      }
      break;
    case 48: // save settings to EEPROM
      mySetupData->set_fposition(fcurrentPosition); // need to forth save setting????????
      break;
    case 56: // set motorspeed delay for current speed setting
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      driverboard->setstepdelay(WorkString.toInt());
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

    // compatibilty with myFocuserpro2 in LOCALSERIAL mode
    case 44: // set motorspeed threshold when moving - switches to slowspeed when nearing destination
      //WorkString = receiveString.substring(3, receiveString.length() - 1);
      //motorspeedchangethresholdsteps  = (byte) (WorkString.toInt() & 255);
      break;
    case 45: // get motorspeedchange threshold value
      SendPaket("G200#");
      break;
    case 46: // enable/Disable motorspeed change when moving
      //WorkString = receiveString.substring(3, receiveString.length() - 1);
      //motorspeedchangethresholdenabled = (byte) (WorkString.toInt() & 1);
      break;
    case 47: // get motorspeedchange enabled? on/off
      SendPaket("J0#");
      break;
    case 57: // set Super Slow Jogging Speed
      // ignore
      break;
    case 60: // set MotorSpeed when jogging
      // ignore
      break;
    case 63: // get status of home position switch (0=off, 1=closed, position 0)
      SendPaket("H0#");
      break;
    case 65: // set jogging state enable/disable
      // ignore
      break;
    case 66: // get jogging state enabled/disabled
      SendPaket("K0#");
      break;
    case 67: // set jogging direction, 0=IN, 1=OUT
      // ignore
      break;
    case 68: // get jogging direction, 0=IN, 1=OUT
      SendPaket("V0#");
      break;
    case 89:
      SendPaket("91#");
      break;
  }
}
#endif // if defined(ACCESSPOINT) || defined(STATIONMODE) || defined(LOCALSERIAL) || defined(BLUETOOTHMODE)

// Push button code
#if defined(INOUTPUSHBUTTONS)
void update_pushbuttons(void)
{
  long newpos;
  // PB are active high - pins float low if unconnected
  if ( digitalRead(INPBPIN) == 1 )                         // is pushbutton pressed?
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
#endif

#if defined(INFRAREDREMOTE)
void update_irremote()
{
  // check IR
  if (irrecv.decode(&results))
  {
    int adjpos = 0;
    static long lastcode;
    if ( results.value == KEY_REPEAT )
    {
      results.value = lastcode;                         // repeat last code
    }
    else
    {
      lastcode = results.value;
    }
    switch ( lastcode )
    {
      case IR_SLOW:
        mySetupData->set_motorSpeed(SLOW);
        driverboard->setmotorspeed(mySetupData->get_motorSpeed());     // set the correct delay based on motorSpeed
        break;
      case IR_MEDIUM:
        mySetupData->set_motorSpeed(MED);
        driverboard->setmotorspeed(mySetupData->get_motorSpeed());     // set the correct delay based on motorSpeed
        break;
      case IR_FAST:
        mySetupData->set_motorSpeed(FAST);
        driverboard->setmotorspeed(mySetupData->get_motorSpeed());     // set the correct delay based on motorSpeed
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
      case IR_SETPOSZERO :                              // 0 RESET POSITION TO 0
        adjpos = 0;
        ftargetPosition = 0;
        fcurrentPosition = 0;
        break;
    }
    irrecv.resume();                                    // Receive the next value
    unsigned long newpos;
    if ( adjpos < 0 )
    {
      newpos = fcurrentPosition + adjpos;               // since it is unsigned it will wrap around to highest value possible
      newpos = (newpos > mySetupData->get_maxstep() ) ? 0 : newpos;
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
#endif // if defined(INFRAREDREMOTE)

#if defined(OTAUPDATES)
void startOTA()
{
  ArduinoOTA.setHostname(OTAName);                      // Start the OTA service
  ArduinoOTA.setPassword(OTAPassword);

  ArduinoOTA.onStart([]()
  {
    DebugPrintln(F("Start"));
  });
  ArduinoOTA.onEnd([]()
  {
    DebugPrintln(F("End"));
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
    DebugPrint(F("Progress: "));
    DebugPrintln((progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error)
  {
    DebugPrint(F("Error :"));
    DebugPrintln(error);
    if (error == OTA_AUTH_ERROR) DebugPrintln(F("Auth Failed"));
    else if (error == OTA_BEGIN_ERROR) DebugPrintln(F("Begin Failed"));
    else if (error == OTA_CONNECT_ERROR) DebugPrintln(F("Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR) DebugPrintln(F("Receive Failed"));
    else if (error == OTA_END_ERROR) DebugPrintln(F("End Failed"));
  });
  ArduinoOTA.begin();
  DebugPrintln(F("OTA ready"));
}
#endif // if defined(OTAUPDATES)

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
  DebugPrintln(F("Serial started. Debug on."));
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

  displayfound = false;
#if defined(OLEDGRAPHICS) || defined(OLEDTEXT)
  displayfound = Init_OLED();
#endif

  delay(250);                                           // keep delays small otherwise issue with ASCOM

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
  pinMode(TEMPPIN, INPUT);                              // Configure GPIO pin for temperature probe
  DebugPrintln(checkfortprobestr);
  oledtextmsg(checkfortprobestr, -1, true, true);
  sensor1.begin();                                      // start the temperature sensor1
  DebugPrintln(F("Get # of Tsensors"));
  tprobe1 = sensor1.getDeviceCount();                   // should return 1 if probe connected
  DebugPrint(F("TSensors= "));
  DebugPrintln(tprobe1);
  if (sensor1.getAddress(tpAddress, 0) == true)
  {
    sensor1.setResolution(tpAddress, mySetupData->get_tempprecision());    // set probe resolution
    DebugPrint(F("Tsensors= "));
    DebugPrintln(tprobe1);
    DebugPrint(F("Set Tprecision to "));
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
    DebugPrintln(tprobenotfoundstr);
    oledtextmsg(tprobenotfoundstr, -1, false, true);
  }
#endif // end TEMPERATUREPROBE

  //_____Start WIFI config________________________

  Read_WIFI_config_SPIFFS(mySSID, myPASSWORD); //__ Read mySSID,myPASSWORD from SPIFFS if exist, otherwise use defaults

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
  EasyDDNS.update(60000);                               // Check for New Ip Every 60 Seconds.
  delay(5);
#endif // useduckdns

#if defined(INFRAREDREMOTE)
  irrecv.enableIRIn();                                  // Start the IR
#endif

  isMoving = 0;

#if defined(TEMPERATUREPROBE)
  readtemp(1);
#endif

#if defined(OTAUPDATES)
  DebugPrintln(startotaservicestr);
  oledtextmsg(startotaservicestr, -1, false, true);
  startOTA();                  // Start the OTA service
#endif // if defined(OTAUPDATES)

#ifdef WEBSERVER
  setNotFoundPage();            // set up webserver page for no connection to a myFocuserPro2 controller
  webserver.on("/", WEBSERVER_handleRoot);
  webserver.onNotFound(WEBSERVER_handleNotFound);
  webserver.begin();
  #endif

#ifdef ASCOMREMOTE
  ascomserver.on("/", ASCOM_handleRoot);
  ascomserver.onNotFound(ASCOM_handleNotFound);
  ascomserver.on("/api/v1/focuser/0/connected", HTTP_PUT, ASCOM_handleconnectedput);
  ascomserver.on("/api/v1/focuser/0/interfaceversion", HTTP_GET, ASCOM_handleinterfaceversionget);
  ascomserver.on("/api/v1/focuser/0/name", HTTP_GET, ASCOM_handlenameget);
  ascomserver.on("/api/v1/focuser/0/description", HTTP_GET, ASCOM_handledescriptionget);
  ascomserver.on("/api/v1/focuser/0/driverinfo", HTTP_GET, ASCOM_handledriverinfoget);
  ascomserver.on("/api/v1/focuser/0/driverversion", HTTP_GET, ASCOM_handledriverversionget);
  ascomserver.on("/api/v1/focuser/0/absolute", HTTP_GET, ASCOM_handleabsoluteget);
  ascomserver.on("/api/v1/focuser/0/maxstep", HTTP_GET, ASCOM_handlemaxstepget);
  ascomserver.on("/api/v1/focuser/0/maxincrement", HTTP_GET, ASCOM_handlemaxincrementget);
  ascomserver.on("/api/v1/focuser/0/temperature", HTTP_GET, ASCOM_handletemperatureget);
  ascomserver.on("/api/v1/focuser/0/position", HTTP_GET, ASCOM_handlepositionget);
  ascomserver.on("/api/v1/focuser/0/halt", HTTP_PUT, ASCOM_handlehaltput);
  ascomserver.on("/api/v1/focuser/0/ismoving", HTTP_GET, ASCOM_handleismovingget);
  ascomserver.on("/api/v1/focuser/0/stepsize", HTTP_GET, ASCOM_handlestepsizeget);
  ascomserver.on("/api/v1/focuser/0/connected", HTTP_GET, ASCOM_handleconnectedget);
  ascomserver.on("/api/v1/focuser/0/tempcomp", HTTP_GET, ASCOM_handletempcompget);
  ascomserver.on("/api/v1/focuser/0/tempcomp", HTTP_PUT, ASCOM_handletempcompput);
  ascomserver.on("/api/v1/focuser/0/tempcompavailable", HTTP_GET, ASCOM_handletempcompavailableget);
  ascomserver.on("/api/v1/focuser/0/move", HTTP_PUT, ASCOM_handlemoveput);
  ascomserver.on("/api/v1/focuser/0/supportedactions", HTTP_GET, ASCOM_handlesupportedactionsget);
  ascomserver.begin();
#endif

#if defined(INOUTLEDPINS)
  digitalWrite(INLEDPIN, 0);
  digitalWrite(OUTLEDPIN, 0);
#endif

  DebugPrint(currentposstr);
  DebugPrintln(fcurrentPosition);
  DebugPrint(targetposstr);
  DebugPrintln(ftargetPosition);
  DebugPrintln(setupendstr);
  oledtextmsg(setupendstr, -1, false, true);
}

//_____________________ loop()___________________________________________

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
#if !defined(WEBSERVER) && !defined(ASCOMREMOTE)
  static byte ConnectionStatus = 0;
#endif
#endif

#if defined(LOOPTIMETEST)
  DebugPrint(F("- Loop Start ="));
  DebugPrintln(millis());
#endif

#ifdef WEBSERVER
  webserver.handleClient();
#endif

  // we want this to run if using an accesspoint or stationmode
  // we do not want to run for bluetooth, local serial, webserver or ascomserver
#if !defined(ASCOMREMOTE) && !defined(WEBSERVER) && !defined(BLUETOOTHMODE) && !defined(LOCALSERIAL)
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
        ESP_Communication(ESPDATA);                     // Wifi communication
      }
    }
    else
    {
      ConnectionStatus = 1;
    }
  }
#endif // !defined(ASCOMREMOTE) && !defined(WEBSERVER) && !defined(BLUETOOTHMODE) && !defined(LOCALSERIAL)

#if defined(BLUETOOTHMODE)
  if ( SerialBT.available() )
  {
    processbt();
  }
  // if there is a command from Bluetooth
  if ( queue.count() >= 1 )                             // check for serial command
  {
    ESP_Communication(BTDATA);
  }
#endif // if defined(BLUETOOTHMODE)
#if defined(LOCALSERIAL)
  // if there is a command from Serial port
  if ( Serial.available() )
  {
    processserial();
  }
  if ( queue.count() >= 1 )                             // check for serial command
  {
    ESP_Communication(SERIALDATA);
  }
#endif // if defined(LOCALSERIAL)

#if defined(OTAUPDATES)
  ArduinoOTA.handle();                                  // listen for OTA events
#endif // if defined(OTAUPDATES)

#ifdef ASCOMREMOTE
  ascomserver.handleClient();
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
        DebugPrint(F("Idle=>InitMove"));
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
#if defined(INOUTPUSHBUTTONS)
        update_pushbuttons();
#endif
#if defined(INFRAREDREMOTE)
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

#if defined(TEMPERATUREPROBE)
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
        if ( mySetupData->get_displayenabled() == 1)
        {
          updatecount++;
          if ( updatecount > LCDUPDATEONMOVE )
          {
            updatecount++;
            if ( updatecount > LCDUPDATEONMOVE )
            {
              delay(5);
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
          DebugPrintln(F("=> State_DelayAfterMove"));
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

#if defined(LOCALSERIAL)
void clearSerialPort()
{
  while (Serial.available())
  {
    Serial.read();
  }
}

void processserial()
{
  // Serial.read() only returns a single char so build a command line one char at a time
  // : starts the command, # ends the command, do not store these in the command buffer
  // read the command until the terminating # character
  while (Serial.available() )
  {
    char inChar = Serial.read();
    switch ( inChar )
    {
      case STARTSTR :     // start
        serialline = "";
        break;
      case '\r' :
      case '\n' :
        // ignore
        break;
      case EOFSTR :     // eoc
        queue.push(serialline);
        break;
      default :      // anything else
        serialline = serialline + inChar;
        break;
    }
  }
}
#endif // if defined(LOCALSERIAL)

#if defined(BLUETOOTHMODE)
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
      case STARTSTR :     // start
        btline = "";
        break;
      case '\r' :
      case '\n' :
        // ignore
        break;
      case EOFSTR :     // eoc
        queue.push(btline);
        break;
      default :      // anything else
        btline = btline + inChar;
        break;
    }
  }
}
#endif // if defined(BLUETOOTHMODE)
