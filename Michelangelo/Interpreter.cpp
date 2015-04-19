#include "Interpreter.h"
#include "Axes.h"

UINT cmdLine,
     lineCounter,
     movementLine;
Command command;
UBYTE bufferPosition,
      stateFlags = 0x00;
CommandStates cmdState;
char cmdBuffer[CMDBUFFER_SIZE+1],
     checksum;
float cmdParams[6],
      feedRate,
      extrudeRate;

void initInterpreter() {
  clearPrevCommand();
  lineCounter = 1;
  feedRate = MAXSPEED_LINEAR / 2;
  extrudeRate = MAXSPEED[E] / 4;
}

void interpret(char c) {
  if(cmdState != STATE_CHECKSUM) checksum ^= c;
  switch(c) {
  case ';':
  case '\n':
    checksum ^= c; //Undo the xor done above
    execCommand();
    clearPrevCommand();
    return;
  case '*':
    checksum ^= '*';
    if(cmdState == STATE_INVALID) return;
    cmdState = STATE_CHECKSUM;
    bufferPosition = 0;
    break;
  case ' ': 
    if(cmdState == STATE_INVALID) return;
    process();
    break;
  default:
    if(cmdState == STATE_INVALID) return;
    cmdBuffer[bufferPosition++] = c;
  }
  
  if(bufferPosition >= CMDBUFFER_SIZE)
    cmdState = STATE_INVALID;
}

void process() {
  cmdBuffer[bufferPosition] = '\0';
reparse:
  switch(cmdState) {
  case STATE_LINENUM:
    if(cmdBuffer[0] != 'N') {
      cmdLine = lineCounter;
      cmdState = STATE_COMMAND;
      goto reparse;
    }
    if(bufferPosition < 2) {
      cmdState = STATE_INVALID;
      break;
    }
    cmdLine = sParseUINT(cmdBuffer+1);
    if(cmdLine != lineCounter) {
      cmdLine = lineCounter;
      cmdState = STATE_INVALID;
      break;
    }
    cmdState = STATE_COMMAND;
    break;
  case STATE_COMMAND:
    if((axisFlags) && cmdBuffer[0] != 'M') {
      cmdState = STATE_INVALID;
      break;
    } else if (strEqual(cmdBuffer,"M70"))
      command = CMD_FLAGS;
    else if (strEqual(cmdBuffer,"G00"))
      command = CMD_RPOS;
    else if (strEqual(cmdBuffer,"G01"))
      command = CMD_LINEAR;
    else if (strEqual(cmdBuffer,"G90"))
      command = CMD_MODEABS;
    else if (strEqual(cmdBuffer,"G91"))
      command = CMD_MODEINC;
    else if (strEqual(cmdBuffer,"M77"))
      command = CMD_ECHO;
    else if (strEqual(cmdBuffer,"M00"))
      command = CMD_HALT;
    else if (strEqual(cmdBuffer,"M97"))
      command = CMD_RESUME;
    else if (strEqual(cmdBuffer,"M96"))
      command = CMD_RECOVER;
    else if (strEqual(cmdBuffer,"M114"))
      command = CMD_POSITION;
    else {
      cmdState = STATE_INVALID;
      break;
    }
    cmdState = STATE_PARAMS;  
    break;
  case STATE_PARAMS:
    if(bufferPosition < 2) {
      cmdState = STATE_INVALID;
      break;
    }
    float input;
    input = sParseFloat(cmdBuffer+1);
    if (isnan(input)) {
      cmdState = STATE_INVALID;
      break;
    }
    switch(cmdBuffer[0]) {
      case 'X':
        cmdParams[X] = input;
        break;
      case 'Y':
        cmdParams[Y] = input;
        break;
      case 'Z':
        cmdParams[Z] = input;
        break;
      case 'E':
        cmdParams[E] = input;
        break;
      case 'F':
        cmdParams[F] = input;
        break;
      case 'D':
        cmdParams[D] = input;
        break;
      default:
        cmdState = STATE_INVALID;
        break;
    }
    break;
  case STATE_CHECKSUM:
    UINT check = sParseUINT(cmdBuffer);
    if(checksum != check) cmdState = STATE_INVALID;
    break;
  }
  bufferPosition = 0;
}

void execCommand() {
  if(bufferPosition != 0) process();
  if(cmdState == STATE_INVALID) {
    invalidCommand();
    return;
  }
  noInterrupts();
  switch(command) {
  case CMD_NONE:
    Serial.print("NONE N");
    Serial.print((UINT)cmdLine);
    Serial.print('\n');
    break;
  case CMD_FLAGS:
    Serial.print("N");
    Serial.print((UINT)cmdLine);
    Serial.print(" M70 ");
    Serial.print(axisFlags);
    Serial.print('\n');
    break;
  case CMD_RPOS:
    rapidPositioning();
    break;
  case CMD_LINEAR:
    linearInterpolation();
    break;
  case CMD_ECHO:
    cmdEcho();
    break;
  case CMD_HALT:
    cmdHalt();
    break;
  case CMD_RESUME:
    cmdResume();
    break;
  case CMD_RECOVER:
    cmdRecover();
    break;
  case CMD_POSITION:
    cmdPosition();
    break;
  case CMD_MODEABS:
    stateFlags |= FLAG_ABSOLUTE;
    acknowledgeCommand();
    break;
  case CMD_MODEINC:
    stateFlags &= ~FLAG_ABSOLUTE;
    acknowledgeCommand();
    break;
  }
  interrupts();
  lineCounter++;
}

inline void clearPrevCommand() { 
    command = CMD_NONE;
    cmdState = STATE_LINENUM;
    bufferPosition = 0;
    checksum = 0;
    cmdParams[X] = 0;
    cmdParams[Y] = 0;
    cmdParams[Z] = 0;
    cmdParams[E] = 0;
    cmdParams[F] = 0;
    cmdParams[D] = 0;
}

void doneMoving() {
  Serial.print("DONE N");
  Serial.print(movementLine);
  Serial.print("\n");
}

inline void invalidCommand() {
  Serial.print("INVALID N");
  Serial.print((UINT)cmdLine);
  Serial.print("\n");
  lineCounter = cmdLine;
}

inline void acknowledgeCommand() {
  Serial.print("ACK N");
  Serial.print((UINT)cmdLine);
  Serial.print("\n");
}

void rapidPositioning() {
  float motion[3];
  if(stateFlags & FLAG_ABSOLUTE) {
    motion[X] = cmdParams[X] - axisPosition[X]*STEPLENGTH[X];
    motion[Y] = cmdParams[Y] - axisPosition[Y]*STEPLENGTH[Y];
    motion[Z] = cmdParams[Z] - axisPosition[Z]*STEPLENGTH[Z];
  } else {
    motion[X] = cmdParams[X];
    motion[Y] = cmdParams[Y];
    motion[Z] = cmdParams[Z];
  }
  acknowledgeCommand();
  moveAxis(X,motion[X],MAXSPEED[X]);
  moveAxis(Y,motion[Y],MAXSPEED[Y]);
  moveAxis(Z,motion[Z],MAXSPEED[Z]);
  movementLine = cmdLine;
  startStepperWorker();
}
void linearInterpolation() {
  if(cmdParams[F] > MAXSPEED_LINEAR || cmdParams[D] > MAXSPEED[E]) {
    invalidCommand();
    return;
  }
  
  float motion[3];
  if(stateFlags & FLAG_ABSOLUTE) {
    motion[X] = cmdParams[X] - axisPosition[X]*STEPLENGTH[X];
    motion[Y] = cmdParams[Y] - axisPosition[Y]*STEPLENGTH[Y];
    motion[Z] = cmdParams[Z] - axisPosition[Z]*STEPLENGTH[Z];
  } else {
    motion[X] = cmdParams[X];
    motion[Y] = cmdParams[Y];
    motion[Z] = cmdParams[Z];
  }
  
  float travel = sqrt(pow2(motion[X]) + pow2(motion[Y]) + pow2(motion[Z]));
  
  if(cmdParams[F] > 0) feedRate = cmdParams[F];
  
  float velocity[3];
  velocity[X] = abs(feedRate * (motion[X] / travel));
  velocity[Y] = abs(feedRate * (motion[Y] / travel));
  velocity[Z] = abs(feedRate * (motion[Z] / travel));
  velocity[E] = abs(cmdParams[E] * (feedRate / travel));
  
  acknowledgeCommand();
  moveAxis(X,motion[X],velocity[X]);
  moveAxis(Y,motion[Y],velocity[Y]);
  moveAxis(Z,motion[Z],velocity[Z]);
  moveAxis(E,cmdParams[E],velocity[E]);
  movementLine = cmdLine;
  startStepperWorker();
}


void cmdEcho() {
  Serial.print("ECHO N");
  Serial.print((UINT)cmdLine);
  Serial.print(" X");
  Serial.print(cmdParams[X],DISPLAY_PRECISION);
  Serial.print(" Y");
  Serial.print(cmdParams[Y],DISPLAY_PRECISION);
  Serial.print(" Z");
  Serial.print(cmdParams[Z],DISPLAY_PRECISION);
  Serial.print(" E");
  Serial.print(cmdParams[F],DISPLAY_PRECISION);
  Serial.print(" F");
  Serial.print(cmdParams[E],DISPLAY_PRECISION);
  Serial.print(" D");
  Serial.print(cmdParams[D],DISPLAY_PRECISION);
  Serial.print("\n");
}

inline void cmdHalt() {
  axisFlags &= ~FLAG_ENABLE;
  Serial.print("HALT N");
  Serial.print((UINT)cmdLine);
  Serial.print('\n');
}
void cmdResume() {
  if((axisFlags & FLAG_ENABLE) || !(axisFlags & ~FLAG_ENABLE)) {
    invalidCommand();
    return;
  }
  Serial.print("RESUME N");
  Serial.print((UINT)cmdLine);
  Serial.print('\n');
  axisFlags |= FLAG_ENABLE;
}
void cmdRecover() {
  resetAxes(); 
  if((axisFlags & FLAG_ENABLE) || !(axisFlags & ~FLAG_ENABLE)) {
    invalidCommand();
    return;
  }
  Serial.print("RECOVER N");
  Serial.print((UINT)cmdLine);
  Serial.print('\n');
  axisFlags = FLAG_ENABLE;
}

void cmdPosition() {
  Serial.print("POS N");
  Serial.print((UINT)cmdLine);
  Serial.print(" X");
  Serial.print(axisPosition[X]*STEPLENGTH[X],DISPLAY_PRECISION);
  Serial.print(" Y");
  Serial.print(axisPosition[Y]*STEPLENGTH[Y],DISPLAY_PRECISION);
  Serial.print(" Z");
  Serial.print(axisPosition[Z]*STEPLENGTH[Z],DISPLAY_PRECISION);
  Serial.print(" E");
  Serial.print(axisPosition[E]*STEPLENGTH[E],DISPLAY_PRECISION);
  Serial.print("\n");
}
