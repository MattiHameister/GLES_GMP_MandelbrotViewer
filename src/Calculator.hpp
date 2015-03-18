#pragma once

#include "main.hpp"
#include "Utilities.hpp"
#include "Area.hpp"
class Logic;

class Calculator {
private:
  Logic *logic;
  Range mandelRange;
  int32_t gmpBit;
  iter_t maxIterations;
  bool useGMP;

public:
  Calculator(Logic *l);
  virtual ~Calculator();
  virtual void Bits(int32_t bits);
  virtual iter_t& MaxIterations();
  virtual void RecalcView(long double factorX, long double factorY);
  virtual void RecalcZoom(long double factorLeft, long double factorRight, long double factorBottom);
  virtual bool Calculate(Area<uint32_t> &area);
  virtual void CalculateAll(Area<uint32_t> &area);
  virtual void PrepareCalculation();
  virtual void ToogleGMP();
};
