#include "SynapseCollection.hpp"

SynapseCollection::SynapseCollection()
{
  m_requireAllColocalized = true;
  m_overlapThreshold = 0;
  m_distanceThreshold = 0;
  m_useOverlap = true;
  m_description = "";
}

SynapseCollection::SynapseCollection(std::vector<int> chans)
{
  m_channels = chans;
  m_requireAllColocalized = true;
  m_overlapThreshold = 0;
  m_distanceThreshold = 0;
  m_useOverlap = true;
  m_description = "-";
}

SynapseCollection::~SynapseCollection()
{
  for(std::vector<Synapse*>::iterator it = m_synapses.begin(); it != m_synapses.end(); it++) delete *it;
}

SynapseCollection* SynapseCollection::emptyCopy()
{
  SynapseCollection* sc = new SynapseCollection(m_channels);
  sc->setRequireAll(m_requireAllColocalized);
  for(std::vector< std::vector<int> >::iterator it = m_requiredColocalizations.begin(); it != m_requiredColocalizations.end(); it++) sc->addRequiredColocalization(*it);
  sc->setOverlapThreshold(m_overlapThreshold);
  sc->setDistanceThreshold(m_distanceThreshold);
  sc->setUseOverlap(m_useOverlap);
  sc->setDescription(m_description);
  return sc;
}

void SynapseCollection::removeSynapse(uint32_t index)
{
  delete m_synapses.at(index);
  m_synapses.erase(m_synapses.begin()+index);
}

void SynapseCollection::clearSynapses()
{
  for(std::vector<Synapse*>::iterator it = m_synapses.begin(); it != m_synapses.end(); it++) delete *it;
  m_synapses.clear();
}

void SynapseCollection::addRequiredColocalization(std::vector<int> chans)
{
  std::vector<int> indices;
  int size = m_channels.size();
  for(std::vector<int>::iterator it = chans.begin(); it != chans.end(); it++){
    int index = getChannelIndex(*it);
    if(index == size) return;
    indices.push_back(index);
  }
  m_requiredColocalizations.push_back(indices);
}

std::vector<int> SynapseCollection::getRequiredColocalization(int index)
{
  std::vector<int> chans;
  std::vector<int> indices = m_requiredColocalizations.at(index);
  for(std::vector<int>::iterator it = indices.begin(); it != indices.end(); it++) chans.push_back(m_channels.at(*it));
  return chans;
}

int SynapseCollection::nRequirements()
{
  if(m_requireAllColocalized) return 1;
  return m_requiredColocalizations.size();
}

int SynapseCollection::getChannelIndex(int chan)
{
  int index = 0;
  for(std::vector<int>::iterator it = m_channels.begin(); it != m_channels.end(); it++){
    if(*it == chan) return index;
    index++;
  }
  return index;
}

bool SynapseCollection::computeColocalization(Synapse* s)
{
  if(m_requireAllColocalized){
    if(m_useOverlap){
      int overlap = s->punctaOverlap() - m_overlapThreshold;
      s->setColocalizationScore((double)overlap);
      if(overlap > 0) return true;
      else return false;
    }
    else{
      double distance = m_distanceThreshold - s->maxPunctaDistance(m_channels);
      s->setColocalizationScore(distance);
      if(distance > 0) return true;
      else return false;
    }
    return false;
  }
  double score = 1.0;
  if(m_useOverlap){
    for(std::vector< std::vector<int> >::iterator it = m_requiredColocalizations.begin(); it != m_requiredColocalizations.end(); it++){
      score *= s->punctaOverlap(*it) - m_overlapThreshold;
    }
  }
  else{
    for(std::vector< std::vector<int> >::iterator it = m_requiredColocalizations.begin(); it != m_requiredColocalizations.end(); it++){
      score *= m_distanceThreshold - s->maxPunctaDistance(*it);
    }
  }
  s->setColocalizationScore(score);
  if(score > 0) return true;
  return false;
}

bool SynapseCollection::checkColocalization(StormCluster::StormSynapse& s)
{
  if(m_requireAllColocalized){
    for(int i = 0; i < s.clusters.size(); i++){
      for(int j = i+1; j < s.clusters.size(); j++){
	if(!(s.clusters[i]->colocalWith(s.clusters[j]))) return false;
      }
    }
    return true;
  }
  for(std::vector< std::vector<int> >::iterator it = m_requiredColocalizations.begin(); it != m_requiredColocalizations.end(); it++){
    for(int i = 0; i < it->size(); i++){
      for(int j = i+1; j < it->size(); j++){
	if(!(s.clusters[it->at(i)]->colocalWith(s.clusters[it->at(j)]))) return false;

      }
    }
  }
  return true;
}

void SynapseCollection::write(char* buf, std::ofstream& fout)
{
  int64_t offset = 0;
  NiaUtils::writeShortToBuffer(buf,offset,m_description.length());
  offset += 2;
  for(int i = 0; i < m_description.length(); i++){
    buf[offset] = m_description[i];
    offset++;
  }
  buf[offset] = (char)nChannels();
  offset++;
  for(std::vector<int>::iterator jt = m_channels.begin(); jt != m_channels.end(); jt++){
    buf[offset] = (char)(*jt);
    offset++;
  }
  NiaUtils::writeIntToBuffer(buf,offset,nSynapses());
  offset += 4;
  for(std::vector<Synapse*>::iterator jt = m_synapses.begin(); jt != m_synapses.end(); jt++){
    NiaUtils::writeDoubleToBuffer(buf,offset,(*jt)->colocalizationScore());
    offset += 4;
    for(int i = 0; i < nChannels(); i++){
      NiaUtils::writeIntToBuffer(buf,offset,(*jt)->getPunctumIndex(i));
      offset += 4;
    }
  }
  NiaUtils::writeIntToBuffer(buf,offset,overlapThreshold());
  offset += 4;
  NiaUtils::writeDoubleToBuffer(buf,offset,distanceThreshold());
  offset += 4;
  buf[offset] = (char)useOverlap();
  offset++;
  if(allRequired()){
    buf[offset] = 1;
    offset++;
  }
  else{
    buf[offset] = 0;
    buf[offset+1] = (char)nRequirements();
    offset += 2;
    for(int i = 0; i < nRequirements(); i++){
      std::vector<int> reqs = getRequiredColocalizationByIndex(i);
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

void SynapseCollection::read(char* buf, std::ifstream& fin, std::vector< std::vector<Cluster*> >* punks)
{
  fin.read(buf,2);
  int len = NiaUtils::convertToShort(buf[0],buf[1]);
  fin.read(buf,len+1);
  m_description.append(buf,len);
  int nchan = (int)buf[len];
  fin.read(buf,nchan);
  for(int i = 0; i < nchan; i++) m_channels.push_back((int)buf[i]);
  fin.read(buf,4);
  int nsyn = NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]);
  fin.read(buf,4*(nchan+1)*nsyn);
  int64_t offset = 0;
  for(int i = 0; i < nsyn; i++){
    Synapse* s = new Synapse();
    s->setColocalizationScore(NiaUtils::convertToDouble(buf[offset],buf[offset+1],buf[offset+2],buf[offset+3]));
    offset += 4;
    for(std::vector<int>::iterator it = m_channels.begin(); it != m_channels.end(); it++){
      int index = NiaUtils::convertToInt(buf[offset],buf[offset+1],buf[offset+2],buf[offset+3]);
      s->addPunctum(punks->at(*it).at(index),index);
      offset += 4;
    }
    s->computeCenter();
    addSynapse(s);
  }
  fin.read(buf,10);
  setOverlapThreshold(NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]));
  setDistanceThreshold(NiaUtils::convertToDouble(buf[4],buf[5],buf[6],buf[7]));
  setUseOverlap((int)buf[8] > 0);
  setRequireAll((int)buf[9] > 0);
  if(!allRequired()){
    fin.read(buf,1);
    int nreqs = (int)buf[0];
    for(int i = 0; i < nreqs; i++){
      std::vector<int> reqs;
      fin.read(buf,1);
      int n = (int)buf[0];
      fin.read(buf,n);
      for(int j = 0; j < n; j++) reqs.push_back((int)buf[j]);
      addRequiredColocalizationByIndex(reqs);
    }
  }
}
