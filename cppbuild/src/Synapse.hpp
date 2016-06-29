#ifndef SYNAPSE_HPP
#define SYNAPSE_HPP

#include "Cluster.hpp"

class Synapse : public LocalizedObject
{

protected:
  std::vector<Cluster*> m_puncta;
  std::vector<uint32_t> m_indices;
  double m_colocalizationScore;

public:
  Synapse();
  virtual ~Synapse();
  void addPunctum(Cluster* c, uint32_t index);
  Cluster* getPunctum(uint8_t index){ return m_puncta.at(index); }
  uint32_t getPunctumIndex(uint8_t index){ return m_indices.at(index); }
  uint8_t nPuncta(){ return m_puncta.size(); }
  void setColocalizationScore(double score){ m_colocalizationScore = score; }
  double colocalizationScore(){ return m_colocalizationScore; }
  uint32_t punctaOverlap();
  uint32_t punctaOverlap(std::vector<uint8_t> indices);
  uint32_t size();
  double maxPunctaDistance(std::vector<uint8_t> indices);
  bool isColocalized();
  void computeCenter();
  std::vector<LocalizedObject::Point> getPoints();
  bool contains(LocalizedObject::Point pt);
  
};

#endif
