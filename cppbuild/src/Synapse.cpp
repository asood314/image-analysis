#include "Synapse.hpp"

Synapse::Synapse() :
  m_colocalizationScore(0.0)
{
}

Synapse::~Synapse()
{
  m_puncta.clear();
  m_indices.clear();
}

void Synapse::addPunctum(Cluster* c, uint32_t index)
{
  m_puncta.push_back(c);
  m_indices.push_back(index);
}

uint32_t Synapse::punctaOverlap()
{
  std::vector<LocalizedObject*> c;
  for(std::vector<Cluster*>::iterator it = m_puncta.begin(); it != m_puncta.end(); it++)
    c.push_back(dynamic_cast<LocalizedObject*>(*it));
  return LocalizedObject::findOverlap(c);
}

uint32_t Synapse::punctaOverlap(std::vector<uint8_t> indices)
{
  std::vector<LocalizedObject*> c;
  for(std::vector<uint8_t>::iterator it = indices.begin(); it != indices.end(); it++)
    c.push_back(dynamic_cast<LocalizedObject*>(m_puncta.at(*it)));
  return LocalizedObject::findOverlap(c);
}

uint32_t Synapse::size()
{
  uint32_t size = 0;
  for(std::vector<Cluster*>::iterator it = m_puncta.begin(); it!= m_puncta.end(); it++) size += (*it)->size();
  return size - punctaOverlap();
}

double Synapse::maxPunctaDistance(std::vector<uint8_t> indices)
{
  std::vector<LocalizedObject*> c;
  for(std::vector<uint8_t>::iterator it = indices.begin(); it != indices.end(); it++)
    c.push_back(dynamic_cast<LocalizedObject*>(m_puncta.at(*it)));
  return LocalizedObject::findMaxDistance(c);
}

bool Synapse::isColocalized()
{
  if(m_colocalizationScore > 0.0) return true;
  return false;
}

void Synapse::computeCenter()
{
  uint32_t x = 0;
  uint32_t y = 0;
  uint32_t n = 0;
  for(std::vector<Cluster*>::iterator it = m_puncta.begin(); it != m_puncta.end(); it++){
    LocalizedObject::Point pt = (*it)->center();
    x += pt.x;
    y += pt.y;
    n++;
  }
  m_center.x = (uint16_t)(x / n);
  m_center.y = (uint16_t)(y / n);
}

std::vector<LocalizedObject::Point> Synapse::getPoints()
{
  std::vector<LocalizedObject::Point> retval;
  for(std::vector<Cluster*>::iterator it = m_puncta.begin(); it != m_puncta.end(); it++){
    std::vector<LocalizedObject::Point> points = (*it)->getPoints();
    for(std::vector<LocalizedObject::Point>::iterator jt = points.begin(); jt != points.end(); jt++) retval.push_back(*jt);
  }
  return retval;
}

bool Synapse::contains(LocalizedObject::Point pt)
{
  for(std::vector<Cluster*>::iterator it = m_puncta.begin(); it != m_puncta.end(); it++){
    if((*it)->contains(pt)) return true;
  }
  return false;
}
