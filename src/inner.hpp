/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  inner.hpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 */

#include <libpix.h>
#define PIXIN _Pixin
#define PIXIN_BEG namespace _Pixin {
#define PIXIN_END }

PIXAPI_CPP_BEG
class _Inner {
public:

  static Buffer & get_buffer(const String & str);
  static Buffer & get_buffer(const MemStream &mstm);
  static char * get_buffer(const FileStream &fstm, 
      int &buf_size, int &buf_len, pix_pos &buf_off, pix_pos &buf_pos);
  static _View * get_view(View & v);

};

PIXAPI_CPP_END
