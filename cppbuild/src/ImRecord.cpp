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

void ImRecord::write(std::ofstream& fout)
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
      NiaUtils::writeShortToBuffer(buf,0,(*it)->size());
      NiaUtils::writeShortToBuffer(buf,2,(*it)->peak());
      NiaUtils::writeIntToBuffer(buf,4,(*it)->integratedIntensity());
      std::vector<LocalizedObject::Point> pts = (*it)->getPoints();
      offset = 8;
      for(std::vector<LocalizedObject::Point>::iterator jt = pts.begin(); jt != pts.end(); jt++){
	NiaUtils::writeShortToBuffer(buf,offset,jt->x);
	NiaUtils::writeShortToBuffer(buf,offset+2,jt->y);
	offset += 4;
      }
      fout.write(buf,offset);
    }
  }
  buf[0] = (char)m_synapseCollections.size();
  fout.write(buf,1);
  for(std::vector<SynapseCollection*>::iterator it = m_synapseCollections.begin(); it != m_synapseCollections.end(); it++){
    std::string d = (*it)->description();
    offset = 0;
    NiaUtils::writeShortToBuffer(buf,offset,d.length());
    offset += 2;
    for(int i = 0; i < d.length(); i++){
      buf[offset] = d[i];
      offset++;
    }
    buf[offset] = (char)(*it)->nChannels();
    offset++;
    std::vector<int> chans = (*it)->channels();
    for(std::vector<int>::iterator jt = chans.begin(); jt != chans.end(); jt++){
      buf[offset] = (char)(*jt);
      offset++;
    }
    NiaUtils::writeIntToBuffer(buf,offset,(*it)->nSynapses());
    offset += 4;
    std::vector<Synapse*> vec = (*it)->synapses();
    for(std::vector<Synapse*>::iterator jt = vec.begin(); jt != vec.end(); jt++){
    NiaUtils::writeDoubleToBuffer(buf,offset,(*jt)->colocalizationScore());
      offset += 4;
      for(int i = 0; i < (*it)->nChannels(); i++){
	NiaUtils::writeIntToBuffer(buf,offset,(*jt)->getPunctumIndex(i));
	offset += 4;
      }
    }
    NiaUtils::writeIntToBuffer(buf,offset,(*it)->overlapThreshold());
    offset += 4;
    NiaUtils::writeDoubleToBuffer(buf,offset,(*it)->distanceThreshold());
    offset += 4;
    buf[offset] = (char)(*it)->useOverlap();
    offset++;
    if((*it)->allRequired()){
      buf[offset] = 1;
      offset++;
    }
    else{
      buf[offset] = 0;
      buf[offset+1] = (char)(*it)->nRequirements();
      offset += 2;
      for(int i = 0; i < (*it)->nRequirements(); i++){
	std::vector<int> reqs = (*it)->getRequiredColocalizationByIndex(i);
	buf[offset] = (char)reqs.size();
	offset++;
	for(std::vector<int>::iterator jt = reqs.begin(); jt != reqs.end(); jt++){
	  buf[offset] = (char)(*jt);
	  offset++;
	}
      }
    }
    fout.write(buf,offset);
  }
  buf[0] = (char)nRegions();
  offset = 1;
  for(std::vector<Region*>::iterator it = m_regions.begin(); it != m_regions.end(); it++){
    buf[offset] = (char)(*it)->nVertices();
    offset++;
    std::vector<LocalizedObject::Point> verts = (*it)->vertices();
    for(std::vector<LocalizedObject::Point>::iterator jt = verts.begin(); jt != verts.end(); jt++){
      NiaUtils::writeShortToBuffer(buf,offset,jt->x);
      NiaUtils::writeShortToBuffer(buf,offset+2,jt->y);
      offset += 4;
    }
  }
  fout.write(buf,offset);
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

void ImRecord::read(std::ifstream& fin)
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
      fin.read(buf,8);
      int npix = NiaUtils::convertToShort(buf[0],buf[1]);
      c->setPeakIntensity(NiaUtils::convertToShort(buf[2],buf[3]));
      c->setIntegratedIntensity(NiaUtils::convertToInt(buf[4],buf[5],buf[6],buf[7]));
      fin.read(buf,4*npix);
      offset = 0;
      for(int j = 0; j < npix; j++){
	c->addPoint(NiaUtils::convertToShort(buf[offset],buf[offset+1]),NiaUtils::convertToShort(buf[offset+2],buf[offset+3]));
	offset += 4;
      }
      c->computeCenter();
      clusters.push_back(c);
    }
    m_puncta.push_back(clusters);
  }

  fin.read(buf,1);
  int ncol = (int)buf[0];
  for(int icol = 0; icol < ncol; icol++){
    fin.read(buf,2);
    int len = NiaUtils::convertToShort(buf[0],buf[1]);
    fin.read(buf,len+1);
    std::string d = "";
    d.append(buf,len);
    int nchan = (int)buf[len];
    fin.read(buf,nchan);
    std::vector<int> chans;
    for(int i = 0; i < nchan; i++) chans.push_back((int)buf[i]);
    SynapseCollection* sc = new SynapseCollection(chans);
    fin.read(buf,4);
    int nsyn = NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]);
    fin.read(buf,4*(nchan+1)*nsyn);
    offset = 0;
    for(int i = 0; i < nsyn; i++){
      Synapse* s = new Synapse();
      s->setColocalizationScore(NiaUtils::convertToDouble(buf[offset],buf[offset+1],buf[offset+2],buf[offset+3]));
      offset += 4;
      for(std::vector<int>::iterator it = chans.begin(); it != chans.end(); it++){
	int index = NiaUtils::convertToInt(buf[offset],buf[offset+1],buf[offset+2],buf[offset+3]);
	s->addPunctum(m_puncta.at(*it).at(index),index);
	offset += 4;
      }
      s->computeCenter();
      sc->addSynapse(s);
    }
    fin.read(buf,10);
    sc->setOverlapThreshold(NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]));
    sc->setDistanceThreshold(NiaUtils::convertToDouble(buf[4],buf[5],buf[6],buf[7]));
    sc->setUseOverlap((int)buf[8] > 0);
    sc->setRequireAll((int)buf[9] > 0);
    if(!sc->allRequired()){
      fin.read(buf,1);
      int nreqs = (int)buf[0];
      for(int i = 0; i < nreqs; i++){
	std::vector<int> reqs;
	fin.read(buf,1);
	int n = (int)buf[0];
	fin.read(buf,n);
	for(int j = 0; j < n; j++) reqs.push_back((int)buf[j]);
	sc->addRequiredColocalizationByIndex(reqs);
      }
    }
    m_synapseCollections.push_back(sc);
  }
  
  fin.read(buf,1);
  int nROI = (int)buf[0];
  for(int i = 0; i < nROI; i++){
    Region* r = new Region();
    fin.read(buf,1);
    int nVerts = (int)buf[0];
    fin.read(buf,4*nVerts);
    offset = 0;
    for(int j = 0; j < nVerts; j++){
      LocalizedObject::Point pt;
      pt.x = NiaUtils::convertToShort(buf[offset],buf[offset+1]);
      pt.y = NiaUtils::convertToShort(buf[offset+2],buf[offset+3]);
      r->addVertex(pt);
      offset += 4;
    }
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
