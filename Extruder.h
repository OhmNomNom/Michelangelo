#ifndef Extruder_h_
#define Extruder_h_

#include "Base.h"
#include <math.h>

const float EXT_BETA  = 3950,  
            EXT_T0    = 298.15,
            EXTRUDER_MAX_TEMP = 250.0f,
            EXT_R0    = 100000,
            
            ZERO_C    = 273.15,
            
            KP        = 8  ,
            KI        = 5  ,
            KI2       = 0.9,
            KD        = 60  ;
            
const UBYTE THERMISTOR_PORT = 10,
            HEATER_PORT     = A1;
            
const int   R_BALANCE       = 10000, //Ohm
            BIAS            = 45,
            MAX_OUTPUT      = 255 - BIAS;
            
            
extern float activeTemperature,
             idleTemperature,
             temperatureTolerance;
            
void initExtruder();
float getExtruderTemperature();
void temperatureWorker(const ULONG);
void startTemperatureControl(bool);
void stopTemperatureControl();

#endif
