// ----------------------------------------------------------------------------------------------
// myFP2ESP FOCUSER DATA ROUTINES
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2020. All Rights Reserved.
// (c) Copyright Holger M, 2019-2020. All Rights Reserved.
// ----------------------------------------------------------------------------------------------

#include <ArduinoJson.h>

#if defined(ESP8266)
#include <FS.h>
#else
#include "SPIFFS.h"
#endif

#include "FocuserSetupData.h"
#include "generalDefinitions.h"

SetupData::SetupData(void)
{
  DebugPrintln("Constructor Setupdata");

  this->SnapShotMillis = millis();
  this->ReqSaveData_var  = false;
  this->ReqSaveData_per = false;

  // mount SPIFFS
  if (!SPIFFS.begin())
  {
    DebugPrintln(F("Error occurred when mounting SPIFFS"));
    Serial.println(F("Format SPIFFS, please wait..."));
    SPIFFS.format();
    Serial.println(F("Format SPIFFS done"));
  }
  else
  {
    DebugPrintln("SPIFFS mounted");
  }

  this->LoadConfiguration();
};

// Loads the configuration from a file
byte SetupData::LoadConfiguration()
{
  byte retval = 0;

  // Open file for reading
  File file = SPIFFS.open(filename_persistant, "r");

  if (!file)
  {
    DebugPrintln(F("no config file persistant data found, load default values"));
    LoadDefaultPersistantData();
  }
  else
  {
    String data = file.readString(); // read content of the text file
    DebugPrint(F("SPIFFS. Persistant SetupData= "));
    DebugPrintln(data);           // ... and print on serial

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
      this->tempprecision         = doc_per["tempprecision"];             // 9 -12
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
      this->preset0               = doc_per["preset0"];
      this->preset1               = doc_per["preset1"];
      this->preset2               = doc_per["preset2"];
      this->preset3               = doc_per["preset3"];
      this->preset4               = doc_per["preset4"];
      this->preset5               = doc_per["preset5"];
      this->preset6               = doc_per["preset6"];
      this->preset7               = doc_per["preset7"];
      this->preset8               = doc_per["preset8"];
      this->preset9               = doc_per["preset9"];
      this->webserverport         = doc_per["wsport"];
      this->ascomalpacaport       = doc_per["ascomport"];
      this->webpagerefreshrate    = doc_per["wprefreshrate"];
      this->mdnsport              = doc_per["mdnsport"];
      this->tcpipport             = doc_per["tcpipport"];
    }
    file.close();
    DebugPrintln(F("config file persistant data loaded"));
  }

  file = SPIFFS.open(filename_variable, "r");
  if (!file)
  {
    DebugPrintln(F("no config file variable data found, load default values"));
    LoadDefaultVariableData();
    retval = 1;
  }
  else
  {
    String data = file.readString();  // read content of the text file
    DebugPrint(F("SPIFFS. Variable SetupData= "));
    DebugPrintln(data);               // ... and print on serial

    // Allocate a temporary JsonDocument
    DynamicJsonDocument doc_var(DEFAULTVARDOCSIZE);

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc_var, data);
    if (error)
    {
      DebugPrintln(F("Failed to read variable data file, using default configuration"));
      LoadDefaultVariableData();
      retval = 2;
    }
    else
    {
      this->fposition = doc_var["fpos"];         // last focuser position
      this->focuserdirection = doc_var["fdir"];  // keeps track of last focuser move direction

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

  SPIFFS.remove(filename_persistant);
  SPIFFS.remove(filename_variable);
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
  this->tempprecision         = TEMPPRECISION;        // 0.25 degrees
  this->stepmode              = STEP1;                // step mode 1=full, 2, 4, 8, 16, 32
  this->tcdirection           = DEFAULTOFF;           // temperature compensation direction 1
  this->tempmode              = DEFAULTCELSIUS;       // default is celsius
  this->tempcompenabled       = DEFAULTOFF;           // temperature compensation disabled
  this->lcdupdateonmove       = DEFAULTON;
  this->lcdpagetime           = LCDPAGETIMEMIN;       // 2, 3 -- 10
  this->motorSpeed            = FAST;
  this->displayenabled        = DEFAULTON;
  this->preset0               = 0;
  this->preset1               = 0;
  this->preset2               = 0;
  this->preset3               = 0;
  this->preset4               = 0;
  this->preset5               = 0;
  this->preset6               = 0;
  this->preset7               = 0;
  this->preset8               = 0;
  this->preset9               = 0;
  this->webserverport         = WEBSERVERPORT;    // 80
  this->ascomalpacaport       = ALPACAPORT;       // 4040
  this->webpagerefreshrate    = WS_REFRESHRATE;   // 30s
  this->mdnsport              = MDNSSERVERPORT;   // 7070
  this->tcpipport             = SERVERPORT;       // 2020
  
  this->SavePersitantConfiguration();             // write default values to SPIFFS
}

void SetupData::LoadDefaultVariableData()
{
  this->fposition = DEFAULTPOSITION;                  // last focuser position
  this->focuserdirection = moving_in;                 // keeps track of last focuser move direction
}

// Saves the configuration to a file
boolean SetupData::SaveConfiguration(unsigned long currentPosition, byte DirOfTravel)
{
  //DebugPrintln("SaveConfiguration:");
  if (this->fposition != currentPosition || this->focuserdirection != DirOfTravel)  // last focuser position
  {
    this->fposition = currentPosition;
    this->focuserdirection = DirOfTravel;
    this->ReqSaveData_var = true;
    this->SnapShotMillis = millis();
    DebugPrintln(F("++++++++++++++++++++++++++++++++++++ request for saving variable data"));
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
        DebugPrintln(F("++++++++++++++++++++++++++++++++++++ persistant data saved"));
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
        DebugPrintln(F("++++++++++++++++++++++++++++++++++++ variable data saved"));
      }
      status = true;
      this->ReqSaveData_var = false;
    }
  }
  return status;
}

byte SetupData::SavePersitantConfiguration()
{
  SPIFFS.remove(filename_persistant);                // Delete existing file

  File file = SPIFFS.open(filename_persistant, "w"); // Open file for writing
  if (!file)
  {
    TRACE();
    DebugPrintln(F(CREATEFILEFAILSTR));
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
  doc["tempprecision"]      = this->tempprecision;
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
  doc["preset0"]            = this->preset0;
  doc["preset1"]            = this->preset1;
  doc["preset2"]            = this->preset2;
  doc["preset3"]            = this->preset3;
  doc["preset4"]            = this->preset4;
  doc["preset5"]            = this->preset5;
  doc["preset6"]            = this->preset6;
  doc["preset7"]            = this->preset7;
  doc["preset8"]            = this->preset8;
  doc["preset9"]            = this->preset9;
  doc["wsport"]             = this->webserverport;
  doc["ascomport"]          = this->ascomalpacaport;
  doc["mdnsport"]           = this->mdnsport;
  doc["wprefreshrate"]      = this->webpagerefreshrate;
  doc["tcpipport"]          = this->tcpipport;

  // Serialize JSON to file
  if (serializeJson(doc, file) == 0)
  {
    TRACE();
    DebugPrintln(F(WRITEFILEFAILSTR));
    file.close();     // Close the file
    return false;
  }
  else
  {
    DebugPrintln(F(WRITEFILESUCCESSSTR));
    file.close();     // Close the file
    return true;
  }
}

byte SetupData::SaveVariableConfiguration()
{
  // Delete existing file
  SPIFFS.remove(filename_variable);

  // Open file for writing
  File file = SPIFFS.open(this->filename_variable, "w");
  if (!file)
  {
    TRACE();
    DebugPrintln(F(CREATEFILEFAILSTR));
    return false;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<DEFAULTVARDOCSIZE> doc;

  // Set the values in the document
  doc["fpos"] =  this->fposition;                  // last focuser position
  doc["fdir"] =  this->focuserdirection;           // keeps track of last focuser move direction

  // Serialize JSON to file
  if (serializeJson(doc, file) == 0)
  {
    TRACE();
    DebugPrintln(F(WRITEFILEFAILSTR));
    file.close();     // Close the file
    return false;
  }
  else
  {
    DebugPrintln(F(WRITEFILESUCCESSSTR));
    file.close();     // Close the file
    return true;
  }
}


//__getter
unsigned long SetupData::get_fposition()
{
  return this->fposition; // last focuser position
}

byte SetupData::get_focuserdirection()
{
  return this->focuserdirection; // keeps track of last focuser move direction
}

unsigned long SetupData::get_maxstep()
{
  return this->maxstep; // max steps
}

float SetupData::get_stepsize()
{
  return this->stepsize; // the step size in microns, ie 7.2 - value * 10, so real stepsize = stepsize / 10 (maxval = 25.6)
}

byte SetupData::get_DelayAfterMove()
{
  return this->DelayAfterMove; // delay after movement is finished (maxval=256)
}

byte SetupData::get_backlashsteps_in()
{
  return this->backlashsteps_in; // number of backlash steps to apply for IN moves
}

byte SetupData::get_backlashsteps_out()
{
  return this->backlashsteps_out; // number of backlash steps to apply for OUT moves
}

byte SetupData::get_backlash_in_enabled()
{
  return this->backlash_in_enabled;
}

byte SetupData::get_backlash_out_enabled()
{
  return this->backlash_out_enabled;
}

byte SetupData::get_tempcoefficient()
{
  return this->tempcoefficient; // steps per degree temperature coefficient value (maxval=256)
}

byte SetupData::get_tempprecision()
{
  return this->tempprecision;
}

byte SetupData::get_stepmode()
{
  return this->stepmode;
}

byte SetupData::get_coilpower()
{
  return this->coilpower;
}

byte SetupData::get_reversedirection()
{
  return this->reversedirection;
}

byte SetupData::get_stepsizeenabled()
{
  return this->stepsizeenabled; // if 1, controller returns step size
}

byte SetupData::get_tempmode()
{
  return this->tempmode; // temperature display mode, Celcius=1, Fahrenheit=0
}

byte SetupData::get_lcdupdateonmove()
{
  return this->lcdupdateonmove; // update position on lcd when moving
}

byte SetupData::get_lcdpagetime()
{
  return this->lcdpagetime;
}

byte SetupData::get_tempcompenabled()
{
  return this->tempcompenabled; // indicates if temperature compensation is enabled
}

byte SetupData::get_tcdirection()
{
  return this->tcdirection;
}

byte SetupData::get_motorSpeed()
{
  return this->motorSpeed;
}

byte SetupData::get_displayenabled()
{
  return this->displayenabled;
}

unsigned long SetupData::get_focuserpreset(byte idx)
{
  idx = idx & 0x0f;     // get least significant nibble
  idx = ( idx > 9 ) ? 9 : idx;
  switch ( idx )
  {
    case 0: return this->preset0;
      break;
    case 1: return this->preset1;
      break;
    case 2: return this->preset2;
      break;
    case 3: return this->preset3;
      break;
    case 4: return this->preset4;
      break;
    case 5: return this->preset5;
      break;
    case 6: return this->preset6;
      break;
    case 7: return this->preset7;
      break;
    case 8: return this->preset8;
      break;
    case 9: return this->preset9;
      break;
    default:
      return this->preset0;
  }
}

unsigned long SetupData::get_webserverport(void)
{
  return this->webserverport;
}

unsigned long SetupData::get_ascomalpacaport(void)
{
  return this->ascomalpacaport;
}

int SetupData::get_webpagerefreshrate(void)
{
  return this->webpagerefreshrate;
}

unsigned long SetupData::get_mdnsport(void)
{
  return this->mdnsport;
}

unsigned long SetupData::get_tcpipport(void)
{
  return this->tcpipport;
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

void SetupData::set_tempprecision(byte tempprecision)
{
  this->StartDelayedUpdate(this->tempprecision, tempprecision);
}

void SetupData::set_stepmode(byte stepmode)
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
  idx = idx & 0x0f;     // get least significant nibble
  idx = ( idx > 9 ) ? 9 : idx;
  switch ( idx )
  {
    case 0: this->StartDelayedUpdate(this->preset0, pos);
      break;
    case 1: this->StartDelayedUpdate(this->preset1, pos);
      break;
    case 2: this->StartDelayedUpdate(this->preset2, pos);
      break;
    case 3: this->StartDelayedUpdate(this->preset3, pos);
      break;
    case 4: this->StartDelayedUpdate(this->preset4, pos);
      break;
    case 5: this->StartDelayedUpdate(this->preset5, pos);
      break;
    case 6: this->StartDelayedUpdate(this->preset6, pos);
      break;
    case 7: this->StartDelayedUpdate(this->preset7, pos);
      break;
    case 8: this->StartDelayedUpdate(this->preset8, pos);
      break;
    case 9: this->StartDelayedUpdate(this->preset9, pos);
      break;
    default: this->StartDelayedUpdate(this->preset0, pos);
      break;
  }
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

void SetupData::StartDelayedUpdate(int & org_data, int new_data)
{
  if (org_data != new_data)
  {
    this->ReqSaveData_per = true;
    this->SnapShotMillis = millis();
    org_data = new_data;
    DebugPrintln(F("++++++++++++++++++++++++++++++++++++ request for saving persitant data"));
  }
}

void SetupData::StartDelayedUpdate(unsigned long & org_data, unsigned long new_data)
{
  if (org_data != new_data)
  {
    this->ReqSaveData_per = true;
    this->SnapShotMillis = millis();
    org_data = new_data;
    DebugPrintln(F("++++++++++++++++++++++++++++++++++++ request for saving persitant data"));
  }
}

void SetupData::StartDelayedUpdate(float & org_data, float new_data)
{
  if (org_data != new_data)
  {
    this->ReqSaveData_per = true;
    this->SnapShotMillis = millis();
    org_data = new_data;
    DebugPrintln(F("++++++++++++++++++++++++++++++++++++ request for saving persitant data"));
  }
}

void SetupData::StartDelayedUpdate(byte & org_data, byte new_data)
{
  if (org_data != new_data)
  {
    this->ReqSaveData_per = true;
    this->SnapShotMillis = millis();
    org_data = new_data;
    DebugPrintln(F("++++++++++++++++++++++++++++++++++++ request for saving persitant data"));
  }
}
