#ifndef Extruder_h_
#define Extruder_h_

#include "Base.h"
#include <math.h>

const float EXT_BETA  = 3950,  
            EXT_T0    = 298.15,
            EXT_MAX_TEMP = 250.0f,
            EXT_R0    = 100000,
            
            ZERO_C    = 273.15,
            
            KP        = 6,
            KI        = 1,
            KD        = 0;
            
const UBYTE THERMISTOR_PORT = A0,
            HEATER_PORT     = 10;
            
const int   R_BALANCE       = 10000, //Ohm
            BIAS            = 45,
            MAX_OUTPUT      = 255 - BIAS;
            
            
extern float targetTemperature,
             temperatureRange,
             currentTemperature;
            
void initExtruder();
float getExtruderTemperature();
void temperatureWorker(const ULONG);
void startTemperatureWorker();
void stopTemperatureWorker();

#endif
