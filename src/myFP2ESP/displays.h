// ----------------------------------------------------------------------------------------------
// myFP2ESP DISPLAY ROUTINES AND DEFINITIONS
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2019. All Rights Reserved.
// (c) Copyright Holger M, 2019. All Rights Reserved.
// ----------------------------------------------------------------------------------------------

#ifndef displays_h
#define displays_h

// ----------------------------------------------------------------------------------------------
// EXTERNALS
// ----------------------------------------------------------------------------------------------
extern SetupData *mySetupData;
extern unsigned long fcurrentPosition;      // current focuser position
extern unsigned long ftargetPosition;       // target position
extern String ipStr;                        // ip address
extern boolean displayfound;

#ifdef TEMPERATUREPROBE
extern float readtemp(byte);
#endif
#if defined(OLEDGRAPHICS)
extern SSD1306Wire *myoled;
#endif
#if defined(OLEDTEXT)
extern Adafruit_SSD1306 *myoled;
#endif

// ----------------------------------------------------------------------------------------------
// DATA AND DEFINITIONS
// ----------------------------------------------------------------------------------------------
#define OLED_ADDR             0x3C          // some OLED displays maybe at 0x3F, use I2Cscanner to find the correct address#endif // wemos
#define SCREEN_WIDTH          128           // OLED display width, in pixels
#define SCREEN_HEIGHT         64            // OLED display height, in pixels

// ----------------------------------------------------------------------------------------------
// CODE
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
#ifdef OLEDTEXT
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

void displaylcdpage0(void)      // displaylcd screen
{
  if ( displayfound == false)
  {
    return;
  }
#ifdef OLEDTEXT
  char tempString[20];
  myoled->setCursor(0, 0);
  myoled->print(currentposstr);
  myoled->println(fcurrentPosition);
  myoled->print(targetposstr);
  myoled->println(ftargetPosition);

  myoled->print(coilpwrstr);
  myoled->println(mySetupData->get_coilpower());

  myoled->print(revdirstr);
  myoled->println(mySetupData->get_reversedirection());

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
  myoled->println(mySetupData->get_motorSpeed());

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
#ifdef OLEDTEXT
  myoled->setCursor(0, 0);
  // temperature compensation
  myoled->print(tcompstepsstr);
  myoled->println(mySetupData->get_tempcoefficient());

  myoled->print(tcompstatestr);
  myoled->println(mySetupData->get_tempcompenabled());


  myoled->print(tcompdirstr);
  myoled->println(mySetupData->get_tcdirection());

  myoled->print(backlashinstr);
  myoled->println(mySetupData->get_backlash_in_enabled());

  myoled->print(backlashoutstr);
  myoled->println(mySetupData->get_backlash_out_enabled());

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
#ifdef OLEDTEXT
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

void Update_OledText(void)
{
  if ( displayfound == false)
  {
    return;
  }
#ifdef OLEDTEXT
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
#ifdef OLEDTEXT
  myoled->setCursor(0, 0);
  myoled->print(currentposstr);
  myoled->println(fcurrentPosition);

  myoled->print(targetposstr);
  myoled->println(ftargetPosition);
  myoled->display();
#endif // if defined(OLEDTEXT)
}

#ifdef OLEDTEXT
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


#endif
