#include <Arduino.h>

#include "generalDefinitions.h"
#include "chipModels.h"
#include "myBoardTypes.h"
#include "myBoards.h"

#if (DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2ESP32DRV8825 )
// this is DRV8825 constructor
DriverBoard::DriverBoard(byte brdtype, String brdname, byte smode, byte mspd) : boardtype(brdtype), boardname(brdname)
{
  pinMode(DRV8825ENABLE, OUTPUT);
  pinMode(DRV8825DIR, OUTPUT);
  pinMode(DRV8825STEP, OUTPUT);
  digitalWrite(DRV8825ENABLE, 1);
  setstepmode( smode );
  setmotorspeed( mspd);
}
#endif

#if (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N || DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2EL9110S \
 || DRVBRD == PRO2EESP32ULN2003 || DRVBRD == PRO2EESP32L298N || DRVBRD == PRO2ESP32L293DMINI \
 || DRVBRD == PRO2ESP32L9110S )
// this is constructor for ULN2003, L298N, L9110S and L293DMINI driver boards
DriverBoard::DriverBoard(byte brdtype, String brdname, byte smode, byte mspd, byte pin1, byte pin2, byte pin3, byte pin4) : boardtype(brdtype), boardname(brdname)
{
  // Move the init of inputPins here before init of myStepper to prevent stepper motor jerk
  this->inputPins[0] = pin1;
  this->inputPins[1] = pin2;
  this->inputPins[2] = pin3;
  this->inputPins[3] = pin4;
  for (int inputCount = 0; inputCount < 4; inputCount++) {
    pinMode(this->inputPins[inputCount], OUTPUT);
  }
  switch ( boardtype )
  {
    case PRO2EULN2003:
    case PRO2ESP32ULN2003:
      mystepper = new HalfStepper(STEPSPERREVOLUTION, pin1, pin2, pin3, pin4);
      this->stepdelay = ULNFAST;
      break;
    case PRO2EL298N:
    case PRO2ESP32L298N:
      mystepper = new HalfStepper(STEPSPERREVOLUTION, pin1, pin2, pin3, pin4);
      this->stepdelay = L298NFAST;
      break;
    case PRO2EL293DMINI:
    case PRO2ESP32L293DMINI:
      mystepper = new HalfStepper(STEPSPERREVOLUTION, pin1, pin2, pin3, pin4);
      this->stepdelay = L293DMINIFAST;
      break;
    case PRO2EL9110S:
    case PRO2ESP32L9110S:
      mystepper = new HalfStepper(STEPSPERREVOLUTION, pin1, pin2, pin3, pin4);
      this->stepdelay = L9110SFAST;
      break;
    default:
      // do nothing
      break;
  }
  setstepmode( smode );
  setmotorspeed( mspd);
}
#endif

#if (DRVBRD == PRO2EL293D )
// constructor for L293D motor shield
DriverBoard::DriverBoard(byte brdtype, String brdname, byte stepmode, byte mspeed, byte pin1, byte pin2, byte pin3, byte pin4, byte stepperrpm) : boardtype(brdtype), boardname(brdname)
{
  mystepper = new Stepper(STEPSPERREVOLUTION, pin1, pin2, pin3, pin4);
  this->stepdelay = L293DFAST;
  this->inputPins[0] = pin1;
  this->inputPins[1] = pin2;
  this->inputPins[2] = pin3;
  this->inputPins[3] = pin4;
  setstepmode(stepmode);    // sets stepmode correctly
  setmotorspeed(mspeed);    // sets delay correcrly
  mystepper->setSpeed(stepperrpm);
}
#endif

byte DriverBoard::getstepmode(void)
{
  return this->stepmode;
}

void DriverBoard::setstepmode(byte smode)
{
  switch (this->boardtype)
  {
#if (DRVBRD == PRO2EDRV8825)
    case PRO2EDRV8825:
      // for DRV8825 stepmode is set in hardware jumpers
      // cannot set by software
      smode = DRV8825TEPMODE;       // defined at beginning of myBoards.h
      break;
#endif
#if (DRVBRD == PRO2ESP32DRV8825 )
    case PRO2ESP32DRV8825:
      smode = (smode < STEP1 ) ? STEP1 : smode;
      smode = (smode > STEP32 ) ? STEP32 : smode;
      break;
#endif
#if (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N || DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2EL9110S \
 || DRVBRD == PRO2EESP32ULN2003 || DRVBRD == PRO2EESP32L298N || DRVBRD == PRO2ESP32L293DMINI \
 || DRVBRD == PRO2ESP32L9110S )
    case PRO2EULN2003:
    case PRO2ESP32ULN2003:
    case PRO2EL298N:
    case PRO2ESP32L298N:
    case PRO2EL293DMINI:
    case PRO2ESP32L293DMINI:
    case PRO2EL9110S:
    case PRO2ESP32L9110S:
      switch ( smode )
      {
        case STEP1:
          this->stepmode = STEP1;
          // put this inside DRVBRD test else compiler throws a fit on next line
          mystepper->SetSteppingMode(SteppingMode::FULL);
          break;
        case STEP2:
          this->stepmode = STEP2;
          // put this inside DRVBRD test else compiler throws a fit on next line
          mystepper->SetSteppingMode(SteppingMode::HALF);
          break;
        default:
          smode = STEP1;
          // put this inside DRVBRD test else compiler throws a fit on next line
          mystepper->SetSteppingMode(SteppingMode::FULL);
          this->stepmode = smode;
          break;
      }
      break;
#endif
#if (DRVBRD == PRO2EL293D )
    case PRO2EL293D:
      this->stepmode = STEP1;
      break;
#endif
    default:
      smode = STEP1;
      this->stepmode = smode;
      break;
  }
}

String DriverBoard::getname(void)
{
  return this->boardname;
}

void DriverBoard::enablemotor(void)
{
  switch (this->boardtype)
  {
#if (DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2ESP32DRV8825 )
    case PRO2EDRV8825:
    case PRO2ESP32DRV8825:
      digitalWrite(DRV8825ENABLE, 0);
      break;
#endif
    default:
      // do nothing;
      break;
  }
  delay(1);                     // boards require 1ms before stepping can occur
}

void DriverBoard::releasemotor(void)
{
  switch (this->boardtype)
  {
#if (DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2ESP32DRV8825 )
    case PRO2EDRV8825:
    case PRO2ESP32DRV8825:
      digitalWrite(DRV8825ENABLE, 1);
      break;
#endif
#if (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N || DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2EL9110S \
 || DRVBRD == PRO2EESP32ULN2003 || DRVBRD == PRO2EESP32L298N || DRVBRD == PRO2ESP32L293DMINI \
 || DRVBRD == PRO2ESP32L9110S || DRVBRD == PRO2EL293D )
    case PRO2EULN2003:
    case PRO2ESP32ULN2003:
    case PRO2EL298N:
    case PRO2ESP32L298N:
    case PRO2EL293DMINI:
    case PRO2ESP32L293DMINI:
    case PRO2EL9110S:
    case PRO2ESP32L9110S:
    case PRO2EL293D:
      digitalWrite(this->inputPins[0], 0 );
      digitalWrite(this->inputPins[1], 0 );
      digitalWrite(this->inputPins[2], 0 );
      digitalWrite(this->inputPins[3], 0 );
      break;
#endif
    default:
      // do nothing;
      break;
  }
}

void DriverBoard::movemotor(byte dir)
{
  // handling of inout leds when moving done in main code
  switch (this->boardtype)
  {
#if (DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2ESP32DRV8825 )
    case PRO2EDRV8825:
    case PRO2ESP32DRV8825:
      digitalWrite(DRV8825DIR, dir);        // set Direction of travel
      digitalWrite(DRV8825ENABLE, 0);       // Enable Motor Driver
      digitalWrite(DRV8825STEP, 1);         // Step pin on
      delayMicroseconds(MOTORPULSETIME);
      digitalWrite(DRV8825STEP, 0);
      switch ( this->stepmode )
      {
        case STEP1:
          delayMicroseconds(this->stepdelay);
          break;
        case STEP2:
          delayMicroseconds(this->stepdelay / 2);
          break;
        case STEP4:
          delayMicroseconds(this->stepdelay / 4);
          break;
        case STEP8:
        case STEP16:
        case STEP32:
          delayMicroseconds(this->stepdelay / 8);
          break;
        default:
          delayMicroseconds(this->stepdelay);
          break;
      }
      break;
#endif
#if (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N || DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2EL9110S \
 || DRVBRD == PRO2EESP32ULN2003 || DRVBRD == PRO2EESP32L298N || DRVBRD == PRO2ESP32L293DMINI \
 || DRVBRD == PRO2ESP32L9110S || DRVBRD == PRO2EL293D )
    case PRO2EULN2003:
    case PRO2ESP32ULN2003:
    case PRO2EL298N:
    case PRO2ESP32L298N:
    case PRO2EL293DMINI:
    case PRO2ESP32L293DMINI:
    case PRO2EL9110S:
    case PRO2ESP32L9110S:
    case PRO2EL293D:
      // put this inside DRVBRD test else compiler throws a fit on next line
      (dir == 0 ) ? mystepper->step(1) : mystepper->step(-1);
      delayMicroseconds(this->stepdelay);
      break;
#endif
    default:
      // do nothing
      break;
  }
}

void DriverBoard::setstepdelay(int sdelay)
{
  this->stepdelay = sdelay;
}

byte DriverBoard::getmotorspeed(void)
{
  byte retspd = FAST;

  switch (this->boardtype)
  {
#if (DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2ESP32DRV8825 )
    case PRO2EDRV8825:
    case PRO2ESP32DRV8825:
      switch ( this->stepdelay)
      {
        case DRVFAST : retspd = FAST;
          break;
        case DRVMED : retspd = MED;
          break;
        case DRVSLOW : retspd = SLOW;
      }
      break;
#endif
#if (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2ESP32ULN2003 )
    case PRO2EULN2003:
    case PRO2ESP32ULN2003:
      switch ( this->stepdelay)
      {
        case ULNFAST : retspd = FAST;
          break;
        case ULNMED : retspd = MED;
          break;
        case ULNSLOW : retspd = SLOW;
      }
      break;
#endif
#if (DRVBRD == PRO2EL298N || DRVBRD == PRO2ESP32L298N )
    case PRO2EL298N:
    case PRO2ESP32L298N:
      switch ( this->stepdelay)
      {
        case L298NFAST : retspd = FAST;
          break;
        case L298NMED : retspd = MED;
          break;
        case L298NSLOW : retspd = SLOW;
      }
      break;
#endif
#if (DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2ESP32L293DMINI )
    case PRO2EL293DMINI:
    case PRO2ESP32L293DMINI:
      switch ( this->stepdelay)
      {
        case L293DMINIFAST : retspd = FAST;
          break;
        case L293DMINIMED : retspd = MED;
          break;
        case L293DMINISLOW : retspd = SLOW;
      }
      break;
#endif
#if (DRVBRD == PRO2EL9110S || DRVBRD == PRO2ESP32L9110S )
    case PRO2EL9110S:
    case PRO2ESP32L9110S:
      switch ( this->stepdelay)
      {
        case L9110SFAST : retspd = FAST;
          break;
        case L9110SMED : retspd = MED;
          break;
        case L9110SSLOW : retspd = SLOW;
      }
      break;
#endif
#if (DRVBRD == PRO2EL293D )
    case PRO2EL293D:
      switch ( this->stepdelay)
      {
        case L293DFAST : retspd = FAST;
          break;
        case L293DMED : retspd = MED;
          break;
        case L293DSLOW : retspd = SLOW;
      }
      break;
#endif
    default:
      switch ( this->stepdelay)
      {
        case MSFAST : retspd = FAST;
          break;
        case MSMED : retspd = MED;
          break;
        case MSSLOW : retspd = SLOW;
          break;
        default:
          retspd = FAST;
      }
      break;
  }
  return retspd;
}

void DriverBoard::setmotorspeed(byte spd )
{
  switch ( spd )
  {
    case SLOW:
      switch (this->boardtype)
      {
#if (DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2ESP32DRV8825 )
        case PRO2EDRV8825:
        case PRO2ESP32DRV8825:
          this->stepdelay = DRVSLOW;
          break;
#endif
#if (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2ESP32ULN2003 )
        case PRO2EULN2003:
        case PRO2ESP32ULN2003:
          this->stepdelay = ULNSLOW;
          break;
#endif
#if (DRVBRD == PRO2EL298N || DRVBRD == PRO2ESP32L298N )
        case PRO2EL298N:
        case PRO2ESP32L298N:
          this->stepdelay = L298NSLOW;
          break;
#endif
#if (DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2ESP32L293DMINI )
        case PRO2EL293DMINI:
        case PRO2ESP32L293DMINI:
          this->stepdelay = L293DMINISLOW;
          break;
#endif
#if (DRVBRD == PRO2EL9110S || DRVBRD == PRO2ESP32L9110S )
        case PRO2EL9110S:
        case PRO2ESP32L9110S:
          this->stepdelay = L9110SSLOW;
          break;
#endif
#if (DRVBRD == PRO2EL293D)
        case PRO2EL293D:
          this->stepdelay = L293DSLOW;
          break;
#endif
        default:
          this->stepdelay = MSSLOW;
          break;
      }
      break;
    case MED:
      switch (this->boardtype)
      {
#if (DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2ESP32DRV8825 )
        case PRO2EDRV8825:
        case PRO2ESP32DRV8825:
          this->stepdelay = DRVMED;
          break;
#endif
#if (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2ESP32ULN2003 )
        case PRO2EULN2003:
        case PRO2ESP32ULN2003:
          this->stepdelay = ULNMED;
          break;
#endif
#if (DRVBRD == PRO2EL298N || DRVBRD == PRO2ESP32L298N )
        case PRO2EL298N:
        case PRO2ESP32L298N:
          this->stepdelay = L298NMED;
          break;
#endif
#if (DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2ESP32L293DMINI )
        case PRO2EL293DMINI:
        case PRO2ESP32L293DMINI:
          this->stepdelay = L293DMINIMED;
          break;
#endif
#if (DRVBRD == PRO2EL9110S || DRVBRD == PRO2ESP32L9110S )
        case PRO2EL9110S:
        case PRO2ESP32L9110S:
          this->stepdelay = L9110SMED;
          break;
#endif
#if (DRVBRD == PRO2EL293D)
        case PRO2EL293D:
          this->stepdelay = L293DMED;
          break;
#endif
        default:
          this->stepdelay = MSMED;
          break;
      }
      break;
    case FAST:
      switch (this->boardtype)
      {
#if (DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2ESP32DRV8825 )
        case PRO2EDRV8825:
        case PRO2ESP32DRV8825:
          this->stepdelay = DRVFAST;
          break;
#endif
#if (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2ESP32ULN2003 )
        case PRO2EULN2003:
        case PRO2ESP32ULN2003:
          this->stepdelay = ULNFAST;
          break;
#endif
#if (DRVBRD == PRO2EL298N || DRVBRD == PRO2ESP32L298N )
        case PRO2EL298N:
        case PRO2ESP32L298N:
          this->stepdelay = L298NFAST;
          break;
#endif
#if (DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2ESP32L293DMINI )
        case PRO2EL293DMINI:
        case PRO2ESP32L293DMINI:
          this->stepdelay = L293DMINIFAST;
          break;
#endif
#if (DRVBRD == PRO2EL9110S || DRVBRD == PRO2ESP32L9110S )
        case PRO2EL9110S:
        case PRO2ESP32L9110S:
          this->stepdelay = L9110SFAST;
          break;
#endif
#if (DRVBRD == PRO2EL293D)
        case PRO2EL293D:
          this->stepdelay = L293DFAST;
          break;
#endif
        default:
          this->stepdelay = MSFAST;
          break;
      }
      break;
  }
}
