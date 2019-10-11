#ifndef generalDefinitions_h
#define generalDefinitions_h



// ----------------------------------------------------------------------------------------------
// 1: GENERAL DEFINES -- DO NOT CHANGE
// ----------------------------------------------------------------------------------------------

#define OLED_ADDR             0x3C          // some OLED displays maybe at 0x3F, use I2Cscanner to find the correct address#endif // wemos
#define SCREEN_WIDTH          128           // OLED display width, in pixels
#define SCREEN_HEIGHT         64            // OLED display height, in pixels
#define MOTORPULSETIME        2             // DO NOT CHANGE
#define MOTORRELEASEDELAY     120           // motor release power after 120s

#define SERVERPORT            2020
#define TEMPREFRESHRATE       2000L         // refresh rate between temperature conversions unless an update is requested via serial command
#define SERIALPORTSPEED       115200        // 9600, 14400, 19200, 28800, 38400, 57600, 115200
#define ESPDATA               0
#define BTDATA                1
#define SERIALDATA            2
#define QUEUELENGTH           20            // number of commands that can be saved in the serial queue

#define DEFAULTSTEPSIZE       50.0          // This is the default setting for the step size in microns
#define TEMP_PRECISION        10            // Set the default DS18B20 precision to 0.25 of a degree 9=0.5, 10=0.25, 11=0.125, 12=0.0625
#define LCDUPDATEONMOVE       15            // defines how many steps before refreshing position when moving if lcdupdateonmove is 1
#define FOCUSERUPPERLIMIT     2000000000L   // arbitary focuser limit up to 2000000000
#define FOCUSERLOWERLIMIT     1024L         // lowest value that maxsteps can be
#define LCDPAGETIMEMIN        2             // 2s minimum lcd page display time
#define LCDPAGETIMEMAX        10            // 10s maximum lcd page display time
#define DEFAULTSAVETIME       30000         // default time to wait before saving data to SPIFFS

// You can set the speed of the motor when performing backlash to SLOW, MED or FAST
#define BACKLASHSPEED         SLOW
// Motor speed change threshold not yet implemented
#define MOTORSPEEDCHANGETHRESHOLD 200

#define moving_in             false
#define moving_out            !moving_in

#ifndef SLOW
#define SLOW                  0             // motorspeeds
#endif
#ifndef MED
#define MED                   1
#endif
#ifndef FAST
#define FAST                  2
#endif

#ifndef STEP1
#define STEP1                 1             // stepmodes
#endif
#ifndef STEP2
#define STEP2                 2
#endif
#ifndef STEP4
#define STEP4                 4
#endif
#ifndef STEP8
#define STEP8                 8
#endif
#ifndef STEP16
#define STEP16                16
#endif
#ifndef STEP32
#define STEP32                32
#endif

#define onstr                 "On"
#define offstr                "Off"
#define instr                 "In"
#define outstr                "Out"

// ----------------------------------------------------------------------------------------------
// 2. DEBUGGING                                       // do not change - leave this commented out
// ----------------------------------------------------------------------------------------------
#define DEBUG     1
//#define LOOPTIMETEST 1

#ifdef  DEBUG                                         //Macros are usually in all capital letters.
#define DebugPrint(...) Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
#define DebugPrintln(...) Serial.println(__VA_ARGS__) //DPRINTLN is a macro, debug print with new line
#else
#define DebugPrint(...)                               //now defines a blank line
#define DebugPrintln(...)                             //now defines a blank line
#endif


#endif // generalDefinitions.h
