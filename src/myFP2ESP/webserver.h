// ----------------------------------------------------------------------------------------------
// myFP2ESP WEB SERVER ROUTINES AND DEFINITIONS
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2019. All Rights Reserved.
// (c) Copyright Holger M, 2019. All Rights Reserved.S
// ----------------------------------------------------------------------------------------------

#ifndef webserver_h
#define webserver_h

// ----------------------------------------------------------------------------------------------
// EXTERNALS
// ----------------------------------------------------------------------------------------------
extern SetupData *mySetupData;
extern unsigned long fcurrentPosition;         // current focuser position
extern unsigned long ftargetPosition;          // target position
extern String ipStr;                           // ip address
extern char programVersion[];
extern String programName;
extern byte isMoving;
#ifdef TEMPERATUREPROBE
extern float readtemp(byte);
#endif

// ----------------------------------------------------------------------------------------------
// DATA
// ----------------------------------------------------------------------------------------------
#ifdef WEBSERVER
#define WS_SM1CHECKED             "<input type=\"radio\" name=\"sm\" value=\"1\" Checked> Full"
#define WS_SM1UNCHECKED           "<input type=\"radio\" name=\"sm\" value=\"1\"> Full"
#define WS_SM2CHECKED             "<input type=\"radio\" name=\"sm\" value=\"2\" Checked> 1/2"
#define WS_SM2UNCHECKED           "<input type=\"radio\" name=\"sm\" value=\"2\"> 1/2"
#define WS_SM4CHECKED             "<input type=\"radio\" name=\"sm\" value=\"4\" Checked> 1/4"
#define WS_SM4UNCHECKED           "<input type=\"radio\" name=\"sm\" value=\"4\"> 1/4"
#define WS_SM8CHECKED             "<input type=\"radio\" name=\"sm\" value=\"8\" Checked> 1/8"
#define WS_SM8UNCHECKED           "<input type=\"radio\" name=\"sm\" value=\"8\"> 1/8"
#define WS_SM16CHECKED            "<input type=\"radio\" name=\"sm\" value=\"16\" Checked> 1/16"
#define WS_SM16UNCHECKED          "<input type=\"radio\" name=\"sm\" value=\"16\"> 1/16"
#define WS_SM32CHECKED            "<input type=\"radio\" name=\"sm\" value=\"32\" Checked> 1/32"
#define WS_SM32UNCHECKED          "<input type=\"radio\" name=\"sm\" value=\"32\"> 1/32"

#define WS_MSSLOWCHECKED          "<input type=\"radio\" name=\"ms\" value=\"0\" Checked> Slow"
#define WS_MSSLOWUNCHECKED        "<input type=\"radio\" name=\"ms\" value=\"0\"> Slow"
#define WS_MSMEDCHECKED           "<input type=\"radio\" name=\"ms\" value=\"1\" Checked> Medium"
#define WS_MSMEDUNCHECKED         "<input type=\"radio\" name=\"ms\" value=\"1\"> Medium"
#define WS_MSFASTCHECKED          "<input type=\"radio\" name=\"ms\" value=\"2\" Checked> Fast"
#define WS_MSFASTUNCHECKED        "<input type=\"radio\" name=\"ms\" value=\"2\"> Fast"

#define WS_COPYRIGHT              "<p>(c) R. Brown, Holger M, 2019. All rights reserved.</p>"
#define WS_TITLE                  "<h3>myFP2ESP Web based focus controller</h3>"
#define WS_PAGETITLE              "<title>myFP2ESP WEB SERVER</title>"

String HomePage;
String NotFoundPage;
#if defined(ESP8266)
ESP8266WebServer webserver(WEBSERVERPORT);
#else
WebServer webserver(WEBSERVERPORT);
#endif // if defined(esp8266)

#endif // ifdef webserver

// ----------------------------------------------------------------------------------------------
// CODE
// ----------------------------------------------------------------------------------------------
#ifdef WEBSERVER
void setNotFoundPage()
{
  NotFoundPage = "<html><head></head>";
  NotFoundPage = NotFoundPage + String(WS_PAGETITLE) + "</head>";
  NotFoundPage = NotFoundPage + "<body>" + String(WS_TITLE);
  NotFoundPage = NotFoundPage + String(WS_COPYRIGHT);
  NotFoundPage = NotFoundPage + "<p>IP Address: " + ipStr + ": Port: " + String(WEBSERVERPORT) + ", Firmware Version=" + String(programVersion) + "</p>";
  NotFoundPage = NotFoundPage + "<p>Driverboard = myFP2ESP." + programName + "</p>";
  NotFoundPage = NotFoundPage + "<p>The requested URL was not found</p>";
  NotFoundPage = NotFoundPage + "</body></html>";
}

void WEBSERVER_handleNotFound()
{
  webserver.send(NOTFOUNDWEBPAGE, TEXTPAGETYPE, NotFoundPage);
}

// constructs home page of web server
void SetHomePage()
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
      smbuffer = WS_SM1CHECKED;
      smbuffer = smbuffer + WS_SM2UNCHECKED;
      smbuffer = smbuffer + WS_SM4UNCHECKED;
      smbuffer = smbuffer + WS_SM8UNCHECKED;
      smbuffer = smbuffer + WS_SM16UNCHECKED;
      smbuffer = smbuffer + WS_SM32UNCHECKED;
      break;
    case 2 :
      smbuffer = WS_SM1UNCHECKED;
      smbuffer = smbuffer + WS_SM2CHECKED;
      smbuffer = smbuffer + WS_SM4UNCHECKED;
      smbuffer = smbuffer + WS_SM8UNCHECKED;
      smbuffer = smbuffer + WS_SM16UNCHECKED;
      smbuffer = smbuffer + WS_SM32UNCHECKED;
      break;
    case 4 :
      smbuffer = WS_SM1UNCHECKED;
      smbuffer = smbuffer + WS_SM2UNCHECKED;
      smbuffer = smbuffer + WS_SM4CHECKED;
      smbuffer = smbuffer + WS_SM8UNCHECKED;
      smbuffer = smbuffer + WS_SM16UNCHECKED;
      smbuffer = smbuffer + WS_SM32UNCHECKED;
      break;
    case 8 :
      smbuffer = WS_SM1UNCHECKED;
      smbuffer = smbuffer + WS_SM2UNCHECKED;
      smbuffer = smbuffer + WS_SM4UNCHECKED;
      smbuffer = smbuffer + WS_SM8CHECKED;
      smbuffer = smbuffer + WS_SM16UNCHECKED;
      smbuffer = smbuffer + WS_SM32UNCHECKED;
      break;
    case 16 :
      smbuffer = WS_SM1UNCHECKED;
      smbuffer = smbuffer + WS_SM2UNCHECKED;
      smbuffer = smbuffer + WS_SM4UNCHECKED;
      smbuffer = smbuffer + WS_SM8UNCHECKED;
      smbuffer = smbuffer + WS_SM16CHECKED;
      smbuffer = smbuffer + WS_SM32UNCHECKED;
      break;
    case 32 :
      smbuffer = WS_SM1UNCHECKED;
      smbuffer = smbuffer + WS_SM2UNCHECKED;
      smbuffer = smbuffer + WS_SM4UNCHECKED;
      smbuffer = smbuffer + WS_SM8UNCHECKED;
      smbuffer = smbuffer + WS_SM16UNCHECKED;
      smbuffer = smbuffer + WS_SM32CHECKED;
      break;
    default :
      smbuffer = WS_SM1CHECKED;
      smbuffer = smbuffer + WS_SM2UNCHECKED;
      smbuffer = smbuffer + WS_SM4UNCHECKED;
      smbuffer = smbuffer + WS_SM8UNCHECKED;
      smbuffer = smbuffer + WS_SM16UNCHECKED;
      smbuffer = smbuffer + WS_SM32UNCHECKED;
      break;
  }

  String msbuffer = String(mySetupData->get_motorSpeed());
  switch ( mySetupData->get_motorSpeed() )
  {
    case 0:
      msbuffer = WS_MSSLOWCHECKED;
      msbuffer = msbuffer + WS_MSMEDUNCHECKED;
      msbuffer = msbuffer + WS_MSFASTUNCHECKED;
      break;
    case 1:
      msbuffer = WS_MSSLOWUNCHECKED;
      msbuffer = msbuffer + WS_MSMEDCHECKED;
      msbuffer = msbuffer + WS_MSFASTUNCHECKED;
      break;
    case 2:
      msbuffer = WS_MSSLOWUNCHECKED;
      msbuffer = msbuffer + WS_MSMEDUNCHECKED;
      msbuffer = msbuffer + WS_MSFASTCHECKED;
      break;
    default:
      msbuffer = WS_MSSLOWUNCHECKED;
      msbuffer = msbuffer + WS_MSMEDUNCHECKED;
      msbuffer = msbuffer + WS_MSFASTCHECKED;
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

#ifdef TEMPERATUREPROBE
  String tmbuffer = String(readtemp(0));
#else
  String tmbuffer = "20.0";
#endif
  String trbuffer = String(mySetupData->get_tempprecision());

  String stbuffer;    // stepstomove
  stbuffer = "<input type=\"radio\" name=\"mv\" value=\"-500\" > -500 ";
  stbuffer = stbuffer + "<input type=\"radio\" name=\"mv\" value=\"-100\" > -100 ";
  stbuffer = stbuffer + "<input type=\"radio\" name=\"mv\" value=\"-10\" > -10 ";
  stbuffer = stbuffer + "<input type=\"radio\" name=\"mv\" value=\"-1\" > -1 ";
  stbuffer = stbuffer + "<input type=\"radio\" name=\"mv\" value=\"0\" Checked > 0 ";
  stbuffer = stbuffer + "<input type=\"radio\" name=\"mv\" value=\"1\" > 1 ";
  stbuffer = stbuffer + "<input type=\"radio\" name=\"mv\" value=\"10\" > 10 ";
  stbuffer = stbuffer + "<input type=\"radio\" name=\"mv\" value=\"100\" > 100 ";
  stbuffer = stbuffer + "<input type=\"radio\" name=\"mv\" value=\"500\" > 500 ";

  // construct home page of webserver
  // header
  HomePage = "<head><meta http-equiv=\"refresh\" content=\"" + String(WS_REFRESHRATE) + "\">" + String(WS_PAGETITLE) + "</head><body>";
  HomePage = HomePage + String(WS_TITLE);

  HomePage = HomePage + String(WS_COPYRIGHT);
  HomePage = HomePage + "<p>Driverboard = myFP2ESP." + programName + "<br>";
  HomePage = HomePage + "<myFP2ESP." + programName + "</h3>IP Address: " + ipStr + ", Firmware Version=" + String(programVersion) + "</br>";

  // position. set position. goto position
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><br><b>Focuser Position</b> <input type=\"text\" name=\"fp\" size =\"15\" value=" + fpbuffer + "> ";
  HomePage = HomePage + "<input type=\"submit\" name=\"setpos\" value=\"Set Pos\"> <input type=\"submit\" name=\"gotopos\" value=\"Goto Pos\">"; \
  HomePage = HomePage + " (Target = " + String(ftargetPosition) + ")</form></p>";

  // maxstep
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><b>MaxSteps</b> <input type=\"text\" name=\"fm\" size =\"15\" value=" + mxbuffer + "> ";
  HomePage = HomePage + "<input type=\"submit\" value=\"Submit\"></form>";

  // move buttons
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><b>Move: </b>" + stbuffer;
  HomePage = HomePage + "<input type=\"hidden\" name=\"mv\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

  // Halt button and isMoving
  HomePage = HomePage + "<form action=\"/\" method=\"post\">";
  HomePage = HomePage + "IsMoving = " + imbuffer + " ";
  HomePage = HomePage + "<input type=\"hidden\" name=\"ha\" value=\"true\"><input type=\"submit\" value=\"HALT\"></form>";

  // temperature and temperature resolution
  HomePage = HomePage + "<p><b>Temperature</b> = " + tmbuffer + "</p>";
  HomePage = HomePage + "<form action=\"/\" method=\"post\">";
  HomePage = HomePage + "<b>Temperature Resolution </b><input type=\"text\" name=\"tr\" size =\"3\" value=" + trbuffer + "> ";
  HomePage = HomePage + "<input type=\"submit\" value=\"Submit\"></form>";

  // coilpower
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><b>Coil Power </b>" + cpbuffer ;
  HomePage = HomePage + "<input type=\"hidden\" name=\"cp\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

  // reverse direction
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><b>Reverse Direction </b>" + rdbuffer ;
  HomePage = HomePage + "<input type=\"hidden\" name=\"rd\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

  // stepmode
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><b>Step Mode </b>" + smbuffer + " ";
  HomePage = HomePage + "<input type=\"hidden\" name=\"sm\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

  // motor speed
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><b>Motor Speed: </b>" + msbuffer + " ";
  HomePage = HomePage + "<input type=\"hidden\" name=\"ms\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

  // add HOME button
  HomePage = HomePage + "<p><form action=\"/\" method=\"GET\"><input type=\"submit\" value=\"HOMEPAGE\"></form></p>";
  HomePage = HomePage + "</body></html>\r\n";
}

// handles root page of webserver
// this is called whenever a client requests home page of sebserver
void WEBSERVER_handleRoot()
{
  // if the root page was a HALT request via Submit button

  String halt_str = webserver.arg("ha");
  if ( halt_str != "" )
  {
    DebugPrint( "root() -halt:" );
    DebugPrintln(halt_str);
    ftargetPosition = fcurrentPosition;
  }

  // if set focuser position
  String fpos_str = webserver.arg("setpos");
  if ( fpos_str != "" )
  {
    Serial.print( "setpos:" );
    Serial.println(fpos_str);
    String fp = webserver.arg("fp");
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

  // if goto focuser position
  fpos_str = webserver.arg("gotopos");
  if ( fpos_str != "" )
  {
    Serial.print( "gotopos:" );
    Serial.println(fpos_str);
    String fp = webserver.arg("fp");
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
      ftargetPosition = temp;
    }
  }

  // if update of maxsteps
  String fmax_str = webserver.arg("fm");
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
  String fms_str = webserver.arg("ms");
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
  String fcp_str = webserver.arg("cp");
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
  String frd_str = webserver.arg("rd");
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
  String fsm_str = webserver.arg("sm");
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

  // if update temperature resolution
  String tres_str = webserver.arg("tr");
  if ( tres_str != "" )
  {
    int temp = 0;
    DebugPrint( "root() -temperature resolution:" );
    DebugPrintln(tres_str);
    temp = tres_str.toInt();
    if ( temp < 9 )
    {
      // error, do not change
      temp = 9;
    }
    if ( temp > 12 )
    {
      temp = 12;
    }
    mySetupData->set_tempprecision(temp);
  }

  // if move
  String fmv_str = webserver.arg("mv");
  if ( fmv_str != "" )
  {
    unsigned long temp = 0;
    DebugPrint("root() -move:" );
    DebugPrintln(fmv_str);
    temp = fmv_str.toInt();
    ftargetPosition = fcurrentPosition + temp;
    DebugPrint("Move = "); DebugPrintln(fmv_str);
    DebugPrint("Current = "); DebugPrint(fcurrentPosition);
    DebugPrint(", Target = "); DebugPrintln(ftargetPosition);
    if ( ftargetPosition > mySetupData->get_maxstep() )
    {
      ftargetPosition = mySetupData->get_maxstep();
    }
  }

  DebugPrintln( "root() -build homepage" );

  // construct the homepage now
  SetHomePage();

  // send the homepage to a connected client
  DebugPrintln("root() - send homepage");
  webserver.send(NORMALWEBPAGE, TEXTPAGETYPE, HomePage );
  delay(10);                     // small pause so background ESP8266 tasks can run

}

void start_webserver(void)
{
  setNotFoundPage();            // set up webserver page for no connection to a myFocuserPro2 controller
  // construct the homepage now
  SetHomePage();
  webserver.on("/", WEBSERVER_handleRoot);
  webserver.onNotFound(WEBSERVER_handleNotFound);
  webserver.begin();
}
#endif // ifdef WEBSERVER
// WEBSERVER END ------------------------------------------------------------------------------------

#endif // ifndef webserver_h
