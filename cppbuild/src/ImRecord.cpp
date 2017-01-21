#include "ImRecord.hpp"

ImRecord::ImRecord(int nchan, int w, int h)
{
  m_nchannels = nchan;
  m_imWidth = w;
  m_imHeight = h;
  m_resolutionXY = 0.046;
  for(int i = 0; i < m_nchannels; i++){
    m_channelNames.push_back("");
    m_thresholds.push_back(0);
    m_signalMasks.push_back(NULL);
    m_utilityMasks.push_back(NULL);
    m_puncta.push_back(std::vector<Cluster*>());
  }
}

ImRecord::~ImRecord()
{
  for(std::vector<Mask*>::iterator it = m_signalMasks.begin(); it != m_signalMasks.end(); it++) delete *it;
  for(std::vector<Mask*>::iterator it = m_utilityMasks.begin(); it != m_utilityMasks.end(); it++) delete *it;
  for(std::vector< std::vector<Cluster*> >::iterator it = m_puncta.begin(); it != m_puncta.end(); it++){
    for(std::vector<Cluster*>::iterator jt = it->begin(); jt != it->end(); jt++) delete *jt;
  }
  for(std::vector<SynapseCollection*>::iterator it = m_synapseCollections.begin(); it != m_synapseCollections.end(); it++) delete *it;
  for(std::vector<Region*>::iterator it = m_regions.begin(); it != m_regions.end(); it++) delete *it;
}

void ImRecord::removePunctum(int chan, int index)
{
  std::vector<Cluster*> clusters = m_puncta.at(chan);
  delete clusters.at(index);
  m_puncta[chan].erase(m_puncta[chan].begin()+index);
}

void ImRecord::clearPuncta(int chan)
{
    std::vector<Cluster*> clusters = m_puncta.at(chan);
    for(std::vector<Cluster*>::iterator it = clusters.begin(); it != clusters.end(); it++) delete *it;
    m_puncta[chan].clear();
}

void ImRecord::removeSynapseCollection(int index)
{
  delete m_synapseCollections.at(index);
  m_synapseCollections.erase(m_synapseCollections.begin()+index);
}

void ImRecord::clearSynapseCollections()
{
  for(std::vector<SynapseCollection*>::iterator it = m_synapseCollections.begin(); it != m_synapseCollections.end(); it++) delete *it;
  m_synapseCollections.clear();
}

void ImRecord::removeRegion(int index)
{
  if(m_regions.at(index)) delete m_regions.at(index);
  m_regions.erase(m_regions.begin()+index);
}

void ImRecord::clearRegions()
{
  for(std::vector<Region*>::iterator it = m_regions.begin(); it != m_regions.end(); it++){
    if(*it) delete *it;
  }
  m_regions.clear();
}

Segment* ImRecord::selectSegment(LocalizedObject::Point pt)
{
  double minDist = 999999.9;
  Segment* closest = NULL;
  for(std::vector<Segment*>::iterator it = m_segments.begin(); it != m_segments.end(); it++){
    if((*it)->cluster()->contains(pt)) return *it;
    double dist = (*it)->cluster()->distanceTo(pt);
    if(dist < minDist){
      minDist = dist;
      closest = *it;
    }
  }
  return closest;
}

Cluster* ImRecord::selectPunctum(LocalizedObject::Point pt)
{
  double minDist = 999999.9;
  Cluster* closest = NULL;
  for(std::vector< std::vector<Cluster*> >::iterator it = m_puncta.begin(); it != m_puncta.end(); it++){
    for(std::vector<Cluster*>::iterator jt = it->begin(); jt != it->end(); jt++){
      double dist = (*jt)->distanceTo(pt);
      if(dist < minDist){
	minDist = dist;
	closest = *jt;
      }
    }
  }
  return closest;
}

Cluster* ImRecord::selectPunctum(int chan, LocalizedObject::Point pt)
{
  double minDist = 999999.9;
  Cluster* closest = NULL;
  std::vector<Cluster*> clusters = m_puncta.at(chan);
  for(std::vector<Cluster*>::iterator jt = clusters.begin(); jt != clusters.end(); jt++){
    double dist = (*jt)->distanceTo(pt);
    if(dist < minDist){
      minDist = dist;
      closest = *jt;
    }
  }
  return closest;
}

Synapse* ImRecord::selectSynapse(LocalizedObject::Point pt)
{
  double minDist = 999999.9;
  Synapse* closest = NULL;
  for(std::vector<SynapseCollection*>::iterator it = m_synapseCollections.begin(); it != m_synapseCollections.end(); it++){
    int n = (*it)->nSynapses();
    for(int i = 0; i < n; i++){
      Synapse* s = (*it)->getSynapse(i);
      double dist = s->distanceTo(pt);
      if(dist < minDist){
	minDist = dist;
	closest = s;
      }
    }
  }
  return closest;
}

Synapse* ImRecord::selectSynapseFromCollection(int index, LocalizedObject::Point pt)
{
  double minDist = 999999.9;
  Synapse* closest = NULL;
  SynapseCollection* sc = m_synapseCollections.at(index);
  int n = sc->nSynapses();
  for(int i = 0; i < n; i++){
    Synapse* s = sc->getSynapse(i);
    double dist = s->distanceTo(pt);
    if(dist < minDist){
      minDist = dist;
      closest = s;
    }
  }
  return closest;
}

Mask* ImRecord::getContourMap(int chan)
{
  bool finished = false;
  Mask* oldMask = m_signalMasks[chan]->getCopy();
  Mask* newMask = oldMask->getCopy();
  int count = 1;
  while(!finished){
    finished = true;
    for(int i = 0; i < m_imWidth; i++){
      for(int j = 0; j < m_imHeight; j++){
	if(oldMask->getValue(i,j) < count) continue;
	finished = false;
	int sum = 0;
	for(int i2 = i-1; i2 < i+2; i2++){
	  if(i2 < 0 || i2 >= m_imWidth){
	    sum += 3*count;
	    continue;
	  }
	  for(int j2 = j-1; j2 < j+2; j2++){
	    if(j2 < 0 || j2 >= m_imHeight){
	      sum += count;
	      continue;
	    }
	    sum += oldMask->getValue(i2,j2);
	  }
	}
	newMask->setValue(i,j,count + sum/(9*count));
      }
    }
    delete oldMask;
    oldMask = newMask->getCopy();
    count++;
  }
  delete oldMask;
  return newMask;
}

Mask* ImRecord::segment3(int chan)
{
  Mask* contourMask = getContourMap(chan);
  Mask* segmentMask = new Mask(m_imWidth,m_imHeight,0);
  Mask* used = new Mask(m_imWidth,m_imHeight);
  std::vector< std::vector<double> > vecs;
  vecs.assign(m_imWidth,std::vector<double>(m_imHeight,0.0));
  std::vector<int> xvec,yvec;
  for(int i = 0; i < m_imWidth; i++){
    for(int j = 0; j < m_imHeight; j++){
      int level = contourMask->getValue(i,j);
      if(level < 1){
	vecs[i][j] = -999;
	continue;
      }
      int cx = 0;
      int cy = 0;
      int n = 0;
      std::vector<int> borderX;
      std::vector<int> borderY;
      borderX.push_back(i);
      borderY.push_back(j);
      used->setValue(i,j,1);
      while(borderX.size() > 0 && n < 50){
	uint32_t nborder = borderX.size();
	for(uint32_t b = 0; b < nborder; b++){
	  int bi = borderX[0];
	  int bj = borderY[0];
	  int left = bi-1;
	  int right = bi+2;
	  int top = bj-1;
	  int bottom = bj+2;
	  if(left < 0) left++;
	  if(right > m_imWidth) right--;
	  if(top < 0) top++;
	  if(bottom > m_imHeight) bottom--;
	  cx += bi;
	  cy += bj;
	  n++;
	  xvec.push_back(bi);
	  yvec.push_back(bj);
	  for(int di = left; di < right; di++){
	    for(int dj = top; dj < bottom; dj++){
	      if(contourMask->getValue(di,dj) != level || used->getValue(di,dj) > 0) continue;
	      borderX.push_back(di);
	      borderY.push_back(dj);
	      used->setValue(di,dj,1);
	    }
	  }
	  borderX.erase(borderX.begin());
	  borderY.erase(borderY.begin());
	}
      }
      for(unsigned index = 0; index < borderX.size(); index++) used->setValue(borderX[index],borderY[index],0);

      if(n < 25){
	vecs[i][j] = -999;
	used->setValue(i,j,0);
	for(unsigned index = 0; index < n; index++) used->setValue(xvec[index],yvec[index],0);
	xvec.clear();
	yvec.clear();
	continue;
      }
      cx /= n;
      cy /= n;
      double vxx = 0.0;
      double vyy = 0.0;
      double vxy = 0.0;
      for(unsigned index = 0; index < n; index++){
	int xdiff = xvec[index] - cx;
	int ydiff = yvec[index] - cy;
	vxx += xdiff*xdiff;
	vyy += ydiff*ydiff;
	vxy += xdiff*ydiff;
	used->setValue(xvec[index],yvec[index],0);
      }
      double det = sqrt((vxx - vyy)*(vxx-vyy) + 4*vxy*vxy);
      double eigenval = (vxx + vyy + det) / 2.0;
      double eigenvec = atan((eigenval - vxx) / vxy);
      //double eigenval2 = (vxx + vyy - det) / 2.0;
      vecs[i][j] = eigenvec;
      xvec.clear();
      yvec.clear();
    }
  }

  int segmentID = 1;
  std::vector<Cluster*> clusters;
  for(int i = 0; i < m_imWidth; i++){
    for(int j = 0; j < m_imHeight; j++){
      if(vecs[i][j] < -900 || used->getValue(i,j) > 0) continue;
      Cluster* c  = new Cluster();
      double val = vecs[i][j];
      std::vector<int> borderX;
      std::vector<int> borderY;
      borderX.push_back(i);
      borderY.push_back(j);
      used->setValue(i,j,1);
      while(borderX.size() > 0){
	uint32_t nborder = borderX.size();
	for(uint32_t b = 0; b < nborder; b++){
	  int bi = borderX[0];
	  int bj = borderY[0];
	  int left = bi-1;
	  int right = bi+2;
	  int top = bj-1;
	  int bottom = bj+2;
	  if(left < 0) left++;
	  if(right > m_imWidth) right--;
	  if(top < 0) top++;
	  if(bottom > m_imHeight) bottom--;
	  segmentMask->setValue(bi,bj,segmentID);
	  c->addPoint(bi,bj);
	  for(int di = left; di < right; di++){
	    for(int dj = top; dj < bottom; dj++){
	      if(vecs[di][dj] < -900 || used->getValue(di,dj) > 0) continue;
	      double angle = fabs(vecs[di][dj] - val);
	      if(angle > 1.5708) angle = 3.1416 - angle;
	      if(angle > 0.2) continue;
	      borderX.push_back(di);
	      borderY.push_back(dj);
	      used->setValue(di,dj,1);
	    }
	  }
	  borderX.erase(borderX.begin());
	  borderY.erase(borderY.begin());
	}
      }
      segmentID++;
      clusters.push_back(c);
    }
  }
  std::cout << "Done initial clustering" << std::endl;
  bool finished = false;
  int di[8] = {-1,-1,-1,0,0,1,1,1};
  int dj[8] = {-1,0,1,-1,1,-1,0,1};
  while(!finished){
    finished = true;
    for(int i = 0; i < m_imWidth; i++){
      for(int j = 0; j < m_imHeight; j++){
	if(contourMask->getValue(i,j) < 1 || segmentMask->getValue(i,j) > 0) continue;
	for(int k = 0; k < 8; k++){
	  int i2 = i+di[k];
	  int j2 = j+dj[k];
	  if(i2 < 0 || i2 >= m_imWidth) continue;
	  if(j2 < 0 || j2 >= m_imHeight) continue;
	  int segVal = segmentMask->getValue(i2,j2);
	  if(segVal > 0){
	    segmentMask->setValue(i,j,segVal);
	    clusters[segVal-1]->addPoint(i,j);
	    finished = false;
	    break;
	  }
	}
      }
    }
  }
  std::cout << "Done adding peaks" << std::endl;
  for(std::vector<Cluster*>::iterator clit = clusters.begin(); clit != clusters.end(); clit++){
    (*clit)->computeCenter();
    (*clit)->findBorder();
  }
  
  for(std::vector<Cluster*>::iterator clit = clusters.begin(); clit != clusters.end(); clit++){
    if(!(*clit)) continue;
    int size1 = (*clit)->size();
    if(size1 > 400) continue;
    LocalizedObject::Point cent1 = (*clit)->center();
    finished = false;
    while(!finished){
      finished = true;
      int maxBorder = 0;
      std::vector<Cluster*>::iterator minClust = clusters.end();
      for(std::vector<Cluster*>::iterator cljt = clusters.begin(); cljt != clusters.end(); cljt++){
	if(!(*cljt)) continue;
	if(cljt == clit) continue;
	int size2 = (*cljt)->size();
	LocalizedObject::Point cent2 = (*cljt)->center();
	float dist = sqrt((cent1.x - cent2.x)*(cent1.x - cent2.x) + (cent1.y - cent2.y)*(cent1.y - cent2.y));
	if(dist > (size1+size2)/2) continue;
	int border = (*clit)->getBorderLength(*cljt);
	if(border > maxBorder){
	  maxBorder = border;
	  minClust  = cljt;
	}
      }
      if(minClust != clusters.end()){
	(*clit)->add(*minClust);
	(*clit)->computeCenter();
	(*clit)->findBorder();
	size1 += (*minClust)->size();
	if(size1 < 400) finished = false;
	delete *minClust;
	*minClust = NULL;
      }
    }
  }
  std::cout << "Done merging" << std::endl;
  
  segmentID = 1;
  segmentMask->clear(0,m_imWidth,0,m_imHeight);
  std::vector<Segment*> segments;
  for(std::vector<Cluster*>::iterator clit = clusters.begin(); clit != clusters.end(); clit++){
    if(!(*clit)) continue;
    for(std::vector<LocalizedObject::Point>::iterator pit = (*clit)->begin(); pit != (*clit)->end(); pit++) segmentMask->setValue(pit->x,pit->y,segmentID);
    Segment* s = new Segment(*clit);
    s->findOrientation();
    segments.push_back(s);
    segmentID++;
  }

  int sizeLimit = 4000;
  int nChanges = 1;
  while(sizeLimit < 100000){
    if(nChanges > 0){
      for(int i = segments.size(); i >= 0; i--){
	if(!segments[i]) segments.erase(segments.begin()+i);
      }
      for(unsigned i = 0; i < segments.size(); i++){
	for(unsigned j = i+1; j < segments.size(); j++){
	  if(segments[i]->size() > segments[j]->size()){
	    Segment* tmp = segments[i];
	    segments[i] = segments[j];
	    segments[j] = tmp;
	  }
	}
      }
    }
    else sizeLimit *= 2;
    
    nChanges = 0;
    for(std::vector<Segment*>::iterator sit = segments.begin(); sit != segments.end(); sit++){
      if(!(*sit)) continue;
      int size1 = (*sit)->size();
      if(size1 > sizeLimit) continue;
      double circ1 = (*sit)->circularity();
      //if((*sit)->circularity() > 0.4) continue;
      double vec1 = (*sit)->eigenVector1();
      LocalizedObject::Point cent1 = (*sit)->cluster()->center();
      int perimeter = (*sit)->cluster()->perimeter();
      double minAngle = 1.6;
      int maxBorder = perimeter / 5;
      int totalBorder = 0;
      double maxCirc = 0.0;
      double maxAngle = 1.6;
      std::vector<Segment*>::iterator minSeg = segments.end();
      std::vector<Segment*>::iterator maxSeg = segments.end();
      for(std::vector<Segment*>::iterator sjt = segments.begin(); sjt != segments.end(); sjt++){
	if(!(*sjt)) continue;
	if(sjt == sit) continue;
	double vec2 = (*sjt)->eigenVector1();
	double angle = fabs(vec2 - vec1);
	if(angle > 1.5708) angle = 3.1416 - angle;
	int size2 = (*sjt)->size();
	LocalizedObject::Point cent2 = (*sjt)->cluster()->center();
	float dist = sqrt((cent1.x - cent2.x)*(cent1.x - cent2.x) + (cent1.y - cent2.y)*(cent1.y - cent2.y));
	if(dist > (size1+size2)/2) continue;
	int border = (*sit)->cluster()->getBorderLength((*sjt)->cluster());
	totalBorder += border;
	//if((*sjt)->size() > size1) continue;
	if(border > maxBorder){
	  maxBorder = border;
	  maxSeg = sjt;
	  maxCirc = (*sjt)->circularity();
	  maxAngle = angle;
	}
	if(angle > minAngle) continue;
	if(border > 0 && (angle < 0.5 || perimeter / 3)){
	  minAngle = angle;
	  minSeg  = sjt;
	  //minBorder = border;
	}
      }
      if(totalBorder > perimeter / 4 && maxAngle < 1.6*exp(circ1 + maxCirc - 2)){
	if(maxSeg != segments.end()){
	  (*sit)->merge(*maxSeg);
	  delete *maxSeg;
	  *maxSeg = NULL;
	  nChanges++;
	}
      }
      /*
	else{
	if(minSeg != segments.end()){
	(*sit)->merge(*minSeg);
	delete *minSeg;
	*minSeg = NULL;
	}
	}
      */
    }
  }
  std::cout << "Done merging 2" << std::endl;
  
  nChanges = 1;
  while(nChanges > 0){
    for(int i = segments.size(); i >= 0; i--){
      if(!segments[i]) segments.erase(segments.begin()+i);
    }
    for(unsigned i = 0; i < segments.size(); i++){
      for(unsigned j = i+1; j < segments.size(); j++){
	if(segments[i]->size() > segments[j]->size()){
	  Segment* tmp = segments[i];
	  segments[i] = segments[j];
	  segments[j] = tmp;
	}
      }
    }
    
    nChanges = 0;
    for(std::vector<Segment*>::iterator sit = segments.begin(); sit != segments.end(); sit++){
      if(!(*sit)) continue;
      int size1 = (*sit)->size();
      LocalizedObject::Point cent1 = (*sit)->cluster()->center();
      int perimeter = (*sit)->cluster()->perimeter();
      int maxBorder = perimeter / 3;
      std::vector<Segment*>::iterator maxSeg = segments.end();
      for(std::vector<Segment*>::iterator sjt = segments.begin(); sjt != segments.end(); sjt++){
	if(!(*sjt)) continue;
	if(sjt == sit) continue;
	int size2 = (*sjt)->size();
	LocalizedObject::Point cent2 = (*sjt)->cluster()->center();
	float dist = sqrt((cent1.x - cent2.x)*(cent1.x - cent2.x) + (cent1.y - cent2.y)*(cent1.y - cent2.y));
	if(dist > (size1+size2)/2) continue;
	int border = (*sit)->cluster()->getBorderLength((*sjt)->cluster());
	if(border > maxBorder){
	  maxBorder = border;
	  maxSeg = sjt;
	}
      }
      if(maxSeg != segments.end()){
	(*sit)->merge(*maxSeg);
	delete *maxSeg;
	*maxSeg = NULL;
	nChanges++;
      }
    }
  }
  std::cout << "Done merging 3" << std::endl;

  for(std::vector<Segment*>::iterator sit = segments.begin(); sit != segments.end(); sit++){
    if(!(*sit)) continue;
    int size1 = (*sit)->size();
    //if(size1 > 4000) continue;
    LocalizedObject::Point cent1 = (*sit)->cluster()->center();
    int perimeter = (*sit)->cluster()->perimeter();
    finished = false;
    while(!finished){
      finished = true;
      int maxBorder = maxBorder = perimeter / 5;
      int totalBorder = 0;
      std::vector<Segment*>::iterator maxSeg = segments.end();
      for(std::vector<Segment*>::iterator sjt = segments.begin(); sjt != segments.end(); sjt++){
	if(!(*sjt)) continue;
	if(sjt == sit) continue;
	int size2 = (*sjt)->size();
	LocalizedObject::Point cent2 = (*sjt)->cluster()->center();
	float dist = sqrt((cent1.x - cent2.x)*(cent1.x - cent2.x) + (cent1.y - cent2.y)*(cent1.y - cent2.y));
	if(dist > (size1+size2)/2) continue;
	int border = (*sit)->cluster()->getBorderLength((*sjt)->cluster());
	totalBorder += border;
	if(border > maxBorder){
	  maxBorder = border;
	  maxSeg  = sjt;
	}
      }
      if(maxSeg != segments.end() && totalBorder > perimeter/2){
	if(size1 > (*maxSeg)->size() / 5 || totalBorder > 2*perimeter/3){
	  (*sit)->merge(*maxSeg);
	  size1 += (*maxSeg)->size();
	  perimeter = (*sit)->cluster()->perimeter();
	  finished = false;
	  delete *maxSeg;
	  *maxSeg = NULL;
	}
      }
    }
  }
  std::cout << "Done merging 4" << std::endl;

  nChanges = 1;
  while(nChanges > 0){
    for(int i = segments.size(); i >= 0; i--){
      if(!segments[i]) segments.erase(segments.begin()+i);
    }
    for(unsigned i = 0; i < segments.size(); i++){
      for(unsigned j = i+1; j < segments.size(); j++){
	if(segments[i]->size() > segments[j]->size()){
	  Segment* tmp = segments[i];
	  segments[i] = segments[j];
	  segments[j] = tmp;
	}
      }
    }
    
    nChanges = 0;
    for(std::vector<Segment*>::iterator sit = segments.begin(); sit != segments.end(); sit++){
      if(!(*sit)) continue;
      int size1 = (*sit)->size();
      double circ1 = (*sit)->circularity();
      if((*sit)->circularity() > 0.2) continue;
      double vec1 = (*sit)->eigenVector1();
      LocalizedObject::Point cent1 = (*sit)->cluster()->center();
      int perimeter = (*sit)->cluster()->perimeter();
      double minAngle = 0.5;
      int totalBorder = 0;
      double minCirc = 0.0;
      std::vector<Segment*>::iterator minSeg = segments.end();
      for(std::vector<Segment*>::iterator sjt = segments.begin(); sjt != segments.end(); sjt++){
	if(!(*sjt)) continue;
	if(sjt == sit) continue;
	double vec2 = (*sjt)->eigenVector1();
	double angle = fabs(vec2 - vec1);
	if(angle > 1.5708) angle = 3.1416 - angle;
	int size2 = (*sjt)->size();
	LocalizedObject::Point cent2 = (*sjt)->cluster()->center();
	float dist = sqrt((cent1.x - cent2.x)*(cent1.x - cent2.x) + (cent1.y - cent2.y)*(cent1.y - cent2.y));
	if(dist > (size1+size2)/2) continue;
	int border = (*sit)->cluster()->getBorderLength((*sjt)->cluster());
	totalBorder += border;
	if(size2 > size1 && (*sjt)->circularity() > circ1) continue;
	//if((*sjt)->circularity() > 0.2) continue;
	if(angle > minAngle) continue;
	if(border > 0){
	  minAngle = angle;
	  minSeg  = sjt;
	  minCirc = (*sjt)->circularity();
	}
      }
      if(totalBorder < perimeter / 3 && minAngle < 1.6*exp((circ1 + minCirc - 2)/2.0)){
	if(minSeg != segments.end()){
	  (*sit)->merge(*minSeg);
	  delete *minSeg;
	  *minSeg = NULL;
	  nChanges++;
	}
      }
    }
  }
  std::cout << "Done merging 5" << std::endl;
  /*
  for(std::vector<Segment*>::iterator sit = segments.begin(); sit != segments.end(); sit++){
    if(!(*sit)) continue;
    int size1 = (*sit)->size();
    if(size1 > 4000) continue;
    finished = false;
    while(!finished){
      finished = true;
      LocalizedObject::Point cent1 = (*sit)->cluster()->center();
      int totalBorder = 0;
      int maxBorder = 0;
      std::vector<Segment*>::iterator minSeg = segments.end();
      for(std::vector<Segment*>::iterator sjt = segments.begin(); sjt != segments.end(); sjt++){
	if(!(*sjt)) continue;
	if(sjt == sit) continue;
	int size2 = (*sjt)->size();
	LocalizedObject::Point cent2 = (*sjt)->cluster()->center();
	float dist = sqrt((cent1.x - cent2.x)*(cent1.x - cent2.x) + (cent1.y - cent2.y)*(cent1.y - cent2.y));
	if(dist > (size1+size2)/2) continue;
	int border = (*sit)->cluster()->getBorderLength((*sjt)->cluster());
	if(border > maxBorder){
	  maxBorder = border;
	  minSeg  = sjt;
	}
	totalBorder += border;
      }
      if(totalBorder > (*sit)->cluster()->perimeter()/2){
	(*sit)->merge(*minSeg);
	delete *minSeg;
	*minSeg = NULL;
	if((*sit)->size() < 4000) finished = false;
      }
    }
  }
  std::cout << "Done merging 3" << std::endl;
  */
  segmentID = 1;
  segmentMask->clear(0,m_imWidth,0,m_imHeight);
  for(std::vector<Segment*>::iterator sit = segments.begin(); sit != segments.end(); sit++){
    if(!(*sit)) continue;
    Cluster* c = (*sit)->cluster();
    for(std::vector<LocalizedObject::Point>::iterator pit = c->begin(); pit != c->end(); pit++) segmentMask->setValue(pit->x,pit->y,segmentID);
    segmentID++;
    m_segments.push_back(*sit);
  }
  
  delete contourMask;
  delete used;
  return segmentMask;
}

Mask* ImRecord::segment2(int chan)
{
  Mask* contourMask = getContourMap(chan);
  Mask* segmentMask = new Mask(m_imWidth,m_imHeight,0);
  for(std::vector<Segment*>::iterator sit = m_segments.begin(); sit != m_segments.end(); sit++){
    if(*sit) delete *sit;
  }
  m_segments.clear();

  std::vector<Segment*> segments;
  std::vector<Cluster*> clusters;
  Mask* subMask = contourMask->getCopy();
  for(int x = 0; x < m_imWidth; x++){
    for(int y = 0; y < m_imHeight; y++){
      int val = subMask->getValue(x,y);
      if(val < 1) continue;
      Cluster* c = new Cluster();
      std::vector<int> borderX;
      std::vector<int> borderY;
      borderX.push_back(x);
      borderY.push_back(y);
      subMask->setValue(x,y,0);
      while(borderX.size() > 0){
	uint32_t nborder = borderX.size();
	for(uint32_t b = 0; b < nborder; b++){
	  int bi = borderX.at(0);
	  int bj = borderY.at(0);
	  int left = bi-1;
	  int right = bi+2;
	  int top = bj-1;
	  int bottom = bj+2;
	  if(left < 0) left++;
	  if(right >= m_imWidth) right--;
	  if(top < 0) top++;
	  if(bottom >= m_imHeight) bottom--;
	  c->addPoint(bi,bj);
	  for(int di = left; di < right; di++){
	    for(int dj = top; dj < bottom; dj++){
	      if(subMask->getValue(di,dj) < 1) continue;
	      subMask->setValue(di,dj,0);
	      borderX.push_back(di);
	      borderY.push_back(dj);
	    }
	  }
	  borderX.erase(borderX.begin());
	  borderY.erase(borderY.begin());
	}
      }
      c->computeCenter();
      clusters.push_back(c);
    }
  }

  for(unsigned i = 0; i < clusters.size(); i++){
    for(unsigned j = i+1; j < clusters.size(); j++){
      if(clusters[j]->size() > clusters[i]->size()){
	Cluster* tmp = clusters[i];
	clusters[i] = clusters[j];
	clusters[j] = tmp;
      }
    }
  }
  
  int segmentID = 1;
  int di[8] = {-1,-1,-1,0,0,1,1,1};
  int dj[8] = {-1,0,1,-1,1,-1,0,1};
  std::vector<int> lmx,lmy,lmh;
  for(std::vector<Cluster*>::iterator clit = clusters.begin(); clit != clusters.end(); clit++){
    if((*clit)->size() < 500){
      //for(std::vector<LocalizedObject::Point>::iterator pt = (*clit)->begin(); pt != (*clit)->end(); pt++) segmentMask->setValue(pt->x,pt->y,segmentID);
      //segmentID++;
      //(*clit)->findBorder();
      Segment* newSegment = new Segment(*clit);
      segments.push_back(newSegment);
      continue;
    }
    for(std::vector<LocalizedObject::Point>::iterator pt = (*clit)->begin(); pt != (*clit)->end(); pt++){
      int val = contourMask->getValue(pt->x,pt->y);
      if(val == 0) continue;
      bool islocalMaximum = true;
      for(int k = 0; k < 8; k++){
	int i2 = pt->x+di[k];
	if(i2 < 0 || i2 >= m_imWidth) continue;
	int j2 = pt->y+dj[k];
	if(j2 < 0 || j2 >= m_imHeight) continue;
	if(contourMask->getValue(i2,j2) > val){
	  islocalMaximum = false;
	  break;
	}
      }
      if(islocalMaximum){
	lmx.push_back(pt->x);
	lmy.push_back(pt->y);
	lmh.push_back(val);
      }
    }
    std::cout << "Found " << lmh.size() << " local maxima in cluster of size " << (*clit)->size() << std::endl;
    for(unsigned i = 0; i < lmh.size(); i++){
      int max = lmh[i];
      int imax = i;
      for(unsigned j = i+1; j < lmh.size(); j++){
	if(lmh[j] > max){
	  max = lmh[j];
	  imax = j;
	}
      }
      lmh[imax] = lmh[i];
      lmh[i] = max;
      int temp = lmx[imax];
      lmx[imax] = lmx[i];
      lmx[i] = temp;
      temp = lmy[imax];
      lmy[imax] = lmy[i];
      lmy[i] = temp;
    }
    //std::cout << "Starting with maximum at (" << lmx[0] << "," << lmy[0] << ")" << std::endl;
    subMask->clear(0,m_imWidth,0,m_imHeight);
    for(unsigned i = 0; i < lmh.size(); i++){
      if(segmentMask->getValue(lmx[i],lmy[i]) > 0) continue;
      subMask->setValue(lmx[i],lmy[i],1);
      int size = 1;
      for(std::vector<LocalizedObject::Point>::iterator pt = (*clit)->begin(); pt != (*clit)->end(); pt++){
	if(contourMask->getValue(pt->x,pt->y) != 1) continue;
	if(segmentMask->getValue(pt->x,pt->y) > 0) continue;
	if(contourMask->isMinimallyConnected(lmx[i],lmy[i],pt->x,pt->y,false)){
	  subMask->setValue(pt->x,pt->y,1);
	  size++;
	}
      }
    
      int clusterThreshold = size/100;
      int newSize = size;
      std::vector<Cluster*> borderClusters;
      for(std::vector<LocalizedObject::Point>::iterator pt = (*clit)->begin(); pt != (*clit)->end(); pt++){
	int val = subMask->getValue(pt->x,pt->y);
	if(val != 1) continue;
	Cluster* c = new Cluster();
	std::vector<int> borderX;
	std::vector<int> borderY;
	borderX.push_back(pt->x);
	borderY.push_back(pt->y);
	subMask->setValue(pt->x,pt->y,2);
	while(borderX.size() > 0){
	  uint32_t nborder = borderX.size();
	  for(uint32_t b = 0; b < nborder; b++){
	    int bi = borderX.at(0);
	    int bj = borderY.at(0);
	    int left = bi-1;
	    int right = bi+2;
	    int top = bj-1;
	    int bottom = bj+2;
	    if(left < 0) left++;
	    if(right >= m_imWidth) right--;
	    if(top < 0) top++;
	    if(bottom >= m_imHeight) bottom--;
	    c->addPoint(bi,bj);
	    for(int di = left; di < right; di++){
	      for(int dj = top; dj < bottom; dj++){
		if(subMask->getValue(di,dj) != 1) continue;
		subMask->setValue(di,dj,2);
		borderX.push_back(di);
		borderY.push_back(dj);
	      }
	    }
	    borderX.erase(borderX.begin());
	    borderY.erase(borderY.begin());
	  }
	}
	if(c->size() < clusterThreshold){
	  std::vector<LocalizedObject::Point> pts = c->getPoints();
	  for(std::vector<LocalizedObject::Point>::iterator it = pts.begin(); it != pts.end(); it++) subMask->setValue(it->x,it->y,0);
	  newSize -= c->size();
	  delete c;
	}
	else{
	  c->computeCenter();
	  borderClusters.push_back(c);
	}
      }

      int offset = newSize/2;
      for(std::vector<Cluster*>::iterator it = borderClusters.begin(); it != borderClusters.end(); it++){
	LocalizedObject::Point p = (*it)->center();
	int left = p.x - offset;
	if(left < 0) left = 0;
	int right = p.x + offset;
	if(right > m_imWidth) right = m_imWidth;
	int top = p.y - offset;
	if(top < 0) top = 0;
	int bottom = p.y + offset;
	if(bottom > m_imHeight) bottom = m_imHeight;
	int sum = 0;
	for(int x = left; x < right; x++){
	  for(int y = top; y < bottom; y++) sum += subMask->getValue(x,y);
	}
	if(sum < newSize/8){
	  std::vector<LocalizedObject::Point> pts = (*it)->getPoints();
	  for(std::vector<LocalizedObject::Point>::iterator jt = pts.begin(); jt != pts.end(); jt++) subMask->setValue(jt->x,jt->y,0);
	}
	delete *it;
      }

      for(std::vector<LocalizedObject::Point>::iterator pt = (*clit)->begin(); pt != (*clit)->end(); pt++){
	if(subMask->getValue(pt->x,pt->y) < 2) continue;
	int distx = pt->x - lmx[i];
	int disty = pt->y - lmy[i];
	float dist = sqrt(distx*distx + disty*disty);
	for(std::vector<LocalizedObject::Point>::iterator pt2 = (*clit)->begin(); pt2 != (*clit)->end(); pt2++){
	  if(contourMask->getValue(pt2->x,pt2->y) == 0) continue;
	  if(segmentMask->getValue(pt2->x,pt2->y) > 0) continue;
	  if(subMask->getValue(pt2->x,pt2->y) > 0) continue;
	  distx = pt2->x - lmx[i];
	  disty = pt2->y - lmy[i];
	  if(sqrt(distx*distx + disty*disty) > dist) continue;
	  if(contourMask->isMinimallyConnected(pt2->x,pt2->y,pt->x,pt->y)) subMask->setValue(pt2->x,pt2->y,1);
	}
      }
    
      //int segmentSize = 1;
      //segmentMask->setValue(lmx[i],lmy[i],segmentID);
      Cluster* newSegmentCluster = new Cluster();
      for(std::vector<LocalizedObject::Point>::iterator pt = (*clit)->begin(); pt != (*clit)->end(); pt++){
	//if(contourMask->getValue(x,y) == 0) continue;
	//if(segmentMask->getValue(x,y) > 0) continue;
	if(subMask->getValue(pt->x,pt->y) > 0){//>= mode){
	  newSegmentCluster->addPoint(*pt);
	  segmentMask->setValue(pt->x,pt->y,segmentID);
	  contourMask->setValue(pt->x,pt->y,0);
	  //segmentSize++;
	}
      }
      //newSegment->findBorder();
      Segment* newSegment = new Segment(newSegmentCluster);
      //newSegment->findOrientation();
      segments.push_back(newSegment);
      //std::cout << "Segment " << segmentID << " has size " << segmentSize << std::endl;
      std::cout << "Finished potential segment " << segmentID << std::endl;
      segmentID++;
      subMask->clear(0,m_imWidth,0,m_imHeight);
    }
    delete *clit;
    lmx.clear();
    lmy.clear();
    lmh.clear();
  }

  bool finished = false;
  int minSegmentSize = (int)(0.2 / (m_resolutionXY*m_resolutionXY));
  while(!finished){
    finished = true;
    for(std::vector<Segment*>::iterator sit = segments.begin(); sit != segments.end(); sit++){
      if(!(*sit)) continue;
      for(std::vector<Segment*>::iterator sit2 = segments.begin(); sit2 != segments.end(); sit2++){
	if(!(*sit2)) continue;
	if(sit2 == sit) continue;
	if((*sit2)->cluster()->size() > minSegmentSize) continue;
	if((*sit2)->cluster()->getBorderLength((*sit)->cluster()) > 0){//(*sit2)->cluster()->perimeter()/3){
	  (*sit)->cluster()->add((*sit2)->cluster());
	  (*sit)->cluster()->findBorder();
	  delete *sit2;
	  *sit2 = NULL;
	  finished = false;
	}
      }
    }
  }

  segmentMask->clear(0,m_imWidth,0,m_imHeight);
  segmentID = 1;
  for(std::vector<Segment*>::iterator sit = segments.begin(); sit != segments.end(); sit++){
    if(!(*sit)) continue;
    (*sit)->cluster()->computeCenter();
    (*sit)->findOrientation();
    for(std::vector<LocalizedObject::Point>::iterator pt = (*sit)->cluster()->begin(); pt != (*sit)->cluster()->end(); pt++){
      segmentMask->setValue(pt->x,pt->y,segmentID);
    }
    m_segments.push_back(*sit);
    std::cout << "Segment " << segmentID << " has size " << (*sit)->cluster()->size() << std::endl;
    segmentID++;
  }
  
  delete subMask;
  delete contourMask;
  return segmentMask;
}

Mask* ImRecord::segment(int chan)
{
  Mask* contourMask = getContourMap(chan);
  Mask* nodeMask = new Mask(m_imWidth,m_imHeight);
  std::vector< std::vector< std::vector<LocalizedObject::Point> > >* trail = new std::vector< std::vector< std::vector<LocalizedObject::Point> > >();
  trail->assign(m_imWidth,std::vector< std::vector<LocalizedObject::Point> >());
  for(int i = 0; i < m_imWidth; i++) trail->at(i).assign(m_imHeight,std::vector<LocalizedObject::Point>());
  int di[8] = {-1,-1,-1,0,0,1,1,1};
  int dj[8] = {-1,0,1,-1,1,-1,0,1};
  std::cout << "Finding trails" << std::endl;
  for(int i = 0; i < m_imWidth; i++){
    for(int j = 0; j < m_imHeight; j++){
      if(nodeMask->getValue(i,j) > 0) continue;
      int base = contourMask->getValue(i,j);
      if(base < 1) continue;
      nodeMask->setValue(i,j,1);
      int max = base;
      std::vector<LocalizedObject::Point> steps;
      for(int k = 0; k < 8; k++){
	int i2 = i+di[k];
	if(i2 < 0 || i2 >= m_imWidth) continue;
	int j2 = j+dj[k];
	if(j2 < 0 || j2 >= m_imHeight) continue;
	int val = contourMask->getValue(i2,j2);
	if(val > max){
	  max = val;
	  steps.clear();
	  steps.push_back(LocalizedObject::Point(i2,j2));
	}
	else if(val == max) steps.push_back(LocalizedObject::Point(i2,j2));
      }
      if(steps.size() == 0){
	nodeMask->setValue(i,j,2);
	continue;
      }
      for(std::vector<LocalizedObject::Point>::iterator it = steps.begin(); it != steps.end(); it++){
	trail->at(it->x).at(it->y).push_back(LocalizedObject::Point(i,j));
	if(nodeMask->getValue(it->x,it->y) > 0) continue;
	hike(*it,base,contourMask,nodeMask,trail);
      }
    }
  }
  
  std::cout << "Building segments" << std::endl;
  Mask* segmentMask = new Mask(m_imWidth,m_imHeight);
  int segmentID = 1;
  std::vector<int> borderX,borderY;
  for(int i = 0; i < m_imWidth; i++){
    for(int j = 0; j < m_imHeight; j++){
      if(nodeMask->getValue(i,j) != 2 || segmentMask->getValue(i,j) > 0) continue;
      std::cout << i << " " << j << " " << segmentID << std::endl;
      segmentMask->setValue(i,j,segmentID);
      std::vector<LocalizedObject::Point> steps = trail->at(i).at(j);
      nodeMask->setValue(i,j,3);
      borderX.push_back(i);
      borderY.push_back(j);
      while(borderX.size() > 0){
	int nborder = borderX.size();
	for(int b = 0; b < nborder; b++){
	  int bi = borderX.at(0);
	  int bj = borderY.at(0);
	  int left = bi-1;
	  int right = bi+2;
	  int top = bj-1;
	  int bottom = bj+2;
	  if(left < 0) left++;
	  if(right >= m_imWidth) right--;
	  if(top < 0) top++;
	  if(bottom >= m_imHeight) bottom--;
	  for(int i2 = left; i2 < right; i2++){
	    for(int j2 = top; j2 < bottom; j2++){
	      if(nodeMask->getValue(i2,j2) == 2){
		nodeMask->setValue(i2,j2,3);
		segmentMask->setValue(i2,j2,segmentID);
		borderX.push_back(i2);
		borderY.push_back(j2);
		std::vector<LocalizedObject::Point> tmp = trail->at(i2).at(j2);
		for(std::vector<LocalizedObject::Point>::iterator jt = tmp.begin(); jt != tmp.end(); jt++) steps.push_back(*jt);
	      }
	    }
	  }
	  borderX.erase(borderX.begin());
	  borderY.erase(borderY.begin());
	}
      }
      int newSteps = steps.size();
      while(newSteps > 0){
	//std::cout << newSteps << std::endl;
	for(int k = 0; k < newSteps; k++){
	  LocalizedObject::Point pt = steps[k];
	  if(segmentMask->getValue(pt.x,pt.y) == segmentID){
	    continue;
	  }
	  segmentMask->setValue(pt.x,pt.y,segmentID);
	  std::vector<LocalizedObject::Point> tmp = trail->at(pt.x).at(pt.y);
	  for(std::vector<LocalizedObject::Point>::iterator jt = tmp.begin(); jt != tmp.end(); jt++) steps.push_back(*jt);
	}
	for(int k = 0; k < newSteps; k++) steps.erase(steps.begin());
	newSteps = steps.size();
      }
      segmentID++;
      //segmentID = segmentID % 256;
    }
  }
  
  delete contourMask;
  delete nodeMask;
  delete trail;
  return segmentMask;
  //return nodeMask;
}

void ImRecord::hike(LocalizedObject::Point pt, int base, Mask* contourMask, Mask* nodeMask, std::vector< std::vector< std::vector<LocalizedObject::Point> > >* trail)
{
  nodeMask->setValue(pt.x,pt.y,1);
  int max = contourMask->getValue(pt.x,pt.y);
  //if(max > base) max++;
  std::vector<LocalizedObject::Point> steps;
  int di[8] = {-1,-1,-1,0,0,1,1,1};
  int dj[8] = {-1,0,1,-1,1,-1,0,1};
  for(int k = 0; k < 8; k++){
    int i2 = pt.x+di[k];
    if(i2 < 0 || i2 >= m_imWidth) continue;
    int j2 = pt.y+dj[k];
    if(j2 < 0 || j2 >= m_imHeight) continue;
    int val = contourMask->getValue(i2,j2);
    if(val > max){
      max = val;
      steps.clear();
      steps.push_back(LocalizedObject::Point(i2,j2));
    }
    else if(val == max) steps.push_back(LocalizedObject::Point(i2,j2));
  }
  int stepsTaken = 0;
  for(std::vector<LocalizedObject::Point>::iterator it = steps.begin(); it != steps.end(); it++){
    trail->at(it->x).at(it->y).push_back(LocalizedObject::Point(pt.x,pt.y));
    if(nodeMask->getValue(it->x,it->y) > 0) continue;
    hike(*it,base,contourMask,nodeMask,trail);
    stepsTaken++;
  }
  if(stepsTaken == 0){
    nodeMask->setValue(pt.x,pt.y,2);
    return;
  }
}

Mask* ImRecord::getPunctaMask(int chan, bool outline)
{
  Mask* m = new Mask(m_imWidth,m_imHeight);
  std::vector<Cluster*> vec = m_puncta.at(chan);
  if(outline){
    Mask* m2 = m->getCopy();
    for(std::vector<Cluster*>::iterator jt = vec.begin(); jt != vec.end(); jt++){
      std::vector<LocalizedObject::Point> pts = (*jt)->getPoints();
      for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,1);
      for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++){
	int sum = 0;
	for(int dx = kt->x - 1; dx < kt->x + 2; dx++){
	  if(dx < 0 || dx >= m_imWidth){
	    sum += 3;
	    continue;
	  }
	  for(int dy = kt->y - 1; dy < kt->y + 2; dy++){
	    if(dy < 0 || dy >= m_imHeight) sum += 1;
	    else sum += m->getValue(dx,dy);
	  }
	}
	m2->setValue(kt->x,kt->y,1 - sum/9);
      }
    }
    delete m;
    return m2;
  }
  for(std::vector<Cluster*>::iterator jt = vec.begin(); jt != vec.end(); jt++){
    std::vector<LocalizedObject::Point> pts = (*jt)->getPoints();
    for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,1);
  }
  return m;
}

Mask* ImRecord::getSynapseMask(bool outline)
{
  Mask* m = new Mask(m_imWidth,m_imHeight);
  if(outline){
    Mask* m2 = m->getCopy();
    for(std::vector<SynapseCollection*>::iterator it = m_synapseCollections.begin(); it != m_synapseCollections.end(); it++){
      int nsyns = (*it)->nSynapses();
      for(int isyn = 0; isyn < nsyns; isyn++){
	std::vector<LocalizedObject::Point> pts = (*it)->getSynapse(isyn)->getPoints();
	for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,1);
	for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++){
	  int sum = 0;
	  for(int dx = kt->x - 1; dx < kt->x + 2; dx++){
	    if(dx < 0 || dx >= m_imWidth){
	      sum += 3;
	      continue;
	    }
	    for(int dy = kt->y - 1; dy < kt->y + 2; dy++){
	      if(dy < 0 || dy >= m_imHeight) sum += 1;
	      else sum += m->getValue(dx,dy);
	    }
	  }
	  m2->setValue(kt->x,kt->y,1 - sum/9);
	}
      }
    }
    delete m;
    return m2;
  }
  for(std::vector<SynapseCollection*>::iterator it = m_synapseCollections.begin(); it != m_synapseCollections.end(); it++){
    int nsyns = (*it)->nSynapses();
    for(int isyn = 0; isyn < nsyns; isyn++){
      std::vector<LocalizedObject::Point> pts = (*it)->getSynapse(isyn)->getPoints();
      for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,1);
    }
  }
  return m;
}

Mask* ImRecord::getSynapseMaskFromCollection(int index, bool outline)
{
  Mask* m = new Mask(m_imWidth,m_imHeight);
  if(outline){
    Mask* m2 = m->getCopy();
    SynapseCollection* sc = m_synapseCollections.at(index);
    int nsyns = sc->nSynapses();
    for(int isyn = 0; isyn < nsyns; isyn++){
      std::vector<LocalizedObject::Point> pts = sc->getSynapse(isyn)->getPoints();
      for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,1);
      for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++){
	int sum = 0;
	for(int dx = kt->x - 1; dx < kt->x + 2; dx++){
	  if(dx < 0 || dx >= m_imWidth){
	    sum += 3;
	    continue;
	  }
	  for(int dy = kt->y - 1; dy < kt->y + 2; dy++){
	    if(dy < 0 || dy >= m_imHeight) sum += 1;
	    else sum += m->getValue(dx,dy);
	  }
	}
	m2->setValue(kt->x,kt->y,1 - sum/9);
      }
    }
    delete m;
    return m2;
  }
  SynapseCollection* sc = m_synapseCollections.at(index);
  int nsyns = sc->nSynapses();
  for(int isyn = 0; isyn < nsyns; isyn++){
    std::vector<LocalizedObject::Point> pts = sc->getSynapse(isyn)->getPoints();
    for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,1);
  }
  return m;
}

Mask* ImRecord::getSynapticPunctaMask(int chan, bool outline)
{
  Mask* m = new Mask(m_imWidth,m_imHeight);
  if(outline){
    Mask* m2 = m->getCopy();
    for(std::vector<SynapseCollection*>::iterator it = m_synapseCollections.begin(); it != m_synapseCollections.end(); it++){
      int nsyns = (*it)->nSynapses();
      for(int isyn = 0; isyn < nsyns; isyn++){
	std::vector<LocalizedObject::Point> pts = (*it)->getSynapse(isyn)->getPunctum((*it)->getChannelIndex(chan))->getPoints();
	for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,1);
	for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++){
	  int sum = 0;
	  for(int dx = kt->x - 1; dx < kt->x + 2; dx++){
	    if(dx < 0 || dx >= m_imWidth){
	      sum += 3;
	      continue;
	    }
	    for(int dy = kt->y - 1; dy < kt->y + 2; dy++){
	      if(dy < 0 || dy >= m_imHeight) sum += 1;
	      else sum += m->getValue(dx,dy);
	    }
	  }
	  m2->setValue(kt->x,kt->y,1 - sum/9);
	}
      }
    }
    delete m;
    return m2;
  }
  for(std::vector<SynapseCollection*>::iterator it = m_synapseCollections.begin(); it != m_synapseCollections.end(); it++){
    int nsyns = (*it)->nSynapses();
    for(int isyn = 0; isyn < nsyns; isyn++){
      std::vector<LocalizedObject::Point> pts = (*it)->getSynapse(isyn)->getPunctum((*it)->getChannelIndex(chan))->getPoints();
      for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,1);
    }
  }
  return m;
}

Mask* ImRecord::getSynapticPunctaMaskFromCollection(int index, int chan, bool outline)
{
  Mask* m = new Mask(m_imWidth,m_imHeight);
  if(outline){
    Mask* m2 = m->getCopy();
    SynapseCollection* sc = m_synapseCollections.at(index);
    int nsyns = sc->nSynapses();
    for(int isyn = 0; isyn < nsyns; isyn++){
      std::vector<LocalizedObject::Point> pts = sc->getSynapse(isyn)->getPunctum(sc->getChannelIndex(chan))->getPoints();
      for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,1);
      for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++){
	int sum = 0;
	for(int dx = kt->x - 1; dx < kt->x + 2; dx++){
	  if(dx < 0 || dx >= m_imWidth){
	    sum += 3;
	    continue;
	  }
	  for(int dy = kt->y - 1; dy < kt->y + 2; dy++){
	    if(dy < 0 || dy >= m_imHeight) sum += 1;
	    else sum += m->getValue(dx,dy);
	  }
	}
	m2->setValue(kt->x,kt->y,1 - sum/9);
      }
    }
    delete m;
    return m2;
  }
  SynapseCollection* sc = m_synapseCollections.at(index);
  int nsyns = sc->nSynapses();
  for(int isyn = 0; isyn < nsyns; isyn++){
    std::vector<LocalizedObject::Point> pts = sc->getSynapse(isyn)->getPunctum(sc->getChannelIndex(chan))->getPoints();
    for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,1);
  }
  return m;
}

Mask* ImRecord::getRegionMask(bool outline)
{
  Mask* m = new Mask(m_imWidth,m_imHeight);
  int x1,x2,y1,y2;
  if(outline){
    Mask* m2 = m->getCopy();
    for(std::vector<Region*>::iterator jt = m_regions.begin(); jt != m_regions.end(); jt++){
      (*jt)->getEnclosure(x1,x2,y1,y2);
      x1++;
      x2--;
      y1++;
      y2--;
      for(int i = x1; i < x2; i++){
	for(int j = y1; j < y2; j++){
	  if((*jt)->contains(i,j)) m->setValue(i,j,1);
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
      std::vector<LocalizedObject::Point> axPts = (*jt)->axis();
      for(int i = axPts.size()-1; i > 0; i--){
	double slope = ((double)(axPts[i-1].y) - axPts[i].y) / (axPts[i-1].x - axPts[i].x);
	if(fabs(slope) > 1){
	  double step = (axPts[i-1].y - axPts[i].y)/fabs(axPts[i-1].y - axPts[i].y);
	  int nsteps = (int)((axPts[i-1].y - axPts[i].y)/step);
	  double y = axPts[i].y;
	  double x = axPts[i].x;
	  for(int j = 0; j < nsteps; j++){
	    m2->setValue((int)x,(int)y,1);
	    y += step;
	    x += step/slope;
	  }
	}
	else{
	  double step = (axPts[i-1].x - axPts[i].x)/fabs(axPts[i-1].x - axPts[i].x);
	  int nsteps = (int)((axPts[i-1].x - axPts[i].x)/step);
	  double x = axPts[i].x;
	  double y = axPts[i].y;
	  for(int j = 0; j < nsteps; j++){
	    m2->setValue((int)x,(int)y,1);
	    x += step;
	    y += step*slope;
	  }
	}
      }
    }
    delete m;
    return m2;
  }
  for(std::vector<Region*>::iterator jt = m_regions.begin(); jt != m_regions.end(); jt++){
    (*jt)->getEnclosure(x1,x2,y1,y2);
    for(int i = x1; i < x2; i++){
      for(int j = y1; j < y2; j++){
	if((*jt)->contains(i,j)) m->setValue(i,j,1);
      }
    }
    std::vector<LocalizedObject::Point> axPts = (*jt)->axis();
    for(int i = axPts.size()-1; i > 0; i--){
      double slope = ((double)(axPts[i-1].y) - axPts[i].y) / (axPts[i-1].x - axPts[i].x);
      if(fabs(slope) > 1){
	double step = (axPts[i-1].y - axPts[i].y)/fabs(axPts[i-1].y - axPts[i].y);
	int nsteps = (int)((axPts[i-1].y - axPts[i].y)/step);
	double y = axPts[i].y;
	double x = axPts[i].x;
	for(int j = 0; j < nsteps; j++){
	  m->setValue((int)x,(int)y,1);
	  y += step;
	  x += step/slope;
	}
      }
      else{
	double step = (axPts[i-1].x - axPts[i].x)/fabs(axPts[i-1].x - axPts[i].x);
	int nsteps = (int)((axPts[i-1].x - axPts[i].x)/step);
	double x = axPts[i].x;
	double y = axPts[i].y;
	for(int j = 0; j < nsteps; j++){
	  m->setValue((int)x,(int)y,1);
	  x += step;
	  y += step*slope;
	}
      }
    }
  }
  return m;
}

void ImRecord::calculateRegionStats(Region* r, int postChan)
{
  r->dendriteArea = 0.0;
  int x1,x2,y1,y2;
  r->getEnclosure(x1,x2,y1,y2);
  Mask* m = m_signalMasks.at(postChan);
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      if(m->getValue(i,j) > 0 && r->contains(i,j)) r->dendriteArea += 1.0;
    }
  }
  r->dendriteArea *= m_resolutionXY*m_resolutionXY;
  r->dendriteLength = r->getLength() * m_resolutionXY;
  r->nSynapses.clear();
  r->avgSynapseSize.clear();
  r->avgOverlap.clear();
  r->nPuncta.clear();
  r->avgPunctaSize.clear();
  r->avgPeakIntensity.clear();
  r->avgIntegratedIntensity.clear();
  for(std::vector<SynapseCollection*>::iterator it = m_synapseCollections.begin(); it != m_synapseCollections.end(); it++){
    r->nSynapses.push_back(0);
    r->avgSynapseSize.push_back(0.0);
    r->avgOverlap.push_back(std::vector<double>());
    std::vector<int>::iterator nsyn = r->nSynapses.end()-1;
    std::vector<double>::iterator avgS = r->avgSynapseSize.end()-1;
    std::vector< std::vector<double> >::iterator avgO = r->avgOverlap.end()-1;
    int nreq = (*it)->nRequirements();
    avgO->assign(nreq,0.0);
    int n = (*it)->nSynapses();
    for(int i = 0; i < n; i++){
      Synapse* s = (*it)->getSynapse(i);
      if(!r->contains(s->center())) continue;
      (*nsyn)++;
      (*avgS) += s->size();
      if((*it)->allRequired()) avgO->at(0) += s->punctaOverlap();
      else{
	for(int ireq = 0; ireq < nreq; ireq++) avgO->at(ireq) += s->punctaOverlap((*it)->getRequiredColocalizationByIndex(ireq));
      }
    }
    (*avgS) *= m_resolutionXY*m_resolutionXY/(*nsyn);
    for(std::vector<double>::iterator jt = avgO->begin(); jt != avgO->end(); jt++) (*jt) *= m_resolutionXY*m_resolutionXY/(*nsyn);
  }
  for(std::vector< std::vector<Cluster*> >::iterator it = m_puncta.begin(); it != m_puncta.end(); it++){
    r->nPuncta.push_back(0);
    r->avgPunctaSize.push_back(0.0);
    r->avgPeakIntensity.push_back(0.0);
    r->avgIntegratedIntensity.push_back(0.0);
    std::vector<int>::iterator npunc = r->nPuncta.end()-1;
    std::vector<double>::iterator avgS = r->avgPunctaSize.end()-1;
    std::vector<double>::iterator avgP = r->avgPeakIntensity.end()-1;
    std::vector<double>::iterator avgT = r->avgIntegratedIntensity.end()-1;
    for(std::vector<Cluster*>::iterator jt = it->begin(); jt != it->end(); jt++){
      if(!r->contains((*jt)->center())) continue;
      (*npunc)++;
      (*avgS) += (*jt)->size();
      (*avgP) += (*jt)->peak();
      (*avgT) += (*jt)->integratedIntensity();
    }
    (*avgS) *= m_resolutionXY*m_resolutionXY/(*npunc);
    (*avgP) /= (*npunc);
    (*avgT) /= (*npunc);
  }
}

void ImRecord::printSynapseDensityTable(int postChan, std::string filename)
{
  for(std::vector<Region*>::iterator it = m_regions.begin(); it != m_regions.end(); it++) calculateRegionStats(*it,postChan);
  int nROI = m_regions.size();
  std::vector<Region*>::iterator rit;
  std::ofstream fout(filename.c_str());
  fout << "Field,";
  for(int r = 0; r < nROI; r++) fout << "\"Region " << r << "\",";
  fout << "Average,Description\n";
  for(int chan = 0; chan < m_nchannels; chan++){
    fout << "Channel " << chan << " threshold,";
    for(int r = 0; r < nROI; r++) fout << "-,";
    fout << "" << m_thresholds.at(chan) << ",-\n";
  }
  fout << "\"Dendrite area (um^2)\",";
  double sum = 0;
  for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
    fout << "" << (*rit)->dendriteArea << ",";
    sum += (*rit)->dendriteArea;
  }
  fout << "" << (sum/nROI) << ",-\n";
  fout << "\"Dendrite length (um)\",";
  sum = 0;
  for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
    fout << "" << (*rit)->dendriteLength << ",";
    sum += (*rit)->dendriteLength;
  }
  fout << "" << (sum/nROI) << ",-\n";
  fout << "-,";
  for(rit = m_regions.begin(); rit != m_regions.end(); rit++) fout << "-,";
  fout << "-,-\n";
  int icol = 0;
  for(std::vector<SynapseCollection*>::iterator scit = m_synapseCollections.begin(); scit != m_synapseCollections.end(); scit++){
    fout << "\"Type " << icol << " synapses\",";
    sum = 0;
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
      fout << "" << (*rit)->nSynapses.at(icol) << ",";
      sum += (*rit)->nSynapses.at(icol);
    }
    fout << "" << (sum/nROI) << ",\"" << (*scit)->description() << "\"\n\"Density (per 100 um^2)\",";
    sum = 0;
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
      double densityA = 100 * (*rit)->nSynapses.at(icol)/(*rit)->dendriteArea;
      fout << "" << densityA << ",";
      sum += densityA;
    }
    fout << "" << (sum/nROI) << ",-\n\"Average size (um^2)\",";
    sum = 0;
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
      fout << "" << (*rit)->avgSynapseSize.at(icol) << ",";
      sum += (*rit)->avgSynapseSize.at(icol);
    }
    fout << "" << (sum/nROI) << ",-\n";
    int nreq = (*scit)->nRequirements();
    for(int i = 0; i < nreq; i++){
      fout << "\"Average overlap " << i << " (um^2)\",";
      sum = 0;
      for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
	fout << "" << (*rit)->avgOverlap.at(icol).at(i) << ",";
	sum += (*rit)->avgOverlap.at(icol).at(i);
      }
      fout << "" << (sum/nROI) << ",";
      if((*scit)->allRequired()) fout << "-\n";
      else{
	std::vector<int> req = (*scit)->getRequiredColocalization(i);
	fout << "\"" << m_channelNames.at(req.at(0));
	for(std::vector<int>::iterator it = req.begin()+1; it != req.end(); it++) fout << " and " << m_channelNames.at(*it);
	fout << "\"\n";
      }
    }
    fout << "-,";
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++) fout << "-,";
    fout << "-,-\n";
    icol++;
  }
  for(int chan = 0; chan < m_nchannels; chan++){
    fout << "\"" << m_channelNames.at(chan) << " puncta\",";
    sum = 0;
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
      fout << "" << (*rit)->nPuncta.at(chan) << ",";
      sum += (*rit)->nPuncta.at(chan);
    }
    fout << "" << (sum/nROI) << ",-\n\"Density (per 100 um^2)\",";
    sum = 0;
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
      double densityA = 100 * (*rit)->nPuncta.at(chan)/(*rit)->dendriteArea;
      fout << "" << densityA << ",";
      sum += densityA;
    }
    fout << "" << (sum/nROI) << ",-\n\"Average size (um^2)\",";
    sum = 0;
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
      fout << "" << (*rit)->avgPunctaSize.at(chan) << ",";
      sum += (*rit)->avgPunctaSize.at(chan);
    }
    fout << "" << (sum/nROI) << ",-\n\"Average peak intensity\",";
    sum = 0;
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
      fout << "" << (*rit)->avgPeakIntensity.at(chan) << ",";
      sum += (*rit)->avgPeakIntensity.at(chan);
    }
    fout << "" << (sum/nROI) << ",-\n\"Average integrated intensity\",";
    sum = 0;
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
      fout << "" << (*rit)->avgIntegratedIntensity.at(chan) << ",";
      sum += (*rit)->avgIntegratedIntensity.at(chan);
    }
    fout << "" << (sum/nROI) << ",-\n";
    fout << "-,";
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++) fout << "-,";
    fout << "-,-\n";
  }
  fout.close();
}

void ImRecord::write(std::ofstream& fout, int version)
{
  char* buf = new char[400000];
  buf[0] = (char)m_nchannels;
  NiaUtils::writeShortToBuffer(buf,1,m_imWidth);
  NiaUtils::writeShortToBuffer(buf,3,m_imHeight);
  NiaUtils::writeDoubleToBuffer(buf,5,m_resolutionXY);
  uint64_t offset = 9;
  for(std::vector<int>::iterator it = m_thresholds.begin(); it != m_thresholds.end(); it++){
    NiaUtils::writeShortToBuffer(buf,offset,*it);
    offset += 2;
  }
  fout.write(buf,offset);
  for(int chan = 0; chan < m_nchannels; chan++){
    int len = m_channelNames[chan].length();
    buf[0] = (char)len;
    fout.write(buf,1);
    fout.write(m_channelNames[chan].c_str(),len);
    if(m_signalMasks.at(chan)){
      buf[0] = (char)1;
      offset = 1;
      fout.write(buf,1);
      int npacks = (m_imHeight / 8);
      if(m_imHeight % 8 > 0) npacks++;
      for(int i = 0; i < npacks; i++){
	int startY = 8*i;
	offset = pack(buf,m_signalMasks.at(chan),startY);
	fout.write(buf,offset);
      }
    }
    else{
      buf[0] = (char)0;
      fout.write(buf,1);
    }
    
    NiaUtils::writeIntToBuffer(buf,0,nPuncta(chan));
    fout.write(buf,4);
    for(std::vector<Cluster*>::iterator it = m_puncta.at(chan).begin(); it != m_puncta.at(chan).end(); it++){
      (*it)->write(buf,fout);
    }
  }

  
  buf[0] = (char)m_synapseCollections.size();
  fout.write(buf,1);
  for(std::vector<SynapseCollection*>::iterator it = m_synapseCollections.begin(); it != m_synapseCollections.end(); it++){
    (*it)->write(buf,fout);
  }

  
  buf[0] = (char)nRegions();
  fout.write(buf,1);
  for(std::vector<Region*>::iterator it = m_regions.begin(); it != m_regions.end(); it++){
    if(version == 0) (*it)->writeV00(buf,fout);
    else (*it)->write(buf,fout);
  }
  delete[] buf;
}

uint64_t ImRecord::pack(char* buf, Mask* m, int startY)
{
  uint64_t offset = 0;
  uint8_t nextByte = 0x00;
  int nb = 0;
  for(int j = startY; j < startY+8 && j < m_imHeight; j++){
    for(int i = 0; i < m_imWidth; i++){
      nextByte = nextByte | (((uint8_t)m->getValue(i,j) & 0x01) << nb);
      nb++;
      if(nb > 7){
	buf[offset] = (char)nextByte;
	nextByte = 0x00;
	nb = 0;
	offset++;
      }
    }
  }
  if(nb > 0){
    buf[offset] = (char)nextByte;
    offset++;
  }
  return offset;
}

void ImRecord::read(std::ifstream& fin, int version)
{
  char* buf = new char[400000];
  fin.read(buf,9);
  m_nchannels = (int)buf[0];
  m_imWidth = NiaUtils::convertToShort(buf[1],buf[2]);
  m_imHeight = NiaUtils::convertToShort(buf[3],buf[4]);
  m_resolutionXY = NiaUtils::convertToDouble(buf[5],buf[6],buf[7],buf[8]);
  fin.read(buf,2*m_nchannels);
  uint64_t offset = 0;
  for(int chan = 0; chan < m_nchannels; chan++){
    m_thresholds.push_back(NiaUtils::convertToShort(buf[offset],buf[offset+1]));
    offset += 2;
  }
  int nunpacks = m_imHeight / 8;
  int leftovers = (m_imHeight % 8) * m_imWidth;
  for(int chan = 0; chan < m_nchannels; chan++){
    fin.read(buf,1);
    int len = (int)buf[0];
    fin.read(buf,len);
    m_channelNames.push_back(std::string(buf,len));
    fin.read(buf,1);
    if((int)buf[0] > 0){
      Mask* m = new Mask(m_imWidth,m_imHeight);
      for(int i = 0; i < nunpacks; i++){
	fin.read(buf,m_imWidth);
	unpack(buf,m,8*i);
      }
      if(leftovers > 0){
	int nbytes = leftovers / 8;
	if(leftovers % 8 > 0) nbytes++;
	fin.read(buf,nbytes);
	unpack(buf,m,8*nunpacks);
      }
      m_signalMasks.push_back(m);
    }
    else m_signalMasks.push_back(NULL);

    std::vector<Cluster*> clusters;
    fin.read(buf,4);
    int nClusters = NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]);
    for(int i = 0; i < nClusters; i++){
      Cluster* c = new Cluster();
      c->read(buf,fin);
      c->computeCenter();
      clusters.push_back(c);
    }
    m_puncta.push_back(clusters);
  }

  fin.read(buf,1);
  int ncol = (int)buf[0];
  for(int icol = 0; icol < ncol; icol++){
    SynapseCollection* sc = new SynapseCollection();
    sc->read(buf,fin,&m_puncta);
    m_synapseCollections.push_back(sc);
  }
  
  fin.read(buf,1);
  int nROI = (int)buf[0];
  for(int i = 0; i < nROI; i++){
    Region* r = new Region();
    if(version == 0) r->readV00(buf,fin);
    else r->read(buf,fin);
    m_regions.push_back(r);
  }

  delete[] buf;
}

void ImRecord::unpack(char* buf, Mask* m, int startY)
{
  uint64_t offset = 0;
  uint8_t nextByte = buf[offset];
  int nb = 0;
  for(int j = startY; j < startY+8 && j < m_imHeight; j++){
    for(int i = 0; i < m_imWidth; i++){
      uint8_t val = ((nextByte >> nb) & 0x01);
      m->setValue(i,j,(int)val);
      nb++;
      if(nb > 7){
	offset++;
	nextByte = buf[offset];
	nb = 0;
      }
    }
  }
}

void ImRecord::loadMetaMorphRegions(std::string filename)
{
  std::ifstream fin(filename);
  while(!fin.eof()){
    char buf[500];
    fin.getline(buf,500);
    if(fin.fail()) return;
    boost::char_separator<char> sep(",");
    std::string line(buf);
    boost::tokenizer< boost::char_separator<char> > tokens(line,sep);
    boost::tokenizer< boost::char_separator<char> >::iterator it = tokens.begin();
    for(int i = 0; i < 6; i++) it++;
    boost::char_separator<char> sep2(" ");
    boost::tokenizer< boost::char_separator<char> > tokens2(*it,sep2);
    boost::tokenizer< boost::char_separator<char> >::iterator it2 = tokens2.begin();
    if(boost::lexical_cast<int>(*it2) != 6){
      std::cout << "Format error: didn't find list of vertices where expected" << std::endl;
      return;
    }
    Region* r = new Region();
    it2++;
    int nvert = boost::lexical_cast<int>(*it2);
    it2++;
    while(it2 != tokens2.end()){
      LocalizedObject::Point pt;
      pt.x = boost::lexical_cast<int>(*it2);
      it2++;
      pt.y = boost::lexical_cast<int>(*it2);
      r->addVertex(pt);
      it2++;
    }
    addRegion(r);
  }
}

void ImRecord::loadMetaMorphTraces(std::string filename, int chan, bool overwrite)
{
}
