#include "Base.h"

//Needed to make one that returns 0 on all invalid inputs
UINT sParseUINT(const char *str) {
  UINT buffer = 0;
  for (int i = 0; str[i] != '\0'; i++) {
    if(!isdigit(str[i])) return 0;
    buffer = 10*buffer + (str[i] - 48);
  }
  return buffer;
}

float sParseFloat(const char *str) {
  float buffer = 0;
  float m1 = 10, m2 = 1;
  int i = 0;
  if(str[0] == '-') {
    i++;
    m1 = -10;
    m2 = -1;
  }     
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
