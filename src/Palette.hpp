#pragma once

#include "main.hpp"
#include "Utilities.hpp"
#include <vector>

class Palette {
private:
//    RGBf** palette;
  std::vector<RGBf> palette;
  uint32_t paletteCount;
  void calcPalettePart(float count, int offset, RGBf &start, RGBf &end);
  void calcPalette(std::vector<PaletteDefinition> &pal);

public:
  Palette(uint32_t count, std::vector<PaletteDefinition> &pal);
  virtual ~Palette();
  virtual RGBf& get(uint32_t idx);
};

