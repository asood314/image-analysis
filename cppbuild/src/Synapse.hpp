#ifndef SYNAPSE_HPP
#define SYNAPSE_HPP

#include "Cluster.hpp"

class Synapse : public LocalizedObject
{

protected:
  std::vector<Cluster*> m_puncta;
  std::vector<int> m_indices;
  double m_colocalizationScore;

public:
  Synapse();
  virtual ~Synapse();
  void addPunctum(Cluster* c, int index);
  Cluster* getPunctum(int index){ return m_puncta.at(index); }
  int getPunctumIndex(int index){ return m_indices.at(index); }
  int nPuncta(){ return m_puncta.size(); }
  void setColocalizationScore(double score){ m_colocalizationScore = score; }
  double colocalizationScore(){ return m_colocalizationScore; }
  int punctaOverlap();
  int punctaOverlap(std::vector<int> indices);
  int size();
  double maxPunctaDistance(std::vector<int> indices);
  bool isColocalized();
  void computeCenter();
  std::vector<LocalizedObject::Point> getPoints();
  bool contains(LocalizedObject::Point pt);
  Mask* getMask(int width, int height, bool outline=false);
  
};

#endif
