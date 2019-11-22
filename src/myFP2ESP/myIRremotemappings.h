// ----------------------------------------------------------------------------------------------
// TITLE: myFP2ESP INFRA RED REMOTE CONTROLLER KEY MAPPINGS
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2019. All Rights Reserved.
// (c) Copyright Holger M, 2019. All Rights Reserved.
// ----------------------------------------------------------------------------------------------

#ifndef myIRremotemappings_h
#define myIRremotemappings_h

// This file contains mappings for the various keys of the IRremote

#define KEY_REPEAT      4294967295
#define KEY_CH          16736925        // CH
#define KEY_CHMINUS     16753245        // CH-
#define KEY_CHPLUS      16769565        // CH+
#define KEY_PREV        16720605        // |<<
#define KEY_NEXT        16712445        // |>>
#define KEY_PLAY        16761405        // |>||
#define KEY_MINUS       16769055        // -
#define KEY_PLUS        16754775        // + 
#define KEY_EQUALS      16748655        // EQ
#define KEY_ZERO        16738455        // 0
#define KEY_ONE         16724175
#define KEY_TWO         16718055
#define KEY_THREE       16743045
#define KEY_FOUR        16716015
#define KEY_FIVE        16743045
#define KEY_SIX         16734885
#define KEY_SEVEN       16728765
#define KEY_EIGHT       16730805
#define KEY_NINE        16732845
#define KEY_100PLUS     16750695
#define KEY_200PLUS     16756815

// MAKE A MAP TABLE NOW - DO NOT CHANGE THE LEFT SIDE, IR_xxxxxx
// Motorspeed
#define IR_SLOW         KEY_CHMINUS
#define IR_MEDIUM       KEY_CH
#define IR_FAST         KEY_CHPLUS

#define IR_IN1          KEY_ONE
#define IR_OUT1         KEY_THREE
#define IR_IN10         KEY_FOUR
#define IR_OUT10        KEY_SIX
#define IR_IN50         KEY_SEVEN
#define IR_OUT50        KEY_NINE
#define IR_IN100        KEY_MINUS
#define IR_OUT100       KEY_EQUALS
#define IR_SETPOSZERO   KEY_ZERO

#endif
