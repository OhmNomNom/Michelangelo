#include "Extruder.h"

float targetTemperature = 25.0f,
      temperatureRange  = 2.0f;

void initExtruder() {
  pinMode(HEATER_PORT,OUTPUT);
}

float getExtruderTemperature() {
  float R,T;

  R = float(analogRead(THERMISTOR_PORT)); // the value
  R = R  / (1024.0f - R); //=Rtherm/Rbalance
  R = R_BALANCE * R;

  T  = 1.0f / ( (1.0f/EXT_T0) + (1.0f/EXT_BETA)*log(R/EXT_R0) ); //Beta factor method
  T -= ZERO_C; //T in celsius

  return T;
}

void temperatureWorker() {
  const float currentTemperature = getExtruderTemperature();

  if (currentTemperature <= targetTemperature - temperatureRange) digitalWrite(HEATER_PORT, HIGH);
  else if (currentTemperature > targetTemperature + temperatureRange) digitalWrite(HEATER_PORT, LOW);
  /* PID
  static ULONG prevTime = micros() - PID_PERIOD;
  static float iTerm = 0,
               prevTemperature = targetTemperature;
  
  if(!(stateFlags & FLAG_HOTEND_ON)) return;
  
  const ULONG now = micros();
  const int deltaT = now - prevTime;
  if(deltaT < 0) return;
  prevTime = now;
  const float curTemperature = getExtruderTemperature();

  const float error = targetTemperature - curTemperature;  

  const float pTerm = KP * error;
  iTerm += KI * error;
  const float dTerm = KD * (curTemperature - prevTemperature); //dInput/dt, to avoid "set point change kick"
  
  if(iTerm > MAX_OUTPUT) iTerm = MAX_OUTPUT;
  else if(iTerm < 0)     iTerm = 0;
  
  UINT output = pTerm + iTerm + dTerm;
  
  if(output > MAX_OUTPUT) output = MAX_OUTPUT;
  else if(output < 0)     output = 0;
  
  analogWrite(HEATER_PORT,output);
  */
}

void startTemperatureWorker() {
  Timer3.attachInterrupt(temperatureWorker, PID_PERIOD);
  stateFlags |= FLAG_HOTEND_ON;
}

void stopTemperatureWorker() {
  stateFlags &= ~FLAG_HOTEND_ON;
  analogWrite(HEATER_PORT,0);
  Timer3.detachInterrupt();
}
