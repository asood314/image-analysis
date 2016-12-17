#include "Segment.hpp"

Segment::Segment()
{
  m_cluster = NULL;
  m_circularity = -1;
  m_eigenDir1 = -1;
  m_eigenDir2 = -1;
  m_parent = NULL;
}

Segment::Segment(Cluster* c)
{
  m_cluster = c;
  m_circularity = -1;
  m_eigenDir1 = -1;
  m_eigenDir2 = -1;
  //findOrientation();
  m_parent = NULL;
}

Segment::~Segment()
{
  delete m_cluster;
}

void Segment::findOrientation()
{
  LocalizedObject::Point c = m_cluster->center();
  double vxx = 0.0;
  double vyy = 0.0;
  double vxy = 0.0;
  for(std::vector<LocalizedObject::Point>::iterator it = m_cluster->begin(); it != m_cluster->end(); it++){
    int xdiff = it->x - c.x;
    int ydiff = it->y - c.y;
    vxx += xdiff*xdiff;
    vyy += ydiff*ydiff;
    vxy += xdiff*ydiff;
  }
  double det = sqrt((vxx - vyy)*(vxx-vyy) + 4*vxy*vxy);
  double eigenValue1 = (vxx + vyy + det) / 2.0;
  double eigenValue2 = (vxx + vyy - det) / 2.0;
  m_circularity = eigenValue2 / eigenValue1;
  m_eigenDir1 = atan((eigenValue1 - vxx) / vxy);
  m_eigenDir2 = atan((eigenValue2 - vxx) / vxy);
}

void Segment::setCluster(Cluster* c)
{
  if(m_cluster) delete m_cluster;
  m_cluster = c;
}
