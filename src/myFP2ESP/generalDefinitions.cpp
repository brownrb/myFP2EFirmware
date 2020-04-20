// ----------------------------------------------------------------------------------------------
// myFP2ESP GENERAL DEFINITIONS Textmessages
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2020. All Rights Reserved.
// (c) Copyright Holger M, 2019-2020. All Rights Reserved.
// ----------------------------------------------------------------------------------------------

const char* programVersion        = "119";
const char* ProgramAuthor         = "(c) R BROWN 2020 h120";


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
const char* APSTARTFAILSTR        = "Did not connect to AP ";
const char* SERVERREADYSTR        = "Server Ready= ";
const char* STARTSTR              = "Start";
const char* ENDSTR                = "End";
const char* PROGRESSSTR           = "Progress: ";
const char* ERRORSTR              = "Error= ";
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
const char* TCPCLIENTCONNECTSTR   = "tcp client has connected";
const char* TCPCLIENTDISCONNECTSTR = "tcp client has disconnected";
const char* APCONNECTFAILSTR      = "Did not connect to AP ";
const char* CONNECTEDSTR          = "Connected";
const char* I2CDEVICENOTFOUNDSTR  = "I2C device not found";

const char* WRITEFILEFAILSTR      = "Write to file failed";
const char* WRITEFILESUCCESSSTR   = "Write to file OK";
const char* CREATEFILEFAILSTR     = "Unable to create file";
const char* CHECKWIFICONFIGFILESTR = "check for Wifi config file";
const char* DESERIALIZEERRORSTR   = "Deserialization error";
const char* SERVERNOTRUNNINGSTR   = "Server NOT running";

const char* HPCLOSEDFPNOT0STR     = "HP closed, fcurrentPosition !=0";
const char* HPCLOSEDFP0STR        = "HP closed, fcurrentPosition=0";
const char* HPOPENFPNOT0STR       = "HP Open, fcurrentPosition=0";
const char* HPMOVETILLCLOSEDSTR   = "HP MoveIN till closed";
const char* HPMOVEINERRORSTR      = "HP MoveIN ERROR: HOMESTEPS exceeded";
const char* HPMOVEINSTEPSSTR      = "HP MoveIN stepstaken=";
const char* HPMOVEINFINISHEDSTR   = "HP MoveIN finished";
const char* HPMOVETILLOPENSTR     = "HP Move out till OPEN";
const char* HPMOVEOUTERRORSTR     = "HP MoveOUT ERROR: HOMESTEPS exceeded#";
const char* HPMOVEOUTSTEPSSTR     = "HP MoveOUT stepstaken=";
const char* HPMOVEOUTFINISHEDSTR  = "HP MoveOUT ended";

// temperature probe messages
const char* TPROBESTR             = "Tsensors= ";
const char* TPROBENOTFOUNDSTR     = "Tprobe not found";
const char* GETTEMPPROBESSTR      = "Get # of Tsensors";
const char* SETTPROBERESSTR       = "Set Tprecision to ";
// oled messages

const char* CURRENTPOSSTR       =  "Current Pos = ";
const char* TARGETPOSSTR        =  "Target Pos  = ";
const char* COILPWRSTR          =  "Coil power  = ";
const char* REVDIRSTR           =  "Reverse Dir = ";
const char* STEPMODESTR         =  "Step Mode   = ";
const char* TEMPSTR             =  "Temperature = ";
const char* MOTORSPEEDSTR       =  "Motor Speed = ";
const char* MAXSTEPSSTR         =  "MaxSteps    = ";
const char* TCOMPSTEPSSTR       =  "TComp Steps = ";
const char* TCOMPSTATESTR       =  "TComp State = ";
const char* TCOMPDIRSTR         =  "TComp Dir   = ";
const char* BACKLASHINSTR       =  "Backlash In = ";
const char* BACKLASHOUTSTR      =  "Backlash Out =";
const char* BACKLASHINSTEPSSTR  =  "Backlash In#= ";
const char* BACKLASHOUTSTEPSSTR =  "Backlash Ou#= ";
const char* BLUETOOTHSTR        =  "Bluetooth Mode";
const char* LOCALSERIALSTR      =  "Local Serial Mode";
