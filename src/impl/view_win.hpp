/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  view.static.hpp
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
#include <pix/image_bmp.hpp>
#include <pix/system.hpp>
/*
 * TODO: write
 */

PIXAPI_CPP_BEG

PIXIMPL_BEG
inline void set_default(BITMAPINFO &bmi)
{
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = 0;
  bmi.bmiHeader.biHeight = 0;
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 24;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biSizeImage = 0;
  bmi.bmiHeader.biXPelsPerMeter = 72;
  bmi.bmiHeader.biYPelsPerMeter = 72;
  bmi.bmiHeader.biClrUsed = 0;
  bmi.bmiHeader.biClrImportant = 0; 
}
class View {
public:
	View()
  {
    m_mdc = ::CreateCompatibleDC(0);
    m_cols = 0;
    m_rows = 0;
    m_bmp = 0;
    m_obmp = 0;
    m_ocols = 0;
    m_orows = 0;
  }
	~View()
  {
    if (m_mdc == m_omdc) m_mdc = 0;
    if (m_bmp == m_obmp) m_bmp = 0;
    if (m_mdc) ::DeleteDC(m_mdc);
    if (m_omdc) ::DeleteDC(m_omdc);
    if (m_bmp) ::DeleteObject(m_bmp);
    if (m_obmp) ::DeleteObject(m_obmp);
  }


  bool write_image(ImageReaderBase &img)
  {
    if (m_bmp == m_obmp) m_obmp = 0;
    if (m_bmp) ::DeleteObject(m_bmp);
    if (m_obmp) ::DeleteObject(m_obmp);
    m_obmp = 0;
    m_bmp = 0;
    pix_pos cols = img.cols();
    pix_pos rows = img.rows();
    if (cols <= 0) return false;
    if (rows <= 0) return false;
    HBITMAP bmp = ::CreateCompatibleBitmap(GetDC(0), cols, rows);
    ::SelectObject(m_mdc, bmp);
    if (m_bmp == m_obmp) m_obmp = 0;
    if (m_bmp) ::DeleteObject(m_bmp);
    if (m_obmp) ::DeleteObject(m_obmp);
    m_bmp = bmp;
    m_cols = cols;
    m_rows = rows;

    m_omdc = m_mdc;
    m_obmp = bmp;
    m_ocols = cols;
    m_orows = rows;


    BITMAPINFO bmi;
    set_default(bmi);
    bmi.bmiHeader.biWidth = cols;
    bmi.bmiHeader.biHeight = 1;

    int bpp = img.bpp();
    pix_len rsize = img.row_size();
    pix_len stride = bmp_stride(rsize);
    Block<char> buf(stride);
    char *p = buf;
    pix_pos i,j;

    for(i=0; i < rows; i++)
    {
      img.read(p, 1);
      for ( j = 0 ; j < cols ; j++) {
        swap(p[0], p[2]);
        p += bpp;
      }
      p = buf;
      DWORD ret = ::SetDIBitsToDevice(m_mdc, 0, i, cols, 1, 
        0, 0, 0, 1, p, &bmi, DIB_RGB_COLORS);
    }
    return true;
  }
  
  pix_len write(char *buf, pix_len len)
  {
    return 0;
  }

  bool resize(int cols, int rows)
  {
    if (m_ocols == 0 || m_orows == 0) return false;
    if (cols == 0 && rows == 0) return true;
    if (cols == m_cols && rows == m_rows) return true;
    if (cols == 0) {
      double r = m_ocols;
      r /= m_orows;
      round(cols, rows * r);
    } else if (rows == 0) {
      double r = m_orows;
      r /= m_ocols;
      round(rows, cols * r);
    }
    if (m_omdc == m_mdc) {
      m_mdc = ::CreateCompatibleDC(0);
      m_bmp = 0;
    }
    if (m_bmp) ::DeleteObject(m_bmp);

    m_cols = cols;
    m_rows = rows;
    m_bmp = CreateCompatibleBitmap(GetDC(0), cols, rows);
    ::SelectObject(m_mdc, m_bmp);

		::SetStretchBltMode(m_mdc, HALFTONE);
		::StretchBlt(m_mdc, 0, 0, cols , rows ,
			m_omdc, 0, 0, m_ocols , m_orows, SRCCOPY);
		return true;
  }

  bool bitblt( HDC hdc,
      int xdst = 0, int ydst = 0, 
      int cols = 0, int rows = 0, 
      int xsrc = 0, int ysrc = 0)
  {
    /*
    if (cols<= 0 || cols > m_cols) cols = m_cols;
    if (rows<= 0 || rows > m_rows) rows = m_rows;
    */
    
    if ( !::BitBlt(hdc, xdst, ydst, cols, rows, m_mdc, xsrc, ysrc, SRCCOPY)) {
      int err = GetLastError();
    }
    return true;
  }


private:
	HDC m_mdc;
	HBITMAP m_bmp;
  pix_pos m_cols;
  pix_pos m_rows;

  HDC m_omdc;
	HBITMAP m_obmp;
  pix_pos m_ocols;
  pix_pos m_orows;
};

PIXIMPL_END

PIXAPI_CPP_END
