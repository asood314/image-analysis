#include "SynapseCollection.hpp"

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
