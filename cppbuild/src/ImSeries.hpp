#ifndef IMSERIES_HPP
#define IMSERIES_HPP

#include "ImStack.hpp"

class ImSeries
{

protected:
  ImStack*** m_fourLocations;
  uint8_t m_npos;
  uint8_t m_nt;

public:
  ImSeries();
  ImSeries(uint8_t npos, uint8_t nt);
  ~ImSeries();
  void insert(ImStack* stack, uint8_t pos, uint8_t t);
  ImStack* fourLocation(uint8_t pos, uint8_t t){ return m_fourLocations[pos][t]; }
  uint8_t npos(){ return m_npos; }
  uint8_t nt(){ return m_nt; }

};

#endif
