#ifndef STORM_DATA_HPP
#define STORM_DATA_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

class StormData{

public:
  typedef struct localization{
    int frame,clusterID;
    double x,y,z,sigma1,sigma2,intensity;
  } Blink;

protected:
  std::vector<Blink> m_molecules;

public:
  StormData(){}
  StormData(std::string infile){ read(infile); }
  virtual ~StormData(){}
  void read(std::string infile);
  Blink molecule(int i){ return m_molecules[i]; }
  int nMolecules(){ return m_molecules.size(); }
  void setClusterID(int i, int id){ m_molecules[i].clusterID = id; }
  bool isClustered(int i){ return (m_molecules[i].clusterID >= 0); }
  
};

#endif
