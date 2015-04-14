#ifndef Interpreter_h_
#define Interpreter_h_

#include "Base.h"
#include "Axes.h"

static const UBYTE CMDBUFFER_SIZE = 15;
static const UBYTE DISPLAY_PRECISION = 2;

static const UBYTE FLAG_ABSOLUTE = 0x01;

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
  CMD_MODEINC
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
extern UBYTE bufferPosition,
             stateFlags;
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

#endif
