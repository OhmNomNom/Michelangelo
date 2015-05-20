#ifndef Interpreter_h_
#define Interpreter_h_

#include "Base.h"
#include "Axes.h"
#include "Extruder.h"

static const UBYTE CMDBUFFER_SIZE = 16;

enum Command : UBYTE {
  CMD_NONE = 0,
  CMD_FLAGS,
  CMD_RPOS,
  CMD_LINEAR,
  CMD_ECHO,
  CMD_HALT,
  CMD_RESUME,
  CMD_RECOVER,
  CMD_POSITION,
  CMD_MODEABS,
  CMD_MODEINC,
  CMD_GETTEMP,
  CMD_HOTEND_ON,
  CMD_HOTEND_OFF,
  CMD_SETTEMP,
  CMD_SETPOS
};

enum CommandStates : UBYTE {
  STATE_LINENUM = 0,
  STATE_COMMAND,
  STATE_PARAMS,
  STATE_CHECKSUM,
  
  STATE_INVALID = 255
};

extern UINT cmdLine,
            lineCounter,
            movementLine;
extern Command command;
extern UBYTE bufferPosition;
extern CommandStates cmdState;
extern char cmdBuffer[],
            checksum;
extern float cmdParams[];


void initInterpreter();
void interpret(char);
void process();
void execCommand();
inline void clearPrevCommand();
inline void invalidCommand();
inline void acknowledgeCommand();
void doneMoving();
void rapidPositioning();
void linearInterpolation();
void cmdEcho();
inline void cmdHalt();
void cmdResume();
void cmdRecover();
void cmdPosition();
void cmdSetTemperature();
void cmdSetPos();

#endif
