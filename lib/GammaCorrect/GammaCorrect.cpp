#include "GammaCorrect.h"

GammaCorrect::GammaCorrect(float gammaValue) {
  gamma = gammaValue;
  generateTable();
}

void GammaCorrect::generateTable() {
  for (int i = 0; i < 256; i++) {
    gammaTable[i] = pow(i / 255.0, gamma) * 255.0;
  }
}

byte GammaCorrect::getValue(byte value) { return gammaTable[value]; }
