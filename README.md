# myFP2E Firmware for ESP32 and ESP8266 myFP2 Controllers
This is the firmware for the myFocuserPro2E ESP8266 and ESP32 focus controller projects on Sourceforge.
https://sourceforge.net/projects/arduinoascomfocuserpro2diy/files/myFocuserPro2E/ and 
https://sourceforge.net/projects/myfocuserpro2-esp32/

(c) Robert Brown and Holger Manz, 2019-2020. All rights reserved.

The source code is located in the **src** folder. 

# PDF
To get a clear understanding of myFP2ESP features and options, it is essential to read the PDF.

# Compiling for ESP8266
Compiling the source code requires Arduino IDE **v1.8.9** with the ExpressIF ESP8266 Arduino extensions.
You will need to add the JSON file for the ESP8266 library by using the File->Preferences menu of the 
Arduino IDE and add the location for the library into the board manager
http://arduino.esp8266.com/stable/package_esp8266com_index.json

Once specified, open the board manager, scroll down to ESP8266 and install version **v2.4.0**.
Then you can specify the target board as **Node MCU v1.0 (ESP-12E)** with Flash Size set to 4M (1MB SPIFFS) and upload speed of 115200.

Note: Additional JSON files are separated in board manager by using a comma ,

# Compiling for ESP32
Compiling the source code requires Arduino IDE **v1.8.9** with the ExpressIF ESP32 Arduino extensions.
You will need to add the JSON file for the ESP32 library by using the File->Preferences menu of the 
Arduino IDE and add the location for the library into the board manager
https://dl.espressif.com/dl/package_esp32_index.json

Once specified, open the board manager, scroll down to ESP32 and install the latest version
Then you can specify the target board as **ESP32 Dev** with Flash Size set t 4M (1MB SPIFFS) and upload speed of 115200.

### Note1:  
Additional JSON files are separated in board manager by using a comma ,  
### Note2:  
When installed the folder C:\Program Files (x86)\Arduino\libraries\WiFi prevents compiling of ESP8266 firmware so move the WIFI folder to C:\Program Files (x86)\Arduino\libraries.old\WiFi  

# Additional JSON library Needed
make sure you install the library ArduinoJson-6.x.xip into the Arduino IDE else firmware will not compile.
You can do this within the Arduino IDE by

* Open the Arduino Library Manager
* Search for “ArduinoJson”
* Select the version: 6.11.2
* Click install

The latest version is at https://github.com/bblanchon/ArduinoJson/releases

# Libraries
To compile you will also need to import these libraries in the folder src/myFP2ELibs into the Arduino IDE environment using the Menu - Sketch - Include Library - Add .Zip file

* myOLED
* HalfStepperESP32
* myDallasTemperature

Do not edit or replace any of these library files with others.

# Additional Libraries which must be downloaded and installed
To compile this firmware you need to ensure that you have installed a number of "library" files into the Arduino environment. For each of the links below, you need to DOWNLOAD the ZIP file firat.

Graphics OLED
https://github.com/ThingPulse/esp8266-oled-ssd1306

OneWire
https://github.com/PaulStoffregen/OneWire

IRRemote
https://github.com/crankyoldgit/IRremoteESP8266

DuckDNS
https://github.com/ayushsharma82/EasyDDNS

ESP32 Sketch Data uploader
https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/

ESP8266 Sketch Data uploader
https://github.com/esp8266/arduino-esp8266fs-plugin

After downloading the zip files, you then need to install these into the Arduino IDE environment. To do that, start the Arduino IDE, select Sketch, Include Library, Add .Zip library, then navigate to where the ZIP files are stored and select them (you can only load 1 zip file at a time so please repeat this for all ZIP files).

The Sketch Data uploaders require a different method for installing. The install instructions are found in their respective download pages and require copying the files into a special folder of the Arduino program installation.

Once you have done this, you can start programming the controller.

# Hardware Driver Boards
One firmware file supports all driver boards [a different file supports the L293D motor shield driver board for the ESP8266]. The user must set the **DRVBRD** at the beginning of the firmware file [myBoards.h] to the correct driver board, eg, #define DRVBRD PRO2EULN2003 will set the driver board to an ULN2003 using an ESP8266 chip.

# focuserconfig.h
Configuration information about the controller is specified in the *focuserconfig.h* file. This is where you specify the controller options like TEMPERATURE PROBE and OLEDTEXT, as well as the controller mode such as ACCESSPOINT or STATIONMODE.

# Controller Modes
The controller supports the modes ACCESSPOINT, STATIONMODE, LOCALSERIAL, WEBSERVER and ASCOMREMOTE for ESP8266 with an additional BLUETOOTHMODE for the ESP32.

# Command Process Handler Routine
At present a single command process handler manages received commands for LOCALSERIAL, BLUETOOTH [ESP32 only] of TCP/IP. This must handle each controller mode, so some variables had to be made global to do this.

# FocuserSetupData.cpp and .h files
At present these must be in the same folder as the .ino file, but will later migrate to a library. These files handle the SPIFFS data, saving, restoring and persistance of controller settings. A JSON file in file system SPIFFS area is used to store focuser data.

# myBoards.cpp and .h files
This implements a driver board class for the supported driver boards [DRV8825, ULN2003, L298N, L293DMINI, L9110S]. Speed can be controlled by setting the step delay (in milliseconds). Increasing the value will slow down the stepper. There is one constructor for this class which accepts the boardtype, the user should never have to change the main code within the firmware file which sets up the driver boards. All pins mappings for the controller chip, hardware options and the driver board are specified here.

# generalDefinitions.h
This file contains general definitions for all controllers, such as motor speed, step modes, focuser limits etc.

# ESPQueue.h
This file manages the Bluetooth serial data for ESP32 chips. It is a modified version of the Queue.h file from Steven de Salas. DO NOT MODIFY THIS FILE. It has been modified to work with ESP8266 and ESP32 chips.

# HalfStepperESP32 Library
This library manages the stepping modes for the ULN2003, L298N, L293DMINI and L9110S driver boards. It is a modified version of the HalfStepper library from Tom Biuso. DO NOT MODIFY THESE FILES. It has been modified to work with ESP8266 and ESP32 chips.

# myOLED Libary
This is a special cut down version of the SSD1306AsciiWire library by Bill Greiman. The library has been reduced in size and a number of additional features added. DO NOT MODIFY THESE FILES.

# COMPILE ENVIRONMENT : Tested with 

* Arduino IDE 1.8.9
* ESP8266 Driver Board 2.4.0

Libraries 

* Arduino JSON 6.11.2
* myOLED as in myFP2ELibs
* IRRemoteESP8266 2.7.1
* HalfStepperESP32 as in myFP2ELibs
* Dallas Temperature 3.80
* Wire [as installed with Arduino 1.8.9
* OneWire 2.3.3
* EasyDDNS 1.5.2

# Notes:
You may need to turn 12V off to reprogram chip. Speed is 115200. Sometimes you might need to
remove the chip from PCB before re-programming new firmware. Remember to remove WIFI library
as instructed in PDF when programming ESP32.

# ESP8266 ISSUES
One chip I have boots fine.
Another chip will not boot properly from 12V only. I need to plug in USB cable, press reset 
then turn on 12V for it to boot correctly. ESP8266 Arduino lib 2.2.0 does not work with this 
chip either.

# TODO
Look at what need to be when chip starts up, do we need a delay reset circuit?
