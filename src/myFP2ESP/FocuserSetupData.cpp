// ---------------------------------------------------------------------------
// myFP2ESP FOCUSER DATA ROUTINES
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// COPYRIGHT
// ---------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2021. All Rights Reserved.
// (c) Copyright Holger M, 2019-2021. All Rights Reserved.
// ---------------------------------------------------------------------------

#include <ArduinoJson.h>

#if defined(ESP8266)
#include "FS.h"
#else
#include "SPIFFS.h"
#endif

#include "FocuserSetupData.h"
#include "generalDefinitions.h"

// delay(10) required in ESP8266 code arounf file handling

SetupData::SetupData(void)
{
  DebugPrintln("Constructor Setupdata");

  this->SnapShotMillis = millis();
  this->ReqSaveData_var  = false;
  this->ReqSaveData_per = false;

  if (!SPIFFS.begin())
  {
    DebugPrintln(F("FS !mounted"));
    DebugPrintln(F("Formatting, please wait..."));
    SPIFFS.format();
    DebugPrintln(F("Format FS done"));
  }
  else
  {
    DebugPrintln("FS mounted");
    this->ListDir("/", 0);
  }
  this->LoadConfiguration();
};

// Loads the configuration from a file
byte SetupData::LoadConfiguration()
{
  byte retval = 0;

  // Open file for reading
  File file = SPIFFS.open(filename_persistant, "r");
  delay(10);

  if (!file)
  {
    DebugPrintln(F("no persistant data file, load default values"));
    LoadDefaultPersistantData();
    delay(10);
  }
  else
  {
    delay(10);
    String data = file.readString();                  // read content of the text file
    DebugPrint(F("FS. Persistant SetupData= "));
    DebugPrintln(data);                               // ... and print on serial

    // Allocate a temporary JsonDocument
    DynamicJsonDocument doc_per(DEFAULTDOCSIZE);

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc_per, data);
    if (error)
    {
      DebugPrintln(F("Failed to read persistant data file, using default configuration"));
      LoadDefaultPersistantData();
    }
    else
    {
      this->maxstep               = doc_per["maxstep"];                   // max steps
      this->stepsize              = doc_per["stepsize"];                  // the step size in microns, ie 7.2 - value * 10, so real stepsize = stepsize / 10 (maxval = 25.6)
      this->DelayAfterMove        = doc_per["delayaftermove"];            // delay after movement is finished (maxval=256)
      this->backlashsteps_in      = doc_per["backlashsteps_in"];          // number of backlash steps to apply for IN moves
      this->backlashsteps_out     = doc_per["backlashsteps_out"];         // number of backlash steps to apply for OUT moves
      this->backlash_in_enabled   = doc_per["backlash_in_enabled"];
      this->backlash_out_enabled  = doc_per["backlash_out_enabled"];
      this->tempcoefficient       = doc_per["tempcoefficient"];           // steps per degree temperature coefficient value (maxval=256)
      this->tempresolution        = doc_per["tempresolution"];            // 9 -12
      this->stepmode              = doc_per["stepmode"];
      this->coilpower             = doc_per["coilpwr"];
      this->reversedirection      = doc_per["rdirection"];
      this->stepsizeenabled       = doc_per["stepsizestate"];             // if 1, controller returns step size
      this->tempmode              = doc_per["tempmode"];                  // temperature display mode, Celcius=1, Fahrenheit=0
      this->lcdupdateonmove       = doc_per["lcdupdateonmove"];           // update position on lcd when moving
      this->lcdpagetime           = doc_per["lcdpagetime"];
      this->tempcompenabled       = doc_per["tempcompstate"];             // indicates if temperature compensation is enabled
      this->tcdirection           = doc_per["tcdir"];
      this->motorSpeed            = doc_per["motorspeed"];
      this->displayenabled        = doc_per["displaystate"];
      for (int i = 0; i < 10; i++)
      {
        this->preset[i]           = doc_per["preset"][i];
      }
      this->webserverport         = doc_per["wsport"];
      this->ascomalpacaport       = doc_per["ascomport"];
      this->webpagerefreshrate    = doc_per["wprefreshrate"];
      this->mdnsport              = doc_per["mdnsport"];
      this->tcpipport             = doc_per["tcpipport"];
      this->startscreen           = doc_per["startscrn"];
      this->backcolor             = doc_per["bcol"].as<char*>();
      this->textcolor             = doc_per["tcol"].as<char*>();
      this->headercolor           = doc_per["hcol"].as<char*>();
      this->titlecolor            = doc_per["ticol"].as<char*>();
      this->ascomserverstate      = doc_per["ason"];
      this->webserverstate        = doc_per["wson"];
      this->temperatureprobestate = doc_per["tprobe"];
      this->inoutledstate         = doc_per["leds"];
      this->showhpswmessages      = doc_per["hpswmsg"];
      this->forcedownload         = doc_per["fcdownld"];
      this->oledpageoption        = doc_per["oledpg"].as<char*>();
      this->motorspeeddelay       = doc_per["msdelay"];
    }
    file.close();
    DebugPrintln(F("config file persistant data loaded"));
  }
  delay(10);
  file = SPIFFS.open(filename_variable, "r");
  if (!file)
  {
    DebugPrintln(F("file variable data !found, load default values"));
    LoadDefaultVariableData();
    retval = 1;
  }
  else
  {
    String data = file.readString();                // read content of the text file
    DebugPrint(F("FS. Variable SetupData= "));
    DebugPrintln(data);                             // ... and print on serial

    // Allocate a temporary JsonDocument
    DynamicJsonDocument doc_var(DEFAULTVARDOCSIZE);

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc_var, data);
    if (error)
    {
      DebugPrintln(F("Fail reading variable data file, using default configuration"));
      LoadDefaultVariableData();
      retval = 2;
    }
    else
    {
      this->fposition = doc_var["fpos"];            // last focuser position
      this->focuserdirection = doc_var["fdir"];     // keeps track of last focuser move direction

      // round position to fullstep motor position
      this->fposition = (this->fposition + this->stepmode / 2) / this->stepmode * this->stepmode;
      retval = 3;
    }
  }
  file.close();
  DebugPrintln(F("config file variable data loaded"));
  return retval;
}

void SetupData::SetFocuserDefaults(void)
{
  LoadDefaultPersistantData();
  LoadDefaultVariableData();
  if ( SPIFFS.exists(filename_persistant))
  {
    SPIFFS.remove(filename_persistant);
  }
  delay(10);
  if ( SPIFFS.exists(filename_variable))
  {
    SPIFFS.remove(filename_variable);
  }
}

void SetupData::LoadDefaultPersistantData()
{
  this->maxstep               = DEFAULTMAXSTEPS;
  this->coilpower             = DEFAULTOFF;
  this->reversedirection      = DEFAULTOFF;
  this->stepsizeenabled       = DEFAULTOFF;
  this->stepsize              = DEFAULTSTEPSIZE;
  this->DelayAfterMove        = DEFAULTOFF;
  this->backlashsteps_in      = DEFAULTOFF;
  this->backlashsteps_out     = DEFAULTOFF;
  this->backlash_in_enabled   = DEFAULTON;
  this->backlash_out_enabled  = DEFAULTON;
  this->tempcoefficient       = DEFAULTOFF;
  this->tempresolution        = TEMPRESOLUTION;       // 0.25 degrees
  this->stepmode              = STEP1;                // step mode 1=full, 2, 4, 8, 16, 32
  this->tcdirection           = DEFAULTOFF;           // temperature compensation direction 1
  this->tempmode              = DEFAULTCELSIUS;       // default is celsius
  this->tempcompenabled       = DEFAULTOFF;           // temperature compensation disabled
  this->lcdupdateonmove       = DEFAULTON;
  this->lcdpagetime           = LCDPAGETIMEMIN;       // 2, 3 -- 10
  this->motorSpeed            = FAST;
  this->displayenabled        = DEFAULTON;
  for (int i = 0; i < 10; i++)
  {
    this->preset[i]           = 0;
  }
  this->webserverport         = WEBSERVERPORT;        // 80
  this->ascomalpacaport       = ALPACAPORT;           // 4040
  this->webpagerefreshrate    = WS_REFRESHRATE;       // 30s
  this->mdnsport              = MDNSSERVERPORT;       // 7070
  this->tcpipport             = SERVERPORT;           // 2020
  this->startscreen           = DEFAULTON;
  this->backcolor             = "333333";
  this->textcolor             = "5d6d7e";
  this->headercolor           = "3399ff";
  this->titlecolor            = "8e44ad";
  this->ascomserverstate      = DEFAULTOFF;           // this should be default OFF
  this->webserverstate        = DEFAULTON;           // this should be default OFF
  this->temperatureprobestate = DEFAULTON;           // this should be default OFF - if HW not fitted could crash
  this->inoutledstate         = DEFAULTOFF;           // this should be default OFF - if HW not fitted could crash
  this->showhpswmessages      = DEFAULTOFF;           // this should be default OFF
  this->forcedownload         = DEFAULTOFF;           // this should be default OFF, MANAGEMENT Server only
  this->oledpageoption        = OLEDPGOPTIONALL;
  this->motorspeeddelay       = 0;                    // needs to come from driverboard
  this->SavePersitantConfiguration();                 // write default values to SPIFFS
}

void SetupData::LoadDefaultVariableData()
{
  this->fposition = DEFAULTPOSITION;                  // last focuser position
  this->focuserdirection = moving_in;                 // keeps track of last focuser move direction
}

// Saves the configuration to a file
boolean SetupData::SaveConfiguration(unsigned long currentPosition, byte DirOfTravel)
{
  //Serial.println("SaveConfiguration:");
  if (this->fposition != currentPosition || this->focuserdirection != DirOfTravel)  // last focuser position
  {
    this->fposition = currentPosition;
    this->focuserdirection = DirOfTravel;
    this->ReqSaveData_var = true;
    this->SnapShotMillis = millis();
    DebugPrintln(F("++ request for saving variable data"));
    delay(10);
  }

  byte status = false;
  unsigned long x = millis();

  if ((SnapShotMillis + DEFAULTSAVETIME) < x || SnapShotMillis > x)    // 30s after snapshot
  {
    if (this->ReqSaveData_per == true)
    {
      if (SavePersitantConfiguration() == false)
      {
        DebugPrintln(F("Error save persistant configuration"));
      }
      else
      {
        delay(10);
        DebugPrintln(F("++ persistant data saved"));
      }
      status = true;
      this->ReqSaveData_per = false;
    }

    if (this->ReqSaveData_var == true)
    {
      if (SaveVariableConfiguration() == false)
      {
        DebugPrintln(F("Error save variable configuration"));
      }
      else
      {
        delay(10);
        DebugPrintln(F("++ variable data saved"));
      }
      status = true;
      this->ReqSaveData_var = false;
    }
  }
  return status;
}

boolean SetupData::SaveNow()                                // used by reboot to save settings
{
  return SavePersitantConfiguration();
}

byte SetupData::SavePersitantConfiguration()
{
  delay(10);
  if ( SPIFFS.exists(filename_persistant))
  {
    SPIFFS.remove(filename_persistant);
  }
  delay(10);
  File file = SPIFFS.open(filename_persistant, "w");         // Open file for writing
  if (!file)
  {
    TRACE();
    DebugPrintln(CREATEFILEFAILSTR);
    return false;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<DEFAULTDOCSIZE> doc;

  // Set the values in the document
  doc["maxstep"]            = this->maxstep;                    // max steps
  doc["stepsize"]           = this->stepsize;                   // the step size in microns, ie 7.2 - value * 10, so real stepsize = stepsize / 10 (maxval = 25.6)
  doc["delayaftermove"]     = this->DelayAfterMove;             // delay after movement is finished (maxval=256)
  doc["backlashsteps_in"]   = this->backlashsteps_in;           // number of backlash steps to apply for IN moves
  doc["backlashsteps_out"]  = this->backlashsteps_out;          // number of backlash steps to apply for OUT moves
  doc["backlash_in_enabled"] = this->backlash_in_enabled;
  doc["backlash_out_enabled"] = this->backlash_out_enabled;
  doc["tempcoefficient"]    = this->tempcoefficient;            // steps per degree temperature coefficient value (maxval=256)
  doc["tempresolution"]     = this->tempresolution;
  doc["stepmode"]           = this->stepmode;
  doc["coilpwr"]            = this->coilpower;
  doc["rdirection"]         = this->reversedirection;
  doc["stepsizestate"]      = this->stepsizeenabled;            // if 1, controller returns step size
  doc["tempmode"]           = this->tempmode;                   // temperature display mode, Celcius=1, Fahrenheit=0
  doc["lcdupdateonmove"]    = this->lcdupdateonmove;            // update position on lcd when moving
  doc["lcdpagetime"]        = this->lcdpagetime;                // *100 to give interval between lcd pages display time
  doc["tempcompstate"]      = this->tempcompenabled;            // indicates if temperature compensation is enabled
  doc["tcdir"]              = this->tcdirection;
  doc["motorspeed"]         = this->motorSpeed;
  doc["displaystate"]       = this->displayenabled;
  for (int i = 0; i < 10; i++)
  {
    doc["preset"][i]        = this->preset[i];                  // Json array for presets
  };
  doc["wsport"]             = this->webserverport;
  doc["ascomport"]          = this->ascomalpacaport;
  doc["mdnsport"]           = this->mdnsport;
  doc["wprefreshrate"]      = this->webpagerefreshrate;
  doc["tcpipport"]          = this->tcpipport;
  doc["startscrn"]          = this->startscreen;
  doc["bcol"]               = this->backcolor;
  doc["tcol"]               = this->textcolor;
  doc["hcol"]               = this->headercolor;
  doc["ticol"]              = this->titlecolor;
  doc["ason"]               = this->ascomserverstate;
  doc["wson"]               = this->webserverstate;
  doc["tprobe"]             = this->temperatureprobestate;
  doc["leds"]               = this->inoutledstate;
  doc["hpswmsg"]            = this->showhpswmessages;
  doc["fcdownld"]           = this->forcedownload;
  doc["oledpg"]             = this->oledpageoption;
  doc["msdelay"]            = this->motorspeeddelay;
    
  // Serialize JSON to file
  DebugPrintln("Writing to file");
  if (serializeJson(doc, file) == 0)
  {
    TRACE();
    DebugPrintln(WRITEFILEFAILSTR);
    file.close();                                     // Close the file
    return false;
  }
  else
  {
    DebugPrintln(WRITEFILESUCCESSSTR);
    file.close();                                     // Close the file
    return true;
  }
}

byte SetupData::SaveVariableConfiguration()
{
  // Delete existing file
  if ( SPIFFS.exists(filename_variable))
  {
    SPIFFS.remove(filename_variable);
  }
  delay(10);
  SPIFFS.remove(filename_variable);

  delay(10);
  // Open file for writing
  File file = SPIFFS.open(this->filename_variable, "w");
  if (!file)
  {
    TRACE();
    DebugPrintln(CREATEFILEFAILSTR);
    return false;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<DEFAULTVARDOCSIZE> doc;

  // Set the values in the document
  doc["fpos"] =  this->fposition;                  // last focuser position
  doc["fdir"] =  this->focuserdirection;           // keeps track of last focuser move direction

  if (serializeJson(doc, file) == 0)                // Serialize JSON to file
  {
    TRACE();
    DebugPrintln(WRITEFILEFAILSTR);
    file.close();                                   // Close the file
    return false;
  }
  else
  {
    DebugPrintln(WRITEFILESUCCESSSTR);
    file.close();     // Close the file
    return true;
  }
}

//__getter
unsigned long SetupData::get_fposition()
{
  return this->fposition;             // last focuser position
}

byte SetupData::get_focuserdirection()
{
  return this->focuserdirection;      // keeps track of last focuser move direction
}

unsigned long SetupData::get_maxstep()
{
  return this->maxstep;               // max steps
}

float SetupData::get_stepsize()
{
  return this->stepsize;              // the step size in microns
  // this is the actual measured focuser stepsize in microns amd is reported to ASCOM, so must be valid
  // the amount in microns that the focuser tube moves in one step of the motor
}

byte SetupData::get_DelayAfterMove()
{
  return this->DelayAfterMove;        // delay after movement is finished (maxval=256)
}

byte SetupData::get_backlashsteps_in()
{
  return this->backlashsteps_in;      // number of backlash steps to apply for IN moves
}

byte SetupData::get_backlashsteps_out()
{
  return this->backlashsteps_out;     // number of backlash steps to apply for OUT moves
}

byte SetupData::get_backlash_in_enabled()
{
  return this->backlash_in_enabled;   // apply backlash when moving in [0=!enabled, 1=enabled]
}

byte SetupData::get_backlash_out_enabled()
{
  return this->backlash_out_enabled;  // apply backlash when moving out [0=!enabled, 1=enabled]
}

byte SetupData::get_tempcoefficient()
{
  return this->tempcoefficient;       // steps per degree temperature coefficient value (maxval=256)
}

byte SetupData::get_tempresolution()
{
  return this->tempresolution;        // resolution of temperature measurement 9-12
}

int  SetupData::get_stepmode()
{
  return this->stepmode;              // current step mode
}

byte SetupData::get_coilpower()
{
  return this->coilpower;             // state of coil power, 0 = !enabled, 1= enabled
}

byte SetupData::get_reversedirection()
{
  return this->reversedirection;      // state for reverse direction, 0 = !enabled, 1= enabled
}

byte SetupData::get_stepsizeenabled()
{
  return this->stepsizeenabled;       // if 1, controller returns step size
}

byte SetupData::get_tempmode()
{
  return this->tempmode;              // temperature display mode, Celcius=1, Fahrenheit=0
}

byte SetupData::get_lcdupdateonmove()
{
  return this->lcdupdateonmove;       // update position on lcd when moving
}

byte SetupData::get_lcdpagetime()
{
  return this->lcdpagetime;           // the length of time the page is displayed for
}

byte SetupData::get_tempcompenabled()
{
  return this->tempcompenabled;       // indicates if temperature compensation is enabled
}

byte SetupData::get_tcdirection()
{
  return this->tcdirection;           // indicates the direction in which temperature compensation is applied
}

byte SetupData::get_motorSpeed()
{
  return this->motorSpeed;            // the stepper motor speed, slow, medium, fast
}

byte SetupData::get_displayenabled()
{
  return this->displayenabled;        // the state of the oled display, enabled or !enabled
}

unsigned long SetupData::get_focuserpreset(byte idx)
{
  return this->preset[idx % 10];      // the focuser position for each preset
}

unsigned long SetupData::get_webserverport(void)
{
  return this->webserverport;         // the port number of the webserver
}

unsigned long SetupData::get_ascomalpacaport(void)
{
  return this->ascomalpacaport;       // the port number used by the ALPACA ASCOM Remote server
}

int SetupData::get_webpagerefreshrate(void)
{
  return this->webpagerefreshrate;    // the webpage refresh rate
}

unsigned long SetupData::get_mdnsport(void)
{
  return this->mdnsport;              // the mdns port number
}

unsigned long SetupData::get_tcpipport(void)
{
  return this->tcpipport;             // the tcp/ip port used by the tcp/server
}

byte SetupData::get_showstartscreen(void)
{
  return this->startscreen;           //  the state of startscreen, enabled or !enabled
  // if enabled, show startup messages on the TEXT OLED display
}

String SetupData::get_wp_backcolor()
{
  return this->backcolor;
}

String SetupData::get_wp_textcolor()
{
  return this->textcolor;
}

String SetupData::get_wp_headercolor()
{
  return this->headercolor;
}

String SetupData::get_wp_titlecolor()
{
  return this->titlecolor;
}

byte SetupData::get_ascomserverstate()
{
  return this->ascomserverstate;
}

byte SetupData::get_webserverstate()
{
  return this->webserverstate;
}

byte SetupData::get_temperatureprobestate()
{
  return this->temperatureprobestate;
}

byte SetupData::get_inoutledstate()
{
  return this->inoutledstate;
}

byte SetupData::get_showhpswmsg()
{
  return this->showhpswmessages;
}

byte SetupData::get_forcedownload()
{
  return this->forcedownload;
}

String SetupData::get_oledpageoption()
{
  return this->oledpageoption;
}

int SetupData::get_motorspeeddelay()
{
  return this->motorspeeddelay;
}

//__Setter

void SetupData::set_fposition(unsigned long fposition)
{
  this->fposition = fposition; // last focuser position
}

void SetupData::set_focuserdirection(byte focuserdirection)
{
  this->focuserdirection = focuserdirection; // keeps track of last focuser move direction
}

void SetupData::set_maxstep(unsigned long maxstep)
{
  this->StartDelayedUpdate(this->maxstep, maxstep); // max steps
}

void SetupData::set_stepsize(float stepsize)
{
  this->StartDelayedUpdate(this->stepsize, stepsize); // the step size in microns, ie 7.2 - value * 10, so real stepsize = stepsize / 10 (maxval = 25.6)
}

void SetupData::set_DelayAfterMove(byte DelayAfterMove)
{
  this->StartDelayedUpdate(this->DelayAfterMove, DelayAfterMove); // delay after movement is finished (maxval=256)
}

void SetupData::set_backlashsteps_in(byte backlashsteps)
{
  this->StartDelayedUpdate(this->backlashsteps_in, backlashsteps); // number of backlash steps to apply for IN moves
}

void SetupData::set_backlashsteps_out(byte backlashsteps_out)
{
  this->StartDelayedUpdate(this->backlashsteps_out, backlashsteps_out); // number of backlash steps to apply for OUT moves
}

void SetupData::set_backlash_in_enabled(byte backlash_in_enabled)
{
  this->StartDelayedUpdate(this->backlash_in_enabled, backlash_in_enabled);
}

void SetupData::set_backlash_out_enabled(byte backlash_out_enabled)
{
  this->StartDelayedUpdate(this->backlash_out_enabled, backlash_out_enabled);
}

void SetupData::set_tempcoefficient(byte tempcoefficient)
{
  this->StartDelayedUpdate(this->tempcoefficient, tempcoefficient); // steps per degree temperature coefficient value (maxval=256)
}

void SetupData::set_tempresolution(byte tempresolution)
{
  this->StartDelayedUpdate(this->tempresolution, tempresolution);
}

void SetupData::set_stepmode(int stepmode)
{
  this->StartDelayedUpdate(this->stepmode, stepmode);
}

void SetupData::set_coilpower(byte coilpower)
{
  this->StartDelayedUpdate(this->coilpower, coilpower);
}

void SetupData::set_reversedirection(byte reversedirection)
{
  this->StartDelayedUpdate(this->reversedirection, reversedirection);
}

void SetupData::set_stepsizeenabled(byte stepsizeenabled)
{
  this->StartDelayedUpdate(this->stepsizeenabled, stepsizeenabled); // if 1, controller returns step size
}

void SetupData::set_tempmode(byte tempmode)
{
  this->StartDelayedUpdate(this->tempmode, tempmode); // temperature display mode, Celcius=1, Fahrenheit=0
}

void SetupData::set_lcdupdateonmove(byte lcdupdateonmove)
{
  this->StartDelayedUpdate(this->lcdupdateonmove, lcdupdateonmove); // update position on lcd when moving
}

void SetupData::set_lcdpagetime(byte lcdpagetime)
{
  this->StartDelayedUpdate(this->lcdpagetime, lcdpagetime);
}

void SetupData::set_tempcompenabled(byte tempcompenabled)
{
  this->StartDelayedUpdate(this->tempcompenabled, tempcompenabled); // indicates if temperature compensation is enabled
}

void SetupData::set_tcdirection(byte tcdirection)
{
  this->StartDelayedUpdate(this->tcdirection, tcdirection);
}

void SetupData::set_motorSpeed(byte motorSpeed)
{
  this->StartDelayedUpdate(this->motorSpeed, motorSpeed);
}

void SetupData::set_displayenabled(byte displaystate)
{
  this->StartDelayedUpdate(this->displayenabled, displaystate);
}

void SetupData::set_focuserpreset(byte idx, unsigned long pos)
{
  this->StartDelayedUpdate(this->preset[idx % 10], pos);
}

void SetupData::set_webserverport(unsigned long wsp)
{
  this->StartDelayedUpdate(this->webserverport, wsp);
}

void SetupData::set_ascomalpacaport(unsigned long asp)
{
  this->StartDelayedUpdate(this->ascomalpacaport, asp);
}

void SetupData::set_webpagerefreshrate(int rr)
{
  this->StartDelayedUpdate(this->webpagerefreshrate, rr);
}

void SetupData::set_mdnsport(unsigned long port)
{
  this->StartDelayedUpdate(this->mdnsport, port);
}

void SetupData::set_tcpipport(unsigned long port)
{
  this->StartDelayedUpdate(this->tcpipport, port);
}

void SetupData::set_showstartscreen(byte newval)
{
  this->StartDelayedUpdate(this->startscreen, newval);
}

void SetupData::set_wp_backcolor(String newstr)
{
  this->StartDelayedUpdate(this->backcolor, newstr);
}

void SetupData::set_wp_textcolor(String newstr)
{
  this->StartDelayedUpdate(this->textcolor, newstr);
}

void SetupData::set_wp_headercolor(String newstr)
{
  this->StartDelayedUpdate(this->headercolor, newstr);
}

void SetupData::set_wp_titlecolor(String newstr)
{
  this->StartDelayedUpdate(this->titlecolor, newstr);
}

void SetupData::set_ascomserverstate(byte newval)
{
  this->StartDelayedUpdate(this->ascomserverstate, newval);
}

void SetupData::set_webserverstate(byte newval)
{
  this->StartDelayedUpdate(this->webserverstate, newval);
}

void SetupData::set_temperatureprobestate(byte newval)
{
  this->StartDelayedUpdate(this->temperatureprobestate, newval);
}

void SetupData::set_inoutledstate(byte newval)
{
  this->StartDelayedUpdate(this->inoutledstate, newval);
}

void SetupData::set_showhpswmsg(byte newval)
{
  this->StartDelayedUpdate(this->showhpswmessages, newval);
}

void SetupData::set_forcedownload(byte newval)
{
  this->StartDelayedUpdate(this->forcedownload, newval);
}

void SetupData::set_oledpageoption(String newval)
{
  this->StartDelayedUpdate(this->oledpageoption, newval);
}

void SetupData::set_motorspeeddelay(int newval)
{
  this->StartDelayedUpdate(this->motorspeeddelay, newval);
}

void SetupData::StartDelayedUpdate(int & org_data, int new_data)
{
  if (org_data != new_data)
  {
    this->ReqSaveData_per = true;
    this->SnapShotMillis = millis();
    org_data = new_data;
    DebugPrintln(F("++ request for saving persitant data"));
  }
}

void SetupData::StartDelayedUpdate(unsigned long & org_data, unsigned long new_data)
{
  if (org_data != new_data)
  {
    this->ReqSaveData_per = true;
    this->SnapShotMillis = millis();
    org_data = new_data;
    DebugPrintln(F("++ request for saving persitant data"));
  }
}

void SetupData::StartDelayedUpdate(float & org_data, float new_data)
{
  if (org_data != new_data)
  {
    this->ReqSaveData_per = true;
    this->SnapShotMillis = millis();
    org_data = new_data;
    DebugPrintln(F("++ request for saving persitant data"));
  }
}

void SetupData::StartDelayedUpdate(byte & org_data, byte new_data)
{
  if (org_data != new_data)
  {
    this->ReqSaveData_per = true;
    this->SnapShotMillis = millis();
    org_data = new_data;
    DebugPrintln(F("++ request for saving persitant data"));
  }
}

void SetupData::StartDelayedUpdate(String & org_data, String new_data)
{
  if (org_data != new_data)
  {
    this->ReqSaveData_per = true;
    this->SnapShotMillis = millis();
    org_data = new_data;
    DebugPrintln("Save request for data_per.jsn");
  }
}


void SetupData::ListDir(const char * dirname, uint8_t levels)
{
  // TODO
  // THIS DOES NOT WORK ON ESP8266!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  DebugPrint(F("Listing directory: {"));
#if defined(ESP8266)
  Serial.println("SetupData::ListDir() does not work on ESP8266");
  // this does not work;
#else
  File root = SPIFFS.open(dirname);
  delay(10);
  if (!root)
  {
    DebugPrintln(F(" - failed to open directory"));
  }
  else
  {
    if (!root.isDirectory())
    {
      DebugPrintln(F(" - not a directory"));
    }
    else
    {
      File file = root.openNextFile();
      delay(10);
      int i = 0;
      while (file)
      {
        if (file.isDirectory())
        {
          DebugPrint(F("  DIR : "));
          DebugPrintln(file.name());
          if (levels)
          {
            this->ListDir(file.name(), levels - 1);
          }
        }
        else
        {
          DebugPrint(file.name());
          DebugPrint(F(":"));
          DebugPrint(file.size());
          if ((++i % 6) == 0)
          {
            DebugPrintln("");
          }
          else
          {
            DebugPrint(F("  "));
          }
        }
        delay(10);
        file = root.openNextFile();
      }
      DebugPrintln("}");
    }
  }
#endif
}
