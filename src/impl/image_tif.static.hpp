/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  image_tif.static.hpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 */

#ifndef __IMPL_IMAGE_TIF_HPP__
#define __IMPL_IMAGE_TIF_HPP__
#include <impl/config.h>
#include <tiffio.h>
PIXAPI_CPP_BEG
PIXIMPL_BEG

typedef struct {
  int key;
  int val;
} map_t;

inline int find(int t, const map_t *map, int len, bool reverse = false)
{
  int i;
  if (reverse) {
    for (i = 0; i < len; i++)
      if (map[i].val == t) return map[i].key;
    return -1;
  }
  for (i = 0; i < len ; i++)
    if (map[i].key == t) return map[i].val;
  return -1;
}

inline int tif_cprs(int t, bool reverse = false) 
{
  const map_t map[] = 
  {
    { COMPRESSION_NONE          , PIX_CPRS_NONE },
    { COMPRESSION_CCITTRLE      , PIX_CPRS_RLE },
    { COMPRESSION_CCITTFAX3     , PIX_CPRS_FAX3 }, 
    { COMPRESSION_CCITTFAX4     , PIX_CPRS_FAX4 },
    { COMPRESSION_LZW           , PIX_CPRS_LZW },
    { COMPRESSION_DEFLATE       , PIX_CPRS_DEFLATE },
    { COMPRESSION_ADOBE_DEFLATE , PIX_CPRS_ADOBE },
    { COMPRESSION_OJPEG         , PIX_CPRS_OJPG },
    { COMPRESSION_JPEG          , PIX_CPRS_JPG },
    { COMPRESSION_JBIG          , PIX_CPRS_JBIG },
    { COMPRESSION_JP2000        , PIX_CPRS_JPG2K },
    { COMPRESSION_NEXT          , PIX_CPRS_NEXT },
    { COMPRESSION_CCITTRLEW     , PIX_CPRS_RLEW }, 
    { COMPRESSION_PACKBITS      , PIX_CPRS_PACKBITS },
    { COMPRESSION_THUNDERSCAN   , PIX_CPRS_THUNDERSCAN },
    { COMPRESSION_IT8CTPAD      , PIX_CPRS_IT8CTPAD },
    { COMPRESSION_IT8LW         , PIX_CPRS_IT8LW },
    { COMPRESSION_IT8MP         , PIX_CPRS_IT8MP },
    { COMPRESSION_IT8BL         , PIX_CPRS_IT8BL },
    { COMPRESSION_PIXARFILM     , PIX_CPRS_PIXARFILM },
    { COMPRESSION_PIXARLOG      , PIX_CPRS_PIXARLOG },
    { COMPRESSION_DCS           , PIX_CPRS_DCS },
    { COMPRESSION_SGILOG        , PIX_CPRS_SGILOG },
    { COMPRESSION_SGILOG24      , PIX_CPRS_SGILOG24 },
  };
  const int len = sizeof(map) / sizeof(map_t);
  return find(t, map, len, reverse);
}

inline int tif_cs(int t, bool reverse = false)
{
  const map_t map[] = {
    { PHOTOMETRIC_MINISWHITE , PIX_CS_GRAY },
    { PHOTOMETRIC_MINISBLACK , PIX_CS_GRAY },
    { PHOTOMETRIC_RGB        , PIX_CS_RGB },
    { PHOTOMETRIC_PALETTE    , PIX_CS_PALETTE },
    //case PHOTOMETRIC_MASK:
    { PHOTOMETRIC_SEPARATED  , PIX_CS_CMYK },
    { PHOTOMETRIC_YCBCR      , PIX_CS_YCbCr },
    { PHOTOMETRIC_CIELAB     , PHOTOMETRIC_ICCLAB },
    { PHOTOMETRIC_ITULAB     , PIX_CS_CIELab },
    { PHOTOMETRIC_LOGL       , PIX_CS_LogL },
    { PHOTOMETRIC_LOGLUV     , PIX_CS_LogLuv },
  };
  const int len = sizeof(map) / sizeof(map_t);
  return find(t, map, len, reverse);
}

inline int tif_ort(int t, bool reverse = false)
{
  const map_t map[] = {
    { ORIENTATION_TOPLEFT  , PIX_ORT_TOPLEFT },
    { ORIENTATION_TOPRIGHT , PIX_ORT_TOPRIGHT },
    { ORIENTATION_BOTLEFT  , PIX_ORT_BOTLEFT },
    { ORIENTATION_BOTRIGHT , PIX_ORT_BOTRIGHT },
    { ORIENTATION_LEFTTOP  , PIX_ORT_LEFTTOP },
    { ORIENTATION_LEFTBOT  , PIX_ORT_LEFTBOT },
    { ORIENTATION_RIGHTTOP , PIX_ORT_RIGHTTOP },
    { ORIENTATION_RIGHTBOT , PIX_ORT_RIGHTBOT },
  };
  const int len = sizeof(map) / sizeof(map_t);
  return find(t, map, len, reverse);

}
tsize_t tif_read(thandle_t fd, tdata_t buf, tsize_t size);
tsize_t tif_write(thandle_t fd, tdata_t buf, tsize_t size);
toff_t  tif_seek(thandle_t fd, toff_t off, int whence);
int tif_close(thandle_t fd);
toff_t tif_size(thandle_t fd);
int tif_map(thandle_t fd, tdata_t *addr, toff_t * len);
void tif_unmap(thandle_t fd, tdata_t addr, toff_t len);

class Tif {
public:
  TIFF *m_tif;
public:
  Tif()
  {

  }
  bool open_read(Stream &stm)
  {
    m_tif = TIFFClientOpen("", "r", (thandle_t) this,
        tif_read, tif_write, tif_seek, tif_close, 
        tif_size, tif_map, tif_unmap );
    if (!m_tif) return false;
  }
  bool open_write(Stream &stm)
  {
    m_tif = TIFFClientOpen("", "w", (thandle_t) this,
        tif_read, tif_write, tif_seek, tif_close, 
        tif_size, tif_map, tif_unmap );
    if (!m_tif) return false;
  }
public:
  pix_cprs get_compression()
  {
    uint16 t;
    TIFFGetField(m_tif, TIFFTAG_COMPRESSION, &t);
    return (pix_cprs) tif_cprs(t);
  }
  pix_cs get_cs()
  {
    uint16 t;
    TIFFGetField(m_tif, TIFFTAG_PHOTOMETRIC, &t);
    return (pix_cs) tif_cs(t);
  }
  int get_bits_per_sample()
  {
    uint16 t;
    TIFFGetField(m_tif, TIFFTAG_BITSPERSAMPLE, &t);
    return t;
  }

};

tsize_t tif_read(thandle_t fd, tdata_t buf, tsize_t size)
{
  Tif *tif = (Tif*) fd;
  return 0;
}

tsize_t tif_write(thandle_t fd, tdata_t buf, tsize_t size)
{
  Tif *tif = (Tif*) fd;
  return 0;
}
toff_t  tif_seek(thandle_t fd, toff_t off, int whence)
{
  Tif *tif = (Tif*) fd;
  return 0;
}

int tif_close(thandle_t fd)
{
  Tif *tif = (Tif*) fd;
  return 0;
}
toff_t tif_size(thandle_t fd)
{
  Tif *tif = (Tif*) fd;
  return 0;

}
int tif_map(thandle_t fd, tdata_t *addr, toff_t * len)
{
  Tif *tif = (Tif *) fd;
  return 0;

}
void tif_unmap(thandle_t fd, tdata_t addr, toff_t len)
{
  Tif *tif = (Tif *) fd;
  return;
}



PIXIMPL_END
PIXAPI_CPP_END

#endif
