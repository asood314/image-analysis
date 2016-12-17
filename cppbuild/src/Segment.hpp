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
  double m_circularity,m_eigenDir1,m_eigenDir2;
  SegmentType m_type;
  Segment* m_parent;
  std::vector<Segment*> m_children;

public:
  Segment();
  Segment(Cluster* c);
  virtual ~Segment();
  void findOrientation();
  void setCluster(Cluster* c);
  Cluster* cluster(){ return m_cluster; }
  double circularity(){ return m_circularity; }
  double eigenDirection1(){ return m_eigenDir1; }
  double eigenDirection2(){ return m_eigenDir2; }
  void setType(SegmentType t){ m_type = t; }
  SegmentType type(){ return m_type; }
  void setParent(Segment* p){ m_parent = p; }
  Segment* parent(){ return m_parent; }
  void addChild(Segment* c){ m_children.push_back(c); }
  Segment* child(int i){ return m_children[i]; }
  std::vector<Segment*>::iterator firstChild(){ return m_children.begin(); }
  std::vector<Segment*>::iterator lastChild(){ return m_children.end(); }
  Mask* getMask(int width, int height, bool outline=false){ return m_cluster->getMask(width,height,outline); }
  
};

#endif
