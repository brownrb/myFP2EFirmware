// ----------------------------------------------------------------------------------------------
// myFP2ESP WEB SERVER ROUTINES AND DEFINITIONS
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2019. All Rights Reserved.
// (c) Copyright Holger M, 2019. All Rights Reserved.
// ----------------------------------------------------------------------------------------------

#ifndef webserver_h
#define webserver_h

// ----------------------------------------------------------------------------------------------
// EXTERNS
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
#define WS_SM1CHECKED             "<input type=\"radio\" name=\"sm\" value=\"1\" Checked> 1"
#define WS_SM1UNCHECKED           "<input type=\"radio\" name=\"sm\" value=\"1\"> 1"
#define WS_SM2CHECKED             "<input type=\"radio\" name=\"sm\" value=\"2\" Checked> 2"
#define WS_SM2UNCHECKED           "<input type=\"radio\" name=\"sm\" value=\"2\"> 2"
#define WS_SM4CHECKED             "<input type=\"radio\" name=\"sm\" value=\"4\" Checked> 4"
#define WS_SM4UNCHECKED           "<input type=\"radio\" name=\"sm\" value=\"4\"> 4"
#define WS_SM8CHECKED             "<input type=\"radio\" name=\"sm\" value=\"8\" Checked> 8"
#define WS_SM8UNCHECKED           "<input type=\"radio\" name=\"sm\" value=\"8\"> 8"
#define WS_SM16CHECKED            "<input type=\"radio\" name=\"sm\" value=\"16\" Checked> 16"
#define WS_SM16UNCHECKED          "<input type=\"radio\" name=\"sm\" value=\"16\"> 16"
#define WS_SM32CHECKED            "<input type=\"radio\" name=\"sm\" value=\"32\" Checked> 32"
#define WS_SM32UNCHECKED          "<input type=\"radio\" name=\"sm\" value=\"32\"> 32"

#define WS_MSSLOWCHECKED          "<input type=\"radio\" name=\"ms\" value=\"0\" Checked> S"
#define WS_MSSLOWUNCHECKED        "<input type=\"radio\" name=\"ms\" value=\"0\"> S"
#define WS_MSMEDCHECKED           "<input type=\"radio\" name=\"ms\" value=\"1\" Checked> M"
#define WS_MSMEDUNCHECKED         "<input type=\"radio\" name=\"ms\" value=\"1\"> M"
#define WS_MSFASTCHECKED          "<input type=\"radio\" name=\"ms\" value=\"2\" Checked> F"
#define WS_MSFASTUNCHECKED        "<input type=\"radio\" name=\"ms\" value=\"2\"> F"

#define WS_COPYRIGHT              "(c) R. Brown, Holger M, 2019. All rights reserved"
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
  NotFoundPage = NotFoundPage + "<p>" + String(WS_COPYRIGHT) + "</p>";
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

  // convert current values of position, maxsteps and presets to string types
  String fpbuffer = String(fcurrentPosition);
  String mxbuffer = String(mySetupData->get_maxstep());
  String smbuffer = String(mySetupData->get_stepmode());
  String imbuffer = String(isMoving);
  String p0buffer = String(mySetupData->get_focuserpreset(0));
  String p1buffer = String(mySetupData->get_focuserpreset(1));
  String p2buffer = String(mySetupData->get_focuserpreset(2));
  String p3buffer = String(mySetupData->get_focuserpreset(3));
  String p4buffer = String(mySetupData->get_focuserpreset(4));
  String p5buffer = String(mySetupData->get_focuserpreset(5));
  String p6buffer = String(mySetupData->get_focuserpreset(6));
  String p7buffer = String(mySetupData->get_focuserpreset(7));
  String p8buffer = String(mySetupData->get_focuserpreset(8));
  String p9buffer = String(mySetupData->get_focuserpreset(9));

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

  HomePage = HomePage + "<p>" + String(WS_COPYRIGHT) + "<br>";
  HomePage = HomePage + "Driverboard = myFP2ESP." + programName + ", <br>";
  HomePage = HomePage + "IP Address: " + ipStr + ", Firmware Version=" + String(programVersion) + "</p>";

  // position. set position. goto position
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><br><b>Position</b> <input type=\"text\" name=\"fp\" size =\"15\" value=" + fpbuffer + "> ";
  HomePage = HomePage + "<input type=\"submit\" name=\"setpos\" value=\"Set\"> <input type=\"submit\" name=\"gotopos\" value=\"Goto\">"; \
  HomePage = HomePage + " (Target = " + String(ftargetPosition) + ")</form>";

  // maxstep
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><b>MaxSteps</b> <input type=\"text\" name=\"fm\" size =\"15\" value=" + mxbuffer + "> ";
  HomePage = HomePage + "<input type=\"submit\" value=\"Submit\"></form>";

  // move buttons
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><b>Move: </b>" + stbuffer;
  HomePage = HomePage + "<input type=\"hidden\" name=\"mv\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

  // Halt button and isMoving
  HomePage = HomePage + "<form action=\"/\" method=\"post\">";
  HomePage = HomePage + "<b>IsMoving</b> = " + imbuffer + " ";
  HomePage = HomePage + "<input type=\"hidden\" name=\"ha\" value=\"true\"><input type=\"submit\" value=\"HALT\"></form>";

  // temperature and temperature resolution
  HomePage = HomePage + "<form action=\"/\" method=\"post\">";
  HomePage = HomePage + "<b>Temp</b> = " + tmbuffer + ", ";
  HomePage = HomePage + "<b>Temp Resolution </b><input type=\"text\" name=\"tr\" size =\"3\" value=" + trbuffer + "> ";
  HomePage = HomePage + "<input type=\"submit\" value=\"Submit\"></form><p></p>";

  // stepmode
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><b>Stepmode </b>" + smbuffer + " ";
  HomePage = HomePage + "<input type=\"hidden\" name=\"sm\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

  // motor speed
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><b>Motorspeed: </b>" + msbuffer + " ";
  HomePage = HomePage + "<input type=\"hidden\" name=\"ms\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form><p></p>";

  // coilpower and reverse direction
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><b>Coilpower </b>" + cpbuffer ;
  HomePage = HomePage + "<input type=\"hidden\" name=\"cp\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";
  
  // reverse direction
  HomePage = HomePage + "<form action=\"/\" method=\"post\" ><b>Reverse Direction </b>" + rdbuffer ;
  HomePage = HomePage + "<input type=\"hidden\" name=\"rd\" value=\"true\"><input type=\"submit\" value=\"Submit\"></form>";

  // focuser presets  
  HomePage = HomePage + "<form action=\"/\" method=\"post\"><b>Focuser Preset 0</b> <input type=\"text\" name=\"p0\" size =\"15\" value=" + p0buffer + "> <input type=\"submit\" value=\"Submit\"></form>";
  HomePage = HomePage + "<form action=\"/\" method=\"post\"><b>Focuser Preset 1</b> <input type=\"text\" name=\"p1\" size =\"15\" value=" + p1buffer + "> <input type=\"submit\" value=\"Submit\"></form>";
  HomePage = HomePage + "<form action=\"/\" method=\"post\"><b>Focuser Preset 2</b> <input type=\"text\" name=\"p2\" size =\"15\" value=" + p2buffer + "> <input type=\"submit\" value=\"Submit\"></form>";
  HomePage = HomePage + "<form action=\"/\" method=\"post\"><b>Focuser Preset 3</b> <input type=\"text\" name=\"p3\" size =\"15\" value=" + p3buffer + "> <input type=\"submit\" value=\"Submit\"></form>";
  HomePage = HomePage + "<form action=\"/\" method=\"post\"><b>Focuser Preset 4</b> <input type=\"text\" name=\"p4\" size =\"15\" value=" + p4buffer + "> <input type=\"submit\" value=\"Submit\"></form>";
  HomePage = HomePage + "<form action=\"/\" method=\"post\"><b>Focuser Preset 5</b> <input type=\"text\" name=\"p5\" size =\"15\" value=" + p5buffer + "> <input type=\"submit\" value=\"Submit\"></form>";
  HomePage = HomePage + "<form action=\"/\" method=\"post\"><b>Focuser Preset 6</b> <input type=\"text\" name=\"p6\" size =\"15\" value=" + p6buffer + "> <input type=\"submit\" value=\"Submit\"></form>";
  HomePage = HomePage + "<form action=\"/\" method=\"post\"><b>Focuser Preset 7</b> <input type=\"text\" name=\"p7\" size =\"15\" value=" + p7buffer + "> <input type=\"submit\" value=\"Submit\"></form>";
  HomePage = HomePage + "<form action=\"/\" method=\"post\"><b>Focuser Preset 8</b> <input type=\"text\" name=\"p8\" size =\"15\" value=" + p8buffer + "> <input type=\"submit\" value=\"Submit\"></form>";
  HomePage = HomePage + "<form action=\"/\" method=\"post\"><b>Focuser Preset 9</b> <input type=\"text\" name=\"p9\" size =\"15\" value=" + p9buffer + "> <input type=\"submit\" value=\"Submit\"></form>";

  // add HOME button
  HomePage = HomePage + "<form action=\"/\" method=\"GET\"><input type=\"submit\" value=\"HOMEPAGE\"></form>";
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

  // if update of focuser preset 0
  String fp0_str = webserver.arg("p0");
  if ( fp0_str != "" )
  {
    unsigned long temp = 0;
    DebugPrint( "root() -fp0:" );
    DebugPrintln(fp0_str);
    temp = fp0_str.toInt();
    if ( temp > mySetupData->get_maxstep() )          // if higher than max value
    {
      temp = mySetupData->get_maxstep();
    }
    mySetupData->set_focuserpreset(0, temp);
  }

  // if update of focuser preset 1
  String fp1_str = webserver.arg("p1");
  if ( fp1_str != "" )
  {
    unsigned long temp = 0;
    DebugPrint( "root() -fp1:" );
    DebugPrintln(fp1_str);
    temp = fp1_str.toInt();
    if ( temp > mySetupData->get_maxstep() )          // if higher than max value
    {
      temp = mySetupData->get_maxstep();
    }
    mySetupData->set_focuserpreset(1, temp);
  }

  // if update of focuser preset 2
  String fp2_str = webserver.arg("p2");
  if ( fp2_str != "" )
  {
    unsigned long temp = 0;
    DebugPrint( "root() -fp2:" );
    DebugPrintln(fp2_str);
    temp = fp2_str.toInt();
    if ( temp > mySetupData->get_maxstep() )          // if higher than max value
    {
      temp = mySetupData->get_maxstep();
    }
    mySetupData->set_focuserpreset(2, temp);
  }

  // if update of focuser preset 3
  String fp3_str = webserver.arg("p3");
  if ( fp3_str != "" )
  {
    unsigned long temp = 0;
    DebugPrint( "root() -fp3:" );
    DebugPrintln(fp3_str);
    temp = fp3_str.toInt();
    if ( temp > mySetupData->get_maxstep() )          // if higher than max value
    {
      temp = mySetupData->get_maxstep();
    }
    mySetupData->set_focuserpreset(3, temp);
  }

  // if update of focuser preset 4
  String fp4_str = webserver.arg("p4");
  if ( fp4_str != "" )
  {
    unsigned long temp = 0;
    DebugPrint( "root() -fp4:" );
    DebugPrintln(fp4_str);
    temp = fp4_str.toInt();
    if ( temp > mySetupData->get_maxstep() )          // if higher than max value
    {
      temp = mySetupData->get_maxstep();
    }
    mySetupData->set_focuserpreset(4, temp);
  }

  // if update of focuser preset 5
  String fp5_str = webserver.arg("p5");
  if ( fp5_str != "" )
  {
    unsigned long temp = 0;
    DebugPrint( "root() -fp5:" );
    DebugPrintln(fp5_str);
    temp = fp5_str.toInt();
    if ( temp > mySetupData->get_maxstep() )          // if higher than max value
    {
      temp = mySetupData->get_maxstep();
    }
    mySetupData->set_focuserpreset(5, temp);
  }

  // if update of focuser preset 6
  String fp6_str = webserver.arg("p6");
  if ( fp6_str != "" )
  {
    unsigned long temp = 0;
    DebugPrint( "root() -fp6:" );
    DebugPrintln(fp6_str);
    temp = fp6_str.toInt();
    if ( temp > mySetupData->get_maxstep() )          // if higher than max value
    {
      temp = mySetupData->get_maxstep();
    }
    mySetupData->set_focuserpreset(26, temp);
  }

  // if update of focuser preset 7
  String fp7_str = webserver.arg("p7");
  if ( fp7_str != "" )
  {
    unsigned long temp = 0;
    DebugPrint( "root() -fp7:" );
    DebugPrintln(fp7_str);
    temp = fp7_str.toInt();
    if ( temp > mySetupData->get_maxstep() )          // if higher than max value
    {
      temp = mySetupData->get_maxstep();
    }
    mySetupData->set_focuserpreset(7, temp);
  }

  // if update of focuser preset 8
  String fp8_str = webserver.arg("p8");
  if ( fp8_str != "" )
  {
    unsigned long temp = 0;
    DebugPrint( "root() -fp8:" );
    DebugPrintln(fp8_str);
    temp = fp8_str.toInt();
    if ( temp > mySetupData->get_maxstep() )          // if higher than max value
    {
      temp = mySetupData->get_maxstep();
    }
    mySetupData->set_focuserpreset(8, temp);
  }

  // if update of focuser preset 9
  String fp9_str = webserver.arg("p9");
  if ( fp9_str != "" )
  {
    unsigned long temp = 0;
    DebugPrint( "root() -fp9:" );
    DebugPrintln(fp9_str);
    temp = fp9_str.toInt();
    if ( temp > mySetupData->get_maxstep() )          // if higher than max value
    {
      temp = mySetupData->get_maxstep();
    }
    mySetupData->set_focuserpreset(9, temp);
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
