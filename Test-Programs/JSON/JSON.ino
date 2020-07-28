#include <ArduinoJson.h>

void setup()
{   

    Serial.begin(115200);
    Serial.println();

    StaticJsonDocument<200> doc;
    doc["hello"] = "world";
    doc["pre"][0] = 100;
    doc["pre"][1] = 200;    
    serializeJson(doc, Serial);

}


void loop() {}