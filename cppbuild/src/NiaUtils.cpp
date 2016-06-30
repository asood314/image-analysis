#include "NiaUtils.hpp"

uint16_t NiaUtils::convertToShort(char c0, char c1)
{
  uint16_t b0 = (uint16_t)c0;
  uint16_t b1 = (uint16_t)c1;
  return ((b1 & 0x00ff) << 8) | (b0 & 0x00ff);
}

uint32_t NiaUtils::convertToInt(char c0, char c1, char c2, char c3)
{
  uint32_t b0 = (uint32_t)c0;
  uint32_t b1 = (uint32_t)c1;
  uint32_t b2 = (uint32_t)c2;
  uint32_t b3 = (uint32_t)c3;
  return ((b3 & 0x000000ff) << 24) | ((b2 & 0x000000ff) << 16) | ((b1 & 0x000000ff) << 8) | (b0 & 0x000000ff);
}

double NiaUtils::convertToDouble(char c0, char c1, char c2, char c3)
{
  uint32_t b0 = (uint32_t)c0;
  uint32_t b1 = (uint32_t)c1;
  uint32_t b2 = (uint32_t)c2;
  uint32_t b3 = (uint32_t)c3;
  uint32_t t1000 = ((b3 & 0x000000ff) << 24) | ((b2 & 0x000000ff) << 16) | ((b1 & 0x000000ff) << 8) | (b0 & 0x000000ff);
  return double(t1000) / 1000.0;
}

void NiaUtils::writeShortToBuffer(char* buf, uint64_t offset, uint16_t data)
{
  buf[offset] = (char)(data & 0x00ff);
  buf[offset+1] = (char)((data & 0xff00) >> 8);
}

void NiaUtils::writeIntToBuffer(char* buf, uint64_t offset, uint32_t data)
{
  buf[offset] = (char)(data & 0x000000ff);
  buf[offset+1] = (char)((data & 0x0000ff00) >> 8);
  buf[offset+2] = (char)((data & 0x00ff0000) >> 16);
  buf[offset+3] = (char)((data & 0xff000000) >> 24);
}

void NiaUtils::writeDoubleToBuffer(char* buf, uint64_t offset, double data)
{
  uint32_t t1000 = (uint32_t)(data*1000);
  buf[offset] = (char)(t1000 & 0x000000ff);
  buf[offset+1] = (char)((t1000 & 0x0000ff00) >> 8);
  buf[offset+2] = (char)((t1000 & 0x00ff0000) >> 16);
  buf[offset+3] = (char)((t1000 & 0xff000000) >> 24);
}
