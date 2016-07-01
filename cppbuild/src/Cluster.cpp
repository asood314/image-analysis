#include "Cluster.hpp"
#include <cmath>

Cluster::Cluster() :
  m_peak(0),
  m_total(0)
{
}

Cluster::~Cluster()
{
  m_points.clear();
}

void Cluster::addPoint(uint16_t x, uint16_t y)
{
  LocalizedObject::Point pt;
  pt.x = x;
  pt.y = y;
  m_points.push_back(pt);
}

void Cluster::addPoint(uint16_t x, uint16_t y, uint16_t i)
{
  LocalizedObject::Point pt;
  pt.x = x;
  pt.y = y;
  m_points.push_back(pt);
  m_total += i;
}

void Cluster::addPoint(LocalizedObject::Point pt, uint16_t i)
{
  m_points.push_back(pt);
  m_total += i;
}

void Cluster::removePoint(LocalizedObject::Point pt)
{
  for(std::vector<LocalizedObject::Point>::iterator it = m_points.begin(); it != m_points.end(); it++){
    if(it->x == pt.x && it->y == pt.y){
      m_points.erase(it);
      return;
    }
  }
}

uint32_t Cluster::indexOf(LocalizedObject::Point pt)
{
  uint32_t i = 0;
  for(std::vector<LocalizedObject::Point>::iterator it = m_points.begin(); it != m_points.end(); it++){
    if(it->x == pt.x && it->y == pt.y) return i;
    i++;
  }
  return i;
}

uint32_t Cluster::getBorderLength(Cluster* c)
{
  uint32_t len = 0;
  std::vector<LocalizedObject::Point> points2 = c->getPoints();
  for(std::vector<LocalizedObject::Point>::iterator it = m_points.begin(); it != m_points.end(); it++){
    for(std::vector<LocalizedObject::Point>::iterator it2 = points2.begin(); it2 != points2.end(); it2++){
      uint16_t xdiff = 0;
      uint16_t ydiff = 0;
      if(it2->x > it->x) xdiff = it2->x - it->x;
      else xdiff = it->x - it2->x;
      if(xdiff > 1) continue;
      if(it2->y > it->y) ydiff = it2->y - it->y;
      else ydiff = it->y - it2->y;
      if(ydiff < 2){
	len++;
	break;
      }
    }
  }
  return len;
}

void Cluster::add(Cluster* c)
{
  std::vector<LocalizedObject::Point> points2 = c->getPoints();
  for(std::vector<LocalizedObject::Point>::iterator it = points2.begin(); it != points2.end(); it++) m_points.push_back(*it);
  if(c->peak() > m_peak) m_peak = c->peak();
  m_total += c->integratedIntensity();
}

double Cluster::peakToPeakDistance2(Cluster* c)
{
  LocalizedObject::Point pt = c->getPoint(0);
  LocalizedObject::Point pt2 = m_points.at(0);
  return pow((int)pt.x - pt2.x,2) + pow((int)pt.y - pt2.y,2);
}

void Cluster::computeCenter()
{
  uint32_t x = 0;
  uint32_t y = 0;
  uint32_t size = 0;
  for(std::vector<LocalizedObject::Point>::iterator it = m_points.begin(); it != m_points.end(); it++){
    x += it->x;
    y += it->y;
    size++;
  }
  x = x / size;
  y = y / size;
  m_center.x = (uint16_t)x;
  m_center.y = (uint16_t)y;
}

bool Cluster::contains(LocalizedObject::Point pt)
{
  for(std::vector<LocalizedObject::Point>::iterator it = m_points.begin(); it != m_points.end(); it++){
    if(it->x == pt.x && it->y == pt.y) return true;
  }
  return false;
}
