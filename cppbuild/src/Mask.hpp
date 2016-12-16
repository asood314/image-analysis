#ifndef MASK_HPP
#define MASK_HPP

#include <iostream>
#include <vector>
#include <cmath>

class Mask
{

protected:
  std::vector< std::vector<int> > m_mask;
  int m_width;
  int m_height;

public:
  Mask(){}
  Mask(int w, int h){ init(w,h,0); }
  Mask(int w, int h, int startValue){ init(w,h,startValue); }
  Mask(Mask& m);
  virtual ~Mask();
  void init(int w, int h, int startValue);
  bool equals(Mask& m);
  int width(){ return m_width; }
  int height(){ return m_height; }
  int getValue(int x, int y){ return m_mask[x][y]; }
  void setValue(int x, int y, int value){ m_mask[x][y] = value; }
  Mask* inverse();
  void copy(Mask& m);
  Mask* getCopy(){ return new Mask(*this); }
  void OR(Mask& m);
  void multiply(int& val);
  void multiply(Mask& m);
  void add(Mask& m);
  void subtract(Mask& m);
  int sum();
  int sum(int x1, int x2, int y1, int y2);
  void clear(int x1, int x2, int y1, int y2);
  int max(int x1, int x2, int y1, int y2);
  Mask* getBorders();
  bool isMinimallyConnected(int& x1, int& y1, int& x2, int& y2, bool allowBorders=true);

};

#endif
