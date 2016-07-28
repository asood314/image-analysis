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

void Synapse::addPunctum(Cluster* c, int index)
{
  m_puncta.push_back(c);
  m_indices.push_back(index);
}

int Synapse::punctaOverlap()
{
  std::vector<LocalizedObject*> c;
  for(std::vector<Cluster*>::iterator it = m_puncta.begin(); it != m_puncta.end(); it++)
    c.push_back(dynamic_cast<LocalizedObject*>(*it));
  return LocalizedObject::findOverlap(c);
}

int Synapse::punctaOverlap(std::vector<int> indices)
{
  std::vector<LocalizedObject*> c;
  for(std::vector<int>::iterator it = indices.begin(); it != indices.end(); it++)
    c.push_back(dynamic_cast<LocalizedObject*>(m_puncta.at(*it)));
  return LocalizedObject::findOverlap(c);
}

int Synapse::size()
{
  int size = 0;
  for(std::vector<Cluster*>::iterator it = m_puncta.begin(); it!= m_puncta.end(); it++) size += (*it)->size();
  return size - punctaOverlap();
}

double Synapse::maxPunctaDistance(std::vector<int> indices)
{
  std::vector<LocalizedObject*> c;
  for(std::vector<int>::iterator it = indices.begin(); it != indices.end(); it++)
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
  int x = 0;
  int y = 0;
  int n = 0;
  for(std::vector<Cluster*>::iterator it = m_puncta.begin(); it != m_puncta.end(); it++){
    LocalizedObject::Point pt = (*it)->center();
    x += pt.x;
    y += pt.y;
    n++;
  }
  m_center.x = (x / n);
  m_center.y = (y / n);
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

Mask* Synapse::getMask(int width, int height, bool outline)
{
  Mask* m = new Mask(width,height);
  if(outline){
    Mask* m2 = m->getCopy();
    std::vector<LocalizedObject::Point> pts = getPoints();
    for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,1);
    for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++){
      int sum = 0;
      for(int dx = kt->x - 1; dx < kt->x + 2; dx++){
	if(dx < 0 || dx >= width){
	  sum += 3;
	  continue;
	}
	for(int dy = kt->y - 1; dy < kt->y + 2; dy++){
	  if(dy < 0 || dy >= height) sum += 1;
	  else sum += m->getValue(dx,dy);
	}
      }
      m2->setValue(kt->x,kt->y,1 - sum/9);
    }
    delete m;
    return m2;
  }
  std::vector<LocalizedObject::Point> pts = getPoints();
  for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,1);
  return m;
}
