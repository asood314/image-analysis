#include "Region.hpp"

bool Region::contains(LocalizedObject::Point pt)
{
  bool inside = false;
  for(std::vector<LocalizedObject::Point>::iterator it = m_vertices.begin(); it != m_vertices.end()-1; it++){
    std::vector<LocalizedObject::Point>::iterator jt = it + 1;
    if( ((it->y > pt.y) != (jt->y > pt.y)) && (pt.x < it->x + (double(jt->x) - it->x)*(double(pt.y) - it->y)/(double(jt->y) - it->y)) ) inside = !inside;
  }
  std::vector<LocalizedObject::Point>::iterator it = m_vertices.end()-1;
  std::vector<LocalizedObject::Point>::iterator jt = m_vertices.begin();
  if( ((it->y > pt.y) != (jt->y > pt.y)) && (pt.x < it->x + (double(jt->x) - it->x)*(double(pt.y) - it->y)/(double(jt->y) - it->y)) ) inside = !inside;
  return inside;
}

bool Region::contains(uint16_t x, uint16_t y)
{
  bool inside = false;
  for(std::vector<LocalizedObject::Point>::iterator it = m_vertices.begin(); it != m_vertices.end()-1; it++){
    std::vector<LocalizedObject::Point>::iterator jt = it + 1;
    if( ((it->y > y) != (jt->y > y)) && (x < it->x + (double(jt->x) - it->x)*(double(y) - it->y)/(double(jt->y) - it->y)) ) inside = !inside;
  }
  std::vector<LocalizedObject::Point>::iterator it = m_vertices.end()-1;
  std::vector<LocalizedObject::Point>::iterator jt = m_vertices.begin();
  if( ((it->y > y) != (jt->y > y)) && (x < it->x + (double(jt->x) - it->x)*(double(y) - it->y)/(double(jt->y) - it->y)) ) inside = !inside;
  return inside;
}

void Region::getEnclosure(uint16_t& x1, uint16_t& x2, uint16_t& y1, uint16_t& y2)
{
  x1 = 65535;
  x2 = 0;
  y1 = 65535;
  y2 = 0;
  for(std::vector<LocalizedObject::Point>::iterator it = m_vertices.begin(); it != m_vertices.end(); it++){
    if(it->x < x1) x1 = it->x;
    if(it->x > x2) x2 = it->x;
    if(it->y < y1) y1 = it->y;
    if(it->y > y2) y2 = it->y;
  }
}

Mask* Region::getMask(int width, int height, bool outline)
{
  Mask* m = new Mask(width,height);
  uint16_t x1,x2,y1,y2;
  if(outline){
    Mask* m2 = m->getCopy();
    getEnclosure(x1,x2,y1,y2);
    x1++;
    x2--;
    y1++;
    y2--;
    for(uint16_t i = x1; i < x2; i++){
      for(uint16_t j = y1; j < y2; j++){
	if(contains(i,j)) m->setValue(i,j,1);
	else m->setValue(i,j,0);
      }
    }
    for(uint16_t i = x1; i < x2; i++){
      for(uint16_t j = y1; j < y2; j++){
	if(m->getValue(i,j) == 0) continue;
	int sum = 0;
	for(int dx = i - 1; dx < i + 2; dx++){
	  for(int dy = j - 1; dy < j + 2; dy++){
	    int val = m->getValue(dx,dy);
	    sum += val;
	  }
	}
	m2->setValue(i,j,1 - sum/9);
      }
    }
    delete m;
    return m2;
  }
  getEnclosure(x1,x2,y1,y2);
  for(uint16_t i = x1; i < x2; i++){
    for(uint16_t j = y1; j < y2; j++){
      if(contains(i,j)) m->setValue(i,j,1);
    }
  }
  return m;
}
