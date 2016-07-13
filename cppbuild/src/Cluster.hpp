#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "LocalizedObject.hpp"
#include "Mask.hpp"

class Cluster : public LocalizedObject
{

protected:
  std::vector<LocalizedObject::Point> m_points;
  uint16_t m_peak;
  uint32_t m_total;

public:
  Cluster();
  virtual ~Cluster();
  Cluster* getCopy();
  void addPoint(uint16_t x, uint16_t y);
  void addPoint(LocalizedObject::Point pt){ m_points.push_back(pt); }
  void addPoint(uint16_t x, uint16_t y, uint16_t i);
  void addPoint(LocalizedObject::Point pt, uint16_t i);
  LocalizedObject::Point getPoint(uint32_t index){ return m_points.at(index); }
  void setPoint(uint32_t index, LocalizedObject::Point pt){ m_points.at(index) = pt; }
  void removePoint(uint32_t index){ m_points.erase(m_points.begin()+index); }
  void removePoint(LocalizedObject::Point pt);
  void clearPoints(){ m_points.clear(); }
  void setPeakIntensity(uint16_t i){ m_peak = i; }
  uint16_t peak(){ return m_peak; }
  void setIntegratedIntensity(uint32_t i){ m_total = i; }
  uint32_t integratedIntensity(){ return m_total; }
  uint32_t indexOf(LocalizedObject::Point pt);
  uint32_t size(){ return m_points.size(); }
  uint32_t getBorderLength(Cluster* c);
  void add(Cluster* c);
  double peakToPeakDistance2(Cluster* c);
  Mask* getMask(int width, int height, bool outline=false);
  void setCenter(LocalizedObject::Point cent){ m_center = cent; }
  void computeCenter();
  bool contains(LocalizedObject::Point pt);
  std::vector<LocalizedObject::Point> getPoints(){ return m_points; }
  
};

#endif
