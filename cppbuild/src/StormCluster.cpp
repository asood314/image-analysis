#include "StormCluster.hpp"

StormCluster::StormCluster()
{
  m_centerX = 0.0;
  m_centerY = 0.0;
  m_centerZ = 0.0;
  m_intensity = 0.0;
}

std::vector<StormCluster*> StormCluster::cluster(StormData* blinks)
{
  std::vector<StormCluster*> retVal;
  std::vector<StormData::Blink> border;
  int clusterID = 1;
  bool finished = false;
  retVal.push_back(new StormCluster());
  retVal[0]->addMolecule(blinks->molecule(0));
  blinks->setClusterID(0,0);
  border.push_back(blinks->molecule(0));
  std::cout << "Total number of molecules: " << blinks->nMolecules() << std::endl;
  double threshold = 0.0;
  for(int i = 0; i < blinks->nMolecules(); i++) threshold += blinks->molecule(i).intensity;
  threshold = (threshold / blinks->nMolecules()) * 20;
  while(!finished){
    int nborder = border.size();
    for(int i = 0; i < blinks->nMolecules(); i++){
      if(blinks->isClustered(i)) continue;
      StormData::Blink b = blinks->molecule(i);
      double minDist = 9999999.9;
      for(std::vector<StormData::Blink>::iterator bit = border.begin(); bit != border.end(); bit++){
	double xdiff = bit->x - b.x;
	double ydiff = bit->y - b.y;
	double zdiff = bit->z - b.z;
	double dist = sqrt(xdiff*xdiff + ydiff*ydiff + zdiff*zdiff);
	if(dist < 50.0){
	  retVal[bit->clusterID]->addMolecule(b);
	  blinks->setClusterID(i,bit->clusterID);
	  border.push_back(blinks->molecule(i));
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
  }
  for(int i = retVal.size()-1; i >= 0; i--){
    //if(retVal[i]->nMolecules() < 5) retVal.erase(retVal.begin()+i);
    //else retVal[i]->computeCenter();
    retVal[i]->computeCenter();
    if(retVal[i]->intensity() < threshold) retVal.erase(retVal.begin()+i);
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

double StormCluster::distanceTo(StormCluster* c)
{
  double xdiff = m_centerX - c->centerX();
  double ydiff = m_centerY - c->centerY();
  double zdiff = m_centerZ - c->centerZ();
  return sqrt(xdiff*xdiff + ydiff*ydiff + zdiff*zdiff);
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
      if(dist < 50.0) return true;
    }
  }
  return false;
}

void StormCluster::shiftXY(double shiftX, double shiftY)
{
  m_centerX += shiftX;
  m_centerY += shiftY;
  for(std::vector<StormData::Blink>::iterator mit = m_molecules.begin(); mit != m_molecules.end(); mit++){
    (*mit).x += shiftX;
    (*mit).y += shiftY;
  }
}
