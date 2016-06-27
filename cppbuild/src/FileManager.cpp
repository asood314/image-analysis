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
  uint8_t np = 0;
  uint8_t nt = 0;
  std::vector<ImStack*> stacks;
  uint8_t nx[4] = {1,1,1,1};
  for(m_it = m_fileList.begin(); m_it != m_fileList.end(); m_it++){
    for(uint8_t i = 0; i < m_it->np*m_it->nt; i++) stacks.push_back(new ImStack(m_it->nw,m_it->nz));
    Dimension* order = m_it->order;
    nx[WAVELENGTH] = m_it->nw;
    nx[ZSLICE] = m_it->nz;
    nx[POSITION] = np + m_it->np;
    nx[TIMEPOINT] = nt + m_it->nt;
    uint8_t ix[4] = {0,0,np,nt};
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
  std::vector<ImStack*> stacks;
  for(uint8_t i = 0; i < m_it->np*m_it->nt; i++) stacks.push_back(new ImStack(m_it->nw,m_it->nz));
  Dimension* order = m_it->order;
  uint8_t nx[4] = {1,1,1,1};
  nx[WAVELENGTH] = m_it->nw;
  nx[ZSLICE] = m_it->nz;
  nx[POSITION] = m_it->np;
  nx[TIMEPOINT] = m_it->nt;
  uint8_t ix[4] = {0,0,0,0};
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
  m_it++;
  ImSeries* retVal = new ImSeries(nx[POSITION],nx[TIMEPOINT]);
  for(int i = 0; i < nx[POSITION]; i++){
    for(int j = 0; j < nx[TIMEPOINT]; j++){
      retVal->insert(stacks.at(i*nx[TIMEPOINT]+j),i,j);
    }
  }
  return retVal;
}
