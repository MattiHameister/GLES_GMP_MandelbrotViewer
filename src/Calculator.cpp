#include "Calculator.hpp"
#include "Logic.hpp"

class ICruncher {
public:
  virtual void prepareForX(uint32_t x) = 0;
  virtual void prepareForY(uint32_t y) = 0;
  virtual iter_t render(uint32_t x, uint32_t y, iter_t maxIterations) = 0;
  virtual ~ICruncher() {};
};

class CruncherGMP : public ICruncher {
private:
  Logic *logic;
  mpf_t tmpDiff;
  mpf_t tmp;
  mpf_t c_im;
  mpf_t c_re;
  mpf_t z_re;
  mpf_t z_im;
  mpf_t z_re2;
  mpf_t z_im2;
  Range mandelRange;

public:
  CruncherGMP(int32_t gmpBit, Range &mR, Logic *l) {
    logic = l;
    mandelRange = mR;
    mpf_init2(tmpDiff,gmpBit);
    mpf_init2(tmp,gmpBit);
    mpf_init2(c_im,gmpBit);
    mpf_init2(c_re,gmpBit);
    mpf_init2(z_im,gmpBit);
    mpf_init2(z_re,gmpBit);
    mpf_init2(z_im2,gmpBit);
    mpf_init2(z_re2,gmpBit);
  }

  virtual void prepareForX(uint32_t x) {
    //c_re = minRe + x * re_factor;
    mpf_mul_ui(tmpDiff, mandelRange.re_factor, x);
    mpf_add(c_re,mandelRange.minRe,tmpDiff);
  }

  virtual void prepareForY(uint32_t y) {
    //c_im = maxIm - y * im_factor;
    mpf_mul_ui(tmpDiff, mandelRange.im_factor, y);
    mpf_sub(c_im,mandelRange.maxIm,tmpDiff);
  }

  virtual iter_t render(uint32_t x, uint32_t y, iter_t maxIterations) {
    mpf_set(z_re,c_re);
    mpf_set(z_im,c_im);

    int32_t isInside = 1;
    iter_t n;

    for (n = 0; n < maxIterations; ++n) {
      //Z_re2 = Z_re * Z_re;
      mpf_mul(z_re2,z_re,z_re);
      //Z_im2 = Z_im * Z_im;
      mpf_mul(z_im2,z_im,z_im);

      //Z_re2 + Z_im2 > 4
      mpf_add(tmp, z_re2, z_im2);
      if (mpf_cmp_ui(tmp,4) > 0) {
        isInside = 0;
        break;
      }
      //Z_im = 2 * Z_re * Z_im + c_im;
      mpf_mul_ui(tmp, z_re, 2);
      mpf_mul(tmp,tmp,z_im);
      mpf_add(z_im,tmp,c_im);

      //Z_re = Z_re2 - Z_im2 + c_re;
      mpf_sub(tmp,z_re2,z_im2);
      mpf_add(z_re,tmp,c_re);

      // Thread hat den Stop-Befehl bekommen.
      if(logic->ShouldStop()) {
        return true;
      }

    }
    if (isInside) {
      n = IS_INSIDE;
    }
    logic->Background()->Iteration(x,y) = n;
    return n;
  }

  virtual ~CruncherGMP() {
    mpf_clear(c_im);
    mpf_clear(c_re);
    mpf_clear(z_im);
    mpf_clear(z_re);
    mpf_clear(z_im2);
    mpf_clear(z_re2);
    mpf_clear(tmpDiff);
    mpf_clear(tmp);
  }
};

class CruncherDouble : public ICruncher {
private:
  Logic *logic;
  long double c_im;
  long double c_re;
  long double z_re;
  long double z_im;
  long double z_re2;
  long double z_im2;

  long double maxRe;
  long double minRe;
  long double maxIm;
  long double minIm;
  long double re_factor;
  long double im_factor;
  Range mandelRange;

public:
  CruncherDouble(int32_t gmpBit, Range &mR, Logic *l) {
    logic = l;
    mandelRange = mR;
    maxRe = mpf_get_d(mandelRange.maxRe);
    minRe = mpf_get_d(mandelRange.minRe);
    maxIm = mpf_get_d(mandelRange.maxIm);
    minIm = mpf_get_d(mandelRange.minIm);
    re_factor = mpf_get_d(mandelRange.re_factor);
    im_factor = mpf_get_d(mandelRange.im_factor);
  }

  virtual void prepareForX(uint32_t x) {
    c_re = minRe + x * re_factor;
  }

  virtual void prepareForY(uint32_t y) {
    c_im = maxIm - y * im_factor;
  }

  virtual iter_t render(uint32_t x, uint32_t y, iter_t maxIterations) {
    z_re = c_re;
    z_im = c_im;

    int32_t isInside = 1;
    iter_t n;

    for (n = 0; n < maxIterations; ++n) {
      z_re2 = z_re * z_re;
      z_im2 = z_im * z_im;

      if (z_re2 + z_im2 > 4) {
        isInside = 0;
        break;
      }
      z_im = 2 * z_re * z_im + c_im;
      z_re = z_re2 - z_im2 + c_re;

      if(logic->ShouldStop()) {
        return true;
      }

    }
    if (isInside) {
      n = IS_INSIDE;
    }
    logic->Background()->Iteration(x,y) = n;
    return n;
  }

  virtual ~CruncherDouble() {
  }
};

Calculator::Calculator(Logic *l)
{
  useGMP = false;
  logic = l;
  gmpBit = 128;
  maxIterations = 100;
  mpf_init2(mandelRange.minRe,gmpBit);
  mpf_init2(mandelRange.maxRe,gmpBit);
  mpf_init2(mandelRange.minIm,gmpBit);
  mpf_init2(mandelRange.maxIm,gmpBit);
  mpf_init2(mandelRange.re_factor,gmpBit);
  mpf_init2(mandelRange.im_factor,gmpBit);

  mpf_set_d(mandelRange.minRe,-2.0);
  mpf_set_d(mandelRange.maxRe,1.0);
  mpf_set_d(mandelRange.minIm,-1.2);
}

Calculator::~Calculator()
{
  mpf_clear(mandelRange.minRe);
  mpf_clear(mandelRange.maxRe);
  mpf_clear(mandelRange.minIm);
  mpf_clear(mandelRange.maxIm);
  mpf_clear(mandelRange.re_factor);
  mpf_clear(mandelRange.im_factor);
}

void Calculator::PrepareCalculation()
{
  mpf_t tmpDiff;
  mpf_t tmp;

  mpf_init2(tmpDiff,gmpBit);
  mpf_init2(tmp,gmpBit);

  mpf_sub(tmpDiff,mandelRange.maxRe,mandelRange.minRe);

  long double sX = logic->Background()->Width();
  long double sY = logic->Background()->Height();
  mpf_set_d(tmp, sY/sX);
  //	maxIm = minIm + (maxRe - minRe) * sizeY / sizeX;
  mpf_mul(tmp,tmpDiff,tmp);
  mpf_add(mandelRange.maxIm,mandelRange.minIm,tmp);
  //	re_factor = (maxRe - minRe) / (sizeX - 1);
  mpf_set_d(tmp, sX-1.0);
  mpf_div(mandelRange.re_factor, tmpDiff, tmp);
  //	im_factor = (maxIm - minIm) / (sizeY - 1);
  mpf_sub(tmpDiff,mandelRange.maxIm,mandelRange.minIm);
  mpf_set_d(tmp, sY-1.0);
  mpf_div(mandelRange.im_factor,tmpDiff, tmp);

  mpf_clear(tmpDiff);
  mpf_clear(tmp);
}

void Calculator::Bits(int32_t bits)
{
  mpf_set_prec(mandelRange.minRe,bits);
  mpf_set_prec(mandelRange.maxRe,bits);
  mpf_set_prec(mandelRange.minIm,bits);
  mpf_set_prec(mandelRange.maxIm,bits);
  mpf_set_prec(mandelRange.re_factor,bits);
  mpf_set_prec(mandelRange.im_factor,bits);
}

void Calculator::RecalcView(long double factorX, long double factorY)
{
  mpf_t facX;
  mpf_init2(facX,gmpBit);
  mpf_set_d(facX,factorX);
  mpf_t facY;
  mpf_init2(facY,gmpBit);
  mpf_set_d(facY,factorY);
  mpf_t tmpDiff;
  mpf_init2(tmpDiff,gmpBit);
  mpf_t tmp;
  mpf_init2(tmp,gmpBit);

  mpf_sub(tmpDiff,mandelRange.maxRe,mandelRange.minRe);

  //minRe-=(maxRe - minRe)*factorX;
  mpf_mul(tmp,tmpDiff,facX);
  mpf_sub(mandelRange.maxRe,mandelRange.maxRe,tmp);
  //maxRe-=(maxRe - minRe)*factorX;
  mpf_sub(mandelRange.minRe,mandelRange.minRe,tmp);

  //minIm+=(maxIm - minIm)*factorY;
  mpf_sub(tmpDiff,mandelRange.maxIm,mandelRange.minIm);
  mpf_mul(tmp,tmpDiff,facY);
  mpf_add(mandelRange.minIm,mandelRange.minIm,tmp);

  mpf_clear(tmp);
  mpf_clear(tmpDiff);
  mpf_clear(facX);
  mpf_clear(facY);
}

void Calculator::RecalcZoom(long double factorLeft, long double factorRight, long double factorBottom)
{
  mpf_t facLeft;
  mpf_init2(facLeft,gmpBit);
  mpf_set_d(facLeft,factorLeft);
  mpf_t facRight;
  mpf_init2(facRight,gmpBit);
  mpf_set_d(facRight,factorRight);
  mpf_t facBottom;
  mpf_init2(facBottom,gmpBit);
  mpf_set_d(facBottom,factorBottom);
  mpf_t tmpDiff;
  mpf_init2(tmpDiff,gmpBit);
  mpf_t tmp;
  mpf_init2(tmp,gmpBit);

  //(maxRe - minRe)
  mpf_sub(tmpDiff,mandelRange.maxRe,mandelRange.minRe);

  //minRe+=(maxRe - minRe)*factorLeft;
  mpf_mul(tmp,tmpDiff,facLeft);
  mpf_add(mandelRange.minRe,mandelRange.minRe,tmp);

  //maxRe-=(maxRe - minRe)*factorRight;
  mpf_mul(tmp,tmpDiff,facRight);
  mpf_sub(mandelRange.maxRe,mandelRange.maxRe,tmp);

  //minIm+=(maxIm - minIm)*factorBottom;
  mpf_sub(tmpDiff,mandelRange.maxIm,mandelRange.minIm);
  mpf_mul(tmp,tmpDiff,facBottom);
  mpf_add(mandelRange.minIm,mandelRange.minIm,tmp);

  mpf_clear(tmp);
  mpf_clear(tmpDiff);
  mpf_clear(facLeft);
  mpf_clear(facRight);
  mpf_clear(facBottom);
}

void Calculator::CalculateAll(Area<uint32_t> &area)
{
  ICruncher *crunch = 0;
  if(useGMP) {
    crunch = new CruncherGMP(gmpBit, mandelRange, logic);
  } else {
    crunch = new CruncherDouble(gmpBit, mandelRange, logic);
  }
  for(uint32_t x = area.X1; x<=area.X2; x++) {
    crunch->prepareForX(x);
    for(uint32_t y = area.Y1; y<=area.Y2; y++) {
      crunch->prepareForY(y);
      crunch->render(x,y,maxIterations);
    }
  }
  delete crunch;
}

bool Calculator::Calculate(Area<uint32_t> &area)
{
  bool split = false;
  ICruncher *crunch = 0;
  if(useGMP) {
    crunch = new CruncherGMP(gmpBit, mandelRange, logic);
  } else {
    crunch = new CruncherDouble(gmpBit, mandelRange, logic);
  }
  //#############TOP#####################
  Area<uint32_t> line;
  area.TopLine(line);
  crunch->prepareForY(line.Y1);
  iter_t iterB = IS_BLANK;
  for(uint32_t x = line.X1; x<=line.X2; x++) {
    crunch->prepareForX(x);
    iter_t same = crunch->render(x,line.Y1,maxIterations);
    if(!split) {
      if(iterB == IS_BLANK) {
        iterB = same;
      } else if(iterB != same) {
        split=true;
      }
    }
  }
  //#############TOP#####################
  //#############BOTTOM#####################
  area.BottomLine(line);
  crunch->prepareForY(line.Y1);
  for(uint32_t x = line.X1; x<=line.X2; x++) {
    crunch->prepareForX(x);
    iter_t same = crunch->render(x,line.Y1,maxIterations);
    if(!split) {
      if(iterB == IS_BLANK) {
        iterB = same;
      } else if(iterB != same) {
        split=true;
      }
    }
  }
  //#############BOTTOM#####################
  //#############RIGHT#####################
  area.RightLine(line);
  crunch->prepareForX(line.X1);
  for(uint32_t y = line.Y1; y<=line.Y2; y++) {
    crunch->prepareForY(y);
    iter_t same = crunch->render(line.X1,y,maxIterations);
    if(!split) {
      if(iterB == IS_BLANK) {
        iterB = same;
      } else if(iterB != same) {
        split=true;
      }
    }
  }
  //#############RIGHT#####################
  //#############LEFT#####################
  area.LeftLine(line);
  crunch->prepareForX(line.X1);
  for(uint32_t y = line.Y1; y<=line.Y2; y++) {
    crunch->prepareForY(y);
    iter_t same = crunch->render(line.X1,y,maxIterations);
    if(!split) {
      if(iterB == IS_BLANK) {
        iterB = same;
      } else if(iterB != same) {
        split=true;
      }
    }
  }
  //#############LEFT#####################
  //area has the same borders -> fill the area
  if(!split) {
    for(uint32_t x = area.X1+1; x<=area.X2-1; x++) {
      for(uint32_t y = area.Y1+1; y<=area.Y2-1; y++) {
        logic->Background()->Iteration(x,y) = iterB;
      }
    }
  }
  delete crunch;
  return split;
}

iter_t& Calculator::MaxIterations()
{
  return maxIterations;
}

void Calculator::ToogleGMP()
{
  useGMP = !useGMP;
}
