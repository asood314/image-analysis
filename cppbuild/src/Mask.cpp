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

Mask* Mask::getBorders()
{
  Mask* retVal = new Mask(m_width,m_height);
  for(int i = 0; i < m_width; i++){
    for(int j = 0; j < m_height; j++){
      if(m_mask[i][j] < 1) continue;
      int sum = 0;
      for(int di = i-1; di < i+2; di++){
	if(di < 0 || di >= m_width){
	  sum += 3;
	  continue;
	}
	for(int dj = j-1; dj < j+2; dj++){
	  if(dj < 0 || dj >= m_height) sum += 1;
	  else sum += m_mask[di][dj];
	}
      }
      retVal->setValue(i,j,1 - sum/9);
    }
  }
  return retVal;
}

bool Mask::isMinimallyConnected(int& x1, int& y1, int& x2, int& y2, bool allowBorders)
{
  if(x1 == x2 && y1 == y2) return true;
  double slope = ((double)(y2) - y1) / (x2 - x1);
  if(x1 == x2) slope = 99999999.9;
  if(fabs(slope) > 1){
    double step = (y2 - y1)/fabs(y2 - y1);
    int nsteps = (int)((y2 - y1)/step);
    double y = y1;
    double x = x1;
    if(allowBorders){
      for(int j = 0; j < nsteps; j++){
	if(m_mask[(int)x][(int)y] == 0) return false;
	y += step;
	x += step/slope;
      }
    }
    else{
      for(int j = 0; j < nsteps; j++){
	if(m_mask[(int)x][(int)y] < 2) return false;
	y += step;
	x += step/slope;
      }
    }
  }
  else{
    double step = (x2 - x1)/fabs(x2 - x1);
    int nsteps = (int)((x2 - x1)/step);
    double x = x1;
    double y = y1;
    if(allowBorders){
      for(int j = 0; j < nsteps; j++){
	if(m_mask[(int)x][(int)y] == 0) return false;
	x += step;
	y += step*slope;
      }
    }
    else{
      for(int j = 0; j < nsteps; j++){
	if(m_mask[(int)x][(int)y] < 2) return false;
	x += step;
	y += step*slope;
      }
    }
  }
  return true;
  
}

bool Mask::isMinimallyConnected(int& x1, int& y1, int& x2, int& y2, int id1, int id2)
{
  if(x1 == x2 && y1 == y2) return true;
  double slope = ((double)(y2) - y1) / (x2 - x1);
  if(x1 == x2) slope = 99999999.9;
  if(fabs(slope) > 1){
    double step = (y2 - y1)/fabs(y2 - y1);
    int nsteps = (int)((y2 - y1)/step);
    double y = y1;
    double x = x1;
    for(int j = 0; j < nsteps; j++){
      int val = m_mask[(int)x][(int)y];
      if(val > 0 && val != id1 && val != id2) return false;
      y += step;
      x += step/slope;
    }
  }
  else{
    double step = (x2 - x1)/fabs(x2 - x1);
    int nsteps = (int)((x2 - x1)/step);
    double x = x1;
    double y = y1;
    for(int j = 0; j < nsteps; j++){
      int val = m_mask[(int)x][(int)y];
      if(val > 0 && val != id1 && val != id2) return false;
      x += step;
      y += step*slope;
    }
  }
  return true;
}
