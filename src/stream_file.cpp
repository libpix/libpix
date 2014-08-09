/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  stream_file.cpp
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
#include <inner.hpp>
#include <impl/stream_file.hpp>



PIXAPI_CPP_BEG

class _FileStream {
public:
  impl::FileStream impl;
};

FileStream::FileStream()
{
  m_stm = new _FileStream;

}
FileStream::~FileStream()
{
  if (m_stm) delete m_stm;
}

bool FileStream::open(const String &path, int flag)
{
  if (m_stm->impl.open(path, flag)) {
    m_name.clear();
    String str;
    m_name-"file"-'#'-path-'@'-format(str, (void*)this);
    m_flag = flag;
    return true;

  }
  m_name.clear();
  m_flag = 0;
  return false;
}
bool FileStream::close()
{
  if (!m_stm->impl.close()) return false;
  m_name.clear();
  m_flag = 0;
  return true;
}
bool FileStream::eof() const
{
  return m_stm->impl.eof();

}

pix_pos FileStream::seek(pix_pos pos, int whence)
{
  return m_stm->impl.seek(pos, whence);

}
pix_pos FileStream::tell() const
{
  return m_stm->impl.tell();
}
pix_len FileStream::length() const
{
  return m_stm->impl.length();
}
pix_len FileStream::trunc(pix_len len)
{
  return m_stm->impl.trunc(len);
}
pix_len FileStream::reserve(pix_len len)
{
  return m_stm->impl.reserve(len);

}

bool FileStream::flush()
{
  return m_stm->impl.flush();
}

const char *FileStream::rp(pix_len &len, pix_len minlen)
{
  return m_stm->impl.rp(len, minlen);

}
char *FileStream::wp(pix_len &len, pix_len minlen)
{
  return m_stm->impl.wp(len, minlen);

}

pix_len FileStream::read(char *buf , pix_len len)
{
  return Stream::read(buf, len);
}
pix_len FileStream::write(const char *buf , pix_len len)
{
  return Stream::write(buf, len);
}
pix_len FileStream::write(Stream &stm, pix_len len)
{
  return Stream::write(stm, len);
}

char * _Inner::get_buffer(const FileStream &fstm, 
    int &buf_size, int &buf_len, pix_pos &buf_off, pix_pos &buf_pos)
{
  return fstm.m_stm->impl.get_buffer(buf_size, buf_len, buf_off, buf_pos);
}

PIXAPI_CPP_END


