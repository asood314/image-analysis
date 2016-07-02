#ifndef IMSERIES_HPP
#define IMSERIES_HPP

#include "ImStack.hpp"

class ImSeries
{

protected:
  std::vector<ImStack*> m_fourLocations;
  uint8_t m_npos;
  uint8_t m_nt;
  std::string m_name;

public:
  ImSeries();
  ImSeries(uint8_t npos, uint8_t nt);
  ~ImSeries();
  void insert(ImStack* stack, uint8_t pos, uint8_t t);
  void setName(std::string nm){ m_name = nm; }
  ImStack* fourLocation(uint8_t pos, uint8_t t){ return m_fourLocations.at(pos*m_nt + t); }
  uint8_t npos(){ return m_npos; }
  uint8_t nt(){ return m_nt; }
  std::string name(){ return m_name; }

};

#endif
