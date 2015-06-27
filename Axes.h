#ifndef Axes_h_
#define Axes_h_

#include "Base.h"
#include "Interpreter.h"

static const UBYTE X_MOVPORT   = 9,
                   Y_MOVPORT   = 6,
                   Z_MOVPORT   = 4,
                   E_MOVPORT   = 16,
                  
                   X_DIRPORT   = 8,
                   Y_DIRPORT   = 7,
                   Z_DIRPORT   = 5,
                   E_DIRPORT   = 14;
                  
static const UINT  X_STEPSMILLI = 315,
                   Y_STEPSMILLI = 315,
                   Z_STEPSMILLI = 630,
                   E_STEPSMILLI = 194;
                  
static const bool  X_INVERT    = false,
                   Y_INVERT    = false,
                   Z_INVERT    = false,
                   E_INVERT    = false;
                  
static const float X_MAXSPEED  = 20.0,
                   Y_MAXSPEED  = 20.0,
                   Z_MAXSPEED  = 20.0,
                   E_MAXSPEED  = 0.5,
                   MAXSPEED_LINEAR = 20.0;


static const UBYTE FLAG     [] = {FLAG_X    ,FLAG_Y    ,FLAG_Z    ,FLAG_E    },
                   MOVPORT  [] = {X_MOVPORT ,Y_MOVPORT ,Z_MOVPORT ,E_MOVPORT },
                   DIRPORT  [] = {X_DIRPORT ,Y_DIRPORT ,Z_DIRPORT ,E_DIRPORT };
                   
static const UINT  STEPSMILLI [] = {X_STEPSMILLI,Y_STEPSMILLI,Z_STEPSMILLI,E_STEPSMILLI};

static const float MAXSPEED [] = {X_MAXSPEED,Y_MAXSPEED,Z_MAXSPEED,E_MAXSPEED},
                   STEPLENGTH[] = {1.0f/X_STEPSMILLI,1.0f/Y_STEPSMILLI,1.0f/Z_STEPSMILLI,1.0f/E_STEPSMILLI} ;
                  
static const bool  INVERT   [] = {X_INVERT  ,Y_INVERT  ,Z_INVERT  ,E_INVERT  };

struct Axis {
  ULONG stepTime;
  ULONG steps;
  SLONG lastMicros;
};

//extern Axis Axes[];
extern volatile SLONG axisPosition[];
extern volatile SBYTE axisDirection[];
void initAxes();
void resetAxes();
bool moveAxis(ParamIndex,float,float);
void stepperWorker(const ULONG);
void startStepperControl();
void stopStepperControl();

#endif
