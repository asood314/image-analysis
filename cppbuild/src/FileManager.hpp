#ifndef FILE_MANAGER_HPP
#define FILE_MANAGER_HPP

#include <vector>
#include "ImSeries.hpp"
#include <iostream>
#include <fstream>
#include <boost/thread.hpp>
#include "NiaBuffer.hpp"

class FileManager
{

public:
  enum Dimension{ WAVELENGTH=0,ZSLICE,POSITION,TIMEPOINT,NONE };

  typedef struct input_file{
    std::vector<std::string> fnames;
    std::string sname;
    uint8_t nw;
    uint8_t nz;
    uint8_t np;
    uint8_t nt;
    Dimension order[4];
    double resolutionXY;
  } input_file;

protected:
  std::vector<input_file> m_fileList;
  std::vector<input_file>::iterator m_it;

public:
  FileManager();
  virtual ~FileManager();
  ImSeries* load();
  ImSeries* loadNext();
  ImSeries* loadMT();
  int ntasks(bool zproject);
  bool empty(){ return m_fileList.size() == 0; }
  void reset(){ m_it = m_fileList.begin(); }
  void addInputFile(input_file phil){ m_fileList.push_back(phil); }
  void clearInputFiles(){ m_fileList.clear(); }
  void saveInputFiles(std::ofstream& fout, int index);
  std::string getName(int index){ return m_fileList.at(index).sname; }
  uint8_t getNW(int index){ return m_fileList.at(index).nw; }
  uint8_t getNZ(int index){ return m_fileList.at(index).nz; }
  uint8_t getNP(int index){ return m_fileList.at(index).np; }
  uint8_t getNT(int index){ return m_fileList.at(index).nt; }
  Dimension* getOrder(int index){ return m_fileList.at(index).order; }

};

#endif
