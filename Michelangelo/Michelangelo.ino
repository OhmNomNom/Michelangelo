#include "Axes.h"
#include "Interpreter.h"
#include "Extruder.h"

void setup() {
  Serial.begin(9600);
  initAxes();
  initInterpreter();
}

void loop() {
    while (Serial.available() > 0) interpret(Serial.read());
    getExtruderTemperature();
}
