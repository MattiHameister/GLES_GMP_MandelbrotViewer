#pragma once
#include <cmath>

/*
  simple structure to hold an calculate
  with Areas like 0x0 to 10x10
*/
template<class value_t> class Area {
public:
  value_t X1;
  value_t Y1;
  value_t Z1;
  value_t X2;
  value_t Y2;
  value_t Z2;

  Area() {
    X1 = Y1 = Z1 = X2 = Y2 = Z2 = 0;
  }

  bool IsZero() const {
    return X1 == Y1 == Z1 == X2 == Y2 == Z2 == 0;
  }

  /*
    for example 1x1 to 0x0 is not valid
  */
  bool IsValid() const {
    return (X1 <= X2) && (Y1 <= Y2) && (Z1 <= Z2);
  }

  /* TODO
    proper variant of this for 3D
  */
  value_t Size() const {
    value_t x = X2-X1;
    value_t y = Y2-Y1;
    return std::max(x,y);
  }

  void AddOffset(const Area &offset) {
    X1 += offset.X1;
    Y1 += offset.Y1;
    Z1 += offset.Z1;
    X2 -= offset.X2;
    Y2 -= offset.Y2;
    Z2 -= offset.Z2;
  }


  /* TODO
    proper variant of this for 3D
  */
  void TopLine(Area &top) {
    top.X1 = X1;
    top.Y1 = Y1;
    top.X2 = X2;
    top.Y2 = Y1;
  }

  void BottomLine(Area &bottom) {
    bottom.X1 = X1;
    bottom.Y1 = Y2;
    bottom.X2 = X2;
    bottom.Y2 = Y2;
  }

  void LeftLine(Area &left) {
    left.X1 = X1;
    left.Y1 = Y1;
    left.X2 = X1;
    left.Y2 = Y2;
  }

  void RightLine(Area &right) {
    right.X1 = X2;
    right.Y1 = Y1;
    right.X2 = X2;
    right.Y2 = Y2;
  }
};
