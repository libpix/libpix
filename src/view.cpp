/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  view.cpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 */

/*
 * @file
 * @brief
 */


#include <pix/view.hpp>
#include <pix/system.hpp>
#include <inner.hpp>
#ifdef PIX_WIN
#include <impl/view_win.hpp>
#endif

PIXAPI_CPP_BEG
class _View {
public:
  impl::View impl;
};

View::View()
{
  m_view = new _View;
}
View::~View()
{
  if (m_view) delete m_view;
}
void View::resize(pix_pos width, pix_pos height)
{
  m_view->impl.resize(width, height);


}
pix_pos View::write(const char *buf, pix_pos rows)
{
  return 0;

}
bool View::write_image(ImageReaderBase &img)
{
  return m_view->impl.write_image(img);
}
_View * _Inner::get_view(View &view)
{
  return view.m_view;
}

PIXAPI_CPP_END

#ifdef PIX_WIN
PIXAPI_CPP_BEG
void bitblt(View &view, HDC hdc,
    int xdst, int ydst,
    int width, int height,
    int xsrc, int ysrc)
{
  _View *v = _Inner::get_view(view);
  v->impl.bitblt(hdc, xdst, ydst, width, height, xsrc, ysrc);
}

PIXAPI_CPP_END
#endif
