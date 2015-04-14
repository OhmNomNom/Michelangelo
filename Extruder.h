#ifndef Extruder_h_
#define Extruder_h_

#include "Base.h"
#include "math.h"

const float EXT_BETA  = 3950,
            EXT_T0    = 298.15,
            EXT_R0    = 100000,
            
            ZERO_C = 273.15;
            
const UBYTE THERMISTOR_PORT = A0,
            R_BALANCE       = 47000;

float getExtruderTemperature();

#endif
