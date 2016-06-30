#include "ImRecord.hpp"

ImRecord::ImRecord(uint8_t nchan, uint16_t w, uint16_t h)
{
  m_nchannels = nchan;
  m_imWidth = w;
  m_imHeight = h;
  m_resolutionXY = 0.046;
  for(uint8_t i = 0; i < m_nchannels; i++){
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

void ImRecord::removePunctum(uint8_t chan, uint32_t index)
{
  std::vector<Cluster*> clusters = m_puncta.at(chan);
  delete clusters.at(index);
  clusters.erase(clusters.begin()+index);
}

void ImRecord::clearPuncta(uint8_t chan)
{
    std::vector<Cluster*> clusters = m_puncta.at(chan);
    for(std::vector<Cluster*>::iterator it = clusters.begin(); it != clusters.end(); it++) delete *it;
    clusters.clear();
}

void ImRecord::removeSynapseCollection(uint8_t index)
{
  delete m_synapseCollections.at(index);
  m_synapseCollections.erase(m_synapseCollections.begin()+index);
}

Cluster* ImRecord::selectPunctum(LocalizedObject::Point pt)
{
  double minDist = 999999.9;
  Cluster* closest = NULL;
  for(std::vector< std::vector<Cluster*> >::iterator it = m_puncta.begin(); it != m_puncta.end(); it++){
    for(std::vector<Cluster*>::iterator jt = it->begin(); jt != it->end(); jt++){
      double dist = (*jt)->distanceTo(pt);
      if(dist > minDist){
	minDist = dist;
	closest = *jt;
      }
    }
  }
  return closest;
}

Cluster* ImRecord::selectPunctum(uint8_t chan, LocalizedObject::Point pt)
{
  double minDist = 999999.9;
  Cluster* closest = NULL;
  std::vector<Cluster*> clusters = m_puncta.at(chan);
  for(std::vector<Cluster*>::iterator jt = clusters.begin(); jt != clusters.end(); jt++){
    double dist = (*jt)->distanceTo(pt);
    if(dist > minDist){
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
    uint32_t n = (*it)->nSynapses();
    for(uint32_t i = 0; i < n; i++){
      Synapse* s = (*it)->getSynapse(i);
      double dist = s->distanceTo(pt);
      if(dist > minDist){
	minDist = dist;
	closest = s;
      }
    }
  }
  return closest;
}

Synapse* ImRecord::selectSynapseFromCollection(uint8_t index, LocalizedObject::Point pt)
{
  double minDist = 999999.9;
  Synapse* closest = NULL;
  SynapseCollection* sc = m_synapseCollections.at(index);
  uint32_t n = sc->nSynapses();
  for(uint32_t i = 0; i < n; i++){
    Synapse* s = sc->getSynapse(i);
    double dist = s->distanceTo(pt);
    if(dist > minDist){
      minDist = dist;
      closest = s;
    }
  }
  return closest;
}

Mask* ImRecord::getPunctaMask(uint8_t chan, bool outline)
{
  Mask* m = new Mask(m_imWidth,m_imHeight);
  if(outline){
    for(std::vector< std::vector<Cluster*> >::iterator it = m_puncta.begin(); it != m_puncta.end(); it++){
      for(std::vector<Cluster*>::iterator jt = it->begin(); jt != it->end(); jt++){
	std::vector<LocalizedObject::Point> pts = (*jt)->getPoints();
	for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,2);
	for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++){
	  uint8_t sum = 0;
	  for(uint16_t dx = kt->x - 1; dx < kt->x + 2; dx++){
	    if(dx >= m_imWidth){
	      sum += 6;
	      continue;
	    }
	    for(uint16_t dy = kt->y - 1; dy < kt->y + 2; dy++){
	      if(dy >= m_imHeight) sum += 2;
	      else sum += m->getValue(dx,dy);
	    }
	  }
	  m->setValue(kt->x,kt->y,sum/18);
	}
      }
    }
    return m;
  }
  for(std::vector< std::vector<Cluster*> >::iterator it = m_puncta.begin(); it != m_puncta.end(); it++){
    for(std::vector<Cluster*>::iterator jt = it->begin(); jt != it->end(); jt++){
      std::vector<LocalizedObject::Point> pts = (*jt)->getPoints();
      for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,1);
    }
  }
  return m;
}

Mask* ImRecord::getSynapseMask(bool outline)
{
  Mask* m = new Mask(m_imWidth,m_imHeight);
  if(outline){
    for(std::vector<SynapseCollection*>::iterator it = m_synapseCollections.begin(); it != m_synapseCollections.end(); it++){
      uint32_t nsyns = (*it)->nSynapses();
      for(uint32_t isyn = 0; isyn < nsyns; isyn++){
	std::vector<LocalizedObject::Point> pts = (*it)->getSynapse(isyn)->getPoints();
	for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,2);
	for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++){
	  uint8_t sum = 0;
	  for(uint16_t dx = kt->x - 1; dx < kt->x + 2; dx++){
	    if(dx >= m_imWidth){
	      sum += 6;
	      continue;
	    }
	    for(uint16_t dy = kt->y - 1; dy < kt->y + 2; dy++){
	      if(dy >= m_imHeight) sum += 2;
	      else sum += m->getValue(dx,dy);
	    }
	  }
	  m->setValue(kt->x,kt->y,sum/18);
	}
      }
    }
    return m;
  }
  for(std::vector<SynapseCollection*>::iterator it = m_synapseCollections.begin(); it != m_synapseCollections.end(); it++){
    uint32_t nsyns = (*it)->nSynapses();
    for(uint32_t isyn = 0; isyn < nsyns; isyn++){
      std::vector<LocalizedObject::Point> pts = (*it)->getSynapse(isyn)->getPoints();
      for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,1);
    }
  }
  return m;
}

Mask* ImRecord::getSynapseMaskFromCollection(uint8_t index, bool outline)
{
  Mask* m = new Mask(m_imWidth,m_imHeight);
  if(outline){
    SynapseCollection* sc = m_synapseCollections.at(index);
    uint32_t nsyns = sc->nSynapses();
    for(uint32_t isyn = 0; isyn < nsyns; isyn++){
      std::vector<LocalizedObject::Point> pts = sc->getSynapse(isyn)->getPoints();
      for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,2);
      for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++){
	uint8_t sum = 0;
	for(uint16_t dx = kt->x - 1; dx < kt->x + 2; dx++){
	  if(dx >= m_imWidth){
	    sum += 6;
	    continue;
	  }
	  for(uint16_t dy = kt->y - 1; dy < kt->y + 2; dy++){
	    if(dy >= m_imHeight) sum += 2;
	    else sum += m->getValue(dx,dy);
	    }
	}
	m->setValue(kt->x,kt->y,sum/18);
      }
    }
    return m;
  }
  SynapseCollection* sc = m_synapseCollections.at(index);
  uint32_t nsyns = sc->nSynapses();
  for(uint32_t isyn = 0; isyn < nsyns; isyn++){
    std::vector<LocalizedObject::Point> pts = sc->getSynapse(isyn)->getPoints();
    for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,1);
  }
  return m;
}

Mask* ImRecord::getSynapticPunctaMask(uint8_t chan, bool outline)
{
  Mask* m = new Mask(m_imWidth,m_imHeight);
  if(outline){
    for(std::vector<SynapseCollection*>::iterator it = m_synapseCollections.begin(); it != m_synapseCollections.end(); it++){
      uint32_t nsyns = (*it)->nSynapses();
      for(uint32_t isyn = 0; isyn < nsyns; isyn++){
	std::vector<LocalizedObject::Point> pts = (*it)->getSynapse(isyn)->getPunctum((*it)->getChannelIndex(chan))->getPoints();
	for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,2);
	for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++){
	  uint8_t sum = 0;
	  for(uint16_t dx = kt->x - 1; dx < kt->x + 2; dx++){
	    if(dx >= m_imWidth){
	      sum += 6;
	      continue;
	    }
	    for(uint16_t dy = kt->y - 1; dy < kt->y + 2; dy++){
	      if(dy >= m_imHeight) sum += 2;
	      else sum += m->getValue(dx,dy);
	    }
	  }
	  m->setValue(kt->x,kt->y,sum/18);
	}
      }
    }
    return m;
  }
  for(std::vector<SynapseCollection*>::iterator it = m_synapseCollections.begin(); it != m_synapseCollections.end(); it++){
    uint32_t nsyns = (*it)->nSynapses();
    for(uint32_t isyn = 0; isyn < nsyns; isyn++){
      std::vector<LocalizedObject::Point> pts = (*it)->getSynapse(isyn)->getPunctum((*it)->getChannelIndex(chan))->getPoints();
      for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,1);
    }
  }
  return m;
}

Mask* ImRecord::getSynapticPunctaMaskFromCollection(uint8_t index, uint8_t chan, bool outline)
{
  Mask* m = new Mask(m_imWidth,m_imHeight);
  if(outline){
    SynapseCollection* sc = m_synapseCollections.at(index);
    uint32_t nsyns = sc->nSynapses();
    for(uint32_t isyn = 0; isyn < nsyns; isyn++){
      std::vector<LocalizedObject::Point> pts = sc->getSynapse(isyn)->getPunctum(sc->getChannelIndex(chan))->getPoints();
      for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,2);
      for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++){
	uint8_t sum = 0;
	for(uint16_t dx = kt->x - 1; dx < kt->x + 2; dx++){
	  if(dx >= m_imWidth){
	    sum += 6;
	    continue;
	  }
	  for(uint16_t dy = kt->y - 1; dy < kt->y + 2; dy++){
	    if(dy >= m_imHeight) sum += 2;
	    else sum += m->getValue(dx,dy);
	    }
	}
	m->setValue(kt->x,kt->y,sum/18);
      }
    }
    return m;
  }
  SynapseCollection* sc = m_synapseCollections.at(index);
  uint32_t nsyns = sc->nSynapses();
  for(uint32_t isyn = 0; isyn < nsyns; isyn++){
    std::vector<LocalizedObject::Point> pts = sc->getSynapse(isyn)->getPunctum(sc->getChannelIndex(chan))->getPoints();
    for(std::vector<LocalizedObject::Point>::iterator kt = pts.begin(); kt != pts.end(); kt++) m->setValue(kt->x,kt->y,1);
  }
  return m;
}

void ImRecord::calculateRegionStats(Region* r, uint8_t& postChan)
{
  r->dendriteArea = 0.0;
  uint16_t x1,x2,y1,y2;
  r->getEnclosure(x1,x2,y1,y2);
  Mask* m = m_signalMasks.at(postChan);
  for(uint16_t i = x1; i < x2; i++){
    for(uint16_t j = y1; j < y2; j++){
      if(m->getValue(i,j) > 0 && r->contains(i,j)) r->dendriteArea += 1.0;
    }
  }
  r->dendriteArea *= m_resolutionXY*m_resolutionXY;
  for(std::vector<SynapseCollection*>::iterator it = m_synapseCollections.begin(); it != m_synapseCollections.end(); it++){
    r->nSynapses.push_back(0);
    r->avgSynapseSize.push_back(0.0);
    r->avgOverlap.push_back(std::vector<double>());
    std::vector<uint32_t>::iterator nsyn = r->nSynapses.end()-1;
    std::vector<double>::iterator avgS = r->avgSynapseSize.end()-1;
    std::vector< std::vector<double> >::iterator avgO = r->avgOverlap.end()-1;
    uint8_t nreq = (*it)->nRequirements();
    avgO->assign(nreq,0.0);
    uint32_t n = (*it)->nSynapses();
    for(uint32_t i = 0; i < n; i++){
      Synapse* s = (*it)->getSynapse(i);
      if(!r->contains(s->center())) continue;
      (*nsyn)++;
      (*avgS) += s->size();
      if((*it)->allRequired()) avgO->at(0) = s->punctaOverlap();
      else{
	for(uint8_t ireq = 0; ireq < nreq; ireq++) avgO->at(ireq) += s->punctaOverlap((*it)->getRequiredColocalization(ireq));
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
    std::vector<uint32_t>::iterator npunc = r->nPuncta.end()-1;
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

void ImRecord::printSynapseDensityTable(uint8_t& postChan, std::vector<std::string> chanNames, std::string filename)
{
  for(std::vector<Region*>::iterator it = m_regions.begin(); it != m_regions.end(); it++) calculateRegionStats(*it,postChan);
  uint8_t nROI = m_regions.size();
  std::vector<Region*>::iterator rit;
  std::ofstream fout(filename.c_str());
  fout << "Field,";
  for(uint8_t r = 0; r < nROI; r++) fout << "\"Region " << r << "\",";
  fout << "Average,Description\n";
  for(uint8_t chan = 0; chan < m_nchannels; chan++){
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
  uint8_t icol = 0;
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
    fout << "" << (sum/nROI) << ",-\n\"Average size\",";
    sum = 0;
    for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
      fout << "" << (*rit)->avgSynapseSize.at(icol) << ",";
      sum += (*rit)->avgSynapseSize.at(icol);
    }
    fout << "" << (sum/nROI) << ",-\n";
    for(int i = 0; i < (*scit)->nRequirements(); i++){
      fout << "\"Average overlap " << i << "\",";
      sum = 0;
      for(rit = m_regions.begin(); rit != m_regions.end(); rit++){
	fout << "" << (*rit)->avgOverlap.at(icol).at(i) << ",";
	sum += (*rit)->avgOverlap.at(icol).at(i);
      }
      fout << "" << (sum/nROI) << ",";
      if((*scit)->allRequired()) fout << "-\n";
      else{
	std::vector<uint8_t> req = (*scit)->getRequiredColocalization(i);
	fout << "\"" << chanNames.at(req.at(0));
	for(std::vector<uint8_t>::iterator it = req.begin()+1; it != req.end(); it++) fout << " and " << chanNames.at(*it);
	fout << "\"\n";
      }
    }
    fout << "-,-,-,-,-,-\n";
    icol++;
  }
  for(int chan = 0; chan < m_nchannels; chan++){
    fout << "\"" << chanNames.at(chan) << " puncta\",";
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
    fout << "" << (sum/nROI) << ",-\n\"Average size\",";
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
    fout << "-,-,-,-,-,-\n";
  }
}

void ImRecord::write(std::ofstream& fout)
{
  char* buf = new char[400000];
  buf[0] = (char)m_nchannels;
  NiaUtils::writeShortToBuffer(buf,1,m_imWidth);
  NiaUtils::writeShortToBuffer(buf,3,m_imHeight);
  NiaUtils::writeDoubleToBuffer(buf,5,m_resolutionXY);
  uint64_t offset = 9;
  for(std::vector<uint16_t>::iterator it = m_thresholds.begin(); it != m_thresholds.end(); it++){
    NiaUtils::writeShortToBuffer(buf,offset,*it);
    offset += 2;
  }
  fout.write(buf,offset);
  for(uint8_t chan = 0; chan < m_nchannels; chan++){
    if(m_signalMasks.at(chan)){
      buf[0] = (char)1;
      uint32_t size = m_imWidth*m_imHeight;
      offset = 1;
      for(uint32_t i = 0; i < size; i += 8){
	pack(buf,offset,m_signalMasks.at(chan),i);
	if(offset > 200000){
	  fout.write(buf,offset);
	  offset = 0;
	}
      }
      fout.write(buf,offset);
    }
    else{
      buf[0] = (char)0;
      fout.write(buf,1);
    }
    NiaUtils::writeIntToBuffer(buf,0,nPuncta(chan));
    fout.write(buf,4);
    for(std::vector<Cluster*>::iterator it = m_puncta.at(chan).begin(); it != m_puncta.at(chan).end(); it++){
      NiaUtils::writeShortToBuffer(buf,0,(uint16_t)(*it)->size());
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
}

void ImRecord::pack(char* buf, uint64_t& offset, Mask* m, uint32_t index)
{
  uint16_t x = index % m_imWidth; 
  uint16_t y = index / m_imWidth;
  uint8_t nb = 0;
  uint64_t retval = 0;
  for(uint16_t i = x; nb < 64 && i < m_imWidth; i++){
    retval = retval | ((m->getValue(i,y) & 0x01) << nb);
    nb++;
  }
  while(nb < 64){
    x = 0;
    y++;
    for(uint16_t i = x; nb < 64 && i < m_imWidth; i++){
      retval = retval | ((m->getValue(i,y) & 0x01) << nb);
      nb++;
    }
  }
  buf[offset] = (char)(retval & 0x00000000000000ff);
  buf[offset+1] = (char)((retval & 0x000000000000ff00) >> 8);
  buf[offset+2] = (char)((retval & 0x0000000000ff0000) >> 16);
  buf[offset+3] = (char)((retval & 0x00000000ff000000) >> 24);
  buf[offset+4] = (char)((retval & 0x000000ff00000000) >> 32);
  buf[offset+5] = (char)((retval & 0x0000ff0000000000) >> 40);
  buf[offset+6] = (char)((retval & 0x00ff000000000000) >> 48);
  buf[offset+7] = (char)((retval & 0xff00000000000000) >> 56);
  offset += 8;
}

void ImRecord::read(std::ifstream& fin)
{
}

void ImRecord::loadMetaMorphRegions(std::string filename)
{
}

void ImRecord::loadMetaMorphTraces(std::string filename, uint8_t chan, bool overwrite)
{
}
