/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  src/image_jpg.cpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 */

#include <pix/image_jpg.hpp>
#include <impl/image_jpg_ijg.static.hpp>
#include <stdio.h>


PIXAPI_CPP_BEG

class _Jpg {
public:
  impl::Jpg impl;
public:
  _Jpg(int type) : impl(type) { }

};

JpgReader::JpgReader()
{
  m_jpg = new _Jpg(1);

}

JpgReader::~JpgReader()
{
  close();
  if (m_jpg) delete m_jpg;
}

bool JpgReader::open(Stream &stm)
{
  if (!m_jpg->impl.open(stm)) {
    pix_log_error("open failed", PIX_ERR_JPG);
    return false;

  }
  param(m_jpg->impl.param());

  return true;
}
bool JpgReader::close()
{
  m_jpg->impl.close();
  ImageReader::close();
  return true;
}
bool JpgReader::ready()
{
  if (line() >= 0) return true;
  if (size() <= 0) return false;
  if (!m_jpg->impl.ready()) return false;
  lock(true);
  line(0);
  return true;
}


pix_pos JpgReader::read(char *buf, pix_pos rows)
{
  if (!ready()) return 0;
  pix_pos ll = this->line();
  if ( 0 == rows ) rows = this->rows() - ll;
  pix_pos l = m_jpg->impl.read(buf, rows);
  this->line(ll + l);
  return l;
}

JpgWriter::JpgWriter()
{
  m_jpg = new _Jpg(2);
}

JpgWriter::~JpgWriter()
{
  close();
  if (m_jpg) delete m_jpg;
}

bool JpgWriter::open(Stream &stm)
{
  if (!m_jpg->impl.open(stm)) {
    printf("open failed\n");
    return false;

  }
  return true;
}
bool JpgWriter::close()
{
  m_jpg->impl.close();
  return ImageWriter::close();
}

bool JpgWriter::ready()
{
  if (line() >= 0) return true;
  if (size() <= 0) return false;
  m_jpg->impl.param(param());
  if (!m_jpg->impl.ready()) return false;
  param(m_jpg->impl.param());
  lock(true);
  line(0);
  return true;
}


pix_pos JpgWriter::write(const char *buf, pix_pos rows)
{
  if (!ready()) return 0;
  pix_pos ll = this->line();
  if ( 0 == rows ) rows = this->rows() - ll;
  pix_pos l = m_jpg->impl.write(buf, rows);
  this->line(ll + l);
  return l;
}
void JpgWriter::quality(int q)
{
  if (lock()) return;
  if (q < 0) q = 0;
  else if (q > 100) q = 100;
  m_jpg->impl.quality(q);
}
PIXAPI_CPP_END
