/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  string.hpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *        License:  LGPL
 *
 *    Description:  string class
 *
 * =============================================================
 */
/**
 * @file
 * @brief %string class
 */

#ifndef __PIX_STRING_HPP__
#define __PIX_STRING_HPP__
#include <pix/config.h>
#include <pix/memory.hpp>

PIXAPI_CPP_BEG

/**
 * @defgroup string String
 * @brief %string, encoding, data to string convertion
 * @{
 * 
 */
class _String;
/**
 * @brief %string class have encoding convertion 
 */
class String : public Base {
public:
  friend class _Inner;

public: /* Construction and destructions */
  String(const char *buf = 0 , pix_len length = 0,
      pix_enc encode = PIX_ENC_AUTO);           /**< Constructor */
  String(const pix_wchar *buf, pix_len length = 0);
  String(const String &str);                    /**< Copy Constructor */
  ~String();                                    /**< Destructor */

public: /* Member operators */
  String & operator=(char ch) { return assign(ch); }
  String & operator=(pix_wchar ch) { return assign(ch); }
  String & operator=(const char *str) { return assign(str); }
  String & operator=(char *str) { return assign(str); }
  String & operator=(const pix_wchar *str) { return assign(str); }
  String & operator=(pix_wchar *str) { return assign(str);}
  String & operator=(const String &str) { return assign(str); }
  String & operator=(String &str) { return assign(str); }

  template<class T>
  String & operator=(T& val) { return format(this->clear(), val); }

  template<class T>
  String & operator=(const T& val) { return format(this->clear(), val); }

  bool operator==(const String &str) const { return equal(str); }
  bool operator!=(const String &str) const { return !equal(str); }
  char operator[](pix_pos pos) const { return at(pos); }
  char& operator[](pix_pos pos) {return at(pos); }
public: /* Casting operators */

  /**
   * @brief Cast to const char*.
   * @return 0 if encoding is not ansi compatible.
   */
  operator const char*() const
  {
    if (width() == 1) return c_str();
    return 0;
  }
  operator char*()
  {
    if (width() == 1) return str();
    return 0;
  }

  /**
   * @brief Cast to const pix_wchar *, or const wchar_t *.
   * @return 0 if the encoding is not same as the wchar_t's encoding.
   */
  operator const pix_wchar*() const
  {
    if (encode() == pix_enc_wchar()) return (const pix_wchar*)c_str();
    return 0;
  }
  operator pix_wchar*()
  {
    if (encode() == pix_enc_wchar()) return (pix_wchar*)str();
    return 0;
  }
  /**
   * @brief Cast to const pix_ucs2 *.
   * @return 0 if the encoding is not same as pix_ucs2
   * 
   */
  operator const pix_ucs2*() const
  {
    if (encode() == pix_enc_ucs2() ) return (const pix_ucs2*)c_str();
    return 0;
  }
  operator pix_ucs2*()
  {
    if (encode() == pix_enc_ucs2() ) return (pix_ucs2*)str();
    return 0;
  }
  /**
   * @brief Cast to const pix_ucs4 *.
   * @return 0 if the encoding is not same as pix_ucs4
   */
  
  operator const pix_ucs4*() const
  {
    if (encode() == pix_enc_ucs4() ) return (const pix_ucs4*)c_str();
    return 0;
  }
    
  operator pix_ucs4*()
  {
    if (encode() == pix_enc_ucs4() ) return (pix_ucs4*)str();
    return 0;
  }
  /*
  operator char*();
  operator pix_wchar*();
  operator pix_ucs2*();
  operator pix_ucs4*();
  */

public: /* Interface */
  /**
   * @brief Minimum bytes of the encoding , 1 for ansi compatible encoding.
   */
  int width() const { return pix_enc_width(encode()); }
  /**
   * @brief Get encoding
   */
  pix_enc encode() const;
  /**
   * @brief Set encoding
   *
   * Do NOT do any conversion, just modify the encoding.
   * use chars() to do the conversion.
   * 
   */
  pix_enc encode(pix_enc enc);
  /**
   * @brief Test if empty.
   */
  bool empty() const;
  /**
   * @brief Test equal to str.
   */
  bool equal(const String &str) const;
  /**
   * @brief Get allocated size
   * 
   */
  pix_len size() const;
  /**
   * @brief Get effective length.
   */
  pix_len length() const;
  /**
   * @brief Get length()/width();
   * 
   */
  pix_len strlen() const { return length() / width(); }

  /**
   * @brief Get pointer to the const buffer
   * @see operator const char *()
   *
   * never return zero, different from operator const char *().
   */
  const char * c_str() const;
  /**
   * @brief Get pointer to the buffer
   * @see operator char*()
   *
   * never return zero, different from operator char *().
   * 
   */
  char * str();

  /**
   * @brief Get char at pos
   * 
   */
  char& at(pix_pos pos);
  /**
   * @brief Set char at pos
   */
  char at(pix_pos pos) const;
  /**
   * @brief Swap str and self
   */
  String & swap(String &str);
  /**
   * @brief Swap buf and self buf
   */
  void swap(Buffer & buf);
  /**
   * @brief Reserve back or front 
   * @param size <0 Reserve |size| bytes before the string
   *        >0 Reserve |size| bytes after the string
   */

  String & reserve(pix_len size);
  /**
   * @brief Set to empty string
   * 
   */

  String & clear();
  /**
   * @brief Assign with char, convert with encoding
   * @param ch The char to assign
   * @param count Repeat count times
   */
  String & assign(char ch, pix_len count = 1);

  String & assign(pix_wchar ch, pix_len count = 1);

  /**
   * @brief Assign with buffer,  DO NOT do any conversion.
   */
  String & assign(const char *buf, pix_len len = 0, pix_enc encode = PIX_ENC_AUTO);
  /*
   * 
   */
  String & assign(const pix_wchar *buf, pix_len len = 0);

  /**
   * @brief Assign with String object, convert with encoding
   */
  String & assign(const String & str);

  /**
   * @brief Concatenate char, convert with encoding
   * @param ch The char to concatenate
   * @param count Repeat count times
   */

  String & append(char ch, pix_len count = 1);

  String & append(pix_wchar ch, pix_len count = 1);
  /**
   * @brief Concatenate buffer, DO NOT do any conversion
   */
  String & append(const char *buf, pix_len len = 0, pix_enc encode = PIX_ENC_AUTO);
  /*
   * 
   */
  String &append(const pix_wchar *buf, pix_len len = 0);
  /**
   * @brief Concatenate String object, convert with encoding
   */
  String & append(const String & str);
  /**
   * @brief Insert [count] chars to the string
   * @param pos position before the char
   * @param ch char to put
   * @param count number of chars to put
   */

  String & insert(pix_pos pos, char ch, pix_len count = 1);

  String & insert(pix_pos pos, pix_wchar ch, pix_len count = 1);
  /**
   * @brief Insert str to the string
   * @param pos position before the char
   * @param str string to put
   * @param len length of str. if len == 0, then len = strlen(str)
   * @param encode encoding of str
   */
  String & insert(pix_pos pos, const char *str, pix_len len = 0, 
      pix_enc encode = PIX_ENC_AUTO);

  String & insert(pix_pos pos, const pix_wchar *str, pix_len len = 0);
  /**
   * @brief Insert str to the string
   * 
   */
  String & insert(pix_pos pos, const String &str);
  /**
   * @brief Replace [beg, end] to chars
   * @param beg begin position.
   * @param end end position.
   * @param ch char to put
   * #param count repeat [count] times
   */

  String & replace(pix_pos beg, pix_pos end, char ch, pix_len count = 1);
  String & replace(pix_pos beg, pix_pos end, const char *str, pix_len len = 0, 
      pix_enc encode = PIX_ENC_AUTO);
  String & replace(pix_pos beg, pix_pos end, const String &str);

  String & remove(pix_pos beg, pix_pos end = -1);

  /**
   * @brief Trim the String, delete blank character
   */
  String & trim();
  /**
   * @brief Set the string range [beg, end] 
   *
   *
   */
  
  String & sub(pix_pos beg ,pix_pos end= -1);

  String & repeat(int count = 2);

  /**
   * @brief Reverse the string
   */
  String & reverse() { return sub(-1 , 0); }

  /**
   * @brief Truncate the string.
   * @param len The bytes to keep.
   * If the length() < len, then fill 0s in the blank.
   */
  String & trunc(pix_len len);

  pix_pos find(char ch, pix_len count = 1, pix_pos beg = 0, pix_pos end = -1) const;
  pix_pos find(const char *buf, pix_len len = 0, pix_pos beg = 0, pix_pos end = -1) const;
  pix_pos find(const String &str, pix_pos beg = 0, pix_pos end = -1 ) const;


  bool keep() const;
  bool keep(bool val);

  String & get_sub(String &result, pix_pos beg, pix_pos end = -1) const;

  pix_pos get_token(String &result, pix_pos pos = 0,
    const char *delim=PIX_DELIM ) const;

  pix_pos get_line(String &result, pix_pos pos = 0,
    const char * eol=PIX_EOL ) const;

  pix_pos get_slice(String &result, pix_len len, pix_pos pos = 0) const;

  String & get_indent(String &result, pix_len count =1, const char *indent=" ", 
      const char *neweol=PIX_LF, const char *eol=PIX_EOL ) const;

  String & get_escape(String &result, pix_esc esc = PIX_ESC_C) const;

  /**
   * @brief Transform to other encoding.
   *
   * 
   */
  String & chars(String &result, pix_enc enc = PIX_ENC_AUTO) const;

  /**
   * @brief Transform to pix_wchar, same as wchar_t in standard C.
   *
   * 4 bytes in linux, 2 bytes in windows.
   * 
   */
  String & wchars(String &result) const;

  /**
   * @brief Transform to ucs2.
   *
   * Note: the diffrence between ucs2 and utf-16.
   * ucs2 is a subset of ucs4, fixed width character, or just 16 bit integer.
   * utf-16 is variable width encoding for ucs4.  
   * in unicode basic multilingual plane, the utf-16 and ucs2 are the same.
   * ucs2 can't represent unicode character larger than U+FFFF.
   * utf-16 can represent full unicode ucs4.
   *
   * This function usually use utf-16 as ucs2, if it is out of range,
   * it will log an error PIX_ERR_ICONV.
   */
  String & ucs2(String &result) const;

  /**
   * @brief Transform to ucs4.
   *
   * The full unicode character.
   */
  String & ucs4(String &result) const;


private:
  _String *m_str;
};

String& format(String &result, int num, pix_radix radix = PIX_RADIX_DEC);
String& format(String &result, unsigned int num, pix_radix radix = PIX_RADIX_DEC);
String& format(String &result, long num, pix_radix radix = PIX_RADIX_DEC);
String& format(String &result, unsigned long num, pix_radix radix = PIX_RADIX_DEC);
String& format(String &result, pix_long num, pix_radix radix = PIX_RADIX_DEC);
String& format(String &result, double num , int precision = 0, int scientific = 0);
String& format(String &result, void *ptr);

template<class T>
inline String & operator+=(String &str, const T& post) 
{ 
  return str.append(post); 
}

template<class T>
inline String operator+(const String &str, const T & post)
{
  String ret = str;
  ret.append(post);
  return ret;
}

inline String & operator *=(String &str, int count)
{
  return str.repeat(count);
}

inline String operator*(const String &str, int count)
{
  String ret = str;
  return ret.repeat(count);
}
inline String operator>>(const String &str, int indent)
{
  String ret;
  str.get_indent(ret, indent);
  return ret;
}

inline String &operator-(String &str, char back)
{
  return str.append(back);
}
inline String &operator-(String &str, pix_wchar back)
{
  return str.append(back);

}

inline String &operator-(String &str, const char *back)
{
  return str.append(back);
}
inline String &operator-(String &str, char *back)
{
  return str.append(back);
}
inline String &operator-(String &str, const pix_wchar *back)
{
  return str.append(back);
}
inline String &operator-(String &str, pix_wchar *back)
{
  return str.append(back);
}
inline String &operator-(String &str, const String &back)
{
  return str.append(back);
}

inline String &operator-(String &str, String &back)
{
  return str.append(back);
}

inline String & operator-=(char front , String &str)
{
  return str.insert(0, front);
}
inline String & operator-=(pix_wchar front, String &str)
{
  return str.insert(0, front);
}
inline String & operator-=(const char * front , String &str)
{
  return str.insert(0, front);
}
inline String & operator-=(char * front , String &str)
{
  return str.insert(0, front);
}
inline String & operator-=(const pix_wchar *front, String &str)
{
  return str.insert(0, front);
}
inline String & operator-=(pix_wchar *front, String &str)
{
  return str.insert(0, front);
}

inline String & operator-=(const String & front , String &str)
{
  return str.insert(0, front);
}
inline String & operator-=(String & front , String &str)
{
  return str.insert(0, front);
}

/**
 * @brief Append string to the post
 * @Note Support chain operation, from left to right
 * code
 *    String str("test");
 *    str-"1234"-'5'-678-String("90");
 * endcode
 * 
 */
template<class T>
inline String & operator-(String &str, T& back)
{
  bool keep = str.keep(true);
  format(str, back);
  str.keep(keep);
  return str;
}
template<class T>
inline String & operator-(String &str, const T& back)
{
  bool keep = str.keep(true);
  format(str, back);
  str.keep(keep);
  return str;
}

/**
 * @brief Append string to the front
 * Note Support chain operation, from right to left
 * code
 *  String str("test");
 *  "1234"-='5'-=678-=String("90")-=str;
 * endcode
 * 
 */


template<class T>
inline String & operator-=(const T& front , String &str)
{
  String s;
  format(s, front);
  str.insert(0, s);
  return str;
}

/**
 * @}
 */

PIXAPI_CPP_END

#endif
