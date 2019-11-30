// ----------------------------------------------------------------------------------------------
// myFP2ESP MDNS ROUTINES AND DEFINITIONS
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2019. All Rights Reserved.
// (c) Copyright Holger M, 2019. All Rights Reserved.
// ----------------------------------------------------------------------------------------------

#ifndef mdnsserver_h
#define mdnsserver_h

// ----------------------------------------------------------------------------------------------
// INCLUDES
// ----------------------------------------------------------------------------------------------


// ----------------------------------------------------------------------------------------------
// EXTERNS
// ----------------------------------------------------------------------------------------------
extern String ipStr;
extern String programName;
extern char programVersion[];
extern int staticip;
extern String BLUETOOTHNAME;
extern char mDNSNAME[];

// ----------------------------------------------------------------------------------------------
// DATA AND DEFINITIONS
// ----------------------------------------------------------------------------------------------
#ifndef STATICIPON
#define STATICIPON    1
#endif
#ifndef STATICIPOFF
#define STATICIPOFF   0
#endif

#ifdef MDNSSERVER
#define MDNSSERVERPORT        8080          // mdns server port
#define MDNS_COPYRIGHT        "<p>(c) R. Brown, Holger M, 2019. All rights reserved.</p>"
#define MDNS_TITLE            "<h3>myFP2ESP WiFi focus controller</h3>"
#define MDNS_PAGETITLE        "<title>myFP2ESP mDNS SERVICE</title>"
String MDNSHomePage;
#if defined(ESP8266)
ESP8266WebServer mdnswebserver(MDNSSERVERPORT);
#else
WebServer mdnswebserver(MDNSSERVERPORT);
#endif // if defined(ESP8266)
#endif // ifdef MDNSSERVER

// ----------------------------------------------------------------------------------------------
// CODE
// ----------------------------------------------------------------------------------------------
#ifdef MDNSSERVER
void mdns_sethomepage(void)
{
  MDNSHomePage = "<html><head></head>";
  MDNSHomePage = MDNSHomePage + "<body>" + String(MDNS_TITLE);
  MDNSHomePage = MDNSHomePage + String(MDNS_COPYRIGHT);
  MDNSHomePage = MDNSHomePage + "<p>IP Address: " + ipStr + ": mDNS port: " + String(MDNSSERVERPORT) + ", Firmware Version=" + String(programVersion) + "</p>";
  MDNSHomePage = MDNSHomePage + "<p>Driverboard = myFP2ESP." + programName + "</p>";
#ifdef BLUETOOTHMODE
  MDNSHomePage = MDNSHomePage + "<p>Controller Mode: Bluetooth: Name: " + BLUETOOTHNAME + "</p>";
#endif
#ifdef ACCESSPOINT
  MDNSHomePage = MDNSHomePage + "<p>Controller Mode: Access point</p>";
#endif
#ifdef STATIONMODE
  MDNSHomePage = MDNSHomePage + "<p>Controller Mode: Station mode</p>";
#endif
#ifdef LOCALSERIAL
  MDNSHomePage = MDNSHomePage + "<p>Controller Mode: Local Serial</p>";
#endif
#ifdef WEBSERVER
  MDNSHomePage = MDNSHomePage + "<p>Web server is ON, port: " + String(WEBSERVERPORT) + "</p>";
#else
  MDNSHomePage = MDNSHomePage + "<p>Web server is OFF</p>";
#endif
#ifdef ASCOMREMOTE
  MDNSHomePage = MDNSHomePage + "<p>ASCOM REMOTE server is ON, port: " + String(ALPACAPORT) + "</p>";
#else
  MDNSHomePage = MDNSHomePage + "<p>ASCOM REMOTE server is OFF</p>";
#endif
#ifdef OTAUPDATES
  MDNSHomePage = MDNSHomePage + "<p>OTA updates is ON</p>";
#else
  MDNSHomePage = MDNSHomePage + "<p>OTA updates is OFF</p>";
#endif
  if ( staticip == STATICIPON )
  {
    MDNSHomePage = MDNSHomePage + "<p>Static IP is ON</p>";
  }
  else
  {
    MDNSHomePage = MDNSHomePage + "<p>Static IP is OFF</p>";
  }
  MDNSHomePage = MDNSHomePage + "</body></html>";
}

void mdns_handleNotFound(void)
{
  mdnswebserver.send(NOTFOUNDWEBPAGE, TEXTPAGETYPE, MDNSHomePage);
}

void mdns_handleRoot()
{
  // send the homepage to a connected client
  DebugPrintln("mdns_handleRoot() - send homepage");
  mdnswebserver.send(NORMALWEBPAGE, TEXTPAGETYPE, MDNSHomePage );
}

// MDNS service. find the device using dnsname.local
int start_mdns_service(void)
{
  int rval;
  // set home page for server
  mdns_sethomepage();

  // Set up mDNS responder:
  // the fully-qualified domain name is "mDNSNAME.local"
#if defined(ESP8266)
  if (!MDNS.begin(mDNSNAME, WiFi.localIP()))      // ESP8266 supports additional parameter for IP
#else
  if (!MDNS.begin(mDNSNAME))                      // ESP32 does not support IPaddress parameter
#endif
  {
    DebugPrintln(F("Error setting up MDNS responder!"));
    rval = 0;
  }
  else
  {
    DebugPrintln(F("mDNS responder started"));
    // Add service to MDNS-SD, MDNS.addService(service, proto, port)
    MDNS.addService("http", "tcp", MDNSSERVERPORT);
    // start webserver
    mdnswebserver.on("/", mdns_handleRoot);
    mdnswebserver.onNotFound(mdns_handleNotFound);
    mdnswebserver.begin();
    rval = 1;
  }
  return rval;
}
#endif // ifdef MDNSSERVER



#endif // ifndef mdnsserver_h
