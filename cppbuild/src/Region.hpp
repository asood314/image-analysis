#ifndef REGION_HPP
#define REGION_HPP

#include "LocalizedObject.hpp"

class Region
{

protected:
  std::vector<LocalizedObject::Point> m_vertices;

public:
  Region(){}
  Region(std::vector<LocalizedObject::Point> verts){ m_vertices = verts; }
  virtual ~Region(){}
  bool contains(LocalizedObject::Point& pt);
  void addVertex(LocalizedObject::Point pt){ m_vertices.push_back(pt); }
  uint8_t nVertices(){ return m_vertices.size(); };
  LocalizedObject::Point getVertex(uint8_t index){ return m_vertices.at(index); }
  std::vector<LocalizedObject::Point> vertices(){ return m_vertices; }

};

#endif
