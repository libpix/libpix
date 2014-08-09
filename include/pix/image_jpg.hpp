/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  image_jpg.hpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *        License:  LGPL
 *
 *    Description:  jpeg image format 
 *
 * =============================================================
 */

/**
 * @file
 * @brief %jpeg image format
 */

#ifndef __PIX_IMAGE_JPG_HPP__
#define __PIX_IMAGE_JPG_HPP__

#include <pix/image.hpp>

PIXAPI_CPP_BEG 
/**
 * @addtogroup image
 * @{
 * 
 */

class _Jpg;

/**
 * @brief %jpeg reader.
 */
class JpgReader : public ImageReader{
public:
  JpgReader();
  ~JpgReader();
public:
  bool open(Stream &stm);
  bool close();
  bool ready();
  pix_pos read(char *buf, pix_pos rows = 1);

private:

  _Jpg * m_jpg;
};

/**
 * @brief %jpeg writer.
 */
class JpgWriter : public ImageWriter{
public:
  JpgWriter();
  ~JpgWriter();
public:
  /** compression quality: 0-100 */
  void quality(int q);
public:
  bool open(Stream &stm);
  bool close();
  bool ready();
  pix_pos write(const char *buf, pix_pos rows = 1);
private:
  _Jpg * m_jpg;
};
/**
 * @}
 */

PIXAPI_CPP_END

#endif
