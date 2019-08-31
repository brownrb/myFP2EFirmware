#include <Arduino.h>

#ifndef chipModels_h
#define chipModels_h

#define WEMOS       1                       // ESP8266
#define NODEMCUV1   2                       // ESP8266 E-12, 30P
#define ESP32WROOM  3                       // ESP32 Dev, 30P

// Select the correct chip model, see hardwarePins.h for pin mapping
//#define CHIPMODEL WEMOS
//#define CHIPMODEL NODEMCUV1
#define CHIPMODEL ESP32WROOM


#endif
