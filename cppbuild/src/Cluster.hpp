#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include <iostream>
#include <fstream>
#include "NiaUtils.hpp"
#include "LocalizedObject.hpp"
#include "Mask.hpp"

class Cluster : public LocalizedObject
{

protected:
  std::vector<LocalizedObject::Point> m_points;
  std::vector<LocalizedObject::Point> m_border;
  int m_peak;
  int m_total;

public:
  Cluster();
  virtual ~Cluster();
  Cluster* getCopy();
  void addPoint(int x, int y);
  void addPoint(LocalizedObject::Point pt){ m_points.push_back(pt); }
  void addPoint(int x, int y, int i);
  void addPoint(LocalizedObject::Point pt, int i);
  LocalizedObject::Point getPoint(int index){ return m_points.at(index); }
  void setPoint(int index, LocalizedObject::Point pt){ m_points.at(index) = pt; }
  void removePoint(int index){ m_points.erase(m_points.begin()+index); }
  void removePoint(LocalizedObject::Point pt);
  void clearPoints(){ m_points.clear(); }
  void setPeakIntensity(int i){ m_peak = i; }
  int peak(){ return m_peak; }
  void setIntegratedIntensity(int i){ m_total = i; }
  int integratedIntensity(){ return m_total; }
  int indexOf(LocalizedObject::Point pt);
  int size(){ return m_points.size(); }
  void findBorder();
  int perimeter(){ return m_border.size(); }
  int getBorderLength(Cluster* c);
  Cluster* findBorderWith(Cluster* c);
  void add(Cluster* c);
  double peakToPeakDistance2(Cluster* c);
  Mask* getMask(int width, int height, bool outline=false);
  void write(char* buf, std::ofstream& fout);
  void read(char* buf, std::ifstream& fin);
  void setCenter(LocalizedObject::Point cent){ m_center = cent; }
  void computeCenter();
  bool contains(LocalizedObject::Point pt);
  std::vector<LocalizedObject::Point> getPoints(){ return m_points; }
  std::vector<LocalizedObject::Point>::iterator begin(){ return m_points.begin(); }
  std::vector<LocalizedObject::Point>::iterator end(){ return m_points.end(); }
  std::vector<LocalizedObject::Point>::iterator borderBegin(){ return m_border.begin(); }
  std::vector<LocalizedObject::Point>::iterator borderEnd(){ return m_border.end(); }
  
};

#endif
