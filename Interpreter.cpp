#include "Interpreter.h"

UINT cmdLine,
     lineCounter,
     movementLine;
Command command;
UBYTE bufferPosition;
CommandStates cmdState;
char cmdBuffer[CMDBUFFER_SIZE+1],
     checksum;
float cmdParams[6],
      feedRate = MAXSPEED_LINEAR / 2,
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
    if(cmdLine < lineCounter) {
      cmdLine = lineCounter;
      cmdState = STATE_INVALID;
      break;
    }
    cmdState = STATE_COMMAND;
    break;
  case STATE_COMMAND:
    if((stateFlags) && cmdBuffer[0] != 'M') {
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
    else if (strEqual(cmdBuffer,"M105"))
      command = CMD_GETTEMP;
    else if (strEqual(cmdBuffer,"M03"))
      command = CMD_HOTEND_ON;
    else if (strEqual(cmdBuffer,"M05"))
      command = CMD_HOTEND_OFF;
    else if (strEqual(cmdBuffer,"M104"))
      command = CMD_SETTEMP;
    else if (strEqual(cmdBuffer,"G92"))
      command = CMD_SETPOS;
    else {
      cmdState = STATE_INVALID;
      break;
    }
    cmdState = STATE_PARAMS; 
    cmdParams[X] = cmdParams[Y] = cmdParams[Z] = cmdParams[E] = cmdParams[F] = cmdParams[S] = NAN;
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
      case 'S':
        cmdParams[S] = input;
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
    addToBufferS("NONE N",6);
    addToBufferI(cmdLine);
    addToBufferC('\n');
    break;
  case CMD_FLAGS:
    addToBufferC('N');
    addToBufferI(cmdLine);
    addToBufferS(" M70 ",5);
    addToBufferI(stateFlags);
    addToBufferC(' ');
    addToBufferI(stateFlags);
    addToBufferC('\n');
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
    stateFlags |= FLAG_ABSOLUTE_MODE;
    acknowledgeCommand();
    break;
  case CMD_MODEINC:
    stateFlags &= ~FLAG_ABSOLUTE_MODE;
    acknowledgeCommand();
    break;
  case CMD_SETTEMP:
    cmdSetTemperature();
    break;
  case CMD_SETPOS:
    cmdSetPos();
    break;
  case CMD_GETTEMP:
    addToBufferS("M105 T",6);
    addToBufferF(getExtruderTemperature());
    addToBufferS(" S",2);    
    addToBufferF(targetTemperature);
    addToBufferS(" F",2);    
    addToBufferF(temperatureRange);
    addToBufferC('\n');
    break;
  case CMD_HOTEND_ON:
    startTemperatureWorker();
    acknowledgeCommand();
    break;
  case CMD_HOTEND_OFF:
    stopTemperatureWorker();
    acknowledgeCommand();
    break;
  }
  interrupts();
  lineCounter = cmdLine + 1;
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
    cmdParams[S] = 0;
}

void doneMoving() {
  addToBufferS("DONE N",6);
  addToBufferI(movementLine);
  addToBufferC('\n');
}

inline void invalidCommand() {
  addToBufferS("INVALID N",9);
  addToBufferI(cmdLine);
  addToBufferC('\n');
  lineCounter = cmdLine;
}

inline void acknowledgeCommand() {
  addToBufferS("ACK N",5);
  addToBufferI(cmdLine);
  addToBufferC('\n');
  flushSerial();
}

void rapidPositioning() {
  float motion[3];
  if(stateFlags & FLAG_ABSOLUTE_MODE) {
    motion[X] = isnan(cmdParams[X])?0:(cmdParams[X] - axisPosition[X]*STEPLENGTH[X]);
    motion[Y] = isnan(cmdParams[Y])?0:(cmdParams[Y] - axisPosition[Y]*STEPLENGTH[Y]);
    motion[Z] = isnan(cmdParams[Z])?0:(cmdParams[Z] - axisPosition[Z]*STEPLENGTH[Z]);
  } else {
    motion[X] = isnan(cmdParams[X])?0:cmdParams[X];
    motion[Y] = isnan(cmdParams[Y])?0:cmdParams[Y];
    motion[Z] = isnan(cmdParams[Z])?0:cmdParams[Z];
  }
  float feedRate = NAN;
  if(cmdParams[F] > 0) feedRate = cmdParams[F];
  
  acknowledgeCommand();
  if(!isnan(cmdParams[X])) moveAxis(X,motion[X],isnan(feedRate)?MAXSPEED[X]:feedRate);
  if(!isnan(cmdParams[Y])) moveAxis(Y,motion[Y],isnan(feedRate)?MAXSPEED[Y]:feedRate);
  if(!isnan(cmdParams[Z])) moveAxis(Z,motion[Z],isnan(feedRate)?MAXSPEED[Z]:feedRate);
  if(!isnan(cmdParams[E])) moveAxis(E,cmdParams[E],isnan(feedRate)?MAXSPEED[E]:feedRate);

  
  movementLine = cmdLine;
  startStepperWorker();
}
void linearInterpolation() {
  if(cmdParams[F] > MAXSPEED_LINEAR || cmdParams[S] > MAXSPEED[E]) {
    invalidCommand();
    return;
  }
  
  float motion[3];
  if(stateFlags & FLAG_ABSOLUTE_MODE) {
    motion[X] = isnan(cmdParams[X])?0:(cmdParams[X] - axisPosition[X]*STEPLENGTH[X]);
    motion[Y] = isnan(cmdParams[Y])?0:(cmdParams[Y] - axisPosition[Y]*STEPLENGTH[Y]);
    motion[Z] = isnan(cmdParams[Z])?0:(cmdParams[Z] - axisPosition[Z]*STEPLENGTH[Z]);
  } else {
    motion[X] = isnan(cmdParams[X])?0:cmdParams[X];
    motion[Y] = isnan(cmdParams[Y])?0:cmdParams[Y];
    motion[Z] = isnan(cmdParams[Z])?0:cmdParams[Z];
  }
  
  float travel = sqrt(pow2(motion[X]) + pow2(motion[Y]) + pow2(motion[Z]));
  
  if(cmdParams[F] > 0) feedRate = cmdParams[F];
  
  float velocity[3];
  velocity[X] = abs(feedRate * (motion[X] / travel));
  velocity[Y] = abs(feedRate * (motion[Y] / travel));
  velocity[Z] = abs(feedRate * (motion[Z] / travel));
  velocity[E] = abs(cmdParams[E] * (feedRate / travel));
  
  acknowledgeCommand();
  if(!isnan(cmdParams[X])) moveAxis(X,motion[X],velocity[X]);
  if(!isnan(cmdParams[Y])) moveAxis(Y,motion[Y],velocity[Y]);
  if(!isnan(cmdParams[Z])) moveAxis(Z,motion[Z],velocity[Z]);
  if(!isnan(cmdParams[E])) moveAxis(E,cmdParams[E],velocity[E]);
  movementLine = cmdLine;
  startStepperWorker();
}


void cmdEcho() {
  addToBufferS("ECHO N",6);
  addToBufferI(cmdLine);
  addToBufferS(" X",2);
  addToBufferF(cmdParams[X]);
  addToBufferS(" Y",2);
  addToBufferF(cmdParams[Y]);
  addToBufferS(" Z",2);
  addToBufferF(cmdParams[Z]);
  addToBufferS(" E",2);
  addToBufferF(cmdParams[F]);
  addToBufferS(" F",2);
  addToBufferF(cmdParams[E]);
  addToBufferS(" S",2);
  addToBufferF(cmdParams[S]);
  addToBufferC('\n');
}

inline void cmdHalt() {
  stateFlags &= ~FLAG_ENABLE;
  addToBufferS("HALT N",6);
  addToBufferI(cmdLine);
  addToBufferC('\n');
}
void cmdResume() {
  if((stateFlags & FLAG_ENABLE) || !(stateFlags & ~FLAG_ENABLE)) {
    invalidCommand();
    return;
  }
  addToBufferS("RESUME N",8);
  addToBufferI(cmdLine);
  addToBufferC('\n');
  stateFlags |= FLAG_ENABLE;
}
void cmdRecover() {
  resetAxes(); 
  if((stateFlags & FLAG_ENABLE) || !(stateFlags & ~FLAG_ENABLE)) {
    invalidCommand();
    return;
  }
  addToBufferS("RECOVER N",9);
  addToBufferI(cmdLine);
  addToBufferC('\n');
  stateFlags = FLAG_ENABLE;
}

void cmdPosition() {
  addToBufferS("POS N",5);
  addToBufferI(cmdLine);
  addToBufferS(" X",2);
  addToBufferF(axisPosition[X]*STEPLENGTH[X]);
  addToBufferS(" Y",2);
  addToBufferF(axisPosition[Y]*STEPLENGTH[Y]);
  addToBufferS(" Z",2);
  addToBufferF(axisPosition[Z]*STEPLENGTH[Z]);
  addToBufferS(" E",2);
  addToBufferF(axisPosition[E]*STEPLENGTH[E]);
  addToBufferC('\n');
}

void cmdSetTemperature() {
  if(isnan(cmdParams[S]) && isnan(cmdParams[F])) return invalidCommand();
  if(!isnan(cmdParams[S])) targetTemperature = cmdParams[S];
  if(!isnan(cmdParams[F]) && cmdParams[F] > 0) temperatureRange = cmdParams[F];
  acknowledgeCommand();
}

void cmdSetPos() {
  if(!isnan(cmdParams[X])) axisPosition[X] = cmdParams[X] / STEPLENGTH[X];
  if(!isnan(cmdParams[Y])) axisPosition[Y] = cmdParams[Y] / STEPLENGTH[Y];
  if(!isnan(cmdParams[Z])) axisPosition[Z] = cmdParams[Z] / STEPLENGTH[Z];
  acknowledgeCommand();
}
