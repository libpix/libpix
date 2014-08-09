/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  system.cpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 */
#include <pix/system.hpp>
#include <inner.hpp>

#ifdef PIX_WIN
/* ############################## Windows ####################### */
/* According to MSDN, hash_map is obsolote, use unordered_map */
#include <process.h>
#include <unordered_map>

PIXAPI_CPP_BEG

/* ---------------------- platform independent ---------------- */
int sys_page()
{
  return Win::page_size();
}

int sys_gran()
{
  return Win::granularity();
}
pix_enc sys_encode()
{
  return (pix_enc) Win::encode();
}
PIXWIN_BEG
/* ------------------------------------------------------------------ */
const SYSTEM_INFO& system_info()
{
  static SYSTEM_INFO sinfo;
  static bool init = false;
  if (init) return sinfo;
  ::GetSystemInfo(&sinfo);
  init = true;
  return sinfo;
}


LRESULT CALLBACK Window::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  typedef std::unordered_map<HWND, Window*> WndMap;
  static WndMap wndmap;

  switch (msg)
  {
    case WM_CREATE:
      {
        LPCREATESTRUCT info = (LPCREATESTRUCT)lparam;
        Window *wnd = (Window*)info->lpCreateParams;
        return 0;
      }
    case WM_GETICON:
      {
        Window *wnd = wndmap[hwnd];
        wnd->setmsg(msg, wparam,lparam);
        return (LRESULT)wnd->wm_geticon(wparam);
      }
    case WM_DROPFILES:
      {
        Window *wnd = wndmap[hwnd];
        wnd->setmsg(msg, wparam,lparam);
        return wnd->wm_dropfiles((HDROP)wparam);
      }
    case WM_MOVE:
      {
        Window *wnd = wndmap[hwnd];
        wnd->setmsg(msg, wparam, lparam);
        int x = LOWORD(lparam);
        int y = HIWORD(lparam);
        return wnd->wm_move(x, y);
      }
    case WM_MOVING :
      {
        Window *wnd = wndmap[hwnd];
        wnd->setmsg(msg, wparam, lparam);
        RECT *rect = (RECT*)lparam;
        return wnd->wm_moving(rect);
      }
    case WM_SIZE:
      {
        Window *wnd = wndmap[hwnd];
        wnd->setmsg(msg, wparam, lparam);
        int width = LOWORD(lparam);
        int height = HIWORD(lparam);
        return wnd->wm_size(wparam, width, height);
        //InvalidateRect(hwnd , 0 , TRUE);
        //return 0;
      }
    case WM_SIZING:
      {
        Window *wnd = wndmap[hwnd];
        wnd->setmsg(msg, wparam, lparam);
        RECT * rect = (RECT*) lparam;
        return wnd->wm_sizing(wparam, rect);
        //InvalidateRect(hwnd , 0 , TRUE);
        //return 0;
      }

    case WM_ENTERSIZEMOVE :
      {
        Window *wnd = wndmap[hwnd];
        wnd->setmsg(msg, wparam, lparam);
        return wnd->wm_entersizemove();
      }
    case WM_EXITSIZEMOVE :
      {
        Window *wnd = wndmap[hwnd];
        wnd->setmsg(msg, wparam, lparam);
        return wnd->wm_exitsizemove();
      }

    case WM_WINDOWPOSCHANGING:
      {
        Window *wnd = wndmap[hwnd];
        wnd->setmsg(msg, wparam, lparam);
        WINDOWPOS * pos = (WINDOWPOS*) lparam;
        return wnd->wm_windowposchanging(pos);
      }
    case WM_WINDOWPOSCHANGED:
      {
        Window *wnd = wndmap[hwnd];
        wnd->setmsg(msg, wparam, lparam);
        WINDOWPOS * pos = (WINDOWPOS*) lparam;
        return wnd->wm_windowposchanged(pos);
      }

    case WM_PAINT:
      {
        Window *wnd = wndmap[hwnd];
        wnd->setmsg(msg, wparam, lparam);
        return wnd->wm_paint();
      }
    case WM_CLOSE :
      {
        Window *wnd = wndmap[hwnd];
        wnd->setmsg(msg, wparam, lparam);
        return wnd->wm_close();
      }
    case WM_ENABLE :
      {
        Window *wnd = wndmap[hwnd];
        wnd->setmsg(msg, wparam, lparam);
        return wnd->wm_enable(!!wparam);
      }

    case WM_DESTROY:
      {
        Window *wnd = wndmap[hwnd];
        wnd->setmsg(msg, wparam, lparam);
        return wnd->wm_destroy();
      }
    /* ------------------------------------------------------------ */
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MBUTTONDBLCLK:
    case WM_MOUSEMOVE:
    case WM_MOUSEWHEEL:
    case WM_MOUSEHOVER:
    case WM_MOUSEHWHEEL:
    case WM_MOUSELEAVE:
    case WM_MOUSEACTIVATE:
      {
        Window *wnd = wndmap[hwnd];
        wnd->setmsg(msg, wparam, lparam);
        int x = LOWORD(lparam);
        int y = HIWORD(lparam);
        return wnd->wm_mouse(msg, wparam, x, y);
      }

    /* ------------------------------------------------------------ */
    case WM_PIX_CREATE:
      {
        Window *wnd = (Window*) wparam;
        wndmap.insert(WndMap::value_type(hwnd, wnd));
        wnd->m_admin = true;
        return 0;
      }
    case WM_PIX_DELETE:
      {
        Window *wnd = wndmap[hwnd];
        wnd->m_admin = false;
        wndmap.erase(hwnd);
        return 0;
      }
    /* ------------------------------------------------------------ */
    case WM_COMMAND:
      {
        Window *wnd = wndmap[hwnd];
        wnd->setmsg(msg, wparam, lparam);
        return wnd->wm_command(HIWORD(wparam), LOWORD(wparam) );
      }


  }
  return DefWindowProc(hwnd, msg, wparam, lparam);
}

INT_PTR CALLBACK Dialog::dlgproc(HWND hdlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
  typedef std::unordered_map<HWND, Dialog*> DlgMap;
  static DlgMap dlgmap;

  switch (msg) {
    case WM_INITDIALOG:
      {
        Dialog *dlg = (Dialog*) lparam;
        dlgmap.insert(DlgMap::value_type(hdlg, dlg));
        dlg->m_hwnd = hdlg;
        return dlg->wm_init();
      }
    case WM_COMMAND:
      {
        Dialog *dlg = dlgmap[hdlg];
        return dlg->wm_command(HIWORD(wparam), LOWORD(wparam) );
      }
    case WM_TIMER:
      {
        Dialog *dlg = dlgmap[hdlg];
        return dlg->wm_timer((int)wparam);

      }
    case WM_PIX_DELETE:
      {
        Dialog *dlg = dlgmap[hdlg];
        dlgmap.erase(hdlg);
        return TRUE;
      }
  }
  return FALSE;
}

Thread::Thread()
{
  unsigned tid;
  m_hth = (HANDLE)::_beginthreadex( NULL, 0,
      thproc, (void*) this, CREATE_SUSPENDED, &tid);

}
Thread::~Thread()
{
  ::TerminateThread(m_hth, 0);
  ::CloseHandle(m_hth);
}
unsigned __stdcall Thread::thproc(void *param)
{
  Thread *th = (Thread*)param;
  unsigned ret = th->run();
  _endthreadex(ret);
  return ret;
}
PIXWIN_END

PIXAPI_CPP_END
#endif

#ifdef PIX_POSIX
/* ############################ POSIX ###################### */
#include <unistd.h>
PIXAPI_CPP_BEG

int sys_page()
{
  return sysconf(_SC_PAGE_SIZE);
}

int sys_gran()
{
  return sys_page();
}
pix_enc sys_encode()
{
  return PIX_ENC_UTF8;
}
PIXAPI_CPP_END

#endif
