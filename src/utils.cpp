/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  src/utils.cpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 */

#include <pix/utils.hpp>
#include <inner.hpp>
#include <stdio.h>
PIXAPI_CPP_BEG

/* --------------------------IEEE 754---------------------------- */
// NOT working on 32bit systems
/* 
enum {
  EXPN_MASK = 0x7ff0000000000000,
  MANT_MASK = 0x000fffffffffffff,
  FRAC_MASK = 0x001fffffffffffff,
  FIRST_NUM = 0x0010000000000000,
  MOVED_NUM = 0x4000000000000000,
};
*/

PIXIN_BEG
static pix_ulong EXPN_MASK = 0;
static pix_ulong MANT_MASK = 0;
static pix_ulong FRAC_MASK = 0;
static pix_ulong FIRST_NUM = 0;
static pix_ulong MOVED_NUM = 0;

static void float_init()
{
  if (EXPN_MASK) return;
  EXPN_MASK  = 0x7ff00000;
  EXPN_MASK <<= 32;
  MANT_MASK  = 0x000fffff;
  MANT_MASK <<= 32;
  MANT_MASK |= 0xffffffff;
  FRAC_MASK  = 0x001fffff;
  FRAC_MASK <<= 32;
  FRAC_MASK |= 0xffffffff;
  FIRST_NUM  = 0x00100000;
  FIRST_NUM <<= 32;
  MOVED_NUM  = 0x40000000;
  MOVED_NUM <<= 32;

}
PIXIN_END

double float_zero()
{
  return 0.0f;
}

double float_infinite()
{
  union {
    double d;
    pix_ulong l;
  } un;
  un.l = PIXIN::EXPN_MASK;
  return un.d;
}

double float_nan()
{
  union {
    double d;
    pix_ulong l;
  } un;
  un.l = PIXIN::EXPN_MASK + 1;
  return un.d;

}

int float_part(double num, pix_ulong & int_part, int & rexp,
    pix_ulong & frac_part, int prec)
{
  /*
   * IEEE 754
   */
  PIXIN::float_init();

  int_part = 0;
  rexp = 0;
  frac_part = 0;

  union {
    double d;
    pix_ulong l;
  } un;
  int exp;
  pix_ulong man;

  un.d = num;
  exp = static_cast<int>( (un.l & PIXIN::EXPN_MASK) >> 52 );
  man = un.l & PIXIN::MANT_MASK;
  if (exp == 0 && man == 0 ) return 0;          /* zero */
  if (exp > 2046)
    if (man == 0) return 1;                     /* infinite */
    else return -1;                             /* NaN */
  
  if (exp == 0 && man != 0) {                   /* Denormalized */
    man <<= 1;
  } else man |= PIXIN::FIRST_NUM;               /* Normalized */

  exp = exp - 1023 + 1;                         /* exp == 0 means normalized */

  pix_ulong m;
  int e;
  int te = 0;

  if (exp > 0) {                                /* int part */

    m = man;
    e = exp;
    m <<= 10;                                   /* better prec */
    while ( e > 63) {
      m /= 10;
      while ( m < PIXIN::MOVED_NUM ) {
        m <<= 1;
        e --;
      }
      te ++;
    }
    m >>= 63 - e;
    int_part = m;

  }
  rexp = te;

  /*  fractional part */
  m = man;
  e = exp;
  te = 0;

  bool sigdig = false;                          /* using significant digits */
  bool autop = false;                           /* auto prec */

  if (prec == 0) {
    prec = 10;
    autop = true;
  } else if (prec < 0) {
    sigdig = true;
    prec = -prec;
  } 

  if (prec > 15) 
    prec = 15;

  if (e > 53) {                                 /* don't have frac part */
    return 0;
  }
  if (e > 0) {                                  /* have int part */
    m <<= e;
    m &= PIXIN::FRAC_MASK;                      /* remove int part */
    m *= 10;                                    /* the first digit */
  }

  if ( e == 0)                                  /* normalized */
    m *= 10;                                    /* the first digit */

  while ( e < 0 ) {                             /* normalization */
    m *= 10;
    while ( m > PIXIN::FRAC_MASK ) {
      m >>= 1;
      e ++;
      if (e == 0) break;                        /* normalized */
    }

    if (e < 0) te++;                            /* add one zero */

    if (!sigdig && !autop && te > prec) {
      return 0;
    }
  }

  int i;
  pix_ulong t;
  pix_ulong f = 0;
  int p = prec;
  t = m >> 53;
  if (t == 0) {
    te++;
    m *= 10;
  }
  if (!sigdig && !autop) p -= te;

  for (i = 0; i < p; i++) {
    t = m >> 53;                                /* the next digit */
    f = f * 10 + t;                             /* add the next digit */
    t <<= 53;                                   /* move back,omit frac part */
    m -= t;                                     /* get the frac part */
    m *= 10;                                    /* next digit */
  }

  te += p - int_length(f);                      /* add missing zeros */

  t = m >> 53;                                  /* remainder */

  if (t >= 5) {                                 /* round up */
    int len = int_length(f);
    f++;
    if (int_length(f) > len) {
      if (te == 0) {                            /* carry to int_part */
        int_part++;
        f = 0;
        te = len;
      }
      else te--;                                /* remove one zero before*/
    }
  }


  if (autop) f = int_trim(f);

  frac_part = f;
  rexp = -te;
  return 0;
}



PIXAPI_CPP_END


