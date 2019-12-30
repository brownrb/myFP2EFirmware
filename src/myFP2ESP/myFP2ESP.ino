// ----------------------------------------------------------------------------------------------
// TITLE: myFP2ESP FIRMWARE RELEASE 113
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
// 1. Set your DRVBRD in myBoards.h so the correct driver board is used
// 2. Set the correct hardware options and controller modes in focuserconfig.h to match your hardware
// 3. Set your target CPU to match the correct CPU for your board
// 4. Compile and upload to your controller

// ----------------------------------------------------------------------------------------------
// PCB BOARDS
// ----------------------------------------------------------------------------------------------
// The PCB's are available as GERBER files.
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
char mySSID[64] = "myfp2eap";
char myPASSWORD[64] = "myfp2eap";
#endif

// ----------------------------------------------------------------------------------------------
// 8: OTA (OVER THE AIR UPDATING) SSID AND PASSWORD CONFIGURATION
// ----------------------------------------------------------------------------------------------
// You can change the values for OTANAME and OTAPassword if required
#if defined(OTAUPDATES)
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
char mDNSNAME[] = "myfp2eap";
#endif // #ifdef MDNSSERVER

// ----------------------------------------------------------------------------------------------
// 11: DUCKDNS DOMAIN AND TOKEN CONFIGURATION
// ----------------------------------------------------------------------------------------------
// if using DuckDNS you need to set these next two parameters, duckdnsdomain and duckdnstoken
// cannot use DuckDNS with ACCESSPOINT, BLUETOOTHMODE or LOCALSERIAL mode
#ifdef DUCKDNS
const char* duckdnsdomain = "myfp2erobert.duckdns.org";
const char* duckdnstoken = "0a0379d5-3979-44ae-b1e2-6c371a4fe9bf";
#endif // #ifdef DUCKDNS

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
#if defined(JOYSTICK1) || defined(JOYSTICK2)
  Features = Features + ENABLEDJOYSTICK;
#endif
}

// ----------------------------------------------------------------------------------------------
// 16: GLOBAL DATA -- DO NOT CHANGE
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

char programVersion[] = "113";
char ProgramAuthor[]  = "(c) R BROWN 2019";

unsigned long fcurrentPosition;                         // current focuser position
unsigned long ftargetPosition;                          // target position
unsigned long tmppos;

byte tprobe1;                                           // indicate if there is a probe attached to myFocuserPro2
byte isMoving;                                          // is the motor currently moving
String ipStr;                                           // shared between BT mode and other modes
boolean displayfound;

#if defined(BLUETOOTHMODE) || defined(LOCALSERIAL)
#include "ESPQueue.h"                   //  By Steven de Salas
Queue queue(QUEUELENGTH);               // receive serial queue of commands
#ifdef LOCALSERIAL
String serialline;                      // buffer for serial data
#endif
#ifdef BLUETOOTHMODE
String btline;                          // buffer for serial data
#endif
#endif // #if defined(BLUETOOTHMODE) || defined(LOCALSERIAL)

#if defined(ACCESSPOINT) || defined(STATIONMODE) || defined(LOCALSERIAL) || defined(BLUETOOTH)
IPAddress ESP32IPAddress;
String ServerLocalIP;
WiFiServer myserver(SERVERPORT);
WiFiClient myclient;                                    // only one client supported, multiple connections denied
IPAddress myIP;
long rssi;
#endif

#ifdef TEMPERATUREPROBE
#include <OneWire.h>                      // https://github.com/PaulStoffregen/OneWire
#include <myDallasTemperature.h>
OneWire oneWirech1(TEMPPIN);              // setup temperature probe
DallasTemperature sensor1(&oneWirech1);
DeviceAddress tpAddress;                  // holds address of the temperature probe
#endif

#ifdef OLEDTEXT
#include <Wire.h>                           // needed for I2C => OLED display
#include <mySSD1306Ascii.h>
#include <mySSD1306AsciiWire.h>
SSD1306AsciiWire* myoled;
#endif // #ifdef OLEDTEXT

#ifdef OLEDGRAPHICS
#include <SSD1306Wire.h>
#include "images.h"
SSD1306Wire *myoled;
#endif // #ifdef OLEDGRAPHICS

int packetsreceived;
int packetssent;
bool mdnsserverstate;
bool webserverstate;
bool ascomserverstate;
bool managementserverstate;
bool otaservicestate;

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
  DebugPrintln(checkfortprobestr);
  sensor1.begin();                        // start the temperature sensor1
  DebugPrintln(F("Get # of Tsensors"));
  tprobe1 = sensor1.getDeviceCount();     // should return 1 if probe connected
  DebugPrint(F(tprobestr));
  DebugPrintln(tprobe1);
  if (sensor1.getAddress(tpAddress, 0) == true)
  {
    tprobe1 = 1;
    sensor1.setResolution(tpAddress, mySetupData->get_tempprecision());    // set probe resolution
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
  DebugPrint(tempstr);
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
    String tmbuffer = String(readtemp(0), 2);
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
#endif
  return displayfound;
}

// ----------------------------------------------------------------------------------------------
// 19B: OLED TEXT DISPLAY - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------

// Enclose function code in #ifdef - #endif so function declarations are visible

void oledtextmsg(String str, int val, boolean clrscr, boolean nl)
{
#ifdef OLEDTEXT
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
#endif // OLEDTEXT
}

void display_oledtext_page0(void)           // displaylcd screen
{
#ifdef OLEDTEXT
  char tempString[20];
  myoled->home();
  myoled->print(currentposstr);
  myoled->print(fcurrentPosition);
  myoled->clearToEOL();

  myoled->println();
  myoled->print(targetposstr);
  myoled->print(ftargetPosition);
  myoled->clearToEOL();
  myoled->println();

  myoled->print(coilpwrstr);
  myoled->print(mySetupData->get_coilpower());
  myoled->clearToEOL();
  myoled->println();

  myoled->print(revdirstr);
  myoled->print(mySetupData->get_reversedirection());
  myoled->clearToEOL();
  myoled->println();

  // stepmode setting
  myoled->print(stepmodestr);
  myoled->print(mySetupData->get_stepmode());
  myoled->clearToEOL();
  myoled->println();

  //Temperature
  myoled->print(tempstr);
#ifdef TEMPERATUREPROBE
  myoled->print(String(read_temp(0), 2));
#else
  myoled->print("20.0");
#endif
  myoled->print(" c");
  myoled->clearToEOL();
  myoled->println();

  //Motor Speed
  myoled->print(motorspeedstr);
  myoled->print(mySetupData->get_motorSpeed());
  myoled->clearToEOL();
  myoled->println();

  //MaxSteps
  myoled->print(maxstepsstr);
  ltoa(mySetupData->get_maxstep(), tempString, 10);
  myoled->print(tempString);
  myoled->clearToEOL();
  myoled->println();
#endif
}

void display_oledtext_page1(void)
{
#ifdef OLEDTEXT
  // temperature compensation
  myoled->print(tcompstepsstr);
  myoled->print(mySetupData->get_tempcoefficient());
  myoled->clearToEOL();
  myoled->println();

  myoled->print(tcompstatestr);
  myoled->print(mySetupData->get_tempcompenabled());
  myoled->clearToEOL();
  myoled->println();

  myoled->print(tcompdirstr);
  myoled->print(mySetupData->get_tcdirection());
  myoled->clearToEOL();
  myoled->println();

  myoled->print(backlashinstr);
  myoled->print(mySetupData->get_backlash_in_enabled());
  myoled->clearToEOL();
  myoled->println();

  myoled->print(backlashoutstr);
  myoled->print(mySetupData->get_backlash_out_enabled());
  myoled->clearToEOL();
  myoled->println();

  myoled->print(backlashinstepsstr);
  myoled->print(mySetupData->get_backlashsteps_in());
  myoled->clearToEOL();
  myoled->println();

  myoled->print(backlashoutstepsstr);
  myoled->print(mySetupData->get_backlashsteps_out());
  myoled->clearToEOL();
  myoled->println();
#endif
}

void display_oledtext_page2(void)
{
#ifdef OLEDTEXT
#if defined(ACCESSPOINT) || defined(STATIONMODE)
  myoled->setCursor(0, 0);
#if defined(ACCESSPOINT)
  myoled->print(accesspointstr);
  myoled->clearToEOL();
  myoled->println();
#endif
#if defined(STATIONMODE)
  myoled->print(stationmodestr);
  myoled->clearToEOL();
  myoled->println();
#endif
  myoled->print(ssidstr);
  myoled->print(mySSID);
  myoled->clearToEOL();
  myoled->println();
  myoled->print(ipaddressstr);
  myoled->print(ipStr);
  myoled->clearToEOL();
  myoled->println();
#endif // if defined(ACCESSPOINT) || defined(STATIONMODE)

#if defined(WEBSERVER)
  //myoled->setCursor(0, 0);
  myoled->print(webserverstr);
  myoled->clearToEOL();
  myoled->println();
  myoled->print(ipaddressstr);
  myoled->print(ipStr);
  myoled->clearToEOL();
  myoled->println();
  myoled->print(PORTSTR);
  myoled->print(SERVERPORT);
  myoled->clearToEOL();
  myoled->println();
#endif // webserver
#if defined(ASCOMREMOTE)
  myoled->print(ascomremotestr);
  myoled->clearToEOL();
  myoled->println();
  myoled->print(ipaddressstr);
  myoled->print(ipStr);
  myoled->clearToEOL();
  myoled->println();
  myoled->print(PORTSTR);
  myoled->print(ALPACAPORT);
  myoled->clearToEOL();
  myoled->println();
#endif

#if defined(BLUETOOTHMODE)
  myoled->setCursor(0, 0);
  myoled->print(bluetoothstr);
  myoled->clearToEOL();
  myoled->println();
#endif

#if defined(LOCALSERIAL)
  myoled->setCursor(0, 0);
  myoled->println(localserialstr);
#endif
#endif // #ifdef OLEDTEXT
}

void update_oledtext_position(void)
{
#ifdef OLEDTEXT
  myoled->setCursor(0, 0);
  myoled->print(currentposstr);
  myoled->print(fcurrentPosition);
  myoled->clearToEOL();
  myoled->println();

  myoled->print(targetposstr);
  myoled->print(ftargetPosition);
  myoled->clearToEOL();
  myoled->println();
#endif
}

void update_oledtextdisplay(void)
{
#ifdef OLEDTEXT
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
#endif
}

void init_oledtextdisplay(void)
{
#ifdef OLEDTEXT
#if defined(ESP8266)
#if (DRVBRD == PRO2EL293DNEMA) || (DRVBRD == PRO2EL293D28BYJ48)
  Wire.begin(I2CDATAPIN, I2CCLKPIN);        // l293d esp8266 shield
  DebugPrintln("Setup PRO2EL293DNEMA/PRO2EL293D28BYJ48 I2C");
#else
  DebugPrintln("Setup esp8266 I2C");
  Wire.begin();
#endif
#else
  DebugPrintln("Setup esp32 I2C");
  Wire.begin(I2CDATAPIN, I2CCLKPIN);          // esp32
#endif
  myoled = new SSD1306AsciiWire();
  // Setup the OLED
  myoled->begin(&Adafruit128x64, OLED_ADDR);
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
#include <IRremoteESP32.h>
IRrecv irrecv(IRPIN);
decode_results results;
#include "irremotemappings.h"

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

  DebugPrintln(F("joystick: update joystick"));
  joyval = analogRead(JOYINOUTPIN);
  DebugPrint("Raw joyval:");
  DebugPrintln(joyval);
  if ( joyval < (JZEROPOINT - JTHRESHOLD) )
  {
    ftargetPosition--;                            // move IN
    DebugPrint("X IN joyval:");
    DebugPrint(joyval);
    joyspeed = map(joyval, 0, (JZEROPOINT - JTHRESHOLD), MSFAST, MSSLOW);
    DebugPrint(", Speed:");
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
    DebugPrint("X OUT joyval:");
    DebugPrint(joyval);
    joyspeed = map(joyval, 0, (JMAXVALUE - (JZEROPOINT + JTHRESHOLD)), MSSLOW, MSFAST);
    DebugPrint(", Speed:");
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
  DebugPrint("Raw joyval:");
  DebugPrintln(joyval);
  if ( joyval < (JZEROPOINT - JTHRESHOLD) )
  {
    ftargetPosition--;                            // move IN
    DebugPrint("X IN joyval:");
    DebugPrint(joyval);
    joyspeed = map(joyval, 0, (JZEROPOINT - JTHRESHOLD), MSFAST, MSSLOW);
    Debugprint(", Speed:");
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
    DebugPrint("X OUT joyval:");
    DebugPrint(joyval);
    joyspeed = map(joyval, 0, (JMAXVALUE - (JZEROPOINT + JTHRESHOLD)), MSSLOW, MSFAST);
    Debugprint(", Speed:");
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
    DebugPrintln(F("Error setting up MDNS responder!"));
    mdnsserverstate = STOPPED;
  }
  else
  {
    DebugPrintln(F("mDNS responder started"));
    // Add service to MDNS-SD, MDNS.addService(service, proto, port)
    MDNS.addService("http", "tcp", MDNSSERVERPORT);
    mdnsserverstate = RUNNING;
  }
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

// convert the file extension to the MIME type
String MANAGEMENT_getContentType(String filename)
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
bool MANAGEMENT_handleFileRead(String path)
{
  DebugPrintln("handleFileRead: " + path);
  if (path.endsWith("/"))
  {
    path += "index.html";                               // If a folder is requested, send the index file
  }
  String contentType = MANAGEMENT_getContentType(path); // Get the MIME type
  if (SPIFFS.exists(path))                              // If the file exists
  {
    File file = SPIFFS.open(path, "r");                 // Open it
#ifdef MANAGEMENTFORCEDOWNLOAD
    mserver.sendHeader("Content-Type", "application/octet-stream");
    mserver.sendHeader("Content-Disposition", "attachment");
#endif
    size_t sent = mserver.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  DebugPrintln("File Not Found");
  return false;                                         // If the file doesn't exist, return false
}

void MANAGEMENT_listSPIFFSfiles(void)
{
  String path = "/";
  DebugPrintln("MANAGEMENT_listSPIFFSfiles: " + path);
#if defined(ESP8266)
  Dir dir = SPIFFS.openDir(path);
  path = String();
  String output = "[";
  while (dir.next())
  {
    File entry = dir.openFile("r");
    if (output != "[")
    {
      output += ',';
    }
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }
  output += "]";
  mserver.send(200, "text/json", output);
#else // ESP32
  File root = SPIFFS.open(path);
  path = String();

  String output = "[";
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
  output += "]";
  mserver.send(200, "text/json", output);
#endif
}

void MANAGEMENT_handlenotfound()
{
  String MNotFoundPage;

  // load not found page from spiffs - wsnotfound.html
  if (!SPIFFS.begin())
  {
    DebugPrintln(F("management: Error occurred when mounting SPIFFS"));
    DebugPrintln("management: build_default_notfoundpage");
    MNotFoundPage = "<html><head><title>Management Server: Not found></title></head><body><p>The requested URL was not found</p><p><form action=\"/\" method=\"GET\"><input type=\"submit\" value=\"HOMEPAGE\"></form></p></body></html>";
  }
  else
  {
    DebugPrintln("management: SPIFFS mounted");
    if ( SPIFFS.exists("/msnotfound.html"))
    {
      DebugPrintln("management: msnotfound.html found in spiffs");
      // open file for read
      File file = SPIFFS.open("/msnotfound.html", "r");
      // read contents into string
      DebugPrintln("management: read page into string");
      MNotFoundPage = file.readString();

      DebugPrintln("management: processing page start");
      // process for dynamic data
      MNotFoundPage.replace("%MS_IPSTR%", ipStr);
      MNotFoundPage.replace("%MSSERVERPORT%", String(MSSERVERPORT));
      MNotFoundPage.replace("%MSPROGRAMVERSION%", String(programVersion));
      MNotFoundPage.replace("%MSPROGRAMNAME%", String(programName));
      DebugPrintln("management: processing page done");
    }
    else
    {
      DebugPrintln("management: build_default_notfoundpage");
      MNotFoundPage = "<html><head><title>Management Server: Not found></title></head><body><p>The requested URL was not found</p><p><form action=\"/\" method=\"GET\"><input type=\"submit\" value=\"HOMEPAGE\"></form></p></body></html>";
    }
  }
  mserver.send(NOTFOUNDWEBPAGE, TEXTPAGETYPE, MNotFoundPage);
}

void MANAGEMENT_handleroot(void)
{
  // code here to handle a put request
  String msg;
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
  msg = mserver.arg("setport");
  if ( msg != "" )
  {
    DebugPrint( "set web server port: " );
    DebugPrintln(msg);
    String wp = mserver.arg("wp");                      // get webserver port number
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
          if ( newport == MSSERVERPORT )              // if same as management server
          {
            DebugPrintln("wp error: new Port = MSSERVERPORT");
          }
          else if ( newport == ALPACAPORT )           // if same as ASCOM REMOTE server
          {
            DebugPrintln("wp error: new Port = ALPACAPORT");
          }
          else if ( newport == MDNSSERVERPORT )       // if same as mDNS server
          {
            DebugPrintln("wp error: new Port = MDNSSERVERPORT");
          }
          else
          {
            DebugPrintln("New webserver port = " + String(newport));
            mySetupData->set_webserverport(newport);  // assign new port and save it
          }
        }
      }
      else
      {
        DebugPrintln("Attempt to change webserver port when webserver running");
      }
    }
  }

  // construct the homepage now
  String MHomePage;

  // constructs home page of management server
  DebugPrintln(F("management: build_homepage()"));
  // load not found page from spiffs - wsindex.html
  if (!SPIFFS.begin())
  {
    DebugPrintln(F("management: Error occurred when mounting SPIFFS"));
    // could not read index file from SPIFFS
    DebugPrintln("management: build_defaulthomepage");
    MHomePage = "<html><head><title>Management Server:></title></head><body><p>The index file for the Management server was not found.</p><p>Did you forget to upload the data files to SPIFFS?</p></body></html>";
  }
  else
  {
    DebugPrintln("management: SPIFFS mounted");
    if ( SPIFFS.exists("/msindex.html"))
    {
      DebugPrintln("management: msindex.html found in spiffs");
      // open file for read
      File file = SPIFFS.open("/msindex.html", "r");
      // read contents into string
      DebugPrintln("management: read page into string");
      MHomePage = file.readString();

      DebugPrintln("management: processing page start");
      // process for dynamic data
      MHomePage.replace("%MS_IPSTR%", ipStr);
      MHomePage.replace("%MSSERVERPORT%", String(MSSERVERPORT));
      MHomePage.replace("%MSPROGRAMVERSION%", String(programVersion));
      MHomePage.replace("%MSPROGRAMNAME%", String(programName));
#ifdef BLUETOOTHMODE
      MHomePage.replace("%MSCONTROLLERMODE%", "Bluetooth : " + String(BLUETOOTHNAME));
#endif
#ifdef ACCESSPOINT
      MHomePage.replace("%MSCONTROLLERMODE%", "Access point");
#endif
#ifdef STATIONMODE
      MHomePage.replace("%MSCONTROLLERMODE%", "Station mode");
#endif
#ifdef LOCALSERIAL
      MHomePage.replace("%MSCONTROLLERMODE%", "Local Serial");
#endif
      // %WSTATUS%
#ifdef WEBSERVER
      if ( webserverstate == RUNNING )
      {
        MHomePage.replace("%WSTATUS%", "RUNNING");
      }
      else
      {
        MHomePage.replace("%WSTATUS%", "STOPPED");
      }
#else
      MHomePage.replace("%WSTATUS%", "Web server not defined in firmware");
#endif
      MHomePage.replace("%WSPORT%", String(mySetupData->get_webserverport()));
      // %WSBUTTON%
#ifdef WEBSERVER
      if ( webserverstate == RUNNING)
      {
        MHomePage.replace("%WSBUTTON%", "<form action=\"/\" method=\"post\"><input type=\"hidden\" name=\"stopws\" value=\"true\"><input type=\"submit\" value=\"STOP\"></form>");
      }
      else
      {
        MHomePage.replace("%WSBUTTON%", "<form action=\"/\" method=\"post\"><input type=\"hidden\" name=\"startws\" value=\"true\"><input type=\"submit\" value=\"START\"></form>");
      }
#else
      MHomePage.replace("%WSBUTTON%", " ");
#endif
#ifdef ASCOMREMOTE
      if ( ascomserverstate == RUNNING )
      {
        MHomePage.replace("%ASTATUS%", "RUNNING");
      }
      else
      {
        MHomePage.replace("%ASTATUS%", "STOPPED");
      }
#else
      MHomePage.replace("%ASTATUS%", "ASCOM REMOTE server not defined in firmware");
#endif
      MHomePage.replace("%ASPORT%", String(ALPACAPORT));
#ifdef ASCOMREMOTE
      if ( ascomserverstate == RUNNING )
      {
        MHomePage.replace("%ASBUTTON%", "<form action=\"/\" method=\"post\"><input type=\"hidden\" name=\"stopas\" value=\"true\"><input type=\"submit\" value=\"STOP\"></form>");
      }
      else
      {
        MHomePage.replace("%ASBUTTON%", "<form action=\"/\" method=\"post\"><input type=\"hidden\" name=\"startas\" value=\"true\"><input type=\"submit\" value=\"START\"></form>");
      }
#else
      MHomePage.replace("%ASBUTTON%", " ");
#endif
#ifdef OTAUPDATES
      MHomePage.replace("%OTAUSTATUS%", "ON");
#else
      MHomePage.replace("%OTAUSTATUS%", "OFF");
#endif
      if ( staticip == STATICIPON )
      {
        MHomePage.replace("%IPSTATUS%", "ON");
      }
      else
      {
        MHomePage.replace("%IPSTATUS%", "OFF");
      }
#ifdef MDNSSERVER
      if ( mdnsserverstate == RUNNING)
      {
        MHomePage.replace("%MDNSSTATUS%", "RUNNING");
      }
      else
      {
        MHomePage.replace("%MDNSSTATUS%", "STOPPED");
      }
#else
      MHomePage.replace("%MDNSSTATUS%", "mDNS server not defined in firmware");
#endif
      MHomePage.replace("%MDNSPORT%", String(MDNSSERVERPORT));

      // display heap memory for tracking memory loss?
      // only esp32?
      MHomePage.replace("%HEAPMEMORY%", String(ESP.getFreeHeap()));
      DebugPrintln("management: processing page done");
    }
    else
    {
      // could not read index file from SPIFFS
      DebugPrintln("management: build_defaulthomepage");
      MHomePage = "<html><head><title>Management Server:></title></head><body><p>The index file for the Management server was not found.</p><p>Did you forget to upload the data files to SPIFFS?</p></body></html>";
    }
  }

  // send the homepage to a connected client
  DebugPrintln("root() - send homepage");
  mserver.send(NORMALWEBPAGE, TEXTPAGETYPE, MHomePage );
  delay(10);                                      // small pause so background ESP8266 tasks can run
}

void start_management(void)
{
  if (!SPIFFS.begin())
  {
    DebugPrintln(F("start_management: An Error has occurred starting SPIFFS"));
    managementserverstate = STOPPED;
    return;
  }
  mserver.on("/", MANAGEMENT_handleroot);
  //mserver.onNotFound(MANAGEMENT_handlenotfound);
  mserver.onNotFound([]() {                         // if the client requests any URI
    if (!MANAGEMENT_handleFileRead(mserver.uri()))  // send file if it exists
    {
      MANAGEMENT_handlenotfound();                  // otherwise, respond with a 404 (Not Found) error
    }
  });
  mserver.on("/list", MANAGEMENT_listSPIFFSfiles);
  mserver.begin();
  managementserverstate = RUNNING;
}

void stop_management(void)
{
  mserver.stop();
  managementserverstate = STOPPED;
  DebugPrintln(F("stop_management: stopped"));
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
//ESP8266WebServer webserver(WEBSERVERPORT);
ESP8266WebServer *webserver;
#else
//WebServer webserver(WEBSERVERPORT);
WebServer *webserver;
#endif // if defined(esp8266)

void WEBSERVER_handlenotfound()
{
  String WNotFoundPage;
  // load not found page from spiffs - wsnotfound.html
  if (!SPIFFS.begin())
  {
    DebugPrintln(F("webserver: Error occurred when mounting SPIFFS"));
    DebugPrintln("webserver: build_default_notfoundpage");
    WNotFoundPage = "<html><head><title>Web Server: Not found></title></head><body>";
    WNotFoundPage = WNotFoundPage + "<p>The requested URL was not found</p>";
    WNotFoundPage = WNotFoundPage + "<p><form action=\"/\" method=\"GET\"><input type=\"submit\" value=\"HOMEPAGE\"></form></p>";
    WNotFoundPage = WNotFoundPage + "</body></html>";
  }
  else
  {
    DebugPrintln("webserver: SPIFFS mounted");
    if ( SPIFFS.exists("/wsnotfound.html"))
    {
      DebugPrintln("webserver: wsnotfound.html found in spiffs");
      // open file for read
      File file = SPIFFS.open("/wsnotfound.html", "r");
      // read contents into string
      DebugPrintln("webserver: read page into string");
      WNotFoundPage = file.readString();

      DebugPrintln("webserver: processing page start");
      // process for dynamic data
      WNotFoundPage.replace("%WS_IPSTR%", ipStr);
      WNotFoundPage.replace("%WEBSERVERPORT%", String(mySetupData->get_webserverport()));
      WNotFoundPage.replace("%WSPROGRAMVERSION%", String(programVersion));
      WNotFoundPage.replace("%WSPROGRAMNAME%", String(programName));
      DebugPrintln("webserver: processing page done");
    }
    else
    {
      DebugPrintln("webserver: build_default_notfoundpage");
      WNotFoundPage = "<html><head><title>Web Server: Not found></title></head><body>";
      WNotFoundPage = WNotFoundPage + "<p>The requested URL was not found</p>";
      WNotFoundPage = WNotFoundPage + "<p><form action=\"/\" method=\"GET\"><input type=\"submit\" value=\"HOMEPAGE\"></form></p>";
      WNotFoundPage = WNotFoundPage + "</body></html>";
    }
  }
  webserver->send(NOTFOUNDWEBPAGE, TEXTPAGETYPE, WNotFoundPage);
}

// handles root page of webserver
// this is called whenever a client requests home page of sebserver
void WEBSERVER_handleroot()
{
  String WHomePage;

  // if the root page was a HALT request via Submit button
  String halt_str = webserver->arg("ha");
  if ( halt_str != "" )
  {
    DebugPrint( "root() -halt:" );
    DebugPrintln(halt_str);
    ftargetPosition = fcurrentPosition;
  }

  // if set focuser position
  String fp_str;
  fp_str = webserver->arg("setpos");
  if ( fp_str != "" )
  {
    DebugPrint( "setpos:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("fp");
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
  fp_str = webserver->arg("gotopos");
  if ( fp_str != "" )
  {
    DebugPrint( "gotopos:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("fp");
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
    unsigned long temp = 0;
    DebugPrint( "root() -maxsteps:" );
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

  // if set focuser preset 0
  fp_str = webserver->arg("setp0");
  if ( fp_str != "" )
  {
    DebugPrint( "setp0:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p0");
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
      mySetupData->set_focuserpreset(0, temp);
    }
  }

  // if goto focuser preset 0
  fp_str = webserver->arg("gop0");
  if ( fp_str != "" )
  {
    DebugPrint( "gop0:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p0");
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
      mySetupData->set_focuserpreset(0, temp);
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
    DebugPrint( "setp1:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p1");
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
      mySetupData->set_focuserpreset(1, temp);
    }
  }

  // if goto focuser preset 1
  fp_str = webserver->arg("gop1");
  if ( fp_str != "" )
  {
    DebugPrint( "gop1:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p1");
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
      mySetupData->set_focuserpreset(1, temp);
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
    DebugPrint( "setp2:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p2");
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
      mySetupData->set_focuserpreset(2, temp);
    }
  }

  // if goto focuser preset 2
  fp_str = webserver->arg("gop2");
  if ( fp_str != "" )
  {
    DebugPrint( "gop2:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p2");
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
      mySetupData->set_focuserpreset(2, temp);
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
    DebugPrint( "setp3:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p3");
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
      mySetupData->set_focuserpreset(3, temp);
    }
  }

  // if goto focuser preset 3
  fp_str = webserver->arg("gop3");
  if ( fp_str != "" )
  {
    DebugPrint( "gop3:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p3");
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
      mySetupData->set_focuserpreset(3, temp);
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
    DebugPrint( "setp4:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p4");
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
      mySetupData->set_focuserpreset(4, temp);
    }
  }

  // if goto focuser preset 4
  fp_str = webserver->arg("gop4");
  if ( fp_str != "" )
  {
    DebugPrint( "gop4:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p4");
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
      mySetupData->set_focuserpreset(4, temp);
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
    DebugPrint( "setp5:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p5");
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
      mySetupData->set_focuserpreset(5, temp);
    }
  }

  // if goto focuser preset 5
  fp_str = webserver->arg("gop5");
  if ( fp_str != "" )
  {
    DebugPrint( "gop5:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p5");
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
      mySetupData->set_focuserpreset(5, temp);
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
    DebugPrint( "setp6:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p6");
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
      mySetupData->set_focuserpreset(6, temp);
    }
  }

  // if goto focuser preset 6
  fp_str = webserver->arg("gop6");
  if ( fp_str != "" )
  {
    DebugPrint( "gop6:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p6");
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
      mySetupData->set_focuserpreset(6, temp);
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
    DebugPrint( "setp0:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p7");
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
      mySetupData->set_focuserpreset(7, temp);
    }
  }

  // if goto focuser preset 7
  fp_str = webserver->arg("gop7");
  if ( fp_str != "" )
  {
    DebugPrint( "gop0:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p7");
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
      mySetupData->set_focuserpreset(7, temp);
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
    DebugPrint( "setp8:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p8");
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
      mySetupData->set_focuserpreset(8, temp);
    }
  }

  // if goto focuser preset 8
  fp_str = webserver->arg("gop8");
  if ( fp_str != "" )
  {
    DebugPrint( "gop8:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p8");
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
      mySetupData->set_focuserpreset(8, temp);
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
    DebugPrint( "setp9:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p9");
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
      mySetupData->set_focuserpreset(9, temp);
    }
  }

  // if goto focuser preset 9
  fp_str = webserver->arg("gop9");
  if ( fp_str != "" )
  {
    DebugPrint( "gop9:" );
    DebugPrintln(fp_str);
    String fp = webserver->arg("p9");
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
      mySetupData->set_focuserpreset(9, temp);
      ftargetPosition = temp;
#if defined(JOYSTICK1) || defined(JOYSTICK2)
      // restore motorspeed just in case
      driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif
    }
  }

  // if update motorspeed
  String fms_str = webserver->arg("ms");
  if ( fms_str != "" )
  {
    int temp1 = 0;
    DebugPrint( "root() -motorspeed:" );
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
    DebugPrint( "root() -coil power:" );
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
    DebugPrint( "root() -reverse direction:" );
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
    DebugPrint( "root() -stepmode:" );
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
    DebugPrint( "root() -temperature resolution:" );
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

  // if move
  String fmv_str = webserver->arg("mv");
  if ( fmv_str != "" )
  {
    unsigned long temp = 0;
    DebugPrint("root() -move:" );
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
#if defined(JOYSTICK1) || defined(JOYSTICK2)
    // restore motorspeed just in case
    driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif
  }

  DebugPrintln( "root() -build homepage" );

  // construct the homepage now
  DebugPrintln(F("webserver: build_homepage()"));
  // load not found page from spiffs - wsindex.html
  if (!SPIFFS.begin())
  {
    DebugPrintln(F("webserver: Error occurred when mounting SPIFFS"));
    // could not read index file from SPIFFS
    DebugPrintln("webserver: build_defaulthomepage");
    WHomePage = "<html><head><title>Web Server:></title></head><body><p>The index file for the webserver was not found.</p><p>Did you forget to upload the data files to SPIFFS?</p></body></html>";
  }
  else
  {
    DebugPrintln("webserver: SPIFFS mounted");
    if ( SPIFFS.exists("/wsindex.html"))
    {
      DebugPrintln("webserver: wsindex.html found in spiffs");
      // open file for read
      File file = SPIFFS.open("/wsindex.html", "r");
      // read contents into string
      DebugPrintln("webserver: read page into string");
      WHomePage = file.readString();

      DebugPrintln("webserver: processing page start");
      // process for dynamic data
      WHomePage.replace("%WSREFRESHRATE%", String(WS_REFRESHRATE));
      WHomePage.replace("%WSIPSTR%", ipStr);
      WHomePage.replace("%WSWEBSERVERPORT%", String(mySetupData->get_webserverport()));
      WHomePage.replace("%WSPROGRAMVERSION%", String(programVersion));
      WHomePage.replace("%WSPROGRAMNAME%", String(programName));
      WHomePage.replace("%WSCURRENTPOSITION%", String(fcurrentPosition));
      WHomePage.replace("%WSFTARGETPOSITION%", String(ftargetPosition));
      WHomePage.replace("%WSMAXSTEP%", String(mySetupData->get_maxstep()));
      WHomePage.replace("%WSISMOVING%", String(isMoving));
#ifdef TEMPERATUREPROBE
      WHomePage.replace("%WSTEMPERATURE%", String(read_temp(1)));
#else
      WHomePage.replace("%WSTEMPERATURE%", "20.000");
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
      WHomePage.replace("%WSP0BUFFER%", String(mySetupData->get_focuserpreset(0)));
      WHomePage.replace("%WSP1BUFFER%", String(mySetupData->get_focuserpreset(1)));
      WHomePage.replace("%WSP2BUFFER%", String(mySetupData->get_focuserpreset(2)));
      WHomePage.replace("%WSP3BUFFER%", String(mySetupData->get_focuserpreset(3)));
      WHomePage.replace("%WSP4BUFFER%", String(mySetupData->get_focuserpreset(4)));
      WHomePage.replace("%WSP5BUFFER%", String(mySetupData->get_focuserpreset(5)));
      WHomePage.replace("%WSP6BUFFER%", String(mySetupData->get_focuserpreset(6)));
      WHomePage.replace("%WSP7BUFFER%", String(mySetupData->get_focuserpreset(7)));
      WHomePage.replace("%WSP8BUFFER%", String(mySetupData->get_focuserpreset(8)));
      WHomePage.replace("%WSP9BUFFER%", String(mySetupData->get_focuserpreset(9)));
      DebugPrintln("webserver: processing page done");
    }
    else
    {
      // could not read index file from SPIFFS
      DebugPrintln("webserver: build_defaulthomepage");
      WHomePage = "<html><head><title>Web Server:></title></head><body><p>The index file for the webserver was not found.</p><p>Did you forget to upload the data files to SPIFFS?</p></body></html>";
    }
  }

  // send the homepage to a connected client
  DebugPrintln("root() - send homepage");
  webserver->send(NORMALWEBPAGE, TEXTPAGETYPE, WHomePage );
  delay(10);                     // small pause so background ESP8266 tasks can run
}

void start_webserver(void)
{
#if defined(ESP8266)
  webserver = new ESP8266WebServer(mySetupData->get_webserverport());
#else
  webserver = new WebServer(mySetupData->get_webserverport());
#endif // if defined(esp8266) 
  webserver->on("/", WEBSERVER_handleroot);
  webserver->onNotFound(WEBSERVER_handlenotfound);
  webserver->begin();
  webserverstate = RUNNING;
  delay(10);                        // small pause so background tasks can run
}

void stop_webserver(void)
{
  //webserver->stop();
  webserver->close();
  delete webserver;                 // free the webserver pointer and associated memory/code
  webserverstate = STOPPED;
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
#include <WebServer.h>
#endif // if defined(esp8266)

#include "ascomserver.h"

String Focuser_Setup_HomePage;    //  url:/setup/v1/focuser/0/setup
String Setup_HomePage;            //  url:/setup
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

// Construct pages for /setup
void ASCOM_Setup_HomePage()
{
  Setup_HomePage = "<html><head></head>";
  Setup_HomePage = Setup_HomePage + "<title>myFP2ESP ASCOM REMOTE</title></head>";
  Setup_HomePage = Setup_HomePage + "<body><h3>myFP2ESP ASCOM REMOTE</h3>";
  Setup_HomePage = Setup_HomePage + "<p>(c) R. Brown, Holger M, 2019. All rights reserved.</p>";
  Setup_HomePage = Setup_HomePage + "<p>IP Address: " + ipStr + ": Port: " + String(ALPACAPORT) + ", Firmware Version=" + String(programVersion) + "</p>";
  Setup_HomePage = Setup_HomePage + "<p>Driverboard = myFP2ESP." + driverboard->getboardname() + "</p>";
  // add link for setup page /setup/v1/focuser/0/setup
  Setup_HomePage = Setup_HomePage + "<p><a href=\"/setup/v1/focuser/0/setup\">Setup page</a></p>";
  Setup_HomePage = Setup_HomePage + "</body></html>";
}

// constructs ascom setup server page /setup/v1/focuser/0/setup
void ASCOM_Create_Setup_Focuser_HomePage()
{
  // Convert IP address to a string;
  // already in ipStr
  // convert current values of focuserposition and focusermaxsteps to string types
  String fpbuffer = String(fcurrentPosition);
  String mxbuffer = String(mySetupData->get_maxstep());
  String smbuffer = String(mySetupData->get_stepmode());
  String imbuffer = String(isMoving);

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

  // construct home page of webserver
  // header
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

// ----------------------------------------------------------------------------------------------
// Setup functions
// ----------------------------------------------------------------------------------------------
void ASCOM_handle_setup()
{
  // url /setup
  // The web page must describe the overall device, including name, manufacturer and version number.
  // content-type: text/html
  ASCOMServerTransactionID++;
  ASCOM_sendreply( NORMALWEBPAGE, TEXTPAGETYPE, Setup_HomePage);
  delay(10);                     // small pause so background tasks can run
}

void ASCOM_handle_focuser_setup()
{
  // url /setup/v1/focuser/0/setup
  // Configuration web page for the specified device
  // content-type: text/html

  // if set focuser position
  String fpos_str = ascomserver.arg("setpos");
  if ( fpos_str != "" )
  {
    Serial.print( "setpos:" );
    Serial.println(fpos_str);
    String fp = ascomserver.arg("fp");
    if ( fp != "" )
    {
      unsigned long temp = 0;
      Serial.print("fp:");
      Serial.println(fp);
      temp = fp.toInt();
      if ( temp > mySetupData->get_maxstep() )  // if greater than maxStep then set to maxStep
      {
        temp = mySetupData->get_maxstep();
      }
      fcurrentPosition = ftargetPosition = temp;
    }
  }

  // if update of maxsteps
  String fmax_str = ascomserver.arg("fm");
  if ( fmax_str != "" )
  {
    unsigned long temp = 0;
    DebugPrint( "root() -maxsteps:" );
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

  // if update motorspeed
  String fms_str = ascomserver.arg("ms");
  if ( fms_str != "" )
  {
    int temp1 = 0;
    DebugPrint( "root() -motorspeed:" );
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
  String fcp_str = ascomserver.arg("cp");
  if ( fcp_str != "" )
  {
    DebugPrint( "root() -coil power:" );
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
  String frd_str = ascomserver.arg("rd");
  if ( frd_str != "" )
  {
    DebugPrint( "root() -reverse direction:" );
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
  String fsm_str = ascomserver.arg("sm");
  if ( fsm_str != "" )
  {
    int temp1 = 0;
    DebugPrint( "root() -stepmode:" );
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

  DebugPrintln( "root() -build homepage" );

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
  ASCOMServerTransactionID++;
  ASCOM_sendreply( NORMALWEBPAGE, TEXTPAGETYPE, Setup_HomePage);
  delay(10);                     // small pause so background tasks can run
}

void start_ascomremoteserver(void)
{
  DebugPrintln("start_ascomserver()");
  ASCOM_Setup_HomePage();                       // create home page for ascom focuser setup
  ASCOM_Create_Setup_Focuser_HomePage();        // create home page for ascom management setup api

  ascomserver.on("/", ASCOM_handleRoot);        // handle root access
  ascomserver.onNotFound(ASCOM_handleNotFound); // handle url not found 404

  ascomserver.on("/management/apiversions", ASCOM_handleapiversions);
  ascomserver.on("/management/v1/description", ASCOM_handleapidescription);
  ascomserver.on("/management/v1/configureddevices", ASCOM_handleapiconfigureddevices);

  ascomserver.on("/setup", ASCOM_handle_setup);
  ascomserver.on("/setup/v1/focuser/0/setup", ASCOM_handle_focuser_setup);

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
  ascomserverstate = RUNNING;
  delay(10);                     // small pause so background tasks can run
}

void stop_ascomremoteserver(void)
{
  DebugPrintln("stop_ascomserver()");
  ascomserver.stop();
  ascomserverstate = STOPPED;
  delay(10);                     // small pause so background tasks can run
}
#endif // ifdef ASCOMREMOTE
// ASCOM REMOTE END ---------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// 28: OTAUPDATES - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#if defined(OTAUPDATES)
#include <ArduinoOTA.h>

void start_otaservice()
{
  DebugPrintln(startotaservicestr);
  oledtextmsg(startotaservicestr, -1, false, true);
  ArduinoOTA.setHostname(OTAName);                      // Start the OTA service
  ArduinoOTA.setPassword(OTAPassword);

  ArduinoOTA.onStart([]()
  {
    DebugPrintln(startstr);
  });
  ArduinoOTA.onEnd([]()
  {
    DebugPrintln(endstr);
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
    DebugPrint(progressstr);
    DebugPrintln((progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error)
  {
    DebugPrint(errorstr);
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
  DebugPrintln(readystr);
  otaservicestate = RUNNING;
}
#endif // #if defined(OTAUPDATES)

// ----------------------------------------------------------------------------------------------
// 29: DUCKDNS - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
#ifdef USEDUCKSDNS
#include <EasyDDNS.h>                           // https://github.com/ayushsharma82/EasyDDNS

void init_duckdns(void)
{
  DebugPrintln(setupduckdnsstr);
  oledtextmsg(setupduckdnsstr, -1, false, true);
  EasyDDNS.service("duckdns");                  // Enter your DDNS Service Name - "duckdns" / "noip"
  delay(5);
  EasyDDNS.client(duckdnsdomain, duckdnstoken); // Enter ddns Domain & Token | Example - "esp.duckdns.org","1234567"
  delay(5);
  EasyDDNS.update(DUCKDNSREFRESHRATE);          // Check for New Ip Every 60 Seconds.
  delay(5);
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
  oledtextmsg(wifirestartstr, -1, true, false);
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
#if defined(INOUTLEDS)
  ( dir == move_in ) ? digitalWrite(INLEDPIN, 1) : digitalWrite(OUTLEDPIN, 1);
#endif
  driverboard->movemotor(dir);
#if defined(INOUTLEDS)
  ( dir == move_in ) ? digitalWrite(INLEDPIN, 0) : digitalWrite(OUTLEDPIN, 0);
#endif
}

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

#if defined(INOUTLEDS)                                  // Setup IN and OUT LEDS, use as controller power up indicator
  pinMode(INLEDPIN, OUTPUT);
  pinMode(OUTLEDPIN, OUTPUT);
  digitalWrite(INLEDPIN, 1);
  digitalWrite(OUTLEDPIN, 1);
#endif

#if defined(PUSHBUTTONS)                           // Setup IN and OUT Pushbuttons, active high when pressed
  init_pushbuttons();
#endif

  displayfound = false;
#ifdef OLEDTEXT
  init_oledtextdisplay();
#endif
#if defined(OLEDGRAPHICS)
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
  init_temp();
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

    if (attempts > 9)                               // if this attempt is 10 or more tries
    {
<<<<<<< HEAD
      DebugPrintln(wifistartfailstr);
      DebugPrintln(wifirestartstr);
//      oledtextmsg(didnotconnectstr, -1, true, false);
      oledtextmsg("did not connect", -1, true, false);     // //  didnotconnectstr is not defined
      oledtextmsg(mySSID, -1, false, true);
      oledgraphicmsg("Did not connect to AP", -1, true);
=======
      DebugPrintln(apstartfailstr);
      oledtextmsg(apstartfailstr + String(mySSID), -1, true, true);
      oledgraphicmsg(apstartfailstr, -1, true);
>>>>>>> b531656bf26fcef42fb6f4f9acabe225296b7348
      delay(2000);
      software_Reboot(2000);                        // GPIO0 must be HIGH and GPIO15 LOW when calling ESP.restart();
    }
  }
#endif // end STATIONMODE

  oledtextmsg("Connected", -1, true, true);
  delay(100);                                       // keep delays small else issue with ASCOM

#if defined(ACCESSPOINT) || defined(STATIONMODE)
  // Starting TCP Server
  DebugPrintln(starttcpserverstr);
  oledtextmsg(starttcpserverstr, -1, false, true);

  myserver.begin();
  DebugPrintln(F("Get local IP address"));
  ESP32IPAddress = WiFi.localIP();
  delay(100);                                       // keep delays small else issue with ASCOM
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
  DebugPrint(PORTSTR);
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

  // setup duckdns
#ifdef USEDUCKSDNS
  init_duckdns();
#endif

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
  temp_setresolution(mySetupData->get_tempprecision());
  read_temp(1);
#endif

  mdnsserverstate = STOPPED;
  webserverstate = STOPPED;
  ascomserverstate = STOPPED;
  managementserverstate = STOPPED;
  otaservicestate = STOPPED;

#ifdef OTAUPDATES
  start_otaservice();                        // Start the OTA service
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

  DebugPrint(currentposstr);
  DebugPrintln(fcurrentPosition);
  DebugPrint(targetposstr);
  DebugPrintln(ftargetPosition);
  DebugPrintln(setupendstr);
  oledtextmsg(setupendstr, -1, false, true);

#if defined(INOUTLEDS)
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
  if ( otaservicestate == RUNNING )
  {
    ArduinoOTA.handle();                      // listen for OTA events
  }
#endif // ifdef OTAUPDATES


#ifdef ASCOMREMOTE
  if ( ascomserverstate == RUNNING)
  {
    ascomserver.handleClient();
  }
#endif

#ifdef WEBSERVER
  if ( webserverstate == RUNNING )
  {
    webserver->handleClient();
  }
#endif
#ifdef MANAGEMENT
  mserver.handleClient();
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

        if (mySetupData->SaveConfiguration(fcurrentPosition, DirOfTravel)) // save config if needed
        {
          Update_OledGraphics(oled_off);                // Display off after config saved
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
              update_oledtext_position();
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
