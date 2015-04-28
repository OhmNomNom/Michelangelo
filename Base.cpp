#include "Base.h"

UBYTE stateFlags = 0x01;
char serialBuffer[64];
UBYTE bufferPos = 0;

//Needed to make one thatreturns 0 on all invalid inputs
UINT sParseUINT(const char *str) {
  UINT buffer = 0;
  for (int i = 0; str[i] != '\0'; i++) {
    if(!isdigit(str[i])) return 0;
    buffer = 10*buffer + (str[i] - 48);
  }
  return buffer;
}

float sParseFloat(const char *str) {
  if(str[0] == '-')
    return -sParseFloat(str+1);
  float buffer = 0;
  float m1 = 10, m2 = 1;
  int i = 0;
  for (; str[i] != '\0'; i++) {
    if(!isdigit(str[i])) {
      if(str[i] == '.') {
        m1 *= 0.1;
        m2 *= 0.1;
        continue;
      } else return NAN;
    }
    buffer = m1*buffer + m2 * float(str[i] - 48);
    m2 *= m2;
  }
  return buffer;
}

void addToBufferS(const char *data, UBYTE len) {
  memcpy(serialBuffer+bufferPos,data,len);
  bufferPos += len;
}

void addToBufferF(const float &data) {
  dtostrf(data,1,2,serialBuffer+bufferPos);
  while(serialBuffer[++bufferPos] != '\0') continue;
}

void addToBufferC(const char &data) {
  serialBuffer[bufferPos++] = data;
}

void addToBufferI(const int &data) {
  bufferPos += sprintf(serialBuffer+bufferPos, "%d", data);
} 

void flushSerial() {
  if(bufferPos != 0) {
      serialBuffer[bufferPos] = '\0';
      Serial.print(serialBuffer);
      bufferPos = 0;
  }
}
