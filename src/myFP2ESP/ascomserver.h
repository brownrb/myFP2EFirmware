// ----------------------------------------------------------------------------------------------
// myFP2ESP ASCOM SERVER ROUTINES AND DEFINITIONS
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2019. All Rights Reserved.
// (c) Copyright Holger M, 2019. All Rights Reserved.
// ----------------------------------------------------------------------------------------------

#ifndef ascomserver_h
#define ascomserver_h

// ----------------------------------------------------------------------------------------------
// EXTERNS
// ----------------------------------------------------------------------------------------------
extern SetupData *mySetupData;
extern unsigned long fcurrentPosition;         // current focuser position
extern unsigned long ftargetPosition;          // target position
extern byte isMoving;
extern String ipStr;
extern String programName;
extern char programVersion[];
extern DriverBoard* driverboard;
#ifdef TEMPERATUREPROBE
extern float readtemp(byte);
#endif

// ----------------------------------------------------------------------------------------------
// DATA AND DEFINITIONS - ASCOM SETUP WEB PAGE
// ----------------------------------------------------------------------------------------------
#ifdef ASCOMREMOTE
#define AS_COPYRIGHT              "<p>(c) R. Brown, Holger M, 2019. All rights reserved.</p>"
#define AS_TITLE                  "<h3>myFP2ESP ASCOM REMOTE focus controller</h3>"
#define AS_PAGETITLE              "<title>myFP2ESP ASCOM SERVER</title>"

#define AS_SM1CHECKED             "<input type=\"radio\" name=\"sm\" value=\"1\" Checked> Full"
#define AS_SM1UNCHECKED           "<input type=\"radio\" name=\"sm\" value=\"1\"> Full"
#define AS_SM2CHECKED             "<input type=\"radio\" name=\"sm\" value=\"2\" Checked> 1/2"
#define AS_SM2UNCHECKED           "<input type=\"radio\" name=\"sm\" value=\"2\"> 1/2"
#define AS_SM4CHECKED             "<input type=\"radio\" name=\"sm\" value=\"4\" Checked> 1/4"
#define AS_SM4UNCHECKED           "<input type=\"radio\" name=\"sm\" value=\"4\"> 1/4"
#define AS_SM8CHECKED             "<input type=\"radio\" name=\"sm\" value=\"8\" Checked> 1/8"
#define AS_SM8UNCHECKED           "<input type=\"radio\" name=\"sm\" value=\"8\"> 1/8"
#define AS_SM16CHECKED            "<input type=\"radio\" name=\"sm\" value=\"16\" Checked> 1/16"
#define AS_SM16UNCHECKED          "<input type=\"radio\" name=\"sm\" value=\"16\"> 1/16"
#define AS_SM32CHECKED            "<input type=\"radio\" name=\"sm\" value=\"32\" Checked> 1/32"
#define AS_SM32UNCHECKED          "<input type=\"radio\" name=\"sm\" value=\"32\"> 1/32"

#define AS_MSSLOWCHECKED          "<input type=\"radio\" name=\"ms\" value=\"0\" Checked> Slow"
#define AS_MSSLOWUNCHECKED        "<input type=\"radio\" name=\"ms\" value=\"0\"> Slow"
#define AS_MSMEDCHECKED           "<input type=\"radio\" name=\"ms\" value=\"1\" Checked> Medium"
#define AS_MSMEDUNCHECKED         "<input type=\"radio\" name=\"ms\" value=\"1\"> Medium"
#define AS_MSFASTCHECKED          "<input type=\"radio\" name=\"ms\" value=\"2\" Checked> Fast"
#define AS_MSFASTUNCHECKED        "<input type=\"radio\" name=\"ms\" value=\"2\"> Fast"
#endif

// ----------------------------------------------------------------------------------------------
// DATA AND DEFINITIONS
// ----------------------------------------------------------------------------------------------
#ifdef ASCOMREMOTE
#define ASCOMMAXIMUMARGS          10
#define ASCOMSUCCESS              0
#define ASCOMNOTIMPLEMENTED       0x400
#define ASCOMINVALIDVALUE         0x401
#define ASCOMVALUENOTSET          0x402
#define ASCOMNOTCONNECTED         0x407
#define ASCOMINVALIDOPERATION     0x40B
#define ASCOMACTIONNOTIMPLEMENTED 0x40C
#define ASCOMERRORMSGERROR        "Error"
#define ASCOMERRORMSGNULL         ""
#define ASCOMERRORNOTIMPLEMENTED  "Not implemented"
#define ASCOMERRORMSGINVALID      "Invalid operation"
#define ASCOMNAME                 "\"myFP2ESPASCOMR\""
#define ASCOMDESCRIPTION          "\"ASCOM driver for myFP2ESP controllers\""
#define ASCOMDRIVERINFO           "\"myFP2ESP ASCOM Driver (c) R. Brown. 2019\""
#define ASCOMMANAGEMENTINFO       "{\"ServerName\":\"myFP2ESP\",\"Manufacturer\":\"R. Brown\",\"ManufacturerVersion\":\"v1.0\",\"Location\":\"New Zealand\"}"

String Focuser_Setup_HomePage;    //  url:/setup/v1/focuser/0/setup
String Setup_HomePage;            //  url:/setup
unsigned int ASCOMClientID;
unsigned int ASCOMClientTransactionID;
unsigned int ASCOMServerTransactionID = 0;
int ASCOMErrorNumber = 0;
String ASCOMErrorMessage = "";
long ASCOMpos = 0L;
byte ASCOMTempCompState = 0;
byte ASCOMConnectedState = 0;
WiFiClient ascomclient;
#if defined(ESP8266)
ESP8266WebServer ascomserver(ALPACAPORT);
#else
WebServer ascomserver(ALPACAPORT);
#endif // if defined(esp8266)

#endif // ifdef ASCOMREMOTE

// ----------------------------------------------------------------------------------------------
// CODE 
// ----------------------------------------------------------------------------------------------
#ifdef ASCOMREMOTE
// Construct pages for /setup
void ASCOM_Setup_HomePage()
{
  Setup_HomePage = "<html><head></head>";
  Setup_HomePage = Setup_HomePage + "<title>myFP2ESP ASCOM REMOTE</title></head>";
  Setup_HomePage = Setup_HomePage + "<body><h3>myFP2ESP ASCOM REMOTE</h3>";
  Setup_HomePage = Setup_HomePage + "<p>(c) R. Brown, Holger M, 2019. All rights reserved.</p>";
  Setup_HomePage = Setup_HomePage + "<p>IP Address: " + ipStr + ": Port: " + String(ALPACAPORT) + ", Firmware Version=" + String(programVersion) + "</p>";
  Setup_HomePage = Setup_HomePage + "<p>Driverboard = myFP2ESP." + driverboard->getboardname() + "</p>";
  // add link for setup page /setup/v1/focuser/0/setup
  Setup_HomePage = Setup_HomePage + "<p><a href=\"/setup/v1/focuser/0/setup\">Setup page</a></p>";
  Setup_HomePage = Setup_HomePage + "</body></html>";  
}

// constructs ascom setup server page /setup/v1/focuser/0/setup
void ASCOM_Create_Setup_Focuser_HomePage()
{
  // Convert IP address to a string;
  // already in ipStr
  // convert current values of focuserposition and focusermaxsteps to string types
  String fpbuffer = String(fcurrentPosition);
  String mxbuffer = String(mySetupData->get_maxstep());
  String smbuffer = String(mySetupData->get_stepmode());
  String imbuffer = String(isMoving);

  switch ( mySetupData->get_stepmode() )
  {
    case 1:
      smbuffer = AS_SM1CHECKED;
      smbuffer = smbuffer + AS_SM2UNCHECKED;
      smbuffer = smbuffer + AS_SM4UNCHECKED;
      smbuffer = smbuffer + AS_SM8UNCHECKED;
      smbuffer = smbuffer + AS_SM16UNCHECKED;
      smbuffer = smbuffer + AS_SM32UNCHECKED;
      break;
    case 2 :
      smbuffer = AS_SM1UNCHECKED;
      smbuffer = smbuffer + AS_SM2CHECKED;
      smbuffer = smbuffer + AS_SM4UNCHECKED;
      smbuffer = smbuffer + AS_SM8UNCHECKED;
      smbuffer = smbuffer + AS_SM16UNCHECKED;
      smbuffer = smbuffer + AS_SM32UNCHECKED;
      break;
    case 4 :
      smbuffer = AS_SM1UNCHECKED;
      smbuffer = smbuffer + AS_SM2UNCHECKED;
      smbuffer = smbuffer + AS_SM4CHECKED;
      smbuffer = smbuffer + AS_SM8UNCHECKED;
      smbuffer = smbuffer + AS_SM16UNCHECKED;
      smbuffer = smbuffer + AS_SM32UNCHECKED;
      break;
    case 8 :
      smbuffer = AS_SM1UNCHECKED;
      smbuffer = smbuffer + AS_SM2UNCHECKED;
      smbuffer = smbuffer + AS_SM4UNCHECKED;
      smbuffer = smbuffer + AS_SM8CHECKED;
      smbuffer = smbuffer + AS_SM16UNCHECKED;
      smbuffer = smbuffer + AS_SM32UNCHECKED;
      break;
    case 16 :
      smbuffer = AS_SM1UNCHECKED;
      smbuffer = smbuffer + AS_SM2UNCHECKED;
      smbuffer = smbuffer + AS_SM4UNCHECKED;
      smbuffer = smbuffer + AS_SM8UNCHECKED;
      smbuffer = smbuffer + AS_SM16CHECKED;
      smbuffer = smbuffer + AS_SM32UNCHECKED;
      break;
    case 32 :
      smbuffer = AS_SM1UNCHECKED;
      smbuffer = smbuffer + AS_SM2UNCHECKED;
      smbuffer = smbuffer + AS_SM4UNCHECKED;
      smbuffer = smbuffer + AS_SM8UNCHECKED;
      smbuffer = smbuffer + AS_SM16UNCHECKED;
      smbuffer = smbuffer + AS_SM32CHECKED;
      break;
    default :
      smbuffer = AS_SM1CHECKED;
      smbuffer = smbuffer + AS_SM2UNCHECKED;
      smbuffer = smbuffer + AS_SM4UNCHECKED;
      smbuffer = smbuffer + AS_SM8UNCHECKED;
      smbuffer = smbuffer + AS_SM16UNCHECKED;
      smbuffer = smbuffer + AS_SM32UNCHECKED;
      break;
  }

  String msbuffer = String(mySetupData->get_motorSpeed());
  switch ( mySetupData->get_motorSpeed() )
  {
    case 0:
      msbuffer = AS_MSSLOWCHECKED;
      msbuffer = msbuffer + AS_MSMEDUNCHECKED;
      msbuffer = msbuffer + AS_MSFASTUNCHECKED;
      break;
    case 1:
      msbuffer = AS_MSSLOWUNCHECKED;
      msbuffer = msbuffer + AS_MSMEDCHECKED;
      msbuffer = msbuffer + AS_MSFASTUNCHECKED;
      break;
    case 2:
      msbuffer = AS_MSSLOWUNCHECKED;
      msbuffer = msbuffer + AS_MSMEDUNCHECKED;
      msbuffer = msbuffer + AS_MSFASTCHECKED;
      break;
    default:
      msbuffer = AS_MSSLOWUNCHECKED;
      msbuffer = msbuffer + AS_MSMEDUNCHECKED;
      msbuffer = msbuffer + AS_MSFASTCHECKED;
      break;
  }

  String cpbuffer;
  if ( !mySetupData->get_coilpower() )
  {
    cpbuffer = "<input type=\"checkbox\" name=\"cp\" value=\"cp\" > ";
  }
  else
  {
    cpbuffer = "<input type=\"checkbox\" name=\"cp\" value=\"cp\" Checked> ";
  }

  String rdbuffer;
  if ( !mySetupData->get_reversedirection() )
  {
    rdbuffer = "<input type=\"checkbox\" name=\"rd\" value=\"rd\" > ";
  }
  else
  {
    rdbuffer = "<input type=\"checkbox\" name=\"rd\" value=\"rd\" Checked> ";
  }

  // construct home page of webserver
  // header
  Focuser_Setup_HomePage = "<head>" + String(AS_PAGETITLE) + "</head><body>";
  Focuser_Setup_HomePage = Focuser_Setup_HomePage + String(AS_TITLE);

  Focuser_Setup_HomePage = Focuser_Setup_HomePage + String(AS_COPYRIGHT);
  Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<p>Driverboard = myFP2ESP." + programName + "<br>";
  Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<myFP2ESP." + programName + "</h3>IP Address: " + ipStr + ", Firmware Version=" + String(programVersion) + "</br>";

  // position. set position
  Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><br><b>Focuser Position</b> <input type=\"text\" name=\"fp\" size =\"15\" value=" + fpbuffer + "> ";
  Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<input type=\"submit\" name=\"setpos\" value=\"Set Pos\"> </form></p>";

  // maxstep
  Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>MaxSteps</b> <input type=\"text\" name=\"fm\" size =\"15\" value=" + mxbuffer + "> ";
  Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<input type=\"submit\" value=\"Submit\"></form>";

  // coilpower
  Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>Coil Power </b>" + cpbuffer ;
  Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<input type=\"hidden\" name=\"cp\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

  // reverse direction
  Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>Reverse Direction </b>" + rdbuffer ;
  Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<input type=\"hidden\" name=\"rd\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

  // stepmode
  Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>Step Mode </b>" + smbuffer + " ";
  Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<input type=\"hidden\" name=\"sm\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

  // motor speed
  Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>Motor Speed: </b>" + msbuffer + " ";
  Focuser_Setup_HomePage = Focuser_Setup_HomePage + "<input type=\"hidden\" name=\"ms\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

  Focuser_Setup_HomePage = Focuser_Setup_HomePage + "</body></html>\r\n";
}

// generic ASCOM send reply
void ASCOM_sendreply( int replycode, String contenttype, String jsonstr)
{
  DebugPrint("ASCOM_sendreply: replycode:");
  DebugPrint(replycode);
  DebugPrint(" , content-type:");
  DebugPrint(contenttype);
  DebugPrint(", \njson:");
  DebugPrintln(jsonstr);
  // ascomserver.send builds the http header, jsonstr will be in the body
  ascomserver.send(replycode, contenttype, jsonstr );
}

void ASCOM_getURLParameters()
{
  String str;
  // get server args, translate server args to lowercase, they can be mixed case
  DebugPrintln("ASCOM_getURLParameters START");
  DebugPrint("Number of args:");
  DebugPrintln(ascomserver.args());
  for (int i = 0; i < ascomserver.args(); i++)
  {
    if ( i >= ASCOMMAXIMUMARGS )
    {
      break;
    }
    str = ascomserver.argName(i);
    str.toLowerCase();
    DebugPrint("Paramter Found: ");
    DebugPrintln(str);
    if ( str.equals("clientid") )
    {
      ASCOMClientID = (unsigned int) ascomserver.arg(i).toInt();
      DebugPrint("clientID:");
      DebugPrintln(ASCOMClientID);
    }
    if ( str.equals("clienttransactionid") )
    {
      ASCOMClientTransactionID = (unsigned int) ascomserver.arg(i).toInt();
      DebugPrint("clienttransactionid:");
      DebugPrintln(ASCOMClientTransactionID);
    }
    if ( str.equals("tempcomp") )
    {
      String strtmp = ascomserver.arg(i);
      strtmp.toLowerCase();
      if ( strtmp.equals("true") )
      {
        ASCOMTempCompState = 1;
      }
      else
      {
        ASCOMTempCompState = 0;
      }
      DebugPrint("ASCOMTempCompState:");
      DebugPrintln(ASCOMTempCompState);
    }
    if ( str.equals("position") )
    {
      String str = ascomserver.arg(i);
      DebugPrint("ASCOMpos RAW:");
      DebugPrintln(str);
      ASCOMpos = ascomserver.arg(i).toInt();      // this returns a long data type
      DebugPrint("ASCOMpos:");
      DebugPrintln(ASCOMpos);
    }
    if ( str.equals("connected") )
    {
      String strtmp = ascomserver.arg(i);
      strtmp.toLowerCase();
      DebugPrint("conneded RAW:");
      DebugPrintln(str);
      if ( strtmp.equals("true") )
      {
        ASCOMConnectedState = 1;
      }
      else
      {
        ASCOMConnectedState = 0;
      }
      DebugPrint("ASCOMConnectedState:");
      DebugPrintln(ASCOMConnectedState);
    }
    // todo
  }
  DebugPrintln("ASCOM_getURLParameters END");
}

String ASCOM_addclientinfo(String str )
{
  String str1 = str;
  // add clientid
  str1 = str1 +  "\"ClientID\":" + String(ASCOMClientID) + ",";
  // add clienttransactionid
  str1 = str1 + "\"ClientTransactionID\":" + String(ASCOMClientTransactionID) + ",";
  // add ServerTransactionID
  str1 = str1 + "\"ServerTransactionID\":" + String(ASCOMServerTransactionID) + ",";
  // add errornumber
  str1 = str1 + "\"ErrorNumber\":\"" + String(ASCOMErrorNumber) + "\",";
  // add errormessage
  if ( ASCOMErrorMessage == "" )
  {
    str1 = str1 + "\"ErrorMessage\":\"\"}";
  }
  else
  {
    str1 = str1 + "\"ErrorMessage\":\"" + ASCOMErrorMessage + "\"}";
  }
  return str1;
}

// ----------------------------------------------------------------------------------------------
// Setup functions
// ----------------------------------------------------------------------------------------------
void ASCOM_handle_setup()
{
  // url /setup
  // The web page must describe the overall device, including name, manufacturer and version number.
  // content-type: text/html
  ASCOMServerTransactionID++;
  ASCOM_sendreply( NORMALWEBPAGE, TEXTPAGETYPE, Setup_HomePage);
  delay(10);                     // small pause so background tasks can run    
}

void ASCOM_handle_focuser_setup()
{
  // url /setup/v1/focuser/0/setup
  // Configuration web page for the specified device
  // content-type: text/html

  // if set focuser position
  String fpos_str = ascomserver.arg("setpos");
  if ( fpos_str != "" )
  {
    Serial.print( "setpos:" );
    Serial.println(fpos_str);
    String fp = ascomserver.arg("fp");
    if ( fp != "" )
    {
      unsigned long temp = 0;
      Serial.print("fp:");
      Serial.println(fp);
      temp = fp.toInt();
      if ( temp > mySetupData->get_maxstep() )  // if greater than maxStep then set to maxStep
      {
        temp = mySetupData->get_maxstep();
      }
      fcurrentPosition = ftargetPosition = temp;
    }
  }

  // if update of maxsteps
  String fmax_str = ascomserver.arg("fm");
  if ( fmax_str != "" )
  {
    unsigned long temp = 0;
    DebugPrint( "root() -maxsteps:" );
    DebugPrintln(fmax_str);
    temp = fmax_str.toInt();
    if ( temp < fcurrentPosition )                    // if maxstep is less than focuser position
    {
      temp = fcurrentPosition + 1;
    }
    if ( temp < FOCUSERLOWERLIMIT )                   // do not set it less then 1024
    {
      temp = FOCUSERLOWERLIMIT;
    }
    if ( temp > mySetupData->get_maxstep() )          // if higher than max value
    {
      temp = mySetupData->get_maxstep();
    }
    mySetupData->set_maxstep(temp);
  }

  // if update motorspeed
  String fms_str = ascomserver.arg("ms");
  if ( fms_str != "" )
  {
    int temp1 = 0;
    DebugPrint( "root() -motorspeed:" );
    DebugPrintln(fms_str);
    temp1 = fms_str.toInt();
    if ( temp1 < SLOW )
    {
      temp1 = SLOW;
    }
    if ( temp1 > FAST )
    {
      temp1 = FAST;
    }
    mySetupData->set_motorSpeed(temp1);
  }

  // if update coilpower
  String fcp_str = ascomserver.arg("cp");
  if ( fcp_str != "" )
  {
    DebugPrint( "root() -coil power:" );
    DebugPrintln(fcp_str);
    if ( fcp_str == "cp" )
    {
      mySetupData->set_coilpower(1);
    }
    else
    {
      mySetupData->set_coilpower(0);
    }
  }

  // if update reversedirection
  String frd_str = ascomserver.arg("rd");
  if ( frd_str != "" )
  {
    DebugPrint( "root() -reverse direction:" );
    DebugPrintln(frd_str);
    if ( frd_str == "rd" )
    {
      mySetupData->set_reversedirection(1);
    }
    else
    {
      mySetupData->set_reversedirection(0);
    }
  }

  // if update stepmode
  // (1=Full, 2=Half, 4=1/4, 8=1/8, 16=1/16, 32=1/32, 64=1/64, 128=1/128)
  String fsm_str = ascomserver.arg("sm");
  if ( fsm_str != "" )
  {
    int temp1 = 0;
    DebugPrint( "root() -stepmode:" );
    DebugPrintln(fsm_str);
    temp1 = fsm_str.toInt();
    if ( temp1 < STEP1 )
    {
      temp1 = STEP1;
    }
    if ( temp1 > STEP32 )
    {
      temp1 = STEP32;
    }
    mySetupData->set_stepmode(temp1);
  }

  DebugPrintln( "root() -build homepage" );

  // construct the homepage now
  ASCOM_Create_Setup_Focuser_HomePage();

  // send the homepage to a connected client
  ASCOMServerTransactionID++;
  DebugPrintln("root() - send homepage");
  ASCOM_sendreply( NORMALWEBPAGE, TEXTPAGETYPE, Focuser_Setup_HomePage);
  delay(10);                     // small pause so background ESP8266 tasks can run 
}

// ----------------------------------------------------------------------------------------------
// Management API functions
// ----------------------------------------------------------------------------------------------
void ASCOM_handleapiversions()
{
  // url /management/apiversions
  // Returns an integer array of supported Alpaca API version numbers.
  // { "Value": [1,2,3,4],"ClientTransactionID": 9876,"ServerTransactionID": 54321}
  String jsonretstr = "";
  DebugPrintln("ASCOM_handleapiversions:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":[1]," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handleapidescription()
{
  // url /management/v1/description 
  // Returns cross-cutting information that applies to all devices available at this URL:Port.
  // content-type: application/json
  // { "Value": { "ServerName": "Random Alpaca Device", "Manufacturer": "The Briliant Company",
  //   "ManufacturerVersion": "v1.0.0", "Location": "Horsham, UK" },
  //   "ClientTransactionID": 9876, "ServerTransactionID": 54321 }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handleapidescription:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":" + String(ASCOMMANAGEMENTINFO) + "," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handleapiconfigureddevices()
{
  // url /management/v1/configureddevices
  // Returns an array of device description objects, providing unique information for each served device, enabling them to be accessed through the Alpaca Device API.
  // content-type: application/json
  // { "Value": [{"DeviceName": "Super focuser 1","DeviceType": "Focuser","DeviceNumber": 0,"UniqueID": "277C652F-2AA9-4E86-A6A6-9230C42876FA"}],"ClientTransactionID": 9876,"ServerTransactionID": 54321}
  String jsonretstr = "";
  DebugPrintln("ASCOM_handleapiconfigureddevices:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":[{\"DeviceName\":" + String(ASCOMNAME) + ",\"DeviceType\":\"focuser\",\"DeviceNumber\":0,\"UniqueID\":\"7e239e71-d304-4e7e-acda-3ff2e2b68515\"}]," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

// ----------------------------------------------------------------------------------------------
// ASCOM ALPACA API
// ----------------------------------------------------------------------------------------------
void ASCOM_handleinterfaceversionget()
{
  // curl -X GET "/api/v1/focuser/0/interfaceversion?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {"Value": 0,  "ErrorNumber": 0,  "ErrorMessage": "string"}
  String jsonretstr = "";
  DebugPrintln("ASCOM_handleinterfaceversionget:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":3," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handleconnectedput()
{
  // PUT "/api/v1/focuser/0/connected" -H  "accept: application/json" -H  "Content-Type: application/x-www-form-urlencoded" -d "Connected=true&ClientID=1&ClientTransactionID=2"
  // response { "ErrorNumber": 0, "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handleconnectedput:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"ErrorNumber\":0,";
  jsonretstr = jsonretstr + "\"ErrorMessage\":\"\"}";
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handleconnectedget()
{
  // GET "/api/v1/focuser/0/connected?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": true, "ErrorNumber": 0, "ErrorMessage": "string"}

  // Should we just return the value of ASCOMConnectedState?
  String jsonretstr = "";
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":1," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handlenameget()
{
  // curl -X GET "/api/v1/focuser/0/name?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": "string",  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handlenameget:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":" + String(ASCOMNAME) + "," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handledescriptionget()
{
  // GET "/api/v1/focuser/0/description?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": "string",  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handledescriptionget:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":" + String(ASCOMDESCRIPTION) + "," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handledriverinfoget()
{
  // curl -X GET "/api/v1/focuser/0/driverinfo?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": "string",  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handledescriptionget:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":" + String(ASCOMDRIVERINFO) + "," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handledriverversionget()
{
  // curl -X GET "/api/v1/focuser/0/driverversion?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": "string",  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handledriverversionget");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":\"" + String(programVersion) + "\"," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handleabsoluteget()
{
  // curl -X GET "/api/v1/focuser/0/absolute?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": true,  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handleabsoluteget");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  // should this be 1? - yes
  jsonretstr = "{\"Value\":1," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handlemaxstepget()
{
  // curl -X GET "/api/v1/focuser/0/maxstep?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": 0,  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handlemaxstepget");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":" + String(mySetupData->get_maxstep()) + "," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handlemaxincrementget()
{
  // curl -X GET "/api/v1/focuser/0/maxincrement?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": 0,  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handlemaxincrementget");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":" + String(mySetupData->get_maxstep()) + "," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handletemperatureget()
{
  // curl -X GET "/api/v1/focuser/0/temperature?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": 1.100000023841858,  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handletemperatureget");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
#ifdef TEMPERATUREPROBE
  jsonretstr = "{\"Value\":" + String(readtemp(0)) + "," + ASCOM_addclientinfo( jsonretstr );
#else
  jsonretstr = "{\"Value\":20.0," + ASCOM_addclientinfo( jsonretstr );
#endif
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void  ASCOM_handlepositionget()
{
  // curl -X GET "/api/v1/focuser/0/position?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": 0,  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handlepositionget");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":" + String(fcurrentPosition) + "," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void  ASCOM_handlehaltput()
{
  // curl -X PUT "/api/v1/focuser/0/halt" -H  "accept: application/json" -H  "Content-Type: application/x-www-form-urlencoded" -d "ClientID=22&ClientTransactionID=33"
  // { "ErrorNumber": 0, "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handlehaltput");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  ftargetPosition = fcurrentPosition;
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{" + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handleismovingget()
{
  // curl -X GET "/api/v1/focuser/0/ismoving?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": true,  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handleismovingget:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  if ( isMoving == 1 )
  {
    jsonretstr = "{\"Value\":1,"  + ASCOM_addclientinfo( jsonretstr );
  }
  else
  {
    jsonretstr = "{\"Value\":0," + ASCOM_addclientinfo( jsonretstr );
  }
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handlestepsizeget()
{
  // curl -X GET "/api/v1/focuser/0/stepsize?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": 1.100000023841858,  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handlestepsizeget:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  jsonretstr = "{\"Value\":" + String(mySetupData->get_stepsize()) + "," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handletempcompget()
{
  // curl -X GET "/api/v1/focuser/0/tempcomp?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": true,  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handletempcompget:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // The state of temperature compensation mode (if available), else always False.
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
  if ( mySetupData->get_tempcompenabled() == 0 )
  {
    jsonretstr = "{\"Value\":0," + ASCOM_addclientinfo( jsonretstr );
  }
  else
  {
    jsonretstr = "{\"Value\":1," + ASCOM_addclientinfo( jsonretstr );
  }
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handletempcompput()
{
  // curl -X PUT "/api/v1/focuser/0/tempcomp" -H  "accept: application/json" -H  "Content-Type: application/x-www-form-urlencoded" -d "TempComp=true&Client=1&ClientTransactionIDForm=12"
  // {  "ErrorNumber": 0,  "ErrorMessage": "string" }
  // look for parameter tempcomp=true or tempcomp=false
  String jsonretstr = "";
  DebugPrintln("ASCOM_handletempcompput:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
#ifdef TEMPERATUREPROBE
  if ( ASCOMTempCompState == 1 )
  {
    // turn on temperature compensation
    mySetupData->set_tempcompenabled(1);
  }
  else
  {
    // turn off temperature compensation
    mySetupData->set_tempcompenabled(0);
  }
  jsonretstr = "{" + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
#else
  ASCOMErrorNumber = ASCOMNOTIMPLEMENTED;
  ASCOMErrorMessage = ASCOMERRORNOTIMPLEMENTED;
  jsonretstr = "{" + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
#endif
}

void ASCOM_handletempcompavailableget()
{
  // curl -X GET "/api/v1/focuser/0/tempcompavailable?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": true,  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handletempcompavailableget:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();
  // addclientinfo adds clientid, clienttransactionid, servtransactionid, errornumber, errormessage and terminating }
#ifdef TEMPERATUREPROBE
  jsonretstr = "{\"Value\":1," + ASCOM_addclientinfo( jsonretstr );
#else
  jsonretstr = "{\"Value\":0," + ASCOM_addclientinfo( jsonretstr );
#endif
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void  ASCOM_handlemoveput()
{
  // curl -X PUT "/api/v1/focuser/0/move" -H  "accept: application/json" -H  "Content-Type: application/x-www-form-urlencoded" -d "Position=1000&ClientID=22&ClientTransactionID=33"
  // {  "ErrorNumber": 0,  "ErrorMessage": "string" }
  // extract new value
  String jsonretstr = "";
  DebugPrintln("ASCOM_handlemoveput:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  ASCOM_getURLParameters();         // get clientID and clienttransactionID

  // destination is in ASCOMpos
  // this is interfaceversion = 3, so moves are allowed when temperature compensation is on
  unsigned long newpos;
  DebugPrint("ASCOMpos: ");
  DebugPrintln(ASCOMpos);
  if ( ASCOMpos <= 0 )
  {
    newpos = 0;
    DebugPrint("new position: ");
    DebugPrintln(newpos);
    ftargetPosition = newpos;
    jsonretstr = "{" + ASCOM_addclientinfo( jsonretstr );
    ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
  }
  else
  {
    newpos = (unsigned long) ASCOMpos;
    if (newpos > mySetupData->get_maxstep() )
    {
      newpos = mySetupData->get_maxstep();
      ftargetPosition = newpos;
      DebugPrint("new position: ");
      DebugPrintln(newpos);
      jsonretstr = "{" + ASCOM_addclientinfo( jsonretstr );
      ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
    }
    else
    {
      ftargetPosition = newpos;
      DebugPrint("new position: ");
      DebugPrintln(newpos);
      jsonretstr = "{" + ASCOM_addclientinfo( jsonretstr );
      ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
    }
  }
}

void ASCOM_handlesupportedactionsget()
{
  // curl -X GET "/api/v1/focuser/0/supportedactions?ClientID=1&ClientTransactionID=1234" -H  "accept: application/json"
  // {  "Value": [    "string"  ],  "ErrorNumber": 0,  "ErrorMessage": "string" }
  String jsonretstr = "";
  DebugPrintln("ASCOM_handlesupportedactionsget:");
  ASCOMServerTransactionID++;
  ASCOMErrorNumber = 0;
  ASCOMErrorMessage = ASCOMERRORMSGNULL;
  // get clientID and clienttransactionID
  ASCOM_getURLParameters();
  jsonretstr = "{\"Value\": [\"isMoving\",\"MaxStep\",\"Temperature\",\"Position\",\"Absolute\",\"MaxIncrement\",\"StepSize\",\"TempComp\",\"TempCompAvailable\", ]," + ASCOM_addclientinfo( jsonretstr );
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handleNotFound()
{
  String message = "Not Found: ";
  String jsonretstr = "";
  DebugPrintln("ASCOM_handleNotFound:");
  message += "URI: ";
  message += ascomserver.uri();
  message += "\nMethod: ";
  if ( ascomserver.method() == HTTP_GET )
  {
    message += "GET";
  }
  else if ( ascomserver.method() == HTTP_POST )
  {
    message += "POST";
  }
  else if ( ascomserver.method() == HTTP_PUT )
  {
    message += "PUT";
  }
  else if ( ascomserver.method() == HTTP_DELETE )
  {
    message += "DELETE";
  }
  else
  {
    message += "UNKNOWN_METHOD: " + ascomserver.method();
  }
  message += "\nArguments: ";
  message += ascomserver.args();
  message += "\n";
  for (uint8_t i = 0; i < ascomserver.args(); i++)
  {
    message += " " + ascomserver.argName(i) + ": " + ascomserver.arg(i) + "\n";
  }
  DebugPrint("Error: ");
  DebugPrintln(message);
  ASCOMErrorNumber  = ASCOMNOTIMPLEMENTED;
  ASCOMErrorMessage = ASCOMERRORNOTIMPLEMENTED;
  ASCOMServerTransactionID++;
  jsonretstr = "{" + ASCOM_addclientinfo( jsonretstr );
  ASCOM_sendreply( BADREQUESTWEBPAGE, JSONPAGETYPE, jsonretstr);
  delay(10);                     // small pause so background tasks can run
}

void ASCOM_handleRoot()
{
  ASCOMServerTransactionID++;
  ASCOM_sendreply( NORMALWEBPAGE, TEXTPAGETYPE, Setup_HomePage);
  delay(10);                     // small pause so background tasks can run
}

void start_ascomremoteserver(void)
{
  ASCOM_Setup_HomePage();                       // create home page for ascom focuser setup
  ASCOM_Create_Setup_Focuser_HomePage();        // create home page for ascom management setup api
  
  ascomserver.on("/", ASCOM_handleRoot);        // handle root access
  ascomserver.onNotFound(ASCOM_handleNotFound); // handle url not found 404
  
  ascomserver.on("/management/apiversions", ASCOM_handleapiversions);
  ascomserver.on("/management/v1/description", ASCOM_handleapidescription);
  ascomserver.on("/management/v1/configureddevices", ASCOM_handleapiconfigureddevices);

  ascomserver.on("/setup", ASCOM_handle_setup);
  ascomserver.on("/setup/v1/focuser/0/setup", ASCOM_handle_focuser_setup);

  ascomserver.on("/api/v1/focuser/0/connected", HTTP_PUT, ASCOM_handleconnectedput);
  ascomserver.on("/api/v1/focuser/0/interfaceversion", HTTP_GET, ASCOM_handleinterfaceversionget);
  ascomserver.on("/api/v1/focuser/0/name", HTTP_GET, ASCOM_handlenameget);
  ascomserver.on("/api/v1/focuser/0/description", HTTP_GET, ASCOM_handledescriptionget);
  ascomserver.on("/api/v1/focuser/0/driverinfo", HTTP_GET, ASCOM_handledriverinfoget);
  ascomserver.on("/api/v1/focuser/0/driverversion", HTTP_GET, ASCOM_handledriverversionget);
  ascomserver.on("/api/v1/focuser/0/absolute", HTTP_GET, ASCOM_handleabsoluteget);
  ascomserver.on("/api/v1/focuser/0/maxstep", HTTP_GET, ASCOM_handlemaxstepget);
  ascomserver.on("/api/v1/focuser/0/maxincrement", HTTP_GET, ASCOM_handlemaxincrementget);
  ascomserver.on("/api/v1/focuser/0/temperature", HTTP_GET, ASCOM_handletemperatureget);
  ascomserver.on("/api/v1/focuser/0/position", HTTP_GET, ASCOM_handlepositionget);
  ascomserver.on("/api/v1/focuser/0/halt", HTTP_PUT, ASCOM_handlehaltput);
  ascomserver.on("/api/v1/focuser/0/ismoving", HTTP_GET, ASCOM_handleismovingget);
  ascomserver.on("/api/v1/focuser/0/stepsize", HTTP_GET, ASCOM_handlestepsizeget);
  ascomserver.on("/api/v1/focuser/0/connected", HTTP_GET, ASCOM_handleconnectedget);
  ascomserver.on("/api/v1/focuser/0/tempcomp", HTTP_GET, ASCOM_handletempcompget);
  ascomserver.on("/api/v1/focuser/0/tempcomp", HTTP_PUT, ASCOM_handletempcompput);
  ascomserver.on("/api/v1/focuser/0/tempcompavailable", HTTP_GET, ASCOM_handletempcompavailableget);
  ascomserver.on("/api/v1/focuser/0/move", HTTP_PUT, ASCOM_handlemoveput);
  ascomserver.on("/api/v1/focuser/0/supportedactions", HTTP_GET, ASCOM_handlesupportedactionsget);
  ascomserver.begin();
}
#endif // ifdef ASCOMREMOTE
// ASCOM REMOTE END ---------------------------------------------------------------------------------


#endif // ifndef ascomserver_h
