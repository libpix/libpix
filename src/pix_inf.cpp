/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  pix_inf.cpp
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

pix_inf
PIXAPI pix_inf_create()
{
  return new _pix_inf;
}
void 
PIXAPI pix_inf_destroy(pix_inf *inf)
{
  if (!inf) return;
  if (!*inf) return;
  delete *inf;
  *inf = 0;
}
int 
PIXAPI pix_inf_empty(pix_inf inf)
{
  if (!inf) return 1;
  return static_cast<int>(inf->inf->empty());

}
void 
PIXAPI pix_inf_new_node(pix_inf inf)
{
  if (!inf) return;
  inf->inf->new_node();
}
void 
PIXAPI pix_inf_new_node_content(pix_inf inf, pix_str content)
{
  if (!inf) return;
  inf->inf->new_node(*content->str);
}
void 
PIXAPI pix_inf_new_child(pix_inf inf)
{
  if (!inf) return;
  inf->inf->new_child();

}
void 
PIXAPI pix_inf_new_child_content(pix_inf inf, pix_str content)
{
  if (!inf) return;
  inf->inf->new_child(*content->str);
}
void 
PIXAPI pix_inf_top(pix_inf inf)
{
  if (!inf) return;
  inf->inf->top();
}
void 
PIXAPI pix_inf_first(pix_inf inf)
{
  if (!inf) return;
  inf->inf->first();

}
int 
PIXAPI pix_inf_child(pix_inf inf)
{
  if (!inf) return 0;
  return static_cast<int>(inf->inf->child());

}
int 
PIXAPI pix_inf_parent(pix_inf inf)
{
  if (!inf) return 0;
  return static_cast<int>(inf->inf->parent());

}
int 
PIXAPI pix_inf_next(pix_inf inf)
{
  if (!inf) return 0;
  return static_cast<int>(inf->inf->next());
}
int 
PIXAPI pix_inf_prev(pix_inf inf)
{
  if (!inf) return 0;
  return static_cast<int>(inf->inf->prev());

}
void 
PIXAPI pix_inf_data_set(pix_inf inf, pix_str key, pix_str val)
{
  if (!inf) return;
  inf->inf->set_data(*key->str, *val->str);
}
int 
PIXAPI pix_inf_data_get(pix_inf inf, pix_str key, pix_str val)
{
  if (!inf) return 0;
  return static_cast<int>(inf->inf->get_data(*key->str, *val->str));
}

int 
PIXAPI pix_inf_data_first(pix_inf inf, pix_str key , pix_str val)
{
  if (!inf) return 0;
  return static_cast<int>(inf->inf->data_first(*key->str, *val->str));

}
int 
PIXAPI pix_inf_data_next(pix_inf inf, pix_str key, pix_str val)
{
  if (!inf) return 0;
  return static_cast<int>(inf->inf->data_next(*key->str, *val->str));
}

int 
PIXAPI pix_inf_content_get(pix_inf inf, pix_str content)
{
  if (!inf) return 0;
  return static_cast<int>(inf->inf->get_content(*content->str));

}
void 
PIXAPI pix_inf_content_set(pix_inf inf, pix_str content)
{
  if (!inf) return;
  inf->inf->set_content(*content->str);

}

void 
PIXAPI pix_inf_push_str(pix_inf inf, pix_str str)
{
  if (!inf) return;
  Pix::info_push(*inf->inf, *str->str);
}

void 
PIXAPI pix_inf_push_stm(pix_inf inf, pix_stm stm, int width, int row)
{
  if (!inf) return;
  if (!inf->inf) return;
  if (stm->fstm) Pix::info_push(*inf->inf, *stm->fstm, width, row);
  else if (stm->mstm) Pix::info_push(*inf->inf, *stm->mstm, width, row);
}

const char *
PIXAPI pix_inf_chars(pix_inf inf)
{
  if (!inf) return "";
  if (!inf->str) inf->str = new Pix::String;
  format(*inf->str, *inf->inf);
  return inf->str->c_str();
}



