// ----------------------------------------------------------------------------------------------
// myFP2ESP DISPLAY ROUTINES AND DEFINITIONS
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2021. All Rights Reserved.
// (c) Copyright Holger M, 2021. All Rights Reserved.
// ----------------------------------------------------------------------------------------------
#ifndef displays_h
#define displays_h

#ifdef USE_SSD1306                            // For the OLED 128x64 0.96" display using the SSD1306 driver
#include <SSD1306Wire.h>
#endif
#ifdef USE_SSH1106                            // For the OLED 128x64 1.3" display using the SSH1106 driver
#include <SH1106Wire.h>
#endif

#include <mySSD1306AsciiWire.h>

// ----------------------------------------------------------------------------------------------
// DEFINITIONS
// ----------------------------------------------------------------------------------------------

#define SCREEN_WIDTH          128           // OLED display width, in pixels
#define SCREEN_HEIGHT         64            // OLED display height, in pixels
#define OLED_ADDR             0x3C          // some OLED displays maybe at 0x3F, use I2Cscanner to find the correct address

//__helper function

extern bool CheckOledConnected(void);

// ----------------------------------------------------------------------------------------------
// class boddies
// ----------------------------------------------------------------------------------------------
class OLED_NON
{
  public:
    OLED_NON();
    virtual void oledgraphicmsg(String &, int, bool);
    virtual void oled_draw_Wifi(int);

    virtual void oledtextmsg(String, int, boolean, boolean);
    virtual void update_oledtext_position(void);
    virtual void update_oledtextdisplay(void);
    virtual void Update_Oled(const oled_state, const connection_status);
    virtual void oled_draw_reboot(void);

    byte  current_status = oled_on;
    byte  linecount = 0;
};

class OLED_TEXT : public SSD1306AsciiWire, public OLED_NON
{
  public:
    OLED_TEXT();
    void oledtextmsg(String , int , boolean , boolean);
    void update_oledtext_position(void);
    void update_oledtextdisplay(void);
  private:
    void displaylcdpage0(void);      // displaylcd screen
    void displaylcdpage1(void);
    void displaylcdpage2(void);
    void Update_OledText(void);
    void UpdatePositionOledText(void);
    void display_oledtext_page0(void);
    void display_oledtext_page1(void);
    void display_oledtext_page2(void);
};

#ifdef USE_SSD1306
class OLED_GRAPHIC : public SSD1306Wire, public OLED_NON
#else
class OLED_GRAPHIC : public SH1106Wire, public OLED_NON
#endif
{
  public:
    //    OLED_GRAPHIC(uint8_t _address, uint8_t _sda, uint8_t _scl);   // constructor
    OLED_GRAPHIC();
    void Update_Oled(const oled_state, const connection_status);
    void oledgraphicmsg(String &, int, boolean);
    void oled_draw_Wifi(int);
    void oled_draw_reboot(void);

  private:
    void oled_draw_main_update(const connection_status);
    byte count_hb = 0;      // heart beat counter
    long timestamp;
};

#endif // #ifdef displays_h
