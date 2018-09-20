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
    m_stormClusters.push_back(std::vector<StormCluster*>());
  }
}

ImRecord::~ImRecord()
{
  for(std::vector<Mask*>::iterator it = m_signalMasks.begin(); it != m_signalMasks.end(); it++) delete *it;
  for(std::vector<Mask*>::iterator it = m_utilityMasks.begin(); it != m_utilityMasks.end(); it++) delete *it;
  for(std::vector< std::vector<Cluster*> >::iterator it = m_puncta.begin(); it != m_puncta.end(); it++){
    for(std::vector<Cluster*>::iterator jt = it->begin(); jt != it->end(); jt++) delete *jt;
    it->clear();
  }
  m_puncta.clear();
  for(std::vector<SynapseCollection*>::iterator it = m_synapseCollections.begin(); it != m_synapseCollections.end(); it++) delete *it;
  for(std::vector<Region*>::iterator it = m_regions.begin(); it != m_regions.end(); it++) delete *it;
}

ImRecord* ImRecord::emptyCopy()
{
  ImRecord* newRecord = new ImRecord(m_nchannels, m_imWidth, m_imHeight);
  newRecord->setResolutionXY(m_resolutionXY);
  for(int i = 0; i < m_nchannels; i++) newRecord->setChannelName(i,m_channelNames[i]);
  for(int i = 0; i < nRegions(); i++) newRecord->addRegion(m_regions[i]->getCopy());
  for(int i = 0; i < nSynapseCollections(); i++) newRecord->addSynapseCollection(m_synapseCollections[i]->emptyCopy());
  return newRecord;
}

void ImRecord::removePunctum(int chan, int index)
{
  std::vector<Cluster*> clusters = m_puncta[chan];
  delete clusters.at(index);
  m_puncta[chan].erase(m_puncta[chan].begin()+index);
}

void ImRecord::removePunctum(int chan, Cluster* c)
{
  for(std::vector<Cluster*>::iterator clit = m_puncta[chan].begin(); clit != m_puncta[chan].end(); clit++){
    if(c == *clit){
      delete *clit;
      m_puncta[chan].erase(clit);
      return;
    }
  }
}

void ImRecord::clearPuncta(int chan)
{
    std::vector<Cluster*> clusters = m_puncta.at(chan);
    for(std::vector<Cluster*>::iterator clit = clusters.begin(); clit != clusters.end(); clit++) delete *clit;
    m_puncta[chan].clear();
}

void ImRecord::sortPuncta(int chan, ImFrame* frame)
{
  std::vector<Cluster*> clusters = m_puncta.at(chan);
  for(std::vector<Cluster*>::iterator clit = clusters.begin(); clit != clusters.end(); clit++) (*clit)->sort(frame);
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

void ImRecord::clearSegments()
{
  for(std::vector<Segment*>::iterator it = m_segments.begin(); it != m_segments.end(); it++){
    if(*it) delete *it;
  }
  m_segments.clear();
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

Cluster* ImRecord::selectPunctumStrict(int chan, LocalizedObject::Point pt)
{
  std::vector<Cluster*> clusters = m_puncta.at(chan);
  for(std::vector<Cluster*>::iterator jt = clusters.begin(); jt != clusters.end(); jt++){
    if((*jt)->contains(pt)) return *jt;
  }
  return NULL;
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

void ImRecord::convertRegionsToSegments(int chan)
{
  std::vector<Cluster*> clusters;
  Mask* m = m_signalMasks[chan];
  if(!m) return;
  for(int i = 0; i < nRegions(); i++) clusters.push_back(new Cluster());
  for(int i = 0; i < m_imWidth; i++){
    for(int j = 0; j < m_imHeight; j++){
      if(m->getValue(i,j) == 0) continue;
      for(int r = 0; r < nRegions(); r++){
	if(m_regions[r]->contains(i,j)){
	  clusters[r]->addPoint(i,j);
	  break;
	}
      }
    }
  }
  /*
  for(std::vector<Segment*>::iterator sit = m_segments.begin(); sit != m_segments.end(); sit++){
    if(*sit) delete *sit;
  }
  m_segments.clear();
  */
  for(std::vector<Cluster*>::iterator clit = clusters.begin(); clit != clusters.end(); clit++){
    (*clit)->computeCenter();
    (*clit)->findBorder();
    Segment* s = new Segment(*clit);
    s->findOrientation();
    m_segments.push_back(s);
  }
  clearRegions();
}

Mask* ImRecord::segment(int chan)
{
  for(std::vector<Segment*>::iterator sit = m_segments.begin(); sit != m_segments.end(); sit++){
    if(*sit) delete *sit;
  }
  m_segments.clear();
  Mask* contourMask = getContourMap(chan);
  Mask* segmentMask = new Mask(m_imWidth,m_imHeight,0);
  Mask* borderMask = new Mask(m_imWidth,m_imHeight,0);
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
	  //c->addPoint(bi,bj);
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
	if(contourMask->getValue(i,j) < 1) continue;
	int val = segmentMask->getValue(i,j);
	int n[8] = {0,0,0,0,0,0,0,0};
	int id[8] = {0,0,0,0,0,0,0,0};
	for(int k = 0; k < 8; k++){
	  int i2 = i+di[k];
	  int j2 = j+dj[k];
	  if(i2 < 0 || i2 >= m_imWidth) continue;
	  if(j2 < 0 || j2 >= m_imHeight) continue;
	  int segVal = segmentMask->getValue(i2,j2);
	  if(segVal > 0){
	    for(int l = 0; l < 8; l++){
	      if(id[l] == 0){
		id[l] = segVal;
		n[l]++;
		break;
	      }
	      else if(id[l] == segVal){
		n[l]++;
		break;
	      }
	    }
	  }
	}
	int maxN = 0;
	int maxID = 0;
	for(int k = 0; k < 8; k++){
	  if(n[k] > maxN){
	    maxN = n[k];
	    maxID = id[k];
	  }
	}
	if(maxID > 0){
	  if(maxID != val){
	    segmentMask->setValue(i,j,maxID);
	    finished = false;
	  }
	  if(maxN < 8) borderMask->setValue(i,j,1);
	  else borderMask->setValue(i,j,0);
	}
      }
    }
  }
  for(int i = 0; i < m_imWidth; i++){
    for(int j = 0; j < m_imHeight; j++){
      int val = segmentMask->getValue(i,j);
      if(val < 1) continue;
      clusters[val-1]->addPoint(i,j,(borderMask->getValue(i,j) > 0));
    }
  }
  std::cout << "Done adding peaks" << std::endl;
  for(std::vector<Cluster*>::iterator clit = clusters.begin(); clit != clusters.end(); clit++){
    if((*clit)->size() == 0){
      delete *clit;
      *clit = NULL;
    }
    else (*clit)->computeCenter();
    //(*clit)->findBorder();
  }
  std::cout << "Done finding centers" << std::endl;
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

  int sizeLimit = 1000;
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
  
  nChanges = 0;
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

  int last = segments.size()-1;
  for(int i = 0; i < last/2.0; i++){
    Segment* tmp = segments[i];
    segments[i] = segments[last-i];
    segments[last-i] = tmp;
  }
  sizeLimit = 2;
  for(int i = 0; i < 2; i++){
    for(std::vector<Segment*>::iterator sit = segments.begin(); sit != segments.end(); sit++){
      if(!(*sit)) continue;
      int size1 = (*sit)->size();
      //if(size1 > 4000) continue;
      LocalizedObject::Point cent1 = (*sit)->cluster()->center();
      int perimeter = (*sit)->cluster()->perimeter();
      finished = false;
      while(!finished){
	finished = true;
	int maxBorder = perimeter / 8;
	int totalBorder = 0;
	int maxSize = 0;
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
	  //if(size2 > size1*sizeLimit || size1 > size2*sizeLimit) continue;
	  if(border > maxBorder){
	    maxBorder = border;
	    maxSeg  = sjt;
	    maxSize = size2;
	  }
	}
	if(maxSeg != segments.end() && totalBorder > perimeter/2){
	  //if(size1 > (*maxSeg)->size() / 3 || (*maxSeg)->circularity() > 0.1 || totalBorder > 2*perimeter/3){
	  if(maxSize < size1*sizeLimit && size1 < maxSize*sizeLimit){
	    (*sit)->merge(*maxSeg);
	    size1 += maxSize;
	    perimeter = (*sit)->cluster()->perimeter();
	    finished = false;
	    delete *maxSeg;
	    *maxSeg = NULL;
	  }
	}
      }
    }
    sizeLimit = 10000;
  }
  std::cout << "Done merging 4" << std::endl;
  
  segmentID = 1;
  segmentMask->clear(0,m_imWidth,0,m_imHeight);
  for(std::vector<Segment*>::iterator sit = segments.begin(); sit != segments.end(); sit++){
    if(!(*sit)) continue;
    Cluster* c = (*sit)->cluster();
    for(std::vector<LocalizedObject::Point>::iterator pit = c->begin(); pit != c->end(); pit++) segmentMask->setValue(pit->x,pit->y,segmentID);
    segmentID++;
  }
  for(double circularityLimit = 10.1; circularityLimit < 0.45; circularityLimit += 0.05){
    nChanges = 1;
    while(nChanges > 0){
      for(int i = segments.size(); i >= 0; i--){
	if(!segments[i]) segments.erase(segments.begin()+i);
      }
      if(circularityLimit < 0.4){
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
      else{
	for(unsigned i = 0; i < segments.size(); i++){
	  for(unsigned j = i+1; j < segments.size(); j++){
	    if(segments[i]->circularity() > segments[j]->circularity()){
	      Segment* tmp = segments[i];
	      segments[i] = segments[j];
	      segments[j] = tmp;
	    }
	  }
	}
      }
      
      nChanges = 0;
      for(std::vector<Segment*>::iterator sit = segments.begin(); sit != segments.end(); sit++){
	if(!(*sit)) continue;
	int size1 = (*sit)->size();
	double circ1 = (*sit)->circularity();
	if((*sit)->circularity() > circularityLimit) continue;
	LocalizedObject::Point cent1 = (*sit)->cluster()->center();
	int perimeter = (*sit)->cluster()->perimeter();
	double minAngle = 1.6;
	int totalBorder = 0;
	double minCirc = 0.0;
	std::vector<Segment*>::iterator minSeg = segments.end();
	std::vector<Cluster*> borderClusters;
	Cluster* minBC;
	std::vector<int> clusterSizes;
	std::vector<bool> isBordering;
	LocalizedObject::Point minCent;
	int minBorder;
	for(std::vector<Segment*>::iterator sjt = segments.begin(); sjt != segments.end(); sjt++){
	  if(!(*sjt)) continue;
	  if(sjt == sit) continue;
	  int size2 = (*sjt)->size();
	  LocalizedObject::Point cent2 = (*sjt)->cluster()->center();
	  double diffy = cent2.y - cent1.y;
	  double diffx = cent2.x - cent1.x;
	  double dist = sqrt(diffx*diffx + diffy*diffy);
	  if(dist > (size1+size2)/2) continue;
	  int border = (*sit)->cluster()->getBorderLength((*sjt)->cluster());
	  totalBorder += border;
	  std::vector<LocalizedObject::Point> borderCenters = (*sit)->cluster()->findClosestPoints((*sjt)->cluster());
	  if(border < 1){
	    if(!(segmentMask->isMinimallyConnected(cent1.x,cent1.y,cent2.x,cent2.y,segmentMask->getValue(cent1.x,cent1.y),segmentMask->getValue(cent2.x,cent2.y)))) continue;
	    diffx = borderCenters[0].x - borderCenters[1].x;
	    diffy = borderCenters[0].y - borderCenters[1].y;
	    if(sqrt(diffx*diffx + diffy*diffy) > 25) continue;
	    //if(size1 < 1000 && size2 < 1000) continue;
	  }
	  isBordering.push_back((border > 0));
	  borderClusters.push_back((*sjt)->cluster());
	  if((*sjt)->circularity() - circ1 > 0.1 && size2 > size1) continue;
	  if((*sjt)->circularity() - circ1 < -0.1 && size1 > size2) continue;
	  double vec1 = (*sit)->eigenVector1();
	  diffy = borderCenters[0].y - cent1.y;
	  diffx = borderCenters[0].x - cent1.x;
	  dist = sqrt(diffx*diffx + diffy*diffy);
	  double angle = acos(diffx*cos(vec1)/dist + diffy*sin(vec1)/dist);
	  if(fabs(angle) > 1.570796327) vec1 += 3.141592654;
	  double vec2 = (*sjt)->eigenVector1();
	  diffy = cent2.y - borderCenters[1].y;
	  diffx = cent2.x - borderCenters[1].x;
	  if((*sjt)->circularity() > 0.4){
	    vec2 = atan(diffy/diffx);
	  }
	  dist = sqrt(diffx*diffx + diffy*diffy);
	  angle = acos(diffx*cos(vec2)/dist + diffy*sin(vec2)/dist);
	  if(fabs(angle) > 1.570796327) vec2 += 3.141592654;
	  angle = fabs(vec2 - vec1);
	  if(angle > 3.141592654) angle = 6.283185307 - angle;
	  if(angle < minAngle){
	    minAngle = angle;
	    minSeg  = sjt;
	    minCirc = (*sjt)->circularity();
	    //minBC = borderCluster;
	    minCent = cent2;
	    minBorder = border;
	  }
	}
	if(totalBorder < perimeter / 3 && minSeg != segments.end() && minAngle < 0.2618 + 0.7854*exp((circ1 + minCirc - 2)/2.0)){
	  bool uniqueBorder = true;
	  if(circularityLimit < 0.4){
	    for(unsigned i = 0; i < borderClusters.size(); i++){
	      if(borderClusters[i] == (*minSeg)->cluster()) continue;
	      if(minBorder > 0){
		if(!isBordering[i]) continue;
	      }
	      else if(isBordering[i] && size1 < 1000){
		uniqueBorder = false;
		break;
	      }
	      //if(circularityLimit > 0.35 && borderClusters[i]->size() < 2500) continue;
	      std::vector<LocalizedObject::Point> closestPoints = (*minSeg)->cluster()->findClosestPoints(borderClusters[i]);
	      int diffx = closestPoints[0].x - closestPoints[1].x;
	      int diffy = closestPoints[0].y - closestPoints[1].y;
	      if(sqrt(diffx*diffx + diffy*diffy) < 25){
		uniqueBorder = false;
		break;
	      }
	    }
	  }
	  if(uniqueBorder){
	    (*sit)->merge(*minSeg);
	    int segID = segmentMask->getValue(cent1.x,cent1.y);
	    for(std::vector<LocalizedObject::Point>::iterator pit = (*minSeg)->cluster()->begin(); pit != (*minSeg)->cluster()->end(); pit++) segmentMask->setValue(pit->x,pit->y,segID);
	    delete *minSeg;
	    *minSeg = NULL;
	    nChanges++;
	  }
	}
      }
    }
  }
  std::cout << "Done merging 5" << std::endl;
  segmentID = 1;
  segmentMask->clear(0,m_imWidth,0,m_imHeight);
  for(std::vector<Segment*>::iterator sit = segments.begin(); sit != segments.end(); sit++){
    if(!(*sit)) continue;
    Cluster* c = (*sit)->cluster();
    for(std::vector<LocalizedObject::Point>::iterator pit = c->begin(); pit != c->end(); pit++) segmentMask->setValue(pit->x,pit->y,segmentID);
    segmentID++;
    m_segments.push_back(*sit);
  }
  std::cout << segmentID << " segments found" << std::endl;
  
  delete contourMask;
  delete used;
  return segmentMask;
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
  r->nStormSynapses.clear();
  r->avgSynapseSize.clear();
  r->avgOverlap.clear();
  r->nPuncta.clear();
  r->avgPunctaSize.clear();
  r->avgPeakIntensity.clear();
  r->avgIntegratedIntensity.clear();
  for(std::vector<SynapseCollection*>::iterator it = m_synapseCollections.begin(); it != m_synapseCollections.end(); it++){
    r->nSynapses.push_back(0);
    r->nStormSynapses.push_back(0);
    r->avgSynapseSize.push_back(0.0);
    r->avgOverlap.push_back(std::vector<double>());
    std::vector<int>::iterator nsyn = r->nSynapses.end()-1;
    std::vector<int>::iterator nsynStorm = r->nStormSynapses.end()-1;
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
    n = (*it)->nStormSynapses();
    for(int i = 0; i < n; i++){
      StormCluster::StormSynapse s = (*it)->getStormSynapse(i);
      if(!(r->contains(s.centerX,s.centerY))) continue;
      (*nsynStorm)++;
    }
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

void ImRecord::calculateSegmentStats(Segment* r, int matchChan)
{
  Mask* m = r->getMask(m_imWidth,m_imHeight);
  r->dendriteArea = r->size()*m_resolutionXY*m_resolutionXY;
  r->dendriteLength = 1;//r->getLength() * m_resolutionXY;
  r->nSynapses.clear();
  r->nStormSynapses.clear();
  r->avgSynapseSize.clear();
  r->avgOverlap.clear();
  r->nPuncta.clear();
  r->avgPunctaSize.clear();
  r->avgPeakIntensity.clear();
  r->avgIntegratedIntensity.clear();
  for(std::vector<SynapseCollection*>::iterator it = m_synapseCollections.begin(); it != m_synapseCollections.end(); it++){
    r->nSynapses.push_back(0);
    r->nStormSynapses.push_back(0);
    r->avgSynapseSize.push_back(0.0);
    r->avgOverlap.push_back(std::vector<double>());
    std::vector<int>::iterator nsyn = r->nSynapses.end()-1;
    std::vector<int>::iterator nsynStorm = r->nStormSynapses.end()-1;
    std::vector<double>::iterator avgS = r->avgSynapseSize.end()-1;
    std::vector< std::vector<double> >::iterator avgO = r->avgOverlap.end()-1;
    int nreq = (*it)->nRequirements();
    avgO->assign(nreq,0.0);
    int n = (*it)->nSynapses();
    int matchIndex = (*it)->getChannelIndex(matchChan);
    for(int i = 0; i < n; i++){
      Synapse* s = (*it)->getSynapse(i);
      Cluster* c = s->getPunctum(matchIndex);
      LocalizedObject::Point pt = c->center();
      if(m->getValue(pt.x,pt.y) == 0) continue;
      (*nsyn)++;
      (*avgS) += s->size();
      if((*it)->allRequired()) avgO->at(0) += s->punctaOverlap();
      else{
	for(int ireq = 0; ireq < nreq; ireq++) avgO->at(ireq) += s->punctaOverlap((*it)->getRequiredColocalizationByIndex(ireq));
      }
    }
    (*avgS) *= m_resolutionXY*m_resolutionXY/(*nsyn);
    for(std::vector<double>::iterator jt = avgO->begin(); jt != avgO->end(); jt++) (*jt) *= m_resolutionXY*m_resolutionXY/(*nsyn);
    n = (*it)->nStormSynapses();
    for(int i = 0; i < n; i++){
      StormCluster::StormSynapse s = (*it)->getStormSynapse(i);
      if(m->getValue(s.centerX,s.centerY) == 0) continue;
      (*nsynStorm)++;
    }
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
      LocalizedObject::Point pt = (*jt)->center();
      if(m->getValue(pt.x,pt.y) == 0) continue;
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
  for(std::vector<Segment*>::iterator it = m_segments.begin(); it != m_segments.end(); it++) calculateSegmentStats(*it,postChan);
  int nROI = m_regions.size();
  int nSeg = m_segments.size();
  std::vector<Region*>::iterator rit;
  std::vector<Segment*>::iterator sit;
  std::ofstream fout(filename.c_str());
  fout << "Field,";
  for(int r = 0; r < nROI; r++) fout << "\"Region " << r << "\",";
  fout << "Average,";
  for(int r = 0; r < nSeg; r++) fout << "\"Segment " << r << "\",";
  fout << "Average,Description\n";
  for(int chan = 0; chan < m_nchannels; chan++){
    fout << "Channel " << chan << " threshold,";
    for(int r = 0; r < nROI; r++) fout << "-,";
    fout << "" << m_thresholds[chan] << ",";
    for(int r = 0; r < nSeg; r++) fout << "-,";
    fout << "-\n";
  }
  fout << "\"Neurite area (um^2)\",";
  double sum = 0;
  for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
    fout << "" << (*rit)->dendriteArea << ",";
    sum += (*rit)->dendriteArea;
  }
  fout << "" << (sum/nROI) << ",";
  sum = 0;
  for(sit = m_segments.begin(); sit != m_segments.end(); sit++){
    fout << "" << (*sit)->dendriteArea << ",";
    sum += (*sit)->dendriteArea;
  }
  fout << "" << (sum/nSeg) << ",-\n";
  
  fout << "\"Neurite length (um)\",";
  sum = 0;
  for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
    fout << "" << (*rit)->dendriteLength << ",";
    sum += (*rit)->dendriteLength;
  }
  fout << "" << (sum/nROI) << ",";
  for(int i = 0; i < nSeg; i++) fout << "-,";
  fout << "-,-\n-,";
  for(rit = m_regions.begin(); rit != m_regions.end(); rit++) fout << "-,";
  fout << "-,";
  for(int i = 0; i < nSeg; i++) fout << "-,";
  fout << "-,-\n";
  int icol = 0;
  for(std::vector<SynapseCollection*>::iterator scit = m_synapseCollections.begin(); scit != m_synapseCollections.end(); scit++){
    fout << "\"Type " << icol << " synapses\",";
    sum = 0;
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
      fout << "" << (*rit)->nSynapses[icol] << ",";
      sum += (*rit)->nSynapses[icol];
    }
    fout << "" << (sum/nROI) << ",";
    sum = 0;
    for(sit = m_segments.begin(); sit != m_segments.end(); sit++){
      fout << "" << (*sit)->nSynapses[icol] << ",";
      sum += (*sit)->nSynapses[icol];
    }
    fout << "" << (sum/nSeg) << ",\"" << (*scit)->description() << "\"\n\"Density (per 100 um^2)\",";

    sum = 0;
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
      double densityA = 100 * (*rit)->nSynapses[icol]/(*rit)->dendriteArea;
      fout << "" << densityA << ",";
      sum += densityA;
    }
    fout << "" << (sum/nROI) << ",";
    sum = 0;
    for(sit = m_segments.begin(); sit != m_segments.end(); sit++){
      double densityA = 100 * (*sit)->nSynapses[icol]/(*sit)->dendriteArea;
      fout << "" << densityA << ",";
      sum += densityA;
    }
    fout << "" << (sum/nSeg) << ",-\n\"Average size (um^2)\",";
    
    sum = 0;
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
      fout << "" << (*rit)->avgSynapseSize[icol] << ",";
      sum += (*rit)->avgSynapseSize[icol];
    }
    fout << "" << (sum/nROI) << ",";
    sum = 0;
    for(sit = m_segments.begin(); sit != m_segments.end(); sit++){
      fout << "" << (*sit)->avgSynapseSize[icol] << ",";
      sum += (*sit)->avgSynapseSize[icol];
    }
    fout << "" << (sum/nSeg) << ",-\n";
    
    int nreq = (*scit)->nRequirements();
    for(int i = 0; i < nreq; i++){
      fout << "\"Average overlap " << i << " (um^2)\",";
      sum = 0;
      for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
	fout << "" << (*rit)->avgOverlap[icol][i] << ",";
	sum += (*rit)->avgOverlap[icol][i];
      }
      fout << "" << (sum/nROI) << ",";
      sum = 0;
      for(sit = m_segments.begin(); sit != m_segments.end(); sit++){
	fout << "" << (*sit)->avgOverlap[icol][i] << ",";
	sum += (*sit)->avgOverlap[icol][i];
      }
      fout << "" << (sum/nSeg) << ",";
      if((*scit)->allRequired()) fout << "-\n";
      else{
	std::vector<int> req = (*scit)->getRequiredColocalization(i);
	fout << "\"" << m_channelNames[req[0]];
	for(std::vector<int>::iterator it = req.begin()+1; it != req.end(); it++) fout << " and " << m_channelNames[*it];
	fout << "\"\n";
      }
    }
    fout << "-,";
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++) fout << "-,";
    fout << "-,";
    for(sit = m_segments.begin(); sit != m_segments.end(); sit++) fout << "-,";
    fout << "-,-\n";
    
    fout << "\"Storm synapses\",";
    sum = 0;
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
      fout << "" << (*rit)->nStormSynapses[icol] << ",";
      sum += (*rit)->nStormSynapses[icol];
    }
    fout << "" << (sum/nROI) << ",";
    sum = 0;
    for(sit = m_segments.begin(); sit != m_segments.end(); sit++){
      fout << "" << (*sit)->nStormSynapses[icol] << ",";
      sum += (*sit)->nStormSynapses[icol];
    }
    fout << "" << (sum/nSeg) << ",\"" << (*scit)->description() << "\"\n";
    
    fout << "-,";
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++) fout << "-,";
    fout << "-,";
    for(sit = m_segments.begin(); sit != m_segments.end(); sit++) fout << "-,";
    fout << "-,-\n";
    icol++;
  }
  for(int chan = 0; chan < m_nchannels; chan++){
    fout << "\"" << m_channelNames[chan] << " puncta\",";
    sum = 0;
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
      fout << "" << (*rit)->nPuncta[chan] << ",";
      sum += (*rit)->nPuncta[chan];
    }
    fout << "" << (sum/nROI) << ",";
    sum = 0;
    for(sit = m_segments.begin(); sit != m_segments.end(); sit++){
      fout << "" << (*sit)->nPuncta[chan] << ",";
      sum += (*sit)->nPuncta[chan];
    }
    fout << "" << (sum/nSeg) << ",-\n\"Density (per 100 um^2)\",";
    
    sum = 0;
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
      double densityA = 100 * (*rit)->nPuncta[chan]/(*rit)->dendriteArea;
      fout << "" << densityA << ",";
      sum += densityA;
    }
    fout << "" << (sum/nROI) << ",";
    sum = 0;
    for(sit = m_segments.begin(); sit != m_segments.end(); sit++){
      double densityA = 100 * (*sit)->nPuncta[chan]/(*sit)->dendriteArea;
      fout << "" << densityA << ",";
      sum += densityA;
    }
    fout << "" << (sum/nSeg) << ",-\n\"Average size (um^2)\",";
    
    sum = 0;
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
      fout << "" << (*rit)->avgPunctaSize[chan] << ",";
      sum += (*rit)->avgPunctaSize[chan];
    }
    fout << "" << (sum/nROI) << ",";
    sum = 0;
    for(sit = m_segments.begin(); sit != m_segments.end(); sit++){
      fout << "" << (*sit)->avgPunctaSize[chan] << ",";
      sum += (*sit)->avgPunctaSize[chan];
    }
    fout << "" << (sum/nSeg) << ",-\n\"Average peak intensity\",";
    
    sum = 0;
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
      fout << "" << (*rit)->avgPeakIntensity[chan] << ",";
      sum += (*rit)->avgPeakIntensity[chan];
    }
    fout << "" << (sum/nROI) << ",";
    sum = 0;
    for(sit = m_segments.begin(); sit != m_segments.end(); sit++){
      fout << "" << (*sit)->avgPeakIntensity[chan] << ",";
      sum += (*sit)->avgPeakIntensity[chan];
    }
    fout << "" << (sum/nSeg) << ",-\n\"Average integrated intensity\",";
    
    sum = 0;
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
      fout << "" << (*rit)->avgIntegratedIntensity[chan] << ",";
      sum += (*rit)->avgIntegratedIntensity[chan];
    }
    fout << "" << (sum/nROI) << ",";
    sum = 0;
    for(sit = m_segments.begin(); sit != m_segments.end(); sit++){
      fout << "" << (*sit)->avgIntegratedIntensity[chan] << ",";
      sum += (*sit)->avgIntegratedIntensity[chan];
    }
    fout << "" << (sum/nSeg) << ",-\n";
    
    fout << "-,";
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++) fout << "-,";
    fout << "-,";
    for(sit = m_segments.begin(); sit != m_segments.end(); sit++) fout << "-,";
    fout << "-,-\n";
  }
  fout.close();
}

void ImRecord::write(std::ofstream& fout, int version)
{
  char* buf = new char[400000];
  for(int i = 0; i < 400000; i++) buf[i] = 0;
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
    if(m_signalMasks[chan]){
      buf[0] = (char)1;
      offset = 1;
      fout.write(buf,1);
      int npacks = (m_imHeight / 8);
      if(m_imHeight % 8 > 0) npacks++;
      for(int i = 0; i < npacks; i++){
	int startY = 8*i;
	offset = pack(buf,m_signalMasks[chan],startY);
	fout.write(buf,offset);
      }
    }
    else{
      buf[0] = (char)0;
      fout.write(buf,1);
    }
    
    NiaUtils::writeIntToBuffer(buf,0,nPuncta(chan));
    fout.write(buf,4);
    for(std::vector<Cluster*>::iterator it = m_puncta[chan].begin(); it != m_puncta[chan].end(); it++){
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

  if(version > 1){
    NiaUtils::writeIntToBuffer(buf,0,m_segments.size());
    fout.write(buf,4);
    for(std::vector<Segment*>::iterator sit = m_segments.begin(); sit != m_segments.end(); sit++) (*sit)->write(buf,fout);
  }

  for(int i = 0; i < m_nchannels; i++){
    NiaUtils::writeIntToBuffer(buf,0,m_stormClusters[i].size());
    fout.write(buf,4);
    for(std::vector<StormCluster*>::iterator scit = m_stormClusters[i].begin(); scit != m_stormClusters[i].end(); scit++) (*scit)->write(buf,fout);
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
    m_utilityMasks.push_back(NULL);

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
    if(version < 4) m_stormClusters.push_back(std::vector<StormCluster*>());
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

  if(version > 1){
    fin.read(buf,4);
    int nSeg = NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]);
    for(int i = 0; i < nSeg; i++){
      Segment* s = new Segment();
      s->read(buf,fin);
      m_segments.push_back(s);
    }
  }

  if(version > 3){
    for(int i = 0; i < m_nchannels; i++){
      std::vector<StormCluster*> clusters;
      fin.read(buf,4);
      int nclusters = NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]);
      for(int i = 0; i < nclusters; i++){
	StormCluster* s = new StormCluster();
	s->read(buf,fin,version);
	clusters.push_back(s);
      }
      m_stormClusters.push_back(clusters);
    }
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
  fin.close();
}

void ImRecord::loadMetaMorphTraces(std::string filename, int chan, ImFrame* frame, bool overwrite)
{
  std::string line;
  std::ifstream fin(filename);
  if(!m_utilityMasks[chan]){
    m_utilityMasks[chan] = new Mask(m_imWidth,m_imHeight);
  }
  else m_utilityMasks[chan]->clear(0,m_imWidth,0,m_imHeight);
  clearPuncta(chan);
  while(getline(fin,line)){
    boost::char_separator<char> sep(",");
    boost::tokenizer< boost::char_separator<char> > tokens(line,sep);
    boost::tokenizer< boost::char_separator<char> >::iterator it = tokens.begin();
    for(int i = 0; i < 6; i++) it++;
    boost::char_separator<char> sep2(" ");
    boost::tokenizer< boost::char_separator<char> > tokens2(*it,sep2);
    boost::tokenizer< boost::char_separator<char> >::iterator it2 = tokens2.begin();
    if(boost::lexical_cast<int>(*it2) != 6){
      std::cout << "Format error: didn't find list of points where expected" << std::endl;
      return;
    }
    it2++;
    int npoints = boost::lexical_cast<int>(*it2);
    it2++;
    std::vector<LocalizedObject::Point> border;
    Cluster* c = new Cluster();
    int xmax = 0;
    int xmin = m_imWidth;
    int ymax = 0;
    int ymin = m_imHeight;
    while(it2 != tokens2.end()){
      LocalizedObject::Point pt;
      pt.x = boost::lexical_cast<int>(*it2);
      if(pt.x < xmin) xmin = pt.x;
      if(pt.x > xmax) xmax = pt.x;
      it2++;
      pt.y = boost::lexical_cast<int>(*it2);
      if(pt.y < ymin) ymin = pt.y;
      if(pt.y > ymax) ymax = pt.y;
      m_utilityMasks[chan]->setValue(pt.x,pt.y,1);
      c->addPoint(pt,frame->getPixel(pt.x,pt.y));
      border.push_back(pt);
      it2++;
    }
    for(int x = xmin; x < xmax; x++){
      for(int y = ymin; y < ymax; y++){
	if(m_utilityMasks[chan]->getValue(x,y) > 0) continue;
	bool inside = false;
	for(std::vector<LocalizedObject::Point>::iterator it = border.begin(); it != border.end()-1; it++){
	  std::vector<LocalizedObject::Point>::iterator jt = it + 1;
	  if( ((it->y > y) != (jt->y > y)) && (x < it->x + (double(jt->x) - it->x)*(double(y) - it->y)/(double(jt->y) - it->y)) ) inside = !inside;
	}
	std::vector<LocalizedObject::Point>::iterator it = border.end()-1;
	std::vector<LocalizedObject::Point>::iterator jt = border.begin();
	if( ((it->y > y) != (jt->y > y)) && (x < it->x + (double(jt->x) - it->x)*(double(y) - it->y)/(double(jt->y) - it->y)) ) inside = !inside;
	if(inside) c->addPoint(x,y,frame->getPixel(x,y));
      }
    }
    addPunctum(chan,c);
  }
  fin.close();
}

void ImRecord::loadPunctaAnalyzerPuncta(std::string filename, int chan, ImFrame* frame, bool overwrite)
{
  std::ifstream fin(filename.c_str(),std::ifstream::binary);
  char* buf = new char[42000000];
  fin.read(buf,4);
  if(NiaUtils::convertToShort(buf[3],buf[2]) != 42){
    std::cout << "ERROR: Unexpected byte order" << std::endl;
    delete[] buf;
    return;
  }
  fin.read(buf,4);
  uint32_t offset = NiaUtils::convertToInt(buf[3],buf[2],buf[1],buf[0]);
  fin.seekg(offset);
  fin.read(buf,2);
  uint16_t nTags = NiaUtils::convertToShort(buf[1],buf[0]);
  uint32_t width = 1;
  uint32_t height = 1;
  uint32_t nStrips = 0;
  uint32_t stripOffsets = 0;
  uint32_t stripByteCountOffsets = 0;
  for(uint16_t i = 0; i < nTags; i++){
    fin.read(buf,12);
    uint16_t tag = NiaUtils::convertToShort(buf[1],buf[0]);
    uint16_t type = NiaUtils::convertToShort(buf[3],buf[2]);
    if(tag == 273){
      nStrips = NiaUtils::convertToInt(buf[7],buf[6],buf[5],buf[4]);
      if(type == 4) stripOffsets = NiaUtils::convertToInt(buf[11],buf[10],buf[9],buf[8]);
      else if(type == 3) stripOffsets = NiaUtils::convertToShort(buf[9],buf[8]);
      else std::cout << "ERROR: Unknown type for strip offsets\n";
    }
    else if(tag == 279){
      if(type == 4) stripByteCountOffsets = NiaUtils::convertToInt(buf[11],buf[10],buf[9],buf[8]);
      else if(type == 3) stripByteCountOffsets = NiaUtils::convertToShort(buf[9],buf[8]);
      else std::cout << "ERROR: Unknown type for strip byte count offsets\n";
    }
    else if(tag == 256) width = NiaUtils::convertToInt(buf[11],buf[10],buf[9],buf[8]);
    else if(tag == 257) height = NiaUtils::convertToInt(buf[11],buf[10],buf[9],buf[8]);
  }
  ImFrame* pam = new ImFrame(width,height);
  uint32_t index = 0;
  if(nStrips == 1){
    uint32_t nbytes = stripByteCountOffsets;
    fin.seekg(stripOffsets);
    fin.read(buf,nbytes);
    for(uint32_t pixel = 0; pixel < nbytes; pixel++){
      uint32_t y = index / width;
      uint32_t x = index - width*y;
      pam->setPixel(x,y,(uint16_t)buf[pixel]);
      index++;
    }
  }
  else{
    for(uint32_t i = 0; i < nStrips; i++){
      fin.seekg(stripByteCountOffsets + 4*i);
      fin.read(buf,4);
      uint32_t nbytes = NiaUtils::convertToInt(buf[3],buf[2],buf[1],buf[0]);
      fin.seekg(stripOffsets + 4*i);
      fin.read(buf,4);
      uint32_t stripOff = NiaUtils::convertToInt(buf[3],buf[2],buf[1],buf[0]);
      fin.seekg(stripOff);
      fin.read(buf,nbytes);
      for(uint32_t pixel = 0; pixel < nbytes; pixel++){
	uint32_t y = index / width;
	uint32_t x = index - width*y;
	pam->setPixel(x,y,(uint16_t)buf[pixel]);
	index++;
      }
    }
  }
  delete[] buf;
  fin.close();
  if(pam->width() != m_imWidth || pam->height() != m_imHeight){
    std::cout << "Error: size mismatch" << std::endl;
    return;
  }
  if(!m_utilityMasks[chan]){
    m_utilityMasks[chan] = new Mask(m_imWidth,m_imHeight);
  }
  else m_utilityMasks[chan]->clear(0,m_imWidth,0,m_imHeight);
  for(int x = 0; x < m_imWidth; x++){
    for(int y = 0; y < m_imHeight; y++){
      if(pam->getPixel(x,y) > 0) m_utilityMasks[chan]->setValue(x,y,1);
    }
  }
  clearPuncta(chan);
  Mask* subMask = m_utilityMasks[chan]->getCopy();
  std::vector<int> borderX,borderY;
  for(int i = 0; i < frame->width(); i++){
    for(int j = 0; j < frame->height(); j++){
      if(subMask->getValue(i,j) != 1) continue;
      borderX.push_back(i);
      borderY.push_back(j);
      subMask->setValue(i,j,0);
      Cluster* c = new Cluster();
      c->addPoint(i,j,frame->getPixel(i,j));
      while(borderX.size() > 0){
	int bi = borderX.at(0);
	int bj = borderY.at(0);
	int left = bi-1;
	int right = bi+2;
	int top = bj-1;
	int bottom = bj+2;
	if(left < 0) left++;
	if(right >= frame->width()) right--;
	if(top < 0) top++;
	if(bottom >= frame->height()) bottom--;
	for(int di = left; di < right; di++){
	  for(int dj = top; dj < bottom; dj++){
	    if(m_utilityMasks[chan]->getValue(di,dj) > 0 && subMask->getValue(di,dj) > 0){
	      subMask->setValue(di,dj,0);
	      borderX.push_back(di);
	      borderY.push_back(dj);
	      c->addPoint(di,dj,frame->getPixel(di,dj));
	    }
	  }
	}
	borderX.erase(borderX.begin());
	borderY.erase(borderY.begin());
      }
      addPunctum(chan,c);
    }
  }
  delete pam;
  delete subMask;
}

void ImRecord::setStormClusters(int chan, std::vector<StormCluster*> clusters)
{
  for(std::vector<StormCluster*>::iterator clit = m_stormClusters[chan].begin(); clit != m_stormClusters[chan].end(); clit++){
    if(*clit) delete *clit;
  }
  m_stormClusters[chan] = clusters;
}

void ImRecord::shiftStormData(int shiftX_pix, int shiftY_pix, double scale)
{
  double shiftX = shiftX_pix * 1000.0 * m_resolutionXY;
  double shiftY = shiftY_pix * 1000.0 * m_resolutionXY;
  for(int chan = 0; chan < m_nchannels; chan++){
    for(std::vector<StormCluster*>::iterator clit = m_stormClusters[chan].begin(); clit != m_stormClusters[chan].end(); clit++){
      (*clit)->shiftXY(shiftX,shiftY,scale);
    }
  }
}

std::vector<LocalizedObject::Point> ImRecord::getStormClusterCenters(int chan)
{
  Mask* m = new Mask(m_imWidth,m_imHeight,0);
  double step = m_resolutionXY / 2.0;
  std::vector<LocalizedObject::Point> retVal;
  LocalizedObject::Point pt;
  for(std::vector<StormCluster*>::iterator clit = m_stormClusters[chan].begin(); clit != m_stormClusters[chan].end(); clit++){
    pt.x = (int)((*clit)->centerX() / (1000.0 * m_resolutionXY));
    pt.y = (int)((*clit)->centerY() / (1000.0 * m_resolutionXY));
    retVal.push_back(pt);
  }
  return retVal;
}

Mask* ImRecord::getStormClusterMask(int chan)
{
  Mask* m = new Mask(m_imWidth,m_imHeight,0);
  double step = m_resolutionXY / 2.0;
  for(std::vector<StormCluster*>::iterator clit = m_stormClusters[chan].begin(); clit != m_stormClusters[chan].end(); clit++){
    int c_x = (int)((*clit)->centerX() / (1000.0 * m_resolutionXY));
    int c_y = (int)((*clit)->centerY() / (1000.0 * m_resolutionXY));
    if(c_x < 0 || c_y < 0) continue;
    if(c_x >= m_imWidth || c_y >= m_imHeight) continue;
    m->setValue(c_x,c_y,1);
    double threshold = 0.05 * (*clit)->intensity();
    int window = (int)(1.0 / m_resolutionXY);
    int bx = c_x - window;
    if(bx < 0) bx = 0;
    int ex = c_x + window;
    if(ex >= m_imWidth) ex = m_imWidth;
    int by = c_y - window;
    if(by < 0) by = 0;
    int ey = c_y + window;
    if(ey >= m_imHeight) ey = m_imHeight;
    for(int i = bx; i < ex; i++){
      for(int j = by; j < ey; j++){
	double intensity = 0.0;
	double px = (i+0.5) * m_resolutionXY * 1000.0;
	double py = (j+0.5) * m_resolutionXY * 1000.0;
	for(int k = 0; k < (*clit)->nMolecules(); k++){
	  double diffx = px - (*clit)->x(k);
	  double diffy = py - (*clit)->y(k);
	  double dist = sqrt(diffx*diffx + diffy*diffy);
	  double width = (*clit)->sigma1(k);
	  intensity += (*clit)->intensity(k) *  (m_resolutionXY / (6.2831853*dist)) * (exp(-(dist-step)*(dist-step)/(2*width*width)) - exp(-(dist+step)*(dist-step)/(2*width*width)));
	  if(intensity > threshold){
	    m->setValue(i,j,1);
	    break;
	  }
	}
      }
    }
  }
  return m;
}

Mask* ImRecord::getStormClusterLocations(int chan)
{
  Mask* m = new Mask(m_imWidth,m_imHeight,0);
  for(std::vector<StormCluster*>::iterator clit = m_stormClusters[chan].begin(); clit != m_stormClusters[chan].end(); clit++){
    int c_x = (int)((*clit)->centerX() / (1000.0 * m_resolutionXY));
    int c_y = (int)((*clit)->centerY() / (1000.0 * m_resolutionXY));
    if(c_x < 0 || c_y < 0) continue;
    if(c_x >= m_imWidth || c_y >= m_imHeight) continue;
    int window = 1;
    int bx = c_x - window;
    if(bx < 0) bx = 0;
    int ex = c_x + window + 1;
    if(ex >= m_imWidth) ex = m_imWidth;
    int by = c_y - window;
    if(by < 0) by = 0;
    int ey = c_y + window + 1;
    if(ey >= m_imHeight) ey = m_imHeight;
    for(int i = bx; i < ex; i++) m->setValue(i,c_y,1);
    for(int i = by; i < ey; i++) m->setValue(c_x,i,1);
  }
  return m;
}

bool ImRecord::selectStormCluster(double x, double y)
{
  double minDist = 999999.9;
  StormCluster* closest = NULL;
  for(std::vector< std::vector<StormCluster*> >::iterator chit = m_stormClusters.begin(); chit != m_stormClusters.end(); chit++){
    for(std::vector<StormCluster*>::iterator clit = chit->begin(); clit != chit->end(); clit++){
      double xdiff = (*clit)->centerX() - x;
      double ydiff = (*clit)->centerY() - y;
      double dist = sqrt(xdiff*xdiff + ydiff*ydiff);
      if(dist < minDist){
	minDist = dist;
	closest = *clit;
      }
    }
  }
  if(closest){
    closest->toggleSelected();
    return true;
  }
  return false;
}

bool ImRecord::selectStormCluster(int chan, double x, double y)
{
  double minDist = 999999.9;
  StormCluster* closest = NULL;
  std::vector<StormCluster*> clusters = m_stormClusters[chan];
  for(std::vector<StormCluster*>::iterator clit = clusters.begin(); clit != clusters.end(); clit++){
    double xdiff = (*clit)->centerX() - x;
    double ydiff = (*clit)->centerY() - y;
    double dist = sqrt(xdiff*xdiff + ydiff*ydiff);
    if(dist < minDist){
      minDist = dist;
      closest = *clit;
    }
  }
  if(closest){
    closest->toggleSelected();
    return true;
  }
  return false;
}

bool ImRecord::selectStormSynapse(double x, double y)
{
  double minDist = 999999.9;
  StormCluster::StormSynapse closest;
  closest.centerX = -1;
  for(std::vector<SynapseCollection*>::iterator it = m_synapseCollections.begin(); it != m_synapseCollections.end(); it++){
    int n = (*it)->nStormSynapses();
    for(int i = 0; i < n; i++){
      StormCluster::StormSynapse s = (*it)->getStormSynapse(i);
      double xdiff = s.centerX - x;
      double ydiff = s.centerY - y;
      double dist = sqrt(xdiff*xdiff + ydiff*ydiff);
      if(dist < minDist){
	minDist = dist;
	closest = s;
      }
    }
  }
  if(closest.centerX > 0){
    for(std::vector<StormCluster*>::iterator clit = closest.clusters.begin(); clit != closest.clusters.end(); clit++)
      (*clit)->toggleSelected();
    return true;
  }
  return false;
}

double ImRecord::getBlinksPerCount(int chan, ImFrame* frame, StormData* sd)
{
  double scale = 1.12;
  int shiftX = 17;//20;
  int shiftY = 14;//-2;
  std::vector< std::vector<int> > nBlinks;
  nBlinks.assign(m_imWidth, std::vector<int>(m_imHeight,0));
  std::vector< std::vector<int> > nBlinks3;
  nBlinks3.assign(m_imWidth, std::vector<int>(m_imHeight,0));
  for(int i = 0; i < sd->nMolecules(); i++){
    StormData::Blink b = sd->molecule(i);
    int x = (b.x / (1000.0 * m_resolutionXY)) + shiftX;
    int y = (b.y * scale / (1000.0 * m_resolutionXY)) + shiftY;
    if(x < 0 || x >= m_imWidth) continue;
    if(y < 0 || y >= m_imHeight) continue;
    nBlinks3[x][y]++;
    for(int dx = x-1; dx < x+2; dx++){
      if(dx < 0 || dx >= m_imWidth) continue;
      for(int dy = y-1; dy < y+2; dy++){
	if(dy < 0 || dy >= m_imHeight) continue;
	nBlinks[dx][dy]++;
      }
    }
  }
  Mask* m = getPunctaMask(chan);
  double bpc = 0.0;
  double bpc3 = 0.0;
  int n = 0;
  int count = 0;
  for(int i = 0; i < m_imWidth; i++){
    for(int j = 0; j < m_imHeight; j++){
      if(nBlinks[i][j] > 0) count += frame->getPixel(i,j);
      if(m->getValue(i,j) < 1) continue;
      bpc += nBlinks[i][j]*1.0 / frame->getPixel(i,j);
      bpc3 += nBlinks3[i][j]*1.0 / frame->getPixel(i,j);
      n++;
    }
  }
  bpc = bpc / n;
  bpc3 = bpc3 / n;
  double bpc2 = sd->nMolecules() * 1.0 / count;
  std::cout << "Blinks per count: " << bpc << ", " << bpc2 << ", " << bpc3 << std::endl;
  delete m;
  return bpc3;
}
