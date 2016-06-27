#ifndef IMAGE_ANALYSIS_TOOLKIT_HPP
#define IMAGE_ANALYSIS_TOOLKIT_HPP

#include "ImStack.hpp"
#include "ImRecord.hpp"

class ImageAnalysisToolkit
{

protected:
  ImStack* m_image;
  ImRecord* m_record;

public:
  ImageAnalysisToolkit();
  ~ImageAnalysisToolkit();
  void find_signal(uint8_t w, uint8_t z);
  void find_puncta(uint8_t w, uint8_t z);
  void find_synapses(uint8_t z);

};

#endif
