#include "NiaBuffer.hpp"

namespace nia
{
  NiaBuffer nout;
}

NiaBuffer& operator<<(NiaBuffer& buf, std::string s)
{
  buf.lock();
  buf.append(s);
  buf.unlock();
  return buf;
}

NiaBuffer& operator<<(NiaBuffer& buf, const char* s)
{
  buf.lock();
  buf.append(s);
  buf.unlock();
  return buf;
}

NiaBuffer& operator<<(NiaBuffer& buf, int i)
{
  buf.lock();
  buf.append(boost::lexical_cast<std::string>(i));
  buf.unlock();
  return buf;
}

NiaBuffer& operator<<(NiaBuffer& buf, double d)
{
  buf.lock();
  buf.append(boost::lexical_cast<std::string>(d));
  buf.unlock();
  return buf;
}

NiaBuffer& operator<<(NiaBuffer& buf, uint32_t i){ return buf << (int)i; }
NiaBuffer& operator<<(NiaBuffer& buf, uint16_t i){ return buf << (int)i; }
NiaBuffer& operator<<(NiaBuffer& buf, uint8_t i){ return buf << (int)i; }
