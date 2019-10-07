#include <Arduino.h>

#if defined(ESP8266)
 #include <FS.h>
#else
 #include "SPIFFS.h"
#endif


// For AcceeePointMode, your computer will connect to this network to gain
// access to the controller
// For StationMode, You will need to change mySSID and myPASSWORD to that of
// your existing wifi network so that the controller will connect to that
// network when it powers on
const String mySSID = "myfp2esp8266ap";
const String myPASSWORD = "myfp2esp8266ap";


const String filename = "/wifi.jsn";



void setup() 
{
  String s;
  String rSSID;
  String rPASSWORD;
   
  Serial.begin(115200); // Serielle Ausgabe aktivieren
  Serial.println("");  
    
  SPIFFS.begin(); // Filesystem mounten
  Serial.println("start to format SPIFFS, wait some seconds....");  
  SPIFFS.format();

  Serial.println("start to write file");
  File f = SPIFFS.open(filename, "w"); // Datei zum schreiben öffnen
  if (!f)  
    Serial.println("file open failed");
  else
  {
    Serial.println("file open to write"); 
    s = "{SSID:" + mySSID + ",PASSWORD:" + myPASSWORD +"}";
    Serial.println(s);
    f.print(s);
  }
  f.close(); // Wir schließen die Datei
  Serial.println("file closed");

  Serial.println("\nstart to read file");
  f = SPIFFS.open(filename, "r"); // Datei zum lesen öffnen
  if (!f)  
    Serial.println("file open failed");
  else
  {
    Serial.println("file open to read");
    String data = f.readString(); // Inhalt der Textdatei wird gelesen...
    Serial.println("Content of file:");
    Serial.println(data); // ... und wieder ausgegeben
    Serial.println("----------------------------------");
    // extra SSID and print it
    int len = s.length();                 // get length of string
    int bidx = s.indexOf(':');            // find : after SSID keyword
    int eidx = s.indexOf(',');            // find , after the SSID name
    rSSID = s.substring(bidx+1,eidx);     // get all chars from first : to first ,
    Serial.print("SSID:");                // print SSID and itsvalue
    Serial.println(rSSID);
    // extra password and print it
    // find last occurence of :
    bidx = s.lastIndexOf(':');            // find : after PASSWORD keyword
    rPASSWORD = s.substring(bidx,len-1);  // get all remaining chars, len-1 because strings start at pos 0
    Serial.print("Password:");            // print PASSWORD and its value
    Serial.println(rPASSWORD);
    if (data == s)
        Serial.println("well done");
  }
  Serial.println("file closed");      
  f.close();
}

void loop() {}
