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
    delay(100);
}

void timerInterrupt() {
  static ULONG lastPIDMicros = 0,
    prevNow = 0;
  const ULONG now = micros();
  timeInterval = now - prevNow;
  prevNow = now;

  if(isFlagSet(FLAG_ENABLE)) stepperWorker(now);

  if(isFlagSet(FLAG_HOTEND_ON) && lastPIDMicros + PID_PERIOD <= now) {
    lastPIDMicros = now;
    temperatureWorker(now); 
  }
    
}
