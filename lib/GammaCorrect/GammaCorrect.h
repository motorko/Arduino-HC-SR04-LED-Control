#ifndef GAMMA_CORRECT_H
#define GAMMA_CORRECT_H

#include <Arduino.h>
#include <math.h>

class GammaCorrect {
 private:
  byte gammaTable[256];
  float gamma;

  void generateTable();

 public:
  GammaCorrect(float gammaValue = 2.2);
  byte getValue(byte value);
};

#endif
