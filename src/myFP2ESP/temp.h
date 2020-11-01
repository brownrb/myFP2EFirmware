// ----------------------------------------------------------------------------------------------
// TEMPERATURE PROBE : myFP2ESP support for DS18B20 Temperature Sensor
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// COPYRIGHT
// ----------------------------------------------------------------------------------------------
// (c) Copyright Robert Brown 2014-2020. All Rights Reserved.
// (c) Copyright Holger Manz, 2020. All Rights Reserved.
// ----------------------------------------------------------------------------------------------

#include <myDallasTemperature.h>

// ----------------------------------------------------------------------------------------------
// TEMPERATURE - CHANGE AT YOUR OWN PERIL
// ----------------------------------------------------------------------------------------------

class TempProbe : public DallasTemperature
{
  public:
    TempProbe();
    ~TempProbe();
    void  start_temp_probe(void);
    void  stop_temp_probe(void);
    void  temp_setresolution(byte rval);
    float read_temp(byte new_measurement);
    void  update_temp(void);
    byte  get_tprobe1(void);

private:
    byte  tprobe1;                            // indicate if there is a probe attached to myFocuserPro2
};
