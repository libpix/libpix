/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  filter_resize.hpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *        License:  LGPL
 *
 *    Description:  image resizer
 *
 * =============================================================
 */

/**
 * @file
 * @brief %image resizer
 */

#ifndef __PIX_FILTER_RESIZE_HPP__
#define __PIX_FILTER_RESIZE_HPP__

#include <pix/image.hpp>

PIXAPI_CPP_BEG
/**
 * @addtogroup image
 * @{
 * 
 */
class _Resize;
/**
 * @brief %image resizer
 * 
 */

class ResizeFilter : public ImageFilter {
public:
  ResizeFilter(pix_pol pol = PIX_POL_BILINEAR);
  ~ResizeFilter();
public:
  bool zoom(pix_pos cols, pix_pos rows);
  double hres() const;
  double vres() const;
  pix_unit unit() const;
  bool hres(double res);
  bool vres(double res);
  bool unit(pix_unit u);

  bool open(ImageReaderBase &img);
  bool ready();
  pix_pos read(char *buf, pix_pos rows = 0);
private:
  _Resize *m_resize;

};
/**
 * @}
 */

PIXAPI_CPP_END

#endif

