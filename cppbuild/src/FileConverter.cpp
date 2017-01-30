#include "FileConverter.hpp"

FileManager::input_file FileConverter::read(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename, int version)
{
  if(version == 0) return readV00(fm,kit,recs,filename);
  else return readV01(fm,kit,recs,filename);
}

void FileConverter::write(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename, int seriesID, int version)
{
  if(version == 0) writeV00(fm,kit,recs,filename,seriesID);
  else writeV01(fm,kit,recs,filename,seriesID);
}

FileManager::input_file FileConverter::readV00(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename)
{
  std::ifstream fin(filename.c_str(),std::ifstream::binary);
  FileManager::input_file infile;
  char buf[500];
  fin.read(buf,4);
  int len = NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]);
  fin.read(buf,len);
  infile.sname = std::string(buf,len);
  fin.read(buf,12);
  infile.nw = (int)buf[0];
  infile.nz = (int)buf[1];
  infile.np = (int)buf[2];
  infile.nt = (int)buf[3];
  char minibuf[4];
  for(int i = 4; i < 8; i++) minibuf[i-4] = buf[i];
  std::string ordString = std::string(minibuf);
  infile.order[3-ordString.find_first_of('W')] = FileManager::WAVELENGTH;
  infile.order[3-ordString.find_first_of('Z')] = FileManager::ZSLICE;
  infile.order[3-ordString.find_first_of('P')] = FileManager::POSITION;
  infile.order[3-ordString.find_first_of('T')] = FileManager::TIMEPOINT;
  int nfiles = NiaUtils::convertToInt(buf[8],buf[9],buf[10],buf[11]);
  for(int i = 0; i < nfiles; i++){
    fin.read(buf,4);
    len = NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]);
    fin.read(buf,len);
    std::string fname(buf,len);
    infile.fnames.push_back(fname);
  }
  kit->read(fin);
  if(recs){
    int prevSize = recs->size();
    fin.read(buf,1);
    int nrecs = infile.np*infile.nt;
    if((int)buf[0] < 1) nrecs *= infile.nz;
    for(int i = 0; i < nrecs; i++){
      ImRecord* rec = new ImRecord();
      rec->read(fin,0);
      recs->push_back(rec);
    }
    infile.resolutionXY = recs->at(prevSize)->resolutionXY();
  }
  //fm->addInputFile(infile);
  fin.close();
  return infile;
}

void FileConverter::writeV00(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename, int seriesID)
{
  std::ofstream fout(filename.c_str(),std::ofstream::binary);
  fm->saveInputFiles(fout,seriesID);
  kit->write(fout);
  char buf[1];
  if(recs->size() > fm->getNP(0)*fm->getNT(0)) buf[0] = 0;
  else buf[0] = 1;
  fout.write(buf,1);
  for(std::vector<ImRecord*>::iterator rit = recs->begin(); rit != recs->end(); rit++){
    (*rit)->write(fout,0);
  }
  fout.close();
}

FileManager::input_file FileConverter::readV01(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename)
{
  std::ifstream fin(filename.c_str(),std::ifstream::binary);
  FileManager::input_file infile;
  char buf[500];
  fin.read(buf,7);
  int version = NiaUtils::convertToInt(buf[3],buf[4],buf[5],buf[6]);
  fin.read(buf,4);
  int len = NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]);
  fin.read(buf,len);
  infile.sname = std::string(buf,len);
  fin.read(buf,12);
  infile.nw = (int)buf[0];
  infile.nz = (int)buf[1];
  infile.np = (int)buf[2];
  infile.nt = (int)buf[3];
  char minibuf[4];
  for(int i = 4; i < 8; i++) minibuf[i-4] = buf[i];
  std::string ordString = std::string(minibuf);
  infile.order[3-ordString.find_first_of('W')] = FileManager::WAVELENGTH;
  infile.order[3-ordString.find_first_of('Z')] = FileManager::ZSLICE;
  infile.order[3-ordString.find_first_of('P')] = FileManager::POSITION;
  infile.order[3-ordString.find_first_of('T')] = FileManager::TIMEPOINT;
  int nfiles = NiaUtils::convertToInt(buf[8],buf[9],buf[10],buf[11]);
  for(int i = 0; i < nfiles; i++){
    fin.read(buf,4);
    len = NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]);
    fin.read(buf,len);
    std::string fname(buf,len);
    infile.fnames.push_back(fname);
  }
  kit->read(fin);
  if(recs){
    int prevSize = recs->size();
    fin.read(buf,1);
    int nrecs = infile.np*infile.nt;
    if((int)buf[0] < 1) nrecs *= infile.nz;
    for(int i = 0; i < nrecs; i++){
      ImRecord* rec = new ImRecord();
      rec->read(fin,version);
      recs->push_back(rec);
    }
    infile.resolutionXY = recs->at(prevSize)->resolutionXY();
  }
  //fm->addInputFile(infile);
  fin.close();
  return infile;
}

void FileConverter::writeV01(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename, int seriesID)
{
  char buf[10];
  std::ofstream fout(filename.c_str(),std::ofstream::binary);
  buf[0] = 'V';
  buf[1] = 'V';
  buf[2] = 'V';
  NiaUtils::writeIntToBuffer(buf,3,nia::niaVersion);
  fout.write(buf,7);
  fm->saveInputFiles(fout,seriesID);
  kit->write(fout);
  if(recs->size() > fm->getNP(0)*fm->getNT(0)) buf[0] = 0;
  else buf[0] = 1;
  fout.write(buf,1);
  for(std::vector<ImRecord*>::iterator rit = recs->begin(); rit != recs->end(); rit++){
    (*rit)->write(fout,nia::niaVersion);
  }
  fout.close();
}
