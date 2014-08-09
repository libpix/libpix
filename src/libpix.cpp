#include <libpix.h>
#include <inner.hpp>
#include <stdio.h>
#include <string.h>
#include <errno.h>

PIXIN_BEG
static void lib_init(void)
{

}
static void lib_fini(void)
{
}

static int lib_main(int argc, char *argv[])
{
  return 0;
}
PIXIN_END

/*  --------------------  platform dependencies --------------------- */
#ifdef PIX_WIN

#endif

#ifdef PIX_POSIX
#include <unistd.h>

extern "C" __attribute__((constructor))
void _libpix_init(void)
{
  PIXIN::lib_init();
}

extern "C" __attribute__((destructor))
void _libpix_fini(void)
{
  PIXIN::lib_fini();
}

#endif


/*  ------------------------ sys & cfg ------------------------- */
PIXIN_BEG

struct Lib {

  const char *name;
  const char *ver;
  int endian;
  int word;
  int page;
  int gran;
  int fbuf;
  pix_uint umask;

  pix_enc enc;

  int log;
  int eid;
  pix_log_func logfn;

  Lib()
  {
    name = "";
    ver = "";
    union {
      short s;
      char c[2];
    } un;
    un.s = 1;
    endian = un.c[0];
    word = sizeof(void*) << 3;
    page = Pix::sys_page();
    gran = Pix::sys_gran();
    enc = Pix::sys_encode();
    fbuf = 1024 * 1024;
    umask = 0022;

    log = 2;
    eid = 0;
    logfn = 0;
  }
  int &errid()
  {
    return errno;
  }

};

static Lib & lib() {
  static Lib slib;
  return slib;
}
PIXIN_END


const char * 
PIXAPI pix_sys_name()
{
  return PIXIN::lib().name;
}
const char * 
PIXAPI pix_sys_ver()
{
  return PIXIN::lib().ver;
}
int 
PIXAPI pix_sys_endian()
{
  return PIXIN::lib().endian;
}
int 
PIXAPI pix_sys_word()
{
  return PIXIN::lib().word;
}

int 
PIXAPI pix_sys_page()
{
  return PIXIN::lib().page;
}


pix_enc 
PIXAPI pix_sys_enc()
{
  return PIXIN::lib().enc;
}

pix_enc 
PIXAPI pix_cfg_enc(pix_enc enc)
{
  return PIXIN::lib().enc = enc;
}


pix_len 
PIXAPI pix_sys_fbuf()
{
  return PIXIN::lib().fbuf;
}
int
PIXAPI pix_sys_gran()
{
  return PIXIN::lib().gran;
}

pix_uint
PIXAPI pix_sys_umask()
{
  return PIXIN::lib().umask;
}


void 
PIXAPI pix_sys_show()
{
  Pix::Info libpix_info;
  Pix::String str;
  str-pix_sys_name()-' '-pix_sys_ver();
  libpix_info.new_node("libpix 0.0.1");

  libpix_info.new_child("system");
  if (pix_sys_endian()) str = "LE";
  else str = "BE";
  libpix_info.set_data("endian", str);
  format(str, pix_sys_word());
  libpix_info.set_data("word", str-" bits");
  format(str, pix_sys_page());
  libpix_info.set_data("page", str-" bytes");
  format(str, pix_sys_gran());
  libpix_info.set_data("gran", str-" bytes");

  libpix_info.new_node("type");
  int ii;
  ii = sizeof(pix_pos) << 3;
  libpix_info.set_data("pos", format(str, ii)-" bits");
  ii = sizeof(pix_len) << 3;
  libpix_info.set_data("len", format(str, ii)-" bits");
  ii = sizeof(pix_wchar) << 3;
  libpix_info.set_data("wchar", format(str, ii)-" bits");

  
  libpix_info.new_node("string");
  str=pix_chars_enc(pix_sys_enc());
  libpix_info.set_data("encode", str);
  str=pix_chars_enc(pix_enc_ucs2());
  libpix_info.set_data("ucs2", str);
  str=pix_chars_enc(pix_enc_ucs4());
  libpix_info.set_data("ucs4", str);
  libpix_info.set_data("iconv", "support");

  libpix_info.new_node("stream");
  libpix_info.new_child("file");
  format(str, pix_sys_fbuf());
  libpix_info.set_data("fbuf", str-" bytes");
  str='0';
  pix_uint umask = pix_sys_umask();
  pix_uint u = (umask & 0700) >> 6;
  str-u;
  u = (umask & 0070) >> 3;
  str-u;
  u = umask & 0007;
  str-u;
  libpix_info.set_data("umask", str);
  libpix_info.new_node("memory");
  libpix_info.parent();

  libpix_info.new_node("image");
  libpix_info.new_child("format");
  libpix_info.set_data("jpg", "support");
  libpix_info.set_data("tif", "todo");
  libpix_info.set_data("png", "todo");

  libpix_info.new_node("filter");
  libpix_info.set_data("resize", "builtin");
  libpix_info.set_data("icc",  "todo");

  libpix_info.new_node("text");
  libpix_info.set_data("freetype", "todo");
  libpix_info.parent();

  libpix_info.top();

  format(str, libpix_info);
  printf("%s\n", str.c_str());
}

pix_uint
PIXAPI pix_cfg_umask(pix_uint umask)
{
  pix_uint ret = PIXIN::lib().umask;
  umask &= 0777;
  return ret;
}

int 
PIXAPI pix_cfg_fbuf(int size)
{
  int g = PIXIN::lib().gran;
  if (size <= g) size = g;
  else {
    size /= g;
    size *= g;
  }
  return PIXIN::lib().fbuf = size;
}

void
PIXAPI pix_cfg_log(int level)
{
  PIXIN::lib().log = level;
}

void
PIXAPI pix_cfg_log_handler(pix_log_func logfn)
{
  PIXIN::lib().logfn = logfn;
}

/* ######################### Log ############################## */

PIXIN_BEG

static void logfn(const char *msg, int type)
{
  switch (type) {
    case 1:
      {
        int errid = pix_log_errid();
        fprintf(stderr, "ERROR : %s\n", msg);
        break;
      }
    case 2:
      {
        fprintf(stdout, "WARNING : %s\n", msg);
        break;
      }
    case 3:
      {
        fprintf(stdout, "%s\n", msg);
        break;
      }
  }
}
PIXIN_END

int
PIXAPI pix_log()
{
  return PIXIN::lib().log;
}

void
PIXAPI pix_log_msg(const char *msg)
{
  if (pix_log() < 3) return;
  if (PIXIN::lib().logfn) PIXIN::lib().logfn(msg, 1);
  else PIXIN::logfn(msg, 3);
}
void
PIXAPI pix_log_warn(const char *msg)
{
  if (pix_log() < 2) return;
  if (PIXIN::lib().logfn) PIXIN::lib().logfn(msg, 2);
  else PIXIN::logfn(msg, 2);
}
void
PIXAPI pix_log_error(const char *msg, int eid)
{
  PIXIN::lib().errid() = eid;
  if (pix_log() < 1) return;
  if (PIXIN::lib().logfn) PIXIN::lib().logfn(msg, 3);
  else PIXIN::logfn(msg, 1);
}

int
PIXAPI pix_log_errid()
{
  return PIXIN::lib().errid();
}

void
PIXAPI pix_log_clear()
{
  PIXIN::lib().errid() = 0;

}


