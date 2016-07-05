#include "ImSeries.hpp"

ImSeries::ImSeries()
{
  m_npos = -1;
  m_nt = -1;
  m_name = "";
  m_resolutionXY = 0.046;
}

ImSeries::ImSeries(uint8_t npos, uint8_t nt)
{
  m_npos = npos;
  m_nt = nt;
  m_fourLocations.assign(npos*nt,NULL);
  m_name = "";
  m_resolutionXY = 0.046;
}

ImSeries::~ImSeries()
{
  for(std::vector<ImStack*>::iterator it = m_fourLocations.begin(); it != m_fourLocations.end(); it++){
    if(*it) delete *it;
  }
}

void ImSeries::insert(ImStack* stack, uint8_t pos, uint8_t t)
{
  if(pos < m_npos && t < m_nt){
    uint16_t index = pos*m_nt + t;
    if(m_fourLocations.at(index)) delete m_fourLocations.at(index);
    m_fourLocations.at(index) = stack;
  }
}
