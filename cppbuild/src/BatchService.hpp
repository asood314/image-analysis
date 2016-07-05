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
  std::vector< std::vector<ImRecord*> > m_records;

public:
  BatchService();
  virtual ~BatchService();
  void analyzeProjection(int seriesID, uint8_t p, uint8_t t);
  void analyzePlane(int seriesID, uint8_t p, uint8_t t, uint8_t z);
  void run();
  void setName(std::string name){ m_name = name; }
  std::string name(){ return m_name; }
  void setMaxThreads(uint8_t max){ m_maxThreads = max; }
  uint8_t maxThreads(){ return m_maxThreads; }
  void setZProject(bool tf){ m_zproject = tf; }
  bool zproject(){ return m_zproject; }
  ImageAnalysisToolkit* iat(){ return m_iat; };
  FileManager* fileManager(){ return m_fileManager; }
  
};

#endif
