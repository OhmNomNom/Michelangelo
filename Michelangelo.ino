#include "TimerThree.h"
#include "Interpreter.h"
#include "Extruder.h"
#include "Base.h"
  
void setup() {
  Serial.begin(9600);
  
  initAxes();
  initInterpreter();
  initExtruder();
  
  Timer3.attachInterrupt(timerInterrupt, WORKER_PERIOD); //Run the interrupt every WORKER_PERIOD
}

void loop() {
    while (Serial.available()) interpret(Serial.read()); //If we're getting commands, interpret them
    flushSerial(); //Flush any output put in the buffer
}

void timerInterrupt() {
  static ULONG lastPIDMicros = 0, //Last time we ran the heater PID thing
    prevNow = 0; //Last time we ran this function
  
  const ULONG now = micros();
  timeInterval = now - prevNow; //Store it for debugging
  prevNow = now;

  if(isFlagSet(FLAG_ENABLE)) stepperWorker(now); //Move the motors

  if(isFlagSet(FLAG_HOTEND_ON) && lastPIDMicros + PID_PERIOD <= now) {
    lastPIDMicros = now;
    temperatureWorker(now); //Do the heater PID thing
  }
    
}
