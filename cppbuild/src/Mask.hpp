#ifndef MASK_HPP
#define MASK_HPP

#include <iostream>

class Mask
{

protected:
  uint8_t** m_mask;
  uint16_t m_width;
  uint16_t m_height;

public:
  Mask(){ m_mask = NULL; }
  Mask(uint16_t w, uint16_t h){ init(w,h,0); }
  Mask(uint16_t w, uint16_t h, uint8_t startValue){ init(w,h,startValue); }
  Mask(Mask& m);
  virtual ~Mask();
  void init(uint16_t w, uint16_t h, uint8_t startValue);
  bool equals(Mask& m);
  uint16_t width(){ return m_width; }
  uint16_t height(){ return m_height; }
  uint8_t getValue(uint16_t& x, uint16_t& y){ return m_mask[x][y]; }
  void setValue(uint16_t& x, uint16_t& y, uint8_t value){ m_mask[x][y] = value; }
  Mask* inverse();
  void copy(Mask& m);
  Mask* getCopy(){ return new Mask(*this); }
  void OR(Mask& m);
  void multiply(uint8_t& val);
  void multiply(Mask& m);
  void add(Mask& m);
  void subtract(Mask& m);
  uint32_t sum();
  uint32_t sum(uint16_t& x1, uint16_t& x2, uint16_t& y1, uint16_t& y2);
  void clear(uint16_t& x1, uint16_t& x2, uint16_t& y1, uint16_t& y2);
  uint16_t max(uint16_t& x1, uint16_t& x2, uint16_t& y1, uint16_t& y2);

};

#endif
