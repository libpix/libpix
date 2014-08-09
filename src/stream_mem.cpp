/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  stream_mem.cpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 */

#include <pix/stream.hpp>
#include "impl/stream_mem.hpp"
#include "inner.hpp"

PIXAPI_CPP_BEG
class _MemStream {
public:
  impl::MemStream impl;

};

MemStream::MemStream()
{
  m_stm = new _MemStream;
}
MemStream::~MemStream()
{
  if (m_stm) delete m_stm;
}
bool MemStream::open(int flag)
{
  if (m_stm->impl.open(flag)) {
    m_flag = flag;
    String str;
    m_name-"mem@"-format(str, (void*)this); 
    return true;
  }
  m_flag = 0;
  return false;
}
bool MemStream::open(const char *buf, int len)
{
  return m_stm->impl.open(buf, len);
}
bool MemStream::open(char *buf, int len)
{
  if (!m_stm->impl.open(buf, len)) {
    

  }
  return false;
}
void MemStream::swap(Buffer &buf)
{
  return m_stm->impl.swap(buf);

}
pix_len MemStream::size() const
{
  return m_stm->impl.size();
}

bool MemStream::close()
{
  if (m_stm->impl.close()) {
    m_flag = 0;
    return true;
  }
  return false;
}

bool MemStream::eof() const
{
  return m_stm->impl.eof();
}

pix_pos MemStream::seek(pix_pos pos, int whence)
{
  return m_stm->impl.seek(pos, whence);
}

pix_pos MemStream::tell() const
{
  return m_stm->impl.tell();
}
pix_len MemStream::length() const
{
  return m_stm->impl.length();
}
pix_len MemStream::trunc(pix_len len)
{
  return m_stm->impl.trunc(len);
}
pix_len MemStream::reserve(pix_len len)
{
  return m_stm->impl.reserve(len);
}

const char * MemStream::rp(pix_len &len, pix_len minlen)
{
  return m_stm->impl.rp(len, minlen);
}
char * MemStream::wp(pix_len &len, pix_len minlen)
{
  return m_stm->impl.wp(len, minlen);
}
pix_len MemStream::read(char *buf , pix_len len)
{
  return Stream::read(buf, len);

}

pix_len MemStream::write(const char *buf , pix_len len)
{
  return Stream::write(buf, len);
}

pix_len MemStream::write(Stream &stm, pix_len len)
{
  return Stream::write(stm, len);
}

Buffer & _Inner::get_buffer(const MemStream &mstm)
{
  return mstm.m_stm->impl.get_buffer();
}





PIXAPI_CPP_END



