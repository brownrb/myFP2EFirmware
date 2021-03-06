// ---------------------------------------------------------------------------
// myFP2ESP FOCUSER DATA DEFINITIONS
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// COPYRIGHT
// ---------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2021. All Rights Reserved.
// (c) Copyright Holger M, 2019-2021. All Rights Reserved.
// ---------------------------------------------------------------------------

#include <Arduino.h>

#include "generalDefinitions.h"

//#define DEFAULTPOSITION       5000L               // moved to generalDefinitions.h
//#define DEFAULTMAXSTEPS       80000L
#define DEFAULTOFF              0
#define DEFAULTON               1
#define DEFAULTCELSIUS          1
#define DEFAULTFAHREN           0
#define DEFAULTDOCSIZE          2048
#define DEFAULTVARDOCSIZE       64

class SetupData
{
  public:
    SetupData(void);
    byte LoadConfiguration(void);
    boolean SaveConfiguration(unsigned long, byte);
    boolean SaveNow(void);
    void SetFocuserDefaults(void);

    //  getter
    unsigned long get_fposition();
    byte get_focuserdirection();
    unsigned long get_maxstep();
    float get_stepsize();
    byte get_DelayAfterMove();
    byte get_backlashsteps_in();
    byte get_backlashsteps_out();
    byte get_backlash_in_enabled();
    byte get_backlash_out_enabled();
    byte get_tempcoefficient();
    byte get_tempresolution();
    int  get_stepmode();
    byte get_coilpower();
    byte get_reversedirection();
    byte get_stepsizeenabled();
    byte get_tempmode();
    byte get_lcdupdateonmove();
    byte get_lcdpagetime();
    byte get_tempcompenabled();
    byte get_tcdirection();
    byte get_motorSpeed();
    byte get_displayenabled();
    unsigned long get_focuserpreset(byte);
    unsigned long get_webserverport();
    unsigned long get_ascomalpacaport();
    int get_webpagerefreshrate();
    unsigned long get_mdnsport();
    unsigned long get_tcpipport();
    byte get_showstartscreen();
    String  get_wp_backcolor();
    String  get_wp_textcolor();
    String  get_wp_headercolor();
    String  get_wp_titlecolor();
    byte get_ascomserverstate();
    byte get_webserverstate();
    byte get_inoutledstate();
    byte get_temperatureprobestate();
    byte get_showhpswmsg();
    byte get_forcedownload();
    String  get_oledpageoption();
    int get_motorspeeddelay();
    int get_homepositionswitch();
      
    //__setter
    void set_fposition(unsigned long);
    void set_focuserdirection(byte);
    void set_maxstep(unsigned long);
    void set_stepsize(float);
    void set_DelayAfterMove(byte);
    void set_backlashsteps_in(byte);
    void set_backlashsteps_out(byte);
    void set_backlash_in_enabled(byte);
    void set_backlash_out_enabled(byte);
    void set_tempcoefficient(byte);
    void set_tempresolution(byte);
    void set_stepmode(int);
    void set_coilpower(byte);
    void set_reversedirection(byte);
    void set_stepsizeenabled(byte);
    void set_tempmode(byte);
    void set_lcdupdateonmove(byte);
    void set_lcdpagetime(byte);
    void set_tempcompenabled(byte);
    void set_tcdirection(byte);
    void set_motorSpeed(byte);
    void set_displayenabled(byte);
    void set_focuserpreset(byte, unsigned long);
    void set_webserverport(unsigned long);
    void set_ascomalpacaport(unsigned long);
    void set_webpagerefreshrate(int);
    void set_mdnsport(unsigned long);
    void set_tcpipport(unsigned long);
    void set_showstartscreen(byte);
    void set_wp_backcolor(String);
    void set_wp_textcolor(String);
    void set_wp_headercolor(String);
    void set_wp_titlecolor(String);
    void set_ascomserverstate(byte);
    void set_webserverstate(byte);
    void set_temperatureprobestate(byte);
    void set_inoutledstate(byte);
    void set_showhpswmsg(byte);
    void set_forcedownload(byte);
    void set_oledpageoption(String);
    void set_motorspeeddelay(int);
    void set_homepositionswitch(int);
     
  private:
    byte SavePersitantConfiguration();
    byte SaveVariableConfiguration();

    void LoadDefaultPersistantData(void);
    void LoadDefaultVariableData(void);

    void StartDelayedUpdate(unsigned long &, unsigned long);
    void StartDelayedUpdate(float &, float);
    void StartDelayedUpdate(byte &, byte);
    void StartDelayedUpdate(int &, int);
    void StartDelayedUpdate(String &, String);
    void ListDir(const char*, uint8_t);

    boolean ReqSaveData_var;        // Flag for request save variable data
    boolean ReqSaveData_per;        // Flag for request save persitant data

    const String filename_persistant = "/data_per.jsn"; // persistant JSON setup data
    const String filename_variable = "/data_var.jsn";    // variable  JSON setup data

    unsigned long fposition;        // last focuser position
    byte focuserdirection;          // keeps track of last focuser move direction
    // these are not used in code
    // unsigned long fposition_org;    // last focuser position
    // byte focuserdirection_org;      // keeps track of last focuser move direction
    unsigned long SnapShotMillis;

    // dataset_persistant
    unsigned long maxstep;          // max steps
    float stepsize;                 // the step size in microns, ie 7.2 - value * 10, so real stepsize = stepsize / 10 (maxval = 25.6)
    byte DelayAfterMove;            // delay after movement is finished (maxval=256)
    byte backlashsteps_in;          // number of backlash steps to apply for IN moves
    byte backlashsteps_out;         // number of backlash steps to apply for OUT moves
    byte backlash_in_enabled;       // if 1, backlash is enabled for IN movements (lower or -ve moves)
    byte backlash_out_enabled;      // if 1, backlash is enabled for OUT movements (higher or +ve moves)
    byte tempcoefficient;           // steps per degree temperature coefficient value (maxval=255)
    byte tempresolution;            // 9 -12
    int  stepmode;                  // stepping mode
    byte coilpower;                 // if 1, coil power is enabled
    byte reversedirection;          // if 1, motor direction is reversed
    byte stepsizeenabled;           // if 1, controller returns step size
    byte tempmode;                  // temperature display mode, Celcius=1, Fahrenheit=0
    byte lcdupdateonmove;           // update position on lcd when moving
    byte lcdpagetime;               // length of time a display page is shown for
    byte tempcompenabled;           // indicates if temperature compensation is enabled
    byte tcdirection;               // direction in which to apply temperature compensation
    byte motorSpeed;                // speed of motor, slow, medium or fast
    byte displayenabled;            // if 1, display is enabled
    unsigned long preset[10];       // focuser presets can be used with software or ir-remote controller
    unsigned long webserverport;
    unsigned long ascomalpacaport;
    int webpagerefreshrate;
    unsigned long mdnsport;
    unsigned long tcpipport;
    byte startscreen;               // if 1, display shows startscreen messages on bootup
    String  backcolor;
    String  textcolor;
    String  headercolor;
    String  titlecolor;
    byte ascomserverstate;          // if 1, then ascomserver is enabled
    byte webserverstate;            // if 1, then webserver is enabled
    byte temperatureprobestate;     // if 1, then temperature probe is enabled
    byte inoutledstate;             // if 1, in out leds are enabled [only if board supports them]
    byte showhpswmessages;          // if 1, home position switch msg's show on display if enabled
    byte forcedownload;             // if 1, in the MANAGEMENT SERVER, a file is downloaded instead of being displayed is web browser window
    String oledpageoption;
    int motorspeeddelay;
    int homepositionswitch;
};
