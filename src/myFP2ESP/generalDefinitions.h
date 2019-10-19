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
#define ALPACAPORT            80            // ASCOM Remote server port
#define WEBSERVERPORT         80            // Web server port
#define TEMPREFRESHRATE       2000L         // refresh rate between temperature conversions unless an update is requested via serial command
#define SERIALPORTSPEED       115200        // 9600, 14400, 19200, 28800, 38400, 57600, 115200
#define ESPDATA               0
#define BTDATA                1
#define SERIALDATA            2
#define QUEUELENGTH           20            // number of commands that can be saved in the serial queue

#define DEFAULTSTEPSIZE       50.0          // This is the default setting for the step size in microns
#define MINIMUMSTEPSIZE       0.0
#define MAXIMUMSTEPSIZE       100.0
#define TEMP_PRECISION        10            // Set the default DS18B20 precision to 0.25 of a degree 9=0.5, 10=0.25, 11=0.125, 12=0.0625
#define LCDUPDATEONMOVE       15            // defines how many steps before refreshing position when moving if lcdupdateonmove is 1
#define FOCUSERUPPERLIMIT     2000000000L   // arbitary focuser limit up to 2000000000
#define FOCUSERLOWERLIMIT     1024L         // lowest value that maxsteps can be
#define LCDPAGETIMEMIN        2             // 2s minimum lcd page display time
#define LCDPAGETIMEMAX        10            // 10s maximum lcd page display time
#define DEFAULTSAVETIME       30000         // default time to wait before saving data to SPIFFS

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

#define STATEMOVINGSTR        "=>State_Moving"
#define STATEAPPLYBACKLASH    "=>State_ApplyBacklash"
#define EOFSTR                '#'
#define STARTSTR              ':'
#define coilpwrstr            "Coil power  ="
#define revdirstr             "Reverse Dir ="
#define tcompstepsstr         "TComp Steps = "
#define tcompstatestr         "TComp State = "
#define tcompdirstr           "TComp Dir   = "
#define backlashinstr         "Backlash In = "
#define backlashoutstr        "Backlash Out = "
#define backlashinstepsstr    "Backlash In#= "
#define backlashoutstepsstr   "Backlash Ou#= "
#define bluetoothstr          "Bluetooth Mode"
#define localserialstr        "Local Serial Mode"
#define onstr                 "On"
#define offstr                "Off"
#define instr                 "In"
#define outstr                "Out"
#define currentposstr         "Current Pos = "
#define targetposstr          "Target Pos  = "
#define tempstr               "Temperature = "
#define stepmodestr           "Step Mode   = "
#define motorspeedstr         "Motor Speed = "
#define slowstr               "Slow"
#define medstr                "Med"
#define faststr               "Fast"
#define maxstepsstr           "MaxSteps    = "
#define sendstr               "Send= "
#define startstr              "Start"
#define endstr                "End"
#define progressstr           "Progress: "
#define errorstr              "Error= "
#define readystr              "Ready"
#define serialstartstr        "Serial started"
#define debugonstr            "Debug on"
#define bluetoothstartstr     "Bluetooth started"
#define checkfortprobestr     "Check for Tprobe"
#define tprobenotfoundstr     "Tprobe not found"
#define startapstr            "Start Access Point"
#define startsmstr            "Start Station mode"
#define setstaticipstr        "Setup Static IP"
#define attemptsstr           "Attempt= "
#define starttcpserverstr     "Start TCP Server"
#define tcpserverstartedstr   "TCP Server started"
#define tcpserverportstr      "Port= "
#define attemptconnstr        "Attempt connection to= "
#define wifistartfailstr      "WiFi start failed"
#define wifirestartstr        "Restarting"
#define setupdrvbrdstr        "Setup drvbrd= "
#define drvbrddonestr         "Driver board done"
#define setupduckdnsstr       "Setup DuckDNS"
#define setupendstr           "Setup end"
#define startotaservicestr    "Start OTA service"
#define serverreadystr        "Server Ready"
#define ssidstr               "SSID = "
#define ipaddressstr          "IP   = "
#define jsonstring            "jsonstr = "
#define webserverstr          "Webserver: "
#define didnotconnectstr      "Did not connect to "

#define ASCOMSUCCESS              0
#define ASCOMNOTIMPLEMENTED       0x400
#define ASCOMINVALIDVALUE         0x401
#define ASCOMVALUENOTSET          0x402
#define ASCOMNOTCONNECTED         0x407
#define ASCOMINVALIDOPERATION     0x40B
#define ASCOMACTIONNOTIMPLEMENTED 0x40C

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
