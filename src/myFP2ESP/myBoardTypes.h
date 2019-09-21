#include <Arduino.h>

#ifndef myBoardTypes_h
#define myBoardTypes_h

// Definition for board types and general board definitions like stepmode and motorspeeds

// ----------------------------------------------------------------------------------------------
// 1: BOARD DEFINES -- DO NOT CHANGE
// ----------------------------------------------------------------------------------------------
#ifndef WEMOSDRV8825
#define WEMOSDRV8825          35         // if using a drv8825 you also need to set DRV8825STEPMODE in myBoards.h
#endif
#ifndef PRO2EDRV8825
#define PRO2EDRV8825          36         // if using a drv8825 you also need to set DRV8825STEPMODE in myBoards.h
#endif
#ifndef PRO2EULN2003
#define PRO2EULN2003          37
#endif
#ifndef PRO2EL293DNEMA
#define PRO2EL293DNEMA        38        // Motor shield ESP8266 with NEMA motor
#endif
#ifndef PRO2EL293D28BYJ48
#define PRO2EL293D28BYJ48     39        // Motor shield ESP8266 with 28byj48 motor
#endif
#ifndef PRO2EL298N
#define PRO2EL298N            40         // uses PCB layout for ULN2003
#endif
#ifndef PRO2EL293DMINI
#define PRO2EL293DMINI        41         // uses PCB layout for ULN2003
#endif
#ifndef PRO2EL9110S
#define PRO2EL9110S           42         // uses PCB layout for ULN2003
#endif
#ifndef PRO2ESP32DRV8825
#define PRO2ESP32DRV8825      43
#endif
#ifndef PRO2ESP32ULN2003
#define PRO2ESP32ULN2003      44
#endif
#ifndef PRO2ESP32L298N
#define PRO2ESP32L298N        45
#endif
#ifndef PRO2ESP32L293DMINI
#define PRO2ESP32L293DMINI    46         // uses PCB layout for ULN2003
#endif
#ifndef PRO2ESP32L9110S
#define PRO2ESP32L9110S       47         // uses PCB layout for ULN2003
#endif


#endif
