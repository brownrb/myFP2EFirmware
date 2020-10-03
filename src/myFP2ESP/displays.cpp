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
    drawString(64, 14, buffer);
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
  drawString(8, 14, buffer);

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

// ----------------------------------------------------------------------------------------------
// Section OLED TEXT
// ----------------------------------------------------------------------------------------------

void OLED_TEXT::oledtextmsg(String str, int val, boolean clrscr, boolean nl)
{
  if ( clrscr == true)                                  // clear the screen?
  {
    clear();
    setCursor(0, 0);
  }
  if ( nl == true )                                     // need to print a new line?
  {
    if ( val != -1)                                     // need to print a value?
    {
      print(str);
      println(val);
    }
    else
    {
      println(str);
    }
  }
  else
  {
    print(str);
    if ( val != -1 )
    {
      print(val);
    }
  }
  //  display();
}

void OLED_TEXT::displaylcdpage0(void)      // displaylcd screen
{
  char tempString[20];

  setCursor(0, 0);
  print(CURRENTPOSSTR);
  println(driverboard->getposition());
  print(TARGETPOSSTR);
  println(ftargetPosition);

  print(COILPWRSTR);
  println(mySetupData->get_coilpower());

  print(REVDIRSTR);
  println(mySetupData->get_reversedirection());

  // stepmode setting
  print(STEPMODESTR);
  println(mySetupData->get_stepmode());

  //Temperature
  print(TEMPSTR);
  if ( mySetupData->get_temperatureprobestate() == 1)
  {
    print(String(read_temp(0)));
  }
  else
  {
    print("20.0");
  }
  println(" c");

  //Motor Speed
  print(MOTORSPEEDSTR);
  println(mySetupData->get_motorSpeed());

  //MaxSteps
  print(MAXSTEPSSTR);
  ltoa(mySetupData->get_maxstep(), tempString, 10);
  println(tempString);
  //  display();
}

void OLED_TEXT::displaylcdpage1(void)
{
  setCursor(0, 0);
  // temperature compensation
  print(TCOMPSTEPSSTR);
  println(mySetupData->get_tempcoefficient());

  print(TCOMPSTATESTR);
  println(mySetupData->get_tempcompenabled());

  print(TCOMPDIRSTR);
  println(mySetupData->get_tcdirection());

  print(BACKLASHINSTR);
  println(mySetupData->get_backlash_in_enabled());

  print(BACKLASHOUTSTR);
  println(mySetupData->get_backlash_out_enabled());

  print(BACKLASHINSTEPSSTR);
  println(mySetupData->get_backlashsteps_in());

  print(BACKLASHOUTSTEPSSTR);
  println(mySetupData->get_backlashsteps_out());
  //  display();
}

void OLED_TEXT::displaylcdpage2(void)
{
#if defined(ACCESSPOINT) || defined(STATIONMODE)
  setCursor(0, 0);
#if defined(ACCESSPOINT)
  println(ACCESSPOINTSTR);
#endif
#if defined(STATIONMODE)
  println(STATIONMODESTR);
#endif
  print(SSIDSTR);
  print(mySSID);
  println();

  print(IPADDRESSSTR);
  print(ipStr);
  println();
#endif // if defined(ACCESSPOINT) || defined(STATIONMODE)

  if ( mySetupData->get_webserverstate() == 1)
  {
    setCursor(0, 0);
    println(WEBSERVERSTR);
#if defined(ACCESSPOINT)
    println(ACCESSPOINTSTR);
#endif
#if defined(STATIONMODE)
    println(STATIONMODESTR);
#endif
    print(IPADDRESSSTR);
    print(ipStr);
    print(STARTSTR);
    println(String(mySetupData->get_webserverport()));
  }
  if ( mySetupData->get_ascomserverstate() == 1)
  {
    setCursor(0, 0);
    println(ASCOMREMOTESTR);
    print(IPADDRESSSTR);
    print(ipStr);
    print(STARTSTR);
    println(mySetupData->get_ascomalpacaport());
  }

#if defined(BLUETOOTHMODE)
  setCursor(0, 0);
  print(BLUETOOTHSTR);
  println();
#endif
#if defined(LOCALSERIAL)
  setCursor(0, 0);
  println(LOCALSERIALSTR);
#endif
  //  display();
}

void OLED_TEXT::Update_OledText(void)
{
  static unsigned long currentMillis;
  static unsigned long olddisplaytimestampNotMoving = millis();
  static byte displaypage = 0;

  currentMillis = millis();                             // see if the display needs updating
  if (((currentMillis - olddisplaytimestampNotMoving) > ((int)mySetupData->get_lcdpagetime() * 1000)) || (currentMillis < olddisplaytimestampNotMoving))
  {
    olddisplaytimestampNotMoving = currentMillis;       // update the timestamp
    clear();                                            // clrscr OLED
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
}

void OLED_TEXT::UpdatePositionOledText(void)
{
  setCursor(0, 0);
  print(CURRENTPOSSTR);
  println(driverboard->getposition());

  print(TARGETPOSSTR);
  println(ftargetPosition);
  //  display();
}

void OLED_TEXT::update_oledtext_position(void)
{
  setCursor(0, 0);
  print(CURRENTPOSSTR);
  print(driverboard->getposition());
  clearToEOL();
  println();

  print(TARGETPOSSTR);
  print(ftargetPosition);
  clearToEOL();
  println();
  //  display();
}

OLED_TEXT::OLED_TEXT(void)
{

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
  Wire.beginTransmission(OLED_ADDR);                    //check if OLED display is present
  if (Wire.endTransmission() != 0)
  {
    TRACE();
    DebugPrintln(I2CDEVICENOTFOUNDSTR);
    displayfound = false;
  }
  else
  {
    displayfound = true;
    // Setup the OLED
#ifdef USE_SSD1306
    // For the OLED 128x64 0.96" display using the SSD1306 driver
    begin(&Adafruit128x64, OLED_ADDR);
#endif
#ifdef USE_SSH1106
    // For the OLED 128x64 1.3" display using the SSH1106 driver
    begin(&SH1106_128x64, OLED_ADDR);
#endif

    set400kHz();
    setFont(Adafruit5x7);
    //setcolor(WHITE);                    // Draw white text
    clear();                            // clrscr OLED
    //Display_Normal();                 // black on white
    //Display_On();                     // display ON
    //Display_Rotate(0);                // portrait, not rotated
    //Display_Bright();
    if ( mySetupData->get_showstartscreen() )
    {
      println(DRVBRD_ID);                 // print startup screen
      println(programVersion);
      println(ProgramAuthor);
    }
  }
}

// ----------------------------------------------------------------------------------------------
// 19B: OLED TEXT DISPLAY - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------

// Enclose function code in #ifdef - #endif so function declarations are visible

void OLED_TEXT::display_oledtext_page0(void)           // displaylcd screen
{
  char tempString[20];
  home();
  print(CURRENTPOSSTR);
  print(driverboard->getposition());
  clearToEOL();

  println();
  print(TARGETPOSSTR);
  print(ftargetPosition);
  clearToEOL();
  println();

  print(COILPWRSTR);
  print(mySetupData->get_coilpower());
  clearToEOL();
  println();

  print(REVDIRSTR);
  print(mySetupData->get_reversedirection());
  clearToEOL();
  println();

  // stepmode setting
  print(STEPMODESTR);
  print(mySetupData->get_stepmode());
  clearToEOL();
  println();

  //Temperature
  print(TEMPSTR);
  if ( mySetupData->get_temperatureprobestate() == 1)
  {
    print(String(read_temp(0), 2));
  }
  else
  {
    print("20.0");
  }
  print(" c");
  clearToEOL();
  println();

  //Motor Speed
  print(MOTORSPEEDSTR);
  print(mySetupData->get_motorSpeed());
  clearToEOL();
  println();

  //MaxSteps
  print(MAXSTEPSSTR);
  ltoa(mySetupData->get_maxstep(), tempString, 10);
  print(tempString);
  clearToEOL();
  println();
}

void OLED_TEXT::display_oledtext_page1(void)
{
  // temperature compensation
  print(TCOMPSTEPSSTR);
  print(mySetupData->get_tempcoefficient());
  clearToEOL();
  println();

  print(TCOMPSTATESTR);
  print(mySetupData->get_tempcompenabled());
  clearToEOL();
  println();

  print(TCOMPDIRSTR);
  print(mySetupData->get_tcdirection());
  clearToEOL();
  println();

  print(BACKLASHINSTR);
  print(mySetupData->get_backlash_in_enabled());
  clearToEOL();
  println();

  print(BACKLASHOUTSTR);
  print(mySetupData->get_backlash_out_enabled());
  clearToEOL();
  println();

  print(BACKLASHINSTEPSSTR);
  print(mySetupData->get_backlashsteps_in());
  clearToEOL();
  println();

  print(BACKLASHOUTSTEPSSTR);
  print(mySetupData->get_backlashsteps_out());
  clearToEOL();
  println();
}

void OLED_TEXT::display_oledtext_page2(void)
{
#if defined(ACCESSPOINT) || defined(STATIONMODE)
  setCursor(0, 0);
#if defined(ACCESSPOINT)
  print(ACCESSPOINTSTR);
  clearToEOL();
  println();
#endif
#if defined(STATIONMODE)
  print(STATIONMODESTR);
  clearToEOL();
  println();
#endif
  print(SSIDSTR);
  print(mySSID);
  clearToEOL();
  println();
  print(IPADDRESSSTR);
  print(ipStr);
  clearToEOL();
  println();
#endif // if defined(ACCESSPOINT) || defined(STATIONMODE)

  if ( mySetupData->get_webserverstate() == 1)
  {
    //setCursor(0, 0);
    print(WEBSERVERSTR);
    clearToEOL();
    println();
    print(IPADDRESSSTR);
    print(ipStr);
    clearToEOL();
    println();
    print(PORTSTR);
    print(String(mySetupData->get_webserverport()));
    clearToEOL();
    println();
  }
  if ( mySetupData->get_ascomserverstate() == 1)
  {
    print(ASCOMREMOTESTR);
    clearToEOL();
    println();
    print(IPADDRESSSTR);
    print(ipStr);
    clearToEOL();
    println();
    print(PORTSTR);
    print(mySetupData->get_ascomalpacaport());
    clearToEOL();
    println();
  }

#if defined(BLUETOOTHMODE)
  setCursor(0, 0);
  print(BLUETOOTHSTR);
  clearToEOL();
  println();
#endif

#if defined(LOCALSERIAL)
  setCursor(0, 0);
  println(LOCALSERIALSTR);
#endif
}

void OLED_TEXT::update_oledtextdisplay(void)
{
  static unsigned long currentMillis;
  static unsigned long olddisplaytimestampNotMoving = millis();
  static byte displaypage = 0;

  currentMillis = millis();                       // see if the display needs updating
  if (((currentMillis - olddisplaytimestampNotMoving) > ((int)mySetupData->get_lcdpagetime() * 1000)) || (currentMillis < olddisplaytimestampNotMoving))
  {
    olddisplaytimestampNotMoving = currentMillis; // update the timestamp
    clear();                              // clrscr OLED
    switch (displaypage % 3)
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
  }
}

// ----------------------------------------------------------------------------------------------
// OLED TEXT DISPLAY - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
// Enclose function code in #ifdef - #endif so function declarations are visible

/*
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
    myoled->print(CURRENTPOSSTR);
    myoled->print(driverboard->getposition());
    myoled->clearToEOL();

    myoled->println();
    myoled->print(TARGETPOSSTR);
    myoled->print(ftargetPosition);
    myoled->clearToEOL();
    myoled->println();

    myoled->print(COILPWRSTR);
    myoled->print(mySetupData->get_coilpower());
    myoled->clearToEOL();
    myoled->println();

    myoled->print(REVDIRSTR);
    myoled->print(mySetupData->get_reversedirection());
    myoled->clearToEOL();
    myoled->println();

    // stepmode setting
    myoled->print(STEPMODESTR);
    myoled->print(mySetupData->get_stepmode());
    myoled->clearToEOL();
    myoled->println();

    //Temperature
    myoled->print(TEMPSTR);
#ifdef TEMPERATUREPROBE
    myoled->print(String(read_temp(0), 2));
#else
    myoled->print("20.0");
#endif
    myoled->print(" c");
    myoled->clearToEOL();
    myoled->println();

    //Motor Speed
    myoled->print(MOTORSPEEDSTR);
    myoled->print(mySetupData->get_motorSpeed());
    myoled->clearToEOL();
    myoled->println();

    //MaxSteps
    myoled->print(MAXSTEPSSTR);
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
    myoled->print(TCOMPSTEPSSTR);
    myoled->print(mySetupData->get_tempcoefficient());
    myoled->clearToEOL();
    myoled->println();

    myoled->print(TCOMPSTATESTR);
    myoled->print(mySetupData->get_tempcompenabled());
    myoled->clearToEOL();
    myoled->println();

    myoled->print(TCOMPDIRSTR);
    myoled->print(mySetupData->get_tcdirection());
    myoled->clearToEOL();
    myoled->println();

    myoled->print(BACKLASHINSTR);
    myoled->print(mySetupData->get_backlash_in_enabled());
    myoled->clearToEOL();
    myoled->println();

    myoled->print(BACKLASHOUTSTR);
    myoled->print(mySetupData->get_backlash_out_enabled());
    myoled->clearToEOL();
    myoled->println();

    myoled->print(BACKLASHINSTEPSSTR);
    myoled->print(mySetupData->get_backlashsteps_in());
    myoled->clearToEOL();
    myoled->println();

    myoled->print(BACKLASHOUTSTEPSSTR);
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
    myoled->print(ACCESSPOINTSTR);
    myoled->clearToEOL();
    myoled->println();
#endif
#if defined(STATIONMODE)
    myoled->print(STATIONMODESTR);
    myoled->clearToEOL();
    myoled->println();
#endif
    myoled->print(SSIDSTR);
    myoled->print(mySSID);
    myoled->clearToEOL();
    myoled->println();
    myoled->print(IPADDRESSSTR);
    myoled->print(ipStr);
    myoled->clearToEOL();
    myoled->println();
#endif // if defined(ACCESSPOINT) || defined(STATIONMODE)

#if defined(WEBSERVER)
    //myoled->setCursor(0, 0);
    myoled->print(WEBSERVERSTR);
    myoled->clearToEOL();
    myoled->println();
    myoled->print(IPADDRESSSTR);
    myoled->print(ipStr);
    myoled->clearToEOL();
    myoled->println();
    myoled->print(PORTSTR);
    myoled->print(String(mySetupData->get_webserverport()));
    myoled->clearToEOL();
    myoled->println();
#endif // webserver
#if defined(ASCOMREMOTE)
    myoled->print(ASCOMREMOTESTR);
    myoled->clearToEOL();
    myoled->println();
    myoled->print(IPADDRESSSTR);
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
    myoled->print(BLUETOOTHSTR);
    myoled->clearToEOL();
    myoled->println();
#endif

#if defined(LOCALSERIAL)
    myoled->setCursor(0, 0);
    myoled->println(LOCALSERIALSTR);
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
    myoled->print(CURRENTPOSSTR);
    myoled->print(driverboard->getposition());
    myoled->clearToEOL();
    myoled->println();

    myoled->print(TARGETPOSSTR);
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
  DebugPrintln("Setup PRO2EL293DNEMA/PRO2EL293D28BYJ48 I2C");
#else
  DebugPrintln("Setup esp8266 I2C");
  Wire.begin();
#endif
#else
  DebugPrintln("Setup esp32 I2C");
  Wire.begin(I2CDATAPIN, I2CCLKPIN);        // esp32
#endif
  Wire.beginTransmission(OLED_ADDR);        //check if OLED display is present
  if (Wire.endTransmission() != 0)
  {
    TRACE();
    DebugPrintln(I2CDEVICENOTFOUNDSTR);
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
*/
