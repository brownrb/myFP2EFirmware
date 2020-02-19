// ----------------------------------------------------------------------------------------------
// TITLE: myFP2ESP FIRMWARE OFFICIAL RELEASE 118
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

// ----------------------------------------------------------------------------------------------
// TO PROGRAM THE FIRMWARE
// ----------------------------------------------------------------------------------------------
// 1. Set your DRVBRD in myBoards.h so the correct driver board is used
// 2. Set the correct hardware options and controller modes in focuserconfig.h to match your hardware
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
// Arduino IDE 1.8.9
// ESP8266 Driver Board 2.4.0
// Libraries
// Arduino JSON 6.11.2
// myOLED as in myFP2ELibs
// IRRemoteESP32 2.0.1 as in myFP2ELibs
// HalfStepperESP32 as in myFP2ELibs
// myDallas Temperature 3.7.3A as in myFP2ELibs
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
// 2: FOR ALL ESP8266 BOARDS AND R3-WEMOS-ESP32 USING DRV8825 SET DRV8825STEPMODE in myBoards.h
// ----------------------------------------------------------------------------------------------
// Remember to set DRV8825TEPMODE to the correct value if using WEMOS or NODEMCUV1 in myBoards.h

// ----------------------------------------------------------------------------------------------
// 3: FOR ULN2003, L293D, L293DMINI, L298N, L9110S specify STEPSPERREVOLUTION in myBoards.h
// ----------------------------------------------------------------------------------------------
// For these driver boards you MUST set STEPSPERREVOLUTION to the correct value in myBoards.h

// ----------------------------------------------------------------------------------------------
// 4: SPECIFY HARDWARE OPTIONS IN focuserconfig.h
// ----------------------------------------------------------------------------------------------
// Please specify your controller options in focuserconfig.h, such as OLEDTEXT and TEMPERATUREPROBE

// ----------------------------------------------------------------------------------------------
// 5: SPECIFY THE CONTROLLER MODE IN focuserconfig.h
// ----------------------------------------------------------------------------------------------
// Please specify your controller mode in focuserconfig.h, such as ACCESSPOINT and WEBSERVER

// ----------------------------------------------------------------------------------------------
// 6: INCLUDES
// ----------------------------------------------------------------------------------------------
#include "focuserconfig.h"

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
// if using DuckDNS you need to set these next two parameters, duckdnsdomain and duckdnstoken
// cannot use DuckDNS with ACCESSPOINT, BLUETOOTHMODE or LOCALSERIAL mode
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
//int staticip = STATICIPON;      // IP address specified by controller - must be defined correctly
int staticip = STATICIPOFF;       // IP address is generated by network device and is dynamic and can change

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
#include "BluetoothSerial.h"                // needed for Bluetooth comms
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;                   // define BT adapter to use
#endif // BLUETOOTHMODE

// ----------------------------------------------------------------------------------------------
// 14: FIRMWARE CODE START - INCLUDES AND LIBRARIES
// ----------------------------------------------------------------------------------------------
// Compile this with Arduino IDE 1.8.9 with ESP8266 Core library installed v2.5.2 [for ESP8266]
// Make sure target board is set to Node MCU 1.0 (ESP12-E Module) [for ESP8266]

// Project specific includes
#if defined(BLUETOOTHMODE) || defined(LOCALSERIAL)
#include "ESPQueue.h"                       //  By Steven de Salas
#endif

// ----------------------------------------------------------------------------------------------
// 15: CONTROLLER FEATURES -- DO NOT CHANGE
// ----------------------------------------------------------------------------------------------
unsigned long Features = 0L;

void setFeatures()
{
#ifdef LCDDISPLAY
  Features = Features + ENABLEDLCD;
#endif
#ifdef OLEDTEXT
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
#ifdef INFRAREDREMOTE
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
#if defined(JOYSTICK1) || defined(JOYSTICK2)
  Features = Features + ENABLEDJOYSTICK;
#endif
}

// ----------------------------------------------------------------------------------------------
// 16: GLOBAL DATA -- DO NOT CHANGE
// ----------------------------------------------------------------------------------------------

//  StateMachine definition
#define State_Idle              0
#define State_InitMove          1
#define State_ApplyBacklash     2
#define State_Moving            3
#define State_DelayAfterMove    4
#define State_FinishedMove      5
#define State_SetHomePosition   6
#define State_FindHomePosition  7
#define move_in                 0
#define move_out                1
#define oled_off                0
#define oled_on                 1
#define oled_stay               2
//           reversedirection
//__________________________________
//               0   |   1
//__________________________________
//move_out  1||  1   |   0
//move_in   0||  0   |   1

String programName;
DriverBoard* driverboard;

char programVersion[] = "118";
char ProgramAuthor[]  = "(c) R BROWN 2020";

unsigned long fcurrentPosition;             // current focuser position
unsigned long ftargetPosition;              // target position
unsigned long tmppos;
boolean displayfound;

byte tprobe1;                               // indicate if there is a probe attached to myFocuserPro2
byte isMoving;                              // is the motor currently moving
String ipStr;                               // shared between BT mode and other modes

#if defined(BLUETOOTHMODE) || defined(LOCALSERIAL)
#include "ESPQueue.h"                       //  By Steven de Salas
Queue queue(QUEUELENGTH);                   // receive serial queue of commands
#ifdef LOCALSERIAL
String serialline;                          // buffer for serial data
#endif
#ifdef BLUETOOTHMODE
String btline;                              // buffer for serial data
#endif
#endif // #if defined(BLUETOOTHMODE) || defined(LOCALSERIAL)

#if defined(ACCESSPOINT) || defined(STATIONMODE)
IPAddress ESP32IPAddress;
String ServerLocalIP;
WiFiServer myserver(SERVERPORT);
WiFiClient myclient;                        // only one client supported, multiple connections denied
IPAddress myIP;
long rssi;
#endif // #if defined(ACCESSPOINT) || defined(STATIONMODE)

#ifdef TEMPERATUREPROBE
#include <OneWire.h>                        // https://github.com/PaulStoffregen/OneWire
#include <myDallasTemperature.h>
OneWire oneWirech1(TEMPPIN);                // setup temperature probe
DallasTemperature sensor1(&oneWirech1);
DeviceAddress tpAddress;                    // holds address of the temperature probe
#endif

#ifdef OLEDTEXT
#include <Wire.h>                           // needed for I2C => OLED display
#include <mySSD1306Ascii.h>
#include <mySSD1306AsciiWire.h>
SSD1306AsciiWire* myoled;
#endif // #ifdef OLEDTEXT

#ifdef OLEDGRAPHICS
#include <Wire.h>
#include "images.h"
#ifdef USE_SSD1306                          // For the OLED 128x64 0.96" display using the SSD1306 driver
#include <SSD1306Wire.h>
SSD1306Wire* myoled;
#endif
#ifdef USE_SSH1106                          // For the OLED 128x64 1.3" display using the SSH1106 driver
#include <SH1106Wire.h>
SH1106Wire* myoled;
#endif
#endif // #ifdef OLEDGRAPHICS

int packetsreceived;
int packetssent;
bool mdnsserverstate;
bool webserverstate;
bool ascomserverstate;
bool managementserverstate;
bool otaupdatestate;
bool duckdnsstate;

SetupData *mySetupData;

// ----------------------------------------------------------------------------------------------
// 17: FIRMWARE CODE START - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
// Compile this with Arduino IDE 1.8.9 with ESP8266 Core library installed v2.5.2 [for ESP8266]
// Make sure target board is set to Node MCU 1.0 (ESP12-E Module) [for ESP8266]

#include "comms.h"

// ----------------------------------------------------------------------------------------------
// 18: TEMPERATURE - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#ifdef TEMPERATUREPROBE
void init_temp(void)
{
  // start temp probe
  pinMode(TEMPPIN, INPUT);                // Configure GPIO pin for temperature probe
  DebugPrintln(CHECKFORTPROBESTR);
  sensor1.begin();                        // start the temperature sensor1
  DebugPrintln(F(GETTEMPPROBESSTR));
  tprobe1 = sensor1.getDeviceCount();     // should return 1 if probe connected
  DebugPrint(F(TPROBESTR));
  DebugPrintln(tprobe1);
  if (sensor1.getAddress(tpAddress, 0) == true)
  {
    tprobe1 = 1;
    sensor1.setResolution(tpAddress, mySetupData->get_tempprecision());    // set probe resolution
    DebugPrint(F(SETTPROBERESSTR));
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
    DebugPrintln(TPROBENOTFOUNDSTR);
  }
}

void temp_setresolution(byte rval)
{
  sensor1.setResolution(tpAddress, rval);
}

float read_temp(byte new_measurement)
{
  static float lasttemp = 20.0;               // start temp value
  if (!new_measurement)
  {
    return lasttemp;                          // return latest measurement
  }

  float result = sensor1.getTempCByIndex(0);  // get channel 1 temperature, always in celsius
  DebugPrint(F(TEMPSTR));
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

void update_temp(void)
{
  static byte tcchanged = mySetupData->get_tempcompenabled();  // keeps track if tempcompenabled changes

  if (tprobe1 == 1)
  {
    static unsigned long lasttempconversion = 0;
    static byte requesttempflag = 0;                      // start with request
    unsigned long tempnow = millis();

    // see if the temperature needs updating - done automatically every 1.5s
    if (TimeCheck(lasttempconversion, TEMPREFRESHRATE))   // see if the temperature needs updating
    {
      static float tempval;
      static float starttemp;                             // start temperature to use when temperature compensation is enabled

      if ( tcchanged != mySetupData->get_tempcompenabled() )
      {
        tcchanged = mySetupData->get_tempcompenabled();
        if ( tcchanged == 1 )
        {
          starttemp = read_temp(1);
        }
      }

      lasttempconversion = tempnow;               // update time stamp

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

// ----------------------------------------------------------------------------------------------
// 19A: OLED GRAPHICS DISPLAY - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
// Enclose function code in #ifdef - #endif so function declarations are visible

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

void oledgraphicmsg(String str, int val, boolean clrscr)
{
#if defined(OLEDGRAPHICS)
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
#endif
}

void oled_draw_Wifi(int j)
{
#if defined(OLEDGRAPHICS)
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
#endif
}

void oled_draw_main_update(void)
{
#if defined(OLEDGRAPHICS)
  if (displayfound == true)
  {
    myoled->clear();
    myoled->setTextAlignment(TEXT_ALIGN_CENTER);
    myoled->setFont(ArialMT_Plain_10);
    myoled->drawString(64, 0, driverboard->getboardname());
    myoled->drawString(64, 12, "IP= " + ipStr);

    myoled->setTextAlignment(TEXT_ALIGN_LEFT);
#ifdef TEMPERATUREPROBE
    String tmbuffer = String(read_temp(0), 2);
#else
    String tmbuffer = "20.0";
#endif
    myoled->drawString(54, 54, "TEMP:" + tmbuffer + " C");
    myoled->drawString(0, 54, "BL:" + String(mySetupData->get_backlashsteps_out()));
    //myoled->drawString(24, 54, "SM:" + String(driverboard->getstepmode()));

    myoled->setTextAlignment(TEXT_ALIGN_CENTER);
    myoled->setFont(ArialMT_Plain_24);

    char dir = (mySetupData->get_focuserdirection() == move_in ) ? '<' : '>';
    myoled->drawString(64, 28, String(fcurrentPosition, DEC) + ":" +  String(fcurrentPosition % driverboard->getstepmode(), DEC) + ' ' + dir); // Print currentPosition
    myoled->display();
  }
#endif
}

boolean Init_OLED(void)
{
#if defined(OLEDGRAPHICS)
  Wire.begin();
  Wire.setClock(400000L);                               // 400 kHZ max. speed on I2C

#ifdef  DEBUG
  //Scan all I2C addresses for device detection
  Serial.print ("Scan for devices on I2C: ");
  for (int i = 0; i < 128; i++)
  {
    Wire.beginTransmission(i);
    if (!Wire.endTransmission ())
    {
      Serial.print(" 0x");
      Serial.print(i, HEX);
    }
  }
  Serial.println("");
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
#ifdef USE_SSD1306                    // For the OLED 128x64 0.96" display using the SSD1306 driver 
    myoled = new SSD1306Wire(OLED_ADDR , I2CDATAPIN, I2CCLKPIN);
#endif
#ifdef USE_SSH1106                    // For the OLED 128x64 1.3" display using the SSH1106 driver
    myoled = new SH1106Wire(OLED_ADDR , I2CDATAPIN, I2CCLKPIN);
#endif
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
#endif
  return displayfound;
}

// ----------------------------------------------------------------------------------------------
// 19B: OLED TEXT DISPLAY - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#ifdef OLEDTEXT
#endif // #ifdef OLEDTEXT
// Enclose function code in #ifdef - #endif so function declarations are visible

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

void display_oledtext_page0(void)           // displaylcd screen
{
#ifdef OLEDTEXT
  if (displayfound == true)
  {
    char tempString[20];
    myoled->home();
    myoled->print(F(CURRENTPOSSTR));
    myoled->print(fcurrentPosition);
    myoled->clearToEOL();

    myoled->println();
    myoled->print(F(TARGETPOSSTR));
    myoled->print(ftargetPosition);
    myoled->clearToEOL();
    myoled->println();

    myoled->print(F(COILPWRSTR));
    myoled->print(mySetupData->get_coilpower());
    myoled->clearToEOL();
    myoled->println();

    myoled->print(F(REVDIRSTR));
    myoled->print(mySetupData->get_reversedirection());
    myoled->clearToEOL();
    myoled->println();

    // stepmode setting
    myoled->print(F(STEPMODESTR));
    myoled->print(mySetupData->get_stepmode());
    myoled->clearToEOL();
    myoled->println();

    //Temperature
    myoled->print(F(TEMPSTR));
#ifdef TEMPERATUREPROBE
    if ( mySetupData->get_tempmode() == 1)
    {
      // celsius
      myoled->print(String(read_temp(0), 2));
      myoled->print(" c");
    }
    else
    {
      // fahrenheit
      // (0°C × 9/5) + 32 = 32°F
      float ft = read_temp(0);
      ft = (ft * 1.8) + 32;
      myoled->print(String(ft, 2));
      myoled->print(" f");
    }
#else
    if ( mySetupData->get_tempmode() == 1)
    {
      myoled->print("20.00 c");
    }
    else
    {
      myoled->print("68.00 f");
    }
#endif
    myoled->clearToEOL();
    myoled->println();

    //Motor Speed
    myoled->print(F(MOTORSPEEDSTR));
    myoled->print(mySetupData->get_motorSpeed());
    myoled->clearToEOL();
    myoled->println();

    //MaxSteps
    myoled->print(F(MAXSTEPSSTR));
    ltoa(mySetupData->get_maxstep(), tempString, 10);
    myoled->print(tempString);
    myoled->clearToEOL();
    myoled->println();
  }
#endif
}

void display_oledtext_page1(void)
{
#ifdef OLEDTEXT
  if (displayfound == true)
  {
    // temperature compensation
    myoled->print(F(TCOMPSTEPSSTR));
    myoled->print(mySetupData->get_tempcoefficient());
    myoled->clearToEOL();
    myoled->println();

    myoled->print(F(TCOMPSTATESTR));
    myoled->print(mySetupData->get_tempcompenabled());
    myoled->clearToEOL();
    myoled->println();

    myoled->print(F(TCOMPDIRSTR));
    myoled->print(mySetupData->get_tcdirection());
    myoled->clearToEOL();
    myoled->println();

    myoled->print(F(BACKLASHINSTR));
    myoled->print(mySetupData->get_backlash_in_enabled());
    myoled->clearToEOL();
    myoled->println();

    myoled->print(F(BACKLASHOUTSTR));
    myoled->print(mySetupData->get_backlash_out_enabled());
    myoled->clearToEOL();
    myoled->println();

    myoled->print(F(BACKLASHINSTEPSSTR));
    myoled->print(mySetupData->get_backlashsteps_in());
    myoled->clearToEOL();
    myoled->println();

    myoled->print(F(BACKLASHOUTSTEPSSTR));
    myoled->print(mySetupData->get_backlashsteps_out());
    myoled->clearToEOL();
    myoled->println();
  }
#endif
}

void display_oledtext_page2(void)
{
#ifdef OLEDTEXT
  if (displayfound == true)
  {
#if defined(ACCESSPOINT) || defined(STATIONMODE)
    myoled->setCursor(0, 0);
#if defined(ACCESSPOINT)
    myoled->print(F(ACCESSPOINTSTR));
    myoled->clearToEOL();
    myoled->println();
#endif
#if defined(STATIONMODE)
    myoled->print(F(STATIONMODESTR));
    myoled->clearToEOL();
    myoled->println();
#endif
    myoled->print(F(SSIDSTR));
    myoled->print(mySSID);
    myoled->clearToEOL();
    myoled->println();
    myoled->print(F(IPADDRESSSTR));
    myoled->print(ipStr);
    myoled->clearToEOL();
    myoled->println();
#endif // if defined(ACCESSPOINT) || defined(STATIONMODE)

#if defined(WEBSERVER)
    //myoled->setCursor(0, 0);
    myoled->print(F(WEBSERVERSTR));
    myoled->clearToEOL();
    myoled->println();
    myoled->print(F(IPADDRESSSTR));
    myoled->print(ipStr);
    myoled->clearToEOL();
    myoled->println();
    myoled->print(PORTSTR);
    myoled->print(String(mySetupData->get_webserverport()));
    myoled->clearToEOL();
    myoled->println();
#endif // webserver
#if defined(ASCOMREMOTE)
    myoled->print(F(ASCOMREMOTESTR));
    myoled->clearToEOL();
    myoled->println();
    myoled->print(F(IPADDRESSSTR));
    myoled->print(ipStr);
    myoled->clearToEOL();
    myoled->println();
    myoled->print(PORTSTR);
    myoled->print(String(mySetupData->get_ascomalpacaport()));
    myoled->clearToEOL();
    myoled->println();
#endif

#if defined(BLUETOOTHMODE)
    myoled->setCursor(0, 0);
    myoled->print(F(BLUETOOTHSTR));
    myoled->clearToEOL();
    myoled->println();
#endif

#if defined(LOCALSERIAL)
    myoled->setCursor(0, 0);
    myoled->println(F(LOCALSERIALSTR));
#endif
  }
#endif // #ifdef OLEDTEXT
}

void update_oledtext_position(void)
{
#ifdef OLEDTEXT
  if (displayfound == true)
  {
    myoled->setCursor(0, 0);
    myoled->print(F(CURRENTPOSSTR));
    myoled->print(fcurrentPosition);
    myoled->clearToEOL();
    myoled->println();

    myoled->print(F(TARGETPOSSTR));
    myoled->print(ftargetPosition);
    myoled->clearToEOL();
    myoled->println();
  }
#endif
}

void update_oledtextdisplay(void)
{
#ifdef OLEDTEXT
  if (displayfound == true)
  {
    static unsigned long currentMillis;
    static unsigned long olddisplaytimestampNotMoving = millis();
    static byte displaypage = 0;

    currentMillis = millis();                       // see if the display needs updating
    // if (((currentMillis - olddisplaytimestampNotMoving) > ((int)mySetupData->get_lcdpagetime() * 1000)) || (currentMillis < olddisplaytimestampNotMoving))
    if (TimeCheck(olddisplaytimestampNotMoving, (int)mySetupData->get_lcdpagetime() * 1000))   // see if the display needs updating
    {
      olddisplaytimestampNotMoving = currentMillis; // update the timestamp
      myoled->clear();                              // clrscr OLED
      switch (displaypage)
      {
        case 0:   display_oledtext_page0();
          break;
        case 1:   display_oledtext_page1();
          break;
        case 2:   display_oledtext_page2();
          break;
        default:  display_oledtext_page0();
          break;
      }
      displaypage++;
      displaypage = (displaypage > 2) ? 0 : displaypage;
    }
  }
#endif
}

void init_oledtextdisplay(void)
{
#ifdef OLEDTEXT
#if defined(ESP8266)
#if (DRVBRD == PRO2EL293DNEMA) || (DRVBRD == PRO2EL293D28BYJ48)
  Wire.begin(I2CDATAPIN, I2CCLKPIN);      // l293d esp8266 shield
  DebugPrintln(F("Setup PRO2EL293DNEMA/PRO2EL293D28BYJ48 I2C"));
#else
  DebugPrintln(F("Setup esp8266 I2C"));
  Wire.begin();
#endif
#else
  DebugPrintln(F("Setup esp32 I2C"));
  Wire.begin(I2CDATAPIN, I2CCLKPIN);        // esp32
#endif
  Wire.beginTransmission(OLED_ADDR);                    //check if OLED display is present
  if (Wire.endTransmission() != 0)
  {
    TRACE();
    DebugPrintln(F(I2CDEVICENOTFOUNDSTR));
    displayfound = false;
  }
  else
  {
    displayfound = true;
    myoled = new SSD1306AsciiWire();
    // Setup the OLED
#ifdef USE_SSD1306
    // For the OLED 128x64 0.96" display using the SSD1306 driver
    myoled->begin(&Adafruit128x64, OLED_ADDR);
#endif
#ifdef USE_SSH1106
    // For the OLED 128x64 1.3" display using the SSH1106 driver
    myoled->begin(&SH1106_128x64, OLED_ADDR);
#endif
    myoled->set400kHz();
    myoled->setFont(Adafruit5x7);
    myoled->clear();                          // clrscr OLED
    myoled->Display_Normal();                 // black on white
    myoled->Display_On();                     // display ON
    myoled->Display_Rotate(0);                // portrait, not rotated
    myoled->Display_Bright();
#ifdef SHOWSTARTSCRN
    myoled->println(programName);             // print startup screen
    myoled->println(programVersion);
    myoled->println(ProgramAuthor);
#endif // showstartscreen
  }
#endif // OLEDTEXT
}

// ----------------------------------------------------------------------------------------------
// 20: HOMEPOSITION SWITCH - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#ifdef HOMEPOSITIONSWITCH
volatile int hpswstate;

void IRAM_ATTR hpsw_isr()
{
  // internal pullup = 1 when switch is OPEN, hpswstate of 0
  hpswstate = !(digitalRead(HPSWPIN));
  // when switch is closed, level is 0 so hpswstate is 1
}

void init_homepositionswitch(void)
{
  // setup home position switch
  pinMode(HPSWPIN, INPUT_PULLUP);
  // setup interrupt, falling, when switch is closed, pin goes low
  attachInterrupt(HPSWPIN, hpsw_isr, CHANGE);
  hpswstate = 0;
}
#endif // #ifdef HOMEPOSITIONSWITCH

// ----------------------------------------------------------------------------------------------
// 21: INFRARED REMOTE CONTROLLER - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#ifdef INFRAREDREMOTE

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
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
      case IR_SETPOSZERO:                         // 0 RESET POSITION TO 0
        adjpos = 0;
        ftargetPosition = 0;
        fcurrentPosition = 0;
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
#if defined(JOYSTICK1) || defined(JOYSTICK2)
    // restore motorspeed just in case
    driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif
  }
}

void init_irremote(void)
{
  irrecv.enableIRIn();                            // Start the IR
}
#endif // #ifdef INFRAREDREMOTE

// ----------------------------------------------------------------------------------------------
// 22: JOYSTICK - CHANGE AT YOUR OWN PERIL
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
  int joyspeed;
  int joyval;

  DebugPrintln(F(UPDATEJOYSTICKSTR));
  joyval = analogRead(JOYINOUTPIN);
  DebugPrint(F(JOYSTICKVALSTR));
  DebugPrintln(joyval);
  if ( joyval < (JZEROPOINT - JTHRESHOLD) )
  {
    ftargetPosition--;                            // move IN
    DebugPrint(F(JOYSTICKXINVALSTR));
    DebugPrint(joyval);
    joyspeed = map(joyval, 0, (JZEROPOINT - JTHRESHOLD), MSFAST, MSSLOW);
    DebugPrint(F(JOYSTICKSPEEDSTR));
    DebugPrintln(joyspeed);
    driverboard->setstepdelay(joyspeed);
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
    DebugPrint(F(JOYSTICKXOUTVALSTR));
    DebugPrint(joyval);
    joyspeed = map(joyval, 0, (JMAXVALUE - (JZEROPOINT + JTHRESHOLD)), MSSLOW, MSFAST);
    DebugPrint(F(JOYSTICKSPEEDSTR));
    DebugPrintln(joyspeed);
    driverboard->setstepdelay(joyspeed);
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
  int joyspeed;
  int joyval;

  joyval = analogRead(JOYINOUTPIN);               // range is 0 - 4095, midpoint is 2047
  DebugPrint(F(JOYSTICKVALSTR));
  DebugPrintln(joyval);
  if ( joyval < (JZEROPOINT - JTHRESHOLD) )
  {
    ftargetPosition--;                            // move IN
    DebugPrint(F(JOYSTICKXINVALSTR));
    DebugPrint(joyval);
    joyspeed = map(joyval, 0, (JZEROPOINT - JTHRESHOLD), MSFAST, MSSLOW);
    DebugPrint(F(JOYSTICKSPEEDSTR));
    DebugPrintln(joyspeed);
    driverboard->setstepdelay(joyspeed);
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
    DebugPrint(F(JOYSTICKXOUTVALSTR));
    DebugPrint(joyval);
    joyspeed = map(joyval, 0, (JMAXVALUE - (JZEROPOINT + JTHRESHOLD)), MSSLOW, MSFAST);
    DebugPrint(F(JOYSTICKSPEEDSTR));
    DebugPrintln(joyspeed);
    driverboard->setstepdelay(joyspeed);
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
// 23: PUSHBUTTONS - CHANGE AT YOUR OWN PERIL
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
// 24: mDNS SERVER - CHANGE AT YOUR OWN PERIL
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
    DebugPrintln(F(MDNSSTARTFAILSTR));
    mdnsserverstate = STOPPED;
  }
  else
  {
    DebugPrintln(F(MDNSSTARTEDSTR));
    // Add service to MDNS-SD, MDNS.addService(service, proto, port)
    MDNS.addService("http", "tcp", MDNSSERVERPORT);
    mdnsserverstate = RUNNING;
  }
  delay(10);                      // small pause so background tasks can run
}

void stop_mdns_service(void)
{
  DebugPrintln(F(STOPMDNSSERVERSTR));
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
    DebugPrintln(F(SERVERNOTRUNNINGSTR));
  }
  delay(10);                      // small pause so background tasks can run
}
#endif // #ifdef MDNSSERVER

// ----------------------------------------------------------------------------------------------
// 26: MANAGEMENT INTERFACE - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#ifdef MANAGEMENT

#if defined(ESP8266)
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif // if defined(esp8266)

#if defined(ESP8266)
ESP8266WebServer mserver(MSSERVERPORT);
#else
WebServer mserver(MSSERVERPORT);
#endif // if defined(esp8266)

String MHomePage;
String MNotFoundPage;
String MUploadPage;
File fsUploadFile;

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

// send the right file to the client (if it exists)
bool MANAGEMENT_handlefileread(String path)
{
  DebugPrintln("handleFileRead: " + path);
  if (path.endsWith("/"))
  {
    path += "index.html";                               // If a folder is requested, send the index file
  }
  String contentType = MANAGEMENT_getcontenttype(path); // Get the MIME type
  if ( SPIFFS.exists(path) )                            // If the file exists
  {
    File file = SPIFFS.open(path, "r");                 // Open it
#ifdef MANAGEMENTFORCEDOWNLOAD
    if ( path.indexOf(".html") == -1)
    {
      // if not an html file, force download : html files will be displayed in browser
      mserver.sendHeader("Content-Type", "application/octet-stream");
      mserver.sendHeader("Content-Disposition", "attachment");
    }
#endif
    mserver.streamFile(file, contentType);              // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  else
  {
    TRACE();
    DebugPrintln(F(FILENOTFOUNDSTR));
    return false;                                         // If the file doesn't exist, return false
  }
}

void MANAGEMENT_listSPIFFSfiles(void)
{
  if ( !SPIFFS.begin())
  {
    TRACE();
    DebugPrintln(F(SPIFFSNOTSTARTEDSTR));
    return;
  }
  // example code taken from FSBrowser
  String path = "/";
  DebugPrintln("MANAGEMENT_listSPIFFSfiles: " + path);
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
  // load not found page from spiffs - wsnotfound.html
  if (!SPIFFS.begin())
  {
    TRACE();
    DebugPrintln(F(SPIFFSNOTSTARTEDSTR));
    DebugPrintln(F(BUILDDEFAULTPAGESTR));
    MNotFoundPage = "<html><head><title>Management Server></title></head><body><p>URL was not found</p><p><form action=\"/\" method=\"GET\"><input type=\"submit\" value=\"HOMEPAGE\"></form></p></body></html>";
  }
  else
  {
    if ( SPIFFS.exists("/msnotfound.html"))
    {
      // open file for read
      File file = SPIFFS.open("/msnotfound.html", "r");
      // read contents into string
      TRACE();
      DebugPrintln(F(READPAGESTR));
      MNotFoundPage = file.readString();

      TRACE();
      DebugPrintln(F(PROCESSPAGESTARTSTR));
      // process for dynamic data
      MNotFoundPage.replace(String(MYIPSTR), ipStr);
      MNotFoundPage.replace(String(MSPORTSTR), String(MSSERVERPORT));
      MNotFoundPage.replace(String(MSVERSIONSTR), String(programVersion));
      MNotFoundPage.replace(String(MSNAMESTR), String(programName));
      TRACE();
      DebugPrintln(F(PROCESSPAGEENDSTR));
    }
    else
    {
      TRACE();
      DebugPrintln(F(SPIFFSFILENOTFOUNDSTR));
      MNotFoundPage = "<html><head><title>Management Server></title></head><body><p>URL was not found</p><p><form action=\"/\" method=\"GET\"><input type=\"submit\" value=\"HOMEPAGE\"></form></p></body></html>";
    }
  }
  delay(10);                      // small pause so background tasks can run
}

void MANAGEMENT_handlenotfound(void)
{
  mserver.send(NOTFOUNDWEBPAGE, TEXTPAGETYPE, MNotFoundPage);
}

void MANAGEMENT_buildupload(void)
{
  // load not found page from spiffs - wsupload.html
  if (!SPIFFS.begin())
  {
    TRACE();
    DebugPrintln(F(SPIFFSNOTSTARTEDSTR));
    DebugPrintln(F(BUILDDEFAULTPAGESTR));
    MUploadPage = "<html><head><title>Management Server></title></head><body><p>msupload.html not found</p><p><form action=\"/\" method=\"GET\"><input type=\"submit\" value=\"HOMEPAGE\"></form></p></body></html>";
  }
  else
  {
    if ( SPIFFS.exists("/msupload.html"))
    {
      // open file for read
      File file = SPIFFS.open("/msupload.html", "r");
      // read contents into string
      TRACE();
      DebugPrintln(F(READPAGESTR));
      MUploadPage = file.readString();

      TRACE();
      DebugPrintln(F(PROCESSPAGESTARTSTR));
      // process for dynamic data
      MUploadPage.replace(String(MYIPSTR), ipStr);
      MUploadPage.replace(String(MSPORTSTR), String(MSSERVERPORT));
      MUploadPage.replace(String(MSVERSIONSTR), String(programVersion));
      MUploadPage.replace(String(MSNAMESTR), String(programName));
      TRACE();
      DebugPrintln(F(PROCESSPAGEENDSTR));
    }
    else
    {
      TRACE();
      DebugPrintln(F(SPIFFSFILENOTFOUNDSTR));
      MUploadPage = "<html><head><title>Management Server></title></head><body><p>msupload.html not found</p><p><form action=\"/\" method=\"GET\"><input type=\"submit\" value=\"HOMEPAGE\"></form></p></body></html>";
    }
  }
  delay(10);                      // small pause so background tasks can run
}

void MANAGEMENT_displayfileupload(void)
{
  mserver.send(NORMALWEBPAGE, TEXTPAGETYPE, MUploadPage);
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

void MANAGEMENT_buildhome(void)
{
  // constructs home page of management server
  TRACE();
  // load not found page from spiffs - wsindex.html
  if (!SPIFFS.begin())
  {
    // could not read index file from SPIFFS
    TRACE();
    DebugPrintln(F(SPIFFSNOTSTARTEDSTR));
    DebugPrintln(F(BUILDDEFAULTPAGESTR));
    MHomePage = "<html><head><title>Management Server></title></head><body><p>msindex.html not found</p><p>Did you upload the data files to SPIFFS?</p><p><form action=\"/\" method=\"post\"><input type=\"submit\" name=\"reboot\" value=\"Reboot Controller\"> </form></p></body></html>";
  }
  else
  {
    DebugPrintln(F("management: SPIFFS mounted"));
    if ( SPIFFS.exists("/msindex.html"))
    {
      TRACE();
      DebugPrintln(F(FILEFOUNDSTR));
      // open file for read
      File file = SPIFFS.open("/msindex.html", "r");
      // read contents into string
      DebugPrintln(F(READPAGESTR));
      MHomePage = file.readString();

      DebugPrintln(F(PROCESSPAGESTARTSTR));
      // process for dynamic data
      MHomePage.replace(String(MYIPSTR), ipStr);
      MHomePage.replace(String(MSPORTSTR), String(MSSERVERPORT));
      MHomePage.replace(String(MSVERSIONSTR), String(programVersion));
      MHomePage.replace(String(MSNAMESTR), String(programName));
#ifdef BLUETOOTHMODE
      MHomePage.replace(String(MSMODESTR), "BLUETOOTH : " + String(BLUETOOTHNAME));
#endif
#ifdef ACCESSPOINT
      MHomePage.replace(String(MSMODESTR), "ACCESSPOINT");
#endif
#ifdef STATIONMODE
      MHomePage.replace(String(MSMODESTR), "STATIONMODE");
#endif
#ifdef LOCALSERIAL
      MHomePage.replace(String(MSMODESTR), "LOCALSERIAL");
#endif
      // %WSTATUS%
#ifdef WEBSERVER
      if ( webserverstate == RUNNING )
      {
        MHomePage.replace(String(MSSTATUSSTR), String(SERVERSTATERUNSTR));
      }
      else
      {
        MHomePage.replace(String(MSSTATUSSTR), String(SERVERSTATESTOPSTR));
      }
      MHomePage.replace(String(WSREFRESHSTR), "<form action=\"/\" method =\"post\">Refresh Rate: <input type=\"text\" name= \"wr\" size=\"6\" value=" + String(mySetupData->get_webpagerefreshrate()) + "> <input type=\"submit\" name=\"setwsrate\" value=\"Set\"> </form>");
      MHomePage.replace(String(WSPORTSTR), "<form action=\"/\" method =\"post\">Port: <input type=\"text\" name= \"wp\" size=\"6\" value=" + String(mySetupData->get_webserverport()) + "> <input type=\"submit\" name=\"setwsport\" value=\"Set\">");
      if ( webserverstate == RUNNING)
      {
        MHomePage.replace(String(WSBUTTONSTR), String(STOPWSSTR));
      }
      else
      {
        MHomePage.replace(String(WSBUTTONSTR), String(STARTWSSTR));
      }
#else
      MHomePage.replace(String(MSSTATUSSTR), String(NOTDEFINEDSTR));
      MHomePage.replace(String(WSSREFRESHSTR), "Refresh Rate: " + String(mySetupData->get_webpagerefreshrate()));
      MHomePage.replace(String(WSPORTSTR), "Port: " + String(mySetupData->get_webserverport()));
      MHomePage.replace(String(WSBUTTONSTR), " ");
#endif
#ifdef ASCOMREMOTE
      if ( ascomserverstate == RUNNING )
      {
        MHomePage.replace(String(ASSTATUSSTR), String(SERVERSTATERUNSTR));
      }
      else
      {
        MHomePage.replace(String(ASSTATUSSTR), String(SERVERSTATESTOPSTR));
      }
      MHomePage.replace(String(ASPORTSTR), "<form action=\"/\" method =\"post\">Port: <input type=\"text\" name= \"ap\" size=\"8\" value=" + String(mySetupData->get_ascomalpacaport()) + "> <input type=\"submit\" name=\"setasport\" value=\"Set\">");
      if ( ascomserverstate == RUNNING )
      {
        MHomePage.replace(String(ASBUTTONSTR), String(STOPASSTR));
      }
      else
      {
        MHomePage.replace(String(ASBUTTONSTR), String(STARTASSTR));
      }
#else
      MHomePage.replace(String(ASSTATUSSTR), String(NOTDEFINEDSTR));
      MHomePage.replace(String(ASPORTSTR), "Port: " + String(mySetupData->get_ascomalpacaport()));
      MHomePage.replace(String(ASBUTTONSTR), " ");
#endif

#ifdef OTAUPDATES
      if ( otaupdatestate == RUNNING )
      {
        MHomePage.replace(String(OTASTATUSSTR), String(SERVERSTATERUNSTR));
      }
      else
      {
        MHomePage.replace(String(OTASTATUSSTR), String(SERVERSTATESTOPSTR)));
      }
#else
      MHomePage.replace(String(OTASTATUSSTR), String(NOTDEFINEDSTR));
#endif
#ifdef USEDUCKDNS
      if ( duckdnsstate == RUNNING )
      {
        MHomePage.replace(String(DUCKDNSSTATUSSTR), String(SERVERSTATERUNSTR));
      }
      else
      {
        MHomePage.replace(String(DUCKDNSSTATUSSTR), String(SERVERSTATESTOPSTR)));
      }
#else
      MHomePage.replace(String(DUCKDNSSTATUSSTR), String(NOTDEFINEDSTR));
#endif
      if ( staticip == STATICIPON )
      {
        MHomePage.replace(String(IPSTATUSSTR), "ON");
      }
      else
      {
        MHomePage.replace(String(IPSTATUSSTR), "OFF");
      }
#ifdef MDNSSERVER
      if ( mdnsserverstate == RUNNING)
      {
        MHomePage.replace(String(MDNSSTATUSSTR), String(SERVERSTATERUNSTR));
      }
      else
      {
        MHomePage.replace(String(MDNSSTATUSSTR), String(SERVERSTATESTOPSTR));
      }
      MHomePage.replace(String(MDNSPORTSTR), "<form action=\"/\" method =\"post\">Port: <input type=\"text\" name= \"mdnsp\" size=\"8\" value=" + String(mySetupData->get_mdnsport()) + "> <input type=\"submit\" name=\"setmdnsport\" value=\"Set\">");
      if ( mdnsserverstate == RUNNING)
      {
        MHomePage.replace(String(MDNSBUTTONSTR), String(MDNSTOPSTR));
      }
      else
      {
        MHomePage.replace(String(MDNSBUTTONSTR), MDNSSTARTSTR);
      }
#else
      MHomePage.replace(String(MDNSSTATUSSTR), String(NOTDEFINEDSTR));
      MHomePage.replace(String(MDNSPORTSTR), "Port: " + String(mySetupData->get_mdnsport()));
      MHomePage.replace(String(MDNSBUTTONSTR), " ");
#endif
      // display
#if defined(OLEDTEXT) || defined(OLEDGRAPHICS)
      if ( mySetupData->get_displayenabled() == 1 )
      {
        // checked already
        MHomePage.replace(String(DISPLAYSTR), String(DISPLAYONSTR));
      }
      else
      {
        // not checked
        MHomePage.replace(String(DISPLAYSTR), String(DISPLAYOFFSTR));
      }
#else
      MHomePage.replace(String(DISPLAYSTR), String(NOTDEFINEDSTR));
#endif
      // display heap memory for tracking memory loss?
      // only esp32?
      MHomePage.replace("%HEAPMEMORY%", String(ESP.getFreeHeap()));
      DebugPrintln(F(PROCESSPAGEENDSTR));
    }
    else
    {
      // could not read index file from SPIFFS
      TRACE();
      DebugPrintln(F(BUILDDEFAULTPAGESTR));
      MHomePage = "<html><head><title>Management Server></title></head><body><p>msindex.html not found</p><p>Did you upload the data files to SPIFFS?</p><p><form action=\"/\" method=\"post\"><input type=\"submit\" name=\"reboot\" value=\"Reboot Controller\"></form></p></body></html>";
    }
  }
  delay(10);                      // small pause so background tasks can run
}

void MANAGEMENT_handleroot(void)
{
  // code here to handle a put request
  String msg;

  msg = mserver.arg("reboot");
  if ( msg != "" )
  {
    DebugPrintln(F("MANAGEMENT_handleroot: reboot controller: "));
    // services are stopped in the reboot() code

    String WaitPage = "<html><meta http-equiv=refresh content=\"" + String(MSREBOOTPAGEDELAY) + "\"><head><title>Management Server></title></head><body><p>Please wait, controller rebooting.</p></body></html>";
    mserver.send(NORMALWEBPAGE, TEXTPAGETYPE, WaitPage );
    software_Reboot(REBOOTDELAY);
  }

  msg = mserver.arg("startws");
  if ( msg != "" )
  {
    DebugPrintln(F("MANAGEMENT_handleroot: startws: "));
#ifdef WEBSERVER
    start_webserver();
#endif
  }
  msg = mserver.arg("stopws");
  if ( msg != "" )
  {
    DebugPrintln(F("MANAGEMENT_handleroot: stopws: "));
#ifdef WEBSERVER
    stop_webserver();
#endif
  }

  msg = mserver.arg("startas");
  if ( msg != "" )
  {
    DebugPrintln(F("MANAGEMENT_handleroot: startas: "));
#ifdef ASCOMREMOTE
    start_ascomremoteserver();
#endif
  }
  msg = mserver.arg("stopas");
  if ( msg != "" )
  {
    DebugPrintln(F("MANAGEMENT_handleroot: stopas: "));
#ifdef ASCOMREMOTE
    stop_ascomremoteserver();
#endif
  }

  msg = mserver.arg("startmdns");
  if ( msg != "" )
  {
    DebugPrintln(F("MANAGEMENT_handleroot: startmdns: "));
#ifdef MDNSSERVER
    start_mdns_service();
#endif
  }
  msg = mserver.arg("stopmdns");
  if ( msg != "" )
  {
    DebugPrintln(F("MANAGEMENT_handleroot: stopmdns: "));
#ifdef MDNSSERVER
    stop_mdns_service();
#endif
  }

  msg = mserver.arg("setwsport");
  if ( msg != "" )
  {
    DebugPrint("set web server port: ");
    DebugPrintln(msg);
    String wp = mserver.arg("wp");                      // process new webserver port number
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
          // port is the same so do not bother to change it
          DebugPrintln("wp error: new Port = current port");
        }
        else
        {
          if ( newport == MSSERVERPORT )                              // if same as management server
          {
            DebugPrintln("wp error: new Port = MSSERVERPORT");
          }
          else if ( newport == mySetupData->get_ascomalpacaport() )   // if same as ASCOM REMOTE server
          {
            DebugPrintln("wp error: new Port = ALPACAPORT");
          }
          else if ( newport == MDNSSERVERPORT )                       // if same as mDNS server
          {
            DebugPrintln("wp error: new Port = MDNSSERVERPORT");
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

  msg = mserver.arg("setwsrate");
  if ( msg != "" )
  {
    DebugPrint("set web server page refresh rate: ");
    DebugPrintln(msg);
    String wr = mserver.arg("wr");                      // preocess new webserver page refresh rate
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
        DebugPrintln("wr error: new page refresh rate = current page refresh rate");
      }
      else
      {
        if ( newrate < MINREFRESHPAGERATE )
        {
          DebugPrintln("wr error: Page refresh rate too low");
          newrate = MINREFRESHPAGERATE;
        }
        else if ( newrate > MAXREFRESHPAGERATE )
        {
          DebugPrintln("wr error: Page refresh rate too high");
          newrate = MAXREFRESHPAGERATE;
        }
        DebugPrintln("New page refresh rate = " + String(newrate));
        mySetupData->set_webpagerefreshrate(newrate);                  // assign new refresh rate and save it
      }
    }
  }

  msg = mserver.arg("setasport");
  if ( msg != "" )
  {
    DebugPrint("set ascom server port: ");
    DebugPrintln(msg);
    String ap = mserver.arg("ap");                      // process new ascomalpaca port number
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
            DebugPrintln("wp error: new Port = MSSERVERPORT");
          }
          else if ( newport == mySetupData->get_webserverport() )     // if same as webserver
          {
            DebugPrintln("wp error: new Port = ALPACAPORT");
          }
          else if ( newport == MDNSSERVERPORT )                       // if same as mDNS server
          {
            DebugPrintln("wp error: new Port = MDNSSERVERPORT");
          }
          else
          {
            DebugPrintln("New ascomalpaca port = " + String(newport));
            mySetupData->set_ascomalpacaport(newport);                // assign new port and save it
          }
        }
      }
      else
      {
        DebugPrintln("Attempt to change ascomalpaca port when ascomserver running");
      }
    }
  }

  msg = mserver.arg("setmdnsport");
  if ( msg != "" )
  {
    DebugPrint("set web server port: ");
    DebugPrintln(msg);
    String mp = mserver.arg("mdnsp");                      // process new webserver port number
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
          DebugPrintln("mp error: new Port = current port");
        }
        else
        {
          if ( newport == MSSERVERPORT )                              // if same as management server
          {
            DebugPrintln("mp error: new Port = MSSERVERPORT");
          }
          else if ( newport == mySetupData->get_ascomalpacaport() )   // if same as ASCOM REMOTE server
          {
            DebugPrintln("mp error: new Port = ALPACAPORT");
          }
          else if ( newport == WEBSERVERPORT )                        // if same as web server
          {
            DebugPrintln("mp error: new Port = WEBSERVERPORT");
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
  String d_str = mserver.arg("di");
  if ( d_str != "" )
  {
    DebugPrint("Set display state: ");
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
  MANAGEMENT_buildhome();

  // send the homepage to a connected client
  DebugPrintln("root() - send homepage");
  mserver.send(NORMALWEBPAGE, TEXTPAGETYPE, MHomePage );
  delay(10);                                      // small pause so background ESP8266 tasks can run
}

void start_management(void)
{
  if (!SPIFFS.begin())
  {
    TRACE();
    DebugPrintln(F(SPIFFSNOTSTARTEDSTR));
    DebugPrintln(F(SERVERSTATESTOPSTR));
    managementserverstate = STOPPED;
    return;
  }
  MANAGEMENT_buildnotfound();
  MANAGEMENT_buildhome();
  MANAGEMENT_buildupload();
  mserver.on("/", MANAGEMENT_handleroot);
  mserver.on("/list", MANAGEMENT_listSPIFFSfiles);
  mserver.on("/upload", HTTP_GET, MANAGEMENT_displayfileupload);
  mserver.on("/upload", HTTP_POST, []() {
    mserver.send(NORMALWEBPAGE);
  }, MANAGEMENT_handlefileupload );
  mserver.onNotFound([]() {                         // if the client requests any URI
    if (!MANAGEMENT_handlefileread(mserver.uri()))  // send file if it exists
    {
      MANAGEMENT_handlenotfound();                  // otherwise, respond with a 404 (Not Found) error
    }
  });
  mserver.begin();
  managementserverstate = RUNNING;
  TRACE();
  DebugPrintln(F(SERVERSTATESTARTSTR));
  delay(10);                      // small pause so background tasks can run
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
// 26: WEBSERVER - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#ifdef WEBSERVER

#if defined(ESP8266)
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif // if defined(esp8266)

#include "webserver.h"
#if defined(ESP8266)
ESP8266WebServer *webserver;
#else
WebServer *webserver;
#endif // if defined(esp8266)
String WNotFoundPage;
String WHomePage;
String WMovePage;
String WPresetsPage;

void WEBSERVER_buildnotfound(void)
{
  // load not found page from spiffs - wsnotfound.html
  if (!SPIFFS.begin())
  {
    TRACE();
    DebugPrintln(F(SPIFFSNOTSTARTEDSTR));
    DebugPrintln(F(BUILDDEFAULTPAGESTR));
    WNotFoundPage = "<html><head><title>Web Server: Not found></title></head><body>";
    WNotFoundPage = WNotFoundPage + "<p>The requested URL was not found</p>";
    WNotFoundPage = WNotFoundPage + "<p><form action=\"/\" method=\"GET\"><input type=\"submit\" value=\"HOMEPAGE\"></form></p>";
    WNotFoundPage = WNotFoundPage + "</body></html>";
  }
  else
  {
    if ( SPIFFS.exists("/wsnotfound.html"))
    {
      DebugPrintln("webserver: wsnotfound.html found in spiffs");
      // open file for read
      File file = SPIFFS.open("/wsnotfound.html", "r");
      // read contents into string
      DebugPrintln(F(READPAGESTR));
      WNotFoundPage = file.readString();

      DebugPrintln(F(PROCESSPAGESTARTSTR));
      // process for dynamic data
      WNotFoundPage.replace(String(MYIPSTR), ipStr);
      WNotFoundPage.replace(String(WSPORTSTR), String(mySetupData->get_webserverport()));
      WNotFoundPage.replace(String(WSVERSIONSTR), String(programVersion));
      WNotFoundPage.replace(String(WSNAMESTR), String(programName));
      DebugPrintln(F(PROCESSPAGEENDSTR));
    }
    else
    {
      TRACE();
      DebugPrintln(F(SPIFFSFILENOTFOUNDSTR));
      DebugPrintln(F(BUILDDEFAULTPAGESTR));
      WNotFoundPage = "<html><head><title>Web Server: Not found></title></head><body>";
      WNotFoundPage = WNotFoundPage + "<p>The requested URL was not found</p>";
      WNotFoundPage = WNotFoundPage + "<p><form action=\"/\" method=\"GET\"><input type=\"submit\" value=\"HOMEPAGE\"></form></p>";
      WNotFoundPage = WNotFoundPage + "</body></html>";
    }
  }
  delay(10);                      // small pause so background tasks can run
}

void WEBSERVER_handlenotfound(void)
{
  webserver->send(NOTFOUNDWEBPAGE, TEXTPAGETYPE, WNotFoundPage);
}

void WEBSERVER_buildpresets(void)
{
  // construct the presetspage now
  // load not found page from spiffs - wspresets.html
  if (!SPIFFS.begin())
  {
    TRACE();
    DebugPrintln(F(SPIFFSNOTSTARTEDSTR));
    // could not read move file from SPIFFS
    DebugPrintln(F(BUILDDEFAULTPAGESTR));
    WPresetsPage = "<html><head><title>Web Server:></title></head><body><p>wspresets.html not found</p><p>Did you upload the data files to SPIFFS?</p></body></html>";
  }
  else
  {
    if ( SPIFFS.exists("/wspresets.html"))
    {
      DebugPrintln("webserver: wspresets.html found in spiffs");
      // open file for read
      File file = SPIFFS.open("/wspresets.html", "r");
      // read contents into string
      DebugPrintln(F(READPAGESTR));
      WPresetsPage = file.readString();

      DebugPrintln(F(PROCESSPAGESTARTSTR));
      // process for dynamic data
      WPresetsPage.replace(String(WSREFRESHSTR), String(mySetupData->get_webpagerefreshrate()));
      WPresetsPage.replace(String(MYIPSTR), ipStr);
      WPresetsPage.replace(String(WSPORTSTR), String(mySetupData->get_webserverport()));
      WPresetsPage.replace(String(WSVERSIONSTR), String(programVersion));
      WPresetsPage.replace(String(WSNAMESTR), String(programName));
      WPresetsPage.replace(String(WSCURRENTSTR), String(fcurrentPosition));
      WPresetsPage.replace(String(WSTARGETSTR), String(ftargetPosition));
      WPresetsPage.replace(String(WSISMOVINGSTR), String(isMoving));

      WPresetsPage.replace("%WSP0BUFFER%", String(mySetupData->get_focuserpreset(0)));
      WPresetsPage.replace("%WSP1BUFFER%", String(mySetupData->get_focuserpreset(1)));
      WPresetsPage.replace("%WSP2BUFFER%", String(mySetupData->get_focuserpreset(2)));
      WPresetsPage.replace("%WSP3BUFFER%", String(mySetupData->get_focuserpreset(3)));
      WPresetsPage.replace("%WSP4BUFFER%", String(mySetupData->get_focuserpreset(4)));
      WPresetsPage.replace("%WSP5BUFFER%", String(mySetupData->get_focuserpreset(5)));
      WPresetsPage.replace("%WSP6BUFFER%", String(mySetupData->get_focuserpreset(6)));
      WPresetsPage.replace("%WSP7BUFFER%", String(mySetupData->get_focuserpreset(7)));
      WPresetsPage.replace("%WSP8BUFFER%", String(mySetupData->get_focuserpreset(8)));
      WPresetsPage.replace("%WSP9BUFFER%", String(mySetupData->get_focuserpreset(9)));
      DebugPrintln(F(PROCESSPAGEENDSTR));
    }
    else
    {
      // could not read preset file from SPIFFS
      TRACE();
      DebugPrintln(F(SPIFFSFILENOTFOUNDSTR));
      DebugPrintln(F(BUILDDEFAULTPAGESTR));
      WPresetsPage = "<html><head><title>Web Server:></title></head><body><p>wspresets.html not found</p><p>Did you upload the data files to SPIFFS?</p></body></html>";
    }
  }
  delay(10);                      // small pause so background tasks can run
}

void WEBSERVER_handlepresets(void)
{
  // if the root page was a HALT request via Submit button
  String halt_str = webserver->arg("ha");
  if ( halt_str != "" )
  {
    TRACE();
    DebugPrintln(halt_str);
    ftargetPosition = fcurrentPosition;
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
#if defined(JOYSTICK1) || defined(JOYSTICK2)
      // restore motorspeed just in case
      driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif
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
#if defined(JOYSTICK1) || defined(JOYSTICK2)
      // restore motorspeed just in case
      driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif
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
#if defined(JOYSTICK1) || defined(JOYSTICK2)
      // restore motorspeed just in case
      driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif
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
#if defined(JOYSTICK1) || defined(JOYSTICK2)
      // restore motorspeed just in case
      driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif
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
#if defined(JOYSTICK1) || defined(JOYSTICK2)
      // restore motorspeed just in case
      driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif
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
#if defined(JOYSTICK1) || defined(JOYSTICK2)
      // restore motorspeed just in case
      driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif
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
#if defined(JOYSTICK1) || defined(JOYSTICK2)
      // restore motorspeed just in case
      driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif
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
#if defined(JOYSTICK1) || defined(JOYSTICK2)
      // restore motorspeed just in case
      driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif
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
#if defined(JOYSTICK1) || defined(JOYSTICK2)
      // restore motorspeed just in case
      driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif
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
#if defined(JOYSTICK1) || defined(JOYSTICK2)
      // restore motorspeed just in case
      driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif
    }
  }

  WEBSERVER_buildpresets();
  // send the presetspage to a connected client
  DebugPrintln(F(SENDPAGESTR));
  webserver->send(NORMALWEBPAGE, TEXTPAGETYPE, WPresetsPage );
  delay(10);                     // small pause so background ESP8266 tasks can run
}

void WEBSERVER_buildmove(void)
{
  // construct the movepage now
  // load not found page from spiffs - wsmove.html
  if (!SPIFFS.begin())
  {
    TRACE();
    DebugPrintln(F(SPIFFSNOTSTARTEDSTR));
    // could not read move file from SPIFFS
    DebugPrintln(F(BUILDDEFAULTPAGESTR));
    WMovePage = "<html><head><title>Web Server:></title></head><body><p>wsmove.html not found</p><p>Did you upload the data files to SPIFFS?</p></body></html>";
  }
  else
  {
    if ( SPIFFS.exists("/wsmove.html"))
    {
      DebugPrintln("webserver: wsmove.html found in spiffs");
      // open file for read
      File file = SPIFFS.open("/wsmove.html", "r");
      // read contents into string
      DebugPrintln(F(READPAGESTR));
      WMovePage = file.readString();

      DebugPrintln(F(PROCESSPAGESTARTSTR));
      // process for dynamic data
      WMovePage.replace(String(WSREFRESHSTR), String(mySetupData->get_webpagerefreshrate()));
      WMovePage.replace(String(MYIPSTR), ipStr);
      WMovePage.replace(String(WSPORTSTR), String(mySetupData->get_webserverport()));
      WMovePage.replace(String(WSVERSIONSTR), String(programVersion));
      WMovePage.replace(String(WSNAMESTR), String(programName));
      WMovePage.replace(String(WSCURRENTSTR), String(fcurrentPosition));
      WMovePage.replace(String(WSTARGETSTR), String(ftargetPosition));
      WMovePage.replace(String(WSISMOVINGSTR), String(isMoving));
      DebugPrintln(F(PROCESSPAGEENDSTR));
    }
    else
    {
      // could not read move file from SPIFFS
      TRACE();
      DebugPrintln(F(SPIFFSFILENOTFOUNDSTR));
      DebugPrintln(F(BUILDDEFAULTPAGESTR));
      WMovePage = "<html><head><title>Web Server:></title></head><body><p>wsmove.html not found</p><p>Did you upload the data files to SPIFFS?</p></body></html>";
    }
  }
  delay(10);                      // small pause so background tasks can run
}

// handles move page of webserver
// this is called whenever a client requests move
void WEBSERVER_handlemove()
{
  // if the root page was a HALT request via Submit button
  String halt_str = webserver->arg("ha");
  if ( halt_str != "" )
  {
    TRACE();
    DebugPrintln(halt_str);
    ftargetPosition = fcurrentPosition;
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
#if defined(JOYSTICK1) || defined(JOYSTICK2)
    // restore motorspeed just in case
    driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif
  }

  WEBSERVER_buildmove();
  // send the movepage to a connected client
  DebugPrintln(F(SENDPAGESTR));
  webserver->send(NORMALWEBPAGE, TEXTPAGETYPE, WMovePage );
  delay(10);                     // small pause so background ESP8266 tasks can run
}

void WEBSERVER_buildhome(void)
{
  // construct the homepage now
  // load not found page from spiffs - wsindex.html
  if (!SPIFFS.begin())
  {
    TRACE();
    DebugPrintln(F(SPIFFSNOTSTARTEDSTR));
    // could not read index file from SPIFFS
    DebugPrintln(BUILDDEFAULTPAGESTR);
    WHomePage = "<html><head><title>Web Server:></title></head><body><p>wsindex.html not found</p><p>Did you upload the data files to SPIFFS?</p></body></html>";
  }
  else
  {
    if ( SPIFFS.exists("/wsindex.html"))
    {
      DebugPrintln("webserver: wsindex.html found in spiffs");
      // open file for read
      File file = SPIFFS.open("/wsindex.html", "r");
      // read contents into string
      DebugPrintln(F(READPAGESTR));
      WHomePage = file.readString();

      DebugPrintln(F(PROCESSPAGESTARTSTR));
      // process for dynamic data
      WHomePage.replace(String(WSREFRESHSTR), String(mySetupData->get_webpagerefreshrate()));
      WHomePage.replace(String(MYIPSTR), ipStr);
      WHomePage.replace(String(WSPORTSTR), String(mySetupData->get_webserverport()));
      WHomePage.replace(String(WSVERSIONSTR), String(programVersion));
      WHomePage.replace(String(WSNAMESTR), String(programName));
      // if this is a GOTO command then make this target else make current
      String fp_str = webserver->arg("gotopos");
      if ( fp_str != "" )
      {
        WHomePage.replace(String(WSCURRENTSTR), String(ftargetPosition));
      }
      else
      {
        WHomePage.replace(String(WSCURRENTSTR), String(fcurrentPosition));
      }
      WHomePage.replace(String(WSTARGETSTR), String(ftargetPosition));
      WHomePage.replace(String(WSMAXSTEPSTR), String(mySetupData->get_maxstep()));
      WHomePage.replace(String(WSISMOVINGSTR), String(isMoving));
#ifdef TEMPERATUREPROBE
      if ( mySetupData->get_tempmode() == 1)
      {
        WHomePage.replace("%WSTEMPERATURE%", String(read_temp(1), 2));
      }
      else
      {
        float ft = read_temp(1);
        ft = (ft * 1.8) + 32;
        WHomePage.replace("%WSTEMPERATURE%", String(ft, 2));
      }
#else
      if ( mySetupData->get_tempmode() == 1)
      {
        WHomePage.replace("%WSTEMPERATURE%", "20.00");
      }
      else
      {
        WHomePage.replace("%WSTEMPERATURE%", "68.00");
      }
#endif
      WHomePage.replace("%WSTEMPPRECISION%", String(mySetupData->get_tempprecision()));
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
      WHomePage.replace("%WSSMBUFFER%", smbuffer);
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
      WHomePage.replace("%WSMSBUFFER%", msbuffer);
      String cpbuffer;
      if ( !mySetupData->get_coilpower() )
      {
        cpbuffer = "<input type=\"checkbox\" name=\"cp\" value=\"cp\" > ";
      }
      else
      {
        cpbuffer = "<input type=\"checkbox\" name=\"cp\" value=\"cp\" Checked> ";
      }
      WHomePage.replace("%WSCPBUFFER%", cpbuffer);
      String rdbuffer;
      if ( !mySetupData->get_reversedirection() )
      {
        rdbuffer = "<input type=\"checkbox\" name=\"rd\" value=\"rd\" > ";
      }
      else
      {
        rdbuffer = "<input type=\"checkbox\" name=\"rd\" value=\"rd\" Checked> ";
      }
      WHomePage.replace("%WSRDBUFFER%", rdbuffer);
      // display
#if defined(OLEDTEXT) || defined(OLEDGRAPHICS)
      if ( mySetupData->get_displayenabled() == 1 )
      {
        // checked already
        WHomePage.replace(String(DISPLAYSTR), String(DISPLAYONSTR));
      }
      else
      {
        // not checked
        WHomePage.replace(String(DISPLAYSTR), String(DISPLAYOFFSTR));
      }
#else
      WHomePage.replace(String(DISPLAYSTR), "Display " + String(NOTDEFINEDSTR));
#endif
      DebugPrintln(F(PROCESSPAGEENDSTR));
    }
    else
    {
      // could not read index file from SPIFFS
      TRACE();
      DebugPrintln(F(SPIFFSFILENOTFOUNDSTR));
      DebugPrintln(F(BUILDDEFAULTPAGESTR));
      WHomePage = "<html><head><title>Web Server:></title></head><body><p>wsindex.html</p><p>Did you upload the data files to SPIFFS?</p></body></html>";
    }
  }
  delay(10);                      // small pause so background tasks can run
}

void WEBSERVER_handleposition()
{
  webserver->send(200, "text/plane", String(fcurrentPosition)); //Send position value only to client ajax request
}

void WEBSERVER_handleismoving()
{
  webserver->send(200, "text/plane", String(isMoving)); //Send isMoving value only to client ajax request
}

// handles root page of webserver
// this is called whenever a client requests home page of sebserver
void WEBSERVER_handleroot()
{
  // if the root page was a HALT request via Submit button
  String halt_str = webserver->arg("ha");
  if ( halt_str != "" )
  {
    DebugPrint("root() -halt:");
    DebugPrintln(halt_str);
    ftargetPosition = fcurrentPosition;
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
#if defined(JOYSTICK1) || defined(JOYSTICK2)
      // restore motorspeed just in case
      driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif
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
    if ( temp < (long) fcurrentPosition )             // if maxstep is less than focuser position
    {
      temp = (long) fcurrentPosition + 1;
    }
    if ( temp < FOCUSERLOWERLIMIT )                   // do not set it less then 1024
    {
      temp = FOCUSERLOWERLIMIT;
    }
    if ( temp > (long) mySetupData->get_maxstep() )   // if higher than max value
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

  WEBSERVER_buildhome();
  // send the homepage to a connected client
  DebugPrintln(F(SENDPAGESTR));
  webserver->send(NORMALWEBPAGE, TEXTPAGETYPE, WHomePage );
  delay(10);                     // small pause so background ESP8266 tasks can run
}

void start_webserver(void)
{
  DebugPrintln(F(STARTWEBSERVERSTR));
#if defined(ESP8266)
  webserver = new ESP8266WebServer(mySetupData->get_webserverport());
#else
  webserver = new WebServer(mySetupData->get_webserverport());
#endif // if defined(esp8266) 
  WEBSERVER_buildnotfound();
  WEBSERVER_buildhome();
  WEBSERVER_buildmove();
  WEBSERVER_buildpresets();
  webserver->on("/", WEBSERVER_handleroot);
  webserver->on("/move", WEBSERVER_handlemove);
  webserver->on("/presets", WEBSERVER_handlepresets);
  webserver->on("/position", WEBSERVER_handleposition);
  webserver->on("/ismoving", WEBSERVER_handleismoving);
  webserver->onNotFound(WEBSERVER_handlenotfound);
  webserver->begin();
  webserverstate = RUNNING;
  DebugPrintln(F(SERVERSTATESTARTSTR));
  delay(10);                      // small pause so background tasks can run
}

void stop_webserver(void)
{
  if ( webserverstate == RUNNING )
  {
    webserver->close();
    delete webserver;            // free the webserver pointer and associated memory/code
    webserverstate = STOPPED;
    TRACE();
    DebugPrintln(F(SERVERSTATESTOPSTR));
  }
  else
  {
    DebugPrintln(F(SERVERNOTRUNNINGSTR));
  }
  delay(10);                      // small pause so background tasks can run
}
// WEBSERVER END ------------------------------------------------------------------------------------
#endif // #ifdef WEBSERVER

// ----------------------------------------------------------------------------------------------
// 27: ASCOMSERVER - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#ifdef ASCOMREMOTE
#if defined(ESP8266)
#include <ESP8266WebServer.h>
#else
#include <webserver.h>
#endif // if defined(esp8266)

#include "ascomserver.h"

String Focuser_Setup_HomePage;    //  url:/setup/v1/focuser/0/setup
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
ESP8266WebServer *ascomserver;
#else
WebServer *ascomserver;
#endif // if defined(esp8266)

// Construct pages for /setup

// constructs ascom setup server page /setup/v1/focuser/0/setup
void ASCOM_Create_Setup_Focuser_HomePage()
{
  // Convert IP address to a string;
  // already in ipStr
  // convert current values of focuserposition and focusermaxsteps to string types
  String fpbuffer = String(fcurrentPosition);
  String mxbuffer = String(mySetupData->get_maxstep());
  String smbuffer = String(mySetupData->get_stepmode());
  int eflag = 0;

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
  if (SPIFFS.begin())
  {
    if ( SPIFFS.exists("/assetup.html"))
    {
      DebugPrintln("ascomserver: assetup.html found in spiffs");
      // open file for read
      File file = SPIFFS.open("/assetup.html", "r");
      // read contents into string
      DebugPrintln("ascomserver: read page into string");
      Focuser_Setup_HomePage = file.readString();

      DebugPrintln("ascomserver: processing page start");
      // process for dynamic data
      Focuser_Setup_HomePage.replace("%PROGRAMNAME%", String(programName));
      Focuser_Setup_HomePage.replace("%IPSTR%", ipStr);
      Focuser_Setup_HomePage.replace("%ALPACAPORT%", String(mySetupData->get_ascomalpacaport()));
      Focuser_Setup_HomePage.replace("%PROGRAMVERSION%", String(programVersion));
      Focuser_Setup_HomePage.replace("%FPBUFFER%", fpbuffer);
      Focuser_Setup_HomePage.replace("%MXBUFFER%", mxbuffer);
      Focuser_Setup_HomePage.replace("%CPBUFFER%", cpbuffer);
      Focuser_Setup_HomePage.replace("%RDBUFFER%", rdbuffer);
      Focuser_Setup_HomePage.replace("%SMBUFFER%", smbuffer);
      Focuser_Setup_HomePage.replace("%MSBUFFER%", msbuffer);
      DebugPrintln("ascomserver: processing page done");
      eflag = 0;
    }
    else
    {
      // assetup.html not found
      eflag = 1;
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
    Focuser_Setup_HomePage = "<head>" + String(AS_PAGETITLE) + "</head><body>";
    Focuser_Setup_HomePage = Focuser_Setup_HomePage + String(AS_TITLE);

    Focuser_Setup_HomePage = Focuser_Setup_HomePage + String(AS_COPYRIGHT);
    Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<p>Driverboard = myFP2ESP." + programName + "<br>";
    Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<myFP2ESP." + programName + "</h3>IP Address: " + ipStr + ", Firmware Version=" + String(programVersion) + "</br>";

    // position. set position
    Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><br><b>Focuser Position</b> <input type=\"text\" name=\"fp\" size =\"15\" value=" + fpbuffer + "> ";
    Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<input type=\"submit\" name=\"setpos\" value=\"Set Pos\"> </form></p>";

    // maxstep
    Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>MaxSteps</b> <input type=\"text\" name=\"fm\" size =\"15\" value=" + mxbuffer + "> ";
    Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<input type=\"submit\" value=\"Submit\"></form>";

    // coilpower
    Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>Coil Power </b>" + cpbuffer ;
    Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<input type=\"hidden\" name=\"cp\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

    // reverse direction
    Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>Reverse Direction </b>" + rdbuffer ;
    Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<input type=\"hidden\" name=\"rd\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

    // stepmode
    Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>Step Mode </b>" + smbuffer + " ";
    Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<input type=\"hidden\" name=\"sm\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

    // motor speed
    Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>Motor Speed: </b>" + msbuffer + " ";
    Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<input type=\"hidden\" name=\"ms\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

    Focuser_Setup_HomePage = Focuser_Setup_HomePage + "</body></html>\r\n";
  }
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
    DebugPrint("Paramter Found: ");
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
      ASCOMpos = ascomserver->arg(i).toInt();      // this returns a long data type
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

// ----------------------------------------------------------------------------------------------
// Setup functions
// ----------------------------------------------------------------------------------------------
void ASCOM_handle_setup()
{
  // url /setup
  // The web page must describe the overall device, including name, manufacturer and version number.
  // content-type: text/html
  String AS_HomePage;
  // read ashomepage.html from SPIFFS
  if (!SPIFFS.begin())
  {
    DebugPrintln(F("ascomserver: Error occurred when mounting SPIFFS"));
    DebugPrintln("ascomserver: build_default_homepage");
    AS_HomePage = "<html><head><title>ASCOM REMOTE SERVER: Not found></title></head><body>";
    AS_HomePage = AS_HomePage + "<p>SPIFFS could not be started</p>";
    AS_HomePage = AS_HomePage + "<p><p><a href=\"/setup/v1/focuser/0/setup\">Setup page</a></p>";
    AS_HomePage = AS_HomePage + "</body></html>";
  }
  else
  {
    if ( SPIFFS.exists("/ashomepage.html"))
    {
      DebugPrintln("ascomserver: ashomepage.html found in spiffs");
      // open file for read
      File file = SPIFFS.open("/ashomepage.html", "r");
      // read contents into string
      DebugPrintln("ascomserver: read page into string");
      AS_HomePage = file.readString();

      DebugPrintln("ascomserver: processing page start");
      // process for dynamic data
      AS_HomePage.replace("%IPSTR%", ipStr);
      AS_HomePage.replace("%ALPACAPORT%", String(mySetupData->get_ascomalpacaport()));
      AS_HomePage.replace("%PROGRAMVERSION%", String(programVersion));
      AS_HomePage.replace("%PROGRAMNAME%", String(programName));
      DebugPrintln("ascomserver: processing page done");
    }
    else
    {
      DebugPrintln(F("ascomserver: Error occurred finding SPIFFS file ashomepage.html"));
      DebugPrintln("ascomserver: build_default_homepage");
      AS_HomePage = "<html><head><title>ASCOM REMOTE SERVER: Not found></title></head><body>";
      AS_HomePage = AS_HomePage + "<p>SPIFFS could not be started</p>";
      AS_HomePage = AS_HomePage + "<p><p><a href=\"/setup/v1/focuser/0/setup\">Setup page</a></p>";
      AS_HomePage = AS_HomePage + "</body></html>";
    }
  }
  ASCOMServerTransactionID++;
  ASCOM_sendreply( NORMALWEBPAGE, TEXTPAGETYPE, AS_HomePage);
  delay(10);                     // small pause so background tasks can run
}

void ASCOM_handle_focuser_setup()
{
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
    if ( temp < (long) fcurrentPosition )             // if maxstep is less than focuser position
    {
      temp = (long) fcurrentPosition + 1;
    }
    if ( temp < FOCUSERLOWERLIMIT )                   // do not set it less then 1024
    {
      temp = FOCUSERLOWERLIMIT;
    }
    if ( temp > (long) mySetupData->get_maxstep() )   // if higher than max value
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

  // construct the homepage now
  ASCOM_Create_Setup_Focuser_HomePage();

  // send the homepage to a connected client
  ASCOMServerTransactionID++;
  DebugPrintln("root() - send homepage");
  ASCOM_sendreply( NORMALWEBPAGE, TEXTPAGETYPE, Focuser_Setup_HomePage);
  delay(10);                     // small pause so background ESP8266 tasks can run
}

// ----------------------------------------------------------------------------------------------
// Management API functions
// ----------------------------------------------------------------------------------------------
void ASCOM_handleapiversions()
{
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
}

void ASCOM_handleapidescription()
{
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
  jsonretstr = "{\"Value\":[{\"DeviceName\":" + String(ASCOMNAME) + ",\"DeviceType\":\"focuser\",\"DeviceNumber\":0,\"UniqueID\":\"7e239e71-d304-4e7e-acda-3ff2e2b68515\"}]," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

// ----------------------------------------------------------------------------------------------
// ASCOM ALPACA API
// ----------------------------------------------------------------------------------------------
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
#ifdef TEMPERATUREPROBE
  jsonretstr = "{\"Value\":" + String(read_temp(0)) + "," + ASCOM_addclientinfo( jsonretstr );
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
#if defined(JOYSTICK1) || defined(JOYSTICK2)
  // restore motorspeed just in case
  driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif
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
  delay(10);                     // small pause so background tasks can run
}

void ASCOM_handleRoot()
{
  String AS_HomePage;
  // read ashomepage.html from SPIFFS
  if (!SPIFFS.begin())
  {
    DebugPrintln(F("ascomserver: Error occurred when mounting SPIFFS"));
    DebugPrintln("ascomserver: build_default_homepage");
    AS_HomePage = "<html><head><title>ASCOM REMOTE SERVER: Not found></title></head><body>";
    AS_HomePage = AS_HomePage + "<p>SPIFFS could not be started</p>";
    AS_HomePage = AS_HomePage + "<p><p><a href=\"/setup/v1/focuser/0/setup\">Setup page</a></p>";
    AS_HomePage = AS_HomePage + "</body></html>";
  }
  else
  {
    if ( SPIFFS.exists("/ashomepage.html"))
    {
      DebugPrintln("ascomserver: ashomepage.html found in spiffs");
      // open file for read
      File file = SPIFFS.open("/ashomepage.html", "r");
      // read contents into string
      DebugPrintln("ascomserver: read page into string");
      AS_HomePage = file.readString();

      DebugPrintln("ascomserver: processing page start");
      // process for dynamic data
      AS_HomePage.replace("%IPSTR%", ipStr);
      AS_HomePage.replace("%ALPACAPORT%", String(mySetupData->get_ascomalpacaport()));
      AS_HomePage.replace("%PROGRAMVERSION%", String(programVersion));
      AS_HomePage.replace("%PROGRAMNAME%", String(programName));
      DebugPrintln("ascomserver: processing page done");
    }
    else
    {
      DebugPrintln(F("ascomserver: Error occurred finding SPIFFS file ashomepage.html"));
      DebugPrintln("ascomserver: build_default_homepage");
      AS_HomePage = "<html><head><title>ASCOM REMOTE SERVER: Not found></title></head><body>";
      AS_HomePage = AS_HomePage + "<p>SPIFFS could not be started</p>";
      AS_HomePage = AS_HomePage + "<p><p><a href=\"/setup/v1/focuser/0/setup\">Setup page</a></p>";
      AS_HomePage = AS_HomePage + "</body></html>";
    }
  }
  ASCOMServerTransactionID++;
  ASCOM_sendreply( NORMALWEBPAGE, TEXTPAGETYPE, AS_HomePage);
  delay(10);                     // small pause so background tasks can run
}

void start_ascomremoteserver(void)
{
  DebugPrintln("start ascom server");
  ASCOM_Create_Setup_Focuser_HomePage();        // create home page for ascom management setup api

#if defined(ESP8266)
  ascomserver = new ESP8266WebServer(mySetupData->get_ascomalpacaport());
#else
  ascomserver = new WebServer(mySetupData->get_ascomalpacaport());
#endif // if defined(esp8266) 

  ascomserver->on("/", ASCOM_handleRoot);        // handle root access
  ascomserver->onNotFound(ASCOM_handleNotFound); // handle url not found 404

  ascomserver->on("/management/apiversions", ASCOM_handleapiversions);
  ascomserver->on("/management/v1/description", ASCOM_handleapidescription);
  ascomserver->on("/management/v1/configureddevices", ASCOM_handleapiconfigureddevices);

  ascomserver->on("/setup", ASCOM_handle_setup);
  ascomserver->on("/setup/v1/focuser/0/setup", ASCOM_handle_focuser_setup);

  ascomserver->on("/api/v1/focuser/0/connected", HTTP_PUT, ASCOM_handleconnectedput);
  ascomserver->on("/api/v1/focuser/0/interfaceversion", HTTP_GET, ASCOM_handleinterfaceversionget);
  ascomserver->on("/api/v1/focuser/0/name", HTTP_GET, ASCOM_handlenameget);
  ascomserver->on("/api/v1/focuser/0/description", HTTP_GET, ASCOM_handledescriptionget);
  ascomserver->on("/api/v1/focuser/0/driverinfo", HTTP_GET, ASCOM_handledriverinfoget);
  ascomserver->on("/api/v1/focuser/0/driverversion", HTTP_GET, ASCOM_handledriverversionget);
  ascomserver->on("/api/v1/focuser/0/absolute", HTTP_GET, ASCOM_handleabsoluteget);
  ascomserver->on("/api/v1/focuser/0/maxstep", HTTP_GET, ASCOM_handlemaxstepget);
  ascomserver->on("/api/v1/focuser/0/maxincrement", HTTP_GET, ASCOM_handlemaxincrementget);
  ascomserver->on("/api/v1/focuser/0/temperature", HTTP_GET, ASCOM_handletemperatureget);
  ascomserver->on("/api/v1/focuser/0/position", HTTP_GET, ASCOM_handlepositionget);
  ascomserver->on("/api/v1/focuser/0/halt", HTTP_PUT, ASCOM_handlehaltput);
  ascomserver->on("/api/v1/focuser/0/ismoving", HTTP_GET, ASCOM_handleismovingget);
  ascomserver->on("/api/v1/focuser/0/stepsize", HTTP_GET, ASCOM_handlestepsizeget);
  ascomserver->on("/api/v1/focuser/0/connected", HTTP_GET, ASCOM_handleconnectedget);
  ascomserver->on("/api/v1/focuser/0/tempcomp", HTTP_GET, ASCOM_handletempcompget);
  ascomserver->on("/api/v1/focuser/0/tempcomp", HTTP_PUT, ASCOM_handletempcompput);
  ascomserver->on("/api/v1/focuser/0/tempcompavailable", HTTP_GET, ASCOM_handletempcompavailableget);
  ascomserver->on("/api/v1/focuser/0/move", HTTP_PUT, ASCOM_handlemoveput);
  ascomserver->on("/api/v1/focuser/0/supportedactions", HTTP_GET, ASCOM_handlesupportedactionsget);
  ascomserver->begin();
  ascomserverstate = RUNNING;
  delay(10);                     // small pause so background tasks can run
  DebugPrintln(F("start ascom server: RUNNING"));
}

void stop_ascomremoteserver(void)
{
  if ( ascomserverstate == RUNNING )
  {
    DebugPrintln("stop ascom server");
    ascomserver->close();
    delete ascomserver;            // free the ascomserver pointer and associated memory/code
    ascomserverstate = STOPPED;
  }
  else
  {
    DebugPrintln(F(SERVERNOTRUNNINGSTR));
  }
  delay(10);                     // small pause so background tasks can run
}
#endif // ifdef ASCOMREMOTE
// ASCOM REMOTE END -----------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// 28: OTAUPDATES - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#if defined(OTAUPDATES)
#include <ArduinoOTA.h>

void start_otaservice()
{
  DebugPrintln(F(STARTOTASERVICESTR));
  oledtextmsg(STARTOTASERVICESTR, -1, false, true);
  ArduinoOTA.setHostname(OTAName);                      // Start the OTA service
  ArduinoOTA.setPassword(OTAPassword);

  ArduinoOTA.onStart([]()
  {
    DebugPrintln(F(STARTSTR));
  });
  ArduinoOTA.onEnd([]()
  {
    DebugPrintln(F(ENDSTR));
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
    DebugPrint(F(PROGRESSSTR));
    DebugPrintln((progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error)
  {
    DebugPrint(F(ERRORSTR));
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
  DebugPrintln(F(READYSTR));
  otaupdatestate = RUNNING;
}
#endif // #if defined(OTAUPDATES)

// ----------------------------------------------------------------------------------------------
// 29: DUCKDNS - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#ifdef USEDUCKDNS
#include <EasyDDNS.h>                           // https://github.com/ayushsharma82/EasyDDNS

void init_duckdns(void)
{
  DebugPrintln(F(SETUPDUCKDNSSTR));
  oledtextmsg(SETUPDUCKDNSSTR, -1, false, true);
  EasyDDNS.service("duckdns");                  // Enter your DDNS Service Name - "duckdns" / "noip"
  delay(5);
  EasyDDNS.client(duckdnsdomain, duckdnstoken); // Enter ddns Domain & Token | Example - "esp.duckdns.org","1234567"
  delay(5);
  EasyDDNS.update(DUCKDNS_REFREHRATE);          // Check for New Ip Every 60 Seconds.
  delay(5);
  duckdnsstate = RUNNING;
}
#endif // #ifdef USEDUCKSDNS

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

void software_Reboot(int Reboot_delay)
{
  oledtextmsg(WIFIRESTARTSTR, -1, true, false);
#ifdef MDNSSERVER
  stop_mdns_service();
#endif
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

// STEPPER MOTOR ROUTINES
void steppermotormove(byte dir )                // direction move_in, move_out ^ reverse direction
{
#ifdef INOUTLEDS
  ( dir == move_in ) ? digitalWrite(INLEDPIN, 1) : digitalWrite(OUTLEDPIN, 1);
#endif
  driverboard->movemotor(dir);
#ifdef INOUTLEDS
  ( dir == move_in ) ? digitalWrite(INLEDPIN, 0) : digitalWrite(OUTLEDPIN, 0);
#endif
}

bool readwificonfig( char* xSSID, char* xPASSWORD)
{
  const String filename = "/wificonfig.json";
  String SSID;
  String PASSWORD;
  boolean status = false;

  DebugPrintln(F(CHECKWIFICONFIGFILESTR));
  File f = SPIFFS.open(filename, "r");                          // file open to read
  if (!f)
  {
    TRACE();
    DebugPrintln(F(FILENOTFOUNDSTR));
  }
  else
  {
    String data = f.readString();                               // read content of the text file
    DebugPrint(F("Wifi Config data: "));
    DebugPrintln(data);                                         // ... and print on serial

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

void setup()
{
#if defined(DEBUG)
  Serial.begin(SERIALPORTSPEED);
  DebugPrintln(SERIALSTARTSTR);
  DebugPrintln(DEBUGONSTR);
#endif

  mySetupData = new SetupData();                // instantiate object SetUpData with SPIFFS file

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

#ifdef INOUTLEDS                                // Setup LEDS, use as controller power up indicator
  pinMode(INLEDPIN, OUTPUT);
  pinMode(OUTLEDPIN, OUTPUT);
  digitalWrite(INLEDPIN, 1);
  digitalWrite(OUTLEDPIN, 1);
#endif

#ifdef PUSHBUTTONS                              // Setup Pushbuttons, active high when pressed
  init_pushbuttons();
#endif

  displayfound = false;
#ifdef OLEDTEXT
  init_oledtextdisplay();
#endif
#if defined(OLEDGRAPHICS)
  displayfound = Init_OLED();
#endif

  delay(100);                                   // keep delays small otherwise issue with ASCOM

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

#ifdef TEMPERATUREPROBE                         // start temp probe
  init_temp();
#endif // end TEMPERATUREPROBE

#ifdef READWIFICONFIG
#if defined(ACCESSPOINT) || defined(STATIONMODE)
  TRACE();
  readwificonfig(mySSID, myPASSWORD);  // read mySSID,myPASSWORD from SPIFFS if exist, otherwise use defaults
#endif
#endif

#ifdef ACCESSPOINT
  oledtextmsg(STARTAPSTR, -1, true, true);
  DebugPrintln(F(STARTAPSTR));
  WiFi.config(ip, dns, gateway, subnet);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(mySSID, myPASSWORD);
#endif // end ACCESSPOINT

  // this is setup as a station connecting to an existing wifi network
#ifdef STATIONMODE
  DebugPrintln(F(STARTSMSTR));
  oledtextmsg(STARTSMSTR, -1, false, true);
  if (staticip == STATICIPON)                   // if staticip then set this up before starting
  {
    DebugPrintln(F(SETSTATICIPSTR));
    oledtextmsg(SETSTATICIPSTR, -1, false, true);
    WiFi.config(ip, dns, gateway, subnet);
    delay(5);
  }

  /* Log on to LAN */
  WiFi.mode(WIFI_STA);
  byte status = WiFi.begin(mySSID, myPASSWORD); // attempt to start the WiFi
  DebugPrint(F(WIFIBEGINSTATUSSTR));
  DebugPrintln(String(status));
  delay(1000);                                  // wait 500ms

  int attempts = 0;                             // holds the number of attempts/tries
  while (WiFi.status() != WL_CONNECTED)
  {
    DebugPrint(F(ATTEMPTCONNSTR));
    DebugPrintln(mySSID);
    DebugPrint(F(ATTEMPTSSTR));
    DebugPrint(attempts);
    delay(1000);                                // wait 1s
    attempts++;                                 // add 1 to attempt counter to start WiFi
    oledtextmsg(ATTEMPTSSTR, attempts, false, true);
    if (attempts > 10)                          // if this attempt is 11 or more tries
    {
      DebugPrintln(F(APCONNECTFAILSTR));
      DebugPrintln(F(WIFIRESTARTSTR));
      oledtextmsg(APCONNECTFAILSTR + String(mySSID), -1, true, true);
      delay(2000);
      software_Reboot(2000);                    // GPIO0 must be HIGH and GPIO15 LOW when calling ESP.restart();
    }
  }
#endif // end STATIONMODE

  oledtextmsg(CONNECTEDSTR, -1, true, true);
  delay(100);                                   // keep delays small else issue with ASCOM

#if defined(ACCESSPOINT) || defined(STATIONMODE)
  // Starting TCP Server
  DebugPrintln(F(STARTTCPSERVERSTR));
  oledtextmsg(STARTTCPSERVERSTR, -1, false, true);
  myserver.begin();
  DebugPrintln(F(GETLOCALIPSTR));
  ESP32IPAddress = WiFi.localIP();
  delay(100);                                   // keep delays small else issue with ASCOM
  DebugPrintln(F(TCPSERVERSTARTEDSTR));
  oledtextmsg(TCPSERVERSTARTEDSTR, -1, false, true);

  // set packet counts to 0
  packetsreceived = 0;
  packetssent = 0;

  // connection established
  DebugPrint(F(SSIDSTR));
  DebugPrintln(mySSID);
  DebugPrint(F(IPADDRESSSTR));
  DebugPrintln(WiFi.localIP());
  DebugPrint(PORTSTR);
  DebugPrintln(SERVERPORT);
  DebugPrintln(F(SERVERREADYSTR));
  myIP = WiFi.localIP();
  ipStr = String(myIP[0]) + "." + String(myIP[1]) + "." + String(myIP[2]) + "." + String(myIP[3]);
#else
  // it is Bluetooth so set some globals
  ipStr = "0.0.0.0";
#endif // if defined(ACCESSPOINT) || defined(STATIONMODE)

  // assign to current working values
  ftargetPosition = fcurrentPosition = mySetupData->get_fposition();

  DebugPrint(F(SETUPDRVBRDSTR));
  DebugPrintln(DRVBRD);
  oledtextmsg(SETUPDRVBRDSTR, DRVBRD, true, true);

  driverboard = new DriverBoard(DRVBRD);
  // setup firmware filename
  programName = driverboard->getboardname();
  DebugPrintln(F(DRVBRDDONESTR));
  oledtextmsg(DRVBRDDONESTR, -1, false, true);
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
  mySetupData->set_stepsize((float)(mySetupData->get_stepsize() > MAXIMUMSTEPSIZE ) ? MAXIMUMSTEPSIZE : mySetupData->get_stepsize());

  driverboard->setmotorspeed(mySetupData->get_motorSpeed());  // restore motorspeed
  driverboard->setstepmode(mySetupData->get_stepmode());      // restore stepmode

  DebugPrintln(F(CHECKCPWRSTR));
  if (mySetupData->get_coilpower() == 0)
  {
    driverboard->releasemotor();
    DebugPrintln(F(CPWRRELEASEDSTR));
  }

  delay(5);

  // setup home position switch
#ifdef HOMEPOSITIONSWITCH
  init_homepositionswitch();
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

#ifdef TEMPERATUREPROBE
  // restore temperature probe resolution setting
  // temp_setresolution(mySetupData->get_tempprecision());    // redundant as earlier call to init_temp() sets the precision
  read_temp(1);
#endif

  mdnsserverstate = STOPPED;
  webserverstate = STOPPED;
  ascomserverstate = STOPPED;
  managementserverstate = STOPPED;
  otaupdatestate = STOPPED;
  duckdnsstate = STOPPED;

#ifdef OTAUPDATES
  start_otaservice();                       // Start the OTA service
#endif // if defined(OTAUPDATES)

#ifdef MANAGEMENT
  start_management();
#endif

#ifdef WEBSERVER
  start_webserver();
#endif

#ifdef ASCOMREMOTE
  start_ascomremoteserver();
#endif

#ifdef MDNSSERVER
  start_mdns_service();
#endif

  // setup duckdns
#ifdef USEDUCKDNS
  init_duckdns();
#endif

  DebugPrint(F(CURRENTPOSSTR));
  DebugPrintln(fcurrentPosition);
  DebugPrint(F(TARGETPOSSTR));
  DebugPrintln(ftargetPosition);
  DebugPrintln(F(SETUPENDSTR));
  oledtextmsg(SETUPENDSTR, -1, false, true);

#ifdef INOUTLEDS
  digitalWrite(INLEDPIN, 0);
  digitalWrite(OUTLEDPIN, 0);
#endif
}

//_____________________ loop()___________________________________________

//void IRAM_ATTR loop() // ESP32
void loop()
{
  static byte MainStateMachine = State_Idle;
  static byte backlash_count = 0;
  static byte backlash_enabled = 0;
  static byte DirOfTravel = mySetupData->get_focuserdirection();
  static unsigned long TimeStampDelayAfterMove = 0;
  static unsigned long TimeStampPark = millis();
  static byte Parked = false;
  static byte updatecount = 0;
#if defined(ACCESSPOINT) || defined(STATIONMODE)
  static byte ConnectionStatus = 0;
#endif
#ifdef HOMEPOSITIONSWITCH
  static byte stepstaken       = 0;
#endif

#ifdef LOOPTIMETEST
  DebugPrint(F(LOOPSTARTSTR));
  DebugPrintln(millis());
#endif

#if defined(ACCESSPOINT) || defined(STATIONMODE)
  if (ConnectionStatus < 2)
  {
    myclient = myserver.available();
    if (myclient)
    {
      DebugPrintln(F(TCPCLIENTCONNECTSTR));
      if (myclient.connected())
      {
        ConnectionStatus = 2;
      }
    }
    else
    {
      if (ConnectionStatus)
      {
        DebugPrintln(F(TCPCLIENTDISCONNECTSTR));
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
        ESP_Communication(ESPDATA);
      }
    }
    else
    {
      ConnectionStatus = 1;
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
    ESP_Communication(BTDATA);
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
    ESP_Communication(SERIALDATA);
  }
#endif // ifdef LOCALSERIAL

#ifdef OTAUPDATES
  if ( otaupdatestate == RUNNING )
  {
    ArduinoOTA.handle();                      // listen for OTA events
  }
#endif // ifdef OTAUPDATES

#ifdef ASCOMREMOTE
  if ( ascomserverstate == RUNNING)
  {
    ascomserver->handleClient();
  }
#endif

#ifdef WEBSERVER
  if ( webserverstate == RUNNING )
  {
    webserver->handleClient();
  }
#endif

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
      if (fcurrentPosition != ftargetPosition)
      {
        isMoving = 1;
        driverboard->enablemotor();
        MainStateMachine = State_InitMove;
        DebugPrint(F(STATEINITMOVE));
        DebugPrint(F(CURRENTPOSSTR));
        DebugPrintln(fcurrentPosition);
        DebugPrint(F(TARGETPOSSTR));
        DebugPrintln(ftargetPosition);
      }
      else
      {
        // focuser stationary. isMoving is 0
        byte status = mySetupData->SaveConfiguration(fcurrentPosition, DirOfTravel); // save config if needed
        if ( status == true )
        {
          Update_OledGraphics(oled_off);                // Display off after config saved
          DebugPrint(F(CONFIGSAVEDSTR));
          DebugPrintln(status);
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
          Update_OledGraphics(oled_stay);
          update_oledtextdisplay();
        }
        else
        {
          Update_OledGraphics(oled_off);
        }

#ifdef TEMPERATUREPROBE
        update_temp();
#endif // temperatureprobe

        if (Parked == false)
        {
          if (TimeCheck(TimeStampPark, MotorReleaseDelay))   //Power off after MotorReleaseDelay
          {
            // need to obey rule - can only release motor if coil power is disabled
            if ( mySetupData->get_coilpower() == 0 )
            {
              driverboard->releasemotor();
              DebugPrintln(F(RELEASEMOTORSTR));
            }
            Parked = true;
          }
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
        DebugPrintln(STATEMOVINGSTR);
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
          DebugPrint(F(STATEAPPLYBACKLASH));
        }
        else
        {
          // do not apply backlash, go straight to moving
          MainStateMachine = State_Moving;
          DebugPrint(STATEMOVINGSTR);
        }
#else
        // ignore backlash
        MainStateMachine = State_Moving;
        DebugPrint(STATEMOVINGSTR);
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
        DebugPrintln(STATEMOVINGSTR);
      }
      break;

    case State_Moving:
      if ( fcurrentPosition != ftargetPosition )      // must come first else cannot halt
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
              updatecount = 0;
              update_oledtext_position();
            }
          }
        }
#ifdef HOMEPOSITIONSWITCH
        // if switch state = CLOSED and currentPosition != 0
        // need to back OUT a little till switch opens and then set position to 0
        if ( (hpswstate == HPSWCLOSED) && (fcurrentPosition != 0) )
        {
          isMoving = 1;
          fcurrentPosition = ftargetPosition = 0;
          MainStateMachine = State_SetHomePosition;
          DebugPrintln(F(HPCLOSEDFPNOT0STR));
          DebugPrintln(F(STATESETHOMEPOSITION));
#ifdef SHOWHPSWMSGS
#ifdef OLEDTEXT
          myoled->clear();
          myoled->println(HPCLOSEDFP0STR);
#endif  // OLEDTEXT
#endif  // SHOWHPSWMSGS
        }
        // else if switch state = CLOSED and Position = 0
        // need to back OUT a little till switch opens and then set position to 0
        else if ( (hpswstate == HPSWCLOSED) && (fcurrentPosition == 0) )
        {
          isMoving = 1;
          fcurrentPosition = ftargetPosition = 0;
          MainStateMachine = State_SetHomePosition;
          DebugPrintln(F(HPCLOSEDFP0STR));
          DebugPrintln(F(STATESETHOMEPOSITION));
#ifdef SHOWHPSWMSGS
#ifdef OLEDTEXT
          myoled->clear();
          myoled->println(HPCLOSEDFP0STR);
#endif // OLEDTEXT
#endif // SHOWHPSWMSGS
        }
        // else if switchstate = OPEN and Position = 0
        // need to move IN a little till switch CLOSES then
        else if ( (hpswstate == HPSWOPEN) && (fcurrentPosition == 0))
        {
          isMoving = 1;
          fcurrentPosition = ftargetPosition = 0;
          MainStateMachine = State_FindHomePosition;
          DebugPrintln(F(HPOPENFPNOT0STR));
          DebugPrintln(F(STATEFINDHOMEPOSITION));
#ifdef SHOWHPSWMSGS
#ifdef OLEDTEXT
          myoled->clear();
          myoled->println(HPOPENFPNOT0STR);
#endif  // OLEDTEXT
#endif  // SHOWHPSWMSGS
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
              update_oledtext_position();
            }
          }
        }
      }
      else
      {
        MainStateMachine = State_DelayAfterMove;
        DebugPrintln(F(STATEDELAYAFTERMOVE));
      }
      break;

    case State_FindHomePosition:            // move in till home position switch closes
#ifdef HOMEPOSITIONSWITCH
      driverboard->setmotorspeed(SLOW);
      stepstaken = 0;
      DebugPrintln(F(HPMOVETILLCLOSEDSTR));
#ifdef SHOWHPSWMSGS
#ifdef OLEDTEXT
      myoled->println(HPMOVETILLCLOSEDSTR);
#endif // OLEDTEXT
#endif // SHOWHPSWMSGS
      while ( hpswstate == HPSWOPEN )
      {
        // step IN till switch closes
        steppermotormove(DirOfTravel);
        stepstaken++;
        if ( stepstaken > HOMESTEPS )       // this prevents the endless loop if the hpsw is not connected or is faulty
        {
          DebugPrint(F(HPMOVEINERRORSTR));
          break;
        }
      }
      DebugPrint(F(HPMOVEINSTEPSSTR));
      DebugPrint(stepstaken);
#ifdef SHOWHPSWMSGS
#ifdef OLEDTEXT
      myoled->clear();
      myoled->println(HPCLOSEDFP0STR);
#endif  // OLEDTEXT
#endif  // SHOWHPSWMSGS
      DebugPrint(F(HPMOVEINFINISHEDSTR));
      driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif  // HOMEPOSITIONSWITCH
      MainStateMachine = State_SetHomePosition;
      DebugPrint(F(STATESETHOMEPOSITION));
      break;

    case State_SetHomePosition:             // move out till home position switch opens
#ifdef HOMEPOSITIONSWITCH
      driverboard->setmotorspeed(SLOW);
      stepstaken = 0;
      DebugPrintln(F(HPMOVETILLOPENSTR));
#ifdef SHOWHPSWMSGS
#ifdef OLEDTEXT
      myoled->println(HPMOVETILLOPENSTR);
#endif // OLEDTEXT
#endif // SHOWHPSWMSGS
      // if the previous moveIN failed at HOMESTEPS and HPSWITCH is still open then the
      // following while() code will drop through and have no effect and position = 0
      while ( hpswstate == HPSWCLOSED )
      {
        // step out till switch opens
        DirOfTravel = !DirOfTravel;
        steppermotormove(DirOfTravel);
        stepstaken++;
        if ( stepstaken > HOMESTEPS )       // this prevents the endless loop if the hpsw is not connected or is faulty
        {
          DebugPrintln(F(HPMOVEOUTERRORSTR));
          break;
        }
      }
      DebugPrint(F(HPMOVEOUTSTEPSSTR));
      DebugPrintln(stepstaken);
      driverboard->setmotorspeed(mySetupData->get_motorSpeed());
      DebugPrintln(F(HPMOVEOUTFINISHEDSTR));
#ifdef SHOWHPSWMSGS
#ifdef OLEDTEXT
      myoled->clear();
      myoled->println(HPMOVEOUTFINISHEDSTR);
#endif  // OLEDTEXT
#endif  // SHOWHPSWMSGS
#endif  // HOMEPOSITIONSWITCH
      MainStateMachine = State_DelayAfterMove;
      DebugPrintln(F(STATEDELAYAFTERMOVE));
      break;

    case State_DelayAfterMove:
      // apply Delayaftermove, this MUST be done here in order to get accurate timing for DelayAfterMove
      if (TimeCheck(TimeStampDelayAfterMove , mySetupData->get_DelayAfterMove()))
      {
        Update_OledGraphics(oled_on);                   // display on after move
        TimeStampPark  = millis();                      // catch current time
        Parked = false;                                 // mark to park the motor in State_Idle
        MainStateMachine = State_FinishedMove;
        DebugPrintln(F(STATEFINISHEDMOVE));
      }
      break;

    case State_FinishedMove:
      isMoving = 0;
      // coil power is turned off after MotorReleaseDelay expired and Parked==true, see State_Idle
      TRACE();
      MainStateMachine = State_Idle;
      DebugPrintln(F(STATEIDLE));
      break;

    default:
      DebugPrintln(F("Error: wrong State => State_Idle"));
      MainStateMachine = State_Idle;
      break;
  }

#ifdef LOOPTIMETEST
  DebugPrint(F(LOOPENDSTR));
  DebugPrintln(millis());
#endif
} // end Loop()
