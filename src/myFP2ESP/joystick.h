// ----------------------------------------------------------------------------------------------
// myFP2ESP JOYSTICK ROUTINES AND DEFINITIONS
// ----------------------------------------------------------------------------------------------
// 2-AXIS Analog Thumb Joystick for Arduino
// https://www.ebay.com/itm/1PCS-New-PSP-2-Axis-Analog-Thumb-GAME-Joystick-Module-3V-5V-For-arduino-PSP/401236361097
// https://www.ebay.com/itm/1PCS-New-PSP-2-Axis-Analog-Thumb-GAME-Joystick-Module-3V-5V-For-arduino-PSP/232426858990
//
// Keyes KY-023 PS2 style 2-Axis Joystick with Switch
// https://www.ebay.com/itm/Joy-Stick-Breakout-Module-Shield-PS2-Joystick-Game-Controller-For-Arduino/293033141970
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
// DATA AND DEFINES
// ----------------------------------------------------------------------------------------------

#ifdef JOYSTICK1
#define JOYINOUTPIN   34      // ADC1_6, D34 - Wire to X
#define JOYOTHERPIN   35      // ADC1_7, D35 - Do not wire
#define JZEROPOINT    2047
#define JTHRESHOLD    20
#endif //

#ifdef JOYSTICK2
#define JOYINOUTPIN   34      // ADC1_6, D34 - Wire to VRx
#define JOYOTHERPIN   35      // ADC1_7, D35 - Wire to SW
#define JZEROPOINT    2047
#define JTHRESHOLD    20
volatile int joy2swstate;
#endif

// ----------------------------------------------------------------------------------------------
// CODE JOYSTICK1 : DO NOT CHANGE : 2-AXIS Analog Thumb Joystick for Arduino
// ----------------------------------------------------------------------------------------------
#ifdef JOYSTICK1
void update_joystick1(void)
{
  int joypos;
  int joyspeed;
  int joyval;

  joyval = analogRead(JOYINOUTPIN);             // range is 0 - 4095, midpoint is 2047
  DebugPrint("X joypos:");
  DebugPrintln(joyval);
  if ( joyval < (JZEROPOINT - JTHRESHOLD) )
  {
    ftargetPosition--;                          // move IN
    joyval = joyval - JZEROPOINT;
    if ( joyval < 0 )
    {
      joyval = JZEROPOINT - joyval;
    }
    joyspeed = map(joyval, 0, JZEROPOINT, MSSLOW, MSFAST);
    driverboard->setstepdelay(joyspeed);
  }
  else if ( joyval > (JZEROPOINT + JTHRESHOLD) )
  {
    ftargetPosition++;                          // move OUT
    if ( ftargetPosition > mySetupData->get_maxstep())
    {
      ftargetPosition = mySetupData->get_maxstep();
    }
    joyval = joyval - JZEROPOINT;
    if ( joyval < 0 )
    {
      joyval = JZEROPOINT - joyval;
    }
    joyspeed = map(joyval, 0, JZEROPOINT, MSSLOW, MSFAST);
    driverboard->setstepdelay(joyspeed);
  }
}

void init_joystick1(void)
{
  // perform any inititalisations necessary
  // for future use
  pinMode(JOYINOUTPIN, INPUT);
  pinMode(JOYOTHERPIN, INPUT);
}
#endif // #ifdef JOYSTICK1

// ----------------------------------------------------------------------------------------------
// CODE JOYSTICK2 : YOU CAN CHANGE THIS IF YOU WANT : Keyes KY-023 PS2 style 2-Axis Joystick
// IT REMAINS UNSUPPORTED TO ALLOW YOU TO CONFIGURE AS YOU WISH
// ----------------------------------------------------------------------------------------------
#ifdef JOYSTICK2

void IRAM_ATTR joystick2sw_isr()
{
  joy2swstate = !(digitalRead(JOYOTHERPIN));  // joy2swstate will be 1 when switch is pressed
}

void update_joystick2(void)
{
  int joypos;
  int joyspeed;
  int joyval;

  joyval = analogRead(JOYINOUTPIN);             // range is 0 - 4095, midpoint is 2047
  DebugPrint("X joypos:");
  DebugPrintln(joyval);
  if ( joyval < (JZEROPOINT - JTHRESHOLD) )
  {
    ftargetPosition--;                          // move IN
    joyval = joyval - JZEROPOINT;
    if ( joyval < 0 )
    {
      joyval = JZEROPOINT - joyval;
    }
    joyspeed = map(joyval, 0, JZEROPOINT, MSSLOW, MSFAST);
    driverboard->setstepdelay(joyspeed);
  }
  else if ( joyval > (JZEROPOINT + JTHRESHOLD) )
  {
    ftargetPosition++;                          // move OUT
    if ( ftargetPosition > mySetupData->get_maxstep())
    {
      ftargetPosition = mySetupData->get_maxstep();
    }
    joyval = joyval - JZEROPOINT;
    if ( joyval < 0 )
    {
      joyval = JZEROPOINT - joyval;
    }
    joyspeed = map(joyval, 0, JZEROPOINT, MSSLOW, MSFAST);
    driverboard->setstepdelay(joyspeed);
  }

  if ( joy2swstate == 1)     // switch is pressed
  {
    // user defined code here
    // could be a halt
    // could be a home
    // could be a preset
    // insert code here

    // finally reset joystick switch state
    joy2swstate = 0;
  }
}

void init_joystick2(void)
{
  pinMode(JOYINOUTPIN, INPUT);
  pinMode(JOYOTHERPIN, INPUT_PULLUP);
  // setup interrupt, falling, when switch is pressed, pin falls from high to low
  attachInterrupt(JOYOTHERPIN, joystick2sw_isr, FALLING);
  joy2swstate = 0;
}
#endif // #ifdef JOYSTICK2

#endif // #ifndef joystick_h
