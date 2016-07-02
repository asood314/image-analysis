#ifndef IMAGE_ANALYSIS_TOOLKIT_HPP
#define IMAGE_ANALYSIS_TOOLKIT_HPP

#include "ImSeries.hpp"
#include "ImRecord.hpp"
#include <cmath>

class ImageAnalysisToolkit
{

public:
  enum MasterMode{OVERRIDE = 0, OR};

  typedef struct gaussian2D{
    uint16_t peak;
    double meanX,meanY,width2;
    gaussian2D(){}
    gaussian2D(uint16_t p, uint16_t mx, uint16_t my, double w2){
      peak = p;
      meanX = (double)mx;
      meanY = (double)my;
      width2 = w2;
    }
    double calculate(uint16_t x, uint16_t y){
      return peak * exp(-((x-meanX)*(x-meanX) + (y-meanY)*(y-meanY))/(2*width2));
    }
  } Gaus2D;

protected:
  uint8_t m_master;
  MasterMode m_mode;
  uint8_t m_punctaFindingIterations;
  uint16_t m_saturationThreshold;
  std::vector<double> m_localWindow,m_localWindowIncrement;
  std::vector<double> m_minPunctaRadius,m_reclusterThreshold,m_noiseRemovalThreshold;
  std::vector<double> m_peakThreshold,m_floorThreshold;

public:
  ImageAnalysisToolkit();
  virtual ~ImageAnalysisToolkit();
  void makeSeparateConfigs(uint8_t nchan);
  void makeSingleConfig();
  void standardAnalysis(ImStack* stack, ImRecord* rec, int zplane=-1);
  void findSignal(ImFrame* frame, ImRecord* rec, uint8_t chan);
  uint16_t findThreshold(ImFrame* frame);
  void findPuncta(ImFrame* frame, ImRecord* rec, uint8_t chan);
  void findSaturatedPuncta(ImFrame* frame, ImRecord* rec, uint8_t chan);
  void resolveOverlaps(ImFrame* frame, ImRecord* rec, uint8_t chan);
  void findSynapses(ImRecord* rec);
  void write(std::ofstream& fout);
  void read(std::ifstream& fin);

  void setMaster(uint8_t chan){ m_master = chan; }
  void setMode(MasterMode m){ m_mode = m; }
  void setMaxPunctaFindingIterations(uint8_t it){ m_punctaFindingIterations = it; }
  void setSaturationThreshold(uint16_t s){ m_saturationThreshold = s; }
  void setLocalWindow(double lw){ m_localWindow.at(0) = lw; }
  void setLocalWindowIncrement(double lwi){ m_localWindowIncrement.at(0) = lwi; }
  void setMinPunctaRadius(double r){ m_minPunctaRadius.at(0) = r; }
  void setReclusterThreshold(double r){ m_reclusterThreshold.at(0) = r; }
  void setNoiseRemovalThreshold(double nrt){ m_noiseRemovalThreshold.at(0) = nrt; }
  void setPeakThreshold(double p){ m_peakThreshold.at(0) = p; }
  void setFloorThreshold(double f){ m_floorThreshold.at(0) = f; }
  void setLocalWindow(uint8_t chan, double lw){ m_localWindow.at(chan) = lw; }
  void setLocalWindowIncrement(uint8_t chan, double lwi){ m_localWindowIncrement.at(chan) = lwi; }
  void setMinPunctaRadius(uint8_t chan, double r){ m_minPunctaRadius.at(chan) = r; }
  void setReclusterThreshold(uint8_t chan, double r){ m_reclusterThreshold.at(chan) = r; }
  void setNoiseRemovalThreshold(uint8_t chan, double nrt){ m_noiseRemovalThreshold.at(chan) = nrt; }
  void setPeakThreshold(uint8_t chan, double p){ m_peakThreshold.at(chan) = p; }
  void setFloorThreshold(uint8_t chan, double f){ m_floorThreshold.at(chan) = f; }

};

#endif
