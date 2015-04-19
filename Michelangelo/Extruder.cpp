#include "Extruder.h"

float getExtruderTemperature() {
  float R,T;

  R = R_BALANCE * ((1024.0f/float(analogRead(THERMISTOR_PORT))) - 1);

  T  = 1.0f / ( (1.0f/EXT_T0) + (1.0f/EXT_BETA)*log(R/EXT_R0) ); //Beta factor method
  T -= ZERO_C; //T in celsius

  return T;
}
