#ifndef IMSERIES_HPP
#define IMSERIES_HPP

#include "ImStack.hpp"

class ImSeries
{

protected:
  std::vector<ImStack*> m_fourLocations;
  int m_npos;
  int m_nt;
  std::string m_name;
  double m_resolutionXY;

public:
  ImSeries();
  ImSeries(int npos, int nt);
  ~ImSeries();
  void insert(ImStack* stack, int pos, int t);
  void remove(int pos, int t);
  void zproject();
  void divide(int d){
    for(std::vector<ImStack*>::iterator it = m_fourLocations.begin(); it != m_fourLocations.end(); it++){
      if(*it) (*it)->divide(d);
    }
  }
  void setName(std::string nm){ m_name = nm; }
  ImStack* fourLocation(int pos, int t){ return m_fourLocations.at(pos*m_nt + t); }
  int npos(){ return m_npos; }
  int nt(){ return m_nt; }
  std::string name(){ return m_name; }
  void setResolutionXY(double res){ m_resolutionXY = res; }
  double resolutionXY(){ return m_resolutionXY; }

};

#endif
