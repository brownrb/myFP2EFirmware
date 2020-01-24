// ----------------------------------------------------------------------------------------------
// TITLE: myFP2ESP DRIVER BOARD CODE
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2020. All Rights Reserved.
// (c) Copyright Holger M, 2019-2020. All Rights Reserved.
// ----------------------------------------------------------------------------------------------

#include <Arduino.h>
#include "generalDefinitions.h"
#include "myBoards.h"

DriverBoard::DriverBoard(byte brdtype) : boardtype(brdtype)
{
  switch ( brdtype )
  {
#if (DRVBRD == WEMOSDRV8825    || DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2EDRV8825BIG || DRVBRD == PRO2ESP32R3WEMOS )
    case WEMOSDRV8825:
    case PRO2EDRV8825:
    case PRO2EDRV8825BIG:
    case PRO2ESP32R3WEMOS:
      pinMode(ENABLEPIN, OUTPUT);
      pinMode(DIRPIN, OUTPUT);
      pinMode(STEPPIN, OUTPUT);
      digitalWrite(ENABLEPIN, 1);
      DriverBoard::setstepmode(DRV8825TEPMODE);
      break;
#endif
#if( DRVBRD == PRO2ESP32DRV8825 )
    case PRO2ESP32DRV8825:
      pinMode(ENABLEPIN, OUTPUT);
      pinMode(DIRPIN, OUTPUT);
      pinMode(STEPPIN, OUTPUT);
      digitalWrite(ENABLEPIN, 1);
      pinMode(MS1, OUTPUT);
      pinMode(MS2, OUTPUT);
      pinMode(MS3, OUTPUT);
      break;
#endif
#if (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N || DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2EL9110S \
  || DRVBRD == PRO2ESP32ULN2003 || DRVBRD == PRO2ESP32L298N || DRVBRD == PRO2ESP32L2933DMINI \
  || DRVBRD == PRO2ESP32L9110S)
    case PRO2EULN2003:
    case PRO2EL298N:
    case PRO2EL293DMINI:
    case PRO2EL9110S:
    case PRO2ESP32ULN2003:
    case PRO2ESP32L298N:
    case PRO2ESP32L293DMINI:
    case PRO2ESP32L9110S:
#if (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2ESP32ULN2003)
      // IN1ULN, IN3ULN, IN4ULN, IN2ULN
      mystepper = new HalfStepper(STEPSPERREVOLUTION, IN1, IN3, IN4, IN2);
#endif
#if (DRVBRD == PRO2EL298N || DRVBRD == PRO2ESP32L298N)
      // IN1L298N, IN2L298N, IN3L298N, IN4L298N
      mystepper = new HalfStepper(STEPSPERREVOLUTION, IN1, IN2, IN3, IN4);
#endif
#if (DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2ESP32L293DMINI)
      // IN1L293DMINI, IN2L293DMINI, IN3L293DMINI, IN4L293DMINI
      mystepper = new HalfStepper(STEPSPERREVOLUTION, IN1, IN2, IN3, IN4);
#endif
#if (DRVBRD == PRO2EL9110S || DRVBRD == PRO2ESP32L9110S)
      // IN1L9110S, IN2L9110S, IN3L9110S, IN4L9110S
      mystepper = new HalfStepper(STEPSPERREVOLUTION, IN1, IN2, IN3, IN4);
#endif
      // Move the init of inputPins here before init of myStepper to prevent stepper motor jerk
      this->inputPins[0] = IN1;
      this->inputPins[1] = IN2;
      this->inputPins[2] = IN3;
      this->inputPins[3] = IN4;
      for (int inputCount = 0; inputCount < 4; inputCount++) {
        pinMode(this->inputPins[inputCount], OUTPUT);
      }
#if (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2ESP32ULN2003)
      mystepper->setSpeed(5);      // DONE
#endif
#if (DRVBRD == PRO2EL298N || DRVBRD == PRO2ESP32L298N)
      mystepper->setSpeed(20);      // DONE
#endif
#if (DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2ESP32L293DMINI)
      mystepper->setSpeed(5);       // TODO
#endif
#if (DRVBRD == PRO2EL9110S || DRVBRD == PRO2ESP32L9110S)
      mystepper->setSpeed(5);       // TODO
#endif
      this->stepdelay = MSFAST;
      setmotorspeed(FAST);
      break;
#endif
#if (DRVBRD == PRO2EL293DNEMA || DRVBRD == PRO2EL293D28BYJ48 )
    case PRO2EL293DNEMA:
    case PRO2EL293D28BYJ48:
      // Move the init of inputPins here before init of myStepper to prevent stepper motor jerk
#if (DRVBRD == PRO2EL293DNEMA )
      mystepper = new Stepper(STEPSPERREVOLUTION, IN2, IN3, IN1, IN4);
      // constructor was IN2, IN3, IN1, IN4 for NEMA14
      this->inputPins[0] = IN1;
      this->inputPins[1] = IN2;
      this->inputPins[2] = IN3;
      this->inputPins[3] = IN4;
      for (int inputCount = 0; inputCount < 4; inputCount++)
      {
        pinMode(this->inputPins[inputCount], OUTPUT);
      }
      mystepper->setSpeed(SPEEDNEMA);
#endif
#if (DRVBRD == PRO2EL293D28BYJ48 )
      mystepper = new Stepper(STEPSPERREVOLUTION, IN1, IN3, IN2, IN4);
      // constructor was IN1, IN3, IN2, IN4 for 28BYJ48
      this->inputPins[0] = IN1;
      this->inputPins[1] = IN3;
      this->inputPins[2] = IN2;
      this->inputPins[3] = IN4;
      for (int inputCount = 0; inputCount < 4; inputCount++)
      {
        pinMode(this->inputPins[inputCount], OUTPUT);
      }
      mystepper->setSpeed(SPEEDBIPOLAR);
#endif
      setstepmode(STEP1);
      setmotorspeed(FAST);
      break;
#endif
    default:
      // do nothing
      break;
  }
}

// destructor
DriverBoard::~DriverBoard()
{
#if ( DRVBRD == PRO2EULN2003   || DRVBRD == PRO2ESP32ULN2003  \
   || DRVBRD == PRO2EL298N     || DRVBRD == PRO2ESP32L298N    \
   || DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2ESP32L293MINI \
   || DRVBRD == PRO2EL9110S    || DRVBRD == PRO2ESPL9110S)    \
   || (DRVBRD == PRO2EL293DNEMA || DRVBRD == PRO2EL293D28BYJ48 )
  delete mystepper;
#endif
}

String DriverBoard::getboardname(void)
{
  String retstr;
  switch ( boardtype )
  {
    case WEMOSDRV8825:
      retstr  = F("WEMOSDRV8825");
      break;
    case PRO2EDRV8825:
      retstr = F("PRO2EDRV8825");
      break;
    case PRO2EDRV8825BIG:
      retstr = F("PRO2EDRV8825BIG");
      break;
    case PRO2EULN2003:
      retstr  = F("PRO2EULN2003");
      break;
    case PRO2EL293DNEMA:
      retstr  = F("PRO2EL293DNEMA");
      break;
    case PRO2EL293D28BYJ48:
      retstr = F("PRO2EL293D28BYJ48");
      break;
    case PRO2EL298N:
      retstr = F("PRO2EL298N");
      break;
    case PRO2EL293DMINI:
      retstr = F("PRO2EL293DMINI");
      break;
    case PRO2EL9110S:
      retstr = F("PRO2EL9110S");
      break;
    case PRO2ESP32DRV8825:
      retstr = F("PRO2ESP32DRV8825");
      break;
    case PRO2ESP32ULN2003:
      retstr = F("PRO2ESP32ULN2003");
      break;
    case PRO2ESP32L298N:
      retstr = F("PRO2ESP32L298N");
      break;
    case PRO2ESP32L293DMINI:
      retstr = F("PRO2ESP32L293DMINI");
      break;
    case PRO2ESP32L9110S:
      retstr = F("PRO2ESP32L9110S");
      break;
    case PRO2ESP32R3WEMOS:
      retstr = F("PRO2ESP32R3WEMOS");
      break;
    default:
      retstr = F("UNKNOWN");
      break;
  }
  return retstr;
}

byte DriverBoard::getstepmode(void)
{
  return this->stepmode;
}

void DriverBoard::setstepmode(byte smode)
{
  switch (this->boardtype)
  {
#if (DRVBRD == WEMOSDRV8825    || DRVBRD == PRO2EDRV8825 \
  || DRVBRD == PRO2EDRV8825BIG || DRVBRD == PRO2ESP32R3WEMOS )
    case WEMOSDRV8825:
    case PRO2EDRV8825:
    case PRO2EDRV8825BIG:
    case PRO2ESP32R3WEMOS:
      // for DRV8825 stepmode is set in hardware jumpers
      // cannot set by software
      smode = DRV8825TEPMODE;       // defined at beginning of myBoards.h
      break;
#endif
#if (DRVBRD == PRO2ESP32DRV8825 )
    case PRO2ESP32DRV8825:
      smode = (smode < STEP1 ) ? STEP1 : smode;
      smode = (smode > STEP32 ) ? STEP32 : smode;
      this->stepmode = smode;
      switch (smode)
      {
        case STEP1:
          digitalWrite(MS1, 0);
          digitalWrite(MS2, 0);
          digitalWrite(MS3, 0);
          break;
        case STEP2:
          digitalWrite(MS1, 1);
          digitalWrite(MS2, 0);
          digitalWrite(MS3, 0);
          break;
        case STEP4:
          digitalWrite(MS1, 0);
          digitalWrite(MS2, 1);
          digitalWrite(MS3, 0);
          break;
        case STEP8:
          digitalWrite(MS1, 1);
          digitalWrite(MS2, 1);
          digitalWrite(MS3, 0);
          break;
        case STEP16:
          digitalWrite(MS1, 0);
          digitalWrite(MS2, 0);
          digitalWrite(MS3, 1);
          break;
        case STEP32:
          digitalWrite(MS1, 1);
          digitalWrite(MS2, 0);
          digitalWrite(MS3, 1);
          break;
        default:
          digitalWrite(MS1, 0);
          digitalWrite(MS2, 0);
          digitalWrite(MS3, 0);
          smode = STEP1;
          this->stepmode = smode;
          break;
      }
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
          mystepper->SetSteppingMode(SteppingMode::FULL);
          this->stepmode = STEP1;
          break;
        case STEP2:
          mystepper->SetSteppingMode(SteppingMode::HALF);
          this->stepmode = STEP2;
          break;
        default:
          mystepper->SetSteppingMode(SteppingMode::FULL);
          smode = STEP1;
          this->stepmode = smode;
          break;
      }
      break;
#endif
#if (DRVBRD == PRO2EL293DNEMA || DRVBRD == PRO2EL293D28BYJ48 )
    case PRO2EL293DNEMA:
    case PRO2EL293D28BYJ48:
      this->stepmode = STEP1;
      break;
#endif
    default:
      smode = STEP1;
      this->stepmode = smode;
      break;
  }
}

void DriverBoard::enablemotor(void)
{
  switch (this->boardtype)
  {
#if (DRVBRD == WEMOSDRV8825    || DRVBRD == PRO2EDRV8825 \
  || DRVBRD == PRO2EDRV8825BIG || DRVBRD == PRO2ESP32DRV8825 \
  || DRVBRD == PRO2ESP32R3WEMOS )
    case WEMOSDRV8825:
    case PRO2EDRV8825:
    case PRO2EDRV8825BIG:
    case PRO2ESP32DRV8825:
    case PRO2ESP32R3WEMOS:
      digitalWrite(ENABLEPIN, 0);
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
#if (DRVBRD == WEMOSDRV8825    || DRVBRD == PRO2EDRV8825 \
  || DRVBRD == PRO2EDRV8825BIG || DRVBRD == PRO2ESP32DRV8825 \
  || DRVBRD == PRO2ESP32R3WEMOS )
    case WEMOSDRV8825:
    case PRO2EDRV8825:
    case PRO2EDRV8825BIG:
    case PRO2ESP32DRV8825:
    case PRO2ESP32R3WEMOS:
      digitalWrite(ENABLEPIN, 1);
      break;
#endif
#if (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N || DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2EL9110S \
 || DRVBRD == PRO2EESP32ULN2003 || DRVBRD == PRO2EESP32L298N || DRVBRD == PRO2ESP32L293DMINI \
  || DRVBRD == PRO2ESP32L9110S || DRVBRD == PRO2EL293DNEMA || DRVBRD == PRO2EL293D28BYJ48)
    case PRO2EULN2003:
    case PRO2ESP32ULN2003:
    case PRO2EL298N:
    case PRO2ESP32L298N:
    case PRO2EL293DMINI:
    case PRO2ESP32L293DMINI:
    case PRO2EL9110S:
    case PRO2ESP32L9110S:
    case PRO2EL293DNEMA:
    case PRO2EL293D28BYJ48:
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
#if (DRVBRD == WEMOSDRV8825    || DRVBRD == PRO2EDRV8825 \
  || DRVBRD == PRO2EDRV8825BIG || DRVBRD == PRO2ESP32DRV8825 \
  || DRVBRD == PRO2ESP32R3WEMOS )
    case WEMOSDRV8825:
    case PRO2EDRV8825:
    case PRO2EDRV8825BIG:
    case PRO2ESP32DRV8825:
    case PRO2ESP32R3WEMOS:
      digitalWrite(DIRPIN, dir);            // set Direction of travel
      digitalWrite(ENABLEPIN, 0);           // Enable Motor Driver
      digitalWrite(STEPPIN, 1);             // Step pin on
      delayMicroseconds(MOTORPULSETIME);
      digitalWrite(STEPPIN, 0);
      delayMicroseconds(this->stepdelay);   // this controls speed of motor
      break;
#endif
#if (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N || DRVBRD == PRO2EL293DMINI \
  || DRVBRD == PRO2EL9110S  || DRVBRD == PRO2EESP32ULN2003 || DRVBRD == PRO2EESP32L298N \
  || DRVBRD == PRO2ESP32L293DMINI || DRVBRD == PRO2ESP32L9110S)
    case PRO2EULN2003:
    case PRO2ESP32ULN2003:
    case PRO2EL298N:
    case PRO2ESP32L298N:
    case PRO2EL293DMINI:
    case PRO2ESP32L293DMINI:
    case PRO2EL9110S:
    case PRO2ESP32L9110S:
      (dir == 0 ) ? mystepper->step(1) : mystepper->step(-1);
      delayMicroseconds(this->stepdelay);
      break;
#endif
#if( DRVBRD == PRO2EL293DNEMA || DRVBRD == PRO2EL293D28BYJ48)
    case PRO2EL293DNEMA:
    case PRO2EL293D28BYJ48:
      (dir == 0 ) ? mystepper->step(1) : mystepper->step(-1);
      delayMicroseconds(this->stepdelay);
#endif
    default:
      // do nothing
      break;
  }
}

int DriverBoard::getstepdelay(void)
{
  return this->stepdelay;
}

void DriverBoard::setstepdelay(int sdelay)
{
  this->stepdelay = sdelay;
}

byte DriverBoard::getmotorspeed(void)
{
  if ( stepdelay == MSFAST )
  {
    return FAST;
  }
  else if ( stepdelay == MSMED )
  {
    return MED;
  }
  else
    return SLOW;
}

void DriverBoard::setmotorspeed(byte spd )
{
  switch ( spd )
  {
    case SLOW:
      this->stepdelay = MSSLOW;
      break;
    case MED:
      this->stepdelay = MSMED;
      break;
    case FAST:
      this->stepdelay = MSFAST;
      break;
    default:
      this->stepdelay = MSFAST;
      break;
  }
}
