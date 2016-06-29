#ifndef SYNAPSE_COLLECTION_HPP
#define SYNAPSE_COLLECTION_HPP

#include "Synapse.hpp"

class SynapseCollection
{

protected:
  std::vector<uint8_t> m_channels;
  bool m_requireAllColocalized;
  std::vector< std::vector<uint8_t> > m_requiredColocalizations;
  uint32_t m_overlapThreshold;
  double m_distanceThreshold;
  bool m_useOverlap;
  std::vector<Synapse*> m_synapses;
  std::string m_description;

public:
  SynapseCollection(std::vector<uint8_t> chans);
  virtual ~SynapseCollection();
  SynapseCollection* emptyCopy();
  void addSynapse(Synapse* s){ m_synapses.push_back(s); }
  Synapse* getSynapse(uint32_t index){ return m_synapses.at(index); }
  void removeSynapse(uint32_t index);
  uint32_t nSynapses(){ return m_synapses.size(); }
  void setRequireAll(bool tf){ m_requireAllColocalized = tf; }
  bool allRequired(){ return m_requireAllColocalized; }
  void addRequiredColocalization(std::vector<uint8_t> chans);
  void addRequiredColocalizationByIndex(std::vector<uint8_t> indices){ m_requiredColocalizations.push_back(indices); }
  std::vector<uint8_t> getRequiredColocalization(uint8_t index);
  std::vector<uint8_t> getRequiredColocalizationByIndex(uint8_t index){ return m_requiredColocalizations.at(index); }
  uint8_t nRequirements(){ return m_requiredColocalizations.size(); }
  void setOverlapThreshold(uint32_t threshold){ m_overlapThreshold = threshold; }
  uint32_t overlapThreshold(){ return m_overlapThreshold; }
  void setDistanceThreshold(double threshold){ m_distanceThreshold = threshold; }
  double distanceThreshold(){ return m_distanceThreshold; }
  void setUseOverlap(bool tf){ m_useOverlap = tf; }
  bool useOverlap(){ return m_useOverlap; }
  uint8_t nChannels(){ return m_channels.size(); }
  std::vector<uint8_t> channels(){ return m_channels; }
  uint8_t getChannel(uint8_t index){ return m_channels.at(index); }
  uint8_t getChannelIndex(uint8_t chan);
  void setDescription(std::string msg){ m_description = msg; }
  std::string descripton(){ return m_description; }
  bool computeColocalization(Synapse* s);

};

#endif
