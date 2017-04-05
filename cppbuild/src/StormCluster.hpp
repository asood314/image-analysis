#ifndef STORM_CLUSTER_HPP
#define STORM_CLUSTER_HPP

#include <cmath>
#include "StormData.hpp"

class StormCluster{

public:
  typedef struct storm_synapse{
    std::vector<StormCluster*> clusters;
    int centerX,centerY;
  } StormSynapse;

protected:
  std::vector<StormData::Blink> m_molecules;
  double m_centerX,m_centerY,m_centerZ,m_intensity;

public:
  StormCluster();
  virtual ~StormCluster(){}
  static std::vector<StormCluster*> cluster(StormData* blinks);
  void addMolecule(StormData::Blink b);
  void computeCenter();
  double distanceTo(StormCluster* c);
  bool colocalWith(StormCluster* c);
  void shiftXY(double shiftX, double shiftY);
  int nMolecules(){ return m_molecules.size(); }
  StormData::Blink molecule(int i){ return m_molecules[i]; }
  double centerX(){ return m_centerX; }
  double centerY(){ return m_centerY; }
  double centerZ(){ return m_centerZ; }
  double intensity(){ return m_intensity; }
  double x(int i){ return m_molecules[i].x; }
  double y(int i){ return m_molecules[i].y; }
  double z(int i){ return m_molecules[i].z; }
  double sigma1(int i){ return m_molecules[i].sigma1; }
  double sigma2(int i){ return m_molecules[i].sigma2; }
  double intensity(int i){ return m_molecules[i].intensity; }
  
};

#endif
