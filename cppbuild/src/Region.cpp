#include "Region.hpp"

bool Region::contains(LocalizedObject::Point pt)
{
  bool inside = false;
  for(std::vector<LocalizedObject::Point>::iterator it = m_vertices.begin(); it != m_vertices.end()-1; it++){
    std::vector<LocalizedObject::Point>::iterator jt = it + 1;
    if( ((it->y > pt.y) != (jt->y > pt.y)) && (pt.x < it->x + (double(jt->x) - it->x)*(double(pt.y) - it->y)/(double(jt->y) - it->y)) ) inside = !inside;
  }
  std::vector<LocalizedObject::Point>::iterator it = m_vertices.end()-1;
  std::vector<LocalizedObject::Point>::iterator jt = m_vertices.begin();
  if( ((it->y > pt.y) != (jt->y > pt.y)) && (pt.x < it->x + (double(jt->x) - it->x)*(double(pt.y) - it->y)/(double(jt->y) - it->y)) ) inside = !inside;
  return inside;
}

bool Region::contains(int x, int y)
{
  bool inside = false;
  for(std::vector<LocalizedObject::Point>::iterator it = m_vertices.begin(); it != m_vertices.end()-1; it++){
    std::vector<LocalizedObject::Point>::iterator jt = it + 1;
    if( ((it->y > y) != (jt->y > y)) && (x < it->x + (double(jt->x) - it->x)*(double(y) - it->y)/(double(jt->y) - it->y)) ) inside = !inside;
  }
  std::vector<LocalizedObject::Point>::iterator it = m_vertices.end()-1;
  std::vector<LocalizedObject::Point>::iterator jt = m_vertices.begin();
  if( ((it->y > y) != (jt->y > y)) && (x < it->x + (double(jt->x) - it->x)*(double(y) - it->y)/(double(jt->y) - it->y)) ) inside = !inside;
  return inside;
}

void Region::getEnclosure(int& x1, int& x2, int& y1, int& y2)
{
  x1 = 65535;
  x2 = 0;
  y1 = 65535;
  y2 = 0;
  for(std::vector<LocalizedObject::Point>::iterator it = m_vertices.begin(); it != m_vertices.end(); it++){
    if(it->x < x1) x1 = it->x;
    if(it->x > x2) x2 = it->x;
    if(it->y < y1) y1 = it->y;
    if(it->y > y2) y2 = it->y;
  }
}

Mask* Region::getMask(int width, int height, bool outline)
{
  Mask* m = new Mask(width,height);
  int x1,x2,y1,y2;
  if(outline){
    Mask* m2 = m->getCopy();
    getEnclosure(x1,x2,y1,y2);
    x1++;
    x2--;
    y1++;
    y2--;
    for(int i = x1; i < x2; i++){
      for(int j = y1; j < y2; j++){
	if(contains(i,j)) m->setValue(i,j,1);
	else m->setValue(i,j,0);
      }
    }
    for(int i = x1; i < x2; i++){
      for(int j = y1; j < y2; j++){
	if(m->getValue(i,j) == 0) continue;
	int sum = 0;
	for(int dx = i - 1; dx < i + 2; dx++){
	  for(int dy = j - 1; dy < j + 2; dy++){
	    int val = m->getValue(dx,dy);
	    sum += val;
	  }
	}
	m2->setValue(i,j,1 - sum/9);
      }
    }
    delete m;
    for(int i = m_axis.size()-1; i > 0; i--){
      double slope = ((double)(m_axis[i-1].y) - m_axis[i].y) / (m_axis[i-1].x - m_axis[i].x);
      if(fabs(slope) > 1){
	double step = (m_axis[i-1].y - m_axis[i].y)/fabs(m_axis[i-1].y - m_axis[i].y);
	int nsteps = (int)((m_axis[i-1].y - m_axis[i].y)/step);
	double y = m_axis[i].y;
	double x = m_axis[i].x;
	for(int j = 0; j < nsteps; j++){
	  m2->setValue((int)x,(int)y,1);
	  y += step;
	  x += step/slope;
	}
      }
      else{
	double step = (m_axis[i-1].x - m_axis[i].x)/fabs(m_axis[i-1].x - m_axis[i].x);
	int nsteps = (int)((m_axis[i-1].x - m_axis[i].x)/step);
	double x = m_axis[i].x;
	double y = m_axis[i].y;
	for(int j = 0; j < nsteps; j++){
	  m2->setValue((int)x,(int)y,1);
	  x += step;
	  y += step*slope;
	}
      }
    }
    return m2;
  }
  getEnclosure(x1,x2,y1,y2);
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      if(contains(i,j)) m->setValue(i,j,1);
    }
  }
  for(int i = m_axis.size()-1; i > 0; i--){
    double slope = ((double)(m_axis[i-1].y) - m_axis[i].y) / (m_axis[i-1].x - m_axis[i].x);
    if(fabs(slope) > 1){
      double step = (m_axis[i-1].y - m_axis[i].y)/fabs(m_axis[i-1].y - m_axis[i].y);
      int nsteps = (int)((m_axis[i-1].y - m_axis[i].y)/step);
      double y = m_axis[i].y;
      double x = m_axis[i].x;
      for(int j = 0; j < nsteps; j++){
	m->setValue((int)x,(int)y,1);
	y += step;
	x += step/slope;
      }
    }
    else{
      double step = (m_axis[i-1].x - m_axis[i].x)/fabs(m_axis[i-1].x - m_axis[i].x);
      int nsteps = (int)((m_axis[i-1].x - m_axis[i].x)/step);
      double x = m_axis[i].x;
      double y = m_axis[i].y;
      for(int j = 0; j < nsteps; j++){
	m->setValue((int)x,(int)y,1);
	x += step;
	y += step*slope;
      }
    }
  }
  return m;
}

double Region::getLength()
{
  if(m_axis.size() == 0) return 0;
  double len = 0;
  LocalizedObject::Point prev = *(m_axis.begin());
  for(std::vector<LocalizedObject::Point>::iterator it = m_axis.begin(); it != m_axis.end(); it++){
    double distx = (double)(it->x) - prev.x;
    double disty = (double)(it->y) - prev.y;
    len += sqrt(distx*distx + disty*disty);
    prev = *it;
  }
  return len;
}

double Region::axialProjection(LocalizedObject::Point pt)
{
  if(m_axis.size() == 0) return -1;
  double minDist = 99999999.9;
  unsigned minI = 0;
  int minDir = 0;
  double minX = 0;
  double minY = 0;
  for(unsigned i = 0; i < m_axis.size(); i++){
    double distx = (double)(m_axis[i].x) - pt.x;
    double disty = (double)(m_axis[i].y) - pt.y;
    double dist = sqrt(distx*distx + disty*disty);
    if(dist < minDist){
      minDist = dist;
      minI = i;
      minX = m_axis[i].x;
      minY = m_axis[i].y;
    }
  }
  if(minI > 0){
    double slope = ((double)(m_axis[minI-1].y) - m_axis[minI].y) / (m_axis[minI-1].x - m_axis[minI].x);
    if(fabs(slope) > 1){
      bool finished = false;
      double step = (m_axis[minI-1].y - m_axis[minI].y)/fabs(m_axis[minI-1].y - m_axis[minI].y);
      double y = m_axis[minI].y + step;
      double x = m_axis[minI].x + step/slope;
      while(!finished){
	double distx = x - pt.x;
	double disty = y - pt.y;
	double dist = sqrt(distx*distx + disty*disty);
	if(dist < minDist){
	  minDist = dist;
	  minDir = -1;
	  minX = x;
	  minY = y;
	  y += step;
	  x += step/slope;
	}
	else finished = true;
      }
    }
    else{
      bool finished = false;
      double step = (m_axis[minI-1].x - m_axis[minI].x)/fabs(m_axis[minI-1].x - m_axis[minI].x);
      double x = m_axis[minI].x + step;
      double y = m_axis[minI].y + step*slope;
      while(!finished){
	double distx = x - pt.x;
	double disty = y - pt.y;
	double dist = sqrt(distx*distx + disty*disty);
	if(dist < minDist){
	  minDist = dist;
	  minDir = -1;
	  minX = x;
	  minY = y;
	  x += step;
	  y += step*slope;
	}
	else finished = true;
      }
    }
    if(minDir < 0){
      double distx = minX - m_axis[minI-1].x;
      double disty = minY - m_axis[minI-1].y;
      double dist = sqrt(distx*distx + disty*disty);
      for(unsigned i = 0; i < minI-1; i++){
	distx = (double)(m_axis[i+1].x) - m_axis[i].x;
	disty = (double)(m_axis[i+1].y) - m_axis[i].y;
	dist += sqrt(distx*distx + disty*disty);
      }
      return dist;
    }
  }
  if(minI < m_axis.size()-1){
    double slope = ((double)(m_axis[minI+1].y) - m_axis[minI].y) / (m_axis[minI+1].x - m_axis[minI].x);
    if(fabs(slope) > 1){
      bool finished = false;
      double step = (m_axis[minI+1].y - m_axis[minI].y)/fabs(m_axis[minI+1].y - m_axis[minI].y);
      double y = m_axis[minI].y + step;
      double x = m_axis[minI].x + step/slope;
      while(!finished){
	double distx = x - pt.x;
	double disty = y - pt.y;
	double dist = sqrt(distx*distx + disty*disty);
	if(dist < minDist){
	  minDist = dist;
	  minDir = 1;
	  minX = x;
	  minY = y;
	  y += step;
	  x += step/slope;
	}
	else finished = true;
      }
    }
    else{
      bool finished = false;
      double step = (m_axis[minI+1].x - m_axis[minI].x)/fabs(m_axis[minI+1].x - m_axis[minI].x);
      double x = m_axis[minI].x + step;
      double y = m_axis[minI].y + step*slope;
      while(!finished){
	double distx = x - pt.x;
	double disty = y - pt.y;
	double dist = sqrt(distx*distx + disty*disty);
	if(dist < minDist){
	  minDist = dist;
	  minDir = 1;
	  minX = x;
	  minY = y;
	  x += step;
	  y += step*slope;
	}
	else finished = true;
      }
    }
    if(minDir > 0){
      double distx = minX - m_axis[minI].x;
      double disty = minY - m_axis[minI].y;
      double dist = sqrt(distx*distx + disty*disty);
      for(unsigned i = 0; i < minI; i++){
	distx = (double)(m_axis[i+1].x) - m_axis[i].x;
	disty = (double)(m_axis[i+1].y) - m_axis[i].y;
	dist += sqrt(distx*distx + disty*disty);
      }
      return dist;
    }
  }
  double dist = 0;
  for(unsigned i = 0; i < minI; i++){
    double distx = (double)(m_axis[i+1].x) - m_axis[i].x;
    double disty = (double)(m_axis[i+1].y) - m_axis[i].y;
    dist += sqrt(distx*distx + disty*disty);
  }
  return dist;
}

Region* Region::getCopy()
{
  Region* r = new Region(m_vertices);
  r->setAxis(m_axis);
  return r;
}

void Region::writeV00(char* buf, std::ofstream& fout)
{
  buf[0] = (char)nVertices();
  int offset = 1;
  for(std::vector<LocalizedObject::Point>::iterator jt = m_vertices.begin(); jt != m_vertices.end(); jt++){
    NiaUtils::writeShortToBuffer(buf,offset,jt->x);
    NiaUtils::writeShortToBuffer(buf,offset+2,jt->y);
    offset += 4;
  }
  fout.write(buf,offset);
}

void Region::readV00(char* buf, std::ifstream& fin)
{
  fin.read(buf,1);
  int nVerts = (int)buf[0];
  fin.read(buf,4*nVerts);
  int offset = 0;
  for(int j = 0; j < nVerts; j++){
    LocalizedObject::Point pt;
    pt.x = NiaUtils::convertToShort(buf[offset],buf[offset+1]);
    pt.y = NiaUtils::convertToShort(buf[offset+2],buf[offset+3]);
    addVertex(pt);
    offset += 4;
  }
}

void Region::write(char* buf, std::ofstream& fout)
{
  buf[0] = (char)nVertices();
  int offset = 1;
  for(std::vector<LocalizedObject::Point>::iterator jt = m_vertices.begin(); jt != m_vertices.end(); jt++){
    NiaUtils::writeShortToBuffer(buf,offset,jt->x);
    NiaUtils::writeShortToBuffer(buf,offset+2,jt->y);
    offset += 4;
  }
  buf[offset] = (char)m_axis.size();
  offset++;
  for(std::vector<LocalizedObject::Point>::iterator jt = m_axis.begin(); jt != m_axis.end(); jt++){
    NiaUtils::writeShortToBuffer(buf,offset,jt->x);
    NiaUtils::writeShortToBuffer(buf,offset+2,jt->y);
    offset += 4;
  }
  fout.write(buf,offset);
}

void Region::read(char* buf, std::ifstream& fin)
{
  fin.read(buf,1);
  int nVerts = (int)buf[0];
  fin.read(buf,4*nVerts);
  int offset = 0;
  for(int j = 0; j < nVerts; j++){
    LocalizedObject::Point pt;
    pt.x = NiaUtils::convertToShort(buf[offset],buf[offset+1]);
    pt.y = NiaUtils::convertToShort(buf[offset+2],buf[offset+3]);
    addVertex(pt);
    offset += 4;
  }
  fin.read(buf,1);
  nVerts = (int)buf[0];
  fin.read(buf,4*nVerts);
  offset = 0;
  for(int j = 0; j < nVerts; j++){
    LocalizedObject::Point pt;
    pt.x = NiaUtils::convertToShort(buf[offset],buf[offset+1]);
    pt.y = NiaUtils::convertToShort(buf[offset+2],buf[offset+3]);
    m_axis.push_back(pt);
    offset += 4;
  }
}
