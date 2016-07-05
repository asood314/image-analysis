#include "BatchService.hpp"
#include <sstream>

BatchService::BatchService()
{
  m_name = "job0_";
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

void BatchService::run()
{
  m_data.push_back(m_fileManager->loadNext());
  m_activeThreads = 0;
  int currentS = 0;
  m_nStacks.push_back(m_data.at(0)->npos()*m_data.at(0)->nt());
  if(!m_zproject){
    std::vector<int> nvec;
    std::vector<ImRecord*> rvec;
    for(int i = 0; i < m_data.at(0)->npos(); i++){
      for(int j = 0; j < m_data.at(0)->nt(); j++){
	int nz = m_data.at(0)->fourLocation(i,j)->nz();
	nvec.push_back(nz);
	for(int k = 0; k < nz; k++) rvec.push_back(NULL);
      }
    }
    m_nPlanes.push_back(nvec);
    m_records.push_back(rvec);
  }
  else{
    std::vector<ImRecord*> rvec;
    rvec.assign(m_data.at(0)->npos()*m_data.at(0)->nt(),NULL);
    m_records.push_back(rvec);
  }
  while(m_data.at(currentS)){
    for(uint8_t currentP = 0; currentP < m_data.at(currentS)->npos(); currentP++){
      for(uint8_t currentT = 0; currentT < m_data.at(currentS)->nt(); currentT++){
	if(m_zproject){
	  while(m_activeThreads == m_maxThreads) boost::this_thread::sleep(boost::posix_time::millisec(60000));
	  m_mtx.lock();
	  m_threadpool.create_thread(boost::bind(&BatchService::analyzeProjection, this, currentS, currentP, currentT));
	  m_activeThreads++;
	  for(int i = 0; i < currentS; i++){
	    if(m_nStacks.at(i) == 0){
	      std::ostringstream filename;
	      filename << m_name << m_fileManager->getName(i) << ".nia";
	      std::ofstream fout(filename.str().c_str(),std::ofstream::binary);
	      m_fileManager->saveInputFiles(fout,i);
	      m_iat->write(fout);
	      char buf[1];
	      if(m_zproject) buf[0] = 1;
	      else buf[0] = 0;
	      fout.write(buf,1);
	      for(std::vector<ImRecord*>::iterator rit = m_records.at(i).begin(); rit != m_records.at(i).end(); rit++){
		(*rit)->write(fout);
		delete *rit;
	      }
	      fout.close();
	      m_nStacks.at(i) = -1;
	    }
	  }
	  m_mtx.unlock();
	}
	else{
	  for(uint8_t currentZ = 0; currentZ < m_data.at(currentS)->fourLocation(currentP,currentT)->nz(); currentZ++){
	    while(m_activeThreads == m_maxThreads) boost::this_thread::sleep(boost::posix_time::millisec(60000));
	    m_mtx.lock();
	    m_threadpool.create_thread(boost::bind(&BatchService::analyzePlane, this, currentS, currentP, currentT, currentZ));
	    m_activeThreads++;
	    for(int i = 0; i < currentS; i++){
	      if(m_nStacks.at(i) == 0){
		std::ofstream fout(m_fileManager->getName(currentS),std::ofstream::binary);
		m_fileManager->saveInputFiles(fout,currentS);
		m_iat->write(fout);
		char buf[1];
		if(m_zproject) buf[0] = 1;
		else buf[0] = 0;
		fout.write(buf,1);
		for(std::vector<ImRecord*>::iterator rit = m_records.at(i).begin(); rit != m_records.at(i).end(); rit++){
		  (*rit)->write(fout);
		  delete *rit;
		}
		fout.close();
		m_nStacks.at(i) = -1;
	      }
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
    m_nStacks.push_back(m_data.at(currentS)->npos()*m_data.at(currentS)->nt());
    if(!m_zproject){
      std::vector<int> nvec;
      std::vector<ImRecord*> rvec;
      for(int i = 0; i < m_data.at(currentS)->npos(); i++){
	for(int j = 0; j < m_data.at(currentS)->nt(); j++){
	  int nz = m_data.at(currentS)->fourLocation(i,j)->nz();
	  nvec.push_back(nz);
	  for(int k = 0; k < nz; k++) rvec.push_back(NULL);
	}
      }
      m_nPlanes.push_back(nvec);
      m_records.push_back(rvec);
    }
    else{
      std::vector<ImRecord*> rvec;
      rvec.assign(m_data.at(currentS)->npos()*m_data.at(currentS)->nt(),NULL);
      m_records.push_back(rvec);
    }
  }
  m_threadpool.join_all();
  for(std::vector<ImSeries*>::iterator it = m_data.begin(); it != m_data.end(); it++) delete *it;
  m_data.clear();
  m_nStacks.clear();
  m_nPlanes.clear();
  m_records.clear();
  m_fileManager->clearInputFiles();
}

void BatchService::analyzeProjection(int seriesID, uint8_t p, uint8_t t)
{
  ImStack* stack = m_data.at(seriesID)->fourLocation(p,t);
  ImRecord* record = new ImRecord(stack->nwaves(),stack->frame(0,0)->width(),stack->frame(0,0)->height());
  record->setResolutionXY(m_data.at(seriesID)->resolutionXY());
  std::vector<SynapseCollection*> syncol = m_iat->synapseDefinitions();
  for(std::vector<SynapseCollection*>::iterator it = syncol.begin(); it != syncol.end(); it++) record->addSynapseCollection((*it)->emptyCopy());
  m_iat->standardAnalysis(stack,record,-1);
  boost::lock_guard<boost::mutex> guard(m_mtx);
  m_records.at(seriesID).at(p*m_data.at(seriesID)->nt() + t) = record;
  m_nStacks.at(seriesID) -= 1;
  delete stack;
  m_activeThreads--;
}

void BatchService::analyzePlane(int seriesID, uint8_t p, uint8_t t, uint8_t z)
{
  ImStack* stack = m_data.at(seriesID)->fourLocation(p,t);
  ImRecord* record = new ImRecord(stack->nwaves(),stack->frame(0,z)->width(),stack->frame(0,z)->height());
  record->setResolutionXY(m_data.at(seriesID)->resolutionXY());
  std::vector<SynapseCollection*> syncol = m_iat->synapseDefinitions();
  for(std::vector<SynapseCollection*>::iterator it = syncol.begin(); it != syncol.end(); it++) record->addSynapseCollection((*it)->emptyCopy());
  m_iat->standardAnalysis(stack,record,z);
  boost::lock_guard<boost::mutex> guard(m_mtx);
  m_records.at(seriesID).at(p*m_data.at(seriesID)->nt()*stack->nz() + t*stack->nz() + z) = record;
  m_nPlanes.at(seriesID).at(p*m_data.at(seriesID)->nt()+t) -= 1;
  m_activeThreads--;
}
