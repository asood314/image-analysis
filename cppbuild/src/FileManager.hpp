#ifndef FILE_MANAGER_HPP
#define FILE_MANAGER_HPP

#include <vector>
#include "ImSeries.hpp"
#include <iostream>
#include <fstream>

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
  } input_file;

protected:
  std::vector<input_file> m_fileList;
  std::vector<input_file>::iterator m_it;

public:
  FileManager();
  virtual ~FileManager();
  ImSeries* load();
  ImSeries* loadNext();
  void addInputFile(input_file phil){ m_fileList.push_back(phil); }
  void clearInputFiles(){ m_fileList.clear(); }
  void saveFileList(std::ofstream& fout);

};

#endif
