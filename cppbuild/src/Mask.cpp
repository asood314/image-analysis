#include "Mask.hpp"

Mask::Mask(Mask& m)
{
  m_width = m.width();
  m_height = m.height();
  for(int i = 0; i < m_width; i++) m_mask.push_back(std::vector<int>(m_height));
  for(int i = 0; i < m_width; i++){
    for(int j = 0; j < m_height; j++) m_mask[i][j] = m.getValue(i,j);
  }
}

Mask::~Mask()
{
}

void Mask::init(int w, int h, int startValue)
{
  m_width = w;
  m_height = h;
  for(int i = 0; i < m_width; i++){
    m_mask.push_back(std::vector<int>(m_height));
    for(int j = 0; j < m_height; j++) m_mask[i][j] = startValue;
  }
}

bool Mask::equals(Mask& m)
{
  if(m_width != m.width() || m_height != m.height()) return false;
  for(int i = 0; i < m_width; i++){
    for(int j = 0; j < m_height; j++){
      if(m_mask[i][j] != m.getValue(i,j)) return false;
    }
  }
  return true;
}

Mask* Mask::inverse()
{
  Mask* m = new Mask(*this);
  for(int i = 0; i < m_width; i++){
    for(int j = 0; j < m_height; j++) m->setValue(i,j,1-m_mask[i][j]);
  }
  return m;
}

void Mask::copy(Mask& m)
{
  for(int i = 0; i < m_width; i++){
    for(int j = 0; j < m_height; j++) m_mask[i][j] = m.getValue(i,j);
  }
}

void Mask::OR(Mask& m)
{
  for(int i = 0; i < m_width; i++){
    for(int j = 0; j < m_height; j++){
      if(m.getValue(i,j) > 0) m_mask[i][j] = 1;
    }
  }
}

void Mask::multiply(int& val)
{
  for(int i = 0; i < m_width; i++){
    for(int j = 0; j < m_height; j++) m_mask[i][j] *= val;
  }
}

void Mask::multiply(Mask& m)
{
  for(int i = 0; i < m_width; i++){
    for(int j = 0; j < m_height; j++) m_mask[i][j] *= m.getValue(i,j);
  }
}

void Mask::add(Mask& m)
{
  for(int i = 0; i < m_width; i++){
    for(int j = 0; j < m_height; j++) m_mask[i][j] += m.getValue(i,j);
  }
}

void Mask::subtract(Mask& m)
{
  for(int i = 0; i < m_width; i++){
    for(int j = 0; j < m_height; j++) m_mask[i][j] -= m.getValue(i,j);
  }
}

int Mask::sum()
{
  int sum = 0;
  for(int i = 0; i < m_width; i++){
    for(int j = 0; j < m_height; j++) sum += m_mask[i][j];
  }
  return sum;
}

int Mask::sum(int x1, int x2, int y1, int y2)
{
  int sum = 0;
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++) sum += m_mask[i][j];
  }
  return sum;
}

void Mask::clear(int x1, int x2, int y1, int y2)
{
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++) m_mask[i][j] = 0;
  }
}

int Mask::max(int x1, int x2, int y1, int y2)
{
  int max = 0;
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      if(m_mask[i][j] > max) max = m_mask[i][j];
    }
  }
  return max;
}

