#include <Arduino.h>
#include "generalDefinitions.h"
#include "myBoards.h"


DriverBoard::DriverBoard(byte brdtype) : boardtype(brdtype)
{
  switch ( brdtype )
  {
#if (DRVBRD == WEMOSDRV8825 || DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2ESP32DRV8825)

    case PRO2ESP32DRV8825:
      pinMode(MS1,OUTPUT);        // number of microsteps
      pinMode(MS2,OUTPUT);
      pinMode(MS3,OUTPUT);   
      DriverBoard::setstepmode(DRV8825TEPMODE);
    case WEMOSDRV8825:
    case PRO2EDRV8825:
      pinMode(ENABLEPIN, OUTPUT);
      pinMode(DIRPIN, OUTPUT);
      pinMode(STEPPIN, OUTPUT);
      digitalWrite(ENABLEPIN, 1);
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
      mystepper->setSpeed(5);       // DONE
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
#if (DRVBRD == PRO2EDRV8825)
    case PRO2EDRV8825:
      // for DRV8825 stepmode is set in hardware jumpers
      // cannot set by software
      smode = DRV8825TEPMODE;       // defined at beginning of myBoards.h
      break;
#endif
#if (DRVBRD == PRO2ESP32DRV8825 )
    case PRO2ESP32DRV8825:
      switch (smode)
      {
        case STEP1:
        case STEP2:
        case STEP4:
        case STEP8:
        case STEP16:
        case STEP32:
          break;
        default:
          smode = DRV8825TEPMODE;
          break;
      }

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
      }

      this->stepmode = smode;
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
#if (DRVBRD == WEMOSDRV8825 || DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2ESP32DRV8825 )
    case WEMOSDRV8825:
    case PRO2EDRV8825:
    case PRO2ESP32DRV8825:
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
#if (DRVBRD == WEMOSDRV8825 || DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2ESP32DRV8825)
    case WEMOSDRV8825:
    case PRO2EDRV8825:
    case PRO2ESP32DRV8825:
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
#if (DRVBRD == WEMOSDRV8825 || DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2ESP32DRV8825)
    case WEMOSDRV8825:
    case PRO2EDRV8825:
    case PRO2ESP32DRV8825:
      digitalWrite(DIRPIN, dir);            // set Direction of travel
      digitalWrite(ENABLEPIN, 0);           // Enable Motor Driver
      digitalWrite(STEPPIN, 1);             // Step pin on
      delayMicroseconds(MOTORPULSETIME);
      digitalWrite(STEPPIN, 0);
/*      switch ( this->stepmode )
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
      }*/

      delay(2);     // HM: 23.09.19  Test debug
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
