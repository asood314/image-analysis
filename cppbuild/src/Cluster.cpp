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
  if(i > m_peak){
    m_peak = i;
    pt = m_points[0];
    m_points[0] = m_points[size()-1];
    m_points[size()-1] = pt;
  }
}

void Cluster::addPoint(LocalizedObject::Point pt, int i)
{
  m_points.push_back(pt);
  m_total += i;
  if(i > m_peak){
    m_peak = i;
    LocalizedObject::Point tmp = m_points[0];
    m_points[0] = m_points[size()-1];
    m_points[size()-1] = tmp;
  }
}

void Cluster::addPoint(int x, int y, bool isBorder)
{
  LocalizedObject::Point pt;
  pt.x = x;
  pt.y = y;
  m_points.push_back(pt);
  if(isBorder) m_border.push_back(pt);
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

void Cluster::removePoint(LocalizedObject::Point pt, int i)
{
  for(std::vector<LocalizedObject::Point>::iterator it = m_points.begin(); it != m_points.end(); it++){
    if(it->x == pt.x && it->y == pt.y){
      m_points.erase(it);
      m_total -= i;
      return;
    }
  }
}

void Cluster::removePoints(std::vector<LocalizedObject::Point> pts, int totI)
{
  for(int i = m_points.size()-1; i > -1; i--){
    for(int j = 0; j < pts.size(); j++){
      if(m_points[i].x == pts[j].x && m_points[i].y == pts[j].y){
	m_points.erase(m_points.begin()+i);
	pts.erase(pts.begin()+j);
	break;
      }
    }
  }
  m_total -= totI;
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
  m_borderIndices.clear();
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
  //for(std::vector<LocalizedObject::Point>::iterator kt = m_points.begin(); kt != m_points.end(); kt++){
  for(int k = 0; k < m_points.size(); k++){
    for(int i = 0; i < 8; i++){
      //if(m->getValue(kt->x+dx[i],kt->y+dy[i]) != 1){
      //m_border.push_back(*kt);
      if(m->getValue(m_points[k].x+dx[i],m_points[k].y+dy[i]) != 1){
	m_border.push_back(m_points[k]);
	m_borderIndices.push_back(k);
	break;
      }
    }
  }
  delete m;
}

bool Cluster::onBorder(LocalizedObject::Point pt, Cluster* c)
{
  Cluster* bord = findBorderWith(c);
  if(bord->size() == 0) return false;
  bool retVal = bord->contains(pt);
  delete bord;
  return retVal;
}

bool Cluster::onBorder(LocalizedObject::Point pt)
{
  for(std::vector<LocalizedObject::Point>::iterator it = m_border.begin(); it != m_border.end(); it++){
    if(it->x == pt.x && it->y == pt.y) return true;
  }
  return false;
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
  return len;
}

Cluster* Cluster::findBorderWith(Cluster* c)
{
  Cluster* retVal = new Cluster();
  if(c->perimeter() == 0) c->findBorder();
  for(std::vector<LocalizedObject::Point>::iterator it = m_border.begin(); it != m_border.end(); it++){
    for(std::vector<LocalizedObject::Point>::iterator jt = c->borderBegin(); jt != c->borderEnd(); jt++){
      if(abs(it->x - jt->x) < 2 && abs(it->y - jt->y) < 2){
	retVal->addPoint(*it);
	break;
      }
    }
  }
  return retVal;
}

Cluster* Cluster::exciseBorderWith(Cluster* c)
{
  Cluster* retVal = new Cluster();
  if(c->perimeter() == 0) c->findBorder();
  //for(std::vector<LocalizedObject::Point>::iterator it = m_points.begin(); it != m_points.end(); it++){
  for(int i = m_border.size()-1; i >= 0; i--){
    for(std::vector<LocalizedObject::Point>::iterator jt = c->borderBegin(); jt != c->borderEnd(); jt++){
      //if(abs(it->x - jt->x) < 2 && abs(it->y - jt->y) < 2){
      if(abs(m_border[i].x - jt->x) < 2 && abs(m_border[i].y - jt->y) < 2){
	//retVal->addPoint(*it);
	retVal->addPoint(m_border[i]);
	m_border.erase(m_border.begin()+i);
	m_points.erase(m_points.begin()+m_borderIndices[i]);
	break;
      }
    }
  }
  return retVal;
}

std::vector<LocalizedObject::Point> Cluster::findClosestPoints(Cluster* c)
{
  std::vector<LocalizedObject::Point> retVal;
  retVal.push_back(m_center);
  retVal.push_back(c->center());
  double minDist = sqrt((m_center.x - retVal[1].x)*(m_center.x - retVal[1].x) + (m_center.y - retVal[1].y)*(m_center.y - retVal[1].y));
  if(c->perimeter() == 0) c->findBorder();
  LocalizedObject::Point borderCenter;
  borderCenter.x = 0;
  borderCenter.y = 0;
  int nborder = 0;
  for(std::vector<LocalizedObject::Point>::iterator it = m_border.begin(); it != m_border.end(); it++){
    for(std::vector<LocalizedObject::Point>::iterator jt = c->borderBegin(); jt != c->borderEnd(); jt++){
      double dist = sqrt((it->x - jt->x)*(it->x - jt->x) + (it->y - jt->y)*(it->y - jt->y));
      if(dist < minDist){
	minDist = dist + 0.01;
	retVal[0] = *it;
	retVal[1] = *jt;
	if(dist < 1.4){
	  borderCenter.x += it->x;
	  borderCenter.y += it->y;
	  nborder++;
	  break;
	}
      }
    }
  }
  if(nborder > 0){
    borderCenter.x = borderCenter.x / nborder;
    borderCenter.y = borderCenter.y / nborder;
    retVal[0] = borderCenter;
    retVal[1] = borderCenter;
  }
  return retVal;
}

void Cluster::add(Cluster* c)
{
  std::vector<LocalizedObject::Point> points2 = c->getPoints();
  int s = size();
  for(std::vector<LocalizedObject::Point>::iterator it = points2.begin(); it != points2.end(); it++) m_points.push_back(*it);
  if(c->peak() > m_peak){
    m_peak = c->peak();
    LocalizedObject::Point tmp = m_points[0];
    m_points[0] = m_points[s];
    m_points[s] = tmp;
  }
  m_total += c->integratedIntensity();
}

double Cluster::peakToPeakDistance2(Cluster* c)
{
  LocalizedObject::Point pt = c->getPoint(0);
  LocalizedObject::Point pt2 = m_points[0];
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

bool Cluster::contains(int x, int y)
{
  for(std::vector<LocalizedObject::Point>::iterator it = m_points.begin(); it != m_points.end(); it++){
    if(it->x == x && it->y == y) return true;
  }
  return false;
}

void Cluster::swap(int index1, int index2)
{
  LocalizedObject::Point tmp = m_points[index1];
  m_points[index1] = m_points[index2];
  m_points[index2] = tmp;
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

void Cluster::dump()
{
  std::cout << "Peak Intensity: " << m_peak << std::endl;
  std::cout << "Total Intensity: " << m_total << std::endl;
  for(std::vector<LocalizedObject::Point>::iterator pit = m_points.begin(); pit != m_points.end(); pit++){
    std::cout << "(" << pit->x << "," << pit->y << ")" << std::endl;
  }
}
