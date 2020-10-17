// ----------------------------------------------------------------------------------------------
// temp.cpp : myFP2ESP temperature probe
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2020. All Rights Reserved.
// (c) Copyright Holger Manz, 2020. All Rights Reserved.
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// EXTERNALS
// ----------------------------------------------------------------------------------------------

#include <Arduino.h>
#include "FocuserSetupData.h"
#include "myBoards.h"
#include "generalDefinitions.h"
#include <OneWire.h>                          // https://github.com/PaulStoffregen/OneWire
#include <myDallasTemperature.h>

OneWire oneWirech1(TEMPPIN);                  // setup temperature probe
DallasTemperature sensor1(&oneWirech1);
DeviceAddress tpAddress;                      // holds address of the temperature probe

extern SetupData *mySetupData;
extern bool TimeCheck(unsigned long, unsigned long);
extern unsigned long ftargetPosition;       // target position

byte    tprobe1;                            // indicate if there is a probe attached to myFocuserPro2


// ----------------------------------------------------------------------------------------------
// 17: TEMPERATURE - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------
void init_temp(void)
{
  // start temp probe
  pinMode(TEMPPIN, INPUT);                // Configure GPIO pin for temperature probe
  DebugPrintln(CHECKFORTPROBESTR);
  sensor1.begin();                        // start the temperature sensor1
  DebugPrintln(GETTEMPPROBESSTR);
  tprobe1 = sensor1.getDeviceCount();     // should return 1 if probe connected
  DebugPrint(TPROBESTR);
  DebugPrintln(tprobe1);
  if (sensor1.getAddress(tpAddress, 0) == true)   // get the address so we can set the probe resolution
  {
    tprobe1 = 1;                                  // address was found so there was a probe
    sensor1.setResolution(tpAddress, mySetupData->get_tempprecision());    // set probe resolution
    DebugPrint(SETTPROBERESSTR);
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
    sensor1.requestTemperatures();                // request the sensor to begin a temperature reading
  }
  else
  {
    DebugPrintln(TPROBENOTFOUNDSTR);
  }
}

void temp_setresolution(byte rval)
{
  sensor1.setResolution(tpAddress, rval);         // set the probe resolution (9=0.25, 12=0.00125)
}

float read_temp(byte new_measurement)
{
  static float lasttemp = 20.0;                   // keep track of previous temperature value
  if (!new_measurement)
  {
    return lasttemp;                              // return previous measurement
  }

  float result = sensor1.getTempCByIndex(0);      // get temperature, always in celsius
  DebugPrint(TEMPSTR);
  DebugPrintln(result);
  if (result > -40.0 && result < 80.0)            // avoid erronous readings
  {
    lasttemp = result;
  }
  else
  {
    result = lasttemp;
  }
  return result;
}

void update_temp(void)
{
  static byte tcchanged = mySetupData->get_tempcompenabled();  // keeps track if tempcompenabled changes

  if (tprobe1 == 1)
  {
    static unsigned long lasttempconversion = 0;
    static byte requesttempflag = 0;                  // start with request
    unsigned long tempnow = millis();

    // see if the temperature needs updating - done automatically every 1.5s
    if (TimeCheck(lasttempconversion, TEMPREFRESHRATE))   // see if the temperature needs updating
    {
      static float tempval;
      static float starttemp;                         // start temperature to use when temperature compensation is enabled

      if ( tcchanged != mySetupData->get_tempcompenabled() )
      {
        tcchanged = mySetupData->get_tempcompenabled();
        if ( tcchanged == 1 )
        {
          starttemp = read_temp(1);
        }
      }

      lasttempconversion = tempnow;                   // update time stamp

      if (requesttempflag)
      {
        tempval = read_temp(1);
      }
      else
      {
        sensor1.requestTemperatures();
      }

      requesttempflag ^= 1; // toggle flag

      if (mySetupData->get_tempcompenabled() == 1)    // check for temperature compensation
      {
        if ((abs)(starttemp - tempval) >= 1)          // calculate if temp has moved by more than 1 degree
        {
          unsigned long newPos;
          byte temperaturedirection;                  // did temperature fall (1) or rise (0)?
          temperaturedirection = (tempval < starttemp) ? 1 : 0;
          if (mySetupData->get_tcdirection() == 0)    // check if tc direction for compensation is inwards
          {
            // temperature compensation direction is in, if a fall then move in else move out
            if ( temperaturedirection == 1 )          // check if temperature is falling
            {
              newPos = ftargetPosition - mySetupData->get_tempcoefficient();    // then move inwards
            }
            else
            {
              newPos = ftargetPosition + mySetupData->get_tempcoefficient();    // else move outwards
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
          starttemp = tempval;                        // save this current temp point for future reference
        } // end of check for tempchange >=1
      } // end of check for tempcomp enabled
    } // end of check for temperature needs updating
  } // end of if tprobe
}
