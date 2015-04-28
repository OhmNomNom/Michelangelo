#ifndef Base_h_
#define Base_h_

#include <Arduino.h>
#include <math.h>

#define VERSION 0.1

typedef uint32_t ULONG;
typedef int32_t  SLONG;
typedef uint8_t  UBYTE;
typedef int8_t   SBYTE;
typedef uint16_t UINT;

#define strEqual(m,n) strcmp(m,n) == 0
#define pow2(n) (n*n)

static const UBYTE FLAG_ENABLE        = 0x01,
                   FLAG_X             = 0x02,
                   FLAG_Y             = 0x04,
                   FLAG_Z             = 0x08,
                   E_FLAG             = 0x10,
                   FLAG_HOTEND_ON     = 0x20,
                   FLAG_ABSOLUTE_MODE = 0x40;
                   
const ULONG PID_PERIOD      = 100000,
            WORKER_PERIOD   = 10;

extern UBYTE stateFlags;
extern char serialBuffer[];
extern UBYTE bufferPos;

UINT sParseUINT(const char*);
float sParseFloat(const char*);
void addToBufferS(const char*,UBYTE);
void addToBufferF(const float&);
void addToBufferC(const char&);
void addToBufferI(const int&);
void flushSerial(void);

#endif
