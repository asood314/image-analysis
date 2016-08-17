#ifndef IMAGE_ANALYSIS_TOOLKIT_HPP
#define IMAGE_ANALYSIS_TOOLKIT_HPP

#include "ImSeries.hpp"
#include "ImRecord.hpp"
#include "NiaBuffer.hpp"
#include <cmath>

class ImageAnalysisToolkit
{

public:
  enum MasterMode{OVERRIDE = 0, OR};

  typedef struct gaussian2D{
    int peak;
    double meanX,meanY,width2;
    gaussian2D(){}
    gaussian2D(int p, int mx, int my, double w2){
      peak = p;
      meanX = (double)mx;
      meanY = (double)my;
      width2 = w2;
    }
    double calculate(int x, int y){
      return peak * exp(-((x-meanX)*(x-meanX) + (y-meanY)*(y-meanY))/(2*width2));
    }
  } Gaus2D;

protected:
  int m_master,m_postChan;
  MasterMode m_mode;
  int m_punctaFindingIterations;
  int m_saturationThreshold;
  std::vector<std::string> m_channelNames;
  std::vector<double> m_localWindow,m_localWindowIncrement;
  std::vector<double> m_minPunctaRadius,m_reclusterThreshold,m_noiseRemovalThreshold;
  std::vector<double> m_peakThreshold,m_floorThreshold;
  std::vector<SynapseCollection*> m_synapseDefinitions;

public:
  ImageAnalysisToolkit();
  virtual ~ImageAnalysisToolkit();
  void makeSeparateConfigs(int nchan);
  void makeSingleConfig();
  void standardAnalysis(ImStack* stack, ImRecord* rec, int arg_zplane=-1);
  void findSignal(ImStack* analysisStack, ImRecord* rec, int zplane);
  void findSignal(ImFrame* frame, ImRecord* rec, int chan);
  int findThreshold(ImFrame* frame);
  void findPuncta(ImFrame* frame, ImRecord* rec, int chan);
  void findSaturatedPuncta(ImFrame* frame, ImRecord* rec, int chan);
  void resolveOverlaps(ImFrame* frame, ImRecord* rec, int chan);
  void findSynapses(ImRecord* rec);
  void write(std::ofstream& fout);
  void read(std::ifstream& fin);
  int getBitDepth();
  void setBitDepth(int bd){ m_saturationThreshold = (int)(pow(2,bd) - 2);}

  void setMaster(int chan){ m_master = chan; }
  void setPostChan(int chan){ m_postChan = chan; }
  void setMode(MasterMode m){ m_mode = m; }
  void setMaxPunctaFindingIterations(int it){ m_punctaFindingIterations = it; }
  void setSaturationThreshold(int s){ m_saturationThreshold = s; }
  void setChannelNames(std::vector<std::string> names){ m_channelNames = names; }
  void setLocalWindow(double lw){ m_localWindow.at(0) = lw; }
  void setLocalWindowIncrement(double lwi){ m_localWindowIncrement.at(0) = lwi; }
  void setMinPunctaRadius(double r){ m_minPunctaRadius.at(0) = r; }
  void setReclusterThreshold(double r){ m_reclusterThreshold.at(0) = r; }
  void setNoiseRemovalThreshold(double nrt){ m_noiseRemovalThreshold.at(0) = nrt; }
  void setPeakThreshold(double p){ m_peakThreshold.at(0) = p; }
  void setFloorThreshold(double f){ m_floorThreshold.at(0) = f; }
  void setLocalWindow(int chan, double lw){ m_localWindow.at(chan) = lw; }
  void setLocalWindowIncrement(int chan, double lwi){ m_localWindowIncrement.at(chan) = lwi; }
  void setMinPunctaRadius(int chan, double r){ m_minPunctaRadius.at(chan) = r; }
  void setReclusterThreshold(int chan, double r){ m_reclusterThreshold.at(chan) = r; }
  void setNoiseRemovalThreshold(int chan, double nrt){ m_noiseRemovalThreshold.at(chan) = nrt; }
  void setPeakThreshold(int chan, double p){ m_peakThreshold.at(chan) = p; }
  void setFloorThreshold(int chan, double f){ m_floorThreshold.at(chan) = f; }

  int master(){ return m_master; }
  int postChan(){ return m_postChan; }
  MasterMode mode(){ return m_mode; }
  int maxPunctaFindingIterations(){ return m_punctaFindingIterations; }
  int saturationThreshold(){ return m_saturationThreshold; }
  std::string getChannelName(int chan){ return m_channelNames.at(chan); }
  std::vector<std::string> getChannelNames(){ return m_channelNames; }
  double localWindow(){ return m_localWindow.at(0); }
  double localWindowIncrement(){ return m_localWindowIncrement.at(0); }
  double minPunctaRadius(){ return m_minPunctaRadius.at(0); }
  double reclusterThreshold(){ return m_reclusterThreshold.at(0); }
  double noiseRemovalThreshold(){ return m_noiseRemovalThreshold.at(0); }
  double peakThreshold(){ return m_peakThreshold.at(0); }
  double floorThreshold(){ return m_floorThreshold.at(0); }
  double getLocalWindow(int chan){ return m_localWindow.at(chan); }
  double getLocalWindowIncrement(int chan){ return m_localWindowIncrement.at(chan); }
  double getMinPunctaRadius(int chan){ return m_minPunctaRadius.at(chan); }
  double getReclusterThreshold(int chan){ return m_reclusterThreshold.at(chan); }
  double getNoiseRemovalThreshold(int chan){ return m_noiseRemovalThreshold.at(chan); }
  double getPeakThreshold(int chan){ return m_peakThreshold.at(chan); }
  double getFloorThreshold(int chan){ return m_floorThreshold.at(chan); }

  void addSynapseDefinition(SynapseCollection* sc){ m_synapseDefinitions.push_back(sc); }
  std::vector<SynapseCollection*> synapseDefinitions(){ return m_synapseDefinitions; };
  void removeSynapseDefinition(int index){
    delete m_synapseDefinitions.at(index);
    m_synapseDefinitions.erase(m_synapseDefinitions.begin()+index);
  }
};

#endif
