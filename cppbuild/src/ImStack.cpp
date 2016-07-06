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
  ImStack* retVal = new ImStack(m_nwaves,1);
  for(uint8_t w = 0; w < m_nwaves; w++){
    ImFrame* f = new ImFrame(m_frames[w][0]->width(),m_frames[w][0]->height());
    for(uint16_t i = 0; i < f->width(); i++){
      for(uint16_t j = 0; j < f->height(); j++){
	uint16_t max = 0;
	for(int z = 0; z < m_nz; z++){
	  uint16_t val = m_frames[w][z]->getPixel(i,j);
	  if(val > max) max = val;
	}
	f->setPixel(i,j,max);
      }
    }
    retVal->insert(f,w,0);
  }
  return retVal;
}
