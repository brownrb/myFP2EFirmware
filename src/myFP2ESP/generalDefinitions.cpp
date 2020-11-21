// ----------------------------------------------------------------------------------------------
// myFP2ESP GENERAL DEFINITIONS Textmessages
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2020. All Rights Reserved.
// (c) Copyright Holger M, 2019-2020. All Rights Reserved.
// ----------------------------------------------------------------------------------------------

const char* programVersion        = "131";
const char* ProgramAuthor         = "(c) R BROWN 2020";

const char* STATEMOVINGSTR        = ">Moving";
const char* STATEAPPLYBACKLASH    = ">ApplyBacklash";
const char* STATESETHOMEPOSITION  = ">SetHomePosition";
const char* STATEFINDHOMEPOSITION = ">FindHomePosition#= ";
const char* STATEDELAYAFTERMOVE   = ">DelayAfterMove= ";
const char* STATEFINISHEDMOVE     = ">FinishedMove";
const char* STATEIDLE             = ">Idle";
const char* STATEINITMOVE         = ">InitMove";
const char* PORTSTR               = "Port= ";
const char* SENDSTR               = "Send= ";
const char* SERIALSTARTSTR        = "Serial started";
const char* DEBUGONSTR            = "Debug on";
const char* BLUETOOTHSTARTSTR     = "Bluetooth started";
const char* ATTEMPTCONNSTR        = "Attempt connection to= ";
const char* APSTARTFAILSTR        = "Fail starting AP ";
const char* SERVERREADYSTR        = "Server Ready= ";
const char* STARTSTR              = "Start";
const char* ENDSTR                = "End";
const char* PROGRESSSTR           = "Progress: ";
const char* ERRORSTR              = "Err= ";
const char* READYSTR              = "Ready";
const char* SETUPDRVBRDSTR        = "Setup drvbrd= ";
const char* DRVBRDDONESTR         = "Driver board done";
const char* CHECKCPWRSTR          = "Check coilpower";
const char* CPWRRELEASEDSTR       = "Coil power released";
const char* STARTAPSTR            = "Start Access Point";
const char* STARTSMSTR            = "Start Station mode";
const char* SETSTATICIPSTR        = "Setup Static IP";
const char* ATTEMPTSSTR           = "Attempt= ";
const char* STARTTCPSERVERSTR     = "Start TCP Server";
const char* TCPSERVERSTARTEDSTR   = "TCP Server started";
const char* GETLOCALIPSTR         = "Get local IP address";
const char* SETUPDUCKDNSSTR       = "Setup DuckDNS";
const char* SETUPENDSTR           = "Setup end";
const char* STARTOTASERVICESTR    = "Start OTA service";
const char* SSIDSTR               = "SSID = ";
const char* IPADDRESSSTR          = "IP   = ";
const char* WIFIRESTARTSTR        = "Restarting";
const char* WIFIBEGINSTATUSSTR    = "Wifi.begin status code: ";
const char* CHECKFORTPROBESTR     = "Check for Tprobe";
const char* ACCESSPOINTSTR        = "Access point: ";
const char* STATIONMODESTR        = "Station mode: ";
const char* CONFIGSAVEDSTR        = "new Config saved: ";
const char* RELEASEMOTORSTR       = "Idle: release motor";
const char* LOOPSTARTSTR          = "Loop Start =";
const char* LOOPENDSTR            = "Loop End =";
const char* TCPCLIENTCONNECTSTR   = "tcp client connected";
const char* TCPCLIENTDISCONNECTSTR = "tcp client disconnected";
const char* APCONNECTFAILSTR      = "Did not connect to AP ";
const char* CONNECTEDSTR          = "Connected";
const char* I2CDEVICENOTFOUNDSTR  = "I2C device !found";
const char* ASCOMREMOTESTR        = "ASCOM Remote";
const char* REBOOTWSSTR           = "Rebooting web-server";
const char* REBOOTTCPSTR          = "Rebooting tcp/ip-server";
const char* REBOOTCNTLRSTR        = "Rebooting controller";
const char* REBOOTASCOMSTR        = "Rebooting ASCOM remote server";
const char* ASCOMSERVERNOTDEFINEDSTR = "ASCOM remote server not defined";
const char* WEBSERVERNOTDEFINEDSTR = "Web server not defined";
const char* SETPGOPTIONSTR        = "set OLED pg opt";
const char* SETPGTIMESTR          = "set OLED pg time";

const char* MANAGEMENTURLNOTFOUNDSTR  = "<html><head><title>Management server></title></head><body><p>URL was not found</p><p><form action=\"/\" method=\"GET\"><input type=\"submit\" value=\"HOME\"></form></p></body></html>";
const char* WEBSERVERURLNOTFOUNDSTR   = "<html><head><title>Web-server></title></head><body><p>URL was not found</p><p><form action=\"/\" method=\"GET\"><input type=\"submit\" value=\"HOME\"></form></p></body></html>";
const char* ASCOMSERVERURLNOTFOUNDSTR = "<html><head><title>ASCOM REMOTE SERVER</title></head><body><p>File not found</p><p><p><a href=\"/setup/v1/focuser/0/setup\">Setup page</a></p></body></html>";

const char* WRITEFILEFAILSTR        = "!write to file";
const char* WRITEFILESUCCESSSTR     = "Write to file OK";
const char* CREATEFILEFAILSTR       = "!create file";
const char* CHECKWIFICONFIGFILESTR  = "check for Wifi config file";
const char* DESERIALIZEERRORSTR     = "Deserialization err";
const char* SERVERNOTRUNNINGSTR     = "Server !running";

// temperature probe messages
const char* TPROBESTR               = "Tsensors= ";
const char* TPROBENOTFOUNDSTR       = "Tprobe !found";
const char* GETTEMPPROBESSTR        = "Get #Tsensors";
const char* SETTPROBERESSTR         = "Set Tprecision to ";

// home position switch
const char* HPCLOSEDFPNOT0STR       = "HP Sw=1, Pos !0";
const char* HPCLOSEDFP0STR          = "HP Sw=1, Pos=0";
const char* HPMOVETILLOPENSTR       = "HP Sw=0, Mov out";
const char* HPMOVEOUTERRORSTR       = "HP Sw=0, Mov out err";
const char* HPMOVEOUTFINISHEDSTR    = "HP Sw=0, Mov out ok";
const char* HPMOVEOUTSTEPSSTR       = "HP Steps, Mov out: ";

// oled messages
const char* CURRENTPOSSTR           =  "Current Pos = ";
const char* TARGETPOSSTR            =  "Target Pos  = ";
const char* COILPWRSTR              =  "Coil power  = ";
const char* REVDIRSTR               =  "Reverse Dir = ";
const char* STEPMODESTR             =  "Step Mode   = ";
const char* TEMPSTR                 =  "Temperature = ";
const char* MOTORSPEEDSTR           =  "Motor Speed = ";
const char* MAXSTEPSSTR             =  "MaxSteps    = ";
const char* TCOMPSTEPSSTR           =  "TComp Steps = ";
const char* TCOMPSTATESTR           =  "TComp State = ";
const char* TCOMPDIRSTR             =  "TComp Dir   = ";
const char* BACKLASHINSTR           =  "Backlash In = ";
const char* BACKLASHOUTSTR          =  "Backlash Out =";
const char* BACKLASHINSTEPSSTR      =  "Backlash In#= ";
const char* BACKLASHOUTSTEPSSTR     =  "Backlash Ou#= ";
const char* BLUETOOTHSTR            =  "Bluetooth Mode";
const char* LOCALSERIALSTR          =  "Local Serial Mode";
const char* FSFILENOTFOUNDSTR       =  "FS file !found";

const char* BACKCOLORINVALIDSTR     = "Back color !valid";
const char* NEWTITLECOLORSTR        = "Title color ";
const char* TITLECOLORINVALIDSTR    = "Title color !valid";
const char* NEWHEADERCOLORSTR       = "Header color ";
const char* HEADERCOLORINVALIDSTR   = "Header color !valid";
const char* NEWTEXTCOLORSTR         = "Text color ";
const char* TEXTCOLORINVALIDSTR     = "Text color !valid";

// joystick messages
const char* UPDATEJOYSTICKSTR       = "joystick: update joystick";
const char* JOYSTICKVALSTR          = "Raw joyval:";
const char* JOYSTICKXINVALSTR       = "X IN joyval:";
const char* JOYSTICKSPEEDSTR        = ", Speed:";
const char* JOYSTICKXOUTVALSTR      = "X OUT joyval:";

// defines for ASCOMSERVER, MDNSSERVER, WEBSERVER

const char* WEBSERVERSTR            = "Webserver: ";

const char* TEXTPAGETYPE            = "text/html";
const char* PLAINTEXTPAGETYPE       = "text/plain";
const char* JSONTEXTPAGETYPE        = "text/json";
const char* JSONPAGETYPE            = "application/json";
const char* FILENOTFOUNDSTR         = "File!found";
const char* FILEFOUNDSTR            = "File found";
const char* CANNOTCREATEFILESTR     = "!create file";

const char* FSNOTSTARTEDSTR         = "!start FS";
const char* BUILDDEFAULTPAGESTR     = "build default page";
const char* SPIFFSFILENOTFOUNDSTR   = "file !found";
const char* READPAGESTR             = "read page into string";
const char* PROCESSPAGESTARTSTR     = "process page start";
const char* PROCESSPAGEENDSTR       = "process page done";
const char* STARTASCOMSERVERSTR     = "start ascom server";
const char* STOPASCOMSERVERSTR      = "stop ascom server";
const char* STARTWEBSERVERSTR       = "start web server";
const char* STOPWEBSERVERSTR        = "stop web server";
const char* STOPMDNSSERVERSTR       = "stop mdns server";

const char* SERVERSTATESTOPSTR      = "STOPPED";
const char* SERVERSTATESTARTSTR     = "STARTED";
const char* SERVERSTATERUNSTR       = "RUNNING";
const char* SENDPAGESTR             = "Send page";
const char* ENABLEDSTR              = "Enabled";
const char* NOTENABLEDSTR           = "Disabled";

const char* MANAGEMENTNOTFOUNDSTR   = "<html><head><title>Management Server</title></head><body><p>URL not found</p><p><form action=\"/\" method=\"GET\"><input type=\"submit\" value=\"HOMEPAGE\"></form></p></body></html>";
const char* WEBSERVERNOTFOUNDSTR    = "<html><head><title>Web Server</title></head><body><p>URL not found</p><p><form action=\"/\" method=\"GET\"><input type=\"submit\" value=\"HOMEPAGE\"></form></p></body></html>";
const char* ASCOMSERVERNOTFOUNDSTR  = "<html><head><title>ASCOM REMOTE Server</title></head><body><p>FS not started</p><p><p><a href=\"/setup/v1/focuser/0/setup\">Setup page</a></p></body></html>";

const char* MDNSSTARTFAILSTR        = "Err setting up MDNS responder";
const char* MDNSSTARTEDSTR          = "mDNS responder started";

const char* CREBOOTSTR              = "<form action=\"/\" method =\"post\"><input type=\"hidden\" name=\"srestart\" value=\"true\"><input type=\"submit\" value=\"REBOOT CONTROLLER\"></form>";

const char* ENABLEBKINSTR           = "<form action=\"/msindex3\" method=\"post\"><b>BL-IN State</b> [%STI%]: <input type=\"hidden\" name=\"enin\" value=\"true\"><input type=\"submit\" value=\"ENABLE\"></form>";
const char* DISABLEBKINSTR          = "<form action=\"/msindex3\" method=\"post\"><b>BL-IN State</b> [%STI%]: <input type=\"hidden\" name=\"diin\" value=\"true\"><input type=\"submit\" value=\"DISABLE\"></form>";
const char* ENABLEBKOUTSTR          = "<form action=\"/msindex3\" method=\"post\"><b>BL-OUT State</b> [%STO%]: <input type=\"hidden\" name=\"enou\" value=\"true\"><input type=\"submit\" value=\"ENABLE\"></form>";
const char* DISABLEBKOUTSTR         = "<form action=\"/msindex3\" method=\"post\"><b>BL-OUT State</b> [%STO%]: <input type=\"hidden\" name=\"diou\" value=\"true\"><input type=\"submit\" value=\"DISABLE\"></form>";
const char* BLINSTEPSTR             = "<form action=\"/msindex3\" method =\"post\"><b>BL-In &nbsp;Steps:</b> <input type=\"text\" name=\"bis\" size=\"6\" value=\"%bins%\"> <input type=\"submit\" name=\"setbis\" value=\"Set\"></form>";
const char* BLOUTSTEPSTR            = "<form action=\"/msindex3\" method =\"post\"><b>BL-Out Steps:</b> <input type=\"text\" name=\"bos\" size=\"6\" value=\"%bous%\"> <input type=\"submit\" name=\"setbos\" value=\"Set\"></form>";

const char* ENABLELEDSTR            = "<form action=\"/msindex2\" method=\"post\"><b>State</b> [%INL%]: <input type=\"hidden\" name=\"startle\" value=\"true\"><input type=\"submit\" value=\"ENABLE\"></form>";
const char* DISABLELEDSTR           = "<form action=\"/msindex2\" method=\"post\"><b>State</b> [%INL%]: <input type=\"hidden\" name=\"stople\" value=\"true\"><input type=\"submit\" value=\"DISABLE\"></form>";
const char* ENABLETEMPSTR           = "<form action=\"/msindex2\" method=\"post\"><b>State</b> [%TPE%]: <input type=\"hidden\" name=\"starttp\" value=\"true\"><input type=\"submit\" value=\"ENABLE\"></form>";
const char* DISABLETEMPSTR          = "<form action=\"/msindex2\" method=\"post\"><b>State</b> [%TPE%]: <input type=\"hidden\" name=\"stoptp\" value=\"true\"><input type=\"submit\" value=\"DISABLE\"></form>";
const char* STOPTSSTR               = "<form action=\"/msindex2\" method=\"post\"><b>Status</b> [%TST%]: <input type=\"hidden\" name=\"stopts\" value=\"true\"><input type=\"submit\" value=\"STOP\"></form>";
const char* STARTTSSTR              = "<form action=\"/msindex2\" method=\"post\"><b>Status</b> [%TST%]: <input type=\"hidden\" name=\"startts\" value=\"true\"><input type=\"submit\" value=\"START\"></form>";
const char* STOPWSSTR               = "<form action=\"/msindex2\" method=\"post\"><b>Status</b> [%WST%]: <input type=\"hidden\" name=\"stopws\" value=\"true\"><input type=\"submit\" value=\"STOP\"></form>";
const char* STARTWSSTR              = "<form action=\"/msindex2\" method=\"post\"><b>Status</b> [%WST%]: <input type=\"hidden\" name=\"startws\" value=\"true\"><input type=\"submit\" value=\"START\"></form>";
const char* STOPASSTR               = "<form action=\"/msindex2\" method=\"post\"><b>Status</b> [%ABT%]: <input type=\"hidden\" name=\"stopas\" value=\"true\"><input type=\"submit\" value=\"STOP\"></form>";
const char* STARTASSTR              = "<form action=\"/msindex2\" method=\"post\"><b>Status</b> [%ABT%]: <input type=\"hidden\" name=\"startas\" value=\"true\"><input type=\"submit\" value=\"START\"></form>";
const char* DISPLAYCSTR             = "<form action=\"/msindex2\" method=\"post\"><b>Temp Mode: </b><input type=\"hidden\" name=\"tm\" value=\"cel\" Checked><input type=\"submit\" value=\"Enable Celsius\"></form>";
const char* DISPLAYFSTR             = "<form action=\"/msindex2\" method=\"post\"><b>Temp Mode: </b><input type=\"hidden\" name=\"tm\" value=\"fah\"><input type=\"submit\" value=\"Enable Fahrenheit\"></form>";

const char* MDNSTOPSTR              = "<form action=\"/\" method=\"post\"><input type=\"hidden\" name=\"stopmdns\" value=\"true\"><input type=\"submit\" value=\"STOP\"></form>";
const char* MDNSSTARTSTR            = "<form action=\"/\" method=\"post\"><input type=\"hidden\" name=\"startmdns\" value=\"true\"><input type=\"submit\" value=\"START\"></form>";
const char* DISPLAYONSTR            = "<form action=\"/\" method=\"post\"><b>Display: </b><input type=\"hidden\" name=\"di\" value=\"doff\" Checked><input type=\"submit\" value=\"Turn Off\"></form>";
const char* DISPLAYOFFSTR           = "<form action=\"/\" method=\"post\"><b>Display: </b><input type=\"hidden\" name=\"di\" value=\"don\"><input type=\"submit\" value=\"Turn On\"></form>";
const char* STARTSCREENONSTR        = "<form action=\"/\" method=\"post\"><b>Startscreen: </b><input type=\"hidden\" name=\"ss\" value=\"ssoff\" Checked><input type=\"submit\" value=\"Turn Off\"></form>";
const char* STARTSCREENOFFSTR       = "<form action=\"/\" method=\"post\"><b>Startscreen: </b><input type=\"hidden\" name=\"ss\" value=\"sson\"><input type=\"submit\" value=\"Turn On\"></form>";
const char* STARTHPSWMONSTR         = "<form action=\"/\" method=\"post\"><b>HPSW Messages: </b><input type=\"hidden\" name=\"hp\" value=\"hpoff\" Checked><input type=\"submit\" value=\"Turn Off\"></form>";
const char* STARTHPSWMOFFSTR        = "<form action=\"/\" method=\"post\"><b>HPSW Messages: </b><input type=\"hidden\" name=\"hp\" value=\"hpon\"><input type=\"submit\" value=\"Turn On\"></form>";
const char* STARTFMDLONSTR          = "<form action=\"/\" method=\"post\"><b>MS Forcedownload: </b><input type=\"hidden\" name=\"fd\" value=\"fdoff\" Checked><input type=\"submit\" value=\"Turn Off\"></form>";
const char* STARTFMDLOFFSTR         = "<form action=\"/\" method=\"post\"><b>MS Forcedownload: </b><input type=\"hidden\" name=\"fd\" value=\"fdon\"><input type=\"submit\" value=\"Turn On\"></form>";

const char* NOTDEFINEDSTR           = "!defined in firmware";
