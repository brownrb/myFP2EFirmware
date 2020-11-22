// ---------------------------------------------------------------------------
// myFP2ESP GENERAL DEFINITIONS
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// COPYRIGHT
// ---------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2020. All Rights Reserved.
// (c) Copyright Holger M, 2019-2020. All Rights Reserved.
// ---------------------------------------------------------------------------

#ifndef generalDefinitions_h
#define generalDefinitions_h

#include <Arduino.h>

// ---------------------------------------------------------------------------
// 1: GENERAL DEFINES -- DO NOT CHANGE
// ---------------------------------------------------------------------------

enum oled_state { oled_off, oled_on };
enum connection_status { disconnected, connected };
//  StateMachine definition
enum StateMachineStates { State_Idle, State_InitMove, State_Moving, State_DelayAfterMove, State_FinishedMove, State_SetHomePosition };

#define DEFAULTPOSITION       5000L
#define DEFAULTMAXSTEPS       80000L

#define DEFAULTSAVETIME       30000         // default time to wait before saving data to FS

#define ALPACAPORT            4040          // ASCOM Remote port
#define WEBSERVERPORT         80            // Web server port
#define MSSERVERPORT          6060          // Management interface - cannot be changed
#define MDNSSERVERPORT        7070          // mDNS service
#define WS_REFRESHRATE        60            // web server page refresh time 60s
#define MINREFRESHPAGERATE    10            // 10s - too low and the overhead becomes too much for the controller
#define MAXREFRESHPAGERATE    900           // 15m
#define DUCKDNS_REFRESHRATE   60000         // duck dns, check ip address every 60s for an update
#define RUNNING               true          // service state running
#define STOPPED               false         // service state stopped
#define MSREBOOTPAGEDELAY     20000         // management service reboot page, time (s) between next page refresh
#define REBOOTDELAY           2000          // When rebooting controller, delay (2s) from msg to actual reboot
#define MotorReleaseDelay     120*1000      // motor release power after 120s

#define OLED_ADDR             0x3C          // some OLED displays maybe at 0x3F, use I2Cscanner to find correct address
#define SCREEN_WIDTH          128           // OLED display width, in pixels
#define SCREEN_HEIGHT         64            // OLED display height, in pixels
#define MINOLEDPAGETIME       2000L
#define MAXOLEDPAGETIME       10000L
#define OLEDPGOPTIONALL       "111"         // oled page enable, ALL pages

#define MOTORPULSETIME        2             // DO NOT CHANGE
#define SERVERPORT            2020          // TCPIP port for myFP2ESP
#define TEMPREFRESHRATE       3000L         // refresh rate between temperature conversions unless an update is requested via serial command
#define SERIALPORTSPEED       115200        // 9600, 14400, 19200, 28800, 38400, 57600, 115200
#define ESPDATA               0             // command has come from tcp/ip
#define BTDATA                1             // command has come from bluetooth
#define SERIALDATA            2             // command has come from serial port
#define QUEUELENGTH           20            // number of commands that can be saved in the serial queue

#define DEFAULTSTEPSIZE       50.0          // This is the default setting for the step size in microns
#define MINIMUMSTEPSIZE       0.0
#define MAXIMUMSTEPSIZE       100.0
#define TEMPPRECISION         10            // Set the default DS18B20 precision to 0.25 of a degree 9=0.5, 10=0.25, 11=0.125, 12=0.0625
#define LCDUPDATEONMOVE       15            // defines how many steps before refreshing position when moving if lcdupdateonmove is 1
#define FOCUSERUPPERLIMIT     2000000000L   // arbitary focuser limit up to 2000000000
#define FOCUSERLOWERLIMIT     1024L         // lowest value that maxsteps can be
#define LCDPAGETIMEMIN        2             // 2s minimum lcd page display time
#define LCDPAGETIMEMAX        10            // 10s maximum lcd page display time
#define HOMESTEPS             200           // Prevent searching for home position switch never returning, this should be > than # of steps between closed and open
#define HPSWOPEN              0             // hpsw states refelect status of switch
#define HPSWCLOSED            1

#define MAXWEBPAGESIZE        3400
#define MAXASCOMPAGESIZE      2000
#define MAXMANAGEMENTPAGESIZE 2600

#ifdef HOMEPOSITIONSWITCH
#define HPS_alert             !((bool)digitalRead(HPSWPIN))
#else
#define HPS_alert             false
#endif

#ifndef SLOW
#define SLOW                  0             // motorspeeds
#endif
#ifndef MED
#define MED                   1
#endif
#ifndef FAST
#define FAST                  2
#endif

// ---------------------------------------------------------------------------
// 2: DO NOT CHANGE
// ---------------------------------------------------------------------------

#define moving_in             false
#define moving_out            !moving_in
#define moving_main           moving_in               

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

#define EOFSTR                '#'
#define STARTCMDSTR           ':'

extern const char* programVersion;
extern const char* ProgramAuthor;

extern const char* STATEMOVINGSTR;
extern const char* STATEAPPLYBACKLASH;
extern const char* STATESETHOMEPOSITION;
extern const char* STATEFINDHOMEPOSITION;
extern const char* STATEDELAYAFTERMOVE;
extern const char* STATEFINISHEDMOVE;
extern const char* STATEIDLE;
extern const char* STATEINITMOVE;
extern const char* PORTSTR;
extern const char* SENDSTR;
extern const char* SERIALSTARTSTR;
extern const char* DEBUGONSTR;
extern const char* BLUETOOTHSTARTSTR;
extern const char* ATTEMPTCONNSTR;
extern const char* APSTARTFAILSTR;
extern const char* SERVERREADYSTR;
extern const char* STARTSTR;
extern const char* ENDSTR;
extern const char* PROGRESSSTR;
extern const char* ERRORSTR;
extern const char* READYSTR;
extern const char* SETUPDRVBRDSTR;
extern const char* DRVBRDDONESTR;
extern const char* CHECKCPWRSTR;
extern const char* CPWRRELEASEDSTR;
extern const char* STARTAPSTR;
extern const char* STARTSMSTR;
extern const char* SETSTATICIPSTR;
extern const char* ATTEMPTSSTR;
extern const char* STARTTCPSERVERSTR;
extern const char* TCPSERVERSTARTEDSTR;
extern const char* GETLOCALIPSTR;
extern const char* SETUPDUCKDNSSTR;
extern const char* SETUPENDSTR;
extern const char* STARTOTASERVICESTR;
extern const char* SSIDSTR;
extern const char* IPADDRESSSTR;
extern const char* WIFIRESTARTSTR;
extern const char* WIFIBEGINSTATUSSTR;
extern const char* CHECKFORTPROBESTR;
extern const char* ACCESSPOINTSTR;
extern const char* STATIONMODESTR;
extern const char* CONFIGSAVEDSTR;
extern const char* RELEASEMOTORSTR;
extern const char* LOOPSTARTSTR;
extern const char* LOOPENDSTR;
extern const char* TCPCLIENTCONNECTSTR;
extern const char* TCPCLIENTDISCONNECTSTR;
extern const char* APCONNECTFAILSTR;
extern const char* CONNECTEDSTR;
extern const char* I2CDEVICENOTFOUNDSTR;
extern const char* ASCOMREMOTESTR;
extern const char* REBOOTWSSTR;
extern const char* REBOOTTCPSTR;
extern const char* REBOOTCNTLRSTR;
extern const char* REBOOTASCOMSTR;
extern const char* ASCOMSERVERNOTDEFINEDSTR;
extern const char* WEBSERVERNOTDEFINEDSTR;
extern const char* SETPGOPTIONSTR;
extern const char* SETPGTIMESTR;

extern const char* MANAGEMENTNOTFOUNDSTR;
extern const char* WEBSERVERNOTFOUNDSTR;
extern const char* ASCOMSERVERNOTFOUNDSTR;

extern const char* WRITEFILEFAILSTR;
extern const char* WRITEFILESUCCESSSTR;
extern const char* CREATEFILEFAILSTR;
extern const char* CHECKWIFICONFIGFILESTR;
extern const char* DESERIALIZEERRORSTR;
extern const char* SERVERNOTRUNNINGSTR;

extern const char* HPCLOSEDFPNOT0STR;
extern const char* HPCLOSEDFP0STR;
extern const char* HPMOVETILLOPENSTR;
extern const char* HPMOVEOUTERRORSTR;
extern const char* HPMOVEOUTSTEPSSTR;
extern const char* HPMOVEOUTFINISHEDSTR;

// temperature probe messages
extern const char* TPROBESTR;
extern const char* TPROBENOTFOUNDSTR;
extern const char* GETTEMPPROBESSTR;
extern const char* SETTPROBERESSTR;

// web page color messages
extern const char* BACKCOLORINVALIDSTR;
extern const char* NEWTITLECOLORSTR;
extern const char* TITLECOLORINVALIDSTR;
extern const char* NEWHEADERCOLORSTR;
extern const char* HEADERCOLORINVALIDSTR;
extern const char* NEWTEXTCOLORSTR;
extern const char* TEXTCOLORINVALIDSTR;

// oled messages
extern const char* CURRENTPOSSTR;
extern const char* TARGETPOSSTR;
extern const char* COILPWRSTR;
extern const char* REVDIRSTR;
extern const char* STEPMODESTR;
extern const char* TEMPSTR;
extern const char* MOTORSPEEDSTR;
extern const char* MAXSTEPSSTR;
extern const char* TCOMPSTEPSSTR;
extern const char* TCOMPSTATESTR;
extern const char* TCOMPDIRSTR;
extern const char* BACKLASHINSTR;
extern const char* BACKLASHOUTSTR;
extern const char* BACKLASHINSTEPSSTR;
extern const char* BACKLASHOUTSTEPSSTR;
extern const char* BLUETOOTHSTR;
extern const char* LOCALSERIALSTR;
extern const char* FSFILENOTFOUNDSTR;

// joystick messages
extern const char* UPDATEJOYSTICKSTR;
extern const char* JOYSTICKVALSTR;
extern const char* JOYSTICKXINVALSTR;
extern const char* JOYSTICKSPEEDSTR;
extern const char* JOYSTICKXOUTVALSTR;

// defines for ASCOMSERVER, MDNSSERVER, WEBSERVER
extern const char* WEBSERVERSTR;

#define NORMALWEBPAGE             200
#define FILEUPLOADSUCCESS         300
#define BADREQUESTWEBPAGE         400
#define NOTFOUNDWEBPAGE           404
#define INTERNALSERVERERROR       500

extern const char* TEXTPAGETYPE;
extern const char* PLAINTEXTPAGETYPE;
extern const char* JSONTEXTPAGETYPE;
extern const char* JSONPAGETYPE;
extern const char* FILENOTFOUNDSTR;
extern const char* FILEFOUNDSTR;
extern const char* CANNOTCREATEFILESTR;

extern const char* FSNOTSTARTEDSTR;
extern const char* BUILDDEFAULTPAGESTR;
extern const char* SPIFFSFILENOTFOUNDSTR;
extern const char* READPAGESTR;
extern const char* PROCESSPAGESTARTSTR;
extern const char* PROCESSPAGEENDSTR;
extern const char* STARTASCOMSERVERSTR;
extern const char* STOPASCOMSERVERSTR;
extern const char* STARTWEBSERVERSTR;
extern const char* STOPWEBSERVERSTR;
extern const char* STOPMDNSSERVERSTR;

extern const char* SERVERSTATESTOPSTR;
extern const char* SERVERSTATESTARTSTR;
extern const char* SERVERSTATERUNSTR;
extern const char* SENDPAGESTR;
extern const char* ENABLEDSTR;
extern const char* NOTENABLEDSTR;

extern const char* MANAGEMENTNOTFOUNDSTR;
extern const char* WEBSERVERNOTFOUNDSTR;
extern const char* ASCOMSERVERNOTFOUNDSTR;

extern const char* MDNSSTARTFAILSTR;
extern const char* MDNSSTARTEDSTR;

extern const char* CREBOOTSTR;

extern const char* ENABLEBKINSTR;
extern const char* DISABLEBKINSTR;
extern const char* ENABLEBKOUTSTR;
extern const char* DISABLEBKOUTSTR;
extern const char* BLINSTEPSTR;
extern const char* BLOUTSTEPSTR;

extern const char* ENABLELEDSTR;
extern const char* DISABLELEDSTR;
extern const char* ENABLETEMPSTR;
extern const char* DISABLETEMPSTR;
extern const char* STOPTSSTR;
extern const char* STARTTSSTR;
extern const char* STOPWSSTR;
extern const char* STARTWSSTR;
extern const char* STOPASSTR;
extern const char* STARTASSTR;
extern const char* DISPLAYCSTR;
extern const char* DISPLAYFSTR;

extern const char* MDNSTOPSTR;
extern const char* MDNSSTARTSTR;
extern const char* DISPLAYONSTR;
extern const char* DISPLAYOFFSTR;
extern const char* STARTSCREENONSTR;
extern const char* STARTSCREENOFFSTR;
extern const char* STARTHPSWMONSTR;
extern const char* STARTHPSWMOFFSTR;
extern const char* STARTFMDLONSTR;
extern const char* STARTFMDLOFFSTR;

extern const char* NOTDEFINEDSTR;

// ---------------------------------------------------------------------------
// 2. TRACING -- DO NOT CHANGE
// ---------------------------------------------------------------------------
// ArduinoTrace - github.com/bblanchon/ArduinoTrace
// Copyright Benoit Blanchon 2018-2019
// Provide a trace fucntion, printing file, line number, function and parameters
// DEBUG needs to be defined to get output to Serial Port
// If DEBUG is not defined nothing happens
#define TRACE() \
DebugPrint(__FILE__); \
DebugPrint(':'); \
DebugPrint(__LINE__); \
DebugPrint(": "); \
DebugPrintln(__PRETTY_FUNCTION__);

// ---------------------------------------------------------------------------
// 3. DEBUGGING -- DO NOT CHANGE
// ---------------------------------------------------------------------------
#define DEBUG 1

#ifdef  DEBUG                                         // Macros are usually in all capital letters.
#define DebugPrint(...) Serial.print(__VA_ARGS__)     // DPRINT is a macro, debug print
#define DebugPrintln(...) Serial.println(__VA_ARGS__) // DPRINTLN is a macro, debug print with new line
#else
#define DebugPrint(...)                               // now defines a blank line
#define DebugPrintln(...)                             // now defines a blank line
#endif

// ---------------------------------------------------------------------------
// 4. HEAP DEBUGGING - DO NOT CHANGE / DO NOT ENABLE
// ---------------------------------------------------------------------------
//#define HEAPDEBUG     1

#ifdef  HEAPDEBUG   
#define HDebugPrint(...) Serial.print(__VA_ARGS__)      // HDebugPrint is a macro, serial print
#define HDebugPrintln(...) Serial.println(__VA_ARGS__)  // HDebugPrintln is a macro, serial print with new line
#define HDebugPrintf(...) Serial.printf(__VA_ARGS__)    // HDebugPrintf is a macro, serial printf
#else
#define HDebugPrint(...)                                // now defines a blank line
#define HDebugPrintln(...)                              // now defines a blank line
#define HDebugPrintf(...)
#endif

// ---------------------------------------------------------------------------
// 5. TIMING TESTS - DO NOT CHANGE / DO NOT ENABLE
// ---------------------------------------------------------------------------
#define TIMEDTESTS 1

#ifdef TIMEDTESTS

#define TIMESETUP                   1
//#define TIMELOOP                    1
#define TIMEWSROOTSEND              1
#define TIMEWSROOTHANDLE            1
#define TIMEWSROOTBUILD             1
#define TIMEWSMOVEHANDLE            1
#define TIMEWSBUILDMOVE             1
#define TIMEWSSENDMOVE              1
#define TIMEWSSENDPRESETS           1
#define TIMEWSHANDLEPRESETS         1
#define TIMEWSBUILDPRESETS          1

#define TIMEMSSENDPG1               1
#define TIMEMSSENDPG2               1
#define TIMEMSSENDPG3               1
#define TIMEMSSENDPG4               1
#define TIMEMSSENDPG5               1
#define TIMEMSBUILDPG1              1
#define TIMEMSBUILDPG2              1
#define TIMEMSBUILDPG3              1
#define TIMEMSBUILDPG4              1
#define TIMEMSBUILDPG5              1
#define TIMEMSHANDLEPG1             1
#define TIMEMSHANDLEPG2             1
#define TIMEMSHANDLEPG3             1
#define TIMEMSHANDLEPG4             1
#define TIMEMSHANDLEPG5             1

#define TIMEASCOMROOT               1
#define TIMEASCOMBUILDSETUP         1
#define TIMEASCOMHANDLESETUP        1
#define TIMEASCOMHANDLEFOCUSERSETUP 1
#define TIMEASCOMHANDLEAPIVER       1
#define TIMEASCOMHANDLEAPIDES       1
#define TIMEASCOMHANDLEAPICON       1
#endif

#endif // generalDefinitions.h
