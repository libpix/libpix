/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  src/image.cpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 */

#include <pix/image.hpp>
#include <stdio.h>
#include <stdlib.h>
PIXAPI_CPP_BEG

void ImageBase::sync()
{
  char *p = m_pm.bits;
  m_bpp = 0;
  m_byte = -1;
  while (*p) {
    if ((*p & 0x7) != 0) m_byte = 0;
    else if (m_byte != 0) {
      int b = *p >> 3;
      if (m_byte < 0) m_byte = b;
      else if ( b != m_byte) m_byte = 0;
    }
    m_bpp += *p;
    p++;
  }
  if (m_byte > 0) m_bpp >>= 3;
  if (sizeof(pix_len) < 8) {
    pix_long rsize = (pix_long) m_pm.cols * m_bpp;
    pix_long size = (pix_long) rsize * m_pm.rows;
    if ((size >> 31) > 0) {
      m_rsize = 0;
      m_size = 0;
      pix_log_error("image size overflow: 32bit limit 2G bytes. Use 64bit.", PIX_ERR_OVERFLOW);
    } else {
      m_rsize = (pix_len)rsize;
      m_size = (pix_len)size;
    }
  } else {
    m_rsize = m_pm.cols * m_bpp;
    m_size = m_rsize * m_pm.rows;
  }
}

pix_pos ImageReaderBase::seek(pix_pos pos, pix_seek whence)
{
  if (!ready()) return -1;
  switch(whence) {
    case PIX_SEEK_SET :
      break;
    case PIX_SEEK_CUR :
      pos += tell();
    case PIX_SEEK_END :
      pos += rows();

  }
  if (tell() == pos) return pos;
  Block<char> buf(stride());
  while (tell() < pos) {
    if (!read(buf, 1)) {
      pix_log_error("image seek: can't seek scanline.", PIX_ERR_SEEK);
      break;
    }
  }
  return tell();
}

ImageReader::~ImageReader()
{
  if (m_stm) delete m_stm;
}
bool ImageReader::open_file(const String &path)
{
  close();
  FileStream *fstm = new FileStream;
  if (!fstm->open(path, PIX_STM_READ)) {
    delete fstm;
    return false;
  }
  if (!open(*fstm)) {
    delete fstm;
    return false;

  }
  m_stm = fstm;
  return true;
}
bool ImageReader::close()
{
  if (m_stm) delete m_stm;
  m_stm = 0;
  ImageBase::close();
  return true;
}
ImageWriter::~ImageWriter()
{
  if (m_stm) delete m_stm;
}
bool ImageWriter::open_file(const String &path)
{
  close();
  FileStream *fstm = new FileStream;
  if (!fstm->open(path, PIX_STM_WCT)) {
    delete fstm;
    return false;
  }
  if (!open(*fstm)) {
    delete fstm;
    return false;
  }
  m_stm = fstm;
  return true;
}

bool ImageWriterBase::write_image(ImageReaderBase &img)
{
  if (!img.ready()) return false;
  param(img.param());
  if (!ready()) return false;
  Block<char> buf(img.stride());
  pix_pos l;
  pix_pos p = 0;
  for (l = 0;l < rows(); l++) {
    if (!img.read(buf, 1)) return false;
    write(buf, 1);
    if (pix_log() >= 3) {
      pix_pos pp = (l + 1) * 100 / rows();
      if (pp > p) {
        p = pp;
        String str;
        str-"write_image: "-p-'%';
        pix_log_msg(str);
      }
    }
  }
  return true;
}

bool ImageWriter::close()
{
  if (m_stm) delete m_stm;
  m_stm = 0;
  ImageBase::close();
  return true;
}

PIXAPI_CPP_END


