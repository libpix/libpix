/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  exif.hpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *        License:  LGPL
 *
 *    Description:  
 *
 * =============================================================
 */
/**
 * @file
 * @brief %exif information
 */

#ifndef __EXIF_HPP__
#define __EXIF_HPP__

#include <pix/config.h>
#include <pix/string.hpp>

PIXAPI_CPP_BEG
/**
 * @addtogroup image
 * @{
 */
/**
 * @brief TODO %exif params
 * 
 */
class Exif : public Base {
public: 
  Exif() : m_hres(0), m_vres(0), m_res_unit(PIX_UNIT) { }

  double hres() const { return m_hres;} 
  double vres() const { return m_hres;}
  pix_unit res_unit() const { return m_res_unit; }

  void hres(double res) { m_hres = res; }
  void vres(double res) { m_vres = res; }
  void res_unit(pix_unit unit) { m_res_unit = unit; }
  
/* TODO
  String m_manuf;
  String m_model;
  String m_software;
  String m_datetime;
  String m_expotime;
  String m_expoprog;
  double fn;
  String m_ver;
  String m_otime;
  String m_dtime;
  String m_components;

  pix_orient m_orient;
  pix_comp m_comp;
  pix_unit m_unit;
  double x_res;
  double y_res;
*/
private:
    double m_hres;
    double m_vres;
    pix_unit m_res_unit;
};
/**
 * @}
 */


PIXAPI_CPP_END
#endif


