#include "Mask.hpp"

Mask::Mask(Mask& m)
{
  m_width = m.width();
  m_height = m.height();
  m_mask = new uint8_t*[m_width];
  for(uint16_t i = 0; i < m_width; i++) m_mask[i] = new uint8_t[m_height];
  for(uint16_t i = 0; i < m_width; i++){
    for(uint16_t j = 0; j < m_height; j++) m_mask[i][j] = m.getValue(i,j);
  }
}

Mask::~Mask()
{
  if(m_mask){
    for(uint16_t i = 0; i < m_width; i++) delete[] m_mask[i];
    delete[] m_mask;
  }
}

void Mask::init(uint16_t w, uint16_t h, uint8_t startValue)
{
  m_width = w;
  m_height = h;
  m_mask = new uint8_t*[m_width];
  for(uint16_t i = 0; i < m_width; i++) m_mask[i] = new uint8_t[m_height];
}

bool Mask::equals(Mask& m)
{
  if(m_width != m.width() || m_height != m.height()) return false;
  for(uint16_t i = 0; i < m_width; i++){
    for(uint16_t j = 0; j < m_height; j++){
      if(m_mask[i][j] != m.getValue(i,j)) return false;
    }
  }
  return true;
}

Mask* Mask::inverse()
{
  Mask* m = new Mask(*this);
  for(uint16_t i = 0; i < m_width; i++){
    for(uint16_t j = 0; j < m_height; j++) m->setValue(i,j,1-m_mask[i][j]);
  }
  return m;
}

void Mask::copy(Mask& m)
{
  for(uint16_t i = 0; i < m_width; i++){
    for(uint16_t j = 0; j < m_height; j++) m_mask[i][j] = m.getValue(i,j);
  }
}

void Mask::OR(Mask& m)
{
  for(uint16_t i = 0; i < m_width; i++){
    for(uint16_t j = 0; j < m_height; j++){
      if(m.getValue(i,j) > 0) m_mask[i][j] = 1;
    }
  }
}

void Mask::multiply(uint8_t& val)
{
  for(uint16_t i = 0; i < m_width; i++){
    for(uint16_t j = 0; j < m_height; j++) m_mask[i][j] *= val;
  }
}

void Mask::multiply(Mask& m)
{
  for(uint16_t i = 0; i < m_width; i++){
    for(uint16_t j = 0; j < m_height; j++) m_mask[i][j] *= m.getValue(i,j);
  }
}

void Mask::add(Mask& m)
{
  for(uint16_t i = 0; i < m_width; i++){
    for(uint16_t j = 0; j < m_height; j++) m_mask[i][j] += m.getValue(i,j);
  }
}

void Mask::subtract(Mask& m)
{
  for(uint16_t i = 0; i < m_width; i++){
    for(uint16_t j = 0; j < m_height; j++) m_mask[i][j] -= m.getValue(i,j);
  }
}

uint32_t Mask::sum()
{
  uint32_t sum = 0;
  for(uint16_t i = 0; i < m_width; i++){
    for(uint16_t j = 0; j < m_height; j++) sum += m_mask[i][j];
  }
  return sum;
}

uint32_t Mask::sum(uint16_t& x1, uint16_t& x2, uint16_t& y1, uint16_t& y2)
{
  uint32_t sum = 0;
  for(uint16_t i = x1; i < x2; i++){
    for(uint16_t j = y1; j < y2; j++) sum += m_mask[i][j];
  }
  return sum;
}

void Mask::clear(uint16_t& x1, uint16_t& x2, uint16_t& y1, uint16_t& y2)
{
  for(uint16_t i = x1; i < x2; i++){
    for(uint16_t j = y1; j < y2; j++) m_mask[i][j] = 0;
  }
}

uint16_t Mask::max(uint16_t& x1, uint16_t& x2, uint16_t& y1, uint16_t& y2)
{
  uint16_t max = 0;
  for(uint16_t i = x1; i < x2; i++){
    for(uint16_t j = y1; j < y2; j++){
      if(m_mask[i][j] > max) max = m_mask[i][j];
    }
  }
  return max;
}

