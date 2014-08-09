/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  pix_stm.cpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 */

#include <pix_obj.h>



pix_stm 
PIXAPI pix_stm_create()
{
  return new _pix_stm;
}
void 
PIXAPI pix_stm_destroy(pix_stm *stm)
{
  if (!stm) return;
  delete *stm;
  *stm = 0;
}

int 
PIXAPI pix_stm_open_file(pix_stm stm, const char *path, int flag)
{
  if (!stm) return 0;
  if (!stm->fopen(path, flag)) return 0;
  return 1;
}
int 
PIXAPI pix_stm_open_mem(pix_stm stm, int flag)
{
  if (!stm) return 0;
  if (!stm->mopen(flag)) return 0;
  return 1;
}

int 
PIXAPI pix_stm_close(pix_stm stm)
{
  if (!stm) return 0;
  if (!stm->close()) return 0;
  return 1;
}

int 
PIXAPI pix_stm_eof(pix_stm stm)
{
  if (!stm) return 1;
  if (!stm->stm) return 1;
  return (int)stm->stm->eof();
}
pix_pos 
PIXAPI pix_stm_seek(pix_stm stm, pix_pos pos, int whence)
{
  if (!stm) return -1;
  if (!stm->stm) return -1;
  return stm->stm->seek(pos, whence);
}
pix_pos 
PIXAPI pix_stm_tell(pix_stm stm)
{
  if (!stm) return -1;
  if (!stm->stm) return -1;
  return stm->stm->tell();
}
pix_len 
PIXAPI pix_stm_length(pix_stm stm)
{
  if (!stm) return 0;
  if (!stm->stm) return 0;
  return stm->stm->length();
}
pix_len 
PIXAPI pix_stm_trunc(pix_stm stm, pix_len len)
{
  if (!stm) return 0;
  if (!stm->stm) return 0;
  return stm->stm->trunc(len);
}
pix_len 
PIXAPI pix_stm_read(pix_stm stm, char * buf, pix_len len)
{
  if (!stm) return 0;
  if (!stm->stm) return 0;
  return stm->stm->read(buf, len);
}
pix_len 
PIXAPI pix_stm_write(pix_stm stm, const char *buf, pix_len len)
{
  if (!stm) return 0;
  if (!stm->stm) return 0;
  return stm->stm->write(buf, len);
}

const char *
PIXAPI pix_stm_name(pix_stm stm)
{
  if (!stm) return 0;
  if (!stm->stm) return 0;
  return stm->stm->name();
}
int 
PIXAPI pix_stm_flag_get(pix_stm stm)
{
  if (!stm) return 0;
  if (!stm->stm) return 0;
  return stm->stm->flag();
}
int 
PIXAPI pix_stm_flag_read(pix_stm stm)
{
  if (!stm) return 0;
  if (!stm->stm) return 0;
  return (int)stm->stm->is_read();
}
int 
PIXAPI pix_stm_flag_write(pix_stm stm)
{
  if (!stm) return 0;
  if (!stm->stm) return 0;
  return (int)stm->stm->is_write();
}

int 
PIXAPI pix_stm_flag_append(pix_stm stm)
{
  if (!stm) return 0;
  if (!stm->stm) return 0;
  return (int)stm->stm->is_append();
}

void 
PIXAPI pix_stm_mem_swap_buf(pix_stm stm, pix_buf buf)
{
  if (!stm) return;
  if (!stm->mstm) return;
  if (!buf) return;
  if (!buf->buf) return;
  stm->mstm->swap(*buf->buf);
}
int 
PIXAPI pix_stm_mem_reserve(pix_stm stm, int len)
{
  if (!stm) return 0;
  if (!stm->mstm) return 0;
  return stm->mstm->reserve(len);
}

int 
PIXAPI pix_stm_mem_size(pix_stm stm)
{
  if (!stm) return 0;
  if (!stm->mstm) return 0;
  return stm->mstm->size();
}


