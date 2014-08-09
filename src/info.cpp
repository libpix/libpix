/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  info.cpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 */
#include <pix/info.hpp>
#include <stdio.h>
#include "impl/info.hpp"
#include "inner.hpp"

PIXAPI_CPP_BEG

class _Info {
public:
  impl::Info impl;
public:

};

Info::Info()
{
  m_info = new _Info;
}
Info::~Info()
{
  if (m_info) delete m_info;
}

void Info::top()
{
  m_info->impl.top();
}

void Info::first()
{
  m_info->impl.first();
}

bool Info::next()
{
  return m_info->impl.next();
}

bool Info::prev()
{
  return m_info->impl.prev();
}

bool Info::child()
{
  return m_info->impl.child();
}

bool Info::parent()
{
  return m_info->impl.parent();
}

void Info::new_node()
{
  m_info->impl.new_node();
}
void Info::new_node(const String &content)
{
  m_info->impl.new_node(content);
}

void Info::new_child()
{
  m_info->impl.new_child();
}
void Info::new_child(const String &content)
{
  m_info->impl.new_child(content);
}


bool Info::get_data(const String &key, String &val) const
{
  return m_info->impl.get_data(key, val);
}
bool Info::get_content(String &content) const
{
  return m_info->impl.get_content(content);

}

void Info::set_data(const String &key, const String &val)
{
  m_info->impl.set_data(key, val);
}

void Info::set_content(const String &content)
{
  m_info->impl.set_content(content);

}


bool Info::empty() const
{
  return m_info->impl.empty();

}

bool Info::data_first(String &key, String &val)
{
  return m_info->impl.data_first(key, val);
}
bool Info::data_next(String &key, String &val)
{
  return m_info->impl.data_next(key, val);

}
void Info::show()
{
  String str;
  format(str, *this);
  printf("%s", str.c_str());
}

String& format(String &result, Info &info, int depth)
{
  if (!result.keep()) result.clear();
  String ret;
  String key;
  String val;

  if (depth == 0) info.top();
  while(!info.empty()) {
    if (info.get_content(key)) {
      while (!key.empty() && key.at(-1) == '\n') key.sub(0, -2);
      ret-key-'\n';
    }
    if (info.data_first(key,val)) {
      ret-"| "-key-" = "-val-'\n';
      while (info.data_next(key, val)) {
        ret-"| "-key-" = "-val-'\n';
      }
    }
    if (info.child()) {
      String res;
      format(res, info, depth + 1);
      String ss;
      res.get_indent(val, 2);

      ret-val;
      info.parent();
    }
    if (!info.next()) break;
    if (depth == 0) ret-'\n';
  }
  result.append(ret);
  return result;
}
PIXIN_BEG
static void line(String &result,int color, const char *buf, pix_len len)
{
  /*
   * 0: white-blue
   * 1: black-white
   * 2: black-purple
   */
  String temp(buf, len);
  if (color == 0) {
    result.append("\033[44;33m");
    temp.get_escape(result, PIX_ESC_DOT);
    result.append("\033[0m");
  } else if (color == 1) {
    result.append("\033[47;30m");
    temp.get_escape(result, PIX_ESC_DOT);
    result.append("\033[0m");
  } else if (color == 2) {
    result.append("\033[45;30m");
    temp.get_escape(result, PIX_ESC_DOT);
    result.append("\033[0m");
  }
}
static pix_pos lines(String &result, int color, pix_pos off, int width, const char *buf, pix_len len)
{
  if (len == 0) return off;
  pix_pos ll = width - off;
  if (len <= ll) {
    line(result, color, buf, len);
    if (len == ll) {
      result.append('\n');
      return 0;
    }
    return len + off;
  } else {
    line(result, color, buf, ll);
    result.append('\n');
    buf+= ll;
    len -= ll;
    while (len >= width) {
      line(result, color, buf, width);
      result.append('\n');
      len -= width;
      buf += width;
    }
    if (len > 0) {
      line(result, color, buf, len);
    }
  }
  return len;
}
PIXIN_END

Info & info_push(Info &info, const Buffer &buf, int width)
{
  String result;
  result.keep(true);
  const char *pp = buf.bp();
  pix_len plen = buf.p_avail();
  pix_len rlen = buf.r_avail();
  pix_len wlen = buf.w_avail();
  pix_len ll;
  ll = PIXIN::lines(result, 0, 0, width, pp, plen);
  pp += plen;
  ll = PIXIN::lines(result, 1, ll, width, pp, rlen);
  pp += rlen;
  ll = PIXIN::lines(result, 0, ll, width, pp, wlen);
  result.keep(false);
  info.new_node(result);
  return info;
}

Info & info_push(Info &info, const String &str, int width)
{
  info.new_node(str);
  String t;
  pix_len len = str.length();
  pix_len sz = str.size();
  double ratio = (double) len/sz;
  info.set_data("length", format(t, len));
  info.set_data("size", format(t, sz));
  info.set_data("ratio", format(t, ratio, 3));
  info.set_data("encode", pix_chars_enc(str.encode()));
  info.new_child();
  Buffer &buf = _Inner::get_buffer(str);
  info_push(info, buf, width);
  info.parent();
  return info;
}


String & pix_stm_flag_str(String &str, int flag)
{
  if (!str.keep()) str.clear();
  if (flag == 0) return str-"CLOSED";
  if (flag & PIX_STM_READ) str-"READ";
  if (flag & PIX_STM_WRITE)
    if (str.empty()) str-"WRITE";
    else str-'|'-"WRITE";
  if (flag & PIX_STM_APPEND)
    if (str.empty()) str-"APPEND";
    else str-'|'-"APPEND";
  if (flag & PIX_STM_CREATE)
    if (str.empty()) str-"CREATE";
    else str-'|'-"CREATE";
  if (flag & PIX_STM_TRUNC)
    if (str.empty()) str-"TRUNC";
    else str-'|'-"TRUNC";

  return str;
}
int buf_view(String &str, const char *bp, const char *rp, const char *wp, int width, int row)
{
  int maxlen = width * row;
  if (!str.keep()) str.clear();
  if (bp == 0 || rp == 0 || wp == 0 || rp < bp || rp >= wp) {
    str-"<no buffer>";
    return -1;
  }
  if (maxlen == 0) maxlen = wp - bp;

  bool kk = str.keep(true);

  int len = maxlen;
  const char *pp = rp;
  if (pp < bp) pp = bp;
  else {
    int r = pp - bp;
    r /= maxlen;
    r *= maxlen;
    pp = bp + r;
  }

  const char *lp = pp + maxlen;
  if (lp > wp) lp = wp;

  int plen = rp - pp;
  int rlen = 1;
  int wlen = lp - rp - 1;
  int ret = pp - bp;
  pix_pos ll;

  ll = PIXIN::lines(str, 0, 0, width, pp, plen);
  pp += plen;
  ll = PIXIN::lines(str, 2, ll, width, pp, rlen);
  pp += rlen;
  ll = PIXIN::lines(str, 0, ll, width, pp, wlen);
  str.keep(kk);
  return ret;
}


Info & info_push(Info &info, const FileStream &fstm, int width, int row)
{
  String str;
  str-"pix_stm"-'\n'-fstm.name();
  info.new_node(str);
  pix_stm_flag_str(str, fstm.flag());
  info.set_data("flag", str);
  info.set_data("pos", format(str, fstm.tell()));
  info.set_data("length", format(str, fstm.length()));

  int buf_size;
  int buf_len;
  pix_pos buf_off;
  pix_pos buf_pos;
  char * buf = _Inner::get_buffer(fstm, buf_size, buf_len, buf_off, buf_pos);
  info.set_data("buf_len", format(str, buf_len));
  info.set_data("buf_size", format(str, buf_size));
  info.set_data("buf_off", format(str, buf_off));
  info.set_data("buf_pos", format(str, buf_pos));
  if (width > 0) {
    const char *bp = buf;
    const char *rp = buf + buf_pos;
    const char *wp = buf + buf_len;
    str.clear();

    int off = buf_view(str, bp, rp, wp, width, row);

    info.new_child(str);
    if (row > 0 && off >=0) info.set_data("view_off", format(str, off));
    info.parent();

  }


  return info;

}

Info & info_push(Info &info, const MemStream &mstm, int width, int row)
{
  String str;
  str-"pix_stm"-'\n'-mstm.name();
  info.new_node(str);
  pix_stm_flag_str(str, mstm.flag());
  info.set_data("flag", str);
  info.set_data("pos", format(str, mstm.tell()));
  info.set_data("length", format(str, mstm.length()));
  info.set_data("size", format(str, mstm.size()));
  if (width > 0) {
    Buffer &buf = _Inner::get_buffer(mstm);
    char *bp = buf.rp();
    char *rp = bp + mstm.tell();
    char *wp = buf.wp() + 1;
    str.clear();
    int off = buf_view(str, bp, rp, wp, width, row);

    info.new_child(str);
    if (row > 0 && off >=0) info.set_data("view_off", format(str, off));
    info.parent();
  }
  return info;

}
Info & info_push(Info &info, const pix_img_pm &pm)
{
  String str;
  info.new_node();
  info.set_data("cs", pix_chars_cs(pm.cs));
  str=(int)pm.bits[0];
  const char *p = pm.bits + 1;
  int depth= *p;
  while (*p) {
    str-' '-(int)(*p);
    depth += (int)*p;
    p++;
  }
  info.set_data("bits", depth-=" ["-=str-']');
  info.set_data("cols", format(str, pm.cols));
  info.set_data("rows", format(str, pm.rows));
  info.set_data("hres", format(str, pm.hres));
  info.set_data("vres", format(str, pm.vres));
  info.set_data("unit", pix_chars_unit(pm.unit));
  if (pm.type == PIX_IMG_JPG) {
    info.set_data("type", "jpg");
    info.set_data("jpg.cs", pix_chars_cs(pm.jpg.cs)); 
    if (pm.jpg.fax) str = "yes";
    else str = "no";
    info.set_data("jpg.fax", str);
    if (pm.jpg.ps) str = "yes";
    else str = "no";
    info.set_data("jpg.ps", str);
    if (pm.jpg.exif) str = "yes";
    else str = "no";
    info.set_data("jpg.exif", str);
    info.set_data("jpg.icc", format(str, pm.jpg.icc));

  }
  return info;
}

Info & info_push(Info &info, const ImageBase &img)
{
  return info_push(info, img.param());

}

PIXAPI_CPP_END

