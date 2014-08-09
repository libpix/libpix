/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  pix_str.cpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 */

#include "pix_obj.h" 


pix_str 
PIXAPI pix_str_create()
{
  return new _pix_str;
}

void 
PIXAPI pix_str_destroy(pix_str *str)
{
  if (!str) return;
  if (!*str) return;
  delete *str;
  *str = 0;
}

void 
PIXAPI pix_str_swap(pix_str str1, pix_str str2)
{
  if (!str1) return;
  if (!str2) return;
  str1->str->swap(*str2->str);
}
void 
PIXAPI pix_str_swap_buf(pix_str str, pix_buf buf)
{
  if (!str) return;
  if (!buf) return;
  str->str->swap(*buf->buf);
}

pix_len 
PIXAPI pix_str_length(pix_str str)
{
  if (!str) return 0;
  return str->str->length();
}
pix_len 
PIXAPI pix_str_size(pix_str str)
{
  if (!str) return 0;
  return str->str->size();
}
int 
PIXAPI pix_str_equal(pix_str str, pix_str str2)
{
  if (!str) return 0;
  if (!str2) return 0;
  if (str->str->equal(*str2->str)) return 1;
  return 0;
}
int 
PIXAPI pix_str_empty(pix_str str)
{
  if (!str) return 1;
  if (str->str->empty()) return 1;
  return 0;
}
const char * 
PIXAPI pix_str_chars(pix_str str)
{
  if (!str) return 0;
  return str->str->c_str();
}
void 
PIXAPI pix_str_clear(pix_str str)
{
  if (!str) return;
  str->str->clear();
}

void 
PIXAPI pix_str_assign(pix_str str, pix_str src)
{
  if (!str) return;
  str->str->assign(*src->str);
}

void 
PIXAPI pix_str_assign_char(pix_str str, char ch, pix_len count)
{
  if (!str) return;
  str->str->assign(ch, count);
}
void 
PIXAPI pix_str_assign_chars(pix_str str, const char *buf, pix_len len, 
    int enc)
{
  if (!str) return;
  str->str->assign(buf, len, (pix_enc) enc);

}

void 
PIXAPI pix_str_append(pix_str str, pix_str src)
{
  if (!str) return;
  str->str->append(*src->str);
}

void 
PIXAPI pix_str_append_char(pix_str str, char ch, pix_len count)
{
  if (!str) return;
  str->str->append(ch, count);
}

void 
PIXAPI pix_str_append_chars(pix_str str, const char *buf, pix_len len, 
    int enc)
{
  if (!str) return;
  str->str->append(buf, len, (pix_enc) enc);
}
void 
PIXAPI pix_str_insert(pix_str str, pix_pos pos, pix_str src)
{
  if (!str) return;
  str->str->insert(pos, *src->str);
}

void 
PIXAPI pix_str_insert_char(pix_str str, pix_pos pos, char ch, pix_len count)
{
  if (!str) return;
  str->str->insert(pos, ch, count);
}
void 
PIXAPI pix_str_insert_chars(pix_str str, pix_pos pos, const char *buf, pix_len len, 
    int enc)
{
  if (!str) return;
  str->str->insert(pos, buf, len, (pix_enc) enc);
}
void 
PIXAPI pix_str_replace(pix_str str, pix_pos beg, pix_pos end, pix_str sub)
{
  if (!str) return;
  str->str->replace(beg, end, *sub->str);

}
void 
PIXAPI pix_str_replace_char(pix_str str, pix_pos beg, pix_pos end, 
    char ch, pix_len count)
{
  if (!str) return;
  str->str->replace(beg, end, ch, count);
}
void 
PIXAPI pix_str_replace_chars(pix_str str, pix_pos beg, pix_pos end, 
    const char *buf, pix_len len, int enc)
{
  if (!str) return;
  str->str->replace(beg, end, buf, len, (pix_enc) enc);
}

pix_pos 
PIXAPI pix_str_find(pix_str str, pix_str sub,
    pix_pos beg, pix_pos end)
{
  if (!str) return -1;
  if (!sub) return -1;
  return str->str->find(*sub->str, beg, end);
}

pix_pos 
PIXAPI pix_str_find_char(pix_str str, char ch, pix_len count, 
    pix_pos beg, pix_pos end)
{
  if (!str) return -1;
  return str->str->find(ch, count, beg, end);

}
pix_pos 
PIXAPI pix_str_find_chars(pix_str str, const char *buf, pix_len len, 
    pix_pos beg, pix_pos end)
{
  if (!str) return -1;
  return str->str->find(buf, len, beg, end);

}

void 
PIXAPI pix_str_remove(pix_str str, pix_pos beg, pix_pos end)
{
  if (!str) return;
  str->str->remove(beg, end);
}

void 
PIXAPI pix_str_sub(pix_str str, pix_pos beg, pix_pos end)
{
  if (!str) return;
  str->str->sub(beg, end);
}

void 
PIXAPI pix_str_trim(pix_str str)
{
  if (!str) return;
  str->str->trim();

}

void 
PIXAPI pix_str_reverse(pix_str str)
{
  if (!str) return;
  str->str->reverse();
}

void 
PIXAPI pix_str_repeat(pix_str str, pix_len count)
{
  if (!str) return;
  str->str->repeat(count);
}
int 
PIXAPI pix_str_keep(pix_str str)
{
  if (!str) return 0;
  return (int)str->str->keep();
}
int 
PIXAPI pix_str_keep(pix_str str, int val)
{
  if (!str) return 0;
  if (str->str->keep(!!val)) return 1;
  return 0;
}

void 
PIXAPI pix_str_get_sub(pix_str str, pix_str result, pix_pos beg, pix_pos end)
{
  if (!str) return;
  if (!result) return;
  str->str->get_sub(*result->str, beg, end);
}

pix_pos 
PIXAPI pix_str_get_line(pix_str str, pix_str result, pix_pos pos, const char *eol)
{
  if (!str) return -1;
  if (!result) return -1;
  return str->str->get_line(*result->str, pos, eol);
}
void 
PIXAPI pix_str_get_indent(pix_str str, pix_str result, pix_len count,
    const char *indent, const char *neweol, const char *eol)
{
  if (!str) return;
  if (!result) return;
  str->str->get_indent(*result->str, count, indent, neweol, eol);
}
void 
PIXAPI pix_str_get_escape(pix_str str, pix_str result,
    int esc)
{
  if (!str) return;
  if (!result) return;
  str->str->get_escape(*result->str, (pix_esc) esc);
}

void 
PIXAPI pix_str_inf(pix_str str, pix_inf inf)
{
  format(*str->str, *inf->inf);
}
