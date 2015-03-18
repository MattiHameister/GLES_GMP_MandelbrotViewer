#include "Utilities.hpp"

RGBf::RGBf()
{
  red = 0.0f;
  green = 0.0f;
  blue = 0.0f;
}

PaletteDefinition::PaletteDefinition(double c, float vR, float vG, float vB)
{
  position = c;
  value.red = vR;
  value.green = vG;
  value.blue = vB;
}
