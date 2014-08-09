/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  pix_buf.cpp
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


pix_buf
PIXAPI pix_buf_create()
{
  return new _pix_buf;
}
void 
PIXAPI pix_buf_destroy(pix_buf *buf)
{
  if (!buf) return;
  if (!*buf) return;
  delete *buf;
  *buf = 0;
}

void 
PIXAPI pix_buf_resize(pix_buf buf, pix_len size)
{
  if (!buf) return;
  if (!buf->buf) return;
  buf->buf->resize(size);
}

void 
PIXAPI pix_buf_expand(pix_buf buf, pix_len size)
{
  if (!buf) return;
  if (!buf->buf) return;
  buf->buf->expand(size);
  
}
void 
PIXAPI pix_buf_move(pix_buf buf, pix_pos pos, pix_len size)
{
  if (!buf) return;
  if (!buf->buf) return;
  buf->buf->move(pos, size);
}
void 
PIXAPI pix_buf_release(pix_buf buf)
{
  if (!buf) return;
  if (!buf->buf) return;
  buf->buf->release();
}

char *
PIXAPI pix_buf_bp(pix_buf buf)
{
  if (!buf) return 0;
  if (!buf->buf) return 0;
  return buf->buf->bp();
}
char *
PIXAPI pix_buf_rp(pix_buf buf)
{
  if (!buf) return 0;
  if (!buf->buf) return 0;
  return buf->buf->rp();
}
char *
PIXAPI pix_buf_wp(pix_buf buf)
{
  if (!buf) return 0;
  if (!buf->buf) return 0;
  return buf->buf->wp();
}
char *
PIXAPI pix_buf_ep(pix_buf buf)
{
  if (!buf) return 0;
  if (!buf->buf) return 0;
  return buf->buf->ep();
}
char *
PIXAPI pix_buf_bp_set(pix_buf buf, char *bp)
{
  if (!buf) return 0;
  if (!buf->buf) return 0;
  return buf->buf->bp(bp);
}
char *
PIXAPI pix_buf_rp_set(pix_buf buf, char *rp)
{
  if (!buf) return 0;
  if (!buf->buf) return 0;
  return buf->buf->rp(rp);
}
char *
PIXAPI pix_buf_wp_set(pix_buf buf, char *wp)
{
  if (!buf) return 0;
  if (!buf->buf) return 0;
  return buf->buf->wp(wp);
}
char *
PIXAPI pix_buf_ep_set(pix_buf buf, char *ep)
{
  if (!buf) return 0;
  if (!buf->buf) return 0;
  return buf->buf->ep(ep);
}
char *
PIXAPI pix_buf_rp_move(pix_buf buf, pix_pos off)
{
  if (!buf) return 0;
  if (!buf->buf) return 0;
  return buf->buf->rp(off);
}
char *
PIXAPI pix_buf_wp_move(pix_buf buf, pix_pos off)
{
  if (!buf) return 0;
  if (!buf->buf) return 0;
  return buf->buf->wp(off);
}

void 
PIXAPI pix_buf_reset(pix_buf buf)
{
  if (!buf) return;
  if (!buf->buf) return;
  buf->buf->reset();
}

void 
PIXAPI pix_buf_r_reset(pix_buf buf)
{
  if (!buf) return;
  if (!buf->buf) return;
  buf->buf->r_reset();
}
void 
PIXAPI pix_buf_w_reset(pix_buf buf)
{
  if (!buf) return;
  if (!buf->buf) return;
  buf->buf->w_reset();
}

pix_pos 
PIXAPI pix_buf_r_pos(pix_buf buf)
{
  if (!buf) return -1;
  if (!buf->buf) return -1;
  return buf->buf->r_pos();
}
pix_pos 
PIXAPI pix_buf_w_pos(pix_buf buf)
{
  if (!buf) return -1;
  if (!buf->buf) return -1;
  return buf->buf->w_pos();
}
pix_len 
PIXAPI pix_buf_avail(pix_buf buf)
{
  if (!buf) return 0;
  if (!buf->buf) return 0;
  return buf->buf->avail();
}
pix_len 
PIXAPI pix_buf_p_avail(pix_buf buf)
{
  if (!buf) return 0;
  if (!buf->buf) return 0;
  return buf->buf->p_avail();
}
pix_len 
PIXAPI pix_buf_r_avail(pix_buf buf)
{
  if (!buf) return 0;
  if (!buf->buf) return 0;
  return buf->buf->r_avail();
}
pix_len 
PIXAPI pix_buf_w_avail(pix_buf buf)
{
  if (!buf) return 0;
  if (!buf->buf) return 0;
  return buf->buf->w_avail();
}

pix_len 
PIXAPI pix_buf_length(pix_buf buf)
{
  if (!buf) return 0;
  if (!buf->buf) return 0;
  return buf->buf->length();
}
pix_len 
PIXAPI pix_buf_size(pix_buf buf)
{
  if (!buf) return 0;
  if (!buf->buf) return 0;
  return buf->buf->size();
}

void 
PIXAPI pix_buf_swap(pix_buf buf1, pix_buf buf2)
{
  if (!buf1) return;
  if (!buf1->buf) return;
  if (!buf2) return;
  if (!buf2->buf) return;
  buf1->buf->swap(*buf2->buf);

}

