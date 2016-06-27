#ifndef IMSTACK_HPP
#define IMSTACK_HPP

#include "ImFrame.hpp"

class ImStack
{

protected:
  ImFrame*** m_frames;
  uint8_t m_nwaves;
  uint8_t m_nz;

public:
  ImStack();
  ImStack(uint8_t nw, uint8_t nz);
  ~ImStack();
  void insert(ImFrame** frames, uint8_t startW, uint8_t startZ, uint8_t nw, uint8_t nz, uint8_t order);
  void insert(ImFrame* frame, uint8_t w, uint8_t z);
  ImStack* zprojection();
  ImFrame* frame(uint8_t w, uint8_t z){ return m_frames[w][z]; }
  uint8_t nwaves(){ return m_nwaves; }
  uint8_t nz(){ return m_nz; }

};

#endif
