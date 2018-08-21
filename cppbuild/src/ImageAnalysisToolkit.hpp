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
  int m_punctaFindingIterations,m_signalFindingIterations;
  int m_saturationThreshold;
  double m_subtractionAmount;
  std::vector<int> m_contaminatedChannels,m_backgroundChannels;
  std::vector<std::string> m_channelNames;
  std::vector<double> m_localWindow,m_backgroundThreshold;
  std::vector<int> m_windowSteps;
  std::vector<double> m_minPunctaRadius,m_maxPunctaRadius,m_reclusterThreshold,m_noiseRemovalThreshold;
  std::vector<double> m_peakThreshold,m_floorThreshold,m_kernelWidth;
  std::vector<SynapseCollection*> m_synapseDefinitions;

public:
  ImageAnalysisToolkit();
  virtual ~ImageAnalysisToolkit();
  void makeSeparateConfigs(int nchan);
  void makeSingleConfig();
  int nConfigs(){ return m_peakThreshold.size(); }
  void standardAnalysis(ImStack* stack, ImRecord* rec, int arg_zplane=-1);
  Mask* findOutliers(ImFrame* frame);
  void filter(ImFrame* frame);
  void findSignal(ImStack* analysisStack, ImRecord* rec, int zplane);
  void findSignal(ImFrame* frame, ImRecord* rec, int chan);
  Mask* applyThreshold(ImFrame* frame, ImRecord* rec, int chan);
  int findThreshold(double kernelWidth, ImFrame* frame);
  int findThreshold(double kernelWidth, ImFrame* frame, Mask* sigMask, Mask* outMask, int prev);
  void findPuncta(ImFrame* frame, ImRecord* rec, int chan);
  void findSaturatedPuncta(ImFrame* frame, ImRecord* rec, int chan);
  void resolveOverlaps(ImFrame* frame, ImRecord* rec, int chan);
  void watershedSegmentation(ImFrame* frame, ImRecord* rec, int chan);
  void findSynapses(ImRecord* rec);
  void findStormSynapses(ImRecord* rec);
  void write(std::ofstream& fout);
  void read(std::ifstream& fin, int version);
  int getBitDepth();
  void setBitDepth(int bd){ m_saturationThreshold = (int)(pow(2,bd) - 2);}

  void setMaster(int chan){ m_master = chan; }
  void setPostChan(int chan){ m_postChan = chan; }
  void setMode(MasterMode m){ m_mode = m; }
  void setMaxPunctaFindingIterations(int it){ m_punctaFindingIterations = it; }
  void setMaxSignalFindingIterations(int it){ m_signalFindingIterations = it; }
  void setSaturationThreshold(int s){ m_saturationThreshold = s; }
  void setKernelWidth(double sigma){ m_kernelWidth[0] = sigma; }
  void setSubtractionAmount(double x){ m_subtractionAmount = x; }
  void setChannelNames(std::vector<std::string> names){ m_channelNames = names; }
  void setLocalWindow(double lw){ m_localWindow[0] = lw; }
  void setWindowSteps(int nsteps){ m_windowSteps[0] = nsteps; }
  void setBackgroundThreshold(double lwi){ m_backgroundThreshold[0] = lwi; }
  void setMinPunctaRadius(double r){ m_minPunctaRadius[0] = r; }
  void setMaxPunctaRadius(double r){ m_maxPunctaRadius[0] = r; }
  void setReclusterThreshold(double r){ m_reclusterThreshold[0] = r; }
  void setNoiseRemovalThreshold(double nrt){ m_noiseRemovalThreshold[0] = nrt; }
  void setPeakThreshold(double p){ m_peakThreshold[0] = p; }
  void setFloorThreshold(double f){ m_floorThreshold[0] = f; }
  void setKernelWidth(int chan, double sigma){ m_kernelWidth[chan] = sigma; }
  void setLocalWindow(int chan, double lw){ m_localWindow[chan] = lw; }
  void setWindowSteps(int chan, int nsteps){ m_windowSteps[chan] = nsteps; }
  void setBackgroundThreshold(int chan, double lwi){ m_backgroundThreshold[chan] = lwi; }
  void setMinPunctaRadius(int chan, double r){ m_minPunctaRadius[chan] = r; }
  void setMaxPunctaRadius(int chan, double r){ m_maxPunctaRadius[chan] = r; }
  void setReclusterThreshold(int chan, double r){ m_reclusterThreshold[chan] = r; }
  void setNoiseRemovalThreshold(int chan, double nrt){ m_noiseRemovalThreshold[chan] = nrt; }
  void setPeakThreshold(int chan, double p){ m_peakThreshold[chan] = p; }
  void setFloorThreshold(int chan, double f){ m_floorThreshold[chan] = f; }
  void setContaminatedChannel(int chan1, int chan2){
    m_contaminatedChannels.push_back(chan1);
    m_backgroundChannels.push_back(chan2);
  }
  void clearContaminatedChannels(){
    m_contaminatedChannels.clear();
    m_backgroundChannels.clear();
  }

  int master(){ return m_master; }
  int postChan(){ return m_postChan; }
  MasterMode mode(){ return m_mode; }
  int maxPunctaFindingIterations(){ return m_punctaFindingIterations; }
  int maxSignalFindingIterations(){ return m_signalFindingIterations; }
  int saturationThreshold(){ return m_saturationThreshold; }
  double kernelWidth(int chan = 0){ return m_kernelWidth[chan]; }
  double subtractionAmount(){ return m_subtractionAmount; }
  std::string getChannelName(int chan){ return m_channelNames[chan]; }
  std::vector<std::string> getChannelNames(){ return m_channelNames; }
  double localWindow(int chan = 0){ return m_localWindow[chan]; }
  int windowSteps(int chan = 0){ return m_windowSteps[chan]; }
  double backgroundThreshold(int chan = 0){ return m_backgroundThreshold[chan]; }
  double minPunctaRadius(int chan = 0){ return m_minPunctaRadius[chan]; }
  double maxPunctaRadius(int chan = 0){ return m_maxPunctaRadius[chan]; }
  double reclusterThreshold(int chan = 0){ return m_reclusterThreshold[chan]; }
  double noiseRemovalThreshold(int chan = 0){ return m_noiseRemovalThreshold[chan]; }
  double peakThreshold(int chan = 0){ return m_peakThreshold[chan]; }
  double floorThreshold(int chan = 0){ return m_floorThreshold[chan]; }
  std::vector<int> contaminatedChannels(){ return m_contaminatedChannels; }
  std::vector<int> backgroundChannels(){ return m_backgroundChannels; }

  void addSynapseDefinition(SynapseCollection* sc){ m_synapseDefinitions.push_back(sc); }
  std::vector<SynapseCollection*> synapseDefinitions(){ return m_synapseDefinitions; };
  void removeSynapseDefinition(int index){
    delete m_synapseDefinitions.at(index);
    m_synapseDefinitions.erase(m_synapseDefinitions.begin()+index);
  }
  void clearSynapseDefinitions(){
    for(std::vector<SynapseCollection*>::iterator scit = m_synapseDefinitions.begin(); scit != m_synapseDefinitions.end(); scit++) delete *scit;
    m_synapseDefinitions.clear();
  }
};

#endif
