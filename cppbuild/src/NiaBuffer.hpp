#ifndef NIABUFFER_HPP
#define NIABUFFER_HPP

#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

class NiaBuffer
{

protected:
  std::string m_buffer;
  boost::mutex m_mtx;

public:
  NiaBuffer(){}
  virtual ~NiaBuffer(){}
  std::string buffer(){ return m_buffer; }
  void clear(){ m_buffer = ""; }
  void append(std::string s){ m_buffer.append(s); }
  void lock(){ m_mtx.lock(); }
  void unlock(){ m_mtx.unlock(); }
  
};

NiaBuffer& operator<<(NiaBuffer& buf, std::string s);
NiaBuffer& operator<<(NiaBuffer& buf, const char* s);
NiaBuffer& operator<<(NiaBuffer& buf, int i);
NiaBuffer& operator<<(NiaBuffer& buf, double d);
NiaBuffer& operator<<(NiaBuffer& buf, uint32_t i);
NiaBuffer& operator<<(NiaBuffer& buf, uint16_t i);
NiaBuffer& operator<<(NiaBuffer& buf, uint8_t i);

namespace nia
{
  extern NiaBuffer nout;
  extern int niaVersion;
}

#endif
