#ifndef BATCH_SERVICE_HPP
#define BATCH_SERVICE_HPP

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include "FileManager.hpp"
#include "ImageAnalysisToolkit.hpp"

class BatchService
{

protected:
  std::string m_name;
  boost::thread_group m_threadpool;
  boost::mutex m_mtx;
  uint8_t m_maxThreads,m_activeThreads;
  bool m_zproject;
  ImageAnalysisToolkit* m_iat;
  FileManager* m_fileManager;
  std::vector<ImSeries*> m_data;
  std::vector<int> m_nStacks;
  std::vector< std::vector<int> >m_nPlanes;

public:
  BatchService();
  virtual ~BatchService();
  void saveJobInfo();
  void analyzeProjection(int seriesID, uint8_t p, uint8_t t);
  void analyzePlane(int seriesID, uint8_t p, uint8_t t, uint8_t z);
  void run();
  void setMaxThreads(uint8_t max){ m_maxThreads = max; }
  ImageAnalysisToolkit* iat(){ return m_iat; };
  FileManager* fileManager(){ return m_fileManager; }
  
};

#endif
