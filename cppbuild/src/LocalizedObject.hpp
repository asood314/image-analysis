#ifndef LOCALIZED_OBJECT_HPP
#define LOCALIZED_OBJECT_HPP

#include <iostream>
#include <vector>

class LocalizedObject
{

public:
  typedef struct point{
    uint16_t x;
    uint16_t y;
    point(){}
    point(uint16_t i, uint16_t j){
      x = i;
      y = j;
    }
  } Point;

protected:
  Point m_center;

public:
  LocalizedObject(){}
  virtual ~LocalizedObject(){}
  virtual void computeCenter(){}
  virtual bool contains(Point pt){ return false; }
  virtual std::vector<Point> getPoints(){ return std::vector<Point>(); }
  uint32_t overlapWith(LocalizedObject& obj);
  static uint32_t findOverlap(std::vector<LocalizedObject*> objs);
  double distanceTo(Point pt);
  double distanceTo(LocalizedObject& obj);
  static double findMaxDistance(std::vector<LocalizedObject*> objs);
  Point center(){ return m_center; }
};

#endif
