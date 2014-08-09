/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  view.hpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *        License:  LGPL
 *
 *    Description:  view class for display. 
 *
 * =============================================================
 */

/**
 * @file
 * @brief %view class for display.
 */

#ifndef __PIX_VIEW_HPP__
#define __PIX_VIEW_HPP__

#include <pix/config.h>
#include <pix/image.hpp>
#include <pix/system.hpp>

PIXAPI_CPP_BEG
/**
 * @addtogroup image
 * @{
 * 
 */
class _View;


/**
 * @brief %view class for display
 * 
 */

class View : public ImageWriterBase {
public:
  View();
  ~View();
public:
  friend class _Inner;
public:
  void resize(pix_pos width, pix_pos height);
  pix_pos write(const char *buf, pix_pos rows = 0);
  bool write_image(ImageReaderBase &img);
private:
  _View *m_view;
};

PIXAPI_CPP_END

#ifdef PIX_WIN
PIXAPI_CPP_BEG
void bitblt(View &view, HDC hdc,
    int dstx, int dsty, 
    int width, int height, 
    int srcx, int srcy);
/**
 * @}
 * 
 */
PIXAPI_CPP_END
#endif /* PIX_WIN */

#endif /* __PIX_VIEW_HPP__ */
