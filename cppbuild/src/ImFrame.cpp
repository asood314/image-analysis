#include "ImFrame.hpp"
#include <cmath>

ImFrame::ImFrame()
{
  m_width = 2048;
  m_height = 2048;
  m_pixels = new uint16_t*[2048];
  for(int i = 0; i < 2048; i++) m_pixels[i] = new uint16_t[2048];
}

ImFrame::ImFrame(const int width, const int height)
{
  m_width = width;
  m_height = height;
  m_pixels = new uint16_t*[width];
  for(int i = 0; i < width; i++) m_pixels[i] = new uint16_t[height];
}

ImFrame::~ImFrame()
{
  for(int i = 0; i < m_width; i++) delete[] m_pixels[i];
  delete[] m_pixels;
}

std::vector<ImFrame*> ImFrame::load(const char* fname)
{
  std::ifstream fin(fname,std::ifstream::binary);
  std::vector<ImFrame*> retval;
  char* buf = new char[42000000];
  fin.read(buf,4);
  if(NiaUtils::convertToShort(buf[2],buf[3]) != 42){
    if(NiaUtils::convertToShort(buf[3],buf[2]) != 42){
      std::cout << "ERROR: Can't find right byte order\n";
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

void ImFrame::divide(int d)
{
  for(uint16_t i = 0; i < m_width; i++){
    for(uint16_t j = 0; j < m_height; j++){
      m_pixels[i][j] = m_pixels[i][j] / d;
    }
  }
}

double ImFrame::mean(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2)
{
  double sum = 0.0;
  for(uint16_t i = x1; i < x2; i++){
    for(uint16_t j = y1; j < y2; j++){
      sum += m_pixels[i][j];
    }
  }
  return sum / ((x2-x1)*(y2-y1));
}

double ImFrame::mean(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, Mask* m)
{
  double sum = 0.0;
  int npix = 0;
  for(uint16_t i = x1; i < x2; i++){
    for(uint16_t j = y1; j < y2; j++){
      int a = m->getValue(i,j);
      sum += a * m_pixels[i][j];
      npix += a;
    }
  }
  if(npix > 0) return sum / npix;
  else return 65536.0;
}

double ImFrame::median(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2)
{
  int* values = new int[65536];
  for(int i = 0; i < 65536; i++) values[i] = 0;
  for(uint16_t i = x1; i < x2; i++){
    for(uint16_t j = y1; j < y2; j++){
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

double ImFrame::median(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, Mask* m)
{
  int* values = new int[65536];
  int target = 0;
  for(int i = 0; i < 65536; i++) values[i] = 0;
  for(uint16_t i = x1; i < x2; i++){
    for(uint16_t j = y1; j < y2; j++){
      int a = m->getValue(i,j);
      values[m_pixels[i][j]] += a;
      target += a;
    }
  }
  int sum = 0;
  target = target/2;
  int index;
  for(index = 0; sum < target; index++) sum += values[index];
  delete[] values;
  return (double)index;
}

double ImFrame::mode(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2)
{
  int* values = new int[65536];
  for(int i = 0; i < 65536; i++) values[i] = 0;
  for(uint16_t i = x1; i < x2; i++){
    for(uint16_t j = y1; j < y2; j++){
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

double ImFrame::mode(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, Mask* m)
{
  int* values = new int[65536];
  int target = 0;
  for(int i = 0; i < 65536; i++) values[i] = 0;
  for(uint16_t i = x1; i < x2; i++){
    for(uint16_t j = y1; j < y2; j++){
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

double ImFrame::std(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2)
{
  double mn = mean(x1,x2,y1,y2);
  double sum = 0.0;
  for(uint16_t i = x1; i < x2; i++){
    for(uint16_t j = y1; j < y2; j++){
      sum += pow(m_pixels[i][j] - mn,2);
    }
  }
  return sqrt(sum / ((x2-x1)*(y2-y1) - 1));
}

double ImFrame::std(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, Mask* m)
{
  double mn = mean(x1,x2,y1,y2,m);
  double sum = 0.0;
  int npix = 0;
  for(uint16_t i = x1; i < x2; i++){
    for(uint16_t j = y1; j < y2; j++){
      int a = m->getValue(i,j);
      sum += a * pow(m_pixels[i][j] - mn,2);
      npix += a;
    }
  }
  if(npix > 1) return sqrt(sum / (npix - 1));
  else return mn;
}

void ImFrame::getMedianStd(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, Mask* m, double& med, double& stdv)
{
  int* values = new int[4096];
  int target = 0;
  for(int i = 0; i < 4096; i++) values[i] = 0;
  double mn = 0.0;
  med = 0.0;
  stdv = 0.0;
  for(uint16_t i = x1; i < x2; i++){
    for(uint16_t j = y1; j < y2; j++){
      int a = m->getValue(i,j);
      int val = m_pixels[i][j] * a;
      values[val] += a;
      target += a;
      mn += val;
    }
  }
  mn = mn / target;
  for(int i = 0; i < 4096; i++) stdv += values[i]*(i - mn)*(i - mn);
  if(target > 1) stdv = sqrt(stdv/(target-1));
  else stdv = mn;
  int sum = 0;
  target = target/2;
  int index;
  for(index = 0; sum < target; index++) sum += values[index];
  med = index;
  delete[] values;
}
