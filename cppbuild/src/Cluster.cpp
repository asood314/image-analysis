#include "Cluster.hpp"
#include <cmath>

Cluster::Cluster() :
  m_peak(0),
  m_total(0)
{
}

Cluster::~Cluster()
{
  m_points.clear();
}

Cluster* Cluster::getCopy()
{
  Cluster* c = new Cluster();
  for(std::vector<LocalizedObject::Point>::iterator it = m_points.begin(); it != m_points.end(); it++) c->addPoint(*it);
  c->setPeakIntensity(m_peak);
  c->setIntegratedIntensity(m_total);
  c->setCenter(m_center);
  return c;
}

void Cluster::addPoint(int x, int y)
{
  LocalizedObject::Point pt;
  pt.x = x;
  pt.y = y;
  m_points.push_back(pt);
}

void Cluster::addPoint(int x, int y, int i)
{
  LocalizedObject::Point pt;
  pt.x = x;
  pt.y = y;
  m_points.push_back(pt);
  m_total += i;
}

void Cluster::addPoint(LocalizedObject::Point pt, int i)
{
  m_points.push_back(pt);
  m_total += i;
}

void Cluster::removePoint(LocalizedObject::Point pt)
{
  for(std::vector<LocalizedObject::Point>::iterator it = m_points.begin(); it != m_points.end(); it++){
    if(it->x == pt.x && it->y == pt.y){
      m_points.erase(it);
      return;
    }
  }
}

int Cluster::indexOf(LocalizedObject::Point pt)
{
  int i = 0;
  for(std::vector<LocalizedObject::Point>::iterator it = m_points.begin(); it != m_points.end(); it++){
    if(it->x == pt.x && it->y == pt.y) return i;
    i++;
  }
  return i;
}

void Cluster::findBorder()
{
  m_border.clear();
  int minX = 999999;
  int minY = 999999;
  int maxX = 0;
  int maxY = 0;
  for(std::vector<LocalizedObject::Point>::iterator kt = m_points.begin(); kt != m_points.end(); kt++){
    if(kt->x > maxX) maxX = kt->x;
    if(kt->x < minX) minX = kt->x;
    if(kt->y > maxY) maxY = kt->y;
    if(kt->y < minY) minY = kt->y;
  }
  Mask* m = new Mask(maxX-minX+3,maxY-minY+3,0);
  for(std::vector<LocalizedObject::Point>::iterator kt = m_points.begin(); kt != m_points.end(); kt++) m->setValue(kt->x - minX + 1,kt->y - minY + 1,1);
  int dx[8] = {-minX,-minX,-minX,1-minX,1-minX,2-minX,2-minX,2-minX};
  int dy[8] = {-minY,1-minY,2-minY,-minY,2-minY,-minY,1-minY,2-minY};
  for(std::vector<LocalizedObject::Point>::iterator kt = m_points.begin(); kt != m_points.end(); kt++){
    for(int i = 0; i < 8; i++){
      if(m->getValue(kt->x+dx[i],kt->y+dy[i]) != 1){
	m_border.push_back(*kt);
	break;
      }
    }
  }
  delete m;
  /*
  for(std::vector<LocalizedObject::Point>::iterator kt = m_points.begin(); kt != m_points.end(); kt++){
    int sum = 0;
    for(std::vector<LocalizedObject::Point>::iterator it = m_points.begin(); it != m_points.end(); it++){
      if(abs(it->x - kt->x) < 2 && abs(it->y - kt->y) < 2) sum++;
    }
    if(sum < 9) m_border.push_back(*kt);
  }
  */
}

int Cluster::getBorderLength(Cluster* c)
{
  int len = 0;
  if(c->perimeter() == 0) c->findBorder();
  for(std::vector<LocalizedObject::Point>::iterator it = m_border.begin(); it != m_border.end(); it++){
    for(std::vector<LocalizedObject::Point>::iterator jt = c->borderBegin(); jt != c->borderEnd(); jt++){
      if(abs(it->x - jt->x) < 2 && abs(it->y - jt->y) < 2){
	len++;
	break;
      }
    }
  }
  /*
  std::vector<LocalizedObject::Point> points2 = c->getPoints();
  for(std::vector<LocalizedObject::Point>::iterator it = m_points.begin(); it != m_points.end(); it++){
    for(std::vector<LocalizedObject::Point>::iterator it2 = points2.begin(); it2 != points2.end(); it2++){
      int xdiff = 0;
      int ydiff = 0;
      if(it2->x > it->x) xdiff = it2->x - it->x;
      else xdiff = it->x - it2->x;
      if(xdiff > 1) continue;
      if(it2->y > it->y) ydiff = it2->y - it->y;
      else ydiff = it->y - it2->y;
      if(ydiff < 2){
	len++;
	break;
      }
    }
  }
  */
  return len;
}

void Cluster::add(Cluster* c)
{
  std::vector<LocalizedObject::Point> points2 = c->getPoints();
  for(std::vector<LocalizedObject::Point>::iterator it = points2.begin(); it != points2.end(); it++) m_points.push_back(*it);
  //for(std::vector<LocalizedObject::Point>::iterator it = c->borderBegin(); it != c->borderEnd(); it++) m_border.push_back(*it);
  if(c->peak() > m_peak) m_peak = c->peak();
  m_total += c->integratedIntensity();
}

double Cluster::peakToPeakDistance2(Cluster* c)
{
  LocalizedObject::Point pt = c->getPoint(0);
  LocalizedObject::Point pt2 = m_points.at(0);
  return pow(pt.x - pt2.x,2) + pow(pt.y - pt2.y,2);
}

void Cluster::computeCenter()
{
  int x = 0;
  int y = 0;
  int size = 0;
  for(std::vector<LocalizedObject::Point>::iterator it = m_points.begin(); it != m_points.end(); it++){
    x += it->x;
    y += it->y;
    size++;
  }
  x = x / size;
  y = y / size;
  m_center.x = x;
  m_center.y = y;
}

bool Cluster::contains(LocalizedObject::Point pt)
{
  for(std::vector<LocalizedObject::Point>::iterator it = m_points.begin(); it != m_points.end(); it++){
    if(it->x == pt.x && it->y == pt.y) return true;
  }
  return false;
}

Mask* Cluster::getMask(int width, int height, bool outline)
{
  Mask* m = new Mask(width,height);
  if(outline){
    Mask* m2 = m->getCopy();
    for(std::vector<LocalizedObject::Point>::iterator kt = m_points.begin(); kt != m_points.end(); kt++) m->setValue(kt->x,kt->y,1);
    for(std::vector<LocalizedObject::Point>::iterator kt = m_points.begin(); kt != m_points.end(); kt++){
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
  for(std::vector<LocalizedObject::Point>::iterator kt = m_points.begin(); kt != m_points.end(); kt++) m->setValue(kt->x,kt->y,1);
  return m;
}

void Cluster::write(char* buf, std::ofstream& fout)
{
  NiaUtils::writeShortToBuffer(buf,0,size());
  NiaUtils::writeShortToBuffer(buf,2,peak());
  NiaUtils::writeIntToBuffer(buf,4,integratedIntensity());
  long offset = 8;
  for(std::vector<LocalizedObject::Point>::iterator jt = m_points.begin(); jt != m_points.end(); jt++){
    NiaUtils::writeShortToBuffer(buf,offset,jt->x);
    NiaUtils::writeShortToBuffer(buf,offset+2,jt->y);
    offset += 4;
  }
  fout.write(buf,offset);
}

void Cluster::read(char* buf, std::ifstream& fin)
{
  fin.read(buf,8);
  int npix = NiaUtils::convertToShort(buf[0],buf[1]);
  setPeakIntensity(NiaUtils::convertToShort(buf[2],buf[3]));
  setIntegratedIntensity(NiaUtils::convertToInt(buf[4],buf[5],buf[6],buf[7]));
  fin.read(buf,4*npix);
  long offset = 0;
  for(int j = 0; j < npix; j++){
    addPoint(NiaUtils::convertToShort(buf[offset],buf[offset+1]),NiaUtils::convertToShort(buf[offset+2],buf[offset+3]));
    offset += 4;
  }
}
