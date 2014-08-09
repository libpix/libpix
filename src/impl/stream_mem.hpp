/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  stream_mem.hpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 */
#ifndef __IMPL_STREAM_MEM__
#define __IMPL_STREAM_MEM__
#include <impl/config.h>
#include <pix/utils.hpp>
#include <impl/string.hpp>

PIXAPI_CPP_BEG

PIXIMPL_BEG


class MemStream {
public:
  Buffer m_buf;
  int m_flag;
  pix_pos m_off;
  pix_len m_len;
  pix_len m_size;
  const char * m_rbuf;
  char *m_wbuf;
public:

  Buffer & get_buffer()
  {
    return m_buf;
  }
public:
  MemStream() : m_flag(0), m_off(0), m_size(0), m_len(0), m_rbuf(0), m_wbuf(0) {

  }
  ~MemStream()
  {
    close();
  }
  bool open(int flag = PIX_STM_RWA)
  {
    if (m_flag) return false;
    m_flag = flag;
    m_off = 0;
    return true;

  }
  bool open(const char *buf, int len)
  {
    if (!m_flag) return false;
    if (!buf) return false;
    if (len <= 0) return false;
    m_flag = PIX_STM_READ;
    m_rbuf = buf;
    m_len = len;
    m_size = len;
    return false;
  }
  bool open(char * buf, int len)
  {
    if (m_flag) return false;
    if (!buf) return false;
    if (len <= 0) return false;
    m_flag = PIX_STM_RW;
    m_wbuf = buf;
    m_len = len;
    m_size = len;
    return false;
  }
  void swap(Buffer &buf)
  {
    m_buf.r_reset();
    m_buf.swap(buf);
    m_off = 0;
  }
  pix_len size() const
  {
    return m_size;
  }
public: /* Virtual interface */
  bool close()
  {
    m_flag = 0;
    m_buf.release();
    m_rbuf = 0;
    m_wbuf = 0;
    m_size = 0;
    m_len = 0;
    m_off = 0;
    return true;
  }
  bool eof() const
  {
    if (!m_flag) return true;
    if (m_off >= length()) return true;
    return false;
  }

  pix_pos seek(pix_pos pos , int whence = PIX_SEEK_SET)
  {
    switch (whence) {
    case PIX_SEEK_SET:
      break;
    case PIX_SEEK_CUR:
      pos += m_off;
      break;
    case PIX_SEEK_END:
      pos += length();
      break;
    }
    if (pos < 0) return pos = 0;
    m_off = pos;
    return m_off;

    return 0;
  }
  pix_pos tell() const
  {
    return m_off;
  }
  pix_len reserve(pix_len len)
  {
    if (!(m_flag & PIX_STM_WRITE)) return size();
    if (len <= 0) return 0;
    if (m_rbuf) return 0;
    if ( m_wbuf) return m_size - m_off;
    pix_pos off;
    if (m_flag & PIX_STM_APPEND) off = m_len;
    else off = m_off;
    if (off + len > m_size) {
      reserve_back(m_buf, off + len - m_len);
      str_end(m_buf);
      m_size = m_buf.r_avail() + m_buf.w_avail() - 4;
    }
    if (m_flag & PIX_STM_APPEND) return m_size - m_len;
    return m_size - m_off;
  }
  pix_len trunc(pix_len len)
  {
    if (!(m_flag & PIX_STM_WRITE)) return m_len;
    if (len < 0) return m_len;
    if (len == m_len) return len;
    if (m_wbuf) {
      if (len <= m_size) {
        if (len > m_len) 
          memset(m_wbuf + m_len, 0, len - m_len);
        m_len = len;
      }
      return m_len;
    }
    if (len < m_len) {
      m_buf.wp(m_buf.rp() + len);
      str_end(m_buf);
      m_len = len;
      return len;
    }
    pix_len dlen = len;
    dlen -= m_len;
    reserve_back(m_buf, dlen);
    memset(m_buf.wp(), 0, dlen);
    m_buf.wp(m_buf.rp() + len);
    str_end(m_buf);
    m_size = m_buf.r_avail() + m_buf.w_avail() - 4;
    m_len = len;
    return len;
  }

  pix_len length() const
  {
    if (!m_flag) return 0;
    return m_len;
  }

  const char * rp(pix_len &len , pix_len minlen)
  {
    pix_pos off = len;
    len = 0;
    if (!(m_flag & PIX_STM_READ)) return 0;

    off += m_off;

    if (off < 0) return 0;

    if (m_rbuf) {
      m_off = off;
      if (off >= m_len) return 0;
      len = m_len - m_off;
      return m_rbuf + off;
    }
    if (m_wbuf) {
      m_off = off;
      if (off >= m_len) return 0;
      len = m_len - m_off;
      return m_wbuf + off;
    }
    m_off = off;
    if (m_off >= m_buf.r_avail()) return 0;
    len = m_buf.r_avail() - off;
    return m_buf.rp() + m_off;
  }

  char * wp(pix_len &len, pix_len minlen)
  {
    pix_pos off = len;
    len = 0;
    if (!(m_flag & PIX_STM_WRITE)) return 0;

    if (m_flag & PIX_STM_APPEND) off += m_len;
    else off += m_off;

    if (off < 0) return 0;

    if (!(m_flag & PIX_STM_APPEND)) m_off = off;


    if (m_wbuf) {
      if (m_off >= m_size) return 0;
      len = m_size - m_off;
      if (m_off > m_len)  m_len = m_off;
      return m_wbuf + m_off;
    }

    if (off <= m_size) {
      if (off > m_len) {
        m_buf.wp(m_buf.rp() + off);
        str_end(m_buf);
        m_len = off;
      }
      if (minlen == 0) {
        len = m_size - off;
        return m_buf.rp() + off;
      }
    } else {
      m_buf.wp(m_buf.rp() + m_size);
      if (minlen == 0) return 0;
    }

    if (off + minlen > m_size) {
      pix_len l = m_len;
      trunc(off + minlen);
      m_len = l;
      m_buf.wp(m_buf.rp() + m_len);
      str_end(m_buf);
    }

    if (off > m_len || m_len == 0) {
      m_buf.wp(m_buf.rp() + off);
      str_end(m_buf);
      m_len = off;
    }


    len = m_size - off;

    return m_buf.rp() + off;
  }

};


PIXIMPL_END

PIXAPI_CPP_END

#endif

