/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  filter_resize.cpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 */

#include <pix/filter_resize.hpp>
#include <pix/utils.hpp>
#include <impl/filter_resize.hpp>

PIXAPI_CPP_BEG
class _Resize {
public:
  impl::Resize impl;
public:
  _Resize(pix_pol pol) : impl(pol) { }
};

ResizeFilter::ResizeFilter(pix_pol pol)
{
  m_resize = new _Resize(pol);
}
ResizeFilter::~ResizeFilter()
{
  if (m_resize) delete m_resize;
}
bool ResizeFilter::open(ImageReaderBase & img)
{
  if( !m_resize->impl.open(img) ) return false;
  param(m_resize->impl.param());
  return true;
}

bool ResizeFilter::ready()
{
  if (m_resize->impl.ready()) {
    param(m_resize->impl.param());
    lock(true);
    return true;
  }
  return false;
}

pix_pos ResizeFilter::read(char *buf, pix_pos rows)
{
  return m_resize->impl.read(buf, rows);
}

bool ResizeFilter::zoom(pix_pos c, pix_pos r)
{
  if (!m_resize->impl.zoom(c, r)) return false;
  param(m_resize->impl.param());
  return true;
}
double ResizeFilter::hres() const
{
  return param().hres;
}

double ResizeFilter::vres() const
{
  return param().vres;
}
pix_unit ResizeFilter::unit() const
{
  return param().unit;

}
bool ResizeFilter::hres(double res)
{
  return false;

}

bool ResizeFilter::vres(double res)
{
  return false;

}
bool ResizeFilter::unit(pix_unit u)
{
  return false;
}


PIXAPI_CPP_END


