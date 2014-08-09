/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  pix/utils.hpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *        License:  LGPL
 *
 *    Description:  utilities and templates
 *
 * =============================================================
 */
/**
 * @file
 * @brief %utility functions and templates
 */
#ifndef __PIX_UTILS_H__
#define __PIX_UTILS_H__
#include <pix/config.h>

PIXAPI_CPP_BEG
/**
 * @defgroup utils Utility
 * @brief %utilities and templates
 * @{
 */


/* -------------------------- Static ---------------------------- */
/* Deal with IEEE 754, float to fix */
int float_part(double num, pix_ulong & int_part, int & exp,
    pix_ulong & frac_part, int precision = 0);

double float_zero();
double float_infinite();
double float_nan();

/* ------------------- Inlines and Templates ---------------------- */
template <class T>
inline T abs(T v)
{
  if (v < 0) return -v;
  return v;
}

template <class T>
inline void swap(T &a, T &b)
{
  T t = a;
  a = b;
  b = t;
}

template<class T>
inline int int_length(T num, int base = 10)
{
  int ret = 0;
  while (num != 0) {
    num /= base;
    ret ++;
  }
  return ret;
}

template<class T>
inline T int_reverse(T num, int base = 10)
{
  T ret = 0;
  while (num) {
    ret = ret * base + num % base;
    num /= base;
  }
  return ret;
}

template<class T>
inline T int_sub(T num, int beg, int end=-1, int base = 10)
{
  int len = int_length(num , base);
  T rev = num;
  while (beg < 0) beg += len;
  while (end < 0) end += len;
  if (beg <= end) rev = int_reverse(num, base);
  else { beg = beg ^ end; end = beg ^ end; beg = beg ^ end; }
  int i;
  T ret = 0;
  for (i = 0; i < len; i++) {
    int t = rev % base;
    if (i >= beg) {
      ret = ret * base + t;
    }
    if (i == end) break;
    rev /= base;
  }
  return ret;
}

template<class T>
inline T int_power(T num, int n)
{
  int i;
  T ret = 1;
  for (i = 0; i < n; i++) ret *= num;
  return ret;
}

template<class T>
inline T int_exp(T &ret, int n, int base = 10)
{
  ret = 1;
  for (int i = 0; i < n; i++) ret *= base;
  return ret;
}

template<class T>
inline T int_shift(T num, int shift, int base = 10)
{
  int i;
  if (shift < 0) {
    shift = -shift;
    for ( i = 0 ; i < shift; i++)
      num /= base;
  } else {
    for ( i = 0 ; i < shift; i++)
      num *= base;
  }
  return num;
}

template<class T>
inline T int_trim(T num, int base = 10)
{
  if (num == 0) return 0;
  while (num % base == 0) num /= base;
  return num;
}

template<class T, class S>
inline T int_append(T front, S back, int base = 10)
{
  int len = int_length(back, base);
  T ret = int_shift(front, len, base);
  ret += back;
  return ret;
}

template<class T>
inline T trunc(T &intd, double fd)
{
  return intd = static_cast<T>(fd);
}

template<class T>
inline T floor(T &intd, double fd)
{
  T d = static_cast<T>(fd);
  if (fd == d) return intd = d;
  if (fd >=0) return intd = d;
  else return intd = d - 1;
}

template<class T>
inline T ceil(T &intd, double fd)
{
  T d = static_cast<T>(fd);
  if (fd == d) return intd = d;
  if (fd >= 0) return intd = d + 1;
  else return intd = d;
}

template<class T>
inline T round(T &intd, double fd)
{
  fd += 0.5;
  return intd = static_cast<T>(fd);
}

/**
 * @}
 */


PIXAPI_CPP_END



#endif

