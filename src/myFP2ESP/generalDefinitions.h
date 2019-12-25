// ----------------------------------------------------------------------------------------------
// myFP2ESP GENERAL DEFINITIONS
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2019. All Rights Reserved.
// (c) Copyright Holger M, 2019. All Rights Reserved. 
// ----------------------------------------------------------------------------------------------

#include <Arduino.h>

#ifndef generalDefinitions_h
#define generalDefinitions_h

// ----------------------------------------------------------------------------------------------
// 1: GENERAL DEFINES -- DO NOT CHANGE
// ----------------------------------------------------------------------------------------------

#define ALPACAPORT            4040          // ASCOM Remote server port
#define WEBSERVERPORT         80            // Web server port
#define MSSERVERPORT          6060          // Management interface
#define MDNSSERVERPORT        7070          // mDNS service
#define WS_REFRESHRATE        30            // web server page refresh time 30s
#define DUCKDNS_REFREHRATE    60000         // duck dns, check ip address every 60s for an update
#define RUNNING               true
#define STOPPED               false
#define MotorReleaseDelay     120*1000      // motor release power after 120s

#define OLED_ADDR             0x3C          // some OLED displays maybe at 0x3F, use I2Cscanner to find correct address
#define SCREEN_WIDTH          128           // OLED display width, in pixels
#define SCREEN_HEIGHT         64            // OLED display height, in pixels

#define MOTORPULSETIME        2             // DO NOT CHANGE
#define SERVERPORT            2020          // TCPIP port for myFP2ESP
#define TEMPREFRESHRATE       3000L         // refresh rate between temperature conversions unless an update is requested via serial command
#define SERIALPORTSPEED       115200        // 9600, 14400, 19200, 28800, 38400, 57600, 115200
#define ESPDATA               0
#define BTDATA                1
#define SERIALDATA            2
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
#define HOMESTEPS             200           // Prevent searching for home position switch never returning, this should be > than # of steps between closed and open
#define HPSWOPEN              0             // hpsw states refelect status of switch
#define HPSWCLOSED            1

// You can set the speed of the motor when performing backlash to SLOW, MED or FAST
#define BACKLASHSPEED         SLOW

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

#define STATEMOVINGSTR        ">Moving"
#define STATEAPPLYBACKLASH    ">ApplyBacklash"
#define STATESETHOMEPOSITION  ">SetHomePosition"
#define STATEFINDHOMEPOSITION ">FindHomePosition#"
#define STATEDELAYAFTERMOVE   ">DelayAfterMove"
#define STATEFINISHEDMOVE     ">FinishedMove"
#define STATEIDLE             ">Idle"
#define STATEINITMOVE         ">InitMove"
#define EOFSTR                '#'
#define STARTSTR              ':'
#define PORTSTR               "Port= "
#define sendstr               "Send= "
#define serialstartstr        "Serial started"
#define debugonstr            "Debug on"
#define bluetoothstartstr     "Bluetooth started"
#define tprobestr             "Tsensors= "
#define attemptconnstr        "Attempt connection to= "
#define wifistartfailstr      "WiFi start failed"

#define serverreadystr        "Server Ready"
#define startstr              "Start"
#define endstr                "End"
#define progressstr           "Progress: "
#define errorstr              "Error= "
#define readystr              "Ready"
#define coilpwrstr            "Coil power  = "
#define revdirstr             "Reverse Dir = "
#define tcompstepsstr         "TComp Steps = "
#define tcompstatestr         "TComp State = "
#define tcompdirstr           "TComp Dir   = "
#define backlashinstr         "Backlash In = "
#define backlashoutstr        "Backlash Out ="
#define backlashinstepsstr    "Backlash In#= "
#define backlashoutstepsstr   "Backlash Ou#= "
#define bluetoothstr          "Bluetooth Mode"
#define localserialstr        "Local Serial Mode"
#define currentposstr         "Current Pos = "
#define targetposstr          "Target Pos  = "
#define tempstr               "Temperature = "
#define stepmodestr           "Step Mode   = "
#define motorspeedstr         "Motor Speed = "
#define maxstepsstr           "MaxSteps    = "
#define setupdrvbrdstr        "Setup drvbrd= "
#define drvbrddonestr         "Driver board done"
#define tprobenotfoundstr     "Tprobe not found"
#define startapstr            "Start Access Point"
#define startsmstr            "Start Station mode"
#define setstaticipstr        "Setup Static IP"
#define attemptsstr           "Attempt= "
#define starttcpserverstr     "Start TCP Server"
#define tcpserverstartedstr   "TCP Server started"
#define setupduckdnsstr       "Setup DuckDNS"
#define setupendstr           "Setup end"
#define startotaservicestr    "Start OTA service"
#define ssidstr               "SSID = "
#define ipaddressstr          "IP   = "
#define wifirestartstr        "Restarting"
#define checkfortprobestr     "Check for Tprobe"
#define accesspointstr        "Access point: "
#define stationmodestr        "Station mode: "

// defines for ASCOMSERVER, MDNSSERVER, WEBSERVER
#define ascomremotestr            "ASCOM Remote: "
#define jsonstring                "jsonstr = "
#define webserverstr              "Webserver: "
#define NORMALWEBPAGE             200
#define BADREQUESTWEBPAGE         400
#define NOTFOUNDWEBPAGE           404
#define ASCOMINTERNALSERVERERROR  500
#define TEXTPAGETYPE              "text/html"
#define JSONPAGETYPE              "application/json"

// Controller Features
#define ENABLEDLCD                1
#define ENABLEDOLED               2
#define ENABLEDTEMPPROBE          4
#define ENABLEDHPSW               8
#define ENABLEDBLUETOOTH          16
#define ENABLEDSTEPPERPWR         32
#define ENABLEDPUSHBUTTONS        64
#define ENABLEDROTARYENCODER      128
#define ENABLEDINFRARED           256
#define ENABLEDBACKLASH           512
#define ENABLEDTFT                1024
#define ENABLENOKIA               2048
#define ENABLEKEYPAD              4096
#define ENABLEDINOUTLEDS          8192
#define ENABLEDBUZZER             16384
#define ENABLEDACCESSPOINT        32768
#define ENABLEDSTATIONMODE        65536
#define ENABLEDLOCALSERIAL        131072
#define ENABLEDOTAUPDATES         262144
#define ENABLEDWEBSERVER          524288
#define ENABLEDASCOMREMOTE        1048576
#define ENABLEDSTATICIP           2097152
#define ENABLEDMDNS               4194304
#define ENABLEDJOYSTICK           8388608

// ----------------------------------------------------------------------------------------------
// 2. DEBUGGING                                       // do not change - leave this commented out
// ----------------------------------------------------------------------------------------------
//#define DEBUG     1
//#define LOOPTIMETEST 1

#ifdef  DEBUG                                         //Macros are usually in all capital letters.
#define DebugPrint(...) Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
#define DebugPrintln(...) Serial.println(__VA_ARGS__) //DPRINTLN is a macro, debug print with new line
#else
#define DebugPrint(...)                               //now defines a blank line
#define DebugPrintln(...)                             //now defines a blank line
#endif


#endif // generalDefinitions.h
