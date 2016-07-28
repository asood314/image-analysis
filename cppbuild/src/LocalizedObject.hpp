#ifndef LOCALIZED_OBJECT_HPP
#define LOCALIZED_OBJECT_HPP

#include <iostream>
#include <vector>

class LocalizedObject
{

public:
  typedef struct point{
    int x;
    int y;
    point(){}
    point(int i, int j){
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
  int overlapWith(LocalizedObject& obj);
  static int findOverlap(std::vector<LocalizedObject*> objs);
  double distanceTo(Point pt);
  double distanceTo(LocalizedObject& obj);
  static double findMaxDistance(std::vector<LocalizedObject*> objs);
  Point center(){ return m_center; }
};

#endif
