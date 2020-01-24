// ----------------------------------------------------------------------------------------------
// myFP2ESP FOCUSER CONFIG DEFINITIONS - SPECIFY HARDWARE OPTIONS AND CONTROLLER MODES HERE
// ----------------------------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2020. All Rights Reserved.
// (c) Copyright Holger M, 2019-2020. All Rights Reserved.
// ----------------------------------------------------------------------------------------------

#include <Arduino.h>
#include "myBoards.h"

#ifndef focuserconfig_h
#define focuserconfig_h

// ------------------------------------------------------------------------------
// 1: SPECIFY HARDWARE OPTIONS HERE
// ------------------------------------------------------------------------------
// Caution: Do not enable a feature if the associated hardware circuits are not fitted on the board
// Enable or disable the specific hardware below

// To enable temperature probe, uncomment the next line
//#define TEMPERATUREPROBE 1

// To enable the OLED TEXT DISPLAY uncomment the next line
//#define OLEDTEXT 1

// To enable the OLED GRAPHICS DISPLAY uncomment the next line
//#define OLEDGRAPHICS 1

// do NOT uncomment HOMEPOSITIONSWITCH if you do not have the switch fitted
// To enable the HOMEPOSITION SWITCH [ESP32 only], uncomment the next line
//#define HOMEPOSITIONSWITCH 1

// To enable backlash in this firmware, uncomment the next line
//#define BACKLASH 2

// To enable In and Out Pushbuttons [ESP32 only], uncomment the next line
//#define PUSHBUTTONS 1

// To enable the 2-Axis Joystick [ESP32 only], uncomment the next line
//#define JOYSTICK1 1

// To enable the Keyes KY-023 PS2 2-Axis Joystick [ESP32 only], uncomment the next line
//#define JOYSTICK2 2

// To enable In and Out LEDS [ESP32 only], uncomment the next line
//#define INOUTLEDS 1

// To enable the Infrared remote controller [ESP32 only], uncomment the next line
//#define INFRAREDREMOTE

// To enable the start boot screen showing startup messages, uncomment the next line
//#define SHOWSTARTSCRN 1

// To display Splash screen graphic, uncomment the next line
//#define SPLASHSCREEN 1

// ----------------------------------------------------------------------------------------------
// 2: SPECIFY THE CONTROLLER MODE HERE - THERE ARE RESTRICTIONS ON WHAT CAN BE COMBINED TOGETHER
// ----------------------------------------------------------------------------------------------

// to enable Bluetooth mode, uncomment the next line [ESP32 only]
//#define BLUETOOTHMODE 1

// to work as an access point, define accesspoint - cannot use DUCKDNS
#define ACCESSPOINT 2

// to work as a station accessing a AP, define stationmode
//#define STATIONMODE 3

// to work only via USB cable as Serial port, uncomment the next line
//#define LOCALSERIAL 4

// To enable OTA updates, uncomment the next line [only works in STATIONMODE]
//#define OTAUPDATES 5

// to enable this focuser for ASCOM ALPACA REMOTE support [Port 4040], uncomment the next line
//#define ASCOMREMOTE 6

// [recommend use Internet Explorer or Microsoft Edge Browser]
// to enable Webserver interface [Port 80], uncomment the next line 
#define WEBSERVER 7

// mdns support [myfp2eap.local:8080]
// to enable multicast DNS, uncomment the next line [only works in STATIONMODE]
//#define MDNSSERVER 8

// Management Server Control Interface [Port 6060] - DO NOT CHANGE
#define MANAGEMENT 9
#define MANAGEMENTFORCEDOWNLOAD 1

// To make the firmware return the correct firmware value when talking to a
// myFocuserpro2 INDI driver [use only for INDI support], uncomment the following line
//#define INDI

// Cannot use DuckDNS with ACCESSPOINT, BLUETOOTHMODE or LOCALSERIAL mode
// To enable DUCKDNS [STATIONMODE only]
//#define USEDUCKDNS 1

// to enable reading SSID and PASSWORD 
// from SPIFFS file wificonfig at boot time, uncomment the following file
//#define READWIFICONFIG 1

// ----------------------------------------------------------------------------------------------------------
// 3: SPECIFY OLED DISPLAY TYPE
// ----------------------------------------------------------------------------------------------------------
// only uncomment one of the following OLEDxxxx lines depending upon your lcd type
// For the OLED 128x64 0.96" display using the SSD1306 driver, uncomment the following line
#define USE_SSD1306   1

// For the OLED 128x64 1.3" display using the SSH1306 driver, uncomment the following line
//#define USE_SSH1306   2

// DO NOT CHANGE
#ifdef OLEDDISPLAY
#if !defined(USE_SSD1306) && !defined(USE_SSH1306)
#halt //Error - you can must define either USE_SSD1306 or USE_SSH1306 is using an OLEDDISPLAY
#endif
#endif

// ------------------------------------------------------------------------------
// 4: DO NOT CHANGE: OPTIONS DRIVER BOARD CHECKS
// ------------------------------------------------------------------------------

#ifndef DRVBRD
#halt // ERROR you must have DRVBRD defined in myBoards.h
#endif

// DO NOT CHANGE
#if (DRVBRD == WEMOSDRV8825 || DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2EDRV8825BIG \
  || DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N   || DRVBRD == PRO2EL293DMINI \
  || DSRVBRD == PRO2EL9110S || DRVBRD == PRO2EL293D   || DRVBRD == PRO2ESP32R3WEMOS )
// no support for pushbuttons, inout leds, irremote
#ifdef PUSHBUTTONS
#halt // ERROR - PUSHBUTTONS not supported for WEMOS or NODEMCUV1 ESP8266 chips
#endif
#ifdef INOUTLEDS
#halt // ERROR - INOUTLEDS not supported for WEMOS or NODEMCUV1 ESP8266 chips
#endif
#ifdef INFRAREDREMOTE
#halt // ERROR - INFRAREDREMOTE not supported for WEMOS or NODEMCUV1 ESP8266 chips
#endif
#if defined(JOYSTICK1) || defined(JOYSTICK2)
#halt // ERROR - JOYSTICK not supported for WEMOS or NODEMCUV1 ESP8266 chips
#endif
#endif // 

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

#if defined(JOYSTICK1) || defined(JOYSTICK2)
#ifdef PUSHBUTTONS
#halt // ERROR - you cannot have PUSHBUTTONS and JOYSTICK enabled at the same time
#endif
#endif

#ifdef JOYSTICK1
#ifdef JOYSTICK2
#halt // ERROR - you cannot have both JOYSTICK1 or JOYSTICK2 defined at the same time
#endif
#endif

#ifdef OLEDTEXT
#ifdef OLEDGRAPHICS
#halt // ERROR - you cannot have both OLEDTEXT or OLEDGRAPHICS defined at the same time
#endif
#endif

// DO NOT CHANGE
#if defined(MDNSSERVER)
#if defined(BLUETOOTHMODE) || defined(LOCALSERIAL) || defined(ACCESSPOINT)
#halt // ERROR, mDNS only available with STATIONMODE
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
//#if defined(WEBSERVER)
//#halt //ERROR you cannot have both ASCOMREMOTE and WEBSERVER enabled at the same time
//#endif
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
  || DSRVBRD == PRO2EL9110S || DRVBRD == PRO2EL293D )
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

// cannot use DuckDNS with ACCESSPOINT, BLUETOOTHMODE or LOCALSERIAL mode
#ifdef USEDUCKDNS
#if defined(BLUETOOTHMODE) || defined(LOCALSERIAL) || defined(ACCESSPOINT)
#halt // Error- DUCKDNS only works with STATIONMODE
#endif
#ifndef STATIONMODE
#halt // Error- DUCKDNS only works with STATIONMODE, you must enable STATIONMODE
#endif
#endif

#ifdef LOCALSERIAL
#ifdef OTAUPDATES
#halt // Error Cannot enable OTAUPDATES with LOCALSERIAL
#endif
#ifdef ASCOMREMOTE
#halt // Error Cannot enable ASCOMREMOTE with LOCALSERIAL
#endif
#ifdef WEBSERVER
#halt // Error Cannot enable WEBSERVER with LOCALSERIAL
#endif
#ifdef MDNSSERVER
#halt // Error Cannot enable MDNSSERVER with LOCALSERIAL
#endif
#ifdef MANAGEMENT
#halt // Error Cannot enable MANAGEMENT with LOCALSERIAL
#endif
#ifdef DEBUG
#halt // Error Cannot enable DEBUG with LOCALSERIAL
#endif
#endif

#ifdef INDI
#ifndef LOCALSERIAL
#halt // ERROR Cannot enable INDI without also enabling LOCALSERIAL
#endif
#endif

#ifdef MANAGEMENT
#if !defined(ACCESSPOINT) && !defined(STATIONMODE)
#halt // ERROR You must have ACCESSPOINT or STATIONMODE defined to enable the MANAGEMENT console
#endif
#endif


#endif
