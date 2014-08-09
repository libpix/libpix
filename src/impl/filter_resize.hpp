/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  filter_resize.hpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 */
#include <impl/config.h>
#include <pix/image.hpp>
#include <string.h>

PIXAPI_CPP_BEG
PIXIMPL_BEG

inline void make_sentinel(char *dst, char *src, int bpp, int num)
{
  while (num --) {
    memcpy(dst, src, bpp);
    dst += bpp;
  }
}
inline void bilinear(char * dst, char *src[], int bpp, 
    pix_uint u, pix_uint v, int radix)
{
  pix_uint one = 1 << radix;
  pix_uint pm[4];
  pm[3] = u * v;
  pm[2] = v * (one - u);
  pm[1] = u * (one - v);
  pm[0] = (one - u) * (one - v);
  pix_uint t;
  while (bpp--) {
    t  = (pix_uchar) src[0][bpp] * pm[0];
    t += (pix_uchar) src[1][bpp] * pm[1];
    t += (pix_uchar) src[2][bpp] * pm[2];
    t += (pix_uchar) src[3][bpp] * pm[3];
    t >>= radix * 2;
    dst[bpp] = t;
  }
}

inline pix_uint sinx_div_x(int d, int radix, int a)
{
  double x = d;
  pix_uint one = 1 << radix;
  x /= one;
  x = abs(x);
  double x2 = x * x;
  double x3 = x2 * x;
  if (x <= 1)
    return round(one, one * ((a+2) * x3 - (a+3) * x2 + 1));
  else if (x <= 2)
    return round(one, one * ( (a * x3 
            - 5 * a * x2 + 8 * a * x - 4 * a)) );
  return 0;
}
inline pix_uchar sat_color(int t)
{
  if ( t < 0) return 0;
  if ( t > 255) return 255;
  return t;
}

inline void bicubic(char *dst, char *src[], int bpp, 
    pix_uint u, pix_uint v, int radix, int a)
{
  pix_uint one = 1 << radix;
  pix_uint two = 1 << (radix + 1);
  int afu[4], afv[4];
  afu[0] = sinx_div_x(one + u, radix, a);
  afu[1] = sinx_div_x(u, radix, a);
  afu[2] = sinx_div_x(one - u, radix, a);
  afu[3] = sinx_div_x(two - u, radix, a);
  afv[0] = sinx_div_x(one + v, radix, a);
  afv[1] = sinx_div_x(v, radix, a);
  afv[2] = sinx_div_x(one - v , radix, a);
  afv[3] = sinx_div_x(two - v , radix, a);
  int tt, t;
  int i, j;
  while (bpp--) {
    t = 0;
    for (i = 0; i < 4; ++i) {
      tt = 0;
      for (j = 0; j < 4; ++j) {
        tt += afu[j] * (pix_uchar)src[i * 4 + j][bpp];
      }
      t += afv[i] * tt;
    }
    t >>= radix * 2;
    dst[bpp] = (char) sat_color(t);
  }
}

class Resize {
public:
  typedef struct {
    pix_int i;
    pix_uint d;
  } num_t;
public:
  pix_pol m_pol;
  ImageReaderBase *m_img;
  int m_bpp;
  pix_pos m_cols;
  pix_pos m_rows;
  double m_hres;
  double m_vres;
  pix_unit m_unit;
  int m_radix;
  int m_sentinel;
  pix_len m_sentisize;
  pix_pos m_sline;    /* src line */
  pix_pos m_scanline; /* dst line  */
  int m_lines;
  pix_len m_linesize;
  num_t* m_srcx;
  num_t* m_srcy;
  Buffer m_buf;
  char **m_line;
  bool m_pass;

public:
  Resize(pix_pol pol) : m_pol(pol), m_srcx(0), m_srcy(0), m_line(0)
  {
    clear();
  }
  ~Resize()
  {
    if (m_srcx) delete []m_srcx;
    if (m_srcy) delete []m_srcy;
    if (m_line) delete []m_line;
  }
public:
  void clear()
  {
    m_img = 0;
    m_bpp = 0;
    m_cols = 0;
    m_rows = 0;
    m_hres = 0.0;
    m_vres = 0.0;
    m_unit = PIX_UNIT;
    m_radix = 8;
    m_sentinel = 0;
    m_sentisize = 0;
    m_sline = 0;
    m_scanline = -1;
    m_lines = 0;
    m_linesize = 0;
    m_line = 0;
    m_pass = false;
    if (m_srcx) delete []m_srcx;
    if (m_srcy) delete []m_srcy;
    if (m_line) delete []m_line;
    m_srcx = 0;
    m_srcy = 0;
  }

  pix_img_pm param()
  {
    pix_img_pm pm;
    if (!m_img) {
      pix_img_pm_init(&pm);
      return pm;
    }
    pm = m_img->param();
    pm.cols = m_cols;
    pm.rows = m_rows;
    pm.hres = m_hres;
    pm.vres = m_vres;
    pm.unit = m_unit;
    pm.type = PIX_IMG_FLT;
    pm.flt.type = PIX_FLT_RESIZE;
    return pm;
  }

  bool open(ImageReaderBase & img)
  {
    if (!img.ready()) return false;
    m_img = &img;
    m_bpp = m_img->bpp();
    m_cols = m_img->cols();
    m_rows = m_img->rows();
    m_hres = m_img->hres();
    m_vres = m_img->vres();
    m_unit = m_img->unit();
    return true;
  }
  bool zoom(pix_pos c, pix_pos r)
  {
    if (c == 0 && r == 0) return false;
    if (c == 0) {
      double t = (double) m_img->cols() / m_img->rows();
      round(c, t * r);
    } else if (r == 0) {
      double t = (double) m_img->rows() / m_img->cols();
      round(r, t * c);
    }
    m_cols = c;
    m_rows = r;
    m_hres = m_hres * c / m_img->cols();
    m_vres = m_vres * r / m_img->rows();
    return true;
  }
  bool ready()
  {
    if (m_scanline >= 0) return true;
    m_pass = false;
    if ( m_img->cols() == m_cols && m_img->rows() == m_rows) {
      m_pass = true;
      return true;
    }
    switch(m_pol) {
      case PIX_POL_NEAREST:
        return ready_near();
      case PIX_POL_BILINEAR:
        return ready_linear();
      case PIX_POL_BICUBIC:
        return ready_cubic();
    }
    return false;
  }
  pix_pos read(char *buf, pix_pos rows)
  {
    if (!m_img) return 0;
    if (!ready()) return 0;
    if (m_pass) return m_img->read(buf, rows);
    switch (m_pol) {
      case PIX_POL_NEAREST:
        {
          pix_pos i;
          for (i = 0; i < rows; i++)
            if (!read_near(buf)) return i;
          return i;
        }
      case PIX_POL_BILINEAR:
        {
          pix_pos i;
          for (i = 0; i < rows; i++)
            if (!read_linear(buf)) return i;
          return i;
        }
      case PIX_POL_BICUBIC:
        {
          pix_pos i;
          for (i = 0; i < rows; i++)
            if (!read_cubic(buf)) return i;
          return i;
        }
    }
    return 0;
  }



public:
  void calc()
  {
    m_srcx = new num_t[m_cols];
    m_srcy = new num_t[m_rows];
    m_line = new char *[m_lines];
    double xr = m_img->cols();
    double yr = m_img->rows();
    xr /= m_cols;
    yr /= m_rows;
    pix_pos x, y;
    for ( x = 0 ; x < m_cols ; x++) {
      //double t = xr * x;
      double t = x;
      t += 0.5;
      t *= xr;
      t += 0.5;
      floor(m_srcx[x].i, t);
      t -= m_srcx[x].i;
      t *= 1<<m_radix;
      round(m_srcx[x].d, t);
    }
    for ( y = 0 ; y < m_rows ; y++) {
      //double t = yr * y;
      double t = y;
      t += 0.5;
      t *= yr;
      t += 0.5;
      floor(m_srcy[y].i, t);
      t -= m_srcy[y].i;
      t *= 1<<m_radix;
      round(m_srcy[y].d, t);
    }
    m_sentisize = m_bpp * m_sentinel; 

    m_linesize = m_img->row_size() + 2 * m_sentisize;
    m_buf.expand( m_linesize * m_lines );
    int i = 0;
    for (i = 0 ; i < m_lines; i++) {
      m_line[i] = m_buf.bp() + m_linesize * i;
    }

  }
  bool load_one(char *buf)
  {
    if (!m_img->read(buf + m_sentisize, 1)) return false;
    make_sentinel(buf, buf + m_sentisize, m_bpp, m_sentinel);
    make_sentinel(buf + m_linesize - m_sentisize,
        buf + m_linesize - m_sentisize - m_bpp, 
        m_bpp, m_sentinel);
    return true;
  }
  bool load(char *buf[], int beg, int end)
  {
    int i, l;
    if (beg < m_sentinel) {
      if (m_img->seek(0) != 0) return false;
      if (!load_one(buf[0])) return false;
      for (i = beg + 1; i <= m_sentinel; i++) {
        memcpy(buf[i - beg], buf[0], m_linesize);

      }
      for (i = m_sentinel + 1; i < end; i++) {
        l = i - m_sentinel;
        if (l != m_img->seek(l)) return false;
        if (!load_one(buf[i - beg])) return false;

      }
      return true;
    }
    int k = m_img->rows() + m_sentinel;
    if (end > k) {
      for ( i = beg; i < k; i++) {
        l = i - m_sentinel;
        if (l != m_img->seek(l)) return false;
        if (!load_one(buf[i - beg])) return false;
      }
      for ( i = k; i < end; i++) {
        memcpy(buf[i - beg], buf[k - 1 - beg], m_linesize);

      }
      return true;
    }
    for (i = beg; i < end; i++) {
      l = i - m_sentinel;
      if (l != m_img->seek(l)) return false;
      if (!load_one(buf[i - beg])) return false;
    }
    return true;
  }
  bool get_line(int l)
  {
    int p = l;
    int plast = p + m_lines;
    int q = m_sline - m_lines;
    int qlast = m_sline;
    if (p == q) return true;
    if ((plast <= q) || (qlast <=p) ) {
      if (!load(m_line , p , plast)) return false;
      m_sline = plast;
      return true;
    }
    int i;
    char *tl;
    if ( p > q) {
      l = qlast - p;
      for ( i = 0 ; i < l ; i++) {
        tl = m_line[i + p - q];
        m_line[i + p - q] = m_line[i];
        m_line[i] = tl;
      }
      if (!load(m_line + l , qlast , plast)) return false;
      m_sline = plast;
      return true;
    }
    if ( p < q) {
      l = plast - q;
      for ( i = l - 1 ; i >= 0 ; i--) {
        tl = m_line[i];
        m_line[i] = m_line[m_lines - i];
        m_line[m_lines - i] = tl;
      }
      if (!load(m_line , p , q)) return false;
      m_sline = plast;
      return true;
    }
    return true;
  }

  bool ready_near()
  {
    m_sentinel = 1;
    m_lines = 2;
    m_scanline = 0;
    m_sline = 0;
    calc();
    pix_pos x, y;
    pix_pos c, r;
    c = m_img->cols();
    r = m_img->rows();
    for ( x = 0 ; x < m_cols ; x++) {
      m_srcx[x].d >>= m_radix - 1;
      if (m_srcx[x].d && m_srcx[x].i < c) m_srcx[x].i++;
    }
    for ( y = 0 ; y < m_rows ; y++) {
      m_srcy[y].d >>= m_radix - 1;
      if (m_srcy[y].d && m_srcy[y].i < r) m_srcy[y].i++;
    }
    return true;
  }

  bool read_near(char *buf)
  {
    if (!get_line(m_srcy[m_scanline].i)) return false;
    char *pixel;
    pix_pos i;
    for (i = 0; i < m_cols; i++) {
      pixel = m_line[0] + m_srcx[i].i * m_bpp;
      memcpy(buf, pixel, m_bpp);
      buf += m_bpp;
    }
    m_scanline ++;
    return true;
  }
  bool ready_linear()
  {
    m_sentinel = 1;
    m_lines = 2;
    m_scanline = 0;
    m_sline = 0;
    calc();
    return true;
  }
  bool read_linear(char *buf)
  {
    if (!get_line(m_srcy[m_scanline].i)) return false;
    char *pixel[4];
    pix_pos i;
    for (i = 0; i < m_cols; i++) {
      pixel[0] = m_line[0] + m_srcx[i].i * m_bpp;
      pixel[1] = pixel[0]  + m_bpp;
      pixel[2] = m_line[1] + m_srcx[i].i * m_bpp;
      pixel[3] = pixel[2] + m_bpp;
      bilinear(buf, pixel, m_bpp, m_srcx[i].d, 
          m_srcy[m_scanline].d, m_radix);
      buf += m_bpp;
    }
    m_scanline ++;
    return true;
  }
  bool ready_cubic()
  {
    m_sentinel = 2;
    m_lines = 4;
    m_scanline = 0;
    m_sline = 0;
    calc();
    return true;
  }
  bool read_cubic(char *buf)
  {
    if (!get_line(m_srcy[m_scanline].i)) return false;
    char *pixel[16];
    pix_pos i;
    for ( i = 0; i < m_cols; i++) {
      pixel[0] = m_line[0] + m_srcx[i].i * m_bpp;
      pixel[1] = pixel[0] + m_bpp;
      pixel[2] = pixel[1] + m_bpp;
      pixel[3] = pixel[2] + m_bpp;

      pixel[4] = m_line[1] + m_srcx[i].i * m_bpp;
      pixel[5] = pixel[4] + m_bpp;
      pixel[6] = pixel[5] + m_bpp;
      pixel[7] = pixel[6] + m_bpp;

      pixel[8] = m_line[2] + m_srcx[i].i * m_bpp;
      pixel[9] = pixel[8] + m_bpp;
      pixel[10]= pixel[9] + m_bpp;
      pixel[11]= pixel[10] + m_bpp;

      pixel[12] = m_line[3] + m_srcx[i].i * m_bpp;
      pixel[13] = pixel[12] + m_bpp;
      pixel[14] = pixel[13] + m_bpp;
      pixel[15] = pixel[14] + m_bpp;

      bicubic(buf, pixel, m_bpp,
          m_srcx[i].d, m_srcy[m_scanline].d, m_radix, -1);
      buf += m_bpp;
    }
    m_scanline++;
    return true;
  }
};

PIXIMPL_END
PIXAPI_CPP_END

