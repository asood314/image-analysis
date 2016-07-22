#ifndef REGION_HPP
#define REGION_HPP

#include "LocalizedObject.hpp"
#include "Mask.hpp"

class Region
{

protected:
  std::vector<LocalizedObject::Point> m_vertices;

public:
  double dendriteArea;
  std::vector<uint32_t> nSynapses,nPuncta;
  std::vector<double> avgSynapseSize,avgPunctaSize,avgPeakIntensity,avgIntegratedIntensity;
  std::vector< std::vector<double> > avgOverlap;
  Region(){}
  Region(std::vector<LocalizedObject::Point> verts){ m_vertices = verts; }
  virtual ~Region(){}
  bool contains(LocalizedObject::Point pt);
  bool contains(uint16_t x, uint16_t y);
  void getEnclosure(uint16_t& x1, uint16_t& x2, uint16_t& y1, uint16_t& y2);
  Mask* getMask(int width, int height, bool outline=true);
  void addVertex(LocalizedObject::Point pt){ m_vertices.push_back(pt); }
  uint8_t nVertices(){ return m_vertices.size(); }
  LocalizedObject::Point getVertex(uint8_t index){ return m_vertices.at(index); }
  std::vector<LocalizedObject::Point> vertices(){ return m_vertices; }
  Region* getCopy(){ return new Region(m_vertices); }

};

#endif
