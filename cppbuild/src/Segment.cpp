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

Mask* Segment::getMask(int width, int height, bool outline, int num)
{
  if(num < 0) return m_cluster->getMask(width,height,outline);
  Mask* m = new Mask(width,height);
  LocalizedObject::Point top = m_cluster->center();
  LocalizedObject::Point bottom = m_cluster->center();
  LocalizedObject::Point left = m_cluster->center();
  LocalizedObject::Point right = m_cluster->center();
  for(std::vector<LocalizedObject::Point>::iterator kt = m_cluster->begin(); kt != m_cluster->end(); kt++){
    m->setValue(kt->x,kt->y,1);
    if(kt->x > right.x) right = *kt;
    if(kt->y > bottom.y) bottom = *kt;
    if(kt->x < left.x) left = *kt;
    if(kt->y < top.y) top = *kt;
  }
  if(outline){
    Mask* m2 = m->getCopy();
    for(std::vector<LocalizedObject::Point>::iterator kt = m_cluster->begin(); kt != m_cluster->end(); kt++){
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
    std::vector<LocalizedObject::Point> label = getLabel(num);
    if(top.y > 15 && top.x > 10 && top.x < width-10){
      for(std::vector<LocalizedObject::Point>::iterator it = label.begin(); it != label.end(); it++){
	m2->setValue(it->x + top.x - 5, it->y + top.y - 12,1);
      }
    }
    else if(left.x > 15 && left.y > 10 && left.y < height-10){
      for(std::vector<LocalizedObject::Point>::iterator it = label.begin(); it != label.end(); it++){
	m2->setValue(it->x + left.x - 14, it->y + left.y - 4,1);
      }
    }
    else if(right.x < width-15 && right.y > 10 && right.y < height-10){
      for(std::vector<LocalizedObject::Point>::iterator it = label.begin(); it != label.end(); it++){
	m2->setValue(it->x + right.x + 3, it->y + left.y - 4,1);
      }
    }
    else if(bottom.y < height-15 && bottom.x > 10 && bottom.x < width-10){
      for(std::vector<LocalizedObject::Point>::iterator it = label.begin(); it != label.end(); it++){
	m2->setValue(it->x + top.x - 5, it->y + top.y + 3,1);
      }
    }
    return m2;
  }
  std::vector<LocalizedObject::Point> label = getLabel(num);
  if(top.y > 15 && top.x > 10 && top.x < width-10){
    for(std::vector<LocalizedObject::Point>::iterator it = label.begin(); it != label.end(); it++){
      m->setValue(it->x + top.x - 5, it->y + top.y - 12,1);
    }
  }
  else if(left.x > 15 && left.y > 10 && left.y < height-10){
    for(std::vector<LocalizedObject::Point>::iterator it = label.begin(); it != label.end(); it++){
      m->setValue(it->x + left.x - 14, it->y + left.y - 4,1);
    }
  }
  else if(right.x < width-15 && right.y > 10 && right.y < height-10){
    for(std::vector<LocalizedObject::Point>::iterator it = label.begin(); it != label.end(); it++){
      m->setValue(it->x + right.x + 3, it->y + left.y - 4,1);
    }
  }
  else if(bottom.y < height-15 && bottom.x > 10 && bottom.x < width-10){
    for(std::vector<LocalizedObject::Point>::iterator it = label.begin(); it != label.end(); it++){
      m->setValue(it->x + top.x - 5, it->y + top.y + 3,1);
    }
  }
  return m;
}

std::vector<LocalizedObject::Point> Segment::getLabel(int regNum)
{
  std::vector<LocalizedObject::Point> retVec;
  retVec.push_back(LocalizedObject::Point(4,2));
  retVec.push_back(LocalizedObject::Point(4,1));
  retVec.push_back(LocalizedObject::Point(3,0));
  retVec.push_back(LocalizedObject::Point(2,0));
  retVec.push_back(LocalizedObject::Point(1,0));
  retVec.push_back(LocalizedObject::Point(0,1));
  retVec.push_back(LocalizedObject::Point(0,2));
  retVec.push_back(LocalizedObject::Point(1,3));
  retVec.push_back(LocalizedObject::Point(2,4));
  retVec.push_back(LocalizedObject::Point(3,5));
  retVec.push_back(LocalizedObject::Point(4,6));
  retVec.push_back(LocalizedObject::Point(4,7));
  retVec.push_back(LocalizedObject::Point(3,8));
  retVec.push_back(LocalizedObject::Point(2,8));
  retVec.push_back(LocalizedObject::Point(1,8));
  retVec.push_back(LocalizedObject::Point(0,7));
  retVec.push_back(LocalizedObject::Point(0,6));
  if(regNum == 0){
    retVec.push_back(LocalizedObject::Point(8,0));
    retVec.push_back(LocalizedObject::Point(7,1));
    retVec.push_back(LocalizedObject::Point(9,1));
    for(int i = 2; i < 7; i++){
      retVec.push_back(LocalizedObject::Point(6,i));
      retVec.push_back(LocalizedObject::Point(10,i));
    }
    retVec.push_back(LocalizedObject::Point(7,7));
    retVec.push_back(LocalizedObject::Point(9,7));
    retVec.push_back(LocalizedObject::Point(8,8));
  }
  else if(regNum == 1){
    retVec.push_back(LocalizedObject::Point(6,2));
    retVec.push_back(LocalizedObject::Point(7,1));
    for(int i = 0; i < 9; i++) retVec.push_back(LocalizedObject::Point(8,i));
    for(int i = 6; i < 11; i++) retVec.push_back(LocalizedObject::Point(i,8));
  }
  else if(regNum == 2){
    retVec.push_back(LocalizedObject::Point(8,0));
    retVec.push_back(LocalizedObject::Point(7,1));
    retVec.push_back(LocalizedObject::Point(9,1));
    for(int i = 2; i < 4; i++){
      retVec.push_back(LocalizedObject::Point(6,i));
      retVec.push_back(LocalizedObject::Point(10,i));
    }
    retVec.push_back(LocalizedObject::Point(9,4));
    retVec.push_back(LocalizedObject::Point(8,5));
    retVec.push_back(LocalizedObject::Point(7,6));
    retVec.push_back(LocalizedObject::Point(6,7));
    for(int i = 6; i < 11; i++) retVec.push_back(LocalizedObject::Point(i,8));
  }
  else if(regNum == 3){
    retVec.push_back(LocalizedObject::Point(6,2));
    retVec.push_back(LocalizedObject::Point(7,1));
    retVec.push_back(LocalizedObject::Point(8,0));
    retVec.push_back(LocalizedObject::Point(9,1));
    retVec.push_back(LocalizedObject::Point(10,2));
    retVec.push_back(LocalizedObject::Point(10,3));
    retVec.push_back(LocalizedObject::Point(9,4));
    retVec.push_back(LocalizedObject::Point(10,5));
    retVec.push_back(LocalizedObject::Point(10,6));
    retVec.push_back(LocalizedObject::Point(9,7));
    retVec.push_back(LocalizedObject::Point(8,8));
    retVec.push_back(LocalizedObject::Point(7,7));
    retVec.push_back(LocalizedObject::Point(6,6));
  }
  else if(regNum == 4){
    for(int i = 0; i < 9; i++) retVec.push_back(LocalizedObject::Point(10,i));
    for(int i = 6; i < 11; i++) retVec.push_back(LocalizedObject::Point(i,4));
    for(int i = 0; i < 4; i++) retVec.push_back(LocalizedObject::Point(6,i));
  }
  else if(regNum == 5){
    retVec.push_back(LocalizedObject::Point(6,6));
    retVec.push_back(LocalizedObject::Point(6,7));
    retVec.push_back(LocalizedObject::Point(7,8));
    retVec.push_back(LocalizedObject::Point(8,8));
    retVec.push_back(LocalizedObject::Point(9,8));
    retVec.push_back(LocalizedObject::Point(10,7));
    retVec.push_back(LocalizedObject::Point(10,6));
    retVec.push_back(LocalizedObject::Point(10,5));
    for(int i = 7; i < 11; i++) retVec.push_back(LocalizedObject::Point(i,4));
    for(int i = 0; i < 4; i++) retVec.push_back(LocalizedObject::Point(6,i));
    for(int i = 7; i < 11; i++) retVec.push_back(LocalizedObject::Point(i,0));
    
  }
  else if(regNum == 6){
    retVec.push_back(LocalizedObject::Point(8,0));
    retVec.push_back(LocalizedObject::Point(7,0));
    retVec.push_back(LocalizedObject::Point(9,0));
    retVec.push_back(LocalizedObject::Point(10,1));
    retVec.push_back(LocalizedObject::Point(10,2));
    for(int i = 1; i < 8; i++) retVec.push_back(LocalizedObject::Point(6,i));
    for(int i = 7; i < 10; i++) retVec.push_back(LocalizedObject::Point(i,8));
    for(int i = 5; i < 8; i++) retVec.push_back(LocalizedObject::Point(10,i));
    for(int i = 7; i < 10; i++) retVec.push_back(LocalizedObject::Point(i,4));
  }
  else if(regNum == 7){
    retVec.push_back(LocalizedObject::Point(6,1));
    for(int i = 6; i < 11; i++) retVec.push_back(LocalizedObject::Point(i,0));
    retVec.push_back(LocalizedObject::Point(9,2));
    retVec.push_back(LocalizedObject::Point(10,1));
    retVec.push_back(LocalizedObject::Point(10,2));
    retVec.push_back(LocalizedObject::Point(8,4));
    retVec.push_back(LocalizedObject::Point(7,6));
    retVec.push_back(LocalizedObject::Point(7,8));
    retVec.push_back(LocalizedObject::Point(9,3));
    retVec.push_back(LocalizedObject::Point(8,5));
    retVec.push_back(LocalizedObject::Point(7,7));
  }
  else if(regNum == 8){
    retVec.push_back(LocalizedObject::Point(6,3));
    retVec.push_back(LocalizedObject::Point(6,2));
    retVec.push_back(LocalizedObject::Point(6,1));
    retVec.push_back(LocalizedObject::Point(7,0));
    retVec.push_back(LocalizedObject::Point(8,0));
    retVec.push_back(LocalizedObject::Point(9,0));
    retVec.push_back(LocalizedObject::Point(10,1));
    retVec.push_back(LocalizedObject::Point(10,2));
    retVec.push_back(LocalizedObject::Point(10,3));
    retVec.push_back(LocalizedObject::Point(9,4));
    retVec.push_back(LocalizedObject::Point(10,5));
    retVec.push_back(LocalizedObject::Point(10,6));
    retVec.push_back(LocalizedObject::Point(10,7));
    retVec.push_back(LocalizedObject::Point(9,8));
    retVec.push_back(LocalizedObject::Point(8,8));
    retVec.push_back(LocalizedObject::Point(7,8));
    retVec.push_back(LocalizedObject::Point(6,7));
    retVec.push_back(LocalizedObject::Point(6,6));
    retVec.push_back(LocalizedObject::Point(6,5));
    retVec.push_back(LocalizedObject::Point(7,4));
    retVec.push_back(LocalizedObject::Point(8,4));
  }
  else if(regNum == 9){
    for(int i = 1; i < 8; i++) retVec.push_back(LocalizedObject::Point(10,i));
    for(int i = 7; i < 10; i++) retVec.push_back(LocalizedObject::Point(i,0));
    for(int i = 7; i < 10; i++) retVec.push_back(LocalizedObject::Point(i,4));
    for(int i = 1; i < 4; i++) retVec.push_back(LocalizedObject::Point(6,i));
    for(int i = 7; i < 10; i++) retVec.push_back(LocalizedObject::Point(i,8));
    retVec.push_back(LocalizedObject::Point(6,7));
    retVec.push_back(LocalizedObject::Point(6,6));
  }
  return retVec;
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
