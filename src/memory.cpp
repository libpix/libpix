/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  memory.cpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 */

/**
 * @file
 * @brief impl. of buffer
 */

#include <pix/memory.hpp>
#include <pix/utils.hpp>
#include <string.h>

PIXAPI_CPP_BEG

void * Base::operator new(size_t size)
{
  return ::operator new(size);
}
void Base::operator delete(void *p)
{
  return ::operator delete(p);
}
void * Base::operator new[] (size_t size)
{
  return ::operator new(size);
}
void Base::operator delete[] (void *p)
{
  return ::operator delete(p);
}

Buffer::Buffer(pix_len len) :
  m_bp(0), m_rp(0), m_wp(0), m_ep(0), m_buf(0)
{
  if (len > 0) {
    m_buf = new char[len];
    m_bp = m_buf;
    m_rp = m_buf;
    m_wp = m_buf;
    m_ep = m_buf + len;
  }
}

Buffer::~Buffer()
{
  release();
}

void Buffer::resize(pix_len tsize)
{
  if (tsize <= 0) return;
  if (m_buf) delete []m_buf;
  m_buf = new char[tsize];
  m_bp = m_buf;
  m_rp = m_buf;
  m_wp = m_buf;
  m_ep = m_buf + tsize;
}

void Buffer::expand(pix_len  wsize)
{
  pix_len wlen = w_avail();
  /* need not expand */
  if (wsize <= wlen) return;
  /* need expand */
  pix_pos wpos = w_pos();
  pix_pos rpos = r_pos();
  pix_len tsize = wpos + wsize;
  char *buf = new char [tsize];
  if (m_buf) {
    memcpy(buf, m_buf , wpos);
    delete []m_buf;
  }
  m_buf = buf;
  m_bp = m_buf;
  m_rp = m_buf + rpos;
  m_wp = m_buf + wpos; 
  m_ep = m_buf + tsize;
  return ;
}

void Buffer::move(pix_pos pos , pix_len wsize)
{
  if (pos < 0) return;
  if (wsize < 0) wsize = 0;
  pix_len rlen = r_avail();
  pix_pos ss = pos + rlen + wsize;
  pix_len s = size();
  if (ss <= s) {
    /* need not expand */
    if (r_pos() == pos) return;
    memmove(m_bp + pos , m_rp , rlen);
    m_rp = m_bp + pos;
    m_wp = m_rp + rlen;
    return;
  }
  /* need expand */
  char * buf = new char [ss];
  if (m_buf) {
    memcpy(buf + pos , m_rp, rlen);
    delete []m_buf;
  }
  m_buf = buf;
  m_bp = m_buf;
  m_rp = m_buf + pos;
  m_wp = m_rp + rlen;
  m_ep = m_buf + ss;
}

void Buffer::release()
{
  if (m_buf) {
    delete []m_buf;
    m_buf = 0;
    m_bp = 0;
    m_rp = 0;
    m_wp = 0;
    m_ep = 0;
  }
}

void Buffer::swap(Buffer &buf)
{
  Pix::swap(m_buf, buf.m_buf);
  Pix::swap(m_bp, buf.m_bp);
  Pix::swap(m_rp, buf.m_rp);
  Pix::swap(m_wp, buf.m_wp);
  Pix::swap(m_ep, buf.m_ep);
}

PIXAPI_CPP_END

