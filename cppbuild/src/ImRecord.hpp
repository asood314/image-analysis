#ifndef IMRECORD_HPP
#define IMRECORD_HPP

#include "SynapseCollection.hpp"
#include "Region.hpp"
#include "Mask.hpp"
#include "NiaUtils.hpp"
#include <iostream>
#include <fstream>

class ImRecord
{

protected:
  uint8_t m_nchannels;
  uint16_t m_imWidth,m_imHeight;
  double m_resolutionXY;
  std::vector<uint16_t> m_thresholds;
  std::vector<Mask*> m_signalMasks;
  std::vector<Mask*> m_utilityMasks;
  std::vector< std::vector<Cluster*> > m_puncta;
  std::vector<SynapseCollection*> m_synapseCollections;
  std::vector<Region*> m_regions;

public:
  ImRecord(){}
  ImRecord(uint8_t nchan, uint16_t w, uint16_t h);
  virtual ~ImRecord();
  void setResolutionXY(double res){ m_resolutionXY = res; }
  double resolutionXY(){ return m_resolutionXY; }
  uint8_t nchannels(){ return m_nchannels; }
  uint16_t imWidth(){ return m_imWidth; }
  uint16_t imHeight(){ return m_imHeight; }
  void setThreshold(uint8_t chan, uint16_t thrshld){ m_thresholds.at(chan) = thrshld; }
  uint16_t getThreshold(uint8_t chan){ return m_thresholds.at(chan); }
  void setSignalMask(uint8_t chan, Mask* m){ m_signalMasks.at(chan) = m; }
  Mask* getSignalMask(uint8_t chan){ return m_signalMasks.at(chan); }
  std::vector<Mask*> signalMasks(){ return m_signalMasks; }
  void setUtilityMask(uint8_t chan, Mask* m){ m_utilityMasks.at(chan) = m; }
  Mask* getUtilityMask(uint8_t chan){ return m_utilityMasks.at(chan); }
  void addRegion(Region* r){ m_regions.push_back(r); }
  Region* getRegion(uint8_t index){ return m_regions.at(index); }
  uint8_t nRegions(){ return m_regions.size(); }
  std::vector<Region*> regions(){ return m_regions; }
  void addPunctum(uint8_t chan, Cluster* c){ m_puncta.at(chan).push_back(c); }
  void removePunctum(uint8_t chan, uint32_t index);
  void clearPuncta(uint8_t chan);
  Cluster* getPunctum(uint8_t chan, uint32_t index){ return m_puncta.at(chan).at(index); }
  uint32_t nPuncta(uint8_t chan){ return m_puncta.at(chan).size(); }
  std::vector<Cluster*> puncta(uint8_t chan){ return m_puncta.at(chan); }
  void addSynapseCollection(SynapseCollection* sc){ m_synapseCollections.push_back(sc); }
  void removeSynapseCollection(uint8_t index);
  SynapseCollection* getSynapseCollection(uint8_t index){ return m_synapseCollections.at(index); }
  uint8_t nSynapseCollections(){ return m_synapseCollections.size(); }
  std::vector<SynapseCollection*> synapseCollections(){ return m_synapseCollections; }
  Cluster* selectPunctum(LocalizedObject::Point pt);
  Cluster* selectPunctum(uint8_t chan, LocalizedObject::Point pt);
  Synapse* selectSynapse(LocalizedObject::Point pt);
  Synapse* selectSynapseFromCollection(uint8_t index, LocalizedObject::Point pt);
  Mask* getPunctaMask(uint8_t chan, bool outline);
  Mask* getSynapseMask(bool outline);
  Mask* getSynapseMaskFromCollection(uint8_t index, bool outline);
  Mask* getSynapticPunctaMask(uint8_t chan, bool outline);
  Mask* getSynapticPunctaMaskFromCollection(uint8_t index, uint8_t chan, bool outline);
  void calculateRegionStats(Region* r, uint8_t& postChan);
  void printSynapseDensityTable(uint8_t& postChan, std::vector<std::string> chanNames, std::string filename);
  void write(std::ofstream& fout);
  void pack(char* buf, uint64_t& offset, Mask* m, uint32_t index);
  void read(std::ifstream& fin);
  void unpack(char* buf, uint64_t offset, Mask* m, uint32_t index);
  void loadMetaMorphRegions(std::string filename);
  void loadMetaMorphTraces(std::string filename, uint8_t chan, bool overwrite);
  
};

#endif
