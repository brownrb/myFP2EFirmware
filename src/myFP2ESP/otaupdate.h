// ----------------------------------------------------------------------------------------------
// myFP2ESP OTAUPDATE ROUTINES AND DEFINITIONS
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2019. All Rights Reserved.
// (c) Copyright Holger M, 2019. All Rights Reserved.
// ----------------------------------------------------------------------------------------------

#ifndef otaupdate_h
#define otaupdate_h

// ----------------------------------------------------------------------------------------------
// INCLUDES
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// EXTERNS
// ----------------------------------------------------------------------------------------------
void oledtextmsg(String, int, boolean, boolean);

// ----------------------------------------------------------------------------------------------
// CODE
// ----------------------------------------------------------------------------------------------
#ifdef OTAUPDATES
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
}
#endif // ifdef OTAUPDATES



#endif // ifndef otaupdate_h
