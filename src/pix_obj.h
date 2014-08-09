/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  pix.h
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 */

#ifndef __PIX_TYPES_H__
#define __PIX_TYPES_H__
#include <libpix.h>
struct _pix_buf {
  bool own;
  Pix::Buffer *buf;
  
  _pix_buf() : buf(0), own(true)
  {
    buf = new Pix::Buffer;
  }
  _pix_buf(Pix::Buffer *b) : buf(b), own(false)
  {
  }
  ~_pix_buf()
  {
    if (own && buf) delete buf;
  }

};

struct _pix_str {
  bool own;
  Pix::String *str;
  
  _pix_str() : str(0), own(true)
  { 
    str = new Pix::String;
  }
  _pix_str(Pix::String *s) : str(s), own(false)
  {

  }
  ~_pix_str()
  {
    if (own && str) delete str;
  }

};

struct _pix_stm {
  bool own;
  Pix::Stream *stm;
  Pix::FileStream *fstm;
  Pix::MemStream *mstm;

  _pix_stm() : stm(0), fstm(0), mstm(0), own(true)
  {

  }
  _pix_stm(Pix::Stream *s) : stm(s), fstm(0), mstm(0), own(false)
  {

  }
  _pix_stm(Pix::FileStream *s) : stm(s), fstm(s), mstm(0), own(false)
  {

  }
  _pix_stm(Pix::MemStream *s) : stm(s), fstm(0), mstm(s), own(false)
  {

  }
  bool fopen(const char *path, int flag)
  {
    if (stm) return false;
    fstm = new Pix::FileStream;
    if (!fstm->open(path, flag)) {
      delete fstm;
      fstm = 0;
      return false;
    }
    stm = fstm;
    return true;
  }
  bool mopen(int flag)
  {
    if (stm) return false;
    mstm = new Pix::MemStream;
    if (!mstm->open(flag)) {
      delete mstm;
      mstm = 0;
      return false;
    }
    stm = mstm;
    return true;
  }
  bool close()
  {
    if (!stm) return true;
    delete stm;
    stm = 0;
    fstm = 0;
    mstm = 0;
    return true;
  }


};

struct _pix_inf {
  bool own;
  Pix::Info* inf;
  Pix::String *str;
  _pix_inf() : str(0), inf(0), own(true)
  { 
    inf = new Pix::Info;
  }
  _pix_inf(Pix::Info *info) : str(0), inf(info), own(false)
  {

  }
  ~_pix_inf() {
    if (str) delete str;
    if (own && inf) delete inf;
  }
};

#endif
