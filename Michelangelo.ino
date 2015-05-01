#include "TimerThree.h"
#include "Interpreter.h"
#include "Extruder.h"
#include "Base.h"
  
void setup() {
  Serial.begin(9600);
  initAxes();
  initInterpreter();
  initExtruder();
  Timer3.attachInterrupt(timerInterrupt, WORKER_PERIOD);
}

void loop() {
    while (Serial.available()) interpret(Serial.read());
    flushSerial();
}

void timerInterrupt() {
  static ULONG lastPIDMicros = 0;
  const ULONG now = micros();

  if(stateFlags & FLAG_ENABLE) stepperWorker(now);

  if(getExtruderTemperature() > EXT_MAX_TEMP)
    analogWrite(HEATER_PORT,0);
  else if((stateFlags & FLAG_HOTEND_ON) && (lastPIDMicros + PID_PERIOD <= now)) {
    lastPIDMicros = now;
    temperatureWorker(now); 
  }
    
}
