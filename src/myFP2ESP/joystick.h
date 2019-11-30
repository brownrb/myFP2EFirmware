// ----------------------------------------------------------------------------------------------
// myFP2ESP JOYSTICK ROUTINES AND DEFINITIONS
// ----------------------------------------------------------------------------------------------
// 2-AXIS Analog Thumb Joystick for Arduino 3V-5V
// https://www.ebay.com/itm/1PCS-New-PSP-2-Axis-Analog-Thumb-GAME-Joystick-Module-3V-5V-For-arduino-PSP/401236361097
// https://www.ebay.com/itm/1PCS-New-PSP-2-Axis-Analog-Thumb-GAME-Joystick-Module-3V-5V-For-arduino-PSP/232426858990
//
// On ESP32 analog input is 0-4095. GND=GND, VCC=3.3V
// ADC2 pins cannot be used when WiFi is being used
// ADC2 [GPIO4/GPIO2/GPIO15/GPIO13/GPIO12/GPIO14/GPIO27/GPIO26/GPIO25]
// If using WiFi use ADC1 pins
// ADC1 [GPIO33/GPIO32/GPIO35/GPIO34/GPIO39/GPIO36]

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2019. All Rights Reserved.
// (c) Copyright Holger M, 2019. All Rights Reserved.
// ----------------------------------------------------------------------------------------------

#ifndef joystick_h
#define joystick_h

// ----------------------------------------------------------------------------------------------
// EXTERNS
// ----------------------------------------------------------------------------------------------
extern SetupData *mySetupData;
extern DriverBoard* driverboard;
extern unsigned long fcurrentPosition;      // current focuser position
extern unsigned long ftargetPosition;       // target position

// ----------------------------------------------------------------------------------------------
// DATA
// ----------------------------------------------------------------------------------------------

// range is 0-4095, so midpoint is 2046/7
// halfway to left is 1024 and halfway to right is 3070
// map(value, fromLow, fromHigh, toLow, toHigh)
// map(value, 0, 4095, 0, 2)  // left, middle, right and up, middle, down

// Alternatively, instead of mapping motorspeed, map driverboard->stepdelay
// pickup MSSLOW, MSFAST limits and map this way
// map(motorspd, 0, 4095, MSSLOW, MSFAST)

#ifdef JOYSTICK
#define JOYLEFT       1024    // <= 
#define JOYRIGHT      3070    // >=
#define JOYSLOW       1024    // <=
#define JOYMED        3070    // >=
#define JOYFAST       2046    //

#define JOYINOUTPIN   34
#define JOYSPEEDPIN   35
#endif // JOYSTICK

// ----------------------------------------------------------------------------------------------
// CODE
// ----------------------------------------------------------------------------------------------
#ifdef JOYSTICK

void update_joystick(void)
{
  int joypos;
  int joyspeed;
  //int motorspd;
  
  joypos = analogRead(JOYINOUTPIN);
  DebugPrint("X joypos:");
  DebugPrintln(joypos);
  joypos = map(joypos, 0, 4095, 0, 2);
  joyspeed = analogRead(JOYSPEEDPIN);
  DebugPrint("Y joyspeed:");
  DebugPrintln(joyspeed);
  //joyspeed = map(joyspeed, 0, 4095, 0, 2);
  joyspeed = map(joyspeed, 0, 4095, MSSLOW, MSFAST);
  
  // process joypos
  if ( joypos == 0 )      // move IN
  {
    ftargetPosition--;
  }
  else if ( joypos == 2)  // move OUT
  {
    ftargetPosition++;
    if ( ftargetPosition > mySetupData->get_maxstep())
    {
      ftargetPosition = mySetupData->get_maxstep();
    }
  }

  // process joyspeed
  driverboard->setstepdelay(joyspeed);
  
  /*
  // process joyspeed
  motorspd = mySetupData->get_motorSpeed();
  switch ( joyspeed )
  {
    case 0:
      // decrement speed
      motorspd--;
      if( motorspd < 0)
      {
        motorspd = 0;
      }
      mySetupData->set_motorSpeed((byte)motorspd);
      break;
    case 1:
      // ignore, leave current speed alone
      break;
    case 2:
      // increment speed
      motorspd++;
      if( motorspd > 2)
      {
        motorspd = 2;
      }
      mySetupData->set_motorSpeed((byte)motorspd);
      break;
  }
  */
}
#endif // #ifdef JOYSTICK


#endif // #ifndef joystick_h
