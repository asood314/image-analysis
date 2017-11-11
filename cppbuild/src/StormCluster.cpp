#include "StormCluster.hpp"

StormCluster::StormCluster()
{
  m_centerX = 0.0;
  m_centerY = 0.0;
  m_centerZ = 0.0;
  m_intensity = 0.0;
  m_selected = false;
}

std::vector<StormCluster*> StormCluster::cluster(StormData* blinks, double bpc)
{
  std::vector<StormCluster*> retVal;
  std::vector<StormData::Blink> border;
  int clusterID = 1;
  bool finished = false;
  retVal.push_back(new StormCluster());
  retVal[0]->addMolecule(blinks->molecule(0));
  blinks->setClusterID(0,0);
  border.push_back(blinks->molecule(0));
  int nMol = blinks->nMolecules();
  std::cout << "Total number of molecules: " << nMol << std::endl;
  std::vector< std::vector<int> > nBlinks;
  nBlinks.assign(1000, std::vector<int>(1000,0));
  for(int i = 0; i < nMol; i++){
    StormData::Blink b = blinks->molecule(i);
    int x = b.x / 50;
    int y = b.y * 1.12 / 50;
    nBlinks[x][y]++;
  }
  int threshold1 = 50 + 10*(bpc - 0.0005)/0.0005;
  int threshold2 = 30;// + 15*(bpc - 0.0005)/0.0005;
  std::vector<int> bkgBlinks;
  while(!finished){
    int nborder = border.size();
    for(int i = 0; i < nMol; i++){
      if(blinks->isClustered(i)) continue;
      StormData::Blink b = blinks->molecule(i);
      int x = (b.x / 50) + 1;
      int y = (b.y * 1.12 / 50) + 1;
      int density = 0;
      for(int dx = x-1; dx < x+2; dx++){
	for(int dy = y-1; dy < y+2; dy++){
	  density += nBlinks[dx][dy];
	}
      }
      if(density < threshold1){
	bkgBlinks.push_back(i);
	continue;
      }
      double minDist = 9999999.9;
      for(std::vector<StormData::Blink>::iterator bit = border.begin(); bit != border.end(); bit++){
	double xdiff = bit->x - b.x;
	double ydiff = bit->y - b.y;
	double zdiff = bit->z - b.z;
	double dist = sqrt(xdiff*xdiff + ydiff*ydiff + zdiff*zdiff);
	if(dist < 100.0){
	  b.clusterID  = bit->clusterID;
	  retVal[bit->clusterID]->addMolecule(b);
	  blinks->setClusterID(i,bit->clusterID);
	  border.push_back(b);
	  minDist = dist;
	  break;
	}
	else if(dist < minDist) minDist = dist;
      }
      if(minDist > 2000.0){
	retVal.push_back(new StormCluster());
	retVal[clusterID]->addMolecule(b);
	blinks->setClusterID(i,clusterID);
	border.push_back(blinks->molecule(i));
	clusterID++;
      }
    }
    std::cout << "Current number of clusters: " << clusterID << std::endl;
    if(border.size() == 0) finished = true;
    for(int i = 0; i < nborder; i++) border.erase(border.begin());
    nMol -= bkgBlinks.size();
    for(int i = bkgBlinks.size()-1; i >= 0; i--){
      blinks->removeMolecule(bkgBlinks[i]);
      bkgBlinks.erase(bkgBlinks.begin()+i);
    }
  }
  for(int i = retVal.size()-1; i >= 0; i--){
    if(retVal[i]->nMolecules() < threshold2){
      delete retVal[i];
      retVal.erase(retVal.begin()+i);
    }
    else{
      retVal[i]->computeCenter();
    }
  }

  return retVal;
}

void StormCluster::addMolecule(StormData::Blink b)
{
  m_molecules.push_back(b);
}

void StormCluster::computeCenter()
{
  m_centerX = 0.0;
  m_centerY = 0.0;
  m_centerZ = 0.0;
  m_intensity = 0.0;
  for(std::vector<StormData::Blink>::iterator mit = m_molecules.begin(); mit != m_molecules.end(); mit++){
    m_centerX += mit->intensity * mit->x;
    m_centerY += mit->intensity * mit->y;
    m_centerZ += mit->intensity * mit->z;
    m_intensity += mit->intensity;
  }
  m_centerX = m_centerX / m_intensity;
  m_centerY = m_centerY / m_intensity;
  m_centerZ = m_centerZ / m_intensity;
}

double StormCluster::estimateVolume()
{
  double minX = 999999;
  double maxX = 0;
  double minY = 999999;
  double maxY = 0;
  double minZ = 999999;
  double maxZ = -999999;
  for(std::vector<StormData::Blink>::iterator mit = m_molecules.begin(); mit != m_molecules.end(); mit++){
    if(mit->x > maxX) maxX = mit->x;
    if(mit->x < minX) minX = mit->x;
    if(mit->y > maxY) maxY = mit->y;
    if(mit->y < minY) minY = mit->y;
    if(mit->z > maxZ) maxZ = mit->z;
    if(mit->z < minZ) minZ = mit->z;
  }
  return (3.14159265351/6)*(maxX - minX)*(maxY - minY)*(maxZ - minZ);
}

double StormCluster::distanceTo(StormCluster* c)
{
  double xdiff = m_centerX - c->centerX();
  double ydiff = m_centerY - c->centerY();
  double zdiff = m_centerZ - c->centerZ();
  return sqrt(xdiff*xdiff + ydiff*ydiff + zdiff*zdiff);
}

double StormCluster::distanceTo(StormData::Blink& b)
{
  double minDist = 9999999.9;
  for(std::vector<StormData::Blink>::iterator mit = m_molecules.begin(); mit != m_molecules.end(); mit++){
    double xdiff = mit->x - b.x;
    double ydiff = mit->y - b.y;
    double zdiff = mit->z - b.z;
    double dist = sqrt(xdiff*xdiff + ydiff*ydiff + zdiff*zdiff);
    if(dist < minDist) minDist = dist;
  }
  return minDist;
}

bool StormCluster::colocalWith(StormCluster* c)
{
  if(distanceTo(c) > 1000.0) return false;
  for(int i = 0; i < c->nMolecules(); i++){
    StormData::Blink b = c->molecule(i);
    for(std::vector<StormData::Blink>::iterator mit = m_molecules.begin(); mit != m_molecules.end(); mit++){
      double xdiff = mit->x - b.x;
      double ydiff = mit->y - b.y;
      double zdiff = mit->z - b.z;
      double dist = sqrt(xdiff*xdiff + ydiff*ydiff + zdiff*zdiff);
      if(dist < 100.0) return true;
    }
  }
  return false;
}

void StormCluster::shiftXY(double shiftX, double shiftY, double scale)
{
  m_centerX += shiftX;
  m_centerY = m_centerY*scale + shiftY;
  for(std::vector<StormData::Blink>::iterator mit = m_molecules.begin(); mit != m_molecules.end(); mit++){
    (*mit).x += shiftX;
    (*mit).y = mit->y * scale + shiftY;
  }
}

void StormCluster::write(char* buf, std::ofstream& fout)
{
  int nmol = m_molecules.size();
  NiaUtils::writeShortToBuffer(buf,0,nmol);
  NiaUtils::writeDoubleToBuffer(buf,2,m_centerX);
  NiaUtils::writeDoubleToBuffer(buf,6,m_centerY);
  NiaUtils::writeDoubleToBuffer(buf,10,m_centerZ);
  NiaUtils::writeDoubleToBuffer(buf,14,m_intensity);
  int offset = 18;
  for(int i = 0; i < nmol; i++){
    StormData::Blink b = m_molecules[i];
    NiaUtils::writeDoubleToBuffer(buf,offset,b.x);
    offset += 4;
    NiaUtils::writeDoubleToBuffer(buf,offset,b.y);
    offset += 4;
    NiaUtils::writeDoubleToBuffer(buf,offset,b.z);
    offset += 4;
    NiaUtils::writeDoubleToBuffer(buf,offset,b.intensity);
    offset += 4;
  }
  fout.write(buf,offset);
}

void StormCluster::read(char* buf, std::ifstream& fin, int version)
{
  fin.read(buf,18);
  int nmol = NiaUtils::convertToShort(buf[0],buf[1]);
  m_centerX = NiaUtils::convertToDouble(buf[2],buf[3],buf[4],buf[5]);
  m_centerY = NiaUtils::convertToDouble(buf[6],buf[7],buf[8],buf[9]);
  m_centerZ = NiaUtils::convertToDouble(buf[10],buf[11],buf[12],buf[13]);
  m_intensity = NiaUtils::convertToDouble(buf[14],buf[15],buf[16],buf[17]);
  fin.read(buf,16*nmol);
  StormData::Blink b;
  int offset = 0;
  for(int i = 0; i < nmol; i++){
    b.x = NiaUtils::convertToDouble(buf[offset],buf[offset+1],buf[offset+2],buf[offset+3]);
    b.y = NiaUtils::convertToDouble(buf[offset+4],buf[offset+5],buf[offset+6],buf[offset+7]);
    b.z = NiaUtils::convertToDouble(buf[offset+8],buf[offset+9],buf[offset+10],buf[offset+11]);
    b.intensity = NiaUtils::convertToDouble(buf[offset+12],buf[offset+13],buf[offset+14],buf[offset+15]);
    m_molecules.push_back(b);
    offset += 16;
  }
}
