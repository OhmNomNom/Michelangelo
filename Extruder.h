#ifndef Extruder_h_
#define Extruder_h_

#include "Base.h"
#include "TimerThree.h"
#include <math.h>

const float EXT_BETA  = 3950,
            EXT_T0    = 298.15,
            EXT_R0    = 100000,
            
            ZERO_C    = 273.15,
            
            KP        = 1,
            KI        = 0,
            KD        = 0;
            
const UBYTE THERMISTOR_PORT = A0,
            HEATER_PORT     = 10;
            
const UINT  R_BALANCE       = 10000,
            PID_PERIOD      = 100,
            MAX_OUTPUT      = 255;
            
extern float targetTemperature,
             temperatureRange;
            
void initExtruder();
float getExtruderTemperature();
void temperatureWorker();
void startTemperatureWorker();
void stopTemperatureWorker();

#endif
