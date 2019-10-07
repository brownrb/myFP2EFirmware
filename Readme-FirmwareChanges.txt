Make sure you install the ESP32 and ESP8266 Arduino Core into the Arduino IDE as outlined in the PDF.
Make sure your Arduino IDE version is 1.8.x

This folder contains
	- library files
	- firmware for esp32 
	- test programs
	- link to ESP32 Arduino Core

There is a common file for the all driver boards. 
Please note that you cannot run ULN2003 firmware on a controller using a DRV8825 driver board and vice-versa.

Precautions
Never connect or disconnect the temperature probe or stepper motor when power is on. If you do, the controller will be damaged.

Change Log

// 101
// Make a table for keys for IR remote to permit easy remapping
// update_irremote() to use new keymap table
// Increase maximum allowable stepsize to 100.0 and add checks for stepsize
// Fix issues with incorrect step modes
// Tidy up redundant stepper motor routines and temperature code in main file
// Fix errorchecking when setting new maxstep value
// Add R3 Wemos ESP32 CH240G board support
// Add LOCALSERIAL
// Add OTA update capability
// Tidy up OLEDTEXT and OLEDGRAPHICS

// 100
// Initial release
