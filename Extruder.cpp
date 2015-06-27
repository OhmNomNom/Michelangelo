#include "Extruder.h"

float activeTemperature = 200.0f,
      idleTemperature = 140.0f,
      temperatureTolerance = 5.0f;

void initExtruder() {
  pinMode(HEATER_PORT,OUTPUT);
  pinMode(THERMISTOR_PORT,INPUT);
  analogWrite(HEATER_PORT,0);
}

float getExtruderTemperature() {
  float R,T;

  R = float(analogRead(THERMISTOR_PORT)); // the value
  R = (1024.0f - R) / R ; //=Rtherm/Rbalance
   R = R_BALANCE * R;

  T  = 1.0f / ( (1.0f/EXT_T0) + (1.0f/EXT_BETA)*log(R/EXT_R0) ); //Beta factor method
  T -= ZERO_C; //T in celsius

  return T;
}

void temperatureWorker(const ULONG now) { 
  const int &targetTemperature = (stateFlags & FLAG_ACTIVE_TEMP) ? activeTemperature : idleTemperature;
  
  if(getExtruderTemperature() >= EXTRUDER_MAX_TEMP) {
    analogWrite(HEATER_PORT,0);
  }
  //if (currentTemperature <= targetTemperature - temperatureRange) digitalWrite(HEATER_PORT, HIGH);
  //else if (currentTemperature > targetTemperature + temperatureRange) digitalWrite(HEATER_PORT, LOW);
  
  static ULONG prevTime = now - PID_PERIOD;
  static float iTerm = 0,
               prevError = 0;
  
  
  const float currentTemperature = getExtruderTemperature();
  
  if(!isFlagSet(FLAG_HOTEND_ON)) return;
  
  const ULONG deltaT = now - prevTime;
  if(deltaT < 0) return;
  prevTime = now;

  const float error = targetTemperature - currentTemperature;  

  const float pTerm = KP * error;
  iTerm *= KI2;
  iTerm += KI * error;
  const float dTerm = KD * (error - prevError); //dInput/dt, to avoid "set point change kick"
  
  prevError = error;
  
  if(iTerm > MAX_OUTPUT) iTerm = MAX_OUTPUT;
  else if(iTerm < 0)     iTerm = 0;
  
  //if (currentTemperature > targetTemperature - temperatureRange || currentTemperature < targetTemperature + temperatureRange )
  //  iTerm = 0;
  
  int output = pTerm + iTerm + dTerm;
  
  if(output > MAX_OUTPUT) output = MAX_OUTPUT;
  else if(output + BIAS < 0) output = -BIAS;
  
  analogWrite(HEATER_PORT,BIAS + output);
}

void startTemperatureControl(bool isActive) {
  setFlag(FLAG_HOTEND_ON);
  if(isActive) setFlag(FLAG_ACTIVE_TEMP);
  else unsetFlag(FLAG_ACTIVE_TEMP);
}

void stopTemperatureControl() {
  unsetFlag(FLAG_HOTEND_ON);
  analogWrite(HEATER_PORT,0);
}
