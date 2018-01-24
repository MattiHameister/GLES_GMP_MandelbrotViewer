#include "Logic.hpp"
#include <iostream>
#include <cmath>
#include "Calculator.hpp"
#include <thread>
#include <functional>

Logic::Logic()
{
  screen = NULL;
  stop = false;
  calculator = new Calculator(this);
  selectZoomX=0;
  selectZoomY=0;
  selectZoomW=0;
  selectZoomH=0;

  selectMoveX=0;
  selectMoveY=0;
  selectMoveW=0;
  selectMoveH=0;

  oversampling = 1;

  redraw = true;
}

Logic::~Logic()
{
  delete calculator;
  Cleanup();
}

bool Logic::RedrawDisplay(bool draw)
{
  bool ret = redraw;
  redraw = draw;
  return ret;
}

void Logic::Cleanup()
{
  delete screen;
}

void Logic::DoLogic(double timeSinceLastRun)
{
}

void Logic::Resize(uint32_t w, uint32_t h)
{
  Stop();
  Cleanup();
  displayWidth = w;
  displayHeight = h;
  Start();
}

Screen* Logic::Background()
{
  return screen;
}

Calculator* Logic::Cruncher()
{
  return calculator;
}

bool Logic::ShouldStop()
{
  return stop;
}

int ThreadCode(Logic *l)
{
//  Logic *l = (Logic*)data;
  while(!l->ShouldStop()) {
    uint32_t job = l->Background()->NextJob();
    if(job>0) {
      Area<uint32_t> area;
      l->Background()->AreaForJob(job,area);
      //      std::cout << job << "---" << area.X1 << "x" << area.Y1 << "->" << area.X2 << "x" << area.Y2 << std::endl;
      if(area.IsValid()) {
        if(area.Size() > 25) {
          bool split = l->Cruncher()->Calculate(area);
          if(l->ShouldStop()) {
            break;
          }
          if(split) {
            l->Background()->SplitJob(job);
          }
        } else {
          l->Cruncher()->CalculateAll(area);
        }
      }
    } else {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
  //  screen->AreaForJob(5,area);
  //  calculator->Calculate(area);
  return 0;
}

void Logic::Start()
{
  screen = new Screen(displayWidth,displayHeight,oversampling);
  calculator->PrepareCalculation();
  stop = false;
  uint32_t numThreads = std::thread::hardware_concurrency();
  if(numThreads == 0) {
    numThreads = 1;
  }
  std::cout << "Anzahl CPUs: " << numThreads << std::endl;
  for(uint32_t i=0; i < numThreads; i++) {
    threads.push_back(std::thread(std::bind(ThreadCode,this)));
  }
}

void Logic::Stop()
{
  stop = true;
  for(auto& thread : threads) {
    thread.join();
  }

  threads.clear();
}

void Logic::Restart()
{
  redraw = true;
  Stop();
  Cleanup();
  Start();
}

void Logic::RecalcZoom()
{
  int32_t diffX = selectZoomW - selectZoomX;
  int32_t diffY = selectZoomH - selectZoomY;
  if(std::abs(diffX) >= 10 || std::abs(diffY) >= 10) {
    long double x = displayWidth;
    long double y = displayHeight;
    long double factorLeft = ((selectZoomX*100.0)/x)/100.0;
    long double factorRight = 1.0-((selectZoomW*100.0)/x)/100.0;
    long double factorBottom = 1.0-((selectZoomH*100.0)/y)/100.0;
    calculator->RecalcZoom(factorLeft,factorRight,factorBottom);
    Restart();
  }
}

void Logic::RecalcView()
{
  long double x = displayWidth;
  long double y = displayHeight;
  int32_t diffX = selectMoveW - selectMoveX;
  int32_t diffY = selectMoveH - selectMoveY;
  if(std::abs(diffX) >= 1 || std::abs(diffY) >= 1) {
    long double factorX = ((diffX*100.0)/x)/100.0;
    long double factorY = ((diffY*100.0)/y)/100.0;
    calculator->RecalcView(factorX, factorY);
    Restart();
  }
}

void Logic::MouseDownEvent(uint32_t button, uint32_t x, uint32_t y)
{
  if(button==1) {
    selectZoomX = x;
    selectZoomY = y;
  } else {
    selectMoveX = x;
    selectMoveY = y;
  }
}

void Logic::MouseUpEvent(uint32_t button, uint32_t x, uint32_t y)
{
  if(button==1) {
    selectZoomW = x;
    selectZoomH = y;
    RecalcZoom();
  } else {
    selectMoveW = x;
    selectMoveH = y;
    RecalcView();
  }
}

void Logic::KeyDownEvent(uint32_t key)
{
  if(key == 1) {
    calculator->MaxIterations() += 100;
    Restart();
  } else if(key == 2) {
    oversampling*=2;
    Restart();
  } else if(key == 3) {
    calculator->ToogleGMP();
    Restart();
  }
}
