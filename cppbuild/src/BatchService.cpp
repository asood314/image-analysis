#include "BatchService.hpp"
#include <sstream>

BatchService::BatchService()
{
  m_name = "job0_";
  m_maxThreads = boost::thread::hardware_concurrency() - 1;
  m_zproject = true;
  m_divisor = 1;
  m_writeTables = false;
  m_iat = new ImageAnalysisToolkit();
  m_fileManager = new FileManager();
  m_progressWindow = NULL;
}

BatchService::~BatchService()
{
  delete m_iat;
  delete m_fileManager;
}

void BatchService::run()
{
  if(m_fileManager->empty()) return;
  m_fileManager->reset();
  m_data.push_back(m_fileManager->loadNext());
  if(m_divisor > 1) m_data[0]->divide(m_divisor);
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
    for(int currentP = 0; currentP < m_data.at(currentS)->npos(); currentP++){
      for(int currentT = 0; currentT < m_data.at(currentS)->nt(); currentT++){
	if(m_zproject){
	  while(m_activeThreads == m_maxThreads) boost::this_thread::sleep(boost::posix_time::millisec(60000));
	  m_mtx.lock();
	  m_threadpool.create_thread(boost::bind(&BatchService::analyzeProjection, this, currentS, currentP, currentT));
	  m_activeThreads++;
	  boost::this_thread::sleep(boost::posix_time::millisec(1000));
	  m_mtx.unlock();
	}
	else{
	  for(int currentZ = 0; currentZ < m_data.at(currentS)->fourLocation(currentP,currentT)->nz(); currentZ++){
	    while(m_activeThreads == m_maxThreads) boost::this_thread::sleep(boost::posix_time::millisec(60000));
	    m_mtx.lock();
	    m_threadpool.create_thread(boost::bind(&BatchService::analyzePlane, this, currentS, currentP, currentT, currentZ));
	    m_activeThreads++;
	    boost::this_thread::sleep(boost::posix_time::millisec(1000));
	    m_mtx.unlock();
	  }
	}
      }
    }
    boost::this_thread::sleep(boost::posix_time::millisec(1000));
    m_data.push_back(m_fileManager->loadNext());
    currentS++;
    if(!(m_data.at(currentS))) continue;
    if(m_divisor > 1) m_data[currentS]->divide(m_divisor);
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
  for(std::vector<ImSeries*>::iterator it = m_data.begin(); it != m_data.end(); it++){
    if(*it) delete *it;
  }
  m_data.clear();
  m_nStacks.clear();
  m_nPlanes.clear();
  m_records.clear();
  m_fileManager->clearInputFiles();
}

void BatchService::run2(std::vector<ImRecord*> recs)
{
  if(m_fileManager->empty()) return;
  m_fileManager->reset();
  m_data.push_back(m_fileManager->loadNext());
  if(m_divisor > 1) m_data[0]->divide(m_divisor);
  m_activeThreads = 0;
  int currentS = 0;
  m_nStacks.push_back(m_data.at(0)->npos()*m_data.at(0)->nt());
  int recIndex = 0;
  if(!m_zproject){
    std::vector<int> nvec;
    std::vector<ImRecord*> rvec;
    for(int i = 0; i < m_data.at(0)->npos(); i++){
      for(int j = 0; j < m_data.at(0)->nt(); j++){
	int nz = m_data.at(0)->fourLocation(i,j)->nz();
	nvec.push_back(nz);
	for(int k = 0; k < nz; k++){
	  rvec.push_back(recs[recIndex]);
	  recIndex++;
	}
      }
    }
    m_nPlanes.push_back(nvec);
    m_records.push_back(rvec);
  }
  else{
    int np = m_data.at(0)->npos();
    int nt = m_data.at(0)->nt();
    int nz = m_data.at(0)->fourLocation(0,0)->nz();
    if(recs.size() > np*nt){
      unsigned index = 0;
      for(int p = 0; p < np; p++){
	for(int t = 0; p < nt; t++){
	  index++;
	  for(int z = 1; z < nz; z++){
	    //if(recs[index]) delete recs[index];
	    recs.erase(recs.begin()+index);
	  }
	}
      }
    }
    std::vector<ImRecord*> rvec;
    for(int i = 0; i < m_data.at(0)->npos()*m_data.at(0)->nt(); i++){
      rvec.push_back(recs[recIndex]);
      recIndex++;
    }
    m_records.push_back(rvec);
  }
  while(m_data.at(currentS)){
    for(int currentP = 0; currentP < m_data.at(currentS)->npos(); currentP++){
      for(int currentT = 0; currentT < m_data.at(currentS)->nt(); currentT++){
	if(m_zproject){
	  while(m_activeThreads == m_maxThreads) boost::this_thread::sleep(boost::posix_time::millisec(60000));
	  m_mtx.lock();
	  m_threadpool.create_thread(boost::bind(&BatchService::analyzeProjection, this, currentS, currentP, currentT));
	  m_activeThreads++;
	  boost::this_thread::sleep(boost::posix_time::millisec(1000));
	  m_mtx.unlock();
	}
	else{
	  for(int currentZ = 0; currentZ < m_data.at(currentS)->fourLocation(currentP,currentT)->nz(); currentZ++){
	    while(m_activeThreads == m_maxThreads) boost::this_thread::sleep(boost::posix_time::millisec(60000));
	    m_mtx.lock();
	    m_threadpool.create_thread(boost::bind(&BatchService::analyzePlane, this, currentS, currentP, currentT, currentZ));
	    m_activeThreads++;
	    boost::this_thread::sleep(boost::posix_time::millisec(1000));
	    m_mtx.unlock();
	  }
	}
      }
    }
    boost::this_thread::sleep(boost::posix_time::millisec(1000));
    m_data.push_back(m_fileManager->loadNext());
    currentS++;
    if(!(m_data.at(currentS))) continue;
    if(m_divisor > 1) m_data[currentS]->divide(m_divisor);
    m_nStacks.push_back(m_data.at(currentS)->npos()*m_data.at(currentS)->nt());
    if(!m_zproject){
      std::vector<int> nvec;
      std::vector<ImRecord*> rvec;
      for(int i = 0; i < m_data.at(currentS)->npos(); i++){
	for(int j = 0; j < m_data.at(currentS)->nt(); j++){
	  int nz = m_data.at(currentS)->fourLocation(i,j)->nz();
	  nvec.push_back(nz);
	  for(int k = 0; k < nz; k++){
	    rvec.push_back(recs[recIndex]);
	    recIndex++;
	  }
	}
      }
      m_nPlanes.push_back(nvec);
      m_records.push_back(rvec);
    }
    else{
      std::vector<ImRecord*> rvec;
      for(int i = 0; i < m_data.at(currentS)->npos()*m_data.at(currentS)->nt(); i++){
	rvec.push_back(recs[recIndex]);
	recIndex++;
      }
      m_records.push_back(rvec);
    }
  }
  m_threadpool.join_all();
  for(std::vector<ImSeries*>::iterator it = m_data.begin(); it != m_data.end(); it++){
    if(*it) delete *it;
  }
  m_data.clear();
  m_nStacks.clear();
  m_nPlanes.clear();
  m_records.clear();
  m_fileManager->clearInputFiles();
}

void BatchService::analyzeProjection(int seriesID, int p, int t)
{
  //---------- Windows only ----------
  SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED);
  //----------------------------------
  nia::nout << "Starting sample " << m_fileManager->getName(seriesID) << ", xy position " << p << ", timepoint " << t << "\n";
  ImStack* stack = m_data.at(seriesID)->fourLocation(p,t);
  ImRecord* record = m_records.at(seriesID).at(p*m_data.at(seriesID)->nt() + t);
  if(!record){
    record = new ImRecord(stack->nwaves(),stack->frame(0,0)->width(),stack->frame(0,0)->height());
    record->setResolutionXY(m_data.at(seriesID)->resolutionXY());
    m_records.at(seriesID).at(p*m_data.at(seriesID)->nt() + t) = record;
  }
  for(int i = 0; i < stack->nwaves(); i++) record->setChannelName(i,m_iat->getChannelName(i));
  std::vector<SynapseCollection*> syncol = m_iat->synapseDefinitions();
  for(std::vector<SynapseCollection*>::iterator it = syncol.begin(); it != syncol.end(); it++) record->addSynapseCollection((*it)->emptyCopy());
  m_iat->standardAnalysis(stack,record,-1);
  if(m_writeTables){
    std::ostringstream tablename;
    tablename << m_name << "_" << m_fileManager->getName(seriesID) << "_xy" << p << "_t" << t << ".csv";
    record->printSynapseDensityTable(m_iat->postChan(),tablename.str());
  }
  boost::lock_guard<boost::mutex> guard(m_mtx);
  m_nStacks.at(seriesID) -= 1;
  m_data.at(seriesID)->remove(p,t);
  if(m_nStacks.at(seriesID) == 0){
    std::ostringstream filename;
    filename << m_name << "_" << m_fileManager->getName(seriesID) << ".nia";
    std::ofstream fout(filename.str().c_str(),std::ofstream::binary);
    m_fileManager->saveInputFiles(fout,seriesID);
    m_iat->write(fout);
    char buf[1];
    buf[0] = 1;
    fout.write(buf,1);
    for(std::vector<ImRecord*>::iterator rit = m_records.at(seriesID).begin(); rit != m_records.at(seriesID).end(); rit++){
      (*rit)->write(fout);
      delete *rit;
    }
    fout.close();
  }
  m_activeThreads--;
  if(m_progressWindow) m_progressWindow->taskCompleted();
  //---------- Windows only ----------
  SetThreadExecutionState(ES_CONTINUOUS);
  //----------------------------------
}

void BatchService::analyzePlane(int seriesID, int p, int t, int z)
{
  //---------- Windows only ----------
  SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED);
  //----------------------------------
  nia::nout << "Starting sample " << m_fileManager->getName(seriesID) << ", xy position " << p << ", timepoint " << t << ", z-plane " << z << "\n";
  ImStack* stack = m_data.at(seriesID)->fourLocation(p,t);
  ImRecord* record = m_records.at(seriesID).at(p*m_data.at(seriesID)->nt()*stack->nz() + t*stack->nz() + z);
  if(!record){
    record = new ImRecord(stack->nwaves(),stack->frame(0,z)->width(),stack->frame(0,z)->height());
    record->setResolutionXY(m_data.at(seriesID)->resolutionXY());
    m_records.at(seriesID).at(p*m_data.at(seriesID)->nt()*stack->nz() + t*stack->nz() + z) = record;
  }
  for(int i = 0; i < stack->nwaves(); i++) record->setChannelName(i,m_iat->getChannelName(i));
  std::vector<SynapseCollection*> syncol = m_iat->synapseDefinitions();
  for(std::vector<SynapseCollection*>::iterator it = syncol.begin(); it != syncol.end(); it++) record->addSynapseCollection((*it)->emptyCopy());
  m_iat->standardAnalysis(stack,record,z);
  if(m_writeTables){
    std::ostringstream tablename;
    tablename << m_name << "_" << m_fileManager->getName(seriesID) << "_xy" << p << "_t" << t << "_z" << z << ".csv";
    record->printSynapseDensityTable(m_iat->postChan(),tablename.str());
  }
  boost::lock_guard<boost::mutex> guard(m_mtx);
  m_nPlanes.at(seriesID).at(p*m_data.at(seriesID)->nt()+t) -= 1;
  if(m_nPlanes.at(seriesID).at(p*m_data.at(seriesID)->nt()+t) == 0){
    m_data.at(seriesID)->remove(p,t);
    m_nStacks.at(seriesID) -= 1;
    if(m_nStacks.at(seriesID) == 0){
      std::ostringstream filename;
      filename << m_name << "_" << m_fileManager->getName(seriesID) << ".nia";
      std::ofstream fout(filename.str().c_str(),std::ofstream::binary);
      m_fileManager->saveInputFiles(fout,seriesID);
      m_iat->write(fout);
      char buf[1];
      buf[0] = 0;
      fout.write(buf,1);
      for(std::vector<ImRecord*>::iterator rit = m_records.at(seriesID).begin(); rit != m_records.at(seriesID).end(); rit++){
	(*rit)->write(fout);
	delete *rit;
      }
      fout.close();
    }
  }
  m_activeThreads--;
  if(m_progressWindow) m_progressWindow->taskCompleted();
  //---------- Windows only ----------
  SetThreadExecutionState(ES_CONTINUOUS);
  //----------------------------------
}
