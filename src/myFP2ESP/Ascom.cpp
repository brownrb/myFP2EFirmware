// ---------------------------------------------------------------------------
// myFP2ESP ASCOM SERVER ROUTINES
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// COPYRIGHT
// ---------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2021. All Rights Reserved.
// (c) Copyright Holger M, 2019-2021. All Rights Reserved.
// ---------------------------------------------------------------------------

#include "generalDefinitions.h"
#include "FocuserSetupData.h"
#include "myBoards.h"
#include "temp.h"

// ----------------------------------------------------------------------------------------------
// 24: ASCOMSERVER - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------

#include <WiFiServer.h>
#include <WiFiClient.h>

extern SetupData *mySetupData;
extern DriverBoard* driverboard;
extern unsigned long ftargetPosition;       // target position
extern volatile bool halt_alert;
extern char   ipStr[]; 
extern byte   isMoving;                     // is the motor currently moving
extern String MSpg;
extern bool   ascomserverstate;
extern bool   ascomdiscoverystate;
extern float  lasttemp;

#if defined(ESP8266)                        // this "define(ESP8266)" comes from Arduino IDE
#undef DEBUG_ESP_HTTP_SERVER                // prevent messages from WiFiServer 
#include <ESP8266WiFi.h>
#include <FS.h>                             // include the SPIFFS library  
#else                                       // otherwise assume ESP32
#include <WiFi.h>
#include "SPIFFS.h"
#endif
#include <SPI.h>

#if defined(ESP8266)
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif // if defined(esp8266)

#if defined(ESP8266)
#undef DEBUG_ESP_HTTP_SERVER
extern ESP8266WebServer mserver;
#else
extern WebServer mserver;
#endif // if defined(esp8266)

#if defined(ESP8266)
#include <ESP8266WebServer.h>
#else
#include "webserver.h"
#endif // if defined(esp8266)

#include "ascomserver.h"
// Implement ASCOM ALPACA DISCOVERY PROTOCOL
#include <WiFiUdp.h>
WiFiUDP ASCOMDISCOVERYUdp;
char packetBuffer[255];                                 // buffer to hold incoming UDP packet

String ASpg;                                            // url:/setup/v1/focuser/0/setup
unsigned int ASCOMClientID;
unsigned int ASCOMClientTransactionID;
unsigned int ASCOMServerTransactionID = 0;
int          ASCOMErrorNumber = 0;
String       ASCOMErrorMessage = "";
long         ASCOMpos = 0L;
byte         ASCOMTempCompState = 0;
byte         ASCOMConnectedState = 0;
WiFiClient   ascomclient;

#if defined(ESP8266)
ESP8266WebServer *ascomserver;
#else
WebServer *ascomserver;
#endif // if defined(esp8266)
void ASCOM_sendmyheader(void)
{
  //ascomserver->sendHeader(F(CACHECONTROLSTR), F(NOCACHENOSTORESTR));
  //ascomserver->sendHeader(F(PRAGMASTR), F(NOCACHESTR));
  //ascomserver->sendHeader(F(EXPIRESSTR), "-1");
  //ascomserver->setContentLength(CONTENT_LENGTH_UNKNOWN);
  //ascomserver->send(NORMALWEBPAGE, F(TEXTPAGETYPE), "");
  ascomserver->client().println("HTTP/1.1 200 OK");
  ascomserver->client().println("Content-type:text/html");
  //ascomserver->client().println("Connection: close");       // only valid on http/1.0
  ascomserver->client().println();
}

void ASCOM_sendmycontent()
{
  ascomserver->client().print(ASpg);
}

// ASCOM ALPCACA REMOTE DISCOVERY
void checkASCOMALPACADiscovery(void)
{
  // (c) Daniel VanNoord
  // https://github.com/DanielVanNoord/AlpacaDiscoveryTests/blob/master/Alpaca8266/Alpaca8266.ino
  // if there's data available, read a packet
  int packetSize = ASCOMDISCOVERYUdp.parsePacket();
  if (packetSize)
  {
    DebugPrint("ASCOM ALPACA Discovery: Rcd packet size: ");
    DebugPrintln(packetSize);
    DebugPrint("From ");
    IPAddress remoteIp = ASCOMDISCOVERYUdp.remoteIP();
    DebugPrint(remoteIp);
    DebugPrint(", on port ");
    DebugPrintln(ASCOMDISCOVERYUdp.remotePort());

    // read the packet into packetBufffer
    int len = ASCOMDISCOVERYUdp.read(packetBuffer, 255);
    if (len > 0)
    {
      packetBuffer[len] = 0;                          // Ensure that it is null terminated
    }
    DebugPrint("Contents: ");
    DebugPrintln(packetBuffer);

    if (len < 16)                                     // No undersized packets allowed
    {
      DebugPrintln(F("Packet is undersized"));
      return;
    }

    if (strncmp("alpacadiscovery1", packetBuffer, 16) != 0)    // 0-14 "alpacadiscovery", 15 ASCII Version number of 1
    {
      DebugPrintln(F("Packet is not correct format"));
      return;
    }

    String strresponse = "{\"alpacaport\":" + String(mySetupData->get_ascomalpacaport()) + "}";
    uint8_t response[36] = { 0 };
    len = strresponse.length();
    DebugPrintln("Response : " + strresponse);
    // copy to response
    for ( int i = 0; i < len; i++ )
    {
      response[i] = (uint8_t) strresponse[i];
    }

    ASCOMDISCOVERYUdp.beginPacket(ASCOMDISCOVERYUdp.remoteIP(), ASCOMDISCOVERYUdp.remotePort());
#if defined(ESP8266)
    ASCOMDISCOVERYUdp.write(response, len);
#else
    ASCOMDISCOVERYUdp.write(response, len);
#endif
    ASCOMDISCOVERYUdp.endPacket();
  }
}

// Construct pages for /setup

// constructs ascom setup server page /setup/v1/focuser/0/setup
void ASCOM_Create_Setup_Focuser_HomePage()
{
#ifdef TIMEASCOMBUILDSETUP
  Serial.print("ascombuildsetup() : ");
  Serial.println(millis());
#endif
  // Convert IP address to a string;
  // already in ipStr
  // convert current values of focuserposition and focusermaxsteps to string types
  String fpbuffer = String(driverboard->getposition());
  String mxbuffer = String(mySetupData->get_maxstep());
  String smbuffer = String(mySetupData->get_stepmode());
  int    eflag    = 0;

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

  // construct setup page of ascom server
  // header
  if ( SPIFFS.begin())
  {
    if ( SPIFFS.exists("/assetup.html"))
    {
      File file = SPIFFS.open("/assetup.html", "r");    // open file for read
      DebugPrintln(READPAGESTR);                     // read contents into string
      ASpg = file.readString();
      file.close();

      DebugPrintln(PROCESSPAGESTARTSTR);
      // process for dynamic data
      String bcol = mySetupData->get_wp_backcolor();
      ASpg.replace("%BKC%", bcol);
      String txtcol = mySetupData->get_wp_textcolor();
      ASpg.replace("%TXC%", txtcol);
      String ticol = mySetupData->get_wp_titlecolor();
      ASpg.replace("%TIC%", ticol);
      String hcol = mySetupData->get_wp_headercolor();
      ASpg.replace("%HEC%", hcol);
      ASpg.replace("%PRN%", String(DRVBRD_ID));
      ASpg.replace("%IPS%", ipStr);
      ASpg.replace("%ALP%", String(mySetupData->get_ascomalpacaport()));
      ASpg.replace("%PRV%", String(programVersion));
      ASpg.replace("%FPB%", fpbuffer);
      ASpg.replace("%MXB%", mxbuffer);
      ASpg.replace("%CPB%", cpbuffer);
      ASpg.replace("%RDB%", rdbuffer);
      ASpg.replace("%SMB%", smbuffer);
      ASpg.replace("%MSB%", msbuffer);
      DebugPrintln(PROCESSPAGEENDSTR);
      eflag = 0;
    }
    else
    {
      eflag = 1;                                        // assetup.html not found
    }
  }
  else
  {
    // spiffs not started
    eflag = 1;
  }
  if ( eflag == 1 )
  {
    // SPIFFS FILE NOT FOUND
    ASpg = "<head>" + String(AS_PAGETITLE) + "</head><body>";
    ASpg = ASpg + String(AS_TITLE);

    ASpg = ASpg + String(AS_COPYRIGHT);
    ASpg = ASpg + "<p>Driverboard = myFP2ESP." + DRVBRD_ID + "<br>";
    ASpg = ASpg + "<myFP2ESP." + DRVBRD_ID + "</h3>IP Address: " + ipStr + ", Firmware Version=" + String(programVersion) + "</br>";

    // position. set position
    ASpg = ASpg + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><br><b>Focuser Position</b> <input type=\"text\" name=\"fp\" size =\"15\" value=" + fpbuffer + "> ";
    ASpg = ASpg + "<input type=\"submit\" name=\"setpos\" value=\"Set Pos\"> </form></p>";

    // maxstep
    ASpg = ASpg + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>MaxSteps</b> <input type=\"text\" name=\"fm\" size =\"15\" value=" + mxbuffer + "> ";
    ASpg = ASpg + "<input type=\"submit\" value=\"Submit\"></form>";

    // coilpower
    ASpg = ASpg + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>Coil Power </b>" + cpbuffer;
    ASpg = ASpg + "<input type=\"hidden\" name=\"cp\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

    // reverse direction
    ASpg = ASpg + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>Reverse Direction </b>" + rdbuffer;
    ASpg = ASpg + "<input type=\"hidden\" name=\"rd\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

    // stepmode
    ASpg = ASpg + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>Step Mode </b>" + smbuffer + " ";
    ASpg = ASpg + "<input type=\"hidden\" name=\"sm\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

    // motor speed
    ASpg = ASpg + "<form action=\"/setup/v1/focuser/0/setup\" method=\"post\" ><b>Motor Speed: </b>" + msbuffer + " ";
    ASpg = ASpg + "<input type=\"hidden\" name=\"ms\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

    ASpg = ASpg + "</body></html>\r\n";
  }
#ifdef TIMEASCOMBUILDSETUP
  Serial.print("ascombuildsetup() : ");
  Serial.println(millis());
#endif
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
  ascomserver->send(replycode, contenttype, jsonstr );
}

void ASCOM_getURLParameters()
{
  String str;
  // get server args, translate server args to lowercase, they can be mixed case
  DebugPrintln("ASCOM_getURLParameters START");
  DebugPrint("Number of args:");
  DebugPrintln(ascomserver->args());
  for (int i = 0; i < ascomserver->args(); i++)
  {
    if ( i >= ASCOMMAXIMUMARGS )
    {
      break;
    }
    str = ascomserver->argName(i);
    str.toLowerCase();
    DebugPrint("Parameter Found: ");
    DebugPrintln(str);
    if ( str.equals("clientid") )
    {
      ASCOMClientID = (unsigned int) ascomserver->arg(i).toInt();
      DebugPrint("clientID:");
      DebugPrintln(ASCOMClientID);
    }
    if ( str.equals("clienttransactionid") )
    {
      ASCOMClientTransactionID = (unsigned int) ascomserver->arg(i).toInt();
      DebugPrint("clienttransactionid:");
      DebugPrintln(ASCOMClientTransactionID);
    }
    if ( str.equals("tempcomp") )
    {
      String strtmp = ascomserver->arg(i);
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
      String str1 = ascomserver->arg(i);
      DebugPrint("ASCOMpos RAW:");
      DebugPrintln(str1);
      ASCOMpos = ascomserver->arg(i).toInt();           // this returns a long data type
      DebugPrint("ASCOMpos:");
      DebugPrintln(ASCOMpos);
    }
    if ( str.equals("connected") )
    {
      String strtmp = ascomserver->arg(i);
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

// ---------------------------------------------------------------------------
// Setup functions
// ---------------------------------------------------------------------------
void ASCOM_handle_setup()
{
#ifdef TIMEASCOMHANDLESETUP
  Serial.print("ascomhandlesetup() : ");
  Serial.println(millis());
#endif
  // url /setup
  // The web page must describe the overall device, including name, manufacturer and version number.
  // content-type: text/html
  String ASpg;
  // spiffs was started earlier when server was started so assume it has started
  if ( SPIFFS.exists("/ashomepage.html"))               // read ashomepage.html from FS
  {
    File file = SPIFFS.open("/ashomepage.html", "r");   // open file for read
    DebugPrintln(READPAGESTR);
    ASpg = file.readString();                           // read contents into string
    file.close();

    DebugPrintln(PROCESSPAGESTARTSTR);
    // process for dynamic data
    String bcol = mySetupData->get_wp_backcolor();
    ASpg.replace("%BKC%", bcol);
    String txtcol = mySetupData->get_wp_textcolor();
    ASpg.replace("%TXC%", txtcol);
    String ticol = mySetupData->get_wp_titlecolor();
    ASpg.replace("%TIC%", ticol);
    String hcol = mySetupData->get_wp_headercolor();
    ASpg.replace("%HEC%", hcol);
    ASpg.replace("%IPS%", ipStr);
    ASpg.replace("%ALP%", String(mySetupData->get_ascomalpacaport()));
    ASpg.replace("%PRV%", String(programVersion));
    ASpg.replace("%PRN%", String(DRVBRD_ID));
    DebugPrintln(PROCESSPAGEENDSTR);
  }
  else
  {
    DebugPrintln(F("ascomserver: Error file ashomepage.html not found"));
    DebugPrintln("ascomserver: build_default_homepage");
    ASpg = ASCOMSERVERNOTFOUNDSTR;
  }
  ASCOMServerTransactionID++;
  ASCOM_sendmyheader();
  ASCOM_sendmycontent();
  ASpg = "";
#ifdef TIMEASCOMHANDLESETUP
  Serial.print("ascomhandlesetup() : ");
  Serial.println(millis());
#endif
  delay(10);                                            // small pause so background tasks can run
}

void ASCOM_handle_focuser_setup()
{
#ifdef TIMEASCOMHANDLEFOCUSERSETUP
  Serial.print("ascomhandlefocusersetup() : ");
  Serial.println(millis());
#endif
  // url /setup/v1/focuser/0/setup
  // Configuration web page for the specified device
  // content-type: text/html

  // if set focuser position
  String fpos_str = ascomserver->arg("setpos");
  if ( fpos_str != "" )
  {
    DebugPrint("setpos:");
    DebugPrintln(fpos_str);
    String fp = ascomserver->arg("fp");
    if ( fp != "" )
    {
      long temp = 0;
      DebugPrint("fp:");
      DebugPrintln(fp);
      temp = fp.toInt();
      temp = ( temp < 0) ? 0 : temp;
      temp = ( temp > (long)mySetupData->get_maxstep()) ? (long) mySetupData->get_maxstep() : temp;
      ftargetPosition = (unsigned long) temp;
      driverboard->setposition(ftargetPosition);
      mySetupData->set_fposition(ftargetPosition);
    }
  }

  // if update of maxsteps
  String fmax_str = ascomserver->arg("fm");
  if ( fmax_str != "" )
  {
    long temp = 0;
    DebugPrint("root() -maxsteps:");
    DebugPrintln(fmax_str);
    temp = fmax_str.toInt();
    if ( temp < (long) driverboard->getposition() )               // if maxstep is less than focuser position
    {
      temp = (long) driverboard->getposition() + 1;
    }
    if ( temp < FOCUSERLOWERLIMIT )                     // do not set it less then 1024
    {
      temp = FOCUSERLOWERLIMIT;
    }
    if ( temp > (long) mySetupData->get_maxstep() )     // if higher than max value
    {
      temp = (long) mySetupData->get_maxstep();
    }
    mySetupData->set_maxstep((unsigned long) temp);
  }

  // if update motorspeed
  String fms_str = ascomserver->arg("ms");
  if ( fms_str != "" )
  {
    int temp1 = 0;
    DebugPrint("root() -motorspeed:");
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
  String fcp_str = ascomserver->arg("cp");
  if ( fcp_str != "" )
  {
    DebugPrint("root() -coil power:");
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
  String frd_str = ascomserver->arg("rd");
  if ( frd_str != "" )
  {
    DebugPrint("root() -reverse direction:");
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
  // (1=Full, 2=Half, 4=1/4, 8=1/8, 16=1/16, 32=1/32, 64=1/64, 128=1/128, 256=1/256)
  String fsm_str = ascomserver->arg("sm");
  if ( fsm_str != "" )
  {
    int temp1 = 0;
    DebugPrint("root() -stepmode:");
    DebugPrintln(fsm_str);
    temp1 = fsm_str.toInt();
    if ( temp1 < STEP1 )
    {
      temp1 = STEP1;
    }
    if ( temp1 > STEP256 )
    {
      temp1 = STEP256;
    }
    mySetupData->set_stepmode(temp1);
  }

  DebugPrintln( "root() -build homepage");

  ASCOM_Create_Setup_Focuser_HomePage();                // construct the homepage now

  // send the homepage to a connected client
  ASCOMServerTransactionID++;
  DebugPrintln("root() - send homepage");
  ASCOM_sendmyheader();
  ASCOM_sendmycontent();
  ASpg = "";
#ifdef TIMEASCOMHANDLEFOCUSERSETUP
  Serial.print("ascomhandlefocusersetup() : ");
  Serial.println(millis());
#endif
  delay(10);                                            // small pause so background ESP8266 tasks can run
}

// ---------------------------------------------------------------------------
// Management API functions
// ---------------------------------------------------------------------------
void ASCOM_handleapiversions()
{
#ifdef TIMEASCOMHANDLEAPIVER
  Serial.print("ascomhandleapiver() : ");
  Serial.println(millis());
#endif
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
#ifdef TIMEASCOMHANDLEAPIVER
  Serial.print("ascomhandleapiver() : ");
  Serial.println(millis());
#endif
}

void ASCOM_handleapidescription()
{
#ifdef TIMEASCOMHANDLEAPICON
  Serial.print("ascomhandleapicon() : ");
  Serial.println(millis());
#endif
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
#ifdef TIMEASCOMHANDLEAPICON
  Serial.print("ascomhandleapicon() : ");
  Serial.println(millis());
#endif
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
  jsonretstr = "{\"Value\":[{\"DeviceName\":" + String(ASCOMNAME) + ",\"DeviceType\":\"focuser\",\"DeviceNumber\":0,\"UniqueID\":\"" + String(ASCOMGUID) + "\"}]," + ASCOM_addclientinfo( jsonretstr );
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

// ---------------------------------------------------------------------------
// ASCOM ALPACA API
// ---------------------------------------------------------------------------
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
  if ( mySetupData->get_temperatureprobestate() == 1 )
  {
    jsonretstr = "{\"Value\":" + String(lasttemp, 2) + "," + ASCOM_addclientinfo( jsonretstr );
  }
  else
  {
    jsonretstr = "{\"Value\":20.0," + ASCOM_addclientinfo( jsonretstr );
  }
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
  jsonretstr = "{\"Value\":" + String(driverboard->getposition()) + "," + ASCOM_addclientinfo( jsonretstr );
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
  halt_alert = true;;
  //ftargetPosition = fcurrentPosition;
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
  if ( mySetupData->get_temperatureprobestate() == 1)
  {
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
  }
  else
  {
    ASCOMErrorNumber = ASCOMNOTIMPLEMENTED;
    ASCOMErrorMessage = ASCOMERRORNOTIMPLEMENTED;
    jsonretstr = "{" + ASCOM_addclientinfo( jsonretstr );
    // sendreply builds http header, sets content type, and then sends jsonretstr
    ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
  }
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
  if ( mySetupData->get_temperatureprobestate() == 1 )
  {
    jsonretstr = "{\"Value\":1," + ASCOM_addclientinfo( jsonretstr );
  }
  else
  {
    jsonretstr = "{\"Value\":0," + ASCOM_addclientinfo( jsonretstr );
  }
  // sendreply builds http header, sets content type, and then sends jsonretstr
  ASCOM_sendreply( NORMALWEBPAGE, JSONPAGETYPE, jsonretstr);
}

void ASCOM_handlemoveput()
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
  message += ascomserver->uri();
  message += "\nMethod: ";
  if ( ascomserver->method() == HTTP_GET )
  {
    message += "GET";
  }
  else if ( ascomserver->method() == HTTP_POST )
  {
    message += "POST";
  }
  else if ( ascomserver->method() == HTTP_PUT )
  {
    message += "PUT";
  }
  else if ( ascomserver->method() == HTTP_DELETE )
  {
    message += "DELETE";
  }
  else
  {
    message += "UNKNOWN_METHOD: " + ascomserver->method();
  }
  message += "\nArguments: ";
  message += ascomserver->args();
  message += "\n";
  for (uint8_t i = 0; i < ascomserver->args(); i++)
  {
    message += " " + ascomserver->argName(i) + ": " + ascomserver->arg(i) + "\n";
  }
  DebugPrint("Error: ");
  DebugPrintln(message);
  ASCOMErrorNumber  = ASCOMNOTIMPLEMENTED;
  ASCOMErrorMessage = ASCOMERRORNOTIMPLEMENTED;
  ASCOMServerTransactionID++;
  jsonretstr = "{" + ASCOM_addclientinfo( jsonretstr );
  ASCOM_sendreply( BADREQUESTWEBPAGE, JSONPAGETYPE, jsonretstr);
  delay(10);                                            // small pause so background tasks can run
}

void ASCOM_handleRoot()
{
  String ASpg;
  // spiffs was started earlier when server was started so assume it has started
  if ( SPIFFS.exists("/ashomepage.html"))               // read ashomepage.html from FS
  {
    DebugPrintln("ascomserver: ashomepage.html found");
    File file = SPIFFS.open("/ashomepage.html", "r");   // open file for read
    DebugPrintln("ascomserver: read page into string");
    ASpg = file.readString();                           // read contents into string
    file.close();

    DebugPrintln("ascomserver: processing page start");
    // process for dynamic data
    String bcol = mySetupData->get_wp_backcolor();
    MSpg.replace("%BKC%", bcol);
    String txtcol = mySetupData->get_wp_textcolor();
    MSpg.replace("%TXC%", txtcol);
    String ticol = mySetupData->get_wp_titlecolor();
    MSpg.replace("%TIC%", ticol);
    String hcol = mySetupData->get_wp_headercolor();
    MSpg.replace("%HEC%", hcol);
    ASpg.replace("%IPS%", ipStr);
    ASpg.replace("%ALP%", String(mySetupData->get_ascomalpacaport()));
    ASpg.replace("%PRV%", String(programVersion));
    ASpg.replace("%PRN%", String(DRVBRD_ID));
    DebugPrintln("ascomserver: processing page done");
  }
  else
  {
    DebugPrintln(F("ascomserver: Error occurred finding SPIFFS file ashomepage.html"));
    DebugPrintln("ascomserver: build_default_homepage");
    ASpg = ASCOMSERVERNOTFOUNDSTR;
  }
  ASCOMServerTransactionID++;
  ASCOM_sendreply( NORMALWEBPAGE, TEXTPAGETYPE, ASpg);
  delay(10);                                            // small pause so background tasks can run
}

void start_ascomremoteserver(void)
{
  if ( !SPIFFS.begin() )
  {
    TRACE();
    DebugPrintln(F(FSNOTSTARTEDSTR));
    DebugPrintln(F(SERVERSTATESTOPSTR));
    ascomserverstate = STOPPED;
    return;
  }
  ASpg.reserve(MAXASCOMPAGESIZE);
  HDebugPrint("Heap before start_ascomremoteserver = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());
  DebugPrintln("start ascom server");

#if defined(ESP8266)
  ascomserver = new ESP8266WebServer(mySetupData->get_ascomalpacaport());
#else
  ascomserver = new WebServer(mySetupData->get_ascomalpacaport());
#endif // if defined(esp8266) 

  if ( ascomdiscoverystate == STOPPED )
  {
    ASCOMDISCOVERYUdp.begin(ASCOMDISCOVERYPORT);
    ascomdiscoverystate = RUNNING;
  }
  ascomserver->on("/", ASCOM_handleRoot);               // handle root access
  ascomserver->onNotFound(ASCOM_handleNotFound);        // handle url not found 404

  ascomserver->on("/management/apiversions",              ASCOM_handleapiversions);
  ascomserver->on("/management/v1/description",           ASCOM_handleapidescription);
  ascomserver->on("/management/v1/configureddevices",     ASCOM_handleapiconfigureddevices);

  ascomserver->on("/setup",                               ASCOM_handle_setup);
  ascomserver->on("/setup/v1/focuser/0/setup",            ASCOM_handle_focuser_setup);

  ascomserver->on("/api/v1/focuser/0/connected",          HTTP_PUT, ASCOM_handleconnectedput);
  ascomserver->on("/api/v1/focuser/0/interfaceversion",   HTTP_GET, ASCOM_handleinterfaceversionget);
  ascomserver->on("/api/v1/focuser/0/name",               HTTP_GET, ASCOM_handlenameget);
  ascomserver->on("/api/v1/focuser/0/description",        HTTP_GET, ASCOM_handledescriptionget);
  ascomserver->on("/api/v1/focuser/0/driverinfo",         HTTP_GET, ASCOM_handledriverinfoget);
  ascomserver->on("/api/v1/focuser/0/driverversion",      HTTP_GET, ASCOM_handledriverversionget);
  ascomserver->on("/api/v1/focuser/0/absolute",           HTTP_GET, ASCOM_handleabsoluteget);
  ascomserver->on("/api/v1/focuser/0/maxstep",            HTTP_GET, ASCOM_handlemaxstepget);
  ascomserver->on("/api/v1/focuser/0/maxincrement",       HTTP_GET, ASCOM_handlemaxincrementget);
  ascomserver->on("/api/v1/focuser/0/temperature",        HTTP_GET, ASCOM_handletemperatureget);
  ascomserver->on("/api/v1/focuser/0/position",           HTTP_GET, ASCOM_handlepositionget);
  ascomserver->on("/api/v1/focuser/0/halt",               HTTP_PUT, ASCOM_handlehaltput);
  ascomserver->on("/api/v1/focuser/0/ismoving",           HTTP_GET, ASCOM_handleismovingget);
  ascomserver->on("/api/v1/focuser/0/stepsize",           HTTP_GET, ASCOM_handlestepsizeget);
  ascomserver->on("/api/v1/focuser/0/connected",          HTTP_GET, ASCOM_handleconnectedget);
  ascomserver->on("/api/v1/focuser/0/tempcomp",           HTTP_GET, ASCOM_handletempcompget);
  ascomserver->on("/api/v1/focuser/0/tempcomp",           HTTP_PUT, ASCOM_handletempcompput);
  ascomserver->on("/api/v1/focuser/0/tempcompavailable",  HTTP_GET, ASCOM_handletempcompavailableget);
  ascomserver->on("/api/v1/focuser/0/move",               HTTP_PUT, ASCOM_handlemoveput);
  ascomserver->on("/api/v1/focuser/0/supportedactions",   HTTP_GET, ASCOM_handlesupportedactionsget);
  ascomserver->begin();
  ascomserverstate = RUNNING;
  DebugPrintln(F("start ascom server: RUNNING"));
  HDebugPrint("Heap after  start_ascomremoteserver = ");
  HDebugPrintf("%u\n", ESP.getFreeHeap());
  delay(10);                                            // small pause so background tasks can run
}

void stop_ascomremoteserver(void)
{
  if ( ascomserverstate == RUNNING )
  {
    DebugPrintln("stop ascom server");
    ascomserver->close();
    delete ascomserver;                                 // free the ascomserver pointer and associated memory/code
    ascomserverstate = STOPPED;
    ASCOMDISCOVERYUdp.stop();                           // stop discovery service
  }
  else
  {
    DebugPrintln(SERVERNOTRUNNINGSTR);
  }

  if ( ascomdiscoverystate == STOPPED )
  {
    ASCOMDISCOVERYUdp.stop();
    ascomdiscoverystate = STOPPED;
  }
  else
  {
    DebugPrintln(SERVERNOTRUNNINGSTR);
  }
  delay(10);                                            // small pause so background tasks can run
}
// ASCOM REMOTE END ----------------------------------------------------------
