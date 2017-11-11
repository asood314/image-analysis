#ifndef SEGMENT_HPP
#define SEGMENT_HPP

#include <iostream>
#include <fstream>
#include <cmath>
#include "Cluster.hpp"
#include "NiaUtils.hpp"

class Segment
{

public:
  enum SegmentType{
    SOMA=0,DENDRITE,AXON,
    PRIMARY_DENDRITE,SECONDARY_DENDRITE,TERTIARY_DENDRITE
  };
  
protected:
  Cluster* m_cluster;
  double m_circularity,m_eigenVector1,m_eigenVector2;
  SegmentType m_type;
  Segment* m_parent;
  std::vector<Segment*> m_children;

public:
  double dendriteArea,dendriteLength;
  std::vector<int> nSynapses,nStormSynapses,nPuncta;
  std::vector<double> avgSynapseSize,avgPunctaSize,avgPeakIntensity,avgIntegratedIntensity;
  std::vector< std::vector<double> > avgOverlap;
  Segment();
  Segment(Cluster* c);
  virtual ~Segment();
  void findOrientation();
  void setCluster(Cluster* c);
  void merge(Segment* s);
  void write(char* buf, std::ofstream& fout);
  void read(char* buf, std::ifstream& fin);
  Cluster* cluster(){ return m_cluster; }
  double circularity(){ return m_circularity; }
  double eigenVector1(){ return m_eigenVector1; }
  double eigenVector2(){ return m_eigenVector2; }
  void setType(SegmentType t){ m_type = t; }
  SegmentType type(){ return m_type; }
  void setParent(Segment* p){ m_parent = p; }
  Segment* parent(){ return m_parent; }
  void addChild(Segment* c){ m_children.push_back(c); }
  Segment* child(int i){ return m_children[i]; }
  std::vector<Segment*>::iterator firstChild(){ return m_children.begin(); }
  std::vector<Segment*>::iterator lastChild(){ return m_children.end(); }
  Mask* getMask(int width, int height, bool outline=false, int num=-1);
  std::vector<LocalizedObject::Point> getLabel(int regNum);
  int size(){ return m_cluster->size(); }
  
};

#endif
