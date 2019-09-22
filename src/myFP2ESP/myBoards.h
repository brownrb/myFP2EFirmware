#include <Arduino.h>
#include "generalDefinitions.h"
#include "myBoardTypes.h"

#ifndef myBoards_h
#define myBoards_h

// ----------------------------------------------------------------------------------------------
// 1: BOARD DEFINES -- DO NOT CHANGE
// ----------------------------------------------------------------------------------------------
// Uncomment only your board - ONLY ONE BOARD SHOULD BE UNCOMMENTED

// ESP8266 Boards
//#define DRVBRD WEMOSDRV8825
//#define DRVBRD PRO2EULN2003           // DONE
//#define DRVBRD PRO2EDRV8825             // DONE
//#define DRVBRD PRO2EL293DNEMA         // DONE, FULL STEPPING ONLY
//#define DRVBRD PRO2EL293D28BYJ48      // DONE, FULL STEPPING ONLY
//#define DRVBRD PRO2EL298N             // DONE
//#define DRVBRD PRO2EL293DMINI
//#define DRVBRD PRO2EL9110S
// ESP32 Boards
//#define DRVBRD PRO2ESP32DRV8825
#define DRVBRD PRO2ESP32ULN2003
//#define DRVBRD PRO2ESP32L298N
//#define DRVBRD PRO2ESP32L293DMINI
//#define DRVBRD PRO2ESP32L9110S

// THIS MUST MATCH THE STEPMODE SET IN HARDWARE JUMPERS ON THE PCB ESP8266-DRV
#define DRV8825TEPMODE    STEP16        // jumpers MS1/2/3 on the PCB for ESP8266

// stepper motor steps per full revolution using full steps
// WARNING: USE THE CORRECT ONE - IF YOU THEN CHANGE STEPMODE THE STEPS MOVED WILL BE INVALID
//#define STEPSPERREVOLUTION 2048        // 28BYJ-48 stepper motor unipolar with ULN2003 board
#define STEPSPERREVOLUTION  200        // NEMA17 FULL STEPPED
//#define STEPSPERREVOLUTION  400        // NEMA14HM11-0404S 0.9 motor FULL STEPPED
//#define STEPSPERREVOLUTION 1028        // 17HS13-0404S-PG5
//#define STEPSPERREVOLUTION 5370        // NEMA17HS13-0404S-PG27
//#define STEPSPERREVOLUTION 1036        // NEMA14HS13-0804S-PG5
//#define STEPSPERREVOLUTION 1036        // NEMA16HS13-0604S-PG5

// do not change, required for L293D motor shield
#define UNIPOLAR28BYJ48   1
#define BIPOLARNEMA       2
#define SPEEDBIPOLAR      48            // DONE
#define SPEEDNEMA         100           // DONE

// ----------------------------------------------------------------------------------------------
// DEFINITIONS FOR BOARDS: DO NOT CHANGE
// ----------------------------------------------------------------------------------------------

#if ( DRVBRD == PRO2EULN2003   || DRVBRD == PRO2ESP32ULN2003  \
   || DRVBRD == PRO2EL298N     || DRVBRD == PRO2ESP32L298N    \
   || DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2ESP32L293MINI \
   || DRVBRD == PRO2EL9110S    || DRVBRD == PRO2ESPL9110S)
#include <HalfStepperESP32.h>
#endif

#if (DRVBRD == PRO2EL293DNEMA || DRVBRD == PRO2EL293D28BYJ48 )
#include <Stepper.h>                // needed for stepper motor and L293D shield, see https://github.com/adafruit/Adafruit-Motor-Shield-library
#endif

#if (DRVBRD == WEMOSDRV8825 )
#define TEMPPIN       4
#define I2CDATAPIN    2
#define I2CCLKPIN     1
#define DIRPIN        13            // D7 GPIOP13
#define STEPPIN       12            // D6 GPIO12
#define ENABLEPIN     14            // D5 GPIO14
#define MSFAST        1
#define MSMED         2000
#define MSSLOW        10000
#endif
#if (DRVBRD == PRO2EDRV8825 )           // DONE, TESTED WITH NEMA14 STEPPER MOTOR
#define TEMPPIN       10
#define I2CDATAPIN    5
#define I2CCLKPIN     4
#define DIRPIN        13                // D7 GPIOP13
#define STEPPIN       12                // D6 GPIO12
#define ENABLEPIN     14                // D5 GPIO14
#define MSFAST        2000              // These values can be adjusted to alter speed of motor
#define MSMED         7000              // do not use values > 14000
#define MSSLOW        12000             // lower values = motor moves faster, higher values = slower
#endif
#if (DRVBRD == PRO2EULN2003 )           // DONE, TESTED FULL/HALF STEPPING WITH 28BYJ48 STEPPER MOTOR
#define TEMPPIN       10
#define I2CDATAPIN    5
#define I2CCLKPIN     4
#define IN1           13
#define IN2           12
#define IN3           14
#define IN4           2
#define MSFAST        2000
#define MSMED         8000
#define MSSLOW        12000
#endif
#if (DRVBRD == PRO2EL293DNEMA)          // DONE, TESTED FULL STEPPING WITH NEMA17 STEPPER MOTOR
#define TEMPPIN       10                // Temperature somehows does not work now for this shield
#define I2CDATAPIN    12                // GPIO12 is D6
#define I2CCLKPIN     14                // GPIO14 is D5
#define IN1           5                 // PWM_A    
#define IN2           0                 // DIR B
#define IN3           4                 // DIR A
#define IN4           2                 // PWM_B 
#define MSFAST        6000              
#define MSMED         9000
#define MSSLOW        13000
#endif
#if (DRVBRD == PRO2EL293D28BYJ48)       // DONE, TESTED FULL STEPPING WITH 28BYJ48 STEPPER MOTOR
#define TEMPPIN       10
#define I2CDATAPIN    12
#define I2CCLKPIN     14
#define IN1           5                 // PWM_A    
#define IN2           0                 // DIR B
#define IN3           4                 // DIR A
#define IN4           2                 // PWM_B 
#define MSFAST        6000              
#define MSMED         9000
#define MSSLOW        13000
#endif
#if (DRVBRD == PRO2EL298N)      // DONE, TESTED FULL/HALF STEPPING WITH NEMA 17 200 STEPPER MOTOR
#define TEMPPIN       10
#define I2CDATAPIN    5
#define I2CCLKPIN     4
#define IN1           13
#define IN2           12
#define IN3           14
#define IN4           2
#define MSFAST        2000
#define MSMED         8000
#define MSSLOW        12000
#endif
#if (DRVBRD == PRO2EL293DMINI)
#define TEMPPIN       10
#define I2CDATAPIN    5
#define I2CCLKPIN     4
#define IN1           13
#define IN2           12
#define IN3           14
#define IN4           2
#define MSFAST        2000
#define MSMED         8000
#define MSSLOW        12000
#endif
#if (DRVBRD == PRO2EL9110S)
#define TEMPPIN       10
#define I2CDATAPIN    5
#define I2CCLKPIN     4
#define IN1           13
#define IN2           12
#define IN3           14
#define IN4           2
#define MSFAST        2000
#define MSMED         8000
#define MSSLOW        12000
#endif
#if (DRVBRD == PRO2ESP32DRV8825 )
#define TEMPPIN       13
#define I2CDATAPIN    21
#define I2CCLKPIN     22
#define DIRPIN        32
#define STEPPIN       33
#define ENABLEPIN     14
#define INPBPIN       34
#define OUTPBPIN      35
#define INLEDPIN      18
#define OUTLEDPIN     19
#define IRPIN         15
#define MS1           27
#define MS2           26
#define MS3           25
#define MSFAST        500
#define MSMED         1000
#define MSSLOW        2000
#endif
#if (DRVBRD == PRO2ESP32ULN2003 || DRVBRD == PRO2ESP32L298N || DRVBRD == PRO2ESP32L293DMINI || DRVBRD == PRO2ESP32L9110S)
#define TEMPPIN       13
#define I2CDATAPIN    21
#define I2CCLKPIN     22
#define IN1           14
#define IN2           27
#define IN3           26
#define IN4           25
#define INPBPIN       34
#define OUTPBPIN      35
#define INLEDPIN      18
#define OUTLEDPIN     19
#define IRPIN         15
#define MSFAST        1
#define MSMED         1000
#define MSSLOW        8000
#endif

// ----------------------------------------------------------------------------------------------
// DRIVER BOARD CLASS : DO NOT CHANGE
// ----------------------------------------------------------------------------------------------

class DriverBoard
{
  public:
    DriverBoard(byte);

    // getter
    byte getmotorspeed(void);
    byte getstepmode(void);
    String getboardname(void);

    // setter
    void setstepdelay(int);
    void setstepmode(byte);
    void movemotor(byte);
    void enablemotor(void);
    void releasemotor(void);
    void setmotorspeed(byte);

  private:
#if ( DRVBRD == PRO2EULN2003   || DRVBRD == PRO2ESP32ULN2003  \
   || DRVBRD == PRO2EL298N     || DRVBRD == PRO2ESP32L298N    \
   || DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2ESP32L293MINI \
   || DRVBRD == PRO2EL9110S    || DRVBRD == PRO2ESPL9110S)
    HalfStepper* mystepper;
#endif
#if (DRVBRD == PRO2EL293DNEMA || DRVBRD == PRO2EL293D28BYJ48 )
    Stepper* mystepper;
#endif
    int inputPins[4];                             // The input pin numbers
    byte boardtype;
    byte stepmode;
    int stepdelay;                                // time in milliseconds to wait between pulses when moving
    int Step;                                     // used to control step count

    // Step sequences NO LONGER USED
    int ulnfull[4] = {B01100, B00110, B00011, B01001};                                    // confirmed
    int ulnhalf[8] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001};    // confirmed

    int l298nfull[4] = {B01100, B00110, B00011, B01001};                                   // confirmed
    int l298nhalf[8] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001};   // confirmed

    int l293dminifull[4] = {B01100, B00110, B00011, B01001};                                  // to test
    int l293dminihalf[8] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001};  // to test

    // A-1A, A1B, B-1A, B-1B
    int l9110sfull[4] = {B00101, B00110, B01010, B01001};                                 // to test
    int l9110shalf[8] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001}; // to test
};
#endif
