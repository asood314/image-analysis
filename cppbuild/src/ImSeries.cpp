#include "ImSeries.hpp"

ImSeries::ImSeries()
{
  m_npos = -1;
  m_nt = -1;
  m_name = "";
  m_resolutionXY = 0.11;
}

ImSeries::ImSeries(int npos, int nt)
{
  m_npos = npos;
  m_nt = nt;
  m_fourLocations.assign(npos*nt,NULL);
  m_name = "";
  m_resolutionXY = 0.11;
}

ImSeries::~ImSeries()
{
  for(std::vector<ImStack*>::iterator it = m_fourLocations.begin(); it != m_fourLocations.end(); it++){
    if(*it) delete *it;
  }
}

void ImSeries::insert(ImStack* stack, int pos, int t)
{
  if(pos < m_npos && t < m_nt){
    int index = pos*m_nt + t;
    if(m_fourLocations.at(index)) delete m_fourLocations.at(index);
    m_fourLocations.at(index) = stack;
  }
}

void ImSeries::remove(int pos, int t)
{
  if(pos < m_npos && t < m_nt){
    int index = pos*m_nt + t;
    if(m_fourLocations.at(index)) delete m_fourLocations.at(index);
    m_fourLocations.at(index) = NULL;
  }
}

void ImSeries::zproject()
{
  for(std::vector<ImStack*>::iterator it = m_fourLocations.begin(); it != m_fourLocations.end(); it++){
    if((*it)){
      ImStack* tmp = *it;
      *it = tmp->zprojection();
      delete tmp;
    }
  }
}
