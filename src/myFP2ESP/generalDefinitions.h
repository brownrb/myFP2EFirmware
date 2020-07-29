// ----------------------------------------------------------------------------------------------
// myFP2ESP GENERAL DEFINITIONS
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2020. All Rights Reserved.
// (c) Copyright Holger M, 2019-2020. All Rights Reserved.
// ----------------------------------------------------------------------------------------------

#ifndef generalDefinitions_h
#define generalDefinitions_h

#include <Arduino.h>

// ----------------------------------------------------------------------------------------------
// 1: GENERAL DEFINES -- DO NOT CHANGE
// ----------------------------------------------------------------------------------------------

#define ALPACAPORT            4040          // ASCOM Remote server port
#define WEBSERVERPORT         80            // Web server port
#define MSSERVERPORT          6060          // Management interface - cannot be changed
#define MDNSSERVERPORT        7070          // mDNS service
#define WS_REFRESHRATE        60            // web server page refresh time 60s
#define MINREFRESHPAGERATE    10            // 10s - too low and the overhead becomes too much for the controller
#define MAXREFRESHPAGERATE    900           // 15m
#define DUCKDNS_REFREHRATE    60000         // duck dns, check ip address every 60s for an update
#define RUNNING               true          // service state running
#define STOPPED               false         // service state stopped
#define MSREBOOTPAGEDELAY     20000         // management service reboot page, time (s) between next page refresh
#define REBOOTDELAY           2000          // When rebooting controller, delay (2s) from msg to actual reboot
#define MotorReleaseDelay     120*1000      // motor release power after 120s

#define OLED_ADDR             0x3C          // some OLED displays maybe at 0x3F, use I2Cscanner to find correct address
#define SCREEN_WIDTH          128           // OLED display width, in pixels
#define SCREEN_HEIGHT         64            // OLED display height, in pixels

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
#define DEFAULTSAVETIME       30000         // default time to wait before saving data to SPIFFS
//#define HOMESTEPS             200           // Prevent searching for home position switch never returning, this should be > than # of steps between closed and open

#ifdef HOMEPOSITIONSWITCH
#define HPS_alert             !((bool)digitalRead(HPSWPIN))
#else
#define HPS_alert             false
#endif

#define SLOW                  0             // motorspeeds
#define MED                   1
#define FAST                  2

// You can set the speed of the motor when performing backlash to SLOW, MED or FAST
#define BACKLASHSPEED         SLOW

// ----------------------------------------------------------------------------------------------
// 2: DO NOT CHANGE
// ----------------------------------------------------------------------------------------------

#define moving_in             false
#define moving_out            !moving_in
#define moving_main           moving_in               

#define STEP1                 1             // stepmodes
#define STEP2                 2
#define STEP4                 4
#define STEP8                 8
#define STEP16                16
#define STEP32                32

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

extern const char* MANAGEMENTURLNOTFOUNDSTR;
extern const char* WEBSERVERURLNOTFOUNDSTR;
extern const char* ASCOMSERVERURLNOTFOUNDSTR;

extern const char* WRITEFILEFAILSTR;
extern const char* WRITEFILESUCCESSSTR;
extern const char* CREATEFILEFAILSTR;
extern const char* CHECKWIFICONFIGFILESTR;
extern const char* DESERIALIZEERRORSTR;
extern const char* SERVERNOTRUNNINGSTR;

extern const char* HPCLOSEDFPNOT0STR;
extern const char* HPCLOSEDFP0STR;
extern const char* HPOPENFPNOT0STR;
extern const char* HPMOVETILLCLOSEDSTR;
extern const char* HPMOVEINERRORSTR;
extern const char* HPMOVEINSTEPSSTR;
extern const char* HPMOVEINFINISHEDSTR;
extern const char* HPMOVETILLOPENSTR;
extern const char* HPMOVEOUTERRORSTR;
extern const char* HPMOVEOUTSTEPSSTR;
extern const char* HPMOVEOUTFINISHEDSTR;

// temperature probe messages
extern const char* TPROBESTR;
extern const char* TPROBENOTFOUNDSTR;
extern const char* GETTEMPPROBESSTR;
extern const char* SETTPROBERESSTR;
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
#define UPDATEJOYSTICKSTR         "joystick: update joystick"
#define JOYSTICKVALSTR            "Raw joyval:"
#define JOYSTICKXINVALSTR         "X IN joyval:"
#define JOYSTICKSPEEDSTR          ", Speed:"
#define JOYSTICKXOUTVALSTR        "X OUT joyval:"

// defines for ASCOMSERVER, MDNSSERVER, WEBSERVER
#define ASCOMREMOTESTR            "ASCOM Remote: "
#define WEBSERVERSTR              "Webserver: "
#define NORMALWEBPAGE             200
#define FILEUPLOADSUCCESS         300
#define BADREQUESTWEBPAGE         400
#define NOTFOUNDWEBPAGE           404
#define INTERNALSERVERERROR       500

#define TEXTPAGETYPE              "text/html"
#define PLAINTEXTPAGETYPE         "text/plain"
#define JSONTEXTPAGETYPE          "text/json"
#define JSONPAGETYPE              "application/json"
#define FILENOTFOUNDSTR           "File!found"
#define FILEFOUNDSTR              "File found"
#define CANNOTCREATEFILESTR       "!create file"

#define FSNOTSTARTEDSTR           "!start FS"
#define BUILDDEFAULTPAGESTR       "build default page"
#define SPIFFSFILENOTFOUNDSTR     "file !found"
#define READPAGESTR               "read page into string"
#define PROCESSPAGESTARTSTR       "process page start"
#define PROCESSPAGEENDSTR         "process page done"
#define STARTASCOMSERVERSTR       "start ascom server"
#define STOPASCOMSERVERSTR        "stop ascom server"
#define STARTWEBSERVERSTR         "start web server"
#define STOPWEBSERVERSTR          "stop web server"
#define STOPMDNSSERVERSTR         "stop mdns server"

#define SERVERSTATESTOPSTR        "STOPPED"
#define SERVERSTATESTARTSTR       "STARTED"
#define SERVERSTATERUNSTR         "RUNNING"
#define SENDPAGESTR               "Send page"

#define MDNSSTARTFAILSTR          "Err setting up MDNS responder"
#define MDNSSTARTEDSTR            "mDNS responder started"

#define STOPTSSTR                 "<form action=\"/\" method=\"post\"><input type=\"hidden\" name=\"stopts\" value=\"true\"><input type=\"submit\" value=\"STOP\"></form>"
#define STARTTSSTR                "<form action=\"/\" method=\"post\"><input type=\"hidden\" name=\"startts\" value=\"true\"><input type=\"submit\" value=\"START\"></form>"

#define STOPWSSTR                 "<form action=\"/\" method=\"post\"><input type=\"hidden\" name=\"stopws\" value=\"true\"><input type=\"submit\" value=\"STOP\"></form>"
#define STARTWSSTR                "<form action=\"/\" method=\"post\"><input type=\"hidden\" name=\"startws\" value=\"true\"><input type=\"submit\" value=\"START\"></form>"

#define STOPASSTR                 "<form action=\"/\" method=\"post\"><input type=\"hidden\" name=\"stopas\" value=\"true\"><input type=\"submit\" value=\"STOP\"></form>"
#define STARTASSTR                "<form action=\"/\" method=\"post\"><input type=\"hidden\" name=\"startas\" value=\"true\"><input type=\"submit\" value=\"START\"></form>"
#define MDNSTOPSTR                "<form action=\"/\" method=\"post\"><input type=\"hidden\" name=\"stopmdns\" value=\"true\"><input type=\"submit\" value=\"STOP\"></form>"
#define MDNSSTARTSTR              "<form action=\"/\" method=\"post\"><input type=\"hidden\" name=\"startmdns\" value=\"true\"><input type=\"submit\" value=\"START\"></form>"
#define DISPLAYONSTR              "<form action=\"/\" method=\"post\"><b>Display: </b><input type=\"hidden\" name=\"di\" value=\"doff\" Checked><input type=\"submit\" value=\"Turn Off\"></form>"
#define DISPLAYOFFSTR             "<form action=\"/\" method=\"post\"><b>Display: </b><input type=\"hidden\" name=\"di\" value=\"don\"><input type=\"submit\" value=\"Turn On\"></form>"
#define DISPLAYCSTR               "<form action=\"/\" method=\"post\"><b>Temp Mode: </b><input type=\"hidden\" name=\"tm\" value=\"cel\" Checked><input type=\"submit\" value=\"Enable Celsius\"></form>"
#define DISPLAYFSTR               "<form action=\"/\" method=\"post\"><b>Temp Mode: </b><input type=\"hidden\" name=\"tm\" value=\"fah\"><input type=\"submit\" value=\"Enable Fahrenheit\"></form>"
#define NOTDEFINEDSTR             "!defined in firmware"

// Controller Features
#define ENABLEDLCD                1L
#define ENABLEDOLED               2L
#define ENABLEDTEMPPROBE          4L
#define ENABLEDHPSW               8L
#define ENABLEDBLUETOOTH          16L
#define ENABLEDSTEPPERPWR         32L
#define ENABLEDPUSHBUTTONS        64L
#define ENABLEDROTARYENCODER      128L
#define ENABLEDINFRARED           256L
#define ENABLEDBACKLASH           512L
#define ENABLEDTFT                1024L
#define ENABLENOKIA               2048L
#define ENABLEKEYPAD              4096L
#define ENABLEDINOUTLEDS          8192L
#define ENABLEDBUZZER             16384L
#define ENABLEDACCESSPOINT        32768L
#define ENABLEDSTATIONMODE        65536L
#define ENABLEDLOCALSERIAL        131072L
#define ENABLEDOTAUPDATES         262144L
#define ENABLEDWEBSERVER          524288L
#define ENABLEDASCOMREMOTE        1048576L
#define ENABLEDSTATICIP           2097152L
#define ENABLEDMDNS               4194304L
#define ENABLEDJOYSTICK           8388608L

// ----------------------------------------------------------------------------------------------
// 2. TRACING -- DO NOT CHANGE
// ----------------------------------------------------------------------------------------------
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

// ----------------------------------------------------------------------------------------------
// 3. DEBUGGING -- DO NOT CHANGE
// ----------------------------------------------------------------------------------------------
//#define DEBUG     1
//#define LOOPTIMETEST 1

#ifdef  DEBUG                                         // Macros are usually in all capital letters.
#define DebugPrint(...) Serial.print(__VA_ARGS__)     // DPRINT is a macro, debug print
#define DebugPrintln(...) Serial.println(__VA_ARGS__) // DPRINTLN is a macro, debug print with new line
#else
#define DebugPrint(...)                               // now defines a blank line
#define DebugPrintln(...)                             // now defines a blank line
#endif

#define TIMESETUP               1
#define TIMELOOP                1
#define MSBUILDROOT             1
#define MSROOT                  1
#define MSHANDLEROOT            1
#define WSSENDPRESETS           1
#define WSBUILDPRESETS          1
#define WSHANDLEPRESETS         1
#define WSSENDMOVE              1
#define WSBUILDMOVE             1
#define WSHANDLEMOVE            1
#define WSSENDROOT              1
#define WSBUILDROOT             1
#define WSHANDLEROOT            1
#define ASSETUP1                1
#define ASCOMHANDLESETUP        1
#define ASCOMHANDLEFOCUSERSETUP 1
#define ASCOMHANDLEAPIVER       1
#define ASCOMHANDLEAPIDES       1
#define ASCOMHANDLEAPICON       1


#endif // generalDefinitions.h
