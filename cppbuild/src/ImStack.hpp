#ifndef IMSTACK_HPP
#define IMSTACK_HPP

#include "ImFrame.hpp"

class ImStack
{

protected:
  ImFrame*** m_frames;
  int m_nwaves;
  int m_nz;

public:
  ImStack();
  ImStack(int nw, int nz);
  ~ImStack();
  void insert(ImFrame** frames, int startW, int startZ, int nw, int nz, int order);
  void insert(ImFrame* frame, int w, int z);
  ImStack* zprojection();
  void divide(int d);
  ImFrame* frame(int w, int z){ return m_frames[w][z]; }
  int nwaves(){ return m_nwaves; }
  int nz(){ return m_nz; }

};

#endif
