/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  image_base.hpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *        License:  LGPL
 *
 *    Description:  base image classes.
 *
 * =============================================================
 */

/**
 * @file
 * @brief %base image classes
 */

#ifndef __PIX_IMAGE_H__
#define __PIX_IMAGE_H__
#include <pix/memory.hpp>
#include <pix/stream.hpp>
#include <pix/string.hpp>

PIXAPI_CPP_BEG
/**
 * @defgroup image Image
 * @brief %image classes
 * @{
 */

/**
 * @brief %base class of all images
 *
 * This class stores image parameters,
 * and calculate parameters for read and write.
 * 
 */
class ImageBase : public Base {

public: /* constructors and destructor */

  ImageBase() : m_lock(false) {
    clear();
  }
  ImageBase(const pix_img_pm &pm) : 
    m_pm(pm), m_lock(false), m_line(-1) { sync(); }
  virtual ~ImageBase() { }

public: /* virtual interface */

  /**
   * @brief stride of one row.
   * 
   * Usually return row_size, some format align the output,
   * so the stride maybe greater than row_size.
   *
   */
  virtual pix_len stride() const { return row_size(); } 

  /**
   * @brief get ready for read or write. 
   *
   * Do nothing if already ready.
   * Reset line to 0 if not ready yet.
   * @return true if success , false if fail.
   */
  virtual bool ready() 
  {
    if (line() >= 0) return true;
    if (size() <= 0) return false;
    lock(true);
    line(0);
    return true;
  } 

  /**
   * @brief finish read or write
   *
   */
  virtual bool close() 
  { 
    lock(false); 
    clear(); 
    return true; 
  } 

  /**
   * @brief the next line to read/write.
   * 
   */
  virtual pix_pos tell() const { return line(); }

  /**
   * @brief seek the line to read/write.
   * 
   */
  virtual pix_pos seek(pix_pos, pix_seek whence = PIX_SEEK_SET) { return line(); }

public: /* params */

  /**
   * @brief get all param of the image
   * 
   */
  pix_img_pm param() const { return m_pm; }

  /**
   * @brief get the color space of the image.
   * 
   */
  pix_cs cs() const { return m_pm.cs; }

  /**
   * @brief get columns of the image.
   * 
   */
  pix_pos cols() const { return m_pm.cols; }

  /**
   * @brief get rows of the image.
   * 
   */
  pix_pos rows() const { return m_pm.rows; }

  /**
   * @brief get the horizontal resolution.
   * 
   */
  double hres() const { return m_pm.hres; }

  /**
   * @brief get the vertical resolution.
   * 
   */
  double vres() const { return m_pm.vres; }

  /**
   * @brief get the resolution unit.
   * 
   */
  pix_unit unit() const { return m_pm.unit; }

  /**
   * @brief get image flags.
   * 
   */
  int flag() const { return m_pm.flag; }

  /**
   * @brief clear the param, 
   * @return false if locked.
   * 
   */
  bool clear() {
    if (m_lock) return false;
    pix_img_pm_init(&m_pm);
    m_byte = 0;
    m_bpp = 0;
    m_rsize = 0;
    m_size = 0;
    m_lock = false;
    m_line = -1;
    return true;
  }

  /**
   * @brief set the param.
   * @return false if locked.
   */
  bool param(const pix_img_pm &pm) {
    if (m_lock) return false;
    m_pm = pm;
    sync();
    return true;
  }

  /**
   * @brief set the color space.
   * @return false if locked.
   */
  bool cs_rgb(int r = 8, int g = 8, int b = 8)
  {
    if (m_lock) return false;
    m_pm.cs = PIX_CS_RGB;
    m_pm.bits[0] = r;
    m_pm.bits[1] = g;
    m_pm.bits[2] = b;
    m_pm.bits[3] = 0;
    sync();
    return true;
  }

  /**
   * @brief set the columns.
   * @return false if locked.
   * 
   */
  bool cols(pix_pos col) {
    if (m_lock) return false;
    m_pm.cols = col;
    m_rsize = m_bpp * m_pm.cols;
    m_size = m_rsize * m_pm.rows;
    return true;
  }

  /**
   * @brief set the rows.
   * @return false if locked.
   * 
   */
  bool rows(pix_pos row) 
  {
    if (m_lock) return false;
    m_pm.rows = row;
    m_size = m_rsize * m_pm.rows;
    return true;
  }

  /**
   * @brief set horizontal resolution.
   * @return false if locked.
   * 
   */
  bool hres(double res)
  {
    if (m_lock) return false;
    m_pm.hres = res;
    return true;
  }

  /**
   * @brief set vertical resolution.
   * @return false if locked.
   * 
   */
  bool vres(double res)
  {
    if (m_lock) return false;
    m_pm.vres = res;
    return true;
  }

  /**
   * @brief set the unit.
   * @return false if locked.
   * 
   */
  bool unit(pix_unit unit)
  {
    if (m_lock) return false;
    m_pm.unit = unit;
    return true;
  }

  /**
   * @brief bytes of one component
   * @return the bytes of one component, 
   *      0 if the component is not aligned,
   *      or it is not byte aligned.
   * 
   */
  int byte() const { return m_byte; }

  /**
   * @brief bits/bytes per pixel
   * @return bytes per pixel, if byte aligned,
   *      bits per pixel, if not aligned.
   */
  int bpp() const { return m_bpp; }

  /**
   * @brief size of one row.
   * @see stride
   * 
   */
  pix_len row_size() const { return m_rsize; }

  /**
   * @brief size of the image.
   * 
   */
  pix_len size() const { return m_size; }
  
protected:
  bool lock() const { return m_lock; }
  void lock(bool yn) { m_lock = yn; }
  pix_pos line() const { return m_line; }
  void line(pix_pos l) { m_line = l; }

private:
  ImageBase(const ImageBase &base) { }          /* prohibit copy construction. */
  void sync();
private:
  pix_img_pm m_pm;
  int m_byte;
  int m_bpp;
  pix_len m_rsize;
  pix_len m_size;
  bool m_lock;
  pix_pos m_line;
};

/**
 * @brief %base Class of image readers.
 * 
 */
class ImageReaderBase : virtual public ImageBase {
public:
  virtual ~ImageReaderBase() { }

  /**
   * @brief read the image.
   * @param buf  Buffer to put the data. Make sure the Buffer have enough space.
   * @param rows Rows to read.
   * @return   Rows successfully read.
   * 
   */
  virtual pix_pos read(char *buf, pix_pos rows = 1) = 0;

  /**
   * @brief read thumbnail of the image.
   * @param buf 0 to get the cols rows and size of the thumbnail.
   * @param cols columns of the thumbnail.
   * @param rows rows of the thumbnail.
   * @return  Size of the thumbnail.
   *
   * A thumbnail is always RGB color image, and will be read faster 
   * than the orignal image.
   * If param buf is 0, just do the calculate, otherwise, start reading.
   * Return value is the sizeof the thumbnail, 
   * return 0 if these's no thumbnails.
   * The original value of param cols and rows are suggestions.
   * They will be modified after the calling of this function,
   * return the real cols and rows.
   * The real cols and rows is not more than the original,
   * if originals are 0, return the maximum thumbnail cols and rows.
   * 
   */
  virtual pix_len read_thumb(char *buf, 
      pix_pos & cols, pix_pos & rows)
  {
    cols = 0;
    rows = 0;
    return 0;
  }
  /**
   * @brief seek scanline for read.
   *
   * Usually, forbid reverse seek, read lines and ignore the data 
   * until to the scanline.
   * 
   */
  pix_pos seek(pix_pos pos, pix_seek whence = PIX_SEEK_SET);

};

/**
 * @brief %base class of image writers
 * 
 */
class ImageWriterBase : virtual public ImageBase {
public:
  virtual ~ImageWriterBase() { }
  virtual pix_pos write(const char *buf, pix_pos row = 1) = 0;
  virtual bool write_image(ImageReaderBase &img);
};

/**
 * @brief %base class for the readers of specified image format.
 *
 * This class is an interface of readers read image format
 * from files, buffers and streams.
 */
class ImageReader : public ImageReaderBase {
public:
  ImageReader() :m_stm(0) {}
  virtual ~ImageReader();
public:

  bool open_file(const String &path);
  bool open_mem(const char *buf, pix_len len);
public:
  virtual bool open(Stream &stm) = 0;
  bool close();
private:
  Stream *m_stm;
};

/**
 * @brief %base class for the writers of specified image format.
 *
 * This class is an interface of writers write image format
 * to files, buffers and streams.
 * 
 */
class ImageWriter : public ImageWriterBase {
public:
  ImageWriter() : m_stm(0) { }
  virtual ~ImageWriter();
public:
  bool open_file(const String &path);

public:
  virtual bool open(Stream &stm) = 0;
  bool close();
private:
  Stream *m_stm;
};

/**
 * @brief %base class for filters.
 *
 * A filter reads from an image reader, 
 * transform the data to other format,
 * and behave like an ordinary image reader.
 * 
 */
class ImageFilter : public ImageReaderBase {
public:
  virtual ~ImageFilter() { }
  virtual bool open(ImageReaderBase &base) = 0;
};

/**
 * @brief %image in memory.
 *
 * An image is stored in memory.
 * It can support many operations which can only do in memory,
 * such as rotations, flip.
 * It behaves like both readers and writers.
 * 
 */
class Image : public ImageReaderBase , public ImageWriterBase {
public:
  pix_pos seek(pix_pos pos, pix_seek whence = PIX_SEEK_SET)
  {
    return tell();
  }
};

/**
 * @brief %wrapper class for read any image format.
 * 
 */
class PixReader : public ImageReader {
public:

};

/**
 * @brief %wrapper class for write any image format.
 * 
 */
class PixWriter : public ImageWriter {
public:
};

/**
 * @brief %filter chain class.
 * 
 */
class PixFilter : public ImageFilter {
public:

};
/**
 * @}
 */

PIXAPI_CPP_END

#endif
