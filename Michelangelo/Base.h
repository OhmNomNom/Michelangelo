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
#define pow2(n) n*n

UINT sParseUINT(const char*);
float sParseFloat(const char*);

#endif
