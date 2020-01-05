// myFP2ESP - Firmware for ESP8266 and ESP32 myFocuserPro2 Controllers
// IRREMOTE Test Program [ESP32]

// DOWNLOAD THIS IRREMOTE LIBRARY
// https://github.com/crankyoldgit/IRremoteESP8266
//
// THEN INSTALL LIBRARY FROM IDE-Sketch-Include Library-Add ZIP library

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#define IRPIN 15
const uint16_t RECV_PIN = IRPIN;
IRrecv irrecv(RECV_PIN);
decode_results results;

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

void setup()
{
  Serial.begin(115200);

  Serial.println("started serial port");
  Serial.println("Start IR");

  Serial.println("Start IR");
  irrecv.enableIRIn();                              // Start the IR
  Serial.println("End setup");
}

//_____________________ loop()___________________________________________

void loop()
{
  update_irremote();
}
