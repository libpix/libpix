/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  src/impl/string.hpp
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
 * @brief
 */

#ifndef __IMPL_STRING_H__
#define __IMPL_STRING_H__

#include <impl/config.h>

#include <pix/memory.hpp>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <wchar.h>
#include <iconv.h>
#include <errno.h>


PIXAPI_CPP_BEG
char * str_null();

PIXIMPL_BEG
/*------------------------- iconv ---------------------  */
inline const char * iconv_str(int enc)
{
  typedef struct {
    int enc;
    const char * str;
  } map_t;
  const map_t map[] = {
    /* Frequent use */
    { PIX_ENC_GBK,      "GBK" },
    { PIX_ENC_GB2312,   "GB2312" },
    { PIX_ENC_UTF8,     "UTF-8" },
    { PIX_ENC_UTF16LE,  "UTF-16LE" },
    { PIX_ENC_UTF32LE,  "UTF-32LE" },
    /* -------------------------- */
    { PIX_ENC_LATIN1,   "LATIN1" },
    { PIX_ENC_LATIN2,   "LATIN2" },
    { PIX_ENC_LATIN3,   "LATIN3" },
    { PIX_ENC_LATIN4,   "LATIN4" },
    { PIX_ENC_CYRILLIC, "CYRILLIC" },
    { PIX_ENC_ARABIC,   "ARABIC" },
    { PIX_ENC_GREEK,    "GREEK" },
    { PIX_ENC_HEBREW,   "HEBREW" },
    { PIX_ENC_LATIN5,   "LATIN5" },
    { PIX_ENC_LATIN6,   "LATIN6" },
    { PIX_ENC_THAI,     "ISO8859-11" },
    { PIX_ENC_LATIN7,   "LATIN7" },
    { PIX_ENC_LATIN8,   "LATIN8" },
    { PIX_ENC_LATIN9,   "ISO8859-15" },
    { PIX_ENC_LATIN10,  "LATIN10" },
    { PIX_ENC_GB2312,   "GB2312" },
    { PIX_ENC_GBK,      "GBK" },
    { PIX_ENC_GB18030,  "GB18030" },
    { PIX_ENC_BIG5,     "BIG5" },
    { PIX_ENC_JIS,      "JIS_X0208" },
    { PIX_ENC_SJIS,     "SJIS" },
    { PIX_ENC_KOREAN,   "KOREAN" },
    { PIX_ENC_VIETNAM,  "CP1258" },
    { PIX_ENC_VISCII,   "VISCII" },
    { PIX_ENC_EUCCN,    "EUC-CN" },
    { PIX_ENC_EUCJP,    "EUC-JP" },
    { PIX_ENC_EUCKR,    "EUC-KR" },
    { PIX_ENC_EUCTW,    "EUC_TW" },
    /* Unicode */
    { PIX_ENC_UTF8,     "UTF-8" },
    { PIX_ENC_UTF7,     "UTF-7" },
    { PIX_ENC_UTF16LE,  "UTF-16LE" },
    { PIX_ENC_UTF16BE,  "UTF-16BE" },
    { PIX_ENC_UTF32LE,  "UTF-32LE" },
    { PIX_ENC_UTF32BE,  "UTF-32BE" },
  };
  const int len = sizeof(map) / sizeof(map_t);
  int i;
  for (i = 0; i < len; i++) {
    if (map[i].enc == enc) return map[i].str;
  }
  return 0;
}
inline pix_len str_estimate(pix_enc dst , pix_enc src, pix_len srclen)
{
  if (src == dst) return srclen;
  return srclen * 2;

}
inline pix_len str_encode( char *dst, pix_len &dstlen, pix_enc dstenc,
    const char *src, pix_len &srclen, pix_enc srcenc )
{
  const char *from = iconv_str(srcenc);
  const char *to = iconv_str(dstenc);
  iconv_t cd = iconv_open(to, from);
  if (!cd) return false;

#ifdef PIX_WIN
  const char * in = src;
#endif
  /* glibc iconv don't have const qualifier, 
   * we use explicit cast, but we don't modify the src content */
#ifdef PIX_POSIX
  char * in = const_cast<char*>(src); 
#endif

  char *out = dst;
  size_t inlen = srclen;
  size_t outlen = dstlen;
  int ret = iconv(cd , &in, &inlen, &out, &outlen);
  if (ret == -1) {
    if (errno == E2BIG) {
      if (pix_log() > 3)
        pix_log_msg("iconv : output buffer not enough, realloc");
    } else if (errno == EILSEQ) {
      if (pix_log() > 0) {
        String s = "iconv : some chars can't be converted from ";
        s.append(from);
        s.append(" to ");
        s.append(to);
        pix_log_error(s, PIX_ERR_ICONV);
        iconv_close(cd);
        return -1;
      }
    } else if (errno == EINVAL) {
      pix_log_warn("iconv : input buffer incomplete");
    }
  }
  iconv_close(cd);
  pix_len olen = dstlen - outlen;
  srclen = inlen;
  dstlen = outlen;
  return olen;
}

/* ---------------------- Inline functions ----------------------  */
inline pix_len str_len(const Buffer &buf)
{
  return buf.r_avail();
}

inline pix_len str_len(const char *str)
{
  return strlen(str);
}
inline pix_len str_len(const pix_wchar *str)
{
  return wcslen(str);
}

inline void reserve_back(Buffer &buf, pix_len len)
{
  if (buf.size() <=0) {
    buf.resize(len + 4);
    return;
  }
  if (len > buf.w_avail() - 4)                     /* write buffer not enough */
    if (len <= buf.avail() - 4) buf.move(0);       /* just rewind, need not alloc */
    else buf.move(0, buf.size() + len + 4);        /* need alloc */
}

inline void reserve_front(Buffer &buf, pix_len len)
{
  if (buf.size() <= 0) {
    buf.resize(len + 4);
    return;
  }
  if (len > buf.p_avail())                      /* pre buffer not enough  */
    if (len <= buf.avail() - 4) buf.move(len);       /* just move the buffer, need not alloc */
    else buf.move(len , buf.size() + len + 4);  /* need alloc */
}

inline void str_copy(char *dst, char ch, pix_len len)
{
  pix_len i;
  for (i = 0; i < len ; i++)
    *dst++ = ch;
}
inline void str_copy(char *dst, pix_wchar ch, pix_len len)
{
  pix_len i;
  pix_wchar *buf = (pix_wchar*)dst;
  for (i = 0; i < len; i++)
    *buf++ = ch;
}

inline void str_copy(char *dst, const char *src, pix_len len)
{
  memcpy(dst, src, len);
}

inline void str_end(Buffer &buf, pix_len len=0)
{
  buf.wp(len);
  buf.wp()[0] = 0;
  buf.wp()[1] = 0;
  buf.wp()[2] = 0;
  buf.wp()[3] = 0;
}

inline void str_move(Buffer &buf, pix_pos pos, pix_len len)
{
  memmove(buf.rp() + pos + len, buf.rp() + pos, buf.r_avail() - pos);
  str_end(buf, len);
}

inline bool str_check(const char *pat, pix_len len, const char *str)
{
  pix_len i;
  for (i = 0; i < len; i++)
    if (str[i] != pat[i]) return false;
  return true;
}

inline bool str_check(char ch, pix_len len , const char *str)
{
  pix_len i;
  for (i = 0; i < len; i++)
    if (str[i] != ch) return false;
  return true;

}

inline const char * str_find(const char *pat, pix_len len, const char *beg, const char *end)
{
  end = end - len + 1;
  const char *p = beg;
  while (p <= end) {
    if (str_check(pat, len, p)) return p;
    p++;
  }
  return 0;
}

inline const char * str_find(char ch, pix_len len, const char *beg, const char *end)
{
  end = end - len + 1;
  const char *p = beg;
  while (p <= end) {
    if (str_check(ch, len, p)) return p;
    p++;
  }
  return 0;
}

inline const char * str_rfind(const char *pat, pix_len len, const char *beg, const char *end)
{
  const char *p = end - len + 1;
  while (p >= beg) {
    if (str_check(pat, len, p)) return p;
    p--;
  }
  return 0;
}

inline const char * str_rfind(char ch, pix_len len, const char *beg, const char *end)
{
  const char *p = end - len + 1;
  while (p >= beg) {
    if (str_check(ch, len, p)) return p;
    p--;
  }
  return 0;
}

inline void str_insert(Buffer &buf, pix_pos pos, const char *str, pix_len len)
{
  pix_pos ll = buf.r_avail();

  if ( 2 * pos <= ll && buf.p_avail() >= len ) {
    /* move front */
    memmove(buf.rp() - len, buf.rp(), len);
    buf.rp(-len);

  } else {
    /* move back */
    reserve_back(buf, len);

    str_move(buf, pos, len);
  }

  char *dst = buf.rp() + pos;
  str_copy(dst, str, len);

  str_end(buf);

}
inline void str_replace(Buffer &buf, pix_pos beg, pix_pos end, const char *str, pix_len ll)
{
  char *rp = buf.rp();
  pix_len len = buf.r_avail();
  pix_len rmlen = end - beg + 1;
  pix_len plen = beg;
  pix_len wlen = len - end - 1;
  pix_pos off = ll - rmlen;

  if (plen < wlen && buf.p_avail() >= off) {
    /*  move the front part off chars forward, put str after  */
    char *newrp = rp - off;
    if (plen > 0) memmove(newrp, rp, plen);
    buf.rp(newrp);
    str_copy(newrp + plen, str, ll);

  } else {
    /* move the back part off chars backward, put str before */
    if (off > 0) reserve_back(buf, off);
    char * src = buf.wp() - wlen;
    char * dst = src + off;
    memmove(dst, src, wlen);
    str_copy(dst - ll, str, ll);
    str_end(buf, off);
  }

}

inline char esc_dot(char ch)
{
  int c = (unsigned char)ch;
  if (c < 32 || c >= 127) return '.';
  return ch;
}

inline bool str_bufenc(Buffer &buf, pix_enc dstenc, const char *src, pix_len slen , pix_enc srcenc)
{
  pix_len dlen = str_estimate(dstenc, srcenc, slen);
  pix_len olen = dlen;
  char *dst;
  const char *sp = src + slen;
  while (slen)  {
    if (olen == 0) olen = slen * 4;
    reserve_back(buf, olen);
    dlen = buf.w_avail() - 4;
    dst = buf.wp();
    olen = str_encode( 
        dst, dlen, dstenc,
        src, slen, srcenc);
    if (olen<0) return false;
    str_end(buf, olen);
    src = sp - slen;
  }
  return true;
}

/* ------------------------------------------------------------ */
class String {
public:
  Buffer m_buf;
  pix_enc m_enc;
  bool m_keep;
public:
  Buffer & get_buffer()
  {
    return m_buf;
  }
public:
  String(const char *buf = 0, pix_len len = 0, 
      pix_enc enc = PIX_ENC_AUTO) : m_keep(false)
  {
    if (enc == PIX_ENC_AUTO) enc = pix_sys_enc();
    m_enc = enc;
    if (buf) {
      if (len == 0) {
        len = str_len(buf);
      }
      m_buf.resize(len + 4);
      memcpy(m_buf.wp() , buf , len);
      str_end(m_buf, len);
    }
  }
  String(const pix_wchar *buf, pix_len len = 0): m_keep(false)
  {
    if (buf) {
      if (len == 0) {
        len = str_len(buf);
      }
      len *= sizeof(pix_wchar);
      m_buf.resize(len + 4);
      memcpy(m_buf.wp(), (const char *)buf, len);
      str_end(m_buf, len);
      if (sizeof(pix_wchar) == 4) m_enc = pix_enc_ucs4();
      else m_enc = pix_enc_ucs2();
    }
  }

  String(const String &str)
  {
    const Buffer &sbuf = str.m_buf;
    m_enc = str.m_enc;
    m_keep = str.m_keep;
    pix_len len = sbuf.r_avail();
    if (len > 0) {
      m_buf.resize(len + 4);
      memcpy(m_buf.wp(), sbuf.rp() , len);
      str_end(m_buf, len);
    }
  }

  void swap(Buffer & buf)
  {
    m_buf.swap(buf);
  }
  int width() const
  {
    return pix_enc_width(m_enc);
  }

  char& at(pix_pos pos)
  {
    if (!m_buf.rp()) return str_null()[0];
    pix_len len = m_buf.r_avail();
    if (pos < 0) pos += len;
    if (pos < 0 || pos >= len) return str_null()[0];
    return *(m_buf.rp()+pos);
  }

  bool equal(const String &str) const
  {
    if (m_enc != str.m_enc) return false;
    if (m_buf.r_avail() != str.m_buf.r_avail()) return false;
    pix_len len = m_buf.r_avail();
    pix_len i;
    const char *p1 = m_buf.rp();
    const char *p2 = str.m_buf.rp();

    for (i = 0; i < len; i++)
      if (p1[i] != p2[i]) return false;
    return true;
  }
  const char * c_str() const
  {
    if (!m_buf.rp()) return str_null();
    return m_buf.rp();
  }
  char * str()
  {
    if (!m_buf.rp()) return str_null();

    return m_buf.rp();
  }

  pix_len length() const
  {
    return m_buf.r_avail();
  }

  pix_len size() const
  {
    return m_buf.size();
  }

  bool empty() const
  {
    return (length() == 0);
  }

  String & clear()
  {
    if (m_buf.size()<=0) return *this;
    m_buf.reset();
    *m_buf.wp() = 0;
    return *this;
  }


  String & reserve(pix_len len)
  {
    if (len == 0) return *this;
    if (len < 0) reserve_front(m_buf, -len);
    reserve_back(m_buf, len);
    str_end(m_buf);
    return *this;
  }
  String & trunc(pix_len len)
  {
    if (len < 0) return *this;
    if (len < length()) {
      m_buf.w_reset();
      str_end(m_buf, len);
      return *this;
    }
    pix_len alen = len - length();
    reserve_back(m_buf, alen);
    memset(m_buf.wp(), 0, alen);
    str_end(m_buf, alen);
    return *this;
  }

  pix_enc encode() const
  {
    return m_enc;
  }
  pix_enc encode(pix_enc enc)
  {
    m_enc = enc;
    return m_enc;
  }
  String & assign(char ch, pix_len count = 1)
  {
    m_buf.reset();
    m_enc = pix_sys_enc();
    return append(ch , count);
  }
  String & assign(pix_wchar ch, pix_len count = 1)
  {
    m_buf.reset();
    m_enc = pix_enc_wchar();
    return append(ch , count);
  }
  String & assign(const char *str, pix_len len, pix_enc enc = PIX_ENC_AUTO)
  {
    if (enc == PIX_ENC_AUTO) enc = pix_sys_enc();
    m_buf.reset();
    return append(str, len, enc);
  }
  String & assign(const pix_wchar *str, pix_len len)
  {
    if (sizeof(pix_wchar) == 2) m_enc = pix_enc_ucs2();
    else m_enc = pix_enc_ucs4();
    m_buf.reset();
    return append(str, len);
  }

  String & assign(const String &str)
  {
    m_buf.reset();
    return append(str);
  }

  String & append(char ch, pix_len len = 1)
  {
    if (len <= 0) return *this;
    pix_enc enc = pix_sys_enc();
    if (enc == m_enc) {
      reserve_back(m_buf, len);

      char *dst = m_buf.wp();
      str_copy(dst, ch, len);

      str_end(m_buf, len);
    } else {
      Block<char> buf(len);
      memset(buf, ch, len);
      str_bufenc(m_buf, m_enc, buf, len, enc);
    }

    return *this;

  }

  String & append(pix_wchar ch, pix_len len = 1)
  {
    // ASCII compatible: UTF-8
    if (len <= 0) return *this;
    pix_enc enc = pix_enc_wchar();
    if (enc == m_enc) {
      reserve_back(m_buf, len);

      char *dst = m_buf.wp();
      str_copy(dst, ch, len);

      str_end(m_buf, len * sizeof(pix_wchar) );
    } else {
      Block<pix_wchar> buf(len);
      pix_len i;
      for (i = 0; i < len; i++)
        buf[i] = ch;
      pix_wchar *p = buf;
      str_bufenc(m_buf, m_enc, (char*)p, sizeof(pix_wchar) * len, enc);
    }

    return *this;

  }

  String & append(const char *str, pix_len size =0, 
      pix_enc enc = PIX_ENC_AUTO)
  {
    if (!str) return *this;
    if (enc == PIX_ENC_AUTO) enc = pix_sys_enc();
    if (size == 0) {
      if (enc == pix_enc_wchar()) {
        size = str_len((pix_wchar*) str) * sizeof(pix_wchar);
      } else size = str_len(str);
    }
    if ( m_enc == enc ) {
      pix_len len;
      if (size > 0) len = size;
      else len = str_len(str);

      reserve_back(m_buf, len);

      char *dst = m_buf.wp();
      str_copy(dst, str, len);

      str_end(m_buf, len);

      return *this;
    } else {
      str_bufenc(m_buf, m_enc, str, size, enc);
    }
    return *this;
  }

  String & append(const pix_wchar *str, pix_len len)
  {
    if (!str) return *this;
    pix_enc enc = pix_enc_wchar();
    if (len == 0) len = str_len(str);
    return append((const char *)str, len * sizeof(pix_wchar), enc);
  }


  String & append(const String &str)
  {
    if (str.empty()) return *this;
    const Buffer &sbuf = str.m_buf;
    if (m_enc == str.m_enc) {
      pix_len len = str_len(sbuf);

      reserve_back(m_buf, len);

      char *dst = m_buf.wp();
      str_copy(dst, sbuf.rp(), len);

      str_end(m_buf, len);

      return *this;
    } else {
      str_bufenc(m_buf, m_enc, str.m_buf.rp(), str.length(), str.m_enc);
    }
    return *this;
  }

  String & insert(pix_pos pos, char ch, pix_len ll = 1)
  {
    // ASCII compatible: UTF-8, multi-byte chars

    if (ll <= 0) return *this;
    pix_len len = m_buf.r_avail();
    if (pos < 0) pos += len;
    if (pos == len) return append(ch, ll);
    if (pos < 0 || pos > len) return *this;

    Block<char> buf(ll);
    char *p = buf;
    memset(p, ch, ll);


    str_insert(m_buf, pos, p, ll);

    return *this;
  }
  String & insert(pix_pos pos, pix_wchar ch, pix_len ll = 1)
  {
    String str;
    str.assign(ch, ll);
    return insert(pos, str);
  }

  String & insert(pix_pos pos, const char *str, pix_len ll = 0, 
      pix_enc enc = PIX_ENC_AUTO)
  {
    if (!str) return *this;
    if (enc == PIX_ENC_AUTO) enc = pix_sys_enc();
    if ( m_enc == enc ) {
      pix_len len = m_buf.r_avail();
      if (pos < 0) pos += len;
      if (pos == len) return append(str, ll, enc);
      if (pos < 0 || pos > len) return *this;

      if (ll <= 0) ll = str_len(str);

      if ( (str >= m_buf.rp() && str < m_buf.wp()) ||
           ( m_buf.rp() >= str && m_buf.rp() <= str + ll)) {
        /* buffer claps, copy first */
        Block<char> buf(ll);
        char *p = buf;

        str_insert(m_buf, pos, p, ll);

      } else {

        str_insert(m_buf, pos, str, ll);

      }

      return *this;
    } else {
      String s(str, ll, enc);
      insert(pos, s);
    }
    return *this;
  }
  String & insert(pix_pos pos, const pix_wchar *str, pix_len ll = 0)
  {
    if (!str) return *this;
    if (m_enc == pix_enc_wchar()) 
      return insert(pos, (const char *)str, ll * sizeof(pix_wchar), m_enc);
    String s(str, ll);
    return insert(pos, s);
  }

  String & insert(pix_pos pos, const String &str)
  {
    if (str.empty()) return *this;
    const Buffer &sbuf = str.m_buf;
    if ( m_enc == str.m_enc ) {
      pix_len len = m_buf.r_avail();
      pix_len ll = str_len(sbuf);
      if (pos < 0) pos += len;
      if (pos == len) return append(str);
      if (pos < 0 || pos > len) return *this;

      if (&m_buf == &sbuf) {
        /* insert self, copy first */
        Block<char> buf(len);
        char *p = buf;
        memcpy(p, sbuf.rp(), ll);

        str_insert(m_buf, pos, p , ll);

      } else {

        str_insert(m_buf, pos, sbuf.rp(), ll);

      }

      return *this;
    } else {
      String s;
      str.chars(s, m_enc);
      insert(pos, s);
    }
    return *this;
  }

  String & replace(pix_pos beg, pix_pos end, char ch, pix_len ll = 1)
  {
    // ASCII compatible: UTF-8, multi-byte chars

    pix_len len = m_buf.r_avail();
    if (beg < 0) beg += len;
    if (end < 0) end += len;
    if (beg < 0 || beg >= len) { return *this;}
    if (end < 0 || end >= len) { return *this;}
    if (beg > end) Pix::swap(beg, end);

    Block<char> buf(ll);
    char *p = buf;
    memset(p, ch, ll);

    str_replace(m_buf, beg, end, p, ll);

    return *this;

  }

  String & replace(pix_pos beg, pix_pos end, 
      const char *str, pix_len ll = 0, pix_enc enc = PIX_ENC_AUTO)
  {
    if (!str) return *this;
    pix_len len = m_buf.r_avail();
    if (ll == 0) ll = strlen(str);

    if (beg < 0) beg += len;
    if (end < 0) end += len;
    if (beg < 0 || beg >= len) { return *this;}
    if (end < 0 || end >= len) { return *this;}
    if (beg > end) Pix::swap(beg, end);
    if (enc == PIX_ENC_AUTO) enc = pix_sys_enc();


    if (m_enc == enc) {

      if ( (str >= m_buf.rp() && str < m_buf.wp()) ||
           ( m_buf.rp() >= str && m_buf.rp() <= str + ll)) {
        /* buffer claps, copy first */
        Block<char> buf(ll);
        char *p = buf;
        memcpy(p, str, ll);

        str_replace(m_buf, beg, end, p, ll);

      } else {

        str_replace(m_buf, beg, end, str, ll);

      }
      return *this;

    }

    return *this;

  }

  String & replace(pix_pos beg, pix_pos end, const String &str)
  {
    if (str.empty()) return *this;
    const Buffer &sbuf = str.m_buf;
    pix_len len = m_buf.r_avail();
    pix_len ll = sbuf.r_avail();
    if (beg < 0) beg += len;
    if (end < 0) end += len;
    if (beg < 0 || beg >= len) { return *this;}
    if (end < 0 || end >= len) { return *this;}
    if (beg > end) Pix::swap(beg, end);

    if (&sbuf == &m_buf) {
      Block<char> buf(ll);
      char *p = buf;
      memcpy(p, sbuf.rp(), ll);

      str_replace(m_buf, beg, end, p, ll);

    } else {

      str_replace(m_buf, beg, end, sbuf.rp(), ll);

    }

    return *this;
  }

  String & trim()
  {
    // ASCII compatible : UTF-8, multi-byte chars
    if (empty()) return *this;
    char *rp = m_buf.rp();
    char *wp = m_buf.wp();
    while ( rp <= wp && isspace(*rp) ) rp++;
    wp--;
    while ( rp <= wp && isspace(*wp) ) wp--; 
    wp++;
    m_buf.rp(rp);
    m_buf.wp(wp);
    return *this;
  }

  String & sub(pix_pos beg, pix_pos end = -1)
  {
    // ASCII compatible : UTF-8, multi-byte chars
    if (empty()) return *this;
    pix_len len = m_buf.r_avail();
    if (beg < 0) beg += len;
    if (end < 0) end += len;
    if (beg < 0 || beg >= len) { clear();return *this;}
    if (end < 0 || end >= len) { clear();return *this;}
    if (beg <= end) {
      m_buf.wp(m_buf.rp() + end + 1);
      m_buf.rp(beg);
    } else {
      m_buf.wp(m_buf.rp() + beg + 1);
      m_buf.rp(end);
      char *bp = m_buf.rp();
      char *ep = m_buf.wp() - 1;
      while (bp < ep) {
        char t = *ep;
        *ep = *bp;
        *bp = t;
        ep--;
        bp++;
      }
    }

    str_end(m_buf);

    return *this;
  }

  String & remove(pix_pos beg , pix_pos end = -1)
  {
    if (empty()) return *this;
    pix_len len = m_buf.r_avail();
    if (beg < 0) beg += len;
    if (end < 0) end += len;
    if (beg < 0 || beg >= len) { return *this;}
    if (end < 0 || end >= len) { return *this;}
    if (beg > end) Pix::swap(beg, end);

    char *rp = m_buf.rp();
    pix_len rmlen = end - beg + 1;
    pix_len plen = beg;
    pix_len wlen = len - end - 1;

    if (plen < wlen) {
      /* move front part */
      char *newrp = rp + rmlen;
      if (plen > 0) memmove(newrp, rp, plen); 
      m_buf.rp(newrp);
    } else {
      /* move back part */
      memmove(rp + beg, rp + end + 1, wlen);
      str_end(m_buf, - rmlen);
    }
    return *this;
  }

  String & repeat(int count = 2)
  {
    if (count <= 1) return *this;
    if (empty()) return *this;
    pix_len l = m_buf.r_avail();
    pix_len len = l * (count - 1);

    reserve_back(m_buf, len);

    pix_len i;
    char *dst = m_buf.wp();
    for (i = 1; i < count; i++) {
      str_copy(dst, m_buf.rp(), l);
      dst += l;
    }
    str_end(m_buf, len);

    return *this;
  }

  pix_pos find(char ch, pix_len count = 1, pix_pos beg = 0, pix_pos end = -1 ) const
  {

    // ASCII compatible : UTF-8, multi-byte chars
    if (empty()) return -1;
    if (count <= 0) return -1;
    pix_len len = m_buf.r_avail();
    if (beg < 0) beg += len;
    if (end < 0) end += len;
    if (beg < 0 || beg >= len) return -1;
    if (end < 0 || end >= len) return -1;

    pix_len ll = count;
    const char *bp = m_buf.rp();
    const char *ep = m_buf.wp();
    const char *fp;
    if (beg <= end) fp=str_find(ch, ll, bp + beg, bp + end);
    else fp = str_rfind(ch, ll, bp + end, bp + beg);

    if (fp) return static_cast<pix_pos>(fp -bp);
    return -1;
  }

  pix_pos find(const char *str, pix_len size = 0, pix_pos beg = 0, pix_pos end = -1) const
  {
    if (empty()) return -1;
    if (!str) return -1;
    pix_len len = m_buf.r_avail();
    if (beg < 0) beg += len;
    if (end < 0) end += len;
    if (beg < 0 || beg >= len) return -1;
    if (end < 0 || end >= len) return -1;

    pix_len ll;
    if (size > 0) ll = size;
    else ll = str_len(str);

    const char *bp = m_buf.rp();
    const char *ep = m_buf.wp();
    const char *pp = str;
    const char *fp;
    if (beg <= end) fp=str_find(pp, ll, bp + beg, bp + end);
    else fp = str_rfind(pp, ll, bp + end, bp + beg);

    if (fp) return static_cast<pix_pos>(fp - bp);
    return -1;

  }

  pix_pos find(const String &str, pix_pos beg = 0, pix_pos end = -1) const
  {
    if (empty()) return -1;
    if (str.empty()) return -1;
    if ( m_enc == str.m_enc  ) {
      pix_len len = m_buf.r_avail();
      if (beg < 0) beg += len;
      if (end < 0) end += len;
      if (beg < 0 || beg >= len) return -1;
      if (end < 0 || end >= len) return -1;

      pix_len ll = str.m_buf.r_avail();

      const char *bp = m_buf.rp();
      const char *ep = m_buf.wp();
      const char *pp = str.m_buf.rp();
      const char *fp;
      if (beg <= end) fp=str_find(pp, ll, bp + beg, bp + end);
      else fp = str_rfind(pp, ll, bp + end, bp + beg);

      if (fp) return static_cast<pix_pos>(fp - bp);
      return -1;
    }
    return -1;
  }

  bool keep(bool val) {
    bool ret = m_keep;
    m_keep = val;
    return ret;
  }
  bool keep() const
  {
    return m_keep;
  }
  String & get_sub(String &sub, pix_pos beg, pix_pos end = -1) const
  {

    if (!sub.keep()) sub.clear();
    if (empty()) return sub;

    if (m_enc == sub.m_enc) {
      pix_len len = m_buf.r_avail();
      if (beg < 0) beg += len;
      if (end < 0) end += len;
      if (beg < 0 || beg >= len ) return sub;
      if (end < 0 || end >= len ) return sub;
      if (beg <= end) {
        len = end - beg + 1;

        reserve_back(sub.m_buf, len);

        char *dst = sub.m_buf.wp();
        str_copy(dst, m_buf.rp() + beg, len);

      } else {
        len = beg - end + 1;

        reserve_back(sub.m_buf, len);

        char *dst = sub.m_buf.wp();
        char *src = m_buf.rp() + end;
        pix_len i;
        for (i = 0; i < len; i++)
          dst[i] = src[len - i - 1];
        
      }
      str_end(sub.m_buf, len);

      return sub;
    }
    return sub;

  }


  pix_pos get_line(String &line, pix_pos pos = 0, 
      const char * eol = PIX_EOL) const
  {
    if (!line.keep()) line.clear();
    if (empty()) return -1;
    if (!eol) eol = PIX_EOL;
    pix_len len = m_buf.r_avail();
    if (pos < 0) pos += len;
    if (pos < 0 || pos >= len ) return -1;
    char delim = eol[0];
    const char *p = eol+1;
    const char *q = p;
    pix_pos f = -1;
    pix_pos r = 0;
    pix_pos l;
    pix_pos fs = -1;
    pix_pos rs = 0;
    pix_pos ls = 0;
    while (1) {
      if (*p == delim || *p == 0) {
        r++;
        l = p - q;
        if (l > 0) {
          f = find(q, l, pos);
          if (f >= 0) {
            if (fs < 0 || f < fs) {
              fs = f;
              rs = r;
              ls = l;
            }
          }
        }
        q = p + 1;
      }
      if (*p == 0) break;
      p++;
    }

    if (fs >= 0) {
      if (fs > pos) get_sub(line, pos, fs - 1);
      return fs + ls;
    } else {
      get_sub(line, pos);
      return 0;
    }
    return -1;
  }


  pix_pos get_slice(String &slice, pix_len width, pix_pos pos = 0) const
  {
    if (!slice.keep()) slice.keep();
    if (empty()) return -1;

    pix_len len = m_buf.r_avail();
    if (pos < 0) pos += len;
    if (pos < 0 || pos >= len ) return -1;
    const char *rp = m_buf.rp() + pos;
    const char *wp = m_buf.wp();
    pix_len ll = wp - rp;
    if (width < ll) ll = width;
    slice.append(rp, ll);
    rp += ll;
    return rp - m_buf.rp();
  }

  String & get_indent(String &result, 
      pix_len count = 1, const char *indent=" ", 
      const char *neweol=PIX_LF, const char *eol=PIX_EOL) const
  {
    if (!result.keep()) result.clear();
    if (empty()) return result;
    String ind=indent;
    String el = neweol;
    ind.repeat(count);
    pix_pos f = 0;
    bool keep = result.keep(true);
    result.append(ind);
    while ((f = get_line(result, f, eol)) > 0) {
      result.append(el);
      result.append(ind);
    }
    if (f < 0) result.sub(0, -ind.length()-1);
    result.keep(keep);
    return result;
  }


  String & get_escape(String &result, pix_esc esc) const
  {
    if (!result.keep()) result.clear();
    if (empty()) return result;
    if (PIX_ESC_DOT == esc) {
      pix_len len = m_buf.r_avail();
      reserve_back(result.m_buf, len);
      const char *src = m_buf.rp();
      char *dst = result.m_buf.wp();
      pix_len i;
      for (i = 0; i < len; i++)
        dst[i] = esc_dot(src[i]);
      str_end(result.m_buf, len);
    }
    return result;

  }
  String & chars(String &result, pix_enc enc) const
  {
    if (PIX_ENC_AUTO == enc) enc = pix_sys_enc();
    Buffer &buf = result.m_buf;
    buf.reset();
    pix_len slen = m_buf.r_avail();
    if (enc == m_enc) {
      buf.expand(slen + 4);
      memcpy(buf.wp(), m_buf.rp(), slen);
      str_end(buf, slen);
      result.m_enc = enc;
      return result;
    }
    str_bufenc(buf, enc, m_buf.rp(), length(), m_enc);
    result.m_enc = enc;
    return result;
  }
  String & wchars(String &result) const
  {
    if (sizeof(pix_wchar) == 2) return ucs2(result);
    if (sizeof(pix_wchar) == 4) return ucs4(result);
    return result;
  }
  String & ucs2(String &result) const
  {
    pix_enc enc = pix_enc_ucs2();
    chars(result, enc);
    if (PIX_ENC_UTF32LE == m_enc ||
        PIX_ENC_UTF32BE == m_enc) {
      if (result.length() * 2 > length()) 
        pix_log_error("ucs2: out of range", PIX_ERR_ICONV);
    }
    return result;

  }
  String &ucs4(String &result) const
  {
    pix_enc enc = pix_enc_ucs4();
    return chars(result, enc);
  }


};

PIXIMPL_END

PIXAPI_CPP_END

#endif

