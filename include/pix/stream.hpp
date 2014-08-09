/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  stream.hpp 
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *        License:  LGPL
 *
 *    Description:  stream classes
 *
 * =============================================================
 */

/**
 * @file
 * @brief %stream classes
 */
#ifndef __PIX_STREAM_H__
#define __PIX_STREAM_H__
#include <pix/memory.hpp>
#include <pix/string.hpp>

PIXAPI_CPP_BEG
/**
 * @defgroup stream Stream
 * @brief %file, memory streams
 * @{
 */

/**
 * @brief %base class of streams
 */

class Stream : public Base {
public: /* constructor and destructor */
  Stream() : m_flag(0) { }
  virtual ~Stream() { };
public: /* virutal interface */
  virtual bool close() = 0;
  virtual bool eof() const = 0;
  virtual pix_pos seek(pix_pos pos , int whence = PIX_SEEK_SET) = 0;
  virtual pix_pos tell() const= 0;
  virtual pix_len length() const= 0;
  virtual pix_len trunc(pix_len len) = 0;
  virtual pix_len reserve(pix_len len) = 0;
  virtual bool flush() = 0;
  virtual const char *rp(pix_len &len, pix_len minlen=0) = 0;
  virtual char *wp(pix_len &len, pix_len minlen=0) = 0;
  virtual pix_len read(char *buf , pix_len len);
  virtual pix_len write(const char *buf , pix_len len);
  virtual pix_len write(Stream &stm, pix_len len = 0);

public: /* common interface */

  const char * name() const { return m_name.c_str(); }
  int flag() const { return m_flag; }
  bool is_open() const { return !!m_flag;}
  bool is_read() const   { return !!(m_flag & PIX_STM_READ); }
  bool is_write() const  { return !!(m_flag & PIX_STM_WRITE); }
  bool is_append() const { return !!(m_flag & PIX_STM_APPEND); }

protected:
  int m_flag;
  String m_name;
};

class _FileStream;

/**
 * @brief %file stream
 * 
 */
class FileStream : public Stream {
public:
  friend class _Inner;
public: /* constructor and destructor */
  FileStream();
  ~FileStream();
public: /* special Interface for FileStream */
  bool open(const String &path, int flag = PIX_STM_READ);
public: /* virtual interface */
  bool close();
  bool eof() const;
  pix_pos seek(pix_pos pos , int whence = PIX_SEEK_SET);
  pix_pos tell() const;
  pix_len length() const;
  pix_len trunc(pix_len len);
  pix_len reserve(pix_len len);
  bool flush();
  const char *rp(pix_len &len, pix_len minlen = 0);
  char *wp(pix_len &len, pix_len minlen = 0);
  pix_len read(char *buf , pix_len len);
  pix_len write(const char *buf , pix_len len);
  pix_len write(Stream &stm, pix_len len = 0);

private:
  FileStream(const FileStream &fstm) { }
  _FileStream * m_stm;
};

class _MemStream;
/**
 * @brief %memory stream
 * 
 */

class MemStream : public Stream {
public:
  friend class _Inner;
public: /* constructor and destructor */
  MemStream();
  ~MemStream();
public: /* special interface for MemStream */
  bool open(int flag = PIX_STM_RWA);
  bool open(const char *buf, int len);
  bool open(char * buf, int len);
  void swap(Buffer &buf);
  pix_len size() const;
public: /* virtual interface */
  bool close();
  bool eof() const;
  pix_pos seek(pix_pos pos , int whence = PIX_SEEK_SET);
  pix_pos tell() const;
  pix_len length() const;
  pix_len trunc(pix_len len);
  pix_len reserve(pix_len len);
  bool flush() { return true; }
  const char *rp(pix_len &len, pix_len minlen = 0);
  char *wp(pix_len &len, pix_len minlen = 0);
  pix_len read(char *buf , pix_len len);
  pix_len write(const char *buf , pix_len len);
  pix_len write(Stream &stm, pix_len len = 0);
private:
  MemStream(const MemStream &mstm) { }

  _MemStream * m_stm;
};
/**
 * @}
 */

PIXAPI_CPP_END

#endif
