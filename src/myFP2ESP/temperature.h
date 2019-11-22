// ----------------------------------------------------------------------------------------------
// TITLE: myFP2ESP FIRMWARE TEMPERATURE
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2019. All Rights Reserved.
// (c) Copyright Holger M, 2019. All Rights Reserved.
// ----------------------------------------------------------------------------------------------

#ifndef temperature_h
#define temperature_h

// ----------------------------------------------------------------------------------------------
// EXTERNS
// ----------------------------------------------------------------------------------------------
extern SetupData *mySetupData;
extern byte tprobe1; 
extern unsigned long ftargetPosition;          // target position
extern void oledtextmsg(String, int, boolean, boolean);
extern byte TimeCheck(unsigned long, unsigned long);

// ----------------------------------------------------------------------------------------------
// DATA
// ----------------------------------------------------------------------------------------------

#ifdef TEMPERATUREPROBE
OneWire oneWirech1(TEMPPIN);            // setup temperature probe
DallasTemperature sensor1(&oneWirech1);
DeviceAddress tpAddress;                // holds address of the temperature probe
#endif // temperatureprobe

// ----------------------------------------------------------------------------------------------
// CODE
// ----------------------------------------------------------------------------------------------
#if defined(TEMPERATUREPROBE)
void inittemp(void)
{
  pinMode(TEMPPIN, INPUT);                              // Configure GPIO pin for temperature probe
  DebugPrintln(checkfortprobestr);
  oledtextmsg(checkfortprobestr, -1, true, true);
  sensor1.begin();                                      // start the temperature sensor1
  DebugPrintln(F("Get # of Tsensors"));
  tprobe1 = sensor1.getDeviceCount();                   // should return 1 if probe connected
  DebugPrint(F("TSensors= "));
  DebugPrintln(tprobe1);
  if (sensor1.getAddress(tpAddress, 0) == true)
  {
    sensor1.setResolution(tpAddress, mySetupData->get_tempprecision());    // set probe resolution
    DebugPrint(F("Tsensors= "));
    DebugPrintln(tprobe1);
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
    oledtextmsg(tprobenotfoundstr, -1, false, true);
  }
}

float readtemp(byte new_measurement)
{
  static float lasttemp = 20.0;                         // start temp value
  if (!new_measurement)
  {
    return lasttemp;                                    // return latest measurement
  }

  float result = sensor1.getTempCByIndex(0);            // get channel 1 temperature, always in celsius
  DebugPrint(F("Temperature = "));
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

void Update_Temp(void)
{
  static byte tcchanged = mySetupData->get_tempcompenabled();  // keeps track if tempcompenabled changes

  if (tprobe1 == 1)
  {
    static unsigned long lasttempconversion = 0;
    static byte requesttempflag = 0;                    // start with request
    unsigned long tempnow = millis();

    // see if the temperature needs updating - done automatically every 1.5s
    if (TimeCheck(lasttempconversion , TEMPREFRESHRATE))
    {
      static float tempval;
      static float starttemp;                           // start temperature to use when temperature compensation is enabled

      if ( tcchanged != mySetupData->get_tempcompenabled() )
      {
        tcchanged = mySetupData->get_tempcompenabled();
        if ( tcchanged == 1 )
        {
          starttemp = readtemp(1);
        }
      }

      lasttempconversion = tempnow;                     // update time stamp

      if (requesttempflag)
      {
        tempval = readtemp(1);
      }
      else
      {
        sensor1.requestTemperatures();
      }

      requesttempflag ^= 1; // toggle flag

      if (mySetupData->get_tempcompenabled() == 1)      // check for temperature compensation
      {
        if ((abs)(starttemp - tempval) >= 1)            // calculate if temp has moved by more than 1 degree
        {
          unsigned long newPos;
          byte temperaturedirection;                    // did temperature fall (1) or rise (0)?
          temperaturedirection = (tempval < starttemp) ? 1 : 0;
          if (mySetupData->get_tcdirection() == 0)      // check if tc direction for compensation is inwards
          {
            // temperature compensation direction is in, if a fall then move in else move out
            if ( temperaturedirection == 1 )            // check if temperature is falling
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
          starttemp = tempval;                          // save this current temp point for future reference
        } // end of check for tempchange >=1
      } // end of check for tempcomp enabled
    } // end of check for temperature needs updating
  } // end of if tprobe
}
#endif // TEMPERATUREPROBE 


#endif
