#include "ImStack.hpp"

ImStack::ImStack()
{
  m_frames = NULL;
  m_nwaves = -1;
  m_nz = -1;
}

ImStack::ImStack(uint8_t nw, uint8_t nz)
{
  m_nwaves = nw;
  m_nz = nz;
  m_frames = new ImFrame**[m_nwaves];
  for(uint8_t i = 0; i < m_nwaves; i++) m_frames[i] = new ImFrame*[nz];
}

ImStack::~ImStack()
{
  if(m_frames){
    for(uint8_t i = 0; i < m_nwaves; i++) delete[] m_frames[i];
    delete[] m_frames;
  }
}

void ImStack::insert(ImFrame** frames, uint8_t startW, uint8_t startZ, uint8_t nw, uint8_t nz, uint8_t order)
{
}

void ImStack::insert(ImFrame* frame, uint8_t w, uint8_t z)
{
  if(w < m_nwaves && z < m_nz) m_frames[w][z] = frame;
}

ImStack* ImStack::zprojection()
{
  return NULL;
}
