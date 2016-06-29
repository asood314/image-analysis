#include "LocalizedObject.hpp"
#include <cmath>

uint32_t LocalizedObject::overlapWith(LocalizedObject& obj)
{
  uint32_t overlap = 0;
  std::vector<Point> points = obj.getPoints();
  for(std::vector<Point>::iterator it = points.begin(); it != points.end(); it++){
    if(contains(*it)) overlap++;
  }
  return overlap;
}

uint32_t LocalizedObject::findOverlap(std::vector<LocalizedObject*> objs)
{
  uint32_t overlap = 0;
  std::vector<Point> points = objs.at(0)->getPoints();
  for(std::vector<Point>::iterator it = points.begin(); it != points.end(); it++){
    bool overlapping = true;
    for(std::vector<LocalizedObject*>::iterator jt = objs.begin()+1; jt != objs.end(); jt++) overlapping = overlapping && (*jt)->contains(*it);
    if(overlapping) overlap++;
  }
  return overlap;
}

double LocalizedObject::distanceTo(Point pt)
{
  double distx = (double)m_center.x - pt.x;
  double disty = (double)m_center.y - pt.y;
  return sqrt(distx*distx - disty*disty);
}

double LocalizedObject::distanceTo(LocalizedObject& obj)
{
  return distanceTo(obj.center());
}

double findMaxDistance(std::vector<LocalizedObject*> objs)
{
  double max = 0.0;
  for(std::vector<LocalizedObject*>::iterator it = objs.begin(); it != objs.end(); it++){
    for(std::vector<LocalizedObject*>::iterator jt = it+1; jt != objs.end(); jt++){
      double dist = (*it)->distanceTo((*jt)->center());
      if(dist > max) max = dist;
    }
  }
  return max;
}
