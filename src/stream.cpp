/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  stream.cpp
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
#include "inner.hpp"
#include <stdio.h>
#include <string.h>

PIXAPI_CPP_BEG
  
pix_len Stream::read(char *buf, pix_len len)
{
  if (!buf) return 0;
  if (!is_read()) return 0;
  if (eof()) return 0;
  pix_len ll = 0;
  pix_len count = 0;
  pix_len g = pix_sys_fbuf();
  pix_len wlen = 0;
  if (len < g) g = len;
  const char * p = rp(ll, g);
  while (len > 0 && ll > 0 && p) {
    wlen = len;
    if (ll < wlen) wlen = ll;
    memcpy(buf, p, wlen);
    len -= wlen;
    count += wlen;
    buf += wlen;
    ll = wlen;
    if (len < g) g = len;
    p = rp(ll, g);
  }
  return count;
}

pix_len Stream::write(const char *buf, pix_len len)
{
  if (!buf) return 0;
  if (!is_write()) return 0;
  pix_len ll = 0;
  pix_len count = 0;
  pix_len g = 1;
  pix_len wlen = 0;
  if (len < g) g = pix_sys_fbuf();
  char * p = wp(ll, g);
  while (len > 0 && ll > 0 && p) {
    wlen = len;
    if (ll < wlen) wlen = ll;
    memcpy(p, buf, wlen);
    len -= wlen;
    count += wlen;
    buf += wlen;
    ll = wlen;
    if (len < g) g = len;
    p = wp(ll, g);
  }
  return count;
}

pix_len Stream::write(Stream &stm, pix_len len)
{
  if (len < 0) return 0;
  if (!stm.is_read()) return 0;
  if (!is_write()) return 0;
  const char *src;
  char *dst;
  if (len == 0) {
    len = stm.length() - stm.tell();
  }
  reserve(len);
  pix_len g = pix_sys_fbuf();
  pix_len slen = 0;
  pix_len dlen = 0;
  pix_len wlen = 0;
  pix_len count = 0;
  if (len < g) g = len;
  src = stm.rp(slen, g);
  dst = wp(dlen, g);
  while (len>0 && slen> 0 && dlen > 0 && src && dst) {
    wlen = len;
    if (slen < wlen) wlen = slen;
    if (dlen < wlen) wlen = dlen;
    memcpy(dst, src, wlen);
    len -= wlen;
    count += wlen;
    if (pix_log() >= 3) {
      String msg;
      double pg = (double) count / (count + len);
      pg *= 100;
      int p = (int) pg;
      msg-"write stm: "-p-"% "-wlen-"/"-count;
      pix_log_msg(msg);
    }
    slen = wlen;
    dlen = wlen;
    if (len < g) g = len;
    src = stm.rp(slen, g);
    dst = wp(dlen, g);
  }
  return count;
}

PIXAPI_CPP_END



