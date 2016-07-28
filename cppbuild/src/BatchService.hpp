#ifndef BATCH_SERVICE_HPP
#define BATCH_SERVICE_HPP

//---------- Windows only ----------
#include <windows.h>
//----------------------------------

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include "FileManager.hpp"
#include "ImageAnalysisToolkit.hpp"
#include "NiaBuffer.hpp"
#include "BatchWindow.hpp"

class BatchService
{

protected:
  std::string m_name;
  boost::thread_group m_threadpool;
  boost::mutex m_mtx;
  int m_maxThreads,m_activeThreads;
  bool m_zproject;
  int m_divisor;
  bool m_writeTables;
  ImageAnalysisToolkit* m_iat;
  FileManager* m_fileManager;
  std::vector<ImSeries*> m_data;
  std::vector<int> m_nStacks;
  std::vector< std::vector<int> >m_nPlanes;
  std::vector< std::vector<ImRecord*> > m_records;
  BatchWindow* m_progressWindow;

public:
  BatchService();
  virtual ~BatchService();
  void analyzeProjection(int seriesID, int p, int t);
  void analyzePlane(int seriesID, int p, int t, int z);
  void run();
  void run2(std::vector<ImRecord*> recs);
  void setName(std::string name){ m_name = name; }
  std::string name(){ return m_name; }
  void setMaxThreads(int max){ m_maxThreads = max; }
  int maxThreads(){ return m_maxThreads; }
  void setZProject(bool tf){ m_zproject = tf; }
  bool zproject(){ return m_zproject; }
  void setDivisor(int d){ m_divisor = d; }
  int divisor(){ return m_divisor; }
  void setWriteTables(bool tf){ m_writeTables = tf; }
  bool writeTables(){ return m_writeTables; }
  ImageAnalysisToolkit* iat(){ return m_iat; };
  FileManager* fileManager(){ return m_fileManager; }
  void setProgressWindow(BatchWindow* bw){ m_progressWindow = bw; }
  
};

#endif
