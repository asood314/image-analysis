#include "Segment.hpp"

Segment::Segment()
{
  m_cluster = NULL;
  m_circularity = -1;
  m_eigenVector1 = -1;
  m_eigenVector2 = -1;
  m_parent = NULL;
}

Segment::Segment(Cluster* c)
{
  m_cluster = c;
  m_circularity = -1;
  m_eigenVector1 = -1;
  m_eigenVector2 = -1;
  //findOrientation();
  m_parent = NULL;
}

Segment::~Segment()
{
  if(m_cluster) delete m_cluster;
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
  m_eigenVector1 = atan((eigenValue1 - vxx) / vxy);
  m_eigenVector2 = atan((eigenValue2 - vxx) / vxy);
}

void Segment::setCluster(Cluster* c)
{
  if(m_cluster) delete m_cluster;
  m_cluster = c;
}

void Segment::merge(Segment* s)
{
  m_cluster->add(s->cluster());
  m_cluster->computeCenter();
  m_cluster->findBorder();
  findOrientation();
}

void Segment::write(char* buf, std::ofstream& fout)
{
  NiaUtils::writeDoubleToBuffer(buf,0,m_circularity*1000);
  NiaUtils::writeDoubleToBuffer(buf,4,(m_eigenVector1+1.5708)*1000);
  NiaUtils::writeDoubleToBuffer(buf,8,(m_eigenVector2+1.5708)*1000);
  fout.write(buf,12);
  m_cluster->write(buf,fout);
}

void Segment::read(char* buf, std::ifstream& fin)
{
  fin.read(buf,12);
  m_circularity = NiaUtils::convertToDouble(buf[0],buf[1],buf[2],buf[3]) / 1000;
  m_eigenVector1 = NiaUtils::convertToDouble(buf[4],buf[5],buf[6],buf[7]) / 1000 - 1.5708;
  m_eigenVector2 = NiaUtils::convertToDouble(buf[8],buf[9],buf[10],buf[11]) / 1000 - 1.5708;
  m_cluster = new Cluster();
  m_cluster->read(buf,fin);
  m_cluster->computeCenter();
}
