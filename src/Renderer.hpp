#pragma once
#include "main.hpp"
#include "Screen.hpp"
#include "Logic.hpp"
#include <fstream>

class Renderer {
private:
  // TODO
  uint32_t programObject;

  Logic *logic;
  uint32_t width;
  uint32_t height;
  float gammaValue;
//    std::vector<uint32_t> pixel;
  uint32_t limitPixel;

  virtual uint32_t TransformCoord(uint32_t x, uint32_t y) const;
  virtual void TransformIndex(uint32_t index, uint32_t &x, uint32_t &y) const;
//    void TGASaveHeader(std::ofstream &file, int16_t width, int16_t height, uint8_t pixelDepth) const;
  void ApplyOrtho(float left, float right, float bottom, float top, float near, float far) const;
  void ApplyTrans(float x, float y, float z) const;

public:
  Renderer(Logic *l);
  virtual ~Renderer();
  virtual void Reset();
  virtual void InitRenderer(uint32_t w, uint32_t h);
  virtual void Render();
//    virtual void MakeScreenshot();
};
