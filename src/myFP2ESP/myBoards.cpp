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


// ____ESP8266 Boards
#if DRVBRD == WEMOSDRV8825
  const char* DRVBRD_ID = "WEMOSDRV8825";
#elif  DRVBRD == PRO2EULN2003
  const char* DRVBRD_ID = "PRO2EULN2003";
#elif  DRVBRD == PRO2EDRV8825
  const char* DRVBRD_ID = "PRO2EDRV8825";
#elif  DRVBRD == PRO2EDRV8825BIG
  const char* DRVBRD_ID = "PRO2EDRV8825BIG";
#elif  DRVBRD == PRO2EL293DNEMA
  const char* DRVBRD_ID = "PRO2EL293DNEMA";
#elif  DRVBRD == PRO2EL293D28BYJ48
  const char* DRVBRD_ID = "PRO2EL293D28BYJ48";
#elif  DRVBRD == PRO2EL298N
  const char* DRVBRD_ID = "PRO2EL298N";
#elif  DRVBRD == PRO2EL293DMINI
  const char* DRVBRD_ID = "PRO2EL293DMINI";
#elif  DRVBRD == PRO2EL9110S
  const char* DRVBRD_ID = "PRO2EL9110S";

// ____ESP32 Boards
#elif  DRVBRD == PRO2ESP32DRV8825
  const char* DRVBRD_ID = "PRO2ESP32DRV8825";
  
#elif  DRVBRD == PRO2ESP32ULN2003
  const char* DRVBRD_ID = "PRO2ESP32ULN2003";
#elif  DRVBRD == PRO2ESP32L298N
  const char* DRVBRD_ID = "PRO2ESP32L298N";
#elif  DRVBRD == PRO2ESP32L293DMINI
  const char* DRVBRD_ID = "PRO2ESP32L293DMINI";
#elif  DRVBRD == PRO2ESP32L9110S
  const char* DRVBRD_ID = "PRO2ESP32L9110S";
#elif  DRVBRD == PRO2ESP32R3WEMOS
  const char* DRVBRD_ID = "PRO2ESP32R3WEMOS";
#else
  const char* DRVBRD_ID = "UNKNOWN";
#endif


#define LED 2

// Stop button is attached to PIN 0 (IO0)
#define BTN_STOP_ALARM    0


bool flag = false;
bool toggle = false;

hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile unsigned long stepcount = 0;


//______________ code section _____

void IRAM_ATTR onTimer()
{
  // Increment the counter and set the time of ISR
//  portENTER_CRITICAL_ISR(&timerMux);

  if (stepcount) 
  {
    digitalWrite(STEPPIN, 0);
    toggle = (toggle == true) ? false : true;
    digitalWrite(LED, toggle);  
    stepcount--;
    flag = true;
    digitalWrite(STEPPIN, 1);    
  }
  else
  {
    if (flag == true)
    {
      digitalWrite(LED, 0);       // debug LED off
      flag = false;
      xSemaphoreGiveFromISR(timerSemaphore, NULL);    // fire up semaphore
    }
  }
}

DriverBoard::DriverBoard(byte brdtype) : boardtype(brdtype)
{
  do  { 
#if DRVBRD == WEMOSDRV8825    || DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2EDRV8825BIG || DRVBRD == PRO2ESP32R3WEMOS 
      pinMode(ENABLEPIN, OUTPUT);
      pinMode(DIRPIN, OUTPUT);
      pinMode(STEPPIN, OUTPUT);
      digitalWrite(ENABLEPIN, 1);
      DriverBoard::setstepmode(DRV8825TEPMODE);
      break;

#elif DRVBRD == PRO2ESP32DRV8825 
      pinMode(ENABLEPIN, OUTPUT);
      pinMode(DIRPIN, OUTPUT);
      pinMode(STEPPIN, OUTPUT);
      digitalWrite(ENABLEPIN, 1);
      pinMode(MS1, OUTPUT);
      pinMode(MS2, OUTPUT);
      pinMode(MS3, OUTPUT);

      pinMode(LED, OUTPUT);      

// Create semaphore to inform us when the timer has fired
      timerSemaphore = xSemaphoreCreateBinary();

      break;

#elif (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2ESP32ULN2003)
      // IN1ULN, IN3ULN, IN4ULN, IN2ULN
      mystepper = new HalfStepper(STEPSPERREVOLUTION, IN1, IN3, IN4, IN2);
      mystepper->setSpeed(5);      

#elif (DRVBRD == PRO2EL298N || DRVBRD == PRO2ESP32L298N)
      // IN1L298N, IN2L298N, IN3L298N, IN4L298N
      mystepper = new HalfStepper(STEPSPERREVOLUTION, IN1, IN2, IN3, IN4);
      mystepper->setSpeed(20); 

#elif (DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2ESP32L293DMINI)
      // IN1L293DMINI, IN2L293DMINI, IN3L293DMINI, IN4L293DMINI
      mystepper = new HalfStepper(STEPSPERREVOLUTION, IN1, IN2, IN3, IN4);
      mystepper->setSpeed(5);

#elif (DRVBRD == PRO2EL9110S || DRVBRD == PRO2ESP32L9110S)
      // IN1L9110S, IN2L9110S, IN3L9110S, IN4L9110S
      mystepper = new HalfStepper(STEPSPERREVOLUTION, IN1, IN2, IN3, IN4);
      mystepper->setSpeed(5); 
#endif

#if (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N || DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2EL9110S \
  || DRVBRD == PRO2ESP32ULN2003 || DRVBRD == PRO2ESP32L298N || DRVBRD == PRO2ESP32L2933DMINI \
  || DRVBRD == PRO2ESP32L9110S)

      // Move the init of inputPins here before init of myStepper to prevent stepper motor jerk
      this->inputPins[0] = IN1;
      this->inputPins[1] = IN2;
      this->inputPins[2] = IN3;
      this->inputPins[3] = IN4;
      for (int i = 0; i < 4; i++) {
        pinMode(this->inputPins[i], OUTPUT);
      }
      this->stepdelay = MSFAST;
      setmotorspeed(FAST);
      break;     
#endif


      // Move the init of inputPins here before init of myStepper to prevent stepper motor jerk
#if (DRVBRD == PRO2EL293DNEMA )
      mystepper = new Stepper(STEPSPERREVOLUTION, IN2, IN3, IN1, IN4);
      // constructor was IN2, IN3, IN1, IN4 for NEMA14
      this->inputPins[0] = IN1;
      this->inputPins[1] = IN2;
      this->inputPins[2] = IN3;
      this->inputPins[3] = IN4;
      for (int i = 0; i < 4; i++)
      {
        pinMode(this->inputPins[i, OUTPUT);
      }
      mystepper->setSpeed(SPEEDNEMA);

#elif (DRVBRD == PRO2EL293D28BYJ48 )
      mystepper = new Stepper(STEPSPERREVOLUTION, IN1, IN3, IN2, IN4);
      // constructor was IN1, IN3, IN2, IN4 for 28BYJ48
      this->inputPins[0] = IN1;
      this->inputPins[1] = IN3;
      this->inputPins[2] = IN2;
      this->inputPins[3] = IN4;
      for (int i = 0; i < 4; i++)
      {
        pinMode(this->inputPins[i], OUTPUT);
      }
      mystepper->setSpeed(SPEEDBIPOLAR);
#endif
#if (DRVBRD == PRO2EL293DNEMA || DRVBRD == PRO2EL293D28BYJ48 )
      setstepmode(STEP1);
      setmotorspeed(FAST);
#endif
  } while(0);
}

byte DriverBoard::getstepmode(void)
{
  return this->stepmode;
}

void DriverBoard::setstepmode(byte smode)
{
  do {
#if (DRVBRD == WEMOSDRV8825    || DRVBRD == PRO2EDRV8825 \
  || DRVBRD == PRO2EDRV8825BIG || DRVBRD == PRO2ESP32R3WEMOS )
      // for DRV8825 stepmode is set in hardware jumpers
      // cannot set by software
      this->stepmode = DRV8825TEPMODE;       // defined at beginning of myBoards.h
      break;

#elif (DRVBRD == PRO2ESP32DRV8825 )

      switch (smode)
      {
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
        case STEP1:          
        default:
          digitalWrite(MS1, 0);
          digitalWrite(MS2, 0);
          digitalWrite(MS3, 0);
          smode = STEP1;
          break;
      }
      this->stepmode = smode;
      break;

#elif (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N || DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2EL9110S \
 || DRVBRD == PRO2EESP32ULN2003 || DRVBRD == PRO2EESP32L298N || DRVBRD == PRO2ESP32L293DMINI \
 || DRVBRD == PRO2ESP32L9110S )
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
           this->stepmode = STEP1;
          break;
      }
      break;

#elif (DRVBRD == PRO2EL293DNEMA || DRVBRD == PRO2EL293D28BYJ48 )
      this->stepmode = STEP1;
      break;
#endif
      this->stepmode = STEP1;
  }  while(0);    
}

void DriverBoard::enablemotor(void)
{
#if (DRVBRD == WEMOSDRV8825    || DRVBRD == PRO2EDRV8825 \
  || DRVBRD == PRO2EDRV8825BIG || DRVBRD == PRO2ESP32DRV8825 \
  || DRVBRD == PRO2ESP32R3WEMOS )
  digitalWrite(ENABLEPIN, 0);
  delay(1);                     // boards require 1ms before stepping can occur      
#endif
}

void DriverBoard::releasemotor(void)
{
#if (DRVBRD == WEMOSDRV8825    || DRVBRD == PRO2EDRV8825 \
  || DRVBRD == PRO2EDRV8825BIG || DRVBRD == PRO2ESP32DRV8825 \
  || DRVBRD == PRO2ESP32R3WEMOS )
      digitalWrite(ENABLEPIN, 1);

#elif (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N || DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2EL9110S \
 || DRVBRD == PRO2EESP32ULN2003 || DRVBRD == PRO2EESP32L298N || DRVBRD == PRO2ESP32L293DMINI \
  || DRVBRD == PRO2ESP32L9110S || DRVBRD == PRO2EL293DNEMA || DRVBRD == PRO2EL293D28BYJ48)
      digitalWrite(this->inputPins[0], 0 );
      digitalWrite(this->inputPins[1], 0 );
      digitalWrite(this->inputPins[2], 0 );
      digitalWrite(this->inputPins[3], 0 );
#endif
}

void DriverBoard::movemotor(byte dir)
{ 
// handling of inout leds when moving done in main code
#if (DRVBRD == WEMOSDRV8825    || DRVBRD == PRO2EDRV8825 \
  || DRVBRD == PRO2EDRV8825BIG || DRVBRD == PRO2ESP32DRV8825 \
  || DRVBRD == PRO2ESP32R3WEMOS )
      digitalWrite(DIRPIN, dir);            // set Direction of travel
      digitalWrite(ENABLEPIN, 0);           // Enable Motor Driver
      digitalWrite(STEPPIN, 1);             // Step pin on
      delayMicroseconds(MOTORPULSETIME);
      digitalWrite(STEPPIN, 0);
      delayMicroseconds(this->stepdelay);   // this controls speed of motor
#endif
#if (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N || DRVBRD == PRO2EL293DMINI \
  || DRVBRD == PRO2EL9110S  || DRVBRD == PRO2EESP32ULN2003 || DRVBRD == PRO2EESP32L298N \
  || DRVBRD == PRO2ESP32L293DMINI || DRVBRD == PRO2ESP32L9110S \
  || DRVBRD == PRO2EL293DNEMA || DRVBRD == PRO2EL293D28BYJ48)
      (dir == 0 ) ? mystepper->step(1) : mystepper->step(-1);
      delayMicroseconds(this->stepdelay);
#endif
}


void DriverBoard::initmove(bool dir, unsigned long steps)
{

  stepcount = steps;

  digitalWrite(DIRPIN, dir);
  digitalWrite(ENABLEPIN, 0);           // Enable Motor Driver
  digitalWrite(STEPPIN, 1);  
  delay(1);

 // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
  // info).
  timer = timerBegin(0, 80, true);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);

  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, 1000, true);  // 50 ms

  // Start an alarm
  timerAlarmEnable(timer);
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

void DriverBoard::setmotorspeed(byte spd)
{
  switch (spd) 
  {
    case SLOW:
      this->stepdelay = MSSLOW;
      break;
    case MED:
      this->stepdelay = MSMED;
      break;
    case FAST:
    default:
      this->stepdelay = MSFAST;
      break;
  }
}