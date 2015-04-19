#ifndef Axes_h_
#define Axes_h_

#include "Base.h"
#include "Interpreter.h"
#include "TimerOne.h"

static const UBYTE X_FLAG      = 0x01,
                   Y_FLAG      = 0x02,
                   Z_FLAG      = 0x04,
                   E_FLAG      = 0x08,
                   FLAG_ENABLE = 0x10,
                  
                   X_MOVPORT   = 4,
                   Y_MOVPORT   = 5,
                   Z_MOVPORT   = 5,
                   E_MOVPORT   = 5,
                  
                   X_DIRPORT   = 7,
                   Y_DIRPORT   = 5,
                   Z_DIRPORT   = 5,
                   E_DIRPORT   = 5;
                  
static const UINT  X_STEPSREV  = 800,
                   Y_STEPSREV  = 800,
                   Z_STEPSREV  = 800,
                   E_STEPSREV  = 800,
                  
                   X_REVSMILI  = 1,
                   Y_REVSMILI  = 1,
                   Z_REVSMILI  = 1,
                   E_REVSMILI  = 1;
                  
static const bool  X_INVERT    = false,
                   Y_INVERT    = false,
                   Z_INVERT    = false,
                   E_INVERT    = false;
                  
static const float X_MAXSPEED  = 4.2,
                   Y_MAXSPEED  = 4.2,
                   Z_MAXSPEED  = 4.2,
                   E_MAXSPEED  = 4.2,
                   MAXSPEED_LINEAR = 4.2;

static const ULONG WORKER_PERIOD = 10;


static const UBYTE FLAG     [] = {X_FLAG    ,Y_FLAG    ,Z_FLAG    ,E_FLAG    },
                   MOVPORT  [] = {X_MOVPORT ,Y_MOVPORT ,Z_MOVPORT ,E_MOVPORT },
                   DIRPORT  [] = {X_DIRPORT ,Y_DIRPORT ,Z_DIRPORT ,E_DIRPORT };
                   
static const UINT  STEPSREV [] = {X_STEPSREV,Y_STEPSREV,Z_STEPSREV,E_STEPSREV},
                   REVSMILI [] = {X_REVSMILI,Y_REVSMILI,Z_REVSMILI,E_REVSMILI};

static const float MAXSPEED [] = {X_MAXSPEED,Y_MAXSPEED,Z_MAXSPEED,E_MAXSPEED},

                   STEPLENGTH[] = {float(X_REVSMILI)/X_STEPSREV, float(Y_REVSMILI)/Y_STEPSREV, float(Z_REVSMILI)/Z_STEPSREV, float(E_REVSMILI)/E_STEPSREV} ;
                  
static const bool  INVERT   [] = {X_INVERT  ,Y_INVERT  ,Z_INVERT  ,E_INVERT  };


enum AxisIndex : UBYTE {
  X = 0,
  Y,
  Z,
  E,
  F,//Feed rate
  D //Extrusion rate
};

struct Axis {
  ULONG stepTime,
        steps,
        lastMicros;
};

extern volatile Axis Axes[];
extern volatile UBYTE axisFlags;
extern volatile SLONG axisPosition[];
extern volatile SBYTE axisDirection[];
void initAxes();
void resetAxes();
bool moveAxis(AxisIndex,float,float);
void stepperWorker();
void startStepperWorker();
void stopStepperWorker();

#endif
