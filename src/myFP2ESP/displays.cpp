// ----------------------------------------------------------------------------------------------
// display.cpp : myFP2ESP OLED DISPLAY ROUTINES
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2020. All Rights Reserved.
// (c) Copyright Holger M, 2020. All Rights Reserved.
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// EXTERNALS
// ----------------------------------------------------------------------------------------------
#if defined(OLEDGRAPHICS)

#include <Arduino.h>
#include "myBoards.h"
#include "focuserconfig.h"
#include "FocuserSetupData.h"
#include "images.h"
#include "generalDefinitions.h"
#include "displays.h"

extern bool TimeCheck(unsigned long, unsigned long);

//__ helper function

bool OLED_NON::OledConnected(void)
{
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
  Serial.println(F(""));
#endif

  Wire.beginTransmission(OLED_ADDR);                    //check if OLED display is present
  if (Wire.endTransmission() != 0)
  {
    DebugPrintln(F("no I2C device found"));
    return false;
  }
  return true;
}

// ----------------------------------------------------------------------------------------------
// CODE NON OLED
// ----------------------------------------------------------------------------------------------

void OLED_NON::oledgraphicmsg(String &str, int val, boolean clrscr) {}
void OLED_NON::oled_draw_Wifi(int j) {}
void OLED_NON::oledtextmsg(String str, int val, boolean clrscr, boolean nl) {}
void OLED_NON::update_oledtext_position(void) {}
void OLED_NON::update_oledtextdisplay(void) {}
void OLED_NON::Update_Oled(const oled_state x, const connection_status y) {}
void OLED_NON::oled_draw_reboot(void) {}
//void OLED_NON::println(const char *c){}

OLED_NON::OLED_NON()
{
  connected = OledConnected();
}

// ----------------------------------------------------------------------------------------------
// CODE OLED GRAPHICMODE
// ----------------------------------------------------------------------------------------------

//__constructor
//OLED_GRAPHIC::OLED_GRAPHIC(uint8_t _address, uint8_t _sda, uint8_t _scl)  :   SSD1306Wire(_address, _sda,_scl, GEOMETRY_128_64)
OLED_GRAPHIC::OLED_GRAPHIC()  :  SSD1306Wire(OLED_ADDR, I2CDATAPIN, I2CCLKPIN, GEOMETRY_128_64) , OLED_NON()
{
  if (connected)
  {

    Serial.println(F("start init()"));
    delay(1000);
    this->init();
    delay(1000);

    flipScreenVertically();
    setFont(ArialMT_Plain_10);
    setTextAlignment(TEXT_ALIGN_LEFT);
    clear();
    if ( mySetupData->get_showstartscreen() )
    {
      drawString(0, 0, "myFocuserPro2 v:" + String(programVersion));
      drawString(0, 12, ProgramAuthor);
    }
    display();

    timestamp = millis();
  }
}

void OLED_GRAPHIC::Update_Oled(const oled_state oled, const connection_status ConnectionStatus)
{
  if (connected)
  {
    if (TimeCheck(timestamp, 750))
    {
      timestamp = millis();

      if (oled == oled_on)
      {
        oled_draw_main_update(ConnectionStatus);
      }
      else
      {
        clear();
        display();
      }
    }
  }
}


void OLED_GRAPHIC::oledgraphicmsg(String &str, int val, boolean clrscr)
{
  setTextAlignment(TEXT_ALIGN_LEFT);
  setFont(ArialMT_Plain_10);

  if (clrscr == true)
  {
    clear();
    linecount = 0;
  }
  if (val != -1)
  {
    str += String(val);
  }
  drawString(0, linecount * 12, str);
  display();
  linecount++;

}

void OLED_GRAPHIC::oled_draw_Wifi(int j)
{
  clear();
  setTextAlignment(TEXT_ALIGN_CENTER);
  setFont(ArialMT_Plain_10);
  drawString(64, 0, "SSID: " + String(mySSID));
  drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits); // draw wifi logo

  for (int i = 1; i < 10; i++)
    drawXbm(12 * i, 56, 8, 8, (i == j) ? activeSymbol : inactiveSymbol);

  display();

}

const char heartbeat[] = { '-', '/' , '|', '\\'};

void OLED_GRAPHIC::oled_draw_main_update(const connection_status ConnectionStatus)
{
  char buffer[80];

  clear();
  setTextAlignment(TEXT_ALIGN_CENTER);
  setFont(ArialMT_Plain_24);

  if (ConnectionStatus == disconnected)
  {
    drawString(64, 28, F("offline"));

    setFont(ArialMT_Plain_10);
    //    drawString(64, 0, driverboard->getboardname());
    drawString(64, 0, DRVBRD_ID);
    snprintf(buffer, sizeof(buffer), "IP= %s", ipStr);
    drawString(64, 12, buffer);
  }
  else
  {
    char dir = (mySetupData->get_focuserdirection() == moving_in ) ? '<' : '>';
    snprintf(buffer, sizeof(buffer), "%lu:%i %c", driverboard->getposition(), (int)(driverboard->getposition() % driverboard->getstepmode()), dir);
    drawString(64, 28, buffer);

    setFont(ArialMT_Plain_10);
    snprintf(buffer, sizeof(buffer), "µSteps: %i MaxPos: %lu", driverboard->getstepmode(), mySetupData->get_maxstep());
    drawString(64, 0, buffer);
    snprintf(buffer, sizeof(buffer), "TargetPos:  %lu", ftargetPosition);
    drawString(64, 12, buffer);
  }

  setTextAlignment(TEXT_ALIGN_LEFT);

  if ( mySetupData->get_temperatureprobestate() == 1)
  {
    snprintf(buffer, sizeof(buffer), "TEMP: %.2f C", read_temp(0));
    drawString(54, 54, buffer);
  }
  else
  {
    snprintf(buffer, sizeof(buffer), "TEMP: %.2f C", 20.0);
  }

  snprintf(buffer, sizeof(buffer), "BL: %i", mySetupData->get_backlashsteps_out());
  drawString(0, 54, buffer);

  snprintf(buffer, sizeof(buffer), "%c", heartbeat[++count_hb % 4]);
  drawString(8, 12, buffer);

  display();
}

void OLED_GRAPHIC::oled_draw_reboot(void)
{
  clear();
  setTextAlignment(TEXT_ALIGN_CENTER);
  setFont(ArialMT_Plain_24);
  drawString(64, 28, "REBOOT"); // Print currentPosition
  display();
}

*/ 

#endif // #if defined(OLEDGRAPHICS)
