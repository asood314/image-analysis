#include "FileManager.hpp"

FileManager::FileManager()
{
  m_it = m_fileList.begin();
}

FileManager::~FileManager()
{
}

ImSeries* FileManager::load()
{
  int np = 0;
  int nt = 0;
  std::vector<ImStack*> stacks;
  int nx[4] = {1,1,1,1};
  for(m_it = m_fileList.begin(); m_it != m_fileList.end(); m_it++){
    for(int i = 0; i < m_it->np*m_it->nt; i++) stacks.push_back(new ImStack(m_it->nw,m_it->nz));
    Dimension* order = m_it->order;
    nx[WAVELENGTH] = m_it->nw;
    nx[ZSLICE] = m_it->nz;
    nx[POSITION] = np + m_it->np;
    nx[TIMEPOINT] = nt + m_it->nt;
    int ix[4] = {0,0,np,nt};
    np += m_it->np;
    nt += m_it->nt;
    for(std::vector<std::string>::iterator file_it = m_it->fnames.begin(); file_it != m_it->fnames.end(); file_it++){
      std::vector<ImFrame*> frames = ImFrame::load((*file_it).c_str());
      for(std::vector<ImFrame*>::iterator fit = frames.begin(); fit != frames.end(); fit++){
	stacks.at(ix[POSITION]*nx[TIMEPOINT] + ix[TIMEPOINT])->insert(*fit,ix[WAVELENGTH],ix[ZSLICE]);
	ix[order[3]]++;
	if(ix[order[3]] == nx[order[3]]){
	  ix[order[3]] = 0;
	  ix[order[2]]++;
	  if(ix[order[2]] == nx[order[2]]){
	    ix[order[2]] = 0;
	    ix[order[1]]++;
	    if(ix[order[1]] == nx[order[1]]){
	      ix[order[1]] = 0;
	      ix[order[0]]++;
	    }
	  }
	}
      }
    }
  }
  ImSeries* retVal = new ImSeries(np,nt);
  retVal->setName(m_fileList.begin()->sname);
  retVal->setResolutionXY(m_fileList.begin()->resolutionXY);
  for(int i = 0; i < np; i++){
    for(int j = 0; j < nt; j++){
      retVal->insert(stacks.at(i*nt+j),i,j);
    }
  }
  return retVal;
}

ImSeries* FileManager::loadNext()
{
  if(m_it == m_fileList.end()) return NULL;
  nia::nout << "Loading image series " << m_it->sname << "\n";
  std::vector<ImStack*> stacks;
  for(int i = 0; i < m_it->np*m_it->nt; i++) stacks.push_back(new ImStack(m_it->nw,m_it->nz));
  Dimension* order = m_it->order;
  int nx[4] = {1,1,1,1};
  nx[WAVELENGTH] = m_it->nw;
  nx[ZSLICE] = m_it->nz;
  nx[POSITION] = m_it->np;
  nx[TIMEPOINT] = m_it->nt;
  int ix[4] = {0,0,0,0};
  for(std::vector<std::string>::iterator file_it = m_it->fnames.begin(); file_it != m_it->fnames.end(); file_it++){
    std::vector<ImFrame*> frames = ImFrame::load((*file_it).c_str());
    for(std::vector<ImFrame*>::iterator fit = frames.begin(); fit != frames.end(); fit++){
      stacks.at(ix[POSITION]*nx[TIMEPOINT] + ix[TIMEPOINT])->insert(*fit,ix[WAVELENGTH],ix[ZSLICE]);
      ix[order[3]]++;
      if(ix[order[3]] == nx[order[3]]){
	ix[order[3]] = 0;
	ix[order[2]]++;
	if(ix[order[2]] == nx[order[2]]){
	  ix[order[2]] = 0;
	  ix[order[1]]++;
	  if(ix[order[1]] == nx[order[1]]){
	    ix[order[1]] = 0;
	    ix[order[0]]++;
	  }
	}
      }
    }
  }
  ImSeries* retVal = new ImSeries(nx[POSITION],nx[TIMEPOINT]);
  retVal->setName(m_it->sname);
  retVal->setResolutionXY(m_it->resolutionXY);
  for(int i = 0; i < nx[POSITION]; i++){
    for(int j = 0; j < nx[TIMEPOINT]; j++){
      retVal->insert(stacks.at(i*nx[TIMEPOINT]+j),i,j);
    }
  }
  m_it++;
  return retVal;
}

ImSeries* FileManager::loadMT()
{
  int np = 0;
  int nt = 0;
  std::vector<ImStack*> stacks;
  int nx[4] = {1,1,1,1};
  for(m_it = m_fileList.begin(); m_it != m_fileList.end(); m_it++){
    for(int i = 0; i < m_it->np*m_it->nt; i++) stacks.push_back(new ImStack(m_it->nw,m_it->nz));
    Dimension* order = m_it->order;
    nx[WAVELENGTH] = m_it->nw;
    nx[ZSLICE] = m_it->nz;
    nx[POSITION] = np + m_it->np;
    nx[TIMEPOINT] = nt + m_it->nt;
    int ix[4] = {0,0,np,nt};
    np += m_it->np;
    nt += m_it->nt;
    std::vector< std::vector<ImFrame*>* > frames;
    boost::thread_group tgroup;
    for(std::vector<std::string>::iterator file_it = m_it->fnames.begin(); file_it != m_it->fnames.end(); file_it++){
      std::vector<ImFrame*>* vec = new std::vector<ImFrame*>();
      frames.push_back(vec);
      tgroup.create_thread(boost::bind(&ImFrame::loadMT,vec,(*file_it).c_str()));
    }
    tgroup.join_all();
    for(std::vector< std::vector<ImFrame*>* >::iterator fit = frames.begin(); fit != frames.end(); fit++){
      for(std::vector<ImFrame*>::iterator fit2 = (*fit)->begin(); fit2 != (*fit)->end(); fit2++){
	stacks.at(ix[POSITION]*nx[TIMEPOINT] + ix[TIMEPOINT])->insert(*fit2,ix[WAVELENGTH],ix[ZSLICE]);
	ix[order[3]]++;
	if(ix[order[3]] == nx[order[3]]){
	  ix[order[3]] = 0;
	  ix[order[2]]++;
	  if(ix[order[2]] == nx[order[2]]){
	    ix[order[2]] = 0;
	    ix[order[1]]++;
	    if(ix[order[1]] == nx[order[1]]){
	      ix[order[1]] = 0;
	      ix[order[0]]++;
	    }
	  }
	}
      }
      delete *fit;
    }
  }
  ImSeries* retVal = new ImSeries(np,nt);
  retVal->setName(m_fileList.begin()->sname);
  retVal->setResolutionXY(m_fileList.begin()->resolutionXY);
  for(int i = 0; i < np; i++){
    for(int j = 0; j < nt; j++){
      retVal->insert(stacks.at(i*nt+j),i,j);
    }
  }
  return retVal;
}

void FileManager::saveInputFiles(std::ofstream& fout, int index)
{
  char buf[20];
  std::vector<input_file>::iterator fit = m_fileList.begin()+index;
  NiaUtils::writeIntToBuffer(buf,0,fit->sname.length());
  fout.write(buf,4);
  fout.write(fit->sname.c_str(),fit->sname.length());
  buf[0] = (char)(fit->nw);
  buf[1] = (char)(fit->nz);
  buf[2] = (char)(fit->np);
  buf[3] = (char)(fit->nt);
  for(int8_t j = 3; j >= 0; j--){
    if(fit->order[j] == FileManager::WAVELENGTH) buf[7-j] = 'W';
    else if(fit->order[j] == FileManager::ZSLICE) buf[7-j] = 'Z';
    else if(fit->order[j] == FileManager::POSITION) buf[7-j] = 'P';
    else if(fit->order[j] == FileManager::TIMEPOINT) buf[7-j] = 'T';
  }
  NiaUtils::writeIntToBuffer(buf,8,fit->fnames.size());
  fout.write(buf,12);
  for(std::vector<std::string>::iterator it = fit->fnames.begin(); it != fit->fnames.end(); it++){
    NiaUtils::writeIntToBuffer(buf,0,it->length());
    fout.write(buf,4);
    fout.write(it->c_str(),it->length());
  }
}

int FileManager::ntasks(bool zproject)
{
  int retVal = 0;
  for(m_it = m_fileList.begin(); m_it != m_fileList.end(); m_it++){
    if(zproject) retVal += m_it->np * m_it->nt;
    else retVal += m_it->np * m_it->nt * m_it->nz;
  }
  return retVal;
}

std::string FileManager::orderString(Dimension* order)
{
  std::string retVal = "WZTP";
  for(int i = 3; i >= 0; i--){
    if(order[i] == WAVELENGTH) retVal[3-i] = 'W';
    else if(order[i] == ZSLICE) retVal[3-i] = 'Z';
    else if(order[i] == TIMEPOINT) retVal[3-i] = 'T';
    else if(order[i] == POSITION) retVal[3-i] = 'P';
  }
  return retVal;
}
