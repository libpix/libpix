/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  image_tif.hpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *        License:  LGPL
 *
 *    Description:  tiff image format
 *
 * =============================================================
 */

/**
 * @file
 * @brief %tiff image format
 *
 */

#ifndef __PIX_IMAGE_TIF_HPP__
#define __PIX_IMAGE_TIF_HPP__

#include <pix/image.hpp>

PIXAPI_CPP_BEG 
/**
 * @addtogroup image
 * @{
 */

class _Tif;
/**
 * @brief TODO tiff reader
 * 
 */

class TifReader : public ImageReader {
public:
private:
  _Tif *m_tif;
};
/**
 * @brief TODO tiff writer
 * 
 */
class TifWriter : public ImageWriter {
public:
private:
  _Tif *m_tif;

};
/**
 * @}
 * 
 */
PIXAPI_CPP_END
#endif
