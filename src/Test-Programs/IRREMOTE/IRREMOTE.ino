// myFP2ESP - Firmware for ESP8266 and ESP32 myFocuserPro2 Controllers
// IRREMOTE Test Program [ESP32]
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
// ----------------------------------------------------------------------------------------------
// 1: SPECIFY DRIVER BOARD HERE
// ----------------------------------------------------------------------------------------------
// DRIVER BOARDS - Please specify your driver board here, only 1 can be defined, see DRVBRD line
#include "myBoardTypes.h"

//Set DRVBRD to the correct driver board above, ONLY ONE!!!!
//#define DRVBRD PRO2ESP32DRV8825
#define DRVBRD PRO2ESP32ULN2003
//#define DRVBRD PRO2ESP32L298N
//#define DRVBRD PRO2ESP32L293DMINI
//#define DRVBRD PRO2ESP32L9110S

// DO NOT CHANGE
#ifndef DRVBRD    // error checking, please do NOT change
#halt // ERROR you must have DRVBRD defined
#endif

// DO NOT CHANGE - Order is very important
#include "generalDefinitions.h"
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

// ----------------------------------------------------------------------------------------------------------
// 4: SPECIFY HARDWARE OPTIONS HERE
// ----------------------------------------------------------------------------------------------------------

#define INFRAREDREMOTE

// DO NOT CHANGE
#if (DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N \
  || DRVBRD == PRO2EL293DMINI || DSRVBRD == PRO2EL9110S)
// no support for pushbuttons, inout leds, irremote
#ifdef INFRAREDREMOTE
#halt // ERROR - INFRAREDREMOTE not supported for WEMOS or NODEMCUV1 ESP8266 chips
#endif
#endif // 

// ----------------------------------------------------------------------------------------------
// 5: SPECIFY THE TYPE OF OLED DISPLAY HERE
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// 6: SPECIFY THE CONTROLLER MODE HERE - ONLY ONE OF THESE MUST BE DEFINED
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// 7. INCLUDES FOR WIFI
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// 8. WIFI NETWORK SSID AND PASSWORD CONFIGURATION
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// 9. DUCKDNS DOMAIN AND TOKEN CONFIGURATION
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// 10. STATIC IP ADDRESS CONFIGURATION
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// 11. FIRMWARE CODE START - INCLUDES AND LIBRARIES
// ----------------------------------------------------------------------------------------------

#include <IRremoteESP32.h>

// ----------------------------------------------------------------------------------------------
// 12. BLUETOOTH MODE - Do not change
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// 13. GLOBAL DATA -- DO NOT CHANGE
// ----------------------------------------------------------------------------------------------

IRrecv irrecv(IRPIN);
decode_results results;

// ----------------------------------------------------------------------------------------------------------
// 16. CODE START - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------------------

#ifdef INFRAREDREMOTE
void update_irremote()
{
  // check IR
  if (irrecv.decode(&results))
  {
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
        Serial.println("CH-");
        break;
      case 16769565:                  // CH+ OUT +1 SLOW
        Serial.println("CH+");
        break;
      case 16720605:                  // |<< IN -10 MEDIUM
        Serial.println("|<<");
        break;
      case 16761405:                  // >>| OUT +10 MEDIUM
        Serial.println("|>>");
        break;
      case 16769055:                  // '-' IN -50 FAST
        Serial.println("-");
        break;
      case 16754775:
        Serial.println("+");
        break;
      case 16748655:                  // 'EQ' OUT +50 FAST
        Serial.println("EQ");
        break;
      case 16738455 :                 // 0 RESET POSITION TO 0
        Serial.println("0");
        break;
    }
    irrecv.resume();                  // Receive the next value
  }
}
#endif

void setup()
{
  Serial.begin(SERIALPORTSPEED);

  Serial.println("started serial port");
  Serial.println("Start IR");

#ifdef INFRAREDREMOTE
  Serial.println("Start IR");
  irrecv.enableIRIn();                              // Start the IR
#endif
  Serial.println("End setup");
}

//_____________________ loop()___________________________________________

void loop()
{
#ifdef INFRAREDREMOTE
  update_irremote();
#endif

}
