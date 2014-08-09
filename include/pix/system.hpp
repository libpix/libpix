/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  system.hpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *        License:  LGPL
 *
 *    Description:  plaform dependencies
 *
 * =============================================================
 */
/**
 * @file
 * @brief %platform dependencies
 * 
 */
#ifndef __PIX_SYSTEM_HPP__
#define __PIX_SYSTEM_HPP__

#include <pix/config.h>
#include <pix/string.hpp>
/* ###################### Platform Independent ################### */

PIXAPI_CPP_BEG
/**
 * @defgroup system System
 * @brief %unified system interface for different platform 
 * @{
 * 
 */

/* page size */
int sys_page();
/* memory alloc granularity */
int sys_gran();

pix_enc sys_encode();

/**
 * @}
 * 
 */

PIXAPI_CPP_END



#if defined(PIX_WIN) || defined(PIX_DOC)
/* ========================= Windows ======================== */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include <tchar.h>
#define PIXWIN_BEG namespace Win {
#define PIXWIN_END }
PIXAPI_CPP_BEG

PIXWIN_BEG
/**
 * @defgroup win System/Windows
 * @brief %useful function and classes for windows
 * @{
 */
/* static functions */
const SYSTEM_INFO & system_info();

/* inlines and templates */
template<class T>
inline DWORD high(T num)
{
  DWORD ret = 0;
  if (sizeof(T) > sizeof(DWORD)) {
    /* get rid of VC warning C4293, shift twice */
    T n = num;
    n >>= 16;
    n >>= 16;
    ret = n;
  }
  return ret;
}
template<class T>
inline DWORD low(T num)
{
  return num & 0xFFFFFFFF;
}

template<class T>
inline bool highlow(T & num, DWORD high, DWORD low)
{
  /* return false if overflow */
  if (sizeof(T) > sizeof(DWORD) ) {
    num = high;
    num <<= 32;
    num += low;
    return true;
  } else num = low;
  if (high > 0) return false;
  return true;
}


inline int page_size()
{
  return system_info().dwPageSize;
}

inline int granularity()
{
  return system_info().dwAllocationGranularity;
}


inline HANDLE create_file(const String &path, int flag)
{ 
  DWORD access = 0;
  DWORD opt = OPEN_EXISTING;

  if (flag & PIX_STM_READ) access |= GENERIC_READ;
  if (flag & PIX_STM_WRITE) {
    access |= GENERIC_READ;
    access |= GENERIC_WRITE;
    if (flag & PIX_STM_TRUNC) opt = TRUNCATE_EXISTING;
  }

  HANDLE hFile;
  String wstr;
  const WCHAR *wpath = path;
  if (!wpath) {
    wpath = path.wchars(wstr);
    if (!wpath) return 0;
  }
  hFile = ::CreateFileW(wpath, access, FILE_SHARE_READ, NULL, opt, FILE_ATTRIBUTE_NORMAL, NULL );

  if (hFile == INVALID_HANDLE_VALUE) {
    if (flag & PIX_STM_WRITE) {
      if (flag & PIX_STM_CREATE) 
        hFile = ::CreateFileW(wpath, access, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL );
    }
  }
  return hFile;
  // return INVALID_HANDLE_VALUE if failed
}

inline pix_len file_size(HANDLE hFile)
{
  DWORD high = 0;
  DWORD low = ::GetFileSize(hFile, &high);
  pix_len size;

  if (!highlow(size, high, low)) {
    pix_log_warn("file_size : overflow! return lower part.");
  }

  return size;
}

inline bool file_end(HANDLE hFile)
{
  return !!SetEndOfFile(hFile);
}

inline HANDLE create_file_map(HANDLE hFile, int flag, pix_len size)
{
  DWORD access=0;
  if (flag & PIX_STM_WRITE) access = PAGE_READWRITE;
  else access = PAGE_READONLY;
  DWORD h = high(size);
  DWORD l = low(size);

  HANDLE hFileMap;
  hFileMap = ::CreateFileMappingW(hFile, NULL, access, h, l, NULL);
  return hFileMap;
  // return 0 if failed
}

inline char * file_map_view(HANDLE hFileMap, int flag, pix_pos off, int len)
{
  DWORD access=0;
  if (flag & PIX_STM_WRITE) access = FILE_MAP_WRITE;
  else access = FILE_MAP_READ;
  DWORD h = high(off);
  DWORD l = low(off);

  return (char *)::MapViewOfFile(hFileMap, access, h, l, len);

}

inline bool file_unmap_view(char *p)
{
  if (p) return !!::UnmapViewOfFile((PVOID)p);
  return true;
}
inline bool file_flush_view(char *p, pix_len len = 0)
{
  if (p) return !!::FlushViewOfFile(p, (SIZE_T)len);
  return false;

}


inline pix_pos file_seek(HANDLE hFile, pix_pos pos, int whence)
{

  DWORD h = high(pos);
  DWORD l = low(pos);

  l = SetFilePointer(hFile, l, (PLONG) &h, whence);
  if (l == INVALID_SET_FILE_POINTER) return -1;
  if (!highlow(pos, h, l)) {
    pix_log_warn("file_seek : overflow! return the lower part");
  }
  return pos;
}

inline bool close(HANDLE handle)
{
  return !!::CloseHandle(handle);
}

inline HANDLE console()
{
  return ::GetConsoleWindow();
}
inline HMODULE module()
{
  return ::GetModuleHandle(NULL);
}
inline void msgloop()
{
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}
inline void msgbox(const String & content, const String &title=L"message", int style = MB_OK, HWND hwnd = NULL)
{
  String wc;
  String wt;
  const WCHAR *cc = content;
  const WCHAR *tt = title;
  if (!cc) {
    cc = content.wchars(wc);
  }
  if (!tt) {
    tt = title.wchars(wt);
  }
  ::MessageBoxW(hwnd, cc, tt, style);
}
inline int acp()
{
  return ::GetACP();
}

inline int encode(int num = 0, bool reverse = false)
{
  typedef struct {
    int acp;
    pix_enc enc;
  } map_t;
  const map_t map[] = {
    /* Simplified Chinese  */
    { 936,   PIX_ENC_GBK },
    { 51936, PIX_ENC_EUCCN },
    { 54936, PIX_ENC_GB18030 },
    /* Traditional Chinese */
    { 950,   PIX_ENC_BIG5 },
    { 51950, PIX_ENC_EUCTW },
    /* Latin */
    { 1250,  PIX_ENC_LATIN2 },
    { 1251,  PIX_ENC_CYRILLIC },
    { 1252,  PIX_ENC_LATIN1 },
    { 1253,  PIX_ENC_GREEK },
    { 1254,  PIX_ENC_LATIN5 },
    { 1255,  PIX_ENC_HEBREW },
    { 1256,  PIX_ENC_ARABIC },
    { 1257,  PIX_ENC_LATIN7 },
    { 28605, PIX_ENC_LATIN9 },
    /* Japanese */
    { 932,   PIX_ENC_SJIS },
    /* Korean */
    { 949,   PIX_ENC_KOREAN },
    { 51949, PIX_ENC_EUCKR },
    /* Vietnam */
    { 1258,  PIX_ENC_VIETNAM },
    /* Unicode */
    { 65000, PIX_ENC_UTF7 },
    { 65001, PIX_ENC_UTF8 },
    { 1200,  PIX_ENC_UTF16LE },
    { 1201,  PIX_ENC_UTF16BE },
    { 12000, PIX_ENC_UTF32LE },
    { 12001, PIX_ENC_UTF32BE },
  };
  const int len = sizeof(map) / sizeof(map_t);

  if (num == 0) num = acp();
  int i;
  if (reverse) {
    for (i = 0; i < len; i++)
      if (map[i].enc == num) return map[i].acp;
    return 0;
  }
  for (i = 0; i < len; i++)
    if (map[i].acp == num) return map[i].enc;
  return PIX_ENC;
}

inline void quit(int exitcode)
{
  ::PostQuitMessage(exitcode);
}

enum {
  WM_PIX = WM_APP + 0xB000,
  WM_PIX_CREATE = WM_PIX + 1,
  WM_PIX_DELETE = WM_PIX + 2,
}; 
class Base {

};
class Paint : public Base {
public:
  Paint(HWND hwnd) : m_hwnd(hwnd)
  {
    m_dc = BeginPaint(m_hwnd, &m_ps);

  }
  ~Paint()
  {
    EndPaint(m_hwnd, &m_ps);
  }
  HDC hdc() const
  {
    return m_dc;
  }
  HWND hwnd() const
  {
    return m_hwnd;
  }
  PAINTSTRUCT ps() const
  {
    return m_ps;
  }
  LONG left() const
  {
    return m_ps.rcPaint.left;
  }
  LONG top() const
  {
    return m_ps.rcPaint.top;
  }
  LONG right() const
  {
    return m_ps.rcPaint.right;
  }
  LONG bottom() const
  {
    return m_ps.rcPaint.bottom;
  }
  LONG cols() const
  {
    return right() - left();
  }
  LONG rows() const
  {
    return bottom() - top();
  }
  bool erase() const
  {
    return !!m_ps.fErase;
  }
  bool text(int x, int y, const WCHAR *buf, int len)
  {
    return !!TextOutW(m_dc, x, y, buf, len);
  }
  bool text(int x, int y, const String &str)
  {
    String wstr;
    const WCHAR *p = str;
    pix_len len = (int) str.strlen();
    if (!p) {
      p = str.wchars(wstr);
      len = wstr.strlen();
    }
    return text(x, y, p, (int) len);
  }


private:
  HWND m_hwnd;
  HDC m_dc;
  PAINTSTRUCT m_ps;


};

class Window : public Base {
public:
  Window(HWND hwnd) : 
    m_hwnd(hwnd), m_name(0), m_admin(false), m_inst(0), m_own(false)
  {
  }
  Window(HINSTANCE hinst = 0):
    m_str(L"PixWnd"), m_hwnd(0), m_admin(false), m_own(true), m_inst(hinst), m_name(0)
  {
    WNDCLASSEXW cls;
    m_name = m_str;
    if (!m_inst) m_inst = module();
    if (!GetClassInfoExW(m_inst, m_name, &cls)) {
      cls.cbSize = sizeof(cls);
      cls.style = CS_HREDRAW | CS_VREDRAW;
      cls.lpfnWndProc = wndproc;
      cls.cbClsExtra = 0;
      cls.cbWndExtra = 0;
      cls.hInstance = m_inst;
      cls.hIcon = LoadIcon(NULL, IDI_APPLICATION);
      cls.hCursor = LoadCursor(NULL, IDC_ARROW);
      cls.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
      cls.lpszMenuName = NULL;
      cls.lpszClassName = m_name;
      cls.hIconSm = NULL;
      if (!RegisterClassExW(&cls)) {
        pix_log_error("Win::Wnd : can't register class", PIX_ERR_WIN);
      }
    }
    DWORD exstyle = 0;
    m_hwnd = ::CreateWindowExW(
        exstyle,
        m_name, L"Test",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, m_inst, NULL );
    send(WM_PIX_CREATE, (WPARAM)this, 0);
  }
  virtual ~Window() 
  {
    if (m_admin) {
      send(WM_PIX_DELETE, 0 , 0);
    }
    if (m_own) {
      if (m_hwnd) ::DestroyWindow(m_hwnd);
    }
  }
public:
  bool admin() const
  {
    return m_admin;
  }
  HWND hwnd() const
  {
    return m_hwnd;
  }

  void show(int cmd=1)
  {
    if (!m_hwnd) {
      pix_log_error("Pix::show : no hwnd", PIX_ERR_WIN);
    }
    ::ShowWindow(m_hwnd, cmd);
    ::UpdateWindow(m_hwnd);
  }
  void drag(bool yn)
  {
    if (!hwnd()) return;
    ::DragAcceptFiles(m_hwnd, (BOOL)yn);
  }
  void text(const String &str)
  {
    if (!hwnd()) return;
    String wstr;
    const WCHAR *w = str;
    if (!w) {
      str.wchars(wstr);
      w = wstr;
      if (!w) return;
    }
    BOOL ret = ::SetWindowTextW(hwnd(), w);
  }
  String text()
  {
    if (!hwnd()) return String(L"");
    int len = ::GetWindowTextLengthW(hwnd());
    Block<WCHAR> buf(len + 1);
    ::GetWindowTextW(hwnd(), buf, len + 1);
    buf[-1] = 0;

    return String ((WCHAR*)buf, len);
  }
  RECT rect()
  {
    RECT r = { 0, 0, 0, 0};
    if (!hwnd()) return r;
    ::GetWindowRect(hwnd(), &r);
    return r;
  }
  void client(int &width, int &height)
  {
    RECT r = { 0, 0, 0, 0};
    width = 0; 
    height = 0;
    if (!hwnd()) return;
    ::GetClientRect(hwnd(), &r);
    width = r.right;
    height = r.bottom;
  }

  bool move(int x, int y, int width, int height, bool repaint = false)
  {
    return !!::MoveWindow(hwnd(), x, y, width, height, (BOOL)repaint);
  }
  bool move_client(int width, int height)
  {
    RECT wrect, crect;
    ::GetWindowRect(hwnd(), &wrect);
    ::GetClientRect(hwnd(), &crect);
    int w = wrect.right - wrect.left - crect.right;
    int h = wrect.bottom - wrect.top - crect.bottom;
    width += w;
    height += h;
    return move(wrect.left, wrect.top, width, height, true);
  }

  HICON icon(int type = ICON_BIG)
  {
    return (HICON)send(WM_GETICON, (WPARAM)type, 0);
  }
  void icon(HICON icon, int type = ICON_BIG)
  {
    send(WM_SETICON, (WPARAM)type, (LPARAM)icon);
  }
  void menu(HMENU m)
  {
    if (!hwnd()) return;
    ::SetMenu(hwnd(), m);
  }

  void msgbox(const String & content, const String &title=L"message", int style = MB_OK)
  {
    String wc;
    String wt;
    const WCHAR *cc = content;
    const WCHAR *tt = title;
    if (!cc) {
      cc = content.wchars(wc);
    }
    if (!tt) {
      tt = title.wchars(wt);
    }
    ::MessageBoxW(m_hwnd, cc, tt, style);
  }

  LRESULT send(UINT msg, WPARAM wparam, LPARAM lparam)
  {
    if (!hwnd()) {
      pix_log_error("Wnd::send : no hwnd", PIX_ERR_WIN);
      return 0;
    }
    return ::SendMessageW(hwnd(), msg, wparam, lparam);
  }
  void redraw(RECT *rect = 0, bool erase = true)
  {
    if (!hwnd()) return;
    ::InvalidateRect(hwnd(), rect, (BOOL)erase);

  }
  WNDPROC proc()
  {
    if (!hwnd()) {
      pix_log_error("Wnd::proc : no hwnd", PIX_ERR_WIN);
      return 0;

    }
    WNDPROC fn = (WNDPROC) ::GetWindowLong(m_hwnd, GWL_WNDPROC);
    if (fn == wndproc) return 0;
    return fn;
  }
  bool proc(WNDPROC fn)
  {
    if (!hwnd()) {
      pix_log_error("Wnd::proc : no hwnd", PIX_ERR_WIN);
      return false;
    }
    if (fn == 0) {
      /* Use  */
      if (::SetWindowLongW(hwnd(), GWL_WNDPROC, (LONG)wndproc)) return false;
      send(WM_PIX_CREATE, (WPARAM)this, 0);
      return true;
    }
    if (::SetWindowLongW(m_hwnd, GWL_WNDPROC, (LONG)fn)) return true;
    return false;
  }
protected:
  UINT msg() const
  {
    return m_msg;
  }
  WPARAM wparam() const
  {
    return m_wparam;
  }
  LPARAM lparam() const
  {
    return m_lparam;
  }

  HRESULT wm_default()
  {
    return DefWindowProc(hwnd(), msg(), wparam(), lparam());
  }

  /* return 0 if process */
  virtual int wm_destroy()
  {
    quit(0);
    return 0;
  }
  virtual void wm_quit(int exitcode)
  {
    return;
  }
  /* return 0 if process */

  virtual LRESULT wm_close()
  {
    return wm_default();
  }
  /* return 0 if process */
  virtual LRESULT wm_enable(bool enable)
  {
    return wm_default();
  }
  virtual HICON wm_geticon(int type)
  {
    return (HICON) wm_default();
  }
  virtual LRESULT wm_sizing(int edge, RECT *rect)
  {
    return wm_default();
  }
  virtual int wm_entersizemove()
  {
    return wm_default();
  }
  virtual int wm_exitsizemove()
  {
    return wm_default();

  }

  /* return 0 if process */
  virtual int wm_paint()
  {
    Paint paint(hwnd());
    return 0;
  }
  /* return 0 if process */
  virtual int wm_size(int type, int width, int hight)
  {
    /* type:
     * SIZE_MAXHIDE
     * SIZE_MAXIMIZED
     * SIZE_MAXSHOW
     * SIZE_MINIMIZED
     * SIZE_RESTORED
     * 
     */
    return wm_default();
  }
  virtual int wm_move(int x, int y)
  {
    return wm_default();
  }
  virtual int wm_windowposchanging(WINDOWPOS *pos)
  {
    return wm_default();
  }
  virtual int wm_windowposchanged(WINDOWPOS *pos)
  {
    return wm_default();
  }
  virtual int wm_moving(RECT *rect)
  {
    return wm_default();
  }
  virtual int wm_dropfiles(HDROP drop)
  {
    return wm_default();
  }
  virtual int wm_command(int type, int id)
  {
    return wm_default();
  }

  /* ---------------------------------------------------- */
  virtual int wm_mouse(int action, int key, int x, int y )
  {
    /*action 
      WM_LBUTTONDOWN
      WM_LBUTTONUP
      WM_LBUTTONDBCLK
      WM_RBUTTONDOWN
      WM_RBUTTONUP
      WM_RBUTTONDBCLK
      WM_MBUTTONDOWN
      WM_MBUTTONUP
      WM_MBUTTONDBCLK
      WM_MOUSEMOVE
      WM_MOUSEWHEEL
      WM_MOUSEHOVER
      WM_MOUSEHWHEEL
      WM_MOUSELEAVE
      WM_MOUSEACTIVATE
     */
    /* key
     * MK_CONTROL
     * MK_LBUTTON
     * MK_MBUTTON
     * MK_RBUTTON
     * MK_SHIFT
     * MK_XBUTTON1
     * MK_XBUTTON2
     */
    return wm_default();
  }

private:
  static LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
private:
  void setmsg(UINT msg, WPARAM wparam, LPARAM lparam)
  {
    m_msg = msg;
    m_wparam = wparam;
    m_lparam = lparam;
  }

private:
  String m_str;
  const WCHAR* m_name;
  bool m_admin;
  bool m_own;
  HWND m_hwnd;
  HINSTANCE m_inst;
  UINT m_msg;
  WPARAM m_wparam;
  LPARAM m_lparam;
};
class Control : public Base {
public:
  Control(int id, HWND hdlg)
  {
    m_hwnd = ::GetDlgItem(hdlg, id);
    m_parent = hdlg;
  }
  Control(HWND hwnd, HWND parent)
  {
    m_hwnd = hwnd;
    m_parent = parent;
  }
  HWND hwnd() { return m_hwnd; }
  HWND parent() { return m_parent; }

  void text(const String &str)
  {
    if (!hwnd()) return;
    String wstr;
    const WCHAR *w = str;
    if (!w) {
      str.wchars(wstr);
      w = wstr;
      if (!w) return;
    }
    BOOL ret = ::SetWindowTextW(hwnd(), w);
  }

  String text()
  {
    if (!hwnd()) return String(L"");
    int len = ::GetWindowTextLengthW(hwnd());
    Block<WCHAR> buf(len + 1);
    ::GetWindowTextW(hwnd(), buf, len + 1);
    buf[-1] = 0;

    return String ((WCHAR*)buf, len);
  }

private:
  HWND m_hwnd;
  HWND m_parent;
};

class EditBox : public Control {
public:
  EditBox(int id, HWND hdlg) : Control(id, hdlg) { }
  EditBox(HWND hwnd, HWND parent) : Control(hwnd, parent) { }

};

class ListBox : public Control {
public:
  ListBox(int id, HWND hdlg) : Control(id, hdlg) { }
  ListBox(HWND hwnd, HWND parent) : Control(hwnd, parent) { }
  bool add_item(const String &str)
  {
    String wstr;
    const WCHAR *ws = str;
    if (!ws) {
      ws = str.wchars(wstr);
      if (!ws) return false;
    }
    ::SendMessageW(hwnd(), LB_ADDSTRING, 0, (LPARAM)ws);
    return true;

  }
  void select(int s)
  {
    ::SendMessageW(hwnd(), LB_SETCURSEL, (WPARAM)s, 0);
  }
private:

};
class ComboBox : public Control {
public:
  ComboBox(int id, HWND hdlg) : Control(id, hdlg) { }
  ComboBox(HWND hwnd, HWND parent) : Control(hwnd, parent) { }
  bool add_item(const String &str)
  {
    String wstr;
    const WCHAR *ws = str;
    if (!ws) {
      ws = str.wchars(wstr);
      if (!ws) return false;
    }
    ::SendMessageW(hwnd(), CB_ADDSTRING, 0, (LPARAM)ws);
    return true;

  }
  bool visible(int num)
  {
    return !!::SendMessageW(hwnd(), CB_SETMINVISIBLE, num, 0);
  }
  void select(int s)
  {
    ::SendMessageW(hwnd(), CB_SETCURSEL, (WPARAM)s, 0);
  }
  int select()
  {
    int sel = ::SendMessageW(hwnd(), CB_GETCURSEL, 0, 0);
    if (sel == CB_ERR) return -1;
    return sel;
  }
private:

};

class Dialog : public Base {
public:
  Dialog(int id, HWND parent = NULL, HINSTANCE inst = NULL)
  {
    m_id = MAKEINTRESOURCEW(id);
    m_inst = inst;
    m_parent = parent;
    m_hwnd = NULL;
  }
  ~Dialog()
  {
    close();
  }

  int modal()
  {
    return (int)::DialogBoxParamW(m_inst, m_id, m_parent, dlgproc, (LPARAM)this);
  }
  void close(int ret = 0)
  {
    ::SendMessageW(hwnd(), WM_PIX_DELETE, 0, 0);
    ::EndDialog(hwnd(), ret);
  }
  HWND hwnd()
  {
    return m_hwnd;

  }
  HINSTANCE inst()
  {
    return m_inst;
  }
  HWND parent()
  {
    return m_parent;
  }


protected:
  virtual bool wm_init()
  {
    return true;
  }
  virtual bool wm_command(int type, int id)
  {
    close(0);
    return true;
  }
  virtual bool wm_timer(int id)
  {
    return false;
  }


private:
  static INT_PTR CALLBACK dlgproc(HWND hdlg, UINT msg, WPARAM wparam, LPARAM lparam);
  LPCWSTR m_id;
  HINSTANCE m_inst;
  HWND m_parent;
  HWND m_hwnd;
};

class Thread : public Base {
public:
  Thread();
  ~Thread();
  bool suspend()
  {
    if ( (DWORD) -1 == ::SuspendThread(m_hth)) return false;
    return true;
  }
  bool resume()
  {
    if ( (DWORD) -1 == ::ResumeThread(m_hth)) return false;
    return true;
  }

protected:
  virtual unsigned run()
  {
    return 0;
  }



private:
  static unsigned __stdcall thproc(void *param);
  HANDLE m_hth;

};
/**
 * @}
 * 
 */

PIXWIN_END

PIXAPI_CPP_END

#endif /* PIX_WIN */

#ifdef PIX_POSIX
#include <unistd.h>

#endif

#endif /* __PIX_SYSTEM_HPP__ */

