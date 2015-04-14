#include "Axes.h"
         
volatile Axis Axes[4];          
volatile UBYTE axisFlags = 0x00;
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

bool moveAxis(AxisIndex axis, float distance, float rate) {
  if((FLAG[axis] & axisFlags) || (rate > MAXSPEED[axis])) return false;
  if(distance == 0) return true;

  axisDirection[axis] = ((distance < 0) xor (INVERT[axis]))?0:1;
  digitalWrite(DIRPORT[axis], axisDirection[axis]);
  axisDirection[axis] = (axisDirection[axis]?1:-1);
  
  if(distance < 0) distance = -distance;
  
  Axes[axis].steps      = distance * STEPSREV[axis] * REVSMILI[axis];
  Axes[axis].stepTime   = 1000000UL / (rate * STEPSREV[axis] * REVSMILI[axis]);
  Axes[axis].lastMicros = 0;
  
  axisFlags |= FLAG[axis];
  return true;
}

void stepperWorker() {
  if(!(axisFlags & FLAG_ENABLE)) return;
  if(!(axisFlags & ~FLAG_ENABLE)) {
    stopStepperWorker();
    return;
  }
  const ULONG mic = micros(); 
 
  if(FLAG[X] & axisFlags && mic - Axes[X].lastMicros >= Axes[X].stepTime) {
    if(Axes[X].steps) {
      digitalWrite(MOVPORT[X], LOW);
      digitalWrite(MOVPORT[X], HIGH);
      Axes[X].steps--;
      Axes[X].lastMicros = mic;
      axisPosition[X] += axisDirection[X];
    } else axisFlags &= ~FLAG[X];
  }
  
  if(FLAG[Y] & axisFlags && mic - Axes[Y].lastMicros >= Axes[Y].stepTime) {
    if(Axes[Y].steps) {
      digitalWrite(MOVPORT[Y], LOW);
      digitalWrite(MOVPORT[Y], HIGH);
      Axes[Y].steps--;
      Axes[Y].lastMicros = mic;
      axisPosition[Y] += axisDirection[Y];
    } else axisFlags &= ~FLAG[Y];
  }
  
  if(FLAG[Z] & axisFlags && mic - Axes[Z].lastMicros >= Axes[Z].stepTime) {
    if(Axes[Z].steps) {
      digitalWrite(MOVPORT[Z], LOW);
      digitalWrite(MOVPORT[Z], HIGH);
      Axes[Z].steps--;
      Axes[Z].lastMicros = mic;
      axisPosition[Z] += axisDirection[Z];
    } else axisFlags &= ~FLAG[Z];
  }
  
  if(FLAG[E] & axisFlags && mic - Axes[E].lastMicros >= Axes[E].stepTime) {
    if(Axes[E].steps) {
      digitalWrite(MOVPORT[E], LOW);
      digitalWrite(MOVPORT[E], HIGH);
      Axes[E].steps--;
      Axes[E].lastMicros = mic;
      axisPosition[E] += axisDirection[E];
    } else axisFlags &= ~FLAG[E];
  }
}

void startStepperWorker() {
  Timer1.attachInterrupt(stepperWorker, WORKER_PERIOD);
  axisFlags |= FLAG_ENABLE;
}

void stopStepperWorker() {
  axisFlags &= ~FLAG_ENABLE;
  Timer1.detachInterrupt();
  doneMoving();
}
