/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  stream_file.hpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 */

#ifndef __IMPL_STREAM_FILE_WIN__
#define __IMPL_STREAM_FILE_WIN__

#include <impl/config.h>
#include <pix/system.hpp>
#include <pix/string.hpp>

#ifdef PIX_POSIX
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#endif

PIXAPI_CPP_BEG

PIXIMPL_BEG

#ifdef PIX_POSIX
inline int getfl(int flag)
{
  int fl = 0;
  if (flag & PIX_STM_WRITE) fl = O_RDWR;
  else if (flag & PIX_STM_READ) fl = O_RDONLY;
  else return 0;

  //if (flag & PIX_STM_APPEND) fl |= O_APPEND;

  if (flag & PIX_STM_CREATE) fl |= O_CREAT;
  if (flag & PIX_STM_TRUNC)  fl |= O_TRUNC;
  return fl;
}
inline int getprot(int flag)
{
  int prot = 0;
  if (flag & PIX_STM_READ) prot = PROT_READ;
  else if (flag & PIX_STM_WRITE) prot = PROT_READ | PROT_WRITE;
  return prot;
}

inline pix_len getsize(int fd)
{
  struct stat statbuf;
  if (fstat(fd, &statbuf) < 0) return 0;
  return statbuf.st_size;
}

#endif

class FileStream {
public:

#ifdef PIX_WIN
  HANDLE m_hFile;
  HANDLE m_hFileMap;
#endif

#ifdef PIX_POSIX
  int m_fd;
  int m_prot;
#endif

  int m_flag;
  pix_pos m_off;
  pix_len m_len;
  pix_len m_size;

  char *m_buf;
  pix_pos m_bufoff;
  pix_len m_buflen;
  pix_len m_bufsize;
  pix_len m_bufpage;


public:
  char * get_buffer(int &buf_size, int &buf_len, pix_pos &buf_off, pix_pos &buf_pos)
  {
    buf_size = m_bufsize;
    buf_len = m_buflen;
    buf_off = m_bufoff;
    buf_pos = m_off - m_bufoff;
    return m_buf;
  }

public:
  FileStream() : 
    m_flag(0), m_off(0), m_len(0), m_size(0),
    m_buf(0), m_bufoff(0), m_buflen(0), m_bufsize(0)
  { 
#ifdef PIX_WIN
    m_hFile = 0;
    m_hFileMap = 0;
#endif

#ifdef PIX_POSIX
    m_fd = 0;
    m_prot = 0;
#endif

    m_bufpage = pix_sys_gran();
  }
  ~FileStream()
  {
    close();
  }
  bool open(const String &path, int flag = PIX_STM_READ)
  {
    if (!flag) {
      pix_log_error("file: already open, close first", PIX_ERR_FILE);
      return false;
    }
    int error = 0;
#ifdef PIX_WIN
    if (m_hFile) return false;
    m_hFile = Win::create_file(path, flag);
    if (m_hFile == INVALID_HANDLE_VALUE) {
      m_hFile = 0;
      error = 1;
    } else m_len = Win::file_size(m_hFile);
#endif

#ifdef PIX_POSIX
    int fl = getfl(flag);
    mode_t mode = 0777 ^ pix_sys_umask();
    mode &= 0666; /* forbit execute prvilege */
    int fd = ::open(path, fl, mode);
    if (fd != -1) {
      m_fd = fd;
      m_prot = getprot(flag);
      m_len = getsize(fd);
    } else error = 1;
#endif
    if (error) {
      m_flag = 0;
      pix_log_error("file: open error", PIX_ERR_FILE);
      return false;
    }

    m_size = m_len;
    m_flag = flag;
    return true;
  }
  bool close()
  {
    if (!m_flag) return true;
    if (m_len < m_size) trunc(m_len);
    unmap();
#ifdef PIX_WIN
    if (m_hFileMap) Win::close(m_hFileMap);
    if (m_hFile) Win::close(m_hFile);
    m_hFileMap = 0;
    m_hFile = 0;
#endif
#ifdef PIX_POSIX
    if (m_fd < 0) return true;
    if (!unmap()) return false;
    if (::close(m_fd) == -1) return false;

#endif
    m_off = 0;
    m_len = 0;
    m_size = 0;
    return true;
  }


  bool map(pix_pos pos, pix_len len)
  {
    if (m_size <= 0) return true;
    pix_pos end = pos + len - 1;
    pix_pos page = pos / m_bufpage;
    pix_pos pend = end / m_bufpage;
    pix_pos boff = page * m_bufpage;
    pix_len blen = m_size - boff;
    pix_len size = m_bufpage * (pend - page + 1);
    if (blen > size) blen = size;

    if (m_buf) {
      if (boff >= m_bufoff && boff + blen <= m_bufoff + m_buflen) return true;
      if (!unmap()) return false;
    }
#ifdef PIX_WIN
    if (m_hFileMap == 0)
      m_hFileMap = Win::create_file_map(m_hFile, m_flag, m_size);
    if (m_hFileMap) 
      m_buf = Win::file_map_view(m_hFileMap, m_flag, boff, blen);
#endif

#ifdef PIX_POSIX
    m_buf = (char *) mmap(0, blen, m_prot, MAP_SHARED, m_fd, boff);
    if (m_buf == MAP_FAILED) {
      pix_log_error("file map: error.", PIX_ERR_FILE);
      m_buf = 0;
    }
    /* 
    if (-1==madvise(m_buf, blen, MADV_WILLNEED | MADV_SEQUENTIAL))
    {
      perror("madvise error");

    }
    */
#endif

    if (m_buf == 0) {
      pix_log_error("file map: error." , PIX_ERR_FILE);
      m_bufoff = 0;
      m_buf = 0;
      m_buflen = 0;
      return false;
    }
    m_bufoff = boff;
    m_buflen = blen;
    ///* DEBUG
    if (pix_log() > 3) {
      String str;
      str-"file map: off="-m_bufoff-", len="-m_buflen;
      pix_log_msg(str);
    }
    //*/
    return true;

  }
  bool unmap()
  {
    if (m_buf) {
      int error = 0;
#ifdef PIX_WIN
      if (!Win::file_unmap_view(m_buf))
        error = 1;
#endif

#ifdef PIX_POSIX
      if (-1 == munmap(m_buf, m_buflen)) 
        error = 1;
#endif
      if (error) {
        pix_log_error("file: unmap error", PIX_ERR_FILE);
        return false;
      }
      /* DEBUG
      if (pix_log() >= 3) {
        String str;
        str-"file unmap: off="-m_bufoff-", len="-m_buflen;
        pix_log_msg(str);
      }
      */
      m_bufoff = 0;
      m_buf = 0;
      m_buflen = 0;
    }
    return true;
  }

  bool flush()
  {
    if (!(m_flag & PIX_STM_WRITE)) return false;
    int error = 0;
#ifdef PIX_WIN
    if (m_buf) 
      if (!Win::file_flush_view(m_buf)) 
        error = 1;
#endif

#ifdef PIX_POSIX
    if (m_buf) 
      if (-1 == msync(m_buf, m_buflen, MS_SYNC)) 
        error = 1;
#endif
    if (error) {
      pix_log_error("file: flush error", PIX_ERR_FILE);
      return false;
    }
    /* DEBUG
    if (pix_log() >= 3) {
      String str;
      str-"file flush: off="-m_bufoff-", len="-m_buflen;
      pix_log_msg(str);
    }
    */

	  return true;
  }

  pix_len trunc(pix_len len)
  {
    if (len < 0) return m_len;
    if (!(m_flag & PIX_STM_WRITE)) return m_len;
    if (!unmap()) return m_len;
    int error = 0;
#ifdef PIX_WIN
    if (m_hFileMap) Win::close(m_hFileMap);
    m_hFileMap = 0;
    Win::file_seek(m_hFile, len, FILE_BEGIN);
    if (!Win::file_end(m_hFile)) error = 1;
#endif

#ifdef PIX_POSIX
    if (::ftruncate(m_fd, len) < 0) error = 1;
#endif
    if (error)
    {
      pix_log_error("file: truncate error", PIX_ERR_FILE);
      return m_len;
    }
    /* DEBUG
    if (pix_log() >= 3) {
      String str;
      str-"file trunc: size="-len;
      pix_log_msg(str);
    }
    */
    m_size = len;
    m_len = len;
    return m_len;
  }

  pix_len reserve(pix_len len)
  {
    if (len < 0) return 0; 
    if (!(m_flag & PIX_STM_WRITE)) return 0;
    pix_pos off;
    if (m_flag & PIX_STM_APPEND) off = m_len;
    else off = m_off;
    if (off + len > m_size) {
      pix_len t = (off + len - 1) / m_bufpage;
      t = m_bufpage * (t + 1);
      pix_len l = m_len;
      trunc(t);
      m_len = l;
    }
    if (m_flag & PIX_STM_APPEND) return m_size - m_len;
    return m_size - m_off;
  }

  bool eof() const
  {
    if (!m_flag) return true;
    if (m_off >= m_len) return true;
    return false;
  }

  pix_pos seek(pix_pos pos, int whence)
  {
    pix_long p = pos;

    switch (whence) {
    case PIX_SEEK_SET:
      break;
    case PIX_SEEK_CUR:
      p += m_off;
      break;
    case PIX_SEEK_END:
      p += m_len;
      break;
    }
    if (p < 0) return p = 0;
    if (sizeof(m_off) != sizeof(pix_long)) {
      if (p >> 31) {
        pix_log_error("file : overflow. 32-bit limit 2G bytes, use 64-bit.", PIX_ERR_OVERFLOW);
        return m_off;
      }
      m_off = p;
    } else m_off = p;
    return m_off;
  }
  pix_pos tell() const
  {
    return m_off;
  }
  pix_len length() const
  {
    return m_len;
  }

public:
  const char *rp(pix_len &len, pix_len minlen = 1)
  {
    if (len < 0) len = 0;
    pix_long off = len;
    len = 0;
    if (!(m_flag & PIX_STM_READ)) return 0;

    off += m_off;

    if (off < 0) return 0;

    if (sizeof(m_off) != sizeof(pix_long)) {
      if ((off + minlen) >> 31) {
        pix_log_error("file : overflow. 32-bit limit 2G bytes, use 64-bit.", PIX_ERR_OVERFLOW);
        return 0;
      }
    }

    m_off = off;
    if (off >= m_len) {
      return 0;
    }

    if (minlen == 0) {
      pix_pos pos = off - m_bufoff;
      if (pos <= m_buflen) {
        len = m_buflen - pos;
        return m_buf + pos;
      }
      return 0;
    }

    if (!map(off, minlen)) return 0;

    pix_pos pos = off - m_bufoff;
    len = m_buflen - pos;
    return m_buf + pos;
  }

  char *wp(pix_len &len, pix_len minlen = 1)
  {
    if (len < 0) len = 0;
    pix_long off = len;
    len = 0;

    if (!(m_flag & PIX_STM_WRITE)) return 0;

    if (m_flag & PIX_STM_APPEND) off += m_len;
    else off += m_off;

    if (off < 0) return 0;

    if (sizeof(m_off) != sizeof(pix_long)) {
      if ((off + minlen) >> 31) {
        pix_log_error("file : overflow. 32-bit limit 2G bytes, use 64-bit.", PIX_ERR_OVERFLOW);
        return 0;
      }
    }

    if (off > m_len) m_len = off;


    if (!(m_flag & PIX_STM_APPEND)) m_off = off;


    if (minlen == 0) {
      pix_pos pos = off - m_bufoff;
      if (pos <= m_buflen) {
        len = m_buflen - pos;
        return m_buf + pos;
      }
      return 0;
    }

    reserve(minlen);

    if (!map(off, minlen)) return 0;

    pix_pos pos = off - m_bufoff;
    len = m_buflen - pos;

    return m_buf + pos;
  }

};


PIXIMPL_END

PIXAPI_CPP_END


#endif
