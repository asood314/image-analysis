#ifndef IMRECORD_HPP
#define IMRECORD_HPP

#include "SynapseCollection.hpp"
#include "Region.hpp"
#include "Mask.hpp"
#include "NiaUtils.hpp"
#include "Segment.hpp"
#include "StormCluster.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

class ImRecord
{

protected:
  int m_nchannels;
  int m_imWidth,m_imHeight;
  double m_resolutionXY;
  std::vector<std::string> m_channelNames;
  std::vector<int> m_thresholds;
  std::vector<Mask*> m_signalMasks;
  std::vector<Mask*> m_utilityMasks;
  std::vector< std::vector<Cluster*> > m_puncta;
  std::vector<SynapseCollection*> m_synapseCollections;
  std::vector<Region*> m_regions;
  std::vector<Segment*> m_segments;
  std::vector< std::vector<StormCluster*> > m_stormClusters;

public:
  ImRecord(){}
  ImRecord(int nchan, int w, int h);
  virtual ~ImRecord();
  void setResolutionXY(double res){ m_resolutionXY = res; }
  double resolutionXY(){ return m_resolutionXY; }
  int nchannels(){ return m_nchannels; }
  int imWidth(){ return m_imWidth; }
  int imHeight(){ return m_imHeight; }
  void setChannelName(int chan, std::string name){ m_channelNames.at(chan) = name; }
  std::string getChannelName(int chan){ return m_channelNames.at(chan); }
  void setThreshold(int chan, int thrshld){ m_thresholds.at(chan) = thrshld; }
  int getThreshold(int chan){ return m_thresholds.at(chan); }
  void setSignalMask(int chan, Mask* m){
    if(m_signalMasks.at(chan)) delete m_signalMasks.at(chan);
    m_signalMasks.at(chan) = m;
  }
  Mask* getSignalMask(int chan){ return m_signalMasks.at(chan); }
  std::vector<Mask*> signalMasks(){ return m_signalMasks; }
  void setUtilityMask(int chan, Mask* m){
    if(m_utilityMasks.at(chan)) delete m_utilityMasks.at(chan);
    m_utilityMasks.at(chan) = m;
  }
  Mask* getUtilityMask(int chan){ return m_utilityMasks.at(chan); }
  void addRegion(Region* r){ m_regions.push_back(r); }
  Region* getRegion(int index){ return m_regions.at(index); }
  void removeRegion(int index);
  int nRegions(){ return m_regions.size(); }
  std::vector<Region*> regions(){ return m_regions; }
  void clearRegions();
  void addPunctum(int chan, Cluster* c){ m_puncta.at(chan).push_back(c); }
  void removePunctum(int chan, int index);
  void clearPuncta(int chan);
  Cluster* getPunctum(int chan, int index){ return m_puncta.at(chan).at(index); }
  int nPuncta(int chan){ return m_puncta.at(chan).size(); }
  std::vector<Cluster*> puncta(int chan){ return m_puncta.at(chan); }
  void addSynapseCollection(SynapseCollection* sc){ m_synapseCollections.push_back(sc); }
  void removeSynapseCollection(int index);
  void clearSynapseCollections();
  SynapseCollection* getSynapseCollection(int index){ return m_synapseCollections.at(index); }
  int nSynapseCollections(){ return m_synapseCollections.size(); }
  std::vector<SynapseCollection*> synapseCollections(){ return m_synapseCollections; }
  Segment* selectSegment(LocalizedObject::Point pt);
  Cluster* selectPunctum(LocalizedObject::Point pt);
  Cluster* selectPunctum(int chan, LocalizedObject::Point pt);
  Synapse* selectSynapse(LocalizedObject::Point pt);
  Synapse* selectSynapseFromCollection(int index, LocalizedObject::Point pt);
  Mask* getContourMap(int chan);
  Mask* segment(int chan);
  Mask* segment2(int chan);
  Mask* segment3(int chan);
  void convertRegionsToSegments(int chan);
  void hike(LocalizedObject::Point pt, int base, Mask* contourMask, Mask* nodeMask, std::vector< std::vector< std::vector<LocalizedObject::Point> > >* trail);
  Mask* getPunctaMask(int chan, bool outline=false);
  Mask* getSynapseMask(bool outline=true);
  Mask* getSynapseMaskFromCollection(int index, bool outline=true);
  Mask* getSynapticPunctaMask(int chan, bool outline=true);
  Mask* getSynapticPunctaMaskFromCollection(int index, int chan, bool outline=true);
  Mask* getRegionMask(bool outline=true);
  void calculateRegionStats(Region* r, int postChan);
  void printSynapseDensityTable(int postChan, std::string filename);
  void write(std::ofstream& fout, int version);
  uint64_t pack(char* buf, Mask* m, int startY);
  //void pack(char* buf, uint64_t& offset, Mask* m, int index);
  void read(std::ifstream& fin, int version);
  void unpack(char* buf, Mask* m, int startY);
  //void unpack(char* buf, uint64_t offset, Mask* m, int index);
  void loadMetaMorphRegions(std::string filename);
  void loadMetaMorphTraces(std::string filename, int chan, bool overwrite);
  void setStormClusters(int chan, std::vector<StormCluster*> clusters);
  int nStormClusters(int chan){ return m_stormClusters[chan].size(); }
  StormCluster* stormCluster(int chan, int i){ return m_stormClusters[chan][i]; }
  void shiftStormData(int shiftX_pix, int shiftY_pix);
  Mask* getStormClusterMask(int chan);
  Mask* getStormClusterLocations(int chan);
  bool selectStormCluster(double x, double y);
  bool selectStormCluster(int chan, double x, double y);
  bool selectStormSynapse(double x, double y);
  
};

#endif
