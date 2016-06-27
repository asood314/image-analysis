#include "ImSeries.hpp"

ImSeries::ImSeries()
{
  m_fourLocations = NULL;
  m_npos = -1;
  m_nt = -1;
}

ImSeries::ImSeries(uint8_t npos, uint8_t nt)
{
  m_npos = npos;
  m_nt = nt;
  m_fourLocations = new ImStack**[m_npos];
  for(uint8_t i = 0; i < m_npos; i++) m_fourLocations[i] = new ImStack*[m_nt];
}

ImSeries::~ImSeries()
{
  if(m_fourLocations){
    for(uint8_t i = 0; i < m_npos; i++) delete[] m_fourLocations[i];
    delete[] m_fourLocations;
  }
}

void ImSeries::insert(ImStack* stack, uint8_t pos, uint8_t t)
{
  if(pos < m_npos && t < m_nt) m_fourLocations[pos][t] = stack;
}
