# myFP2E Firmware for ESP32 and ESP8266
This is the firmware for the myFocuserPro2E ESP8266 and ESP32 focus controller projecta on Sourceforge.
https://sourceforge.net/projects/arduinoascomfocuserpro2diy/files/myFocuserPro2E/
https://sourceforge.net/projects/myfocuserpro2-esp32/

(c) Robert Brown, 2019. All rights reserved.
(c) Holger Manz, 2019. All rights reserved.

The source code is located in the src folder. 

# Compiling for ESP8266
Compiling the source code requires Arduino IDE **v1.8.9** with the ExpressIF ESP8266 Arduino extensions.
You will need to add the JSON file for the ESP8266 library by using the File->Preferences menu of the 
Arduino IDE and add the location for the library into the board manager
http://arduino.esp8266.com/stable/package_esp8266com_index.json

Once specified, open the board manager, scroll down to ESP8266 and install **v2.5.2** (*only this version*) 
Then you can specify the target board as **Node MCU v1.0 (ESP-12E)** with Flash Size set to 4M (1MB SPIFFS) and upload speed of 115200.

Note: Additional JSON files are separated in board manager by using a comma ,

# Compiling for ESP32
Compiling the source code requires Arduino IDE **v1.8.9** with the ExpressIF ESP32 Arduino extensions.
You will need to add the JSON file for the ESP32 library by using the File->Preferences menu of the 
Arduino IDE and add the location for the library into the board manager
https://dl.espressif.com/dl/package_esp32_index.json

Once specified, open the board manager, scroll down to ESP32 and install the latest version
Then you can specify the target board as **ESP32 Dev** with Flash Size set t 4M (1MB SPIFFS) and upload speed of 115200.

Note1: Additional JSON files are separated in board manager by using a comma ,
Note2: When installed the folder C:\Program Files (x86)\Arduino\libraries\WiFi prevents compiling of ESP8266 firmware so move the WIFI folder to
C:\Program Files (x86)\Arduino\libraries.old\WiFi

# Additional JSON library Needed
make sure you install the library ArduinoJson-6.x.xip into the Arduino IDE else firmware will not compile.
You can do this within the Arduino IDE by
- Open the Arduino Library Manager
- Search for “ArduinoJson”
- Select the version: 6.11.3
- Click install

The latest version is at https://github.com/bblanchon/ArduinoJson/releases

After specifying the target board type as NodeMCU v1, you will need to set the Flashsize for SPIFFS, ie, Tools=>Flashsize space for the SPIFFS in ArduinoIDE (512K).

# Libraries
To compile you will also need to import these libraries in the folder src/myFP2ELibs into the Arduino IDE environment using the Menu - Sketch - Include Library - Add .Zip file
- myOLED
- IRREMOTEESP32
- HalfStepperESP32

Do not edit or replace any of these library files with others.

# Additional Libraries which must be downloaded and installed
To compile this firmware you need to ensure that you have installed a number of "library" files into the Arduino environment. For each of the links below, you need to DOWNLOAD the ZIP file firat.

OneWire
https://github.com/PaulStoffregen/OneWire

DuckDNS
https://github.com/ayushsharma82/EasyDDNS

Temperature Probe
https://github.com/milesburton/Arduino-Temperature-Control-Library

After downloading the zip files, you then need to install these into the Arduino IDE environment. To do that, start the Arduino IDE, select Sketch, Include Library, Add .Zip library, then navigate to where the ZIP files are stored and select them (you can only load 1 zip file at a time so please repeat this for all ZIP files).

Once you have done this, you can start programming the controller.

# Hardware Driver Boards
One firmware file supports all driver boards [a different file supports the L293D motor shield driver board for the ESP8266]. The user must set the DRVBRD at the beginning of the firmware file to the correct driver board, eg, #define DRVBRD ULN2003 will set the driver board to an ULN2003, and set the chipModel in the chipModel.h file before compiling.

# Controller Modes
The controller supports the modes ACCESSPOINT and STATION for ESP8266 with an additional BLUETOOTHMODE for the ESP32. 

# Command Process Handler Routine
At present a single command process handler manages received commands for either Bluetooth [ESP32 only] of TCP/IP. This must handle each ontroller mode, so some variables had to be made global to do this.

# FocuserSetupData.cpp and .h files
At present these must be in the same folder as the .ino file, but will later migrate to a library. These files handle the SPIFFS data, saving, restoring and persistance of controller settings. For the ESP8266, there is a now shift away from the use of EEPROM to using a JSON file stored in the file system SPIFFS area.

# myBoards.cpp and .h files
This implements a driver board class for the supported driver boards [DRV8825, ULN2003, L298N, L293DMINI, L9110S]. Speed can be controlled by setting the step delay (in milliseconds). Increasing the value will slow down the stepper. There are two constructors for this class, the user should never have to change the main code within the firmware file which sets up the driver boards. Pins mappings between the controller chip and the driver board are specified here.

#hardwarePins.h
This file specifies the pin mappings for I2C, temperature probes, Push buttons etc for each driver board. Do not change unless you have made your own board and require the pins to be different.

#chipModels.h
This file specifies the chip type being used. The three main chips are WEMOS (ESP8266), NODEMCUV1 (ESP8266 12E) and ESP32WROOM (ESP32). You must set the CHIPMODEL to the correct chip in this file. When that is done the harware pins mappings are automatically assigned correctly.

#generalDefinitions.h
This file contains general definitions for all controllers, such as motor speed, step modes, focuser limits etc.

#ESPQueue.h
This file manages the Bluetooth serial data for ESP32 chips. It is a modified version of the Queue.h file from Steven de Salas. DO NOT MODIFY THIS FILE. It has been modified to work with ESP8266 and ESP32 chips.

#HalfStepperESP32 Library
This library manages the stepping modes for the ULN2003, L298N, L293DMINI and L9110S driver boards. It is a modified version of the HalfStepper library from Tom Biuso. DO NOT MODIFY THESE FILES. It has been modified to work with ESP8266 and ESP32 chips.

#IRRemoteESP32 Library
This library manages communication with an Infra-Red controller. The Library is based on one created by Ken Shirriff but has been modified wto work with ESP8266 and ESP32 chips. DO NOT MODIFY THESE FILES.

#myOLED Libary
This is a special cut down version of the SSD1306AsciiWire library by Bill Greiman. The library has been reduced in size and a number of additional features added. DO NOT MODIFY THESE FILES.



