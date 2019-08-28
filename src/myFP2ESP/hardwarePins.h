#include <Arduino.h>

#ifndef hardwarePins_h
#define hardwarePins_h

#if (CHIPMODEL == WEMOS)                  // esp8266
#define CHIPESP8266         1
#define TEMPPIN             2             // ds18b20 temperature probe WEMOS D1 mini => here we need some clever idea!!!!
#define OLED_ADDR           0x3C          // some OLED displays maybe at 0x3F, use I2Cscanner to find the correct address#endif // wemos
#endif

#if (CHIPMODEL == NODEMCUV1)              // esp8266 esp12e, nodemcu 30p device
#define CHIPESP8266         1
#define I2CDATAPIN          5
#define I2CCLOCKPIN         4
#define TEMPPIN             10
#define OLED_ADDR           0x3C          // some OLED displays maybe at 0x3F, use I2Cscanner to find the correct address
#endif // nodemcuv1

#if (CHIPMODEL == ESP32VROOM)             // esp32 vroom dev, 30p device
#define CHIPESP32           2
#define I2CDATAPIN          21            // i2c for oled 
#define I2CCLOCKPIN         22
#define TEMPPIN             13            // ds18b20 temperature probe
#define INLED               18            // IN direction LED, prewired
#define OUTLED              19            // OUT direction LED, prewired
#define INPB                34            // has 10K pullup resistor, so will be active low
#define OUTPB               35            // has 10K pullup resistor, so will be active low
#define IRPIN               15            // Infra-red remote controller 
#define OLED_ADDR           0x3C          // some OLED displays maybe at 0x3F, use I2Cscanner to find the correct address
#endif // esp32vroom

#endif // hardwarePins.h
