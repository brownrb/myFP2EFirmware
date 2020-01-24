// ----------------------------------------------------------------------------------------------
// myFP2ESP COMMS ROUTINES AND DEFINITIONS
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2020. All Rights Reserved.
// (c) Copyright Holger M, 2019-2020. All Rights Reserved.
// ----------------------------------------------------------------------------------------------

#ifndef comms_h
#define comms_h

// ----------------------------------------------------------------------------------------------
// EXTERNS
// ----------------------------------------------------------------------------------------------
extern unsigned long fcurrentPosition;         // current focuser position
extern unsigned long ftargetPosition;          // target position
extern byte isMoving;
extern String ipStr;
extern char programVersion[];
extern String programName;
extern DriverBoard* driverboard;
extern char mySSID[64];
extern void software_Reboot(int);

#ifdef OLEDTEXT
extern void update_oledtext_position(void);
extern SSD1306AsciiWire* myoled;
#endif
#ifdef TEMPERATUREPROBE
extern float read_temp(byte);
extern void temp_setresolution(byte);
#endif

// ----------------------------------------------------------------------------------------------
// DATA
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// CODE
// ----------------------------------------------------------------------------------------------
#if defined(ACCESSPOINT) || defined(STATIONMODE) || defined(LOCALSERIAL) || defined(BLUETOOTHMODE)
void SendPaket(String str)
{
  DebugPrint(SENDSTR);
  DebugPrintln(str);
#if defined(ACCESSPOINT) || defined(STATIONMODE)  // for Accesspoint or Station mode
  myclient.print(str);
  packetssent++;
#endif // if defined(ACCESSPOINT) || defined(STATIONMODE)
#if defined(BLUETOOTHMODE)  // for bluetooth
  SerialBT.print(str);
#endif
#if defined(LOCALSERIAL)
  Serial.print(str);
#endif
}

void ESP_Communication( byte mode )
{
  byte cmdval;
  String receiveString = "";
  String WorkString = "";
  long paramval = 0;

  switch ( mode )
  {
    case ESPDATA:
      // for Accesspoint or Station mode
      packetsreceived++;
#if defined(ACCESSPOINT) || defined(STATIONMODE)
      receiveString = myclient.readStringUntil(EOFSTR);    // read until terminator
#endif
      break;
#if defined(BLUETOOTHMODE)
    case BTDATA:
      receiveString = STARTCMDSTR + queue.pop();
      break;
#endif
#if defined(LOCALSERIAL)
    case SERIALDATA:                                    // for Serial
      receiveString = STARTCMDSTR + queue.pop();
      break;
#endif
  }

  receiveString += EOFSTR;                                 // put back terminator
  String cmdstr = receiveString.substring(1, 3);
  cmdval = cmdstr.toInt();                              // convert command to an integer
  DebugPrint("recstr=" + receiveString + "  ");
  //  DebugPrintln("cmdstr=" + cmdstr);
  switch (cmdval)
  {
    // all the get go first followed by set
    case 0: // get focuser position
      SendPaket('P' + String(fcurrentPosition) + EOFSTR);
      break;
    case 1: // ismoving
      SendPaket('I' + String(isMoving) + EOFSTR);
      break;
    case 2: // get controller status
      SendPaket("EOK#");
      break;
    case 3: // get firmware version
#ifdef INDI
      SendPaket("F291#");
#else
      SendPaket('F' + String(programVersion) + EOFSTR);
#endif
      break;
    case 4: // get firmware name
      SendPaket('F' + programName + '\r' + '\n' + String(programVersion) + EOFSTR);
      break;
    case 6: // get temperature
#if defined(TEMPERATUREPROBE)
      SendPaket('Z' + String(read_temp(0), 3) + EOFSTR);
#else
      SendPaket("Z20.00#");
#endif
      break;
    case 8: // get maxStep
      SendPaket('M' + String(mySetupData->get_maxstep()) + EOFSTR);
      break;
    case 10: // get maxIncrement
      SendPaket('Y' + String(mySetupData->get_maxstep()) + EOFSTR);
      break;
    case 11: // get coilpower
      SendPaket('O' + String(mySetupData->get_coilpower()) + EOFSTR);
      break;
    case 13: // get reverse direction setting, 00 off, 01 on
      SendPaket('R' + String(mySetupData->get_reversedirection()) + EOFSTR);
      break;
    case 21: // get temp probe resolution
      SendPaket('Q' + String(mySetupData->get_tempprecision()) + EOFSTR);
      break;
    case 24: // get status of temperature compensation (enabled | disabled)
      SendPaket('1' + String(mySetupData->get_tempcompenabled()) + EOFSTR);
      break;
    case 25: // get IF temperature compensation is available
#if defined(TEMPERATUREPROBE)
      SendPaket("A1#"); // this focuser supports temperature compensation
#else
      SendPaket("A0#");
#endif
      break;
    case 26: // get temperature coefficient steps/degree
      SendPaket('B' + String(mySetupData->get_tempcoefficient()) + EOFSTR);
      break;
    case 29: // get stepmode
      SendPaket('S' + String(mySetupData->get_stepmode()) + EOFSTR);
      break;
    case 32: // get if stepsize is enabled
      SendPaket('U' + String(mySetupData->get_stepsizeenabled()) + EOFSTR);
      break;
    case 33: // get stepsize
      SendPaket('T' + String(mySetupData->get_stepsize()) + EOFSTR);
      break;
    case 34: // get the time that an LCD screen is displayed for
      SendPaket('X' + String(mySetupData->get_lcdpagetime()) + EOFSTR);
      break;
    case 37: // get displaystatus
      SendPaket('D' + String(mySetupData->get_displayenabled()) + EOFSTR);
      break;
    case 38: // :38#   Dxx#      Get Temperature mode 1=Celsius, 0=Fahrenheight
      SendPaket('b' + String(mySetupData->get_tempmode()) + EOFSTR);
      break;
    case 39: // get the new motor position (target) XXXXXX
      SendPaket('N' + String(ftargetPosition) + EOFSTR);
      break;
    case 43: // get motorspeed
      SendPaket('C' + String(mySetupData->get_motorSpeed()) + EOFSTR);
      break;
    case 49: // aXXXXX
      SendPaket("ab552efd#");
      break;
    case 51: // return ESP8266Wifi Controller IP Address
      SendPaket('d' + ipStr + EOFSTR);
      break;
    case 52: // return ESP32 Controller number of TCP packets sent
      SendPaket('e' + String(packetssent) + EOFSTR);
      break;
    case 53: // return ESP32 Controller number of TCP packets received
      SendPaket('f' + String(packetsreceived) + EOFSTR);
      break;
    case 54: // return ESP32 Controller SSID
#ifdef LOCALSERIAL
      SendPaket("gSERIAL#");
#endif
#ifdef BLUETOOTH
      SendPaket("gBLUETOOTH#");
#endif
#if !defined(LOCALSERIAL) && !defined(BLUETOOTHMODE)
      SendPaket('g' + String(mySSID) + EOFSTR);
#endif
      break;
    case 55: // get motorspeed delay for current speed setting
      SendPaket('0' + String(driverboard->getstepdelay()) + EOFSTR);
      break;
    case 58: // get controller features
      SendPaket('m' + String(Features) + EOFSTR);
      break;
    case 62: // get update of position on lcd when moving (00=disable, 01=enable)
      SendPaket('L' + String(mySetupData->get_lcdupdateonmove()) + EOFSTR);
      break;
    case 63: // get status of home position switch (0=off, 1=closed, position 0)
#ifdef HOMEPOSITIONSWITCH
      SendPaket('H' + String(digitalRead(HPSWPIN)) + EOFSTR);
#else
      SendPaket("H0#");
#endif
      break;
    case 72: // get DelayAfterMove
      SendPaket('3' + String(mySetupData->get_DelayAfterMove()) + EOFSTR);
      break;
    case 74: // get backlash in enabled status
      SendPaket((mySetupData->get_backlash_in_enabled() == 0) ? "40#" : "41#");
      break;
    case 76: // get backlash OUT enabled status
      SendPaket((mySetupData->get_backlash_out_enabled() == 0) ? "50#" : "51#");
      break;
    case 78: // return number of backlash steps IN
      SendPaket('6' + String(mySetupData->get_backlashsteps_in()) + EOFSTR);
      break;
    case 80: // return number of backlash steps OUT
      SendPaket('7' + String(mySetupData->get_backlashsteps_out()) + EOFSTR);
      break;
    case 83: // get if there is a temperature probe
      SendPaket('c' + String(tprobe1) + EOFSTR);
      break;
    case 87: // get tc direction
      SendPaket('k' + String(mySetupData->get_tcdirection()) + EOFSTR);
      break;
    case 91: // get focuserpreset [0-9]
      {
        byte preset = (byte) (receiveString[3] - '0');
        preset = (preset > 9) ? 9 : preset;
        SendPaket('h' + String(mySetupData->get_focuserpreset(preset)) + EOFSTR);
      }
      break;

    // only the set commands are listed here as they do not require a response
    case 28:              // :28#       None    home the motor to position 0
      ftargetPosition = 0; // if this is a home then set target to 0
#if defined(JOYSTICK1) || defined(JOYSTICK2)
      // restore motorspeed just in case
      driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif
      break;
    case 5: // :05xxxxxx# None    Set new target position to xxxxxx (and focuser initiates immediate move to xxxxxx)
      // only if not already moving
      if ( isMoving == 0 )
      {
        WorkString = receiveString.substring(3, receiveString.length() - 1);
        ftargetPosition = (unsigned long)WorkString.toInt();
        ftargetPosition = (ftargetPosition > mySetupData->get_maxstep()) ? mySetupData->get_maxstep() : ftargetPosition;
        update_oledtext_position();
#if defined(JOYSTICK1) || defined(JOYSTICK2)
        // restore motorspeed just in case
        driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif
      }
      break;
    case 7: // set maxsteps
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      tmppos = (unsigned long)WorkString.toInt();
      delay(5);
      // check to make sure not above largest value for maxstep
      tmppos = (tmppos > FOCUSERUPPERLIMIT) ? FOCUSERUPPERLIMIT : tmppos;
      // check if below lowest set value for maxstep
      tmppos = (tmppos < FOCUSERLOWERLIMIT) ? FOCUSERLOWERLIMIT : tmppos;
      // check to make sure its not less than current focuser position
      tmppos = (tmppos < fcurrentPosition) ? fcurrentPosition : tmppos;
      mySetupData->set_maxstep(tmppos);
      break;
    case 12: // set coil power
      paramval = (byte) (receiveString[3] - '0');
      ( paramval == 1 ) ? driverboard->enablemotor() : driverboard->releasemotor();
      mySetupData->set_coilpower(paramval);
      break;
    case 14: // set reverse direction
      if ( isMoving == 0 )
      {
        mySetupData->set_reversedirection((byte) (receiveString[3] - '0'));
      }
      break;
    case 15: // set motorspeed
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = (byte)WorkString.toInt() & 3;
      mySetupData->set_motorSpeed((byte) paramval);
      driverboard->setmotorspeed((byte) paramval);
      break;
    case 16: // set display to celsius
      mySetupData->set_tempmode(1); // temperature display mode, Celsius=1, Fahrenheit=0
      break;
    case 17: // set display to fahrenheit
      mySetupData->set_tempmode(0); // temperature display mode, Celsius=1, Fahrenheit=0
      break;
    case 18:
      // :180#    None    set the return of user specified stepsize to be OFF - default
      // :181#    None    set the return of user specified stepsize to be ON - reports what user specified as stepsize
      mySetupData->set_stepsizeenabled((byte) (receiveString[3] - '0'));
      break;
    case 19: // :19xxxx#  None   set the step size value - double type, eg 2.1
      {
        WorkString = receiveString.substring(3, receiveString.length() - 1);
        float tempstepsize = (float)WorkString.toFloat();
        tempstepsize = (tempstepsize < MINIMUMSTEPSIZE ) ? MINIMUMSTEPSIZE : tempstepsize;
        tempstepsize = (tempstepsize > MAXIMUMSTEPSIZE ) ? MAXIMUMSTEPSIZE : tempstepsize;
        mySetupData->set_stepsize(tempstepsize);
      }
      break;
    case 20: // set the temperature resolution setting for the DS18B20 temperature probe
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = WorkString.toInt();
      mySetupData->set_tempprecision((byte) paramval);
#ifdef TEMPERATUREPROBE
      temp_setresolution((byte) paramval);
#endif
      break;
    case 22: // set the temperature compensation value to xxx
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = WorkString.toInt();
      mySetupData->set_tempcoefficient((byte)paramval);
      break;
    case 23: // set the temperature compensation ON (1) or OFF (0)
#if defined(TEMPERATUREPROBE)
      mySetupData->set_tempcompenabled((byte) (receiveString[3] - '0'));
#endif
      break;
    case 27: // stop a move - like a Halt
      ftargetPosition = fcurrentPosition;
      break;
    case 30: // set step mode
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = WorkString.toInt();
#if (DRVBRD == PRO2EULN2003 || DRVBRD == PRO2EL298N || DRVBRD == PRO2EL293DMINI || DRVBRD == PRO2EL9110S)
      paramval = (byte)(paramval & 3);      // STEP1 - STEP2
#endif
#if (DRVBRD == PRO2ESP32ULN2003 || DRVBRD == PRO2ESP32L298N || DRVBRD == PRO2ESP32L293DMINI || DRVBRD == PRO2ESP32L9110S)
      paramval = (byte)(paramval & 3);      // STEP1 - STEP2
#endif
#if (DRVBRD == WEMOSDRV8825 || DRVBRD == PRO2EDRV8825 || DRVBRD == PRO2EDRV8825BIG)
      paramval = DRV8825TEPMODE;            // stepmopde set by jumpers
#endif
#if (DRVBRD == PRO2ESP32DRV8825)
      paramval = (paramval < STEP1 ) ? STEP1 : paramval;
      paramval = (paramval > STEP32 ) ? STEP32 : paramval;
#endif
#if (DRVBRD == PRO2EL293DNEMA || DRVBRD == PRO2EL293D28BYJ48)
      paramval = STEP1;
#endif
      // this is the proper way to do this
      driverboard->setstepmode((byte) paramval);
      mySetupData->set_stepmode(driverboard->getstepmode());
      break;
    case 31: // set focuser position
      if ( isMoving == 0 )
      {
        WorkString = receiveString.substring(3, receiveString.length() - 1);
        {
          long tpos = (long)WorkString.toInt();
          tpos = (tpos < 0) ? 0 : tpos;
          tmppos = ((unsigned long) tpos > mySetupData->get_maxstep()) ? mySetupData->get_maxstep() : (unsigned long) tpos;
          fcurrentPosition = ftargetPosition = tmppos;
          mySetupData->set_fposition(tmppos);
        }
      }
      break;
    case 35: // set length of time an LCD page is displayed for in seconds
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = WorkString.toInt();
      paramval = (paramval < LCDPAGETIMEMIN ) ? LCDPAGETIMEMIN : paramval;
      paramval = (paramval > LCDPAGETIMEMAX ) ? LCDPAGETIMEMAX : paramval;
      mySetupData->set_lcdpagetime((byte)paramval);
      break;
    case 36:
      // :360#    None    Disable Display
      // :361#    None    Enable Display
#if defined(OLEDTEXT)
      mySetupData->set_displayenabled((byte) (receiveString[3] - '0'));
      if (mySetupData->get_displayenabled() == 1)
      {
        if ( displayfound == true )
        {
          myoled->Display_On();
        }
      }
      else
      {
        if ( displayfound == true )
        {
          myoled->Display_Off();
        }
      }
#endif // ifdef OLEDTEXT
      break;
    case 40: // reset Arduino myFocuserPro2E controller
      software_Reboot(2000);      // reboot with 2s delay
      break;
    case 42: // reset focuser defaults
      if ( isMoving == 0 )
      {
        mySetupData->SetFocuserDefaults();
        ftargetPosition = fcurrentPosition = mySetupData->get_fposition();
      }
      break;
    case 48: // save settings to EEPROM
      // TODO: THIS SHOULD SAVE "ALL" FOCUSER SETTINGS
      mySetupData->set_fposition(fcurrentPosition); // need to save setting
      break;
    case 56: // set motorspeed delay for current speed setting
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      driverboard->setstepdelay(WorkString.toInt());
      break;
    case 61: // set update of position on lcd when moving (0=disable, 1=enable)
      mySetupData->set_lcdupdateonmove((byte) (receiveString[3] - '0'));
      break;
    case 64: // move a specified number of steps
      if ( isMoving == 0 )
      {
        WorkString = receiveString.substring(3, receiveString.length() - 1);
        long pos = (long)WorkString.toInt() + (long)fcurrentPosition;
        pos  = (pos < 0) ? 0 : pos;
        ftargetPosition = ( pos > (long)mySetupData->get_maxstep()) ? mySetupData->get_maxstep() : (unsigned long)pos;
#if defined(JOYSTICK1) || defined(JOYSTICK2)
        // restore motorspeed just in case
        driverboard->setmotorspeed(mySetupData->get_motorSpeed());
#endif
      }
      break;
    case 71: // set DelayAfterMove in milliseconds
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      mySetupData->set_DelayAfterMove((byte)WorkString.toInt());
      break;
    case 73: // Disable/enable backlash IN (going to lower focuser position)
      mySetupData->set_backlash_in_enabled((byte) (receiveString[3] - '0'));
      break;
    case 75: // Disable/enable backlash OUT (going to lower focuser position)
      mySetupData->set_backlash_out_enabled((byte) (receiveString[3] - '0'));
      break;
    case 77: // set backlash in steps
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      mySetupData->set_backlashsteps_in((byte)WorkString.toInt());
      break;
    case 79: // set backlash OUT steps
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      mySetupData->set_backlashsteps_out((byte)WorkString.toInt());
      break;
    case 88: // set tc direction
      mySetupData->set_tcdirection((byte) (receiveString[3] - '0'));
      break;
    case 90: // Set preset x [0-9] with position value yyyy [unsigned long]
      {
        byte preset = (byte) (receiveString[3] - '0');
        preset = (preset > 9) ? 9 : preset;
        WorkString = receiveString.substring(4, receiveString.length() - 1);
        tmppos = (unsigned long)WorkString.toInt();
        mySetupData->set_focuserpreset( preset, tmppos );
      }
      break;

    // compatibilty with myFocuserpro2 in LOCALSERIAL mode
    case 44: // set motorspeed threshold when moving - switches to slowspeed when nearing destination
      //ignore
      break;
    case 45: // get motorspeedchange threshold value
      SendPaket("G200#");
      break;
    case 46: // enable/Disable motorspeed change when moving
      //ignore
      break;
    case 47: // get motorspeedchange enabled? on/off
      SendPaket("J0#");
      break;
    case 57: // set Super Slow Jogging Speed
      // ignore
      break;
    case 60: // set MotorSpeed when jogging
      // ignore
      break;
    case 65: // set jogging state enable/disable
      // ignore
      break;
    case 66: // get jogging state enabled/disabled
      SendPaket("K0#");
      break;
    case 67: // set jogging direction, 0=IN, 1=OUT
      // ignore
      break;
    case 68: // get jogging direction, 0=IN, 1=OUT
      SendPaket("V0#");
      break;
    case 89:
      SendPaket("91#");
      break;
  }
}
#endif // if defined(ACCESSPOINT) || defined(STATIONMODE) || defined(LOCALSERIAL) || defined(BLUETOOTHMODE)

#if defined(LOCALSERIAL)
void clearSerialPort()
{
  while (Serial.available())
  {
    Serial.read();
  }
}

void processserial()
{
  // Serial.read() only returns a single char so build a command line one char at a time
  // : starts the command, # ends the command, do not store these in the command buffer
  // read the command until the terminating # character
  while (Serial.available() )
  {
    char inChar = Serial.read();
    switch ( inChar )
    {
      case STARTCMDSTR :     // start
        serialline = "";
        break;
      case '\r' :
      case '\n' :
        // ignore
        break;
      case EOFSTR :       // eoc
        queue.push(serialline);
        break;
      default :           // anything else
        serialline = serialline + inChar;
        break;
    }
  }
}
#endif // if defined(LOCALSERIAL)

#if defined(BLUETOOTHMODE)
void clearbtPort()
{
  while (SerialBT.available())
  {
    SerialBT.read();
  }
}

void processbt()
{
  // SerialBT.read() only returns a single char so build a command line one char at a time
  // : starts the command, # ends the command, do not store these in the command buffer
  // read the command until the terminating # character
  while (SerialBT.available() )
  {
    char inChar = SerialBT.read();
    switch ( inChar )
    {
      case STARTCMDSTR :     // start
        btline = "";
        break;
      case '\r' :
      case '\n' :
        // ignore
        break;
      case EOFSTR :       // eoc
        queue.push(btline);
        break;
      default :           // anything else
        btline = btline + inChar;
        break;
    }
  }
}
#endif // if defined(BLUETOOTHMODE)


#endif
