#include "ImFrame.hpp"
#include <cmath>

ImFrame::ImFrame()
{
  m_width = 2048;
  m_height = 2048;
  m_pixels.assign(2048,std::vector<int>(2048));
  //for(int i = 0; i < 2048; i++) m_pixels[i] = new int[2048];
}

ImFrame::ImFrame(const int width, const int height)
{
  m_width = width;
  m_height = height;
  m_pixels.assign(width,std::vector<int>(height));
  //for(int i = 0; i < width; i++) m_pixels[i] = new int[height];
}

ImFrame::~ImFrame()
{
  //for(int i = 0; i < m_width; i++) delete[] m_pixels[i];
  //delete[] m_pixels;
}

std::vector<ImFrame*> ImFrame::load(const char* fname)
{
  std::ifstream fin(fname,std::ifstream::binary);
  std::vector<ImFrame*> retval;
  char* buf = new char[42000000];
  fin.read(buf,4);
  if(NiaUtils::convertToShort(buf[2],buf[3]) != 42){
    if(NiaUtils::convertToShort(buf[3],buf[2]) != 42){
      std::cout << "ERROR: Can't find right byte order" << std::endl;
      delete[] buf;
      return retval;
    }
    retval = loadBig(buf,fin);
  }
  else retval = loadLittle(buf,fin);
  delete[] buf;
  return retval;
}

std::vector<ImFrame*> ImFrame::loadLittle(char* buf, std::ifstream &fin)
{
  std::vector<uint32_t> offsets;
  uint32_t nOffsets = 0;
  fin.read(buf,4);
  offsets.push_back(NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]));
  while(offsets.at(nOffsets) > 0){
    fin.seekg(offsets.at(nOffsets));
    fin.read(buf,2);
    uint16_t nTags = NiaUtils::convertToShort(buf[0],buf[1]);
    fin.seekg(offsets.at(nOffsets) + 2 + 12*nTags);
    nOffsets++;
    fin.read(buf,4);
    offsets.push_back(NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]));
  }
  std::vector<ImFrame*> retVal;
  fin.seekg(offsets.at(0));
  fin.read(buf,2);
  uint16_t nTags = NiaUtils::convertToShort(buf[0],buf[1]);
  uint32_t width = 1;
  uint32_t height = 1;
  for(uint16_t i = 0; i < nTags; i++){
    fin.read(buf,12);
    uint16_t tag = NiaUtils::convertToShort(buf[0],buf[1]);
    if(tag == 256) width = NiaUtils::convertToInt(buf[8],buf[9],buf[10],buf[11]);
    else if(tag == 257) height = NiaUtils::convertToInt(buf[8],buf[9],buf[10],buf[11]);
  }
  for(uint32_t i = 0; i < nOffsets; i++){
    retVal.push_back(new ImFrame(width,height));
    retVal.at(i)->readLittle(buf,fin,offsets.at(i));
  }
  return retVal;
}

std::vector<ImFrame*> ImFrame::loadBig(char* buf, std::ifstream &fin)
{
  std::vector<uint32_t> offsets;
  uint32_t nOffsets = 0;
  fin.read(buf,4);
  offsets.push_back(NiaUtils::convertToInt(buf[3],buf[2],buf[1],buf[0]));
  while(offsets.at(nOffsets) > 0){
    fin.seekg(offsets.at(nOffsets));
    fin.read(buf,2);
    uint16_t nTags = NiaUtils::convertToShort(buf[1],buf[0]);
    fin.seekg(offsets.at(nOffsets) + 2 + 12*nTags);
    nOffsets++;
    fin.read(buf,4);
    offsets.push_back(NiaUtils::convertToInt(buf[3],buf[2],buf[1],buf[0]));
  }
  std::vector<ImFrame*> retVal;
  fin.seekg(offsets.at(0));
  fin.read(buf,2);
  uint16_t nTags = NiaUtils::convertToShort(buf[1],buf[0]);
  uint32_t width = 1;
  uint32_t height = 1;
  for(uint16_t i = 0; i < nTags; i++){
    fin.read(buf,12);
    uint16_t tag = NiaUtils::convertToShort(buf[1],buf[0]);
    if(tag == 256) width = NiaUtils::convertToInt(buf[11],buf[10],buf[9],buf[8]);
    else if(tag == 257) height = NiaUtils::convertToInt(buf[11],buf[10],buf[9],buf[8]);
  }
  for(int i = 0; i < nOffsets; i++){
    retVal.push_back(new ImFrame(width,height));
    retVal.at(i)->readBig(buf,fin,offsets.at(i));
  }
  return retVal;
}

void ImFrame::readLittle(char* buf, std::ifstream &fin, uint32_t offset)
{
  fin.seekg(offset);
  fin.read(buf,2);
  uint16_t nTags = NiaUtils::convertToShort(buf[0],buf[1]);
  uint32_t nStrips = 0;
  uint32_t stripOffsets = 0;
  uint32_t stripByteCountOffsets = 0;
  for(uint16_t i = 0; i < nTags; i++){
    fin.read(buf,12);
    uint16_t tag = NiaUtils::convertToShort(buf[0],buf[1]);
    uint16_t type = NiaUtils::convertToShort(buf[2],buf[3]);
    if(tag == 273){
      nStrips = NiaUtils::convertToInt(buf[4],buf[5],buf[6],buf[7]);
      if(type == 4) stripOffsets = NiaUtils::convertToInt(buf[8],buf[9],buf[10],buf[11]);
      else if(type == 3) stripOffsets = NiaUtils::convertToShort(buf[8],buf[9]);
      else std::cout << "ERROR: Unknown type for strip offsets\n";
    }
    else if(tag == 279){
      if(type == 4) stripByteCountOffsets = NiaUtils::convertToInt(buf[8],buf[9],buf[10],buf[11]);
      else if(type == 3) stripByteCountOffsets = NiaUtils::convertToShort(buf[8],buf[9]);
      else std::cout << "ERROR: Unknown type for strip byte count offsets\n";
    }
  }
  uint32_t index = 0;
  if(nStrips == 1){
    uint32_t nbytes = stripByteCountOffsets;
    fin.seekg(stripOffsets);
    fin.read(buf,nbytes);
    for(uint32_t pixel = 0; pixel < nbytes; pixel += 2){
      uint32_t y = index / m_width;
      uint32_t x = index - m_width*y;
      m_pixels[x][y] = NiaUtils::convertToShort(buf[pixel],buf[pixel+1]);
      index++;
    }
  }
  else{
    uint32_t nbytes,stripOff,pixel,x,y;
    for(uint32_t i = 0; i < nStrips; i++){
      fin.seekg(stripByteCountOffsets);
      fin.read(buf,4);
      nbytes = NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]);
      fin.seekg(stripOffsets);
      fin.read(buf,4);
      stripOff = NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]);
      fin.seekg(stripOff);
      fin.read(buf,nbytes);
      for(pixel = 0; pixel < nbytes; pixel += 2){
	y = index / m_width;
	x = index - m_width*y;
	m_pixels[x][y] = NiaUtils::convertToShort(buf[pixel],buf[pixel+1]);
	index++;
      }
      stripByteCountOffsets += 4;
      stripOffsets += 4;
    }
  }
}

void ImFrame::readBig(char* buf, std::ifstream &fin, uint32_t offset)
{
  fin.seekg(offset);
  fin.read(buf,2);
  uint16_t nTags = NiaUtils::convertToShort(buf[1],buf[0]);
  uint32_t nStrips = 0;
  uint32_t stripOffsets = 0;
  uint32_t stripByteCountOffsets = 0;
  for(uint16_t i = 0; i < nTags; i++){
    fin.read(buf,12);
    uint16_t tag = NiaUtils::convertToShort(buf[1],buf[0]);
    uint16_t type = NiaUtils::convertToShort(buf[3],buf[2]);
    if(tag == 273){
      nStrips = NiaUtils::convertToInt(buf[7],buf[6],buf[5],buf[4]);
      if(type == 4) stripOffsets = NiaUtils::convertToInt(buf[11],buf[10],buf[9],buf[8]);
      else if(type == 3) stripOffsets = NiaUtils::convertToShort(buf[9],buf[8]);
      else std::cout << "ERROR: Unknown type for strip offsets\n";
    }
    else if(tag == 279){
      if(type == 4) stripByteCountOffsets = NiaUtils::convertToInt(buf[11],buf[10],buf[9],buf[8]);
      else if(type == 3) stripByteCountOffsets = NiaUtils::convertToShort(buf[9],buf[8]);
      else std::cout << "ERROR: Unknown type for strip byte count offsets\n";
    }
  }
  uint32_t index = 0;
  if(nStrips == 1){
    uint32_t nbytes = stripByteCountOffsets;
    fin.seekg(stripOffsets);
    fin.read(buf,nbytes);
    for(uint32_t pixel = 0; pixel < nbytes; pixel += 2){
      uint32_t y = index / m_width;
      uint32_t x = index - m_width*y;
      m_pixels[x][y] = NiaUtils::convertToShort(buf[pixel+1],buf[pixel]);
      index++;
    }
  }
  else{
    for(uint32_t i = 0; i < nStrips; i++){
      fin.seekg(stripByteCountOffsets + 4*i);
      fin.read(buf,4);
      uint32_t nbytes = NiaUtils::convertToInt(buf[3],buf[2],buf[1],buf[0]);
      fin.seekg(stripOffsets + 4*i);
      fin.read(buf,4);
      uint32_t stripOff = NiaUtils::convertToInt(buf[3],buf[2],buf[1],buf[0]);
      fin.seekg(stripOff);
      fin.read(buf,nbytes);
      for(uint32_t pixel = 0; pixel < nbytes; pixel += 2){
	uint32_t y = index / m_width;
	uint32_t x = index - m_width*y;
	m_pixels[x][y] = NiaUtils::convertToShort(buf[pixel+1],buf[pixel]);
	index++;
      }
    }
  }
}

void ImFrame::loadMT(std::vector<ImFrame*>* frames, const char* fname)
{
  std::ifstream fin(fname,std::ifstream::binary);
  char* buf = new char[42000000];
  fin.read(buf,4);
  if(NiaUtils::convertToShort(buf[2],buf[3]) != 42){
    if(NiaUtils::convertToShort(buf[3],buf[2]) != 42){
      std::cout << "ERROR: Can't find right byte order\n";
      delete[] buf;
    }
    loadBigMT(frames,buf,fname);
  }
  else loadLittleMT(frames,buf,fname);
  delete[] buf;
}

void ImFrame::loadLittleMT(std::vector<ImFrame*>* frames, char* buf, const char* fname)
{
  std::ifstream fin(fname,std::ifstream::binary);
  std::vector<uint32_t> offsets;
  uint32_t nOffsets = 0;
  fin.read(buf,4);
  offsets.push_back(NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]));
  while(offsets.at(nOffsets) > 0){
    fin.seekg(offsets.at(nOffsets));
    fin.read(buf,2);
    uint16_t nTags = NiaUtils::convertToShort(buf[0],buf[1]);
    fin.seekg(offsets.at(nOffsets) + 2 + 12*nTags);
    nOffsets++;
    fin.read(buf,4);
    offsets.push_back(NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]));
  }
  fin.seekg(offsets.at(0));
  fin.read(buf,2);
  uint16_t nTags = NiaUtils::convertToShort(buf[0],buf[1]);
  uint32_t width = 1;
  uint32_t height = 1;
  for(uint16_t i = 0; i < nTags; i++){
    fin.read(buf,12);
    uint16_t tag = NiaUtils::convertToShort(buf[0],buf[1]);
    if(tag == 256) width = NiaUtils::convertToInt(buf[8],buf[9],buf[10],buf[11]);
    else if(tag == 257) height = NiaUtils::convertToInt(buf[8],buf[9],buf[10],buf[11]);
  }
  boost::thread_group tgroup;
  for(uint32_t i = 0; i < nOffsets; i++){
    frames->push_back(new ImFrame(width,height));
    tgroup.create_thread(boost::bind(&ImFrame::readLittleMT,frames->at(i),buf,fname,offsets[i]));
  }
  tgroup.join_all();
}

void ImFrame::loadBigMT(std::vector<ImFrame*>* frames, char* buf, const char* fname)
{
  std::ifstream fin(fname,std::ifstream::binary);
  std::vector<uint32_t> offsets;
  uint32_t nOffsets = 0;
  fin.read(buf,4);
  offsets.push_back(NiaUtils::convertToInt(buf[3],buf[2],buf[1],buf[0]));
  while(offsets.at(nOffsets) > 0){
    fin.seekg(offsets.at(nOffsets));
    fin.read(buf,2);
    uint16_t nTags = NiaUtils::convertToShort(buf[1],buf[0]);
    fin.seekg(offsets.at(nOffsets) + 2 + 12*nTags);
    nOffsets++;
    fin.read(buf,4);
    offsets.push_back(NiaUtils::convertToInt(buf[3],buf[2],buf[1],buf[0]));
  }
  std::vector<ImFrame*> retVal;
  fin.seekg(offsets.at(0));
  fin.read(buf,2);
  uint16_t nTags = NiaUtils::convertToShort(buf[1],buf[0]);
  uint32_t width = 1;
  uint32_t height = 1;
  for(uint16_t i = 0; i < nTags; i++){
    fin.read(buf,12);
    uint16_t tag = NiaUtils::convertToShort(buf[1],buf[0]);
    if(tag == 256) width = NiaUtils::convertToInt(buf[11],buf[10],buf[9],buf[8]);
    else if(tag == 257) height = NiaUtils::convertToInt(buf[11],buf[10],buf[9],buf[8]);
  }
  boost::thread_group tgroup;
  for(int i = 0; i < nOffsets; i++){
    frames->push_back(new ImFrame(width,height));
    tgroup.create_thread(boost::bind(&ImFrame::readBigMT,frames->at(i),buf,fname,offsets[i]));
  }
  tgroup.join_all();
}

void ImFrame::readLittleMT(char* buf, const char* fname, uint32_t offset)
{
  std::ifstream fin(fname,std::ifstream::binary);
  readLittle(buf,fin,offset);
  fin.close();
}

void ImFrame::readBigMT(char* buf, const char* fname, uint32_t offset)
{
  std::ifstream fin(fname,std::ifstream::binary);
  readBig(buf,fin,offset);
  fin.close();
}

void ImFrame::divide(int d)
{
  for(int i = 0; i < m_width; i++){
    for(int j = 0; j < m_height; j++){
      m_pixels[i][j] = m_pixels[i][j] / d;
    }
  }
}

int ImFrame::min(int x1, int x2, int y1, int y2)
{
  int min = 99999999;
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      if(m_pixels[i][j] < min) min = m_pixels[i][j];
    }
  }
  return min;
}

double ImFrame::mean(int x1, int x2, int y1, int y2)
{
  double sum = 0.0;
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      sum += m_pixels[i][j];
    }
  }
  return sum / ((x2-x1)*(y2-y1));
}

double ImFrame::mean(int x1, int x2, int y1, int y2, Mask* m)
{
  double sum = 0.0;
  int npix = 0;
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      int a = m->getValue(i,j);
      sum += a * m_pixels[i][j];
      npix += a;
    }
  }
  if(npix > 0) return sum / npix;
  else return 65536.0;
}

double ImFrame::median(int x1, int x2, int y1, int y2)
{
  int* values = new int[65536];
  for(int i = 0; i < 65536; i++) values[i] = 0;
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      values[m_pixels[i][j]]++;
    }
  }
  int sum = 0;
  int target = (x2-x1)*(y2-y1)/2;
  int index;
  for(index = 0; sum < target; index++) sum += values[index];
  delete[] values;
  return (double)index;
}

double ImFrame::median(int x1, int x2, int y1, int y2, Mask* m)
{
  std::vector<int> values;
  values.assign(65536, 0);
  int target = 0;
  for(int i = 0; i < 65536; i++) values[i] = 0;
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      int a = m->getValue(i,j);
      values[m_pixels[i][j]] += a;
      target += a;
    }
  }
  int sum = 0;
  target = target/2;
  int index;
  for(index = 0; sum < target; index++) sum += values[index];
  return (double)index;
}

double ImFrame::median(int x1, int x2, int y1, int y2, int offset)
{
  int* values = new int[65536];
  for(int i = 0; i < 65536; i++) values[i] = 0;
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      values[m_pixels[i][j]-offset]++;
    }
  }
  int sum = 0;
  int target = (x2-x1)*(y2-y1)/2;
  int index;
  for(index = 0; sum < target; index++) sum += values[index];
  delete[] values;
  return (double)(index+offset);
}

double ImFrame::median(int x1, int x2, int y1, int y2, int offset, Mask* m)
{
  int* values = new int[65536];
  int target = 0;
  for(int i = 0; i < 65536; i++) values[i] = 0;
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      int a = m->getValue(i,j);
      values[m_pixels[i][j]-offset] += a;
      target += a;
    }
  }
  int sum = 0;
  target = target/2;
  int index;
  for(index = 0; sum < target; index++) sum += values[index];
  delete[] values;
  return (double)(index+offset);
}

double ImFrame::mode(int x1, int x2, int y1, int y2)
{
  int* values = new int[65536];
  for(int i = 0; i < 65536; i++) values[i] = 0;
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      values[m_pixels[i][j]]++;
    }
  }
  int max = 0;
  int index = 0;
  for(int i = 0; i < 65536; i++){
    if(values[i] > max){
      max = values[i];
      index = i;
    }
  }
  delete[] values;
  return (double)index;
}

double ImFrame::mode(int x1, int x2, int y1, int y2, Mask* m)
{
  int* values = new int[65536];
  int target = 0;
  for(int i = 0; i < 65536; i++) values[i] = 0;
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      int a = m->getValue(i,j);
      values[m_pixels[i][j]] += a;
      target += a;
    }
  }
  int max = 0;
  int index = 0;
  for(int i = 0; i < 65536; i++){
    if(values[i] > max){
      max = values[i];
      index = i;
    }
  }
  delete[] values;
  return (double)index;
}

double ImFrame::mode(int x1, int x2, int y1, int y2, int offset)
{
  int* values = new int[65536];
  for(int i = 0; i < 65536; i++) values[i] = 0;
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      values[m_pixels[i][j]-offset]++;
    }
  }
  int max = 0;
  int index = 0;
  for(int i = 0; i < 65536; i++){
    if(values[i] > max){
      max = values[i];
      index = i;
    }
  }
  delete[] values;
  return (double)(index+offset);
}

double ImFrame::mode(int x1, int x2, int y1, int y2, int offset, Mask* m)
{
  int* values = new int[65536];
  int target = 0;
  for(int i = 0; i < 65536; i++) values[i] = 0;
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      int a = m->getValue(i,j);
      values[m_pixels[i][j]-offset] += a;
      target += a;
    }
  }
  int max = 0;
  int index = 0;
  for(int i = 0; i < 65536; i++){
    if(values[i] > max){
      max = values[i];
      index = i;
    }
  }
  delete[] values;
  return (double)(index+offset);
}

double ImFrame::std(int x1, int x2, int y1, int y2)
{
  double mn = mean(x1,x2,y1,y2);
  double sum = 0.0;
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      sum += pow(m_pixels[i][j] - mn,2);
    }
  }
  return sqrt(sum / ((x2-x1)*(y2-y1) - 1));
}

double ImFrame::std(int x1, int x2, int y1, int y2, Mask* m)
{
  double mn = mean(x1,x2,y1,y2,m);
  double sum = 0.0;
  int npix = 0;
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      int a = m->getValue(i,j);
      sum += a * pow(m_pixels[i][j] - mn,2);
      npix += a;
    }
  }
  if(npix > 1) return sqrt(sum / (npix - 1));
  else return mn;
}

double ImFrame::stdLow(int x1, int x2, int y1, int y2, Mask* m)
{
  double mn = median(x1,x2,y1,y2,m);
  double sum = 0.0;
  int npix = 0;
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      int a = m->getValue(i,j);
      if(m_pixels[i][j] < mn){
	sum += a * pow(m_pixels[i][j] - mn,2);
	npix += a;
      }
    }
  }
  if(npix > 1) return sqrt(sum / (npix - 1));
  else return mn;
}

void ImFrame::getMedianStd(int x1, int x2, int y1, int y2, Mask* m, int nVals, double& med, double& stdv)
{
  //int* values = new int[nVals];
  std::vector<int> values;
  values.assign(nVals, 0);
  int target = 0;
  //for(int i = 0; i < nVals; i++) values[i] = 0;
  double mn = 0.0;
  med = 0.0;
  stdv = 0.0;
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      int a = m->getValue(i,j);
      int val = m_pixels[i][j] * a;
      values[val] += a;
      target += a;
      mn += val;
    }
  }
  mn = mn / target;
  for(int i = 0; i < nVals; i++) stdv += values[i]*(i - mn)*(i - mn);
  if(target > 1) stdv = sqrt(stdv/(target-1));
  else stdv = mn;
  int sum = 0;
  target = target/2;
  int index;
  for(index = 0; sum < target; index++) sum += values[index];
  med = index;
  //delete[] values;
}

ImFrame* ImFrame::derivative()
{
  ImFrame* retVal = new ImFrame(m_width,m_height);
  float dx = m_pixels[1][0] - m_pixels[0][0];
  float dy = m_pixels[0][1] - m_pixels[0][0];
  /*
  retVal->setPixel(0,0,(int)sqrt(dx*dx + dy*dy));
  dx = m_pixels[1][m_height-1] - m_pixels[0][m_height-1];
  dy = m_pixels[0][m_height-1] - m_pixels[0][m_height-2];
  retVal->setPixel(0,m_height-1,(int)sqrt(dx*dx + dy*dy));
  dx = m_pixels[m_width-1][0] - m_pixels[m_width-2][0];
  dy = m_pixels[m_width-1][1] - m_pixels[m_width-1][0];
  retVal->setPixel(m_width-1,0,(int)sqrt(dx*dx + dy*dy));
  dx = m_pixels[m_width-1][m_height-1] - m_pixels[m_width-2][m_height-1];
  dy = m_pixels[m_width-1][m_height-1] - m_pixels[m_width-1][m_height-2];
  retVal->setPixel(m_width-1,m_height-1,(int)sqrt(dx*dx + dy*dy));
  for(int i = 1; i < m_height-1; i++){
    dx = m_pixels[1][i] - m_pixels[0][i];
    dy = (m_pixels[0][i+1] - m_pixels[0][i-1])/2;
    retVal->setPixel(0,i,(int)sqrt(dx*dx + dy*dy));
    dx = m_pixels[m_width-1][i] - m_pixels[m_width-2][i];
    dy = (m_pixels[m_width-1][i+1] - m_pixels[m_width-1][i-1])/2;
    retVal->setPixel(m_width-1,i,(int)sqrt(dx*dx + dy*dy));
  }
  for(int i = 1; i < m_width-1; i++){
    dy = m_pixels[i][1] - m_pixels[i][0];
    dx = (m_pixels[i+1][0] - m_pixels[i-1][0])/2;
    retVal->setPixel(i,0,(int)sqrt(dx*dx + dy*dy));
    dy = m_pixels[i][m_height-1] - m_pixels[i][m_height-2];
    dx = (m_pixels[i+1][m_height-1] - m_pixels[i-1][m_height-1])/2;
    retVal->setPixel(i,m_height-1,(int)sqrt(dx*dx + dy*dy));
  }
  for(int i = 1; i < m_width-1; i++){
    for(int j = 1; j < m_height-1; j++){
      dx = (m_pixels[i+1][j] - m_pixels[i-1][j])/2;
      dy = (m_pixels[i][j+1] - m_pixels[i][j-1])/2;
      retVal->setPixel(i,j,(int)sqrt(dx*dx + dy*dy));
    }
  }
  */
  for(int i = 1; i < m_width-1; i++){
    for(int j = 1; j < m_height-1; j++){
      dx = ((float)(m_pixels[i+1][j] - m_pixels[i-1][j]))/2.0;
      dy = ((float)(m_pixels[i][j+1] - m_pixels[i][j-1]))/2.0;
      retVal->setPixel(i,j,(int)sqrt(dx*dx + dy*dy));
    }
  }
  return retVal;
}

ImFrame* ImFrame::derivativeDir()
{
  ImFrame* retVal = new ImFrame(m_width,m_height);
  float dx = m_pixels[1][0] - m_pixels[0][0];
  float dy = m_pixels[0][1] - m_pixels[0][0];
  for(int i = 1; i < m_width-1; i++){
    for(int j = 1; j < m_height-1; j++){
      dx = ((float)(m_pixels[i+1][j] - m_pixels[i-1][j]))/2.0;
      dy = ((float)(m_pixels[i][j+1] - m_pixels[i][j-1]))/2.0;
      retVal->setPixel(i,j,(int)(1000*atan(dy/dx)));
    }
  }
  return retVal;
}

ImFrame* ImFrame::d2EigenvalueMax()
{
  ImFrame* retVal = new ImFrame(m_width,m_height);
  float fxx,fyy,fxy,sum,diff;
  for(int i = 2; i < m_width-2; i++){
    for(int j = 2; j < m_height-2; j++){
      fxx = ((float)(m_pixels[i+2][j] + m_pixels[i-2][j] - 2*m_pixels[i][j]))/4.0;
      fyy = ((float)(m_pixels[i][j+2] + m_pixels[i][j-2] - 2*m_pixels[i][j]))/4.0;
      fxy = ((float)(m_pixels[i+1][j+1] - m_pixels[i+1][j-1] - m_pixels[i-1][j+1] + m_pixels[i-1][j-1]))/4.0;
      sum = fxx + fyy;
      diff = fxx - fyy;
      if(sum < 0) retVal->setPixel(i,j,(int)((sum - sqrt(diff*diff + 4*fxy*fxy))/2.0));
      else retVal->setPixel(i,j,(int)((sum + sqrt(diff*diff + 4*fxy*fxy))/2.0));
    }
  }
  return retVal;
}

ImFrame* ImFrame::d2EigenvectorMax()
{
  ImFrame* retVal = new ImFrame(m_width,m_height);
  float fxx,fyy,fxy,sum,diff,eigenval;
  for(int i = 2; i < m_width-2; i++){
    for(int j = 2; j < m_height-2; j++){
      fxx = ((float)(m_pixels[i+2][j] + m_pixels[i-2][j] - 2*m_pixels[i][j]))/4.0;
      fyy = ((float)(m_pixels[i][j+2] + m_pixels[i][j-2] - 2*m_pixels[i][j]))/4.0;
      fxy = ((float)(m_pixels[i+1][j+1] - m_pixels[i+1][j-1] - m_pixels[i-1][j+1] + m_pixels[i-1][j-1]))/4.0;
      sum = fxx + fyy;
      diff = fxx - fyy;
      if(sum < 0) eigenval = (sum - sqrt(diff*diff + 4*fxy*fxy)) / 2.0;
      else eigenval = (sum + sqrt(diff*diff + 4*fxy*fxy)) / 2.0;
      retVal->setPixel(i,j,(int)(1000*atan((eigenval - fxx)/fxy)));
    }
  }
  return retVal;
}

double ImFrame::percentile(double frac, int x1, int x2, int y1, int y2)
{
  int* values = new int[65536];
  for(int i = 0; i < 65536; i++) values[i] = 0;
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      values[m_pixels[i][j]]++;
    }
  }
  int sum = 0;
  int target = (x2-x1)*(y2-y1)*frac;
  int index;
  for(index = 0; sum < target; index++) sum += values[index];
  delete[] values;
  return (double)index;
}

double ImFrame::percentile(double frac, int x1, int x2, int y1, int y2, Mask* m)
{
  int* values = new int[65536];
  int target = 0;
  for(int i = 0; i < 65536; i++) values[i] = 0;
  for(int i = x1; i < x2; i++){
    for(int j = y1; j < y2; j++){
      int a = m->getValue(i,j);
      values[m_pixels[i][j]] += a;
      target += a;
    }
  }
  int sum = 0;
  target = target * frac;
  int index;
  for(index = 0; sum < target; index++) sum += values[index];
  delete[] values;
  return (double)index;
}
