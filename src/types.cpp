/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  types.cpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 */
#include <pix/config.h>
#include <string.h>


void 
PIXAPI pix_img_pm_init(pix_img_pm *pm)
{
  if (!pm) return;
  memset(pm , 0, sizeof(pix_img_pm));
}

void 
PIXAPI pix_img_pm_init_cs(pix_img_pm *pm, pix_cs cs)
{
  if (!pm) return;
  memset(pm, 0 , sizeof(pix_img_pm));
  pm->cs = cs;
  switch (cs) {
  case PIX_CS_RGB:
  case PIX_CS_YCbCr:
    pm->bits[0] = 8;
    pm->bits[1] = 8;
    pm->bits[2] = 8;
    break;
  case PIX_CS_CMYK:
    pm->bits[0] = 8;
    pm->bits[1] = 8;
    pm->bits[2] = 8;
    pm->bits[3] = 8;
    break;
  }
}
pix_enc
PIXAPI pix_enc_wchar()
{
  if (sizeof(pix_wchar) == 4) return pix_enc_ucs4();
  return pix_enc_ucs2();
}

pix_enc 
PIXAPI pix_enc_ucs2()
{
  return PIX_ENC_UTF16LE;
}

pix_enc 
PIXAPI pix_enc_ucs4()
{
  return PIX_ENC_UTF32LE;
}

int
PIXAPI pix_enc_width(int enc)
{
  switch (enc) {
    case PIX_ENC_UTF7:
    case PIX_ENC_UTF8:
      return 1;
    case PIX_ENC_UTF16LE:
    case PIX_ENC_UTF16BE:
      return 2;
    case PIX_ENC_UTF32LE:
    case PIX_ENC_UTF32BE:
      return 4;
  }
  return 1;
}

int
PIXAPI pix_enc_max_width(int enc)
{
  switch (enc) {
    case PIX_ENC_UTF7:
    case PIX_ENC_UTF8:
      return 6;
    case PIX_ENC_UTF16LE:
    case PIX_ENC_UTF16BE:
      return 4;
    case PIX_ENC_UTF32LE:
    case PIX_ENC_UTF32BE:
      return 4;
  }
  return 2;
}


const char * 
PIXAPI pix_chars_unit(int unit)
{
  switch (unit) {
  case PIX_UNIT_PIXEL:
    return "pixel";
  case PIX_UNIT_INCH:
    return "inch";
  case PIX_UNIT_METER:
    return "meter";
  case PIX_UNIT_CM:
    return "cm";
  case PIX_UNIT_MM:
    return "mm";
  }
  return "<unknown>";
}

const char *
PIXAPI pix_chars_cs(int cs)
{
  switch (cs) {
  case PIX_CS_GRAY:
    return "GRAY";
  case PIX_CS_RGB:
    return "RGB";
  case PIX_CS_CMY:
    return "CMY";
  case PIX_CS_CMYK:
    return "CMYK";
  case PIX_CS_YCbCr:
    return "YCbCr";
  case PIX_CS_YCCK :
    return "YCCK";
  case PIX_CS_YUV :
    return "YUV";
  case PIX_CS_YUVK :
    return "YUVK";
  case PIX_CS_XYZ :
    return "XYZ";
  case PIX_CS_CIELab :
    return "CIELab";
  case PIX_CS_HSV :
    return "HSV";
  case PIX_CS_HLS :
    return "HLS";
  }
  return "<unknown>";
}

const char * 
PIXAPI pix_chars_radix(int radix)
{
  switch (radix) {
  case PIX_RADIX_DEC : 
    return "dec";
  case PIX_RADIX_HEX :
    return "hex";
  case PIX_RADIX_OCT :
    return "oct";
  case PIX_RADIX_BIN :
    return "bin";
  }
  return 0;
}

const char * 
PIXAPI pix_chars_align(int align)
{
  switch (align) {
  case PIX_ALIGN_LEFT :
    return "left";
  case PIX_ALIGN_RIGHT :
    return "right";
  case PIX_ALIGN_CENTER :
    return "center";
  case PIX_ALIGN_AUTO :
    return "auto";
  }
  return 0;
}


const char * 
PIXAPI pix_chars_enc(int enc)
{
  switch (enc) {
    case PIX_ENC :
      return "NONE";
    case PIX_ENC_AUTO :
      return "AUTO";
    case PIX_ENC_ANSI :
      return "ANSI";
    case PIX_ENC_UTF8 :
      return "UTF-8";
    case PIX_ENC_GB2312:
      return "GB2312";
    case PIX_ENC_GBK :
      return "GBK";
    case PIX_ENC_GB18030:
      return "GB18030";
    case PIX_ENC_BIG5:
      return "BIG5";
    case PIX_ENC_JIS:
      return "JIS";
    case PIX_ENC_SJIS:
      return "Shift-JIS";
    case PIX_ENC_KOREAN:
      return "Korean";

    case PIX_ENC_LATIN1 :
      return "Latin1";
    case PIX_ENC_LATIN2 :
      return "Latin2";
    case PIX_ENC_LATIN3 :
      return "Latin3";
    case PIX_ENC_LATIN4 :
      return "Latin4";
    case PIX_ENC_UTF16LE :
      return "UTF-16LE";
    case PIX_ENC_UTF16BE :
      return "UTF-16BE";
    case PIX_ENC_UTF32LE :
      return "UTF-32LE";
    case PIX_ENC_UTF32BE :
      return "UTF-32BE";
  }
  return "UNKNOWN";

}

const char * 
PIXAPI pix_chars_esc(int esc)
{
  switch(esc) {
  case PIX_ESC :
    return "NONE";
  case PIX_ESC_C :
    return "C";
  case PIX_ESC_OCT :
    return "OCT";
  case PIX_ESC_HEX :
    return "HEX";
  case PIX_ESC_DOT :
    return "DOT";
  }
  return 0;
}

