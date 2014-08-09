/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  image_bmp.hpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *        License:  LGPL
 *
 *    Description:  bmp image format  
 *
 * =============================================================
 */

/**
 * @file
 * @brief %bmp image format
 * 
 */

#ifndef __PIX_IMAGE_BMP_HPP__
#define __PIX_IMAGE_BMP_HPP__
#include <pix/image.hpp>
PIXAPI_CPP_BEG
/**
 * @addtogroup image
 * @{
 * 
 */
inline pix_len bmp_stride(pix_len rsize)
{
    return (((rsize << 3) +31)>>5)<<2;
}
/**
 * @brief TODO %bmp reader
 * 
 */

class BmpReader : public ImageReader {
public:
private:
};
/**
 * @brief TODO %bmp writer
 * 
 */
class BmpWriter : public ImageWriter {
public:
private:

};
/**
 * @}
 * 
 */

PIXAPI_CPP_END


#endif
