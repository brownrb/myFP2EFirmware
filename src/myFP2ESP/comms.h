// ---------------------------------------------------------------------------
// myFP2ESP COMMS ROUTINES AND DEFINITIONS
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// COPYRIGHT
// ---------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2021. All Rights Reserved.
// (c) Copyright Holger M, 2019-2021. All Rights Reserved.
// ---------------------------------------------------------------------------

#ifndef comms_h
#define comms_h

// ---------------------------------------------------------------------------
// EXTERNS
// ---------------------------------------------------------------------------
extern unsigned long ftargetPosition;          // target position
extern byte isMoving;
extern char ipStr[];
extern const char* programVersion;
extern const char* DRVBRD_ID;
extern DriverBoard* driverboard;
extern char mySSID[64];
extern void software_Reboot(int);
extern volatile bool halt_alert;

//extern float read_temp(byte);
//extern void temp_setresolution(byte);

//extern byte tprobe1;

extern TempProbe *myTempProbe;
extern void  start_ascomremoteserver(void);
extern void  stop_ascomremoteserver(void);
extern void  init_leds(void);
extern void  start_webserver();
extern void  stop_webserver();

// ---------------------------------------------------------------------------
// DATA
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// CODE
// ---------------------------------------------------------------------------
#if defined(ACCESSPOINT) || defined(STATIONMODE) || defined(LOCALSERIAL) || defined(BLUETOOTHMODE)

void SendMessage(const char *str)
{
  DebugPrint(SENDSTR);
  DebugPrintln(str);

#if defined(ACCESSPOINT) || defined(STATIONMODE)  // for Accesspoint or Station mode
  myclient.print(str);
  packetssent++;
#elif defined(BLUETOOTHMODE)  // for bluetooth
  SerialBT.print(str);
#elif defined(LOCALSERIAL)
  Serial.print(str);
#endif
}

void SendPaket(const char token, const char *str)
{
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%c%s%c", token,  str, EOFSTR);
  SendMessage(buffer);
}

void SendPaket(const char token, const unsigned char val)
{
  char buffer[32];

  snprintf(buffer, sizeof(buffer), "%c%u%c", token,  val, EOFSTR);
  SendMessage(buffer);
}

void SendPaket(const char token, const int val)
{
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%c%i%c", token,  val, EOFSTR);
  SendMessage(buffer);
}

void SendPaket(const char token, const unsigned long val)
{
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%c%lu%c", token,  val, EOFSTR);
  SendMessage(buffer);
}

void SendPaket(const char token, const float val, int i)    // i => decimal place
{
  char buff[32];
  char temp[8];
  // Note Arduino snprintf does not support .2f
  dtostrf(val, 4, i, temp); 
  snprintf(buff, sizeof(buff), "%c%s%c", token, temp, EOFSTR);
  SendMessage(buff);
}

void ESP_Communication()
{
  byte cmdval;
  String receiveString = "";
  String WorkString = "";
  long paramval = 0;

#if defined(BLUETOOTHMODE)
  receiveString = STARTCMDSTR + queue.pop();
#elif defined(LOCALSERIAL)
  receiveString = STARTCMDSTR + queue.pop();
#else   // for Accesspoint or Station mode
  packetsreceived++;
  receiveString = myclient.readStringUntil(EOFSTR);    // read until terminator
#endif

  receiveString += EOFSTR;                                // put back terminator
  String cmdstr = receiveString.substring(1, 3);
  cmdval = cmdstr.toInt();                                // convert command to an integer
  DebugPrint("recstr=" + receiveString + "  ");
  DebugPrintln("cmdstr=" + cmdstr);
  switch (cmdval)
  {
    // all the get values first followed by set values
    case 0: // get focuser position
      SendPaket('P', driverboard->getposition());
      break;
    case 1: // ismoving
      SendPaket('I', isMoving);
      break;
    case 2: // get controller status
      SendPaket('E', "OK");
      break;
    case 3: // get firmware version
#ifdef INDI
      SendPaket('F', "291");
#else
      SendPaket('F', programVersion);
#endif
      break;
    case 4: // get firmware name
      char buffer[32];
      snprintf(buffer, sizeof(buffer), "%s\r\n%s",  DRVBRD_ID, programVersion );
      SendPaket('F', buffer);
      break;
    case 6: // get temperature
        SendPaket('Z', myTempProbe->read_temp(0), 3);
      break;
    case 8: // get maxStep
      SendPaket('M', mySetupData->get_maxstep());
      break;
    case 10: // get maxIncrement
      SendPaket('Y', mySetupData->get_maxstep());
      break;
    case 11: // get coilpower
      SendPaket('O', mySetupData->get_coilpower());
      break;
    case 13: // get reverse direction setting, 00 off, 01 on
      SendPaket('R', mySetupData->get_reversedirection());
      break;
    case 21: // get temp probe resolution
      SendPaket('Q', mySetupData->get_tempprecision());
      break;
    case 24: // get status of temperature compensation (enabled | disabled)
      SendPaket('1', mySetupData->get_tempcompenabled());
      break;
    case 25: // get IF temperature compensation is available
      if ( mySetupData->get_temperatureprobestate() == 1 )
      {
        SendPaket('A', "1"); // this focuser supports temperature compensation
      }
      else
      {
        SendPaket('A', "0");
      }
      break;
    case 26: // get temperature coefficient steps/degree
      SendPaket('B', mySetupData->get_tempcoefficient());
      break;
    case 29: // get stepmode
      SendPaket('S', mySetupData->get_stepmode());
      break;
    case 32: // get if stepsize is enabled
      SendPaket('U', mySetupData->get_stepsizeenabled());
      break;
    case 33: // get stepsize
      SendPaket('T', mySetupData->get_stepsize(), 3);       // ????????????? check format
      break;
    case 34: // get the time that an LCD screen is displayed for
      SendPaket('X', mySetupData->get_lcdpagetime());
      break;
    case 37: // get displaystatus
      SendPaket('D', mySetupData->get_displayenabled());
      break;
    case 38: // :38#   Dxx#      Get Temperature mode 1=Celsius, 0=Fahrenheight
      SendPaket('b', mySetupData->get_tempmode());
      break;
    case 39: // get the new motor position (target) XXXXXX
      SendPaket('N', ftargetPosition);
      break;
    case 43: // get motorspeed
      SendPaket('C', mySetupData->get_motorSpeed());
      break;
    case 49: // aXXXXX
      SendPaket('a', "b552efd");
      break;
    case 51: // return ESP8266Wifi Controller IP Address
      SendPaket('d', ipStr);
      break;
    case 52: // return ESP32 Controller number of TCP packets sent
      SendPaket('e', packetssent);
      break;
    case 53: // return ESP32 Controller number of TCP packets received
      SendPaket('f', packetsreceived);
      break;
    case 54: // return ESP32 Controller SSID
#ifdef LOCALSERIAL
      SendPaket('g', "SERIAL");
#endif
#ifdef BLUETOOTH
      SendPaket('g', "BLUETOOTH");
#endif
#if !defined(LOCALSERIAL) && !defined(BLUETOOTHMODE)
      SendPaket('g', mySSID);
#endif
      break;
    case 55: // get motorspeed delay for current speed setting
      SendPaket('0', driverboard->getstepdelay());
      break;
    case 58: // get controller features .. deprecated
      SendPaket('m', 0);
      break;
    case 62: // get update of position on lcd when moving (00=disable, 01=enable)
      SendPaket('L', mySetupData->get_lcdupdateonmove());
      break;
    case 63: // get status of home position switch (0=off, 1=closed, position 0)
#ifdef HOMEPOSITIONSWITCH
      SendPaket('H', digitalRead(HPSWPIN));
#else
      SendPaket('H', "0");
#endif
      break;
    case 72: // get DelayAfterMove
      SendPaket('3', mySetupData->get_DelayAfterMove());
      break;
    case 74: // get backlash in enabled status
      SendPaket('4', mySetupData->get_backlash_in_enabled());
      break;
    case 76: // get backlash OUT enabled status
      SendPaket('5', mySetupData->get_backlash_out_enabled());
      break;
    case 78: // return number of backlash steps IN
      SendPaket('6', mySetupData->get_backlashsteps_in());
      break;
    case 80: // return number of backlash steps OUT
      SendPaket('7', mySetupData->get_backlashsteps_out());
      break;
    case 83: // get if there is a temperature probe
      SendPaket('c', myTempProbe->get_tprobe1());
      break;
    case 87: // get tc direction
      SendPaket('k', mySetupData->get_tcdirection());
      break;
    case 91: // get focuserpreset [0-9]
      {
        byte preset = (byte) (receiveString[3] - '0');
        preset = (preset > 9) ? 9 : preset;
        SendPaket('h', mySetupData->get_focuserpreset(preset));
      }
      break;
      case 93: // get OLED page display option
      {
        char tempbuff[5];
        mySetupData->get_oledpageoption().toCharArray(tempbuff, mySetupData->get_oledpageoption().length() + 1);
        SendPaket('l', tempbuff);
      }
      break;
    case 95: // get management option
      {
        int option = 0;
        if ( mySetupData->get_ascomserverstate() == 1)
        {
          option += 1;
        }
        if ( mySetupData->get_inoutledstate() == 1 )
        {
          option += 2;
        }
        if (mySetupData->get_temperatureprobestate() == 1)
        {
          option += 4;
        }
        if ( mySetupData->get_webserverstate() == 1)
        {
          option += 8;
        }
        SendPaket('l', option);
      }
      break;

    // only the set commands are listed here as they do not require a response
    case 28:              // :28#       None    home the motor to position 0
      ftargetPosition = 0; // if this is a home then set target to 0
      break;
    case 5: // :05xxxxxx# None    Set new target position to xxxxxx (and focuser initiates immediate move to xxxxxx)
      // only if not already moving
      if ( isMoving == 0 )
      {
        WorkString = receiveString.substring(3, receiveString.length() - 1);
        ftargetPosition = (unsigned long)WorkString.toInt();
        ftargetPosition = (ftargetPosition > mySetupData->get_maxstep()) ? mySetupData->get_maxstep() : ftargetPosition;
        // main loop will update focuser positions
      }
      break;
    case 7: // set maxsteps
      {
        WorkString = receiveString.substring(3, receiveString.length() - 1);
        unsigned long tmppos = (unsigned long)WorkString.toInt();
        delay(5);
        // check to make sure not above largest value for maxstep
        tmppos = (tmppos > FOCUSERUPPERLIMIT) ? FOCUSERUPPERLIMIT : tmppos;
        // check if below lowest set value for maxstep
        tmppos = (tmppos < FOCUSERLOWERLIMIT) ? FOCUSERLOWERLIMIT : tmppos;
        // check to make sure its not less than current focuser position
        tmppos = (tmppos < driverboard->getposition()) ? driverboard->getposition() : tmppos;
        mySetupData->set_maxstep(tmppos);
      }
      break;
    case 12: // set coil power
      paramval = (byte) (receiveString[3] - '0');
      ( paramval == 1 ) ? driverboard->enablemotor() : driverboard->releasemotor();
      mySetupData->set_coilpower(paramval);
      break;
    case 14: // set reverse direction
      if ( isMoving == 0 )
      {
        paramval = (byte) (receiveString[3] - '0');
        ( paramval == 1 ) ? mySetupData->set_reversedirection(1) : mySetupData->set_reversedirection(0);
      }
      break;
    case 15: // set motorspeed
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = (byte)WorkString.toInt() & 3;
      mySetupData->set_motorSpeed((byte) paramval);
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
      if ( mySetupData->get_temperatureprobestate() == 1)
      {
        myTempProbe->temp_setresolution((byte) paramval);
      }
      break;
    case 22: // set the temperature compensation value to xxx
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      paramval = WorkString.toInt();
      mySetupData->set_tempcoefficient((byte)paramval);
      break;
    case 23: // set the temperature compensation ON (1) or OFF (0)
      if ( mySetupData->get_temperatureprobestate() == 1)
      {
        mySetupData->set_tempcompenabled((byte) (receiveString[3] - '0'));
      }
      break;
    case 27: // stop a move - like a Halt
      halt_alert = true;
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
#if (DRVBRD == PRO2ESP32DRV8825 || DRVBRD == PRO2ESP32R3WEMOS)
      if ( paramval < STEP1 )
      {
        paramval = STEP1;
      }
      else if ( paramval > STEP32 )
      {
        paramval = STEP32;
      }
#endif
#if (DRVBRD == PRO2EL293DNEMA || DRVBRD == PRO2EL293D28BYJ48)
      paramval = STEP1;
#endif
      // this is the proper way to do this
      driverboard->setstepmode(paramval);
      mySetupData->set_stepmode(paramval);
      break;
    case 31: // set focuser position
      if ( isMoving == 0 )
      {
        WorkString = receiveString.substring(3, receiveString.length() - 1);
        {
          long tpos = (long)WorkString.toInt();
          tpos = (tpos < 0) ? 0 : tpos;
          unsigned long tmppos = ((unsigned long) tpos > mySetupData->get_maxstep()) ? mySetupData->get_maxstep() : (unsigned long) tpos;
          ftargetPosition = tmppos;
          driverboard->setposition(tmppos);
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
#if defined(OLED_TEXT)
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
#endif // ifdef OLED_TEXT
      break;
    case 40: // reset Arduino myFocuserPro2E controller
      software_Reboot(2000);      // reboot with 2s delay
      break;
    case 42: // reset focuser defaults
      if ( isMoving == 0 )
      {
        mySetupData->SetFocuserDefaults();
        ftargetPosition = mySetupData->get_fposition();
        driverboard->setposition(ftargetPosition);
        mySetupData->set_fposition(ftargetPosition);
      }
      break;
    case 48: // save settings to FS
      mySetupData->set_fposition(driverboard->getposition());       // need to save setting
      mySetupData->SaveNow();                                       // save the focuser settings immediately
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
        long pos = (long)WorkString.toInt() + (long)driverboard->getposition();
        pos  = (pos < 0) ? 0 : pos;
        ftargetPosition = ( pos > (long)mySetupData->get_maxstep()) ? mySetupData->get_maxstep() : (unsigned long)pos;
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
        unsigned long tmppos = (unsigned long)WorkString.toInt();
        mySetupData->set_focuserpreset( preset, tmppos );
      }
      break;
        case 92: // Set OLED page display option
      WorkString = receiveString.substring(3, receiveString.length() - 1);
      mySetupData->set_oledpageoption(WorkString);
      break;
    case 94: // Set management options
      {
        WorkString = receiveString.substring(3, receiveString.length() - 1);
        int option = WorkString.toInt();
        if ( (option & 1) == 1 )
        {
          // ascom server start if not already started
          if ( mySetupData->get_ascomserverstate() == 0)
          {
#if defined(ACCESSPOINT) || defined(STATIONMODE)
            start_ascomremoteserver();
#endif
          }
        }
        else
        {
          // ascom server stop if running
          if ( mySetupData->get_ascomserverstate() == 1)
          {
#if defined(ACCESSPOINT) || defined(STATIONMODE)
            stop_ascomremoteserver();
#endif
          }
        }
        if ( (option & 2) == 2)
        {
          // in out leds start
          if ( mySetupData->get_inoutledstate() == 0)
          {
            mySetupData->set_inoutledstate(1);
            // reinitialise pins
#if (DRVBRD == PRO2ESP32ULN2003 || DRVBRD == PRO2ESP32L298N || DRVBRD == PRO2ESP32L293DMINI || DRVBRD == PRO2ESP32L9110S) || (DRVBRD == PRO2ESP32DRV8825 )
            init_leds();
#endif
          }
        }
        else
        {
          // in out leds stop
          // if disabled then enable
          if ( mySetupData->get_inoutledstate() == 1)
          {
            mySetupData->set_inoutledstate(0);
          }
        }
        if ( (option & 4) == 4)
        {
          // temp probe start
          if (mySetupData->get_temperatureprobestate() == 0)
          {
            mySetupData->set_temperatureprobestate(1);
          }
        }
        else
        {
          // temp probe stop
          if (mySetupData->get_temperatureprobestate() == 1)
          {
            mySetupData->set_temperatureprobestate(0);
          }
        }
        if ( (option & 8) == 8 )
        {
          // set web server option
          if ( mySetupData->get_webserverstate() == 0)
          {
#if defined(ACCESSPOINT) || defined(STATIONMODE)
            start_webserver();
#endif
          }
        }
        else
        {
          if ( mySetupData->get_webserverstate() == 1)
          {
#if defined(ACCESSPOINT) || defined(STATIONMODE)
            stop_webserver();
#endif
          }
        }
      }
      break;
      
    // compatibilty with myFocuserpro2 in LOCALSERIAL mode
    case 44: // set motorspeed threshold when moving - switches to slowspeed when nearing destination
      //ignore
      break;
    case 45: // get motorspeedchange threshold value
      SendPaket('G', "200");
      break;
    case 46: // enable/Disable motorspeed change when moving
      //ignore
      break;
    case 47: // get motorspeedchange enabled? on/off
      SendPaket('J', "0");
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
      SendPaket('K', "0");
      break;
    case 67: // set jogging direction, 0=IN, 1=OUT
      // ignore
      break;
    case 68: // get jogging direction, 0=IN, 1=OUT
      SendPaket('V', "0");
      break;
    case 89:
      SendPaket('9', "1");
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
