#include "Axes.h"
         
Axis Axes[4];
volatile SLONG axisPosition[4];
volatile SBYTE axisDirection[4];

void initAxes() {
  for(int i = 0; i < 4; i++){
    pinMode(MOVPORT[i], OUTPUT);
    pinMode(DIRPORT[i], OUTPUT);
    axisPosition[i] = 0;
    digitalWrite(MOVPORT[i], HIGH);
    digitalWrite(DIRPORT[i], INVERT[i]);
  }
}

void resetAxes() {
  Axes[X].steps = 0;
  Axes[Y].steps = 0;
  Axes[Z].steps = 0;
  Axes[E].steps = 0;
}

bool moveAxis(ParamIndex axis, float distance, float rate) {
  
  if(isFlagSet(FLAG[axis]) || (rate > MAXSPEED[axis])) return false;
  if(distance == 0) return true;

  axisDirection[axis] = ((distance < 0) xor (INVERT[axis]))?0:1;
  digitalWrite(DIRPORT[axis], axisDirection[axis]);
  axisDirection[axis] = (axisDirection[axis]?1:-1);
  
  if(distance < 0) distance = -distance;
  
  Axes[axis].steps      = distance * STEPSMILLI[axis];
  Axes[axis].stepTime   = round(1000000UL / (rate * STEPSMILLI[axis]));
  Axes[axis].lastMicros = 0;
  
  setFlag(FLAG[axis]);
  return true;
}

void stepperWorker(const ULONG now) {
  
  //if((stateFlags & FLAG_HOTEND_ON) && (abs(currentTemperature - activeTemperature) > temperatureTolerance)) return;
  
  if(!isFlagSet(FLAGS_AXES)) {
    stopStepperControl();
    return;
  }    
 
  if(isFlagSet(FLAG[X]) && now - Axes[X].lastMicros >= Axes[X].stepTime) {
    if(Axes[X].steps) {
      digitalWrite(MOVPORT[X], LOW);
      digitalWrite(MOVPORT[X], HIGH);
      Axes[X].steps--;
      Axes[X].lastMicros = now;
      axisPosition[X] += axisDirection[X];
    } else unsetFlag(FLAG[X]);
  }
  
  if(isFlagSet(FLAG[Y]) && now - Axes[Y].lastMicros >= Axes[Y].stepTime) {
    if(Axes[Y].steps) {
      digitalWrite(MOVPORT[Y], LOW);
      digitalWrite(MOVPORT[Y], HIGH);
      Axes[Y].steps--;
      Axes[Y].lastMicros = now;
      axisPosition[Y] += axisDirection[Y];
    } else unsetFlag(FLAG[Y]);
  }
  
  if(isFlagSet(FLAG[Z]) && now - Axes[Z].lastMicros >= Axes[Z].stepTime) {
    if(Axes[Z].steps) {
      digitalWrite(MOVPORT[Z], LOW);
      digitalWrite(MOVPORT[Z], HIGH);
      Axes[Z].steps--;
      Axes[Z].lastMicros = now;
      axisPosition[Z] += axisDirection[Z];
    } else unsetFlag(FLAG[Z]);
  }
  
  if(isFlagSet(FLAG[E]) && now - Axes[E].lastMicros >= Axes[E].stepTime) {
    if(Axes[E].steps) {
      digitalWrite(MOVPORT[E], LOW);
      digitalWrite(MOVPORT[E], HIGH);
      Axes[E].steps--;
      Axes[E].lastMicros = now;
      axisPosition[E] += axisDirection[E];
    } else unsetFlag(FLAG[E]);
  }
}

void startStepperControl() {
  setFlag(FLAG_ENABLE);
}

void stopStepperControl() {
  unsetFlag(FLAG_ENABLE);
  doneMoving();
}
