#include <Arduino.h>

#ifndef myBoards_h
#define myBoards_h

#include "chipModels.h"
#include "hardwarePins.h"

#ifndef PRO2EDRV8825
#define PRO2EDRV8825          35         // if using a drv8825 you also need to set DRV8825STEPMODE in myBoards.h
#endif
#ifndef PRO2EULN2003
#define PRO2EULN2003          36
#endif
#ifndef PRO2EL293D
#define PRO2EL293D            37
#endif
#ifndef PRO2EL298N
#define PRO2EL298N            38         // uses PCB layout for ULN2003
#endif
#ifndef PRO2EL293DMINI
#define PRO2EL293DMINI        39         // uses PCB layout for ULN2003
#endif
#ifndef PRO2EL9110S
#define PRO2EL9110S           40         // uses PCB layout for ULN2003
#endif
#ifndef PRO2ESP32DRV8825
#define PRO2ESP32DRV8825      41
#endif
#ifndef PRO2ESP32ULN2003
#define PRO2ESP32ULN2003      42
#endif
#ifndef PRO2ESP32L298N
#define PRO2ESP32L298N        43
#endif
#ifndef PRO2ESP32L293DMINI
#define PRO2ESP32L293DMINI    44         // uses PCB layout for ULN2003
#endif
#ifndef PRO2ESP32L9110S
#define PRO2ESP32L9110S       45         // uses PCB layout for ULN2003
#endif

#ifndef SLOW
#define SLOW                  0           // motorspeeds
#endif
#ifndef MED
#define MED                   1
#endif
#ifndef FAST
#define FAST                  2
#endif

#define STEP1                 1
#define STEP2                 2
#define STEP4                 4
#define STEP8                 8
#define STEP16                16
#define STEP32                32

#define MOTORSPEEDCHANGETHRESHOLD 200
// You can set the speed of the motor when performing backlash to SLOW, MED or FAST
#define BACKLASHSPEED         SLOW

// YOU MUST CHANGE THIS TO MATCH THE STEPMODE SET IN HARDWARE JUMPERS ON THE PCB ESP8266-DRV
#define DRV8825TEPMODE        STEP16    // jumpers MS1/2/3 on the PCB for ESP8266

// stepper motor steps per full revolution using full steps - Non DRV8825 boards require this to be set
#define STEPSPERREVOLUTION 2048          // 28BYJ-48 stepper motor unipolar with ULN2003 board  
// #define STEPSPERREVOLUTION  200        // NEMA17
// #define STEPSPERREVOLUTION  400        // NEMA14HM11-0404S 0.9 motor
// #define STEPSPERREVOLUTION 1028        // 17HS13-0404S-PG5
// #define STEPSPERREVOLUTION 5370        // NEMA17HS13-0404S-PG27
// #define STEPSPERREVOLUTION 1036        // NEMA14HS13-0804S-PG5
// #define STEPSPERREVOLUTION 1036        // NEMA16HS13-0604S-PG5

//------------------------------------------------------------------------------------------------------
// DEFINES FOR ESP8266 BOARDS
//------------------------------------------------------------------------------------------------------
#if( CHIPMODEL == NODEMCUV1 || CHIPMODEL == WEMOS)            // ESP8266
#define DRV8825DIR            13
#define DRV8825STEP           12
#define DRV8825ENABLE         14
#define MOTORPULSETIME        2           // must be al least 2us, time in microseconds
#define DRVFAST               1           // delays for motorspeed in microseconds
#define DRVMED                2000
#define DRVSLOW               10000

#define IN1ULN                13
#define IN2ULN                12
#define IN3ULN                14
#define IN4ULN                2
#define ULNFAST               1                 // delays for motorspeed in microseconds [250-1000]
#define ULNMED                1000
#define ULNSLOW               8000              // do not use values > 15000 due to accuracy issues

// GPIO13-D7-J2-IN1 - IN1
// GPIO12-D6-J2-IN2 - IN2
// GPIO14-D5-J2-IN3 - IN3
// GPIO02-D4-J2-IN4 - IN4
#define IN1L298N              13
#define IN2L298N              12
#define IN3L298N              14
#define IN4L298N              2
#define L298NFAST             1                 // delays for motorspeed in microseconds [250-1000]
#define L298NMED              1000
#define L298NSLOW             8000              // do not use values > 15000 due to accuracy issues

// GPIO13-D7-J2-IN1 - IN1
// GPIO12-D6-J2-IN2 - IN2
// GPIO14-D5-J2-IN3 - IN3
// GPIO02-D4-J2-IN4 - IN4
#define IN1L293DMINI          13
#define IN2L293DMINI          12
#define IN3L293DMINI          14
#define IN4L293DMINI          2
#define L293DMINIFAST         1                 // delays for motorspeed in microseconds [250-1000]
#define L293DMINIMED          1000
#define L293DMINISLOW         8000              // do not use values > 15000 due to accuracy issues

// GPIO13-D7-J2-IN1 - A1A
// GPIO12-D6-J2-IN2 - A1B
// GPIO14-D5-J2-IN3 - B1A
// GPIO02-D4-J2-IN4 - B1B
#define IN1L9110S             13
#define IN2L9110S             12
#define IN3L9110S             14
#define IN4L9110S             2
#define L9110SFAST            1                 // delays for motorspeed in microseconds [250-1000]
#define L9110SMED             1000
#define L9110SSLOW            8000              // do not use values > 15000 due to accuracy issues
#endif

//------------------------------------------------------------------------------------------------------
// DEFINES FOR ESP32 BOARDS
//------------------------------------------------------------------------------------------------------
#if( CHIPMODEL == ESP32WROOM)           // ESP32 WROOM
#define DRV8825DIR            32        // drv8825 driver board
#define DRV8825STEP           33        // drv8825 driver board
#define DRV8825ENABLE         14        // drv8825 driver board
#define DRV8825M3             25        // drv8825 driver board
#define DRV8825M2             26        // drv8825 driver board
#define DRV8825M1             27        // drv8825 driver board
#define MOTORPULSETIME        5         // drv8825 driver board
#define DRVFAST               500       // delays for motorspeed
#define DRVMED                1000
#define DRVSLOW               2000

#define IN1ULN                14
#define IN2ULN                27
#define IN3ULN                26
#define IN4ULN                25
#define ULNFAST               1                 // delays for motorspeed in microseconds [250-1000]
#define ULNMED                1000
#define ULNSLOW               8000              // do not use values > 15000 due to accuracy issues

// 14 - IN1
// 27 - IN2
// 26 - IN3
// 25 - IN4
#define IN1L298N              14
#define IN2L298N              27
#define IN3L298N              26
#define IN4L298N              25
#define L298NFAST             1                 // delays for motorspeed in microseconds [250-1000]
#define L298NMED              1000
#define L298NSLOW             8000              // do not use values > 15000 due to accuracy issues

// 14 - IN1
// 27 - IN2
// 26 - IN3
// 25 - IN4
#define IN1L293DMINI          14
#define IN2L293DMINI          27
#define IN3L293DMINI          26
#define IN4L293DMINI          25
#define L293DMINIFAST         1                 // delays for motorspeed in microseconds [250-1000]
#define L293DMINIMED          1000
#define L293DMINISLOW         8000              // do not use values > 15000 due to accuracy issues

// 14-IN1 - A1A
// 27-IN2 - A1B
// 26-IN3 - B1A
// 25-IN4 - B1B
#define IN1L9110S             14
#define IN2L9110S             27
#define IN3L9110S             26
#define IN4L9110S             25
#define L9110SFAST            1                 // delays for motorspeed in microseconds [250-1000]
#define L9110SMED             1000
#define L9110SSLOW            8000              // do not use values > 15000 due to accuracy issues
#endif

#if (DRVBRD != PRO2EDRV8825 || DRVBRD != PRO2ESP32DRV8825 )
#include "HalfStepperESP32.h"
#endif

class DriverBoard
{
  public:
    DriverBoard(byte, String, byte, byte);                          // driver drv8825 constructor
    DriverBoard(byte, String, byte, byte, byte, byte, byte, byte);  // driver uln2003, L298N, L293DMini

    // getter
    String getname(void);
    byte getmotorspeed(void);
    byte getstepmode(void);

    // setter
    void setstepdelay(int);
    void setstepmode(byte);
    void movemotor(byte);
    void enablemotor(void);
    void releasemotor(void);
    void setmotorspeed(byte);

  private:
#if (DRVBRD != PRO2EDRV8825 || DRVBRD != PRO2ESP32DRV8825 )
    HalfStepper* mystepper;
#endif
    int inputPins[4];                             // The input pin numbers
    byte boardtype;
    byte stepmode;
    int stepdelay;                                // time in milliseconds to wait between pulses when moving
    String boardname;
    int Step;                                     // used to control step count
};
#endif
