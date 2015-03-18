#pragma once
#include <gmp.h>

class RGBf {
public:
  float red;
  float green;
  float blue;

  RGBf();
};

class Range {
public:
  mpf_t minRe;
  mpf_t maxRe;
  mpf_t minIm;
  mpf_t maxIm;
  mpf_t re_factor;
  mpf_t im_factor;
};

class PaletteDefinition {
public:
  double position;
  RGBf value;

  PaletteDefinition(double c, float vR, float vG, float vB);
};
