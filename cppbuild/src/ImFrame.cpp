#include "ImFrame.hpp"

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
  std::ifstream fin(fname);
  std::vector<ImFrame*> retval;
  char* buf = new char[42000000];
  fin.read(buf,4);
  if(convertToShort(buf[2],buf[3]) != 42){
    if(convertToShort(buf[3],buf[2]) != 42){
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
  offsets.push_back(convertToInt(buf[0],buf[1],buf[2],buf[3]));
  while(offsets.at(nOffsets) > 0){
    fin.seekg(offsets.at(nOffsets));
    fin.read(buf,2);
    uint16_t nTags = convertToShort(buf[0],buf[1]);
    fin.seekg(offsets.at(nOffsets) + 2 + 12*nTags);
    nOffsets++;
    fin.read(buf,4);
    offsets.push_back(convertToInt(buf[0],buf[1],buf[2],buf[3]));
  }
  std::vector<ImFrame*> retVal;
  fin.seekg(offsets.at(0));
  fin.read(buf,2);
  uint16_t nTags = convertToShort(buf[0],buf[1]);
  uint32_t width = 1;
  uint32_t height = 1;
  for(uint16_t i = 0; i < nTags; i++){
    fin.read(buf,12);
    uint16_t tag = convertToShort(buf[0],buf[1]);
    if(tag == 256) width = convertToInt(buf[8],buf[9],buf[10],buf[11]);
    else if(tag == 257) height = convertToInt(buf[8],buf[9],buf[10],buf[11]);
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
  offsets.push_back(convertToInt(buf[3],buf[2],buf[1],buf[0]));
  while(offsets.at(nOffsets) > 0){
    fin.seekg(offsets.at(nOffsets));
    fin.read(buf,2);
    uint16_t nTags = convertToShort(buf[1],buf[0]);
    fin.seekg(offsets.at(nOffsets) + 2 + 12*nTags);
    nOffsets++;
    fin.read(buf,4);
    offsets.push_back(convertToInt(buf[3],buf[2],buf[1],buf[0]));
  }
  std::vector<ImFrame*> retVal;
  fin.seekg(offsets.at(0));
  fin.read(buf,2);
  uint16_t nTags = convertToShort(buf[1],buf[0]);
  uint32_t width = 1;
  uint32_t height = 1;
  for(uint16_t i = 0; i < nTags; i++){
    fin.read(buf,12);
    uint16_t tag = convertToShort(buf[1],buf[0]);
    if(tag == 256) width = convertToInt(buf[11],buf[10],buf[9],buf[8]);
    else if(tag == 257) height = convertToInt(buf[11],buf[10],buf[9],buf[8]);
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
  uint16_t nTags = convertToShort(buf[0],buf[1]);
  uint32_t nStrips = 0;
  uint32_t stripOffsets = 0;
  uint32_t stripByteCountOffsets = 0;
  for(uint16_t i = 0; i < nTags; i++){
    fin.read(buf,12);
    uint16_t tag = convertToShort(buf[0],buf[1]);
    uint16_t type = convertToShort(buf[2],buf[3]);
    if(tag == 273){
      nStrips = convertToInt(buf[4],buf[5],buf[6],buf[7]);
      if(type == 4) stripOffsets = convertToInt(buf[8],buf[9],buf[10],buf[11]);
      else if(type == 3) stripOffsets = convertToShort(buf[8],buf[9]);
      else std::cout << "ERROR: Unknown type for strip offsets\n";
    }
    else if(tag == 279){
      if(type == 4) stripByteCountOffsets = convertToInt(buf[8],buf[9],buf[10],buf[11]);
      else if(type == 3) stripByteCountOffsets = convertToShort(buf[8],buf[9]);
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
      m_pixels[x][y] = convertToShort(buf[pixel],buf[pixel+1]);
      index++;
    }
  }
  else{
    for(uint32_t i = 0; i < nStrips; i++){
      fin.seekg(stripByteCountOffsets + 4*i);
      fin.read(buf,4);
      uint32_t nbytes = convertToInt(buf[0],buf[1],buf[2],buf[3]);
      fin.seekg(stripOffsets + 4*i);
      fin.read(buf,4);
      uint32_t stripOff = convertToInt(buf[0],buf[1],buf[2],buf[3]);
      fin.seekg(stripOff);
      fin.read(buf,nbytes);
      for(uint32_t pixel = 0; pixel < nbytes; pixel += 2){
	uint32_t y = index / m_width;
	uint32_t x = index - m_width*y;
	m_pixels[x][y] = convertToShort(buf[pixel],buf[pixel+1]);
	index++;
      }
    }
  }
}

void ImFrame::readBig(char* buf, std::ifstream &fin, uint32_t offset)
{
  fin.seekg(offset);
  fin.read(buf,2);
  uint16_t nTags = convertToShort(buf[1],buf[0]);
  uint32_t nStrips = 0;
  uint32_t stripOffsets = 0;
  uint32_t stripByteCountOffsets = 0;
  for(uint16_t i = 0; i < nTags; i++){
    fin.read(buf,12);
    uint16_t tag = convertToShort(buf[1],buf[0]);
    uint16_t type = convertToShort(buf[3],buf[2]);
    if(tag == 273){
      nStrips = convertToInt(buf[7],buf[6],buf[5],buf[4]);
      if(type == 4) stripOffsets = convertToInt(buf[11],buf[10],buf[9],buf[8]);
      else if(type == 3) stripOffsets = convertToShort(buf[9],buf[8]);
      else std::cout << "ERROR: Unknown type for strip offsets\n";
    }
    else if(tag == 279){
      if(type == 4) stripByteCountOffsets = convertToInt(buf[11],buf[10],buf[9],buf[8]);
      else if(type == 3) stripByteCountOffsets = convertToShort(buf[9],buf[8]);
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
      m_pixels[x][y] = convertToShort(buf[pixel+1],buf[pixel]);
      index++;
    }
  }
  else{
    for(uint32_t i = 0; i < nStrips; i++){
      fin.seekg(stripByteCountOffsets + 4*i);
      fin.read(buf,4);
      uint32_t nbytes = convertToInt(buf[3],buf[2],buf[1],buf[0]);
      fin.seekg(stripOffsets + 4*i);
      fin.read(buf,4);
      uint32_t stripOff = convertToInt(buf[3],buf[2],buf[1],buf[0]);
      fin.seekg(stripOff);
      fin.read(buf,nbytes);
      for(uint32_t pixel = 0; pixel < nbytes; pixel += 2){
	uint32_t y = index / m_width;
	uint32_t x = index - m_width*y;
	m_pixels[x][y] = convertToShort(buf[pixel+1],buf[pixel]);
	index++;
      }
    }
  }
}

uint16_t ImFrame::convertToShort(char c0, char c1)
{
  uint16_t b0 = (uint16_t)c0;
  uint16_t b1 = (uint16_t)c1;
  return ((b1 & 0x00ff) << 8) | (b0 & 0x00ff);
}

uint32_t ImFrame::convertToInt(char c0, char c1, char c2, char c3)
{
  uint32_t b0 = (uint32_t)c0;
  uint32_t b1 = (uint32_t)c1;
  uint32_t b2 = (uint32_t)c2;
  uint32_t b3 = (uint32_t)c3;
  return ((b3 & 0x000000ff) << 24) | ((b2 & 0x000000ff) << 16) | ((b1 & 0x000000ff) << 8) | (b0 & 0x000000ff);
}
