#include "BatchService.hpp"
#include <sstream>

BatchService::BatchService()
{
  m_name = "job0";
  m_maxThreads = boost::thread::hardware_concurrency() - 1;
  m_zproject = true;
  m_iat = new ImageAnalysisToolkit();
  m_fileManager = new FileManager();
}

BatchService::~BatchService()
{
  delete m_iat;
  delete m_fileManager;
}

void BatchService::saveJobInfo()
{
  std::ostringstream filename;
  filename << m_name << ".batch";
  std::ofstream fout(filename.str().c_str());
  m_fileManager->saveFileList(fout);
  fout.close();
}

void BatchService::run()
{
  m_data.push_back(m_fileManager->loadNext());
  m_activeThreads = 0;
  int currentS = 0;
  if(!m_zproject){
    m_nStacks.push_back(m_data.at(0)->npos()*m_data.at(0)->nt());
    std::vector<int> nvec;
    for(int i = 0; i < m_data.at(0)->npos(); i++){
      for(int j = 0; j < m_data.at(0)->nt(); j++) nvec.push_back(m_data.at(0)->fourLocation(i,j)->nz());
    }
    m_nPlanes.push_back(nvec);
  }
  while(m_data.at(currentS)){
    for(uint8_t currentP = 0; currentP < m_data.at(currentS)->npos(); currentP++){
      for(uint8_t currentT = 0; currentT < m_data.at(currentS)->nt(); currentT++){
	if(m_zproject){
	  while(m_activeThreads == m_maxThreads) boost::this_thread::sleep(boost::posix_time::millisec(60000));
	  m_mtx.lock();
	  m_threadpool.create_thread(boost::bind(&BatchService::analyzeProjection, this, currentS, currentP, currentT));
	  m_activeThreads++;
	  m_mtx.unlock();
	}
	else{
	  for(uint8_t currentZ = 0; currentZ < m_data.at(currentS)->fourLocation(currentP,currentT)->nz(); currentZ++){
	    while(m_activeThreads == m_maxThreads) boost::this_thread::sleep(boost::posix_time::millisec(60000));
	    m_mtx.lock();
	    m_threadpool.create_thread(boost::bind(&BatchService::analyzePlane, this, currentS, currentP, currentT, currentZ));
	    m_activeThreads++;
	    for(int i = 0; i < currentS; i++){
	      if(m_nStacks.at(i) == 0) continue;
	      for(int k = 0; k < m_data.at(i)->npos(); k++){
		for(int j = 0; j < m_data.at(i)->nt(); j++){
		  if(m_nPlanes.at(i).at(k*m_data.at(i)->nt()+j) == 0){
		    delete m_data.at(i)->fourLocation(k,j);
		    m_nPlanes.at(i).at(k*m_data.at(i)->nt()+j) = -1;
		    m_nStacks.at(i) -= 1;
		  }
		}
	      }
	    }
	    m_mtx.unlock();
	  }
	}
      }
    }
    m_data.push_back(m_fileManager->loadNext());
    currentS++;
    if(!m_zproject){
      m_nStacks.push_back(m_data.at(currentS)->npos()*m_data.at(currentS)->nt());
      std::vector<int> nvec;
      for(int i = 0; i < m_data.at(currentS)->npos(); i++){
	for(int j = 0; j < m_data.at(currentS)->nt(); j++) nvec.push_back(m_data.at(currentS)->fourLocation(i,j)->nz());
      }
      m_nPlanes.push_back(nvec);
    }
  }
  m_threadpool.join_all();
  saveJobInfo();
  for(std::vector<ImSeries*>::iterator it = m_data.begin(); it != m_data.end(); it++) delete *it;
  m_data.clear();
  m_nStacks.clear();
  m_nPlanes.clear();
}

void BatchService::analyzeProjection(int seriesID, uint8_t p, uint8_t t)
{
  ImStack* stack = m_data.at(seriesID)->fourLocation(p,t);
  ImRecord* record = new ImRecord(stack->nwaves(),stack->frame(0,0)->width(),stack->frame(0,0)->height());
  m_iat->standardAnalysis(stack,record,-1);
  std::ostringstream filename;
  filename << m_data.at(seriesID)->name() << "_p" << p << "_t" << t << ".nia";
  std::ofstream fout(filename.str().c_str(),std::ofstream::binary);
  m_iat->write(fout);
  record->write(fout);
  fout.close();
  delete record;
  boost::lock_guard<boost::mutex> guard(m_mtx);
  m_activeThreads--;
  delete stack;
}

void BatchService::analyzePlane(int seriesID, uint8_t p, uint8_t t, uint8_t z)
{
  ImStack* stack = m_data.at(seriesID)->fourLocation(p,t);
  ImRecord* record = new ImRecord(stack->nwaves(),stack->frame(0,z)->width(),stack->frame(0,z)->height());
  m_iat->standardAnalysis(stack,record,z);
  std::ostringstream filename;
  filename << m_data.at(seriesID)->name() << "_p" << p << "_t" << t << "_z" << z << ".nia";
  std::ofstream fout(filename.str().c_str(),std::ofstream::binary);
  m_iat->write(fout);
  record->write(fout);
  fout.close();
  delete record;
  boost::lock_guard<boost::mutex> guard(m_mtx);
  m_activeThreads--;
  m_nPlanes.at(seriesID).at(p*m_data.at(seriesID)->nt()+t) -= 1;
}
