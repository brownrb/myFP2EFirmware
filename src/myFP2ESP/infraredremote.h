// ----------------------------------------------------------------------------------------------
// myFP2ESP INFRA-RED REMOTE ROUTINES AND DEFINITIONS
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2019. All Rights Reserved.
// (c) Copyright Holger M, 2019. All Rights Reserved. 
// ----------------------------------------------------------------------------------------------

#ifndef infraredremote_h
#define infraredremote_h

// ----------------------------------------------------------------------------------------------
// EXTERNS
// ----------------------------------------------------------------------------------------------
extern IRrecv irrecv;

// ----------------------------------------------------------------------------------------------
// DATA
// ----------------------------------------------------------------------------------------------

#ifdef INFRAREDREMOTE

#endif // INFRAREDREMOTE

// ----------------------------------------------------------------------------------------------
// CODE
// ----------------------------------------------------------------------------------------------
#if defined(INFRAREDREMOTE)
void update_irremote()
{
  // check IR
  if (irrecv.decode(&results))
  {
    int adjpos = 0;
    static long lastcode;
    if ( results.value == KEY_REPEAT )
    {
      results.value = lastcode;                         // repeat last code
    }
    else
    {
      lastcode = results.value;
    }
    switch ( lastcode )
    {
      case IR_SLOW:
        mySetupData->set_motorSpeed(SLOW);
        driverboard->setmotorspeed(mySetupData->get_motorSpeed());     // set the correct delay based on motorSpeed
        break;
      case IR_MEDIUM:
        mySetupData->set_motorSpeed(MED);
        driverboard->setmotorspeed(mySetupData->get_motorSpeed());     // set the correct delay based on motorSpeed
        break;
      case IR_FAST:
        mySetupData->set_motorSpeed(FAST);
        driverboard->setmotorspeed(mySetupData->get_motorSpeed());     // set the correct delay based on motorSpeed
        break;
      case IR_IN1:
        adjpos = -1;
        break;
      case IR_OUT1:
        adjpos = 1;
        break;
      case IR_IN10:
        adjpos = -10;
        break;
      case IR_OUT10:
        adjpos = 10;
        break;
      case IR_IN50:
        adjpos = -50;
        break;
      case IR_OUT50:
        adjpos = 50;
        break;
      case IR_IN100:
        adjpos = -100;
        break;
      case IR_OUT100:
        adjpos = 100;
        break;
      case IR_SETPOSZERO :                              // 0 RESET POSITION TO 0
        adjpos = 0;
        ftargetPosition = 0;
        fcurrentPosition = 0;
        break;
    }
    irrecv.resume();                                    // Receive the next value
    long newpos;
    if ( adjpos < 0 )
    {
      newpos = (long)fcurrentPosition + adjpos;
      newpos = (newpos < 0 ) ? 0 : newpos;
      ftargetPosition = newpos;
    }
    else if ( adjpos > 0)
    {
      newpos = fcurrentPosition + adjpos;
      newpos = (newpos > mySetupData->get_maxstep()) ? mySetupData->get_maxstep() : newpos;
      ftargetPosition = newpos;
    }
  }
}
#endif // if defined(INFRAREDREMOTE)




#endif
