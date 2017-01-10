#include "ImStack.hpp"

ImStack::ImStack()
{
  m_frames = NULL;
  m_nwaves = -1;
  m_nz = -1;
}

ImStack::ImStack(int nw, int nz)
{
  m_nwaves = nw;
  m_nz = nz;
  m_frames = new ImFrame**[m_nwaves];
  for(int i = 0; i < m_nwaves; i++) m_frames[i] = new ImFrame*[nz];
}

ImStack::~ImStack()
{
  if(m_frames){
    for(int i = 0; i < m_nwaves; i++) delete[] m_frames[i];
    delete[] m_frames;
  }
}

void ImStack::insert(ImFrame** frames, int startW, int startZ, int nw, int nz, int order)
{
}

void ImStack::insert(ImFrame* frame, int w, int z)
{
  if(w < m_nwaves && z < m_nz) m_frames[w][z] = frame;
}

ImStack* ImStack::zprojection()
{
  ImStack* retVal = new ImStack(m_nwaves,1);
  for(int w = 0; w < m_nwaves; w++){
    ImFrame* f = new ImFrame(m_frames[w][0]->width(),m_frames[w][0]->height());
    for(int i = 0; i < f->width(); i++){
      for(int j = 0; j < f->height(); j++){
	int max = 0;
	for(int z = 0; z < m_nz; z++){
	  int val = m_frames[w][z]->getPixel(i,j);
	  if(val > max) max = val;
	}
	f->setPixel(i,j,max);
      }
    }
    retVal->insert(f,w,0);
  }
  return retVal;
}

void ImStack::divide(int d)
{
  for(int w = 0; w < m_nwaves; w++){
    for(int z = 0; z < m_nz; z++){
      m_frames[w][z]->divide(d);
    }
  }
}

void ImStack::derivative(int w, int z)
{
  ImFrame* tmp = m_frames[w][z];
  m_frames[w][z] = tmp->derivative();
  delete tmp;
}

void ImStack::derivative()
{
  for(int w = 0; w < m_nwaves; w++){
    for(int z = 0; z < m_nz; z++) derivative(w,z);
  }
}

void ImStack::derivativeDir(int w, int z)
{
  ImFrame* tmp = m_frames[w][z];
  m_frames[w][z] = tmp->derivativeDir();
  delete tmp;
}

void ImStack::derivativeDir()
{
  for(int w = 0; w < m_nwaves; w++){
    for(int z = 0; z < m_nz; z++) derivativeDir(w,z);
  }
}

void ImStack::d2EigenvalueMax(int w, int z)
{
  ImFrame* tmp = m_frames[w][z];
  m_frames[w][z] = tmp->d2EigenvalueMax();
  delete tmp;
}

void ImStack::d2EigenvalueMax()
{
  for(int w = 0; w < m_nwaves; w++){
    for(int z = 0; z < m_nz; z++) d2EigenvalueMax(w,z);
  }
}

void ImStack::d2EigenvectorMax(int w, int z)
{
  ImFrame* tmp = m_frames[w][z];
  m_frames[w][z] = tmp->d2EigenvectorMax();
  delete tmp;
}

void ImStack::d2EigenvectorMax()
{
  for(int w = 0; w < m_nwaves; w++){
    for(int z = 0; z < m_nz; z++) d2EigenvectorMax(w,z);
  }
}
