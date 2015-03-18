#pragma once

#include "main.hpp"
#include "Screen.hpp"
#include <thread>

class Calculator;

class Logic {
private:
  Screen *screen;
  Calculator *calculator;
  std::vector<std::thread> threads;
  void Cleanup();
  bool stop;
  int32_t selectZoomX;
  int32_t selectZoomY;
  int32_t selectZoomW;
  int32_t selectZoomH;

  int32_t selectMoveX;
  int32_t selectMoveY;
  int32_t selectMoveW;
  int32_t selectMoveH;

  uint32_t displayWidth;
  uint32_t displayHeight;

  uint32_t oversampling;

  virtual void RecalcZoom();
  virtual void RecalcView();

  bool redraw;
public:
  Logic();
  virtual ~Logic();
  virtual bool RedrawDisplay(bool redraw);
  virtual bool ShouldStop();
  virtual void Start();
  virtual void Stop();
  virtual void Restart();
  virtual void DoLogic(double timeSinceLastRun);
  virtual void Resize(uint32_t w, uint32_t h);
  virtual void KeyDownEvent(uint32_t key);
  virtual void MouseDownEvent(uint32_t button, uint32_t x, uint32_t y);
  virtual void MouseUpEvent(uint32_t button, uint32_t x, uint32_t y);
  virtual Screen* Background();
  virtual Calculator* Cruncher();
};
