/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  string.cpp
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
 * @brief impl. of string
 */

#include <pix/string.hpp>
#include <pix/utils.hpp>
#include <pix/info.hpp>
#include "impl/string.hpp"
#include "inner.hpp"
#include <stdio.h>

PIXAPI_CPP_BEG

char *str_null()
{
  static char null[] = { 0, 0, 0, 0};
  return null;
}
class _String {
public:
  impl::String impl;
public:
  _String(const char *buf, pix_len len , pix_enc enc) :
    impl(buf, len , enc) { }
  _String(const pix_wchar *buf, pix_len len) :
    impl(buf, len) { }
  _String(const impl::String &str) :
    impl(str) { }
};


String::String(const char *buf, pix_len len, pix_enc enc)
{
  m_str = new _String(buf, len, enc);
}
String::String(const pix_wchar *buf, pix_len len)
{
  m_str = new _String(buf, len);
}

String::String(const String &str)
{
  m_str = new _String(str.m_str->impl);
}

String::~String()
{
  delete m_str;
}

const char * String::c_str() const
{
  return m_str->impl.c_str(); 
}

char * String::str()
{
  return m_str->impl.str();
}

pix_len String::length() const
{
  return m_str->impl.length();
}

pix_len String::size() const
{
  return m_str->impl.size();
}

bool String::empty() const
{
  return m_str->impl.empty();
}

String & String::clear()
{
  m_str->impl.clear();
  return *this;
}
String & String::reserve(pix_len len)
{
  m_str->impl.reserve(len);
  return *this;
}
String & String::trunc(pix_len len)
{
  m_str->impl.trunc(len);
  return *this;
}

pix_enc String::encode() const
{
  return m_str->impl.encode();
}
pix_enc String::encode(pix_enc enc)
{
  return m_str->impl.encode(enc);
}
String & String::assign(char ch, pix_len count)
{
  m_str->impl.assign(ch, count);
  return *this;
}
String & String::assign(pix_wchar ch, pix_len count)
{
  m_str->impl.assign(ch, count);
  return *this;
}

String & String::assign(const char *buf, pix_len len, pix_enc enc)
{
  m_str->impl.assign(buf, len, enc);
  return *this;

}
String & String::assign(const pix_wchar *buf, pix_len len)
{
  m_str->impl.assign(buf, len);
  return *this;
}

String & String::assign(const String &str)
{
  m_str->impl.assign(str.m_str->impl);
  return *this;
}


String & String::append(char ch, pix_len len)
{
  m_str->impl.append(ch, len);
  return *this;
}
String & String::append(pix_wchar ch, pix_len len)
{
  m_str->impl.append(ch, len);
  return *this;
}


String & String::append(const char *buf, pix_len len, pix_enc enc)
{
  m_str->impl.append(buf, len, enc);
  return *this;
}

String & String::append(const pix_wchar *buf, pix_len len)
{
  m_str->impl.append(buf, len);
  return *this;
}


String & String::append(const String &str)
{
  m_str->impl.append(str.m_str->impl);
  return *this;
}


String & String::insert(pix_pos pos, char ch, pix_len len)
{
  m_str->impl.insert(pos, ch, len);
  return *this;
}
String & String::insert(pix_pos pos, pix_wchar ch, pix_len len)
{
  m_str->impl.insert(pos, ch, len);
  return *this;
}

String & String::insert(pix_pos pos, const char *buf, pix_len len, pix_enc enc)
{
  m_str->impl.insert(pos, buf, len, enc);
  return *this;
}
String & String::insert(pix_pos pos, const pix_wchar *buf, pix_len len)
{
  m_str->impl.insert(pos, buf, len);
  return *this;
}

String & String::insert(pix_pos pos, const String &str)
{
  m_str->impl.insert(pos, str.m_str->impl);
  return *this;
}

String & String::replace(pix_pos beg, pix_pos end, char ch, pix_len count)
{
  m_str->impl.replace(beg, end, ch, count);
  return *this;
}

String & String::replace(pix_pos beg, pix_pos end, const char *str, pix_len len, pix_enc enc)
{
  m_str->impl.replace(beg, end, str, len, enc);
  return *this;
}
String & String::replace(pix_pos beg, pix_pos end, const String &str)
{
  m_str->impl.replace(beg, end, str.m_str->impl);
  return *this;
}

String & String::trim()
{
  m_str->impl.trim();
  return *this;
}

String & String::sub(pix_pos beg, pix_pos end)
{
  m_str->impl.sub(beg, end);
  return *this;
}

String & String::remove(pix_pos beg, pix_pos end)
{
  m_str->impl.remove(beg, end);
  return *this;
}

String & String::get_sub(String &sub, pix_pos beg, pix_pos end) const
{
  m_str->impl.get_sub(sub.m_str->impl, beg, end);
  return sub;
}


String & String::repeat(int count)
{
  m_str->impl.repeat(count);
  return *this;

}

pix_pos String::find(char ch, pix_len count, pix_pos beg, pix_pos end) const
{
  return m_str->impl.find(ch, count, beg, end);

}

pix_pos String::find(const char *buf , pix_len len, pix_pos beg, pix_pos end) const
{
  return m_str->impl.find(buf, len, beg, end);

}

pix_pos String::find(const String &str, pix_pos beg, pix_pos end) const
{
  return m_str->impl.find(str.m_str->impl, beg, end);

}

String& String::swap(String &str)
{
  _String *temp = m_str;
  m_str = str.m_str;
  str.m_str = temp;
  return *this;
}

void String::swap(Buffer &buf)
{
  m_str->impl.swap(buf);
}

bool String::equal(const String &str) const
{
  return m_str->impl.equal(str.m_str->impl);
}

pix_pos String::get_line(String &line, pix_pos pos, 
    const char * eol) const
{
  return m_str->impl.get_line(line.m_str->impl, pos, eol);
}

pix_pos String::get_slice(String &slice, pix_len len, pix_pos pos) const
{
  return m_str->impl.get_slice(slice.m_str->impl, len, pos);
}

String & String::get_escape(String &result, pix_esc esc) const
{
  m_str->impl.get_escape(result.m_str->impl, esc);
  return result;
}

char String::at(pix_pos pos) const
{
  return m_str->impl.at(pos);
}
char & String::at(pix_pos pos)
{
  return m_str->impl.at(pos);
}

bool String::keep(bool val)
{
  return m_str->impl.keep(val);
}

bool String::keep() const
{
  return m_str->impl.keep();

}

String & String::get_indent(String &result, 
    pix_len count, const char *indent, 
    const char *neweol, const char *eol) const
{
  m_str->impl.get_indent(result.m_str->impl, 
      count, indent, neweol, eol);
  return result;
}

String& String::chars(String &result, pix_enc enc) const
{
  m_str->impl.chars(result.m_str->impl, enc);
  return result;
}

String& String::wchars(String &result) const
{
  m_str->impl.wchars(result.m_str->impl);
  return result;
}
String& String::ucs2(String &result) const
{
  m_str->impl.ucs2(result.m_str->impl);
  return result;
}
String& String::ucs4(String &result) const
{
  m_str->impl.ucs4(result.m_str->impl);
  return result;
}

#ifdef PIX_VC
#pragma warning(disable: 4146)
#endif

template <class T>
String& int_to_str(String & str, T num , pix_radix radix = PIX_RADIX_DEC)
{
  if (num == 0) {
    str+='0';
    return str;
  }
  char temp[80];
  char *p = temp;
  if (num < 0) {
    *p++ ='-';
    num = -num;
  }
  int base = 10;

  if (radix == PIX_RADIX_DEC) base = 10;
  else if (radix == PIX_RADIX_HEX) base = 16;
  else if (radix == PIX_RADIX_OCT) base = 8;
  else if (radix == PIX_RADIX_BIN) base = 2;

  int len = int_length(num);
  pix_ulong reverse = int_reverse((pix_ulong) num, base);

  while (len) {
    char ch;
    int t = reverse % base;
    if (t < 10) ch = '0' + t;
    else ch = 'a' + t - 10;
    *p++= ch;
    reverse /=base;
    len --;
  }
  *p = 0;
  return str.append(temp);
}

String& format(String &ret, int num , pix_radix radix)
{
  if (!ret.keep()) ret.clear();
  return int_to_str(ret, num , radix);
}
String& format(String &ret, unsigned int num , pix_radix radix)
{
  if (!ret.keep()) ret.clear();
  return int_to_str(ret, num , radix);
}

String& format(String &ret, long num , pix_radix radix)
{
  if (!ret.keep()) ret.clear();
  return int_to_str(ret, num , radix);
}
String& format(String &ret, unsigned long num , pix_radix radix)
{
  if (!ret.keep()) ret.clear();
  return int_to_str(ret, num , radix);
}


String& format(String &ret, pix_long num , pix_radix radix)
{
  if (!ret.keep()) ret.clear();
  return int_to_str(ret, num , radix);
}

String& format(String &str, double num , int prec, int sci)
{
  if (!str.keep()) str.clear();
  pix_ulong ip;
  int ex;
  pix_ulong fp;

  int p;
  if (prec > 15) prec = 15;
  else if (prec < -15) prec = -15;              /* max precision */

  if (sci < 0 || sci > 2) sci = 0;              /* 0:auto 1:scientific 2:non-scientif */

  if (prec == 0) p = 10;                        /* auto precision */
  else if (prec > 0) p = prec;                  /* precision */
  else p = -prec;                               /* significant digits */


  int ret = float_part(num, ip, ex, fp, prec);  /* disassemble IEEE754 double float */

  if (ret == -1) return str+="<NaN>";
  if (ret == 1) 
    if (num > 0) return str+="<infinite>";
    else return str+="<-infinite>";
  if (num < 0) str+='-';
  bool sc = false;

  if (sci == 0 ) {                              /* auto scientific */
    if (ex > 0) sc = true;                      /* large number */
    if (ip == 0 && ex < -10) sc = true;         /* small fraction */
  } else if (sci == 1) sc = true;               /* force scientific */

  if (sc) {                                     /* scientific */
    int ee;
    pix_ulong nu;
    int len;

    if (ex > 0) {                               /* large number */
      nu = ip;
      len = int_length(nu);
      nu = int_sub(nu, 0, p);                   /* first p+1 numbers */
      if (nu % 10 > 4) nu = nu / 10 + 1;        /* round up */
      else nu = nu / 10;
      ex = ex + len - p;                        /* fix exponent */
      len = int_length(nu);
      ee = len + ex - 1;                        /* scientific exponent */
      if (prec == 0) {
        nu = int_trim(nu);                      /* trim the trailing zeros */
        if (nu < 10) nu *= 10;
      }

      String ss;
      int_to_str(ss, nu);
      str += ss.insert(1, '.');
    } else if (ip == 0) {                       /* pure fraction */
      if (fp == 0) return str+="0.0";
      ee = ex - 1;                              /* scientifc exponent */
      nu = fp;
      String ss;
      if (nu < 10) nu *= 10;                    /* deal with one digit  */
      int_to_str(ss, nu);
      ss.insert(1, '.');
      str+=ss;
    } else {                                    /* mixed */
      nu = ip;
      ee = int_length(nu) - 1;                  /* scientific exponent */
      nu = int_shift(nu, -ex);                  /* add zeros */
      nu = int_append(nu, fp);                  /* add ramainder */

      if (int_length(nu) > p) {
        nu = int_sub(nu, 0, p);               /* first p+1 numbers */
        if (nu % 10 > 4) nu = nu / 10 + 1;      /* round up */
        else nu = nu / 10;
      }

      if (prec == 0) {                          /* auto precision */
        nu = int_trim(nu);                      /* trim the trailing zeros */
        if (nu < 10) nu *= 10;
      }

      String ss;
      int_to_str(ss, nu);
      str += ss.insert(1, '.');

    }
    str.append('E');
    int_to_str(str, ee);                        /* add scientific exponent */
    return str;
  }
  if (1) {                                      /* non-scientific */
    int_to_str(str, ip);
    if (ex > 0) str.append('0', ex);
    if (fp) {                                   /* fraction part is not zero */
      str.append('.');
      if (ex < 0) str.append('0', -ex);
      int_to_str(str, fp);
    } else {                                    /* fraction part is zero */
      str.append('.');
      if (prec == 0) str.append('0');
      else str.append('0', p);
    }
  }
  return str;
}

String& format(String &result, void *ptr)
{
  if (!result.keep()) result.clear();
  int len = sizeof(void*) * 2;
  int i = 0;
  pix_ulong num = (pix_ulong) ptr;
  pix_ulong revert = 0;
  for(i = 0; i < len; i++) {
    revert <<= 4;
    revert += num & 0x0f;
    num >>= 4;
  }
  for (i = 0; i < len; i++) {
    int n = revert & 0x0f;
    char ch;
    if (n >=0 && n <= 9) ch = '0' + n;
    else ch = 'A' + n - 10;
    result-ch;
    revert >>= 4;
  }
  return result;
}



Buffer & _Inner::get_buffer(const String &str)
{
  return str.m_str->impl.get_buffer();
}



PIXAPI_CPP_END




