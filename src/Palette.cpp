#include "Palette.hpp"

/*
void printRGB(uint32_t idx, RGBf &mid, bool check) {
    float r = mid.red;
    float g = mid.green;
    float b = mid.blue;
    if(check) {
      if((r!=b || r!=g || b!=g))
        std::cout << idx << " >> " << r << " | " << g << " | " << b << std::endl;
    } else {
        std::cout << idx << " ++ " << r << " | " << g << " | " << b << std::endl;
     }
}
*/
void Palette::calcPalettePart(float count, int offset, RGBf &start, RGBf &end)
{
//  std::cout << "offset: " << offset << " count: " << count << std::endl;
  palette[offset] = start;

  for(int i=1; i<=count; i++) {
    float f = i;
    RGBf mid;
    mid.red = start.red - ((start.red - end.red)/count)*f;
    mid.green = start.green - ((start.green - end.green)/count)*f;
    mid.blue = start.blue - ((start.blue - end.blue)/count)*f;

    palette[offset+i] = mid;
  }
}

void Palette::calcPalette(std::vector<PaletteDefinition> &pal)
{
  int count = 0;
  int offset = 0;
  RGBf end;
  for(size_t i=0; i<pal.size()-1; i++) {
    PaletteDefinition palette = pal.at(i);
    PaletteDefinition paletteEnd = pal.at(i+1);

    count = paletteCount*paletteEnd.position;
    offset = paletteCount*palette.position;
    count -= offset;

    RGBf start;
    start.red = palette.value.red;
    start.green = palette.value.green;
    start.blue = palette.value.blue;
    end.red = paletteEnd.value.red;
    end.green = paletteEnd.value.green;
    end.blue = paletteEnd.value.blue;

    calcPalettePart(count, offset, start, end);
  }
}

Palette::Palette(uint32_t count, std::vector<PaletteDefinition> &pal)
{
  paletteCount = count;
  palette.resize(paletteCount+1);
  calcPalette(pal);
}

Palette::~Palette()
{
}

RGBf& Palette::get(uint32_t idx)
{
  idx = idx % (paletteCount+1);
  return palette[idx];
}
