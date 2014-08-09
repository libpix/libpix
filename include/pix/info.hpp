/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  info.hpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *        License:  LGPL
 *
 *    Description:  structured information
 *
 * =============================================================
 */

/**
 * @file
 * @brief %structrued information
 * 
 */

#ifndef __PIX_INFO_HPP__
#define __PIX_INFO_HPP__

#include <pix/memory.hpp>
#include <pix/string.hpp>
#include <pix/stream.hpp>
#include <pix/image.hpp>


PIXAPI_CPP_BEG

/**
 * @defgroup info Info
 * @brief %structured information
 * @{
 */
class _Info;

/**
 * @brief %structured information collector
 * 
 */
class Info : public Base{
public:
  Info();
  ~Info();

  void new_node();
  void new_node(const String &content);
  void new_child();
  void new_child(const String &content);

  void top();
  void first();
  bool child();
  bool parent();
  bool next();
  bool prev();

  bool data_first(String &key, String &val); 
  bool data_next(String &key, String &val);

  bool empty() const;
  bool get_content(String &content) const;
  bool get_data(const String &key, String &val) const ;
  void set_content(const String &content);
  void set_data(const String &key, const String &val);
  void show();

private:
  Info(const Info &inf) { }
  _Info *m_info;
};

Info & info_push(Info &info, const Buffer &buf, int width = 64);
Info & info_push(Info &info, const String &str, int width = 64);
Info & info_push(Info &info, const FileStream &fstm, int width = 64, int row = 10);
Info & info_push(Info &info, const MemStream &mstm, int width = 64, int row = 10);
Info & info_push(Info &info, const pix_img_pm &pm);
Info & info_push(Info &info, const ImageBase &img);

template<class T>
Info & operator << (Info &info, const T &obj)
{
  return info_push(info, obj);
}

String& format(String &result, Info &info, int depth = 0);

/**
 * @}
 */

PIXAPI_CPP_END

#endif
