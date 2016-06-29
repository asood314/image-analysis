#include "Region.hpp"

bool Region::contains(LocalizedObject::Point& pt)
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
