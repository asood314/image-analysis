#ifndef SYNAPSE_COLLECTION_HPP
#define SYNAPSE_COLLECTION_HPP

#include <iostream>
#include <fstream>
#include "NiaUtils.hpp"
#include "Synapse.hpp"
#include "StormCluster.hpp"

class SynapseCollection
{

protected:
  std::vector<int> m_channels;
  bool m_requireAllColocalized;
  std::vector< std::vector<int> > m_requiredColocalizations;
  uint32_t m_overlapThreshold;
  double m_distanceThreshold;
  bool m_useOverlap;
  std::vector<Synapse*> m_synapses;
  std::vector<StormCluster::StormSynapse> m_stormSynapses;
  std::string m_description;

public:
  SynapseCollection();
  SynapseCollection(std::vector<int> chans);
  virtual ~SynapseCollection();
  SynapseCollection* emptyCopy();
  void addSynapse(Synapse* s){ m_synapses.push_back(s); }
  Synapse* getSynapse(uint32_t index){ return m_synapses.at(index); }
  void removeSynapse(uint32_t index);
  void clearSynapses();
  uint32_t nSynapses(){ return m_synapses.size(); }
  std::vector<Synapse*> synapses(){ return m_synapses; }
  void addStormSynapse(StormCluster::StormSynapse s){ m_stormSynapses.push_back(s); }
  StormCluster::StormSynapse getStormSynapse(int index){ return m_stormSynapses[index]; }
  void clearStormSynapses(){ m_stormSynapses.clear(); }
  int nStormSynapses(){ return m_stormSynapses.size(); }
  void setRequireAll(bool tf){ m_requireAllColocalized = tf; }
  bool allRequired(){ return m_requireAllColocalized; }
  void addRequiredColocalization(std::vector<int> chans);
  void addRequiredColocalizationByIndex(std::vector<int> indices){ m_requiredColocalizations.push_back(indices); }
  std::vector<int> getRequiredColocalization(int index);
  std::vector<int> getRequiredColocalizationByIndex(int index){ return m_requiredColocalizations.at(index); }
  int nRequirements();
  void setOverlapThreshold(uint32_t threshold){ m_overlapThreshold = threshold; }
  uint32_t overlapThreshold(){ return m_overlapThreshold; }
  void setDistanceThreshold(double threshold){ m_distanceThreshold = threshold; }
  double distanceThreshold(){ return m_distanceThreshold; }
  void setUseOverlap(bool tf){ m_useOverlap = tf; }
  bool useOverlap(){ return m_useOverlap; }
  int nChannels(){ return m_channels.size(); }
  std::vector<int> channels(){ return m_channels; }
  int getChannel(int index){ return m_channels.at(index); }
  int getChannelIndex(int chan);
  void setDescription(std::string msg){ m_description = msg; }
  std::string description(){ return m_description; }
  bool computeColocalization(Synapse* s);
  bool checkColocalization(StormCluster::StormSynapse& s);
  void write(char* buf, std::ofstream& fout);
  void read(char* buf, std::ifstream& fin, std::vector< std::vector<Cluster*> >* punks);

};

#endif
