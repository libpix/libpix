/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  impl/image_jpg.static.hpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  Used only by image_jpg.cpp 
 *
 * =============================================================
 */

/*
 * Have static function , only include by image_jpg.cpp
 */
#ifndef __IMPL_IMAGE_JPEG_IJG_STATIC_HPP__ 
#define __IMPL_IMAGE_JPEG_IJG_STATIC_HPP__

#include <impl/config.h>
#include <pix/memory.hpp>
#include <pix/stream.hpp>
#include <pix/exif.hpp>
#include <stdio.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <stdio.h>
#include <pix/info.hpp>

PIXAPI_CPP_BEG
PIXIMPL_BEG

#define ICC_MARKER  (JPEG_APP0 + 2)   /*  JPEG marker code for ICC */
#define ICC_OVERHEAD_LEN  14          /*  size of non-profile data in APP2 */
#define MAX_BYTES_IN_MARKER  65533    /*  maximum data len of a JPEG marker */
#define MAX_DATA_BYTES_IN_MARKER  (MAX_BYTES_IN_MARKER - ICC_OVERHEAD_LEN)
#define PS_FIXED_TO_FLOAT(h, l) ((double) (h) + ((double) (l)/(1<<16)))

inline pix_cs jpg_pix_cs(J_COLOR_SPACE jcs)
{
  switch (jcs) {
  case JCS_GRAYSCALE:
    return PIX_CS_GRAY;
  case JCS_RGB:
    return PIX_CS_RGB;
  case JCS_YCbCr:
    return PIX_CS_YCbCr;
  case JCS_CMYK:
    return PIX_CS_CMYK;
  case JCS_YCCK:
    return PIX_CS_YCCK;
  }
  return PIX_CS;
}

inline J_COLOR_SPACE jpg_jcs(pix_cs cs)
{
  switch (cs) {
  case PIX_CS_GRAY:
    return JCS_GRAYSCALE;
  case PIX_CS_RGB:
    return JCS_RGB;
  case PIX_CS_YCbCr:
    return JCS_YCbCr;
  case PIX_CS_CMYK:
    return JCS_CMYK;
  case PIX_CS_YCCK:
    return JCS_YCCK;
  }
  return JCS_UNKNOWN;
}

inline pix_unit jpg_pix_unit(int v)
{
  if (v == 1) return PIX_UNIT_INCH;
  if (v == 2) return PIX_UNIT_CM;
  return PIX_UNIT;

}
inline pix_unit exif_pix_unit(int v)
{
  if (v == 2) return PIX_UNIT_INCH;
  if (v == 3) return PIX_UNIT_CM;
  return PIX_UNIT;

}
inline int jpg_unit(pix_unit unit)
{
  if (unit == PIX_UNIT_INCH) return 1;
  if (unit == PIX_UNIT_CM) return 2;
  return 0;
}
inline int exif_unit(pix_unit unit)
{
  if (unit == PIX_UNIT_INCH) return 2;
  if (unit == PIX_UNIT_CM) return 3;
  return 0;
}

inline unsigned int get_exif_uint(unsigned char *data, int endian)
{
  union {
    unsigned int i;
    unsigned char c[4];
  } un;
  if (endian) {
    un.c[0] = GETJOCTET(data[0]);
    un.c[1] = GETJOCTET(data[1]);
    un.c[2] = GETJOCTET(data[2]);
    un.c[3] = GETJOCTET(data[3]);
  } else {
    un.c[0] = GETJOCTET(data[3]);
    un.c[1] = GETJOCTET(data[2]);
    un.c[2] = GETJOCTET(data[1]);
    un.c[3] = GETJOCTET(data[0]);

  }
  return un.i;
}

inline bool marker_is_icc(jpeg_saved_marker_ptr marker)
{
  if (  marker->marker == ICC_MARKER &&
        marker->data_length >= ICC_OVERHEAD_LEN &&
        /*  verify the identifying string */
        GETJOCTET(marker->data[0]) == 0x49 &&
        GETJOCTET(marker->data[1]) == 0x43 &&
        GETJOCTET(marker->data[2]) == 0x43 &&
        GETJOCTET(marker->data[3]) == 0x5F &&
        GETJOCTET(marker->data[4]) == 0x50 &&
        GETJOCTET(marker->data[5]) == 0x52 &&
        GETJOCTET(marker->data[6]) == 0x4F &&
        GETJOCTET(marker->data[7]) == 0x46 &&
        GETJOCTET(marker->data[8]) == 0x49 &&
        GETJOCTET(marker->data[9]) == 0x4C &&
        GETJOCTET(marker->data[10]) == 0x45 &&
        GETJOCTET(marker->data[11]) == 0x0 ) return true;
  return false;
}

inline bool process_exif(JOCTET FAR * data, int len, Exif *exif)
{
  int endian; /* 1 LE, 0 BE */
  int number_of_tags, tagnum;
  int firstoffset;
  //JDIMENSION new_value;

  if (len < 12) return false;

  if (GETJOCTET(data[0]) == 0x49 && GETJOCTET(data[1]) == 0x49)
    endian = 1;                                 /* little endian */
  else if (GETJOCTET(data[0]) == 0x4D && GETJOCTET(data[1]) == 0x4D)
    endian = 0;
  else return false;

  /* Check Tag Mark */
  if (endian) {
    if (GETJOCTET(data[3]) != 0) return false;
    if (GETJOCTET(data[2]) != 0x2A) return false;
  } else {
    if (GETJOCTET(data[2]) != 0) return false;
    if (GETJOCTET(data[3]) != 0x2A) return false;
  }

  /* Get first IFD offset (ffset to IFD0) */
  if (endian) {
    if (GETJOCTET(data[7]) != 0) return false;
    if (GETJOCTET(data[6]) != 0) return false;
    firstoffset = GETJOCTET(data[5]);
    firstoffset <<= 8;
    firstoffset += GETJOCTET(data[4]);
  } else {
    if (GETJOCTET(data[4]) != 0) return false;
    if (GETJOCTET(data[5]) != 0) return false;
    firstoffset = GETJOCTET(data[6]);
    firstoffset <<= 8;
    firstoffset += GETJOCTET(data[7]);
  }

  if (firstoffset > len - 2) return false; /* check end of data segment */

  /* Get the number of directory entries contained in this IFD */
  if (endian) {
    number_of_tags = GETJOCTET(data[firstoffset + 1]);
    number_of_tags <<= 8;
    number_of_tags += GETJOCTET(data[firstoffset]);
  } else {
    number_of_tags = GETJOCTET(data[firstoffset]);
    number_of_tags <<= 8;
    number_of_tags += GETJOCTET(data[firstoffset + 1]);
  }

  if (number_of_tags == 0) return false;
  firstoffset += 2;

  /* Search for ExifSubIFD offset Tag in IFD0 */

  double hres = 0.0, vres = 0.0;
  pix_unit res_unit = PIX_UNIT;

  for ( ; ; ) {
    if (firstoffset > len - 12) return false; /* check end of data segment */
    /* Get Tag number */
    if (endian) {
      tagnum = GETJOCTET(data[firstoffset + 1]);
      tagnum <<= 8;
      tagnum += GETJOCTET(data[firstoffset]);
    } else {
      tagnum = GETJOCTET(data[firstoffset]);
      tagnum <<= 8;
      tagnum += GETJOCTET(data[firstoffset + 1]);
    }

    if (tagnum == 0x011a) { /* xdpi */
      unsigned int offset = get_exif_uint(data + firstoffset + 8, endian);
      unsigned int a = get_exif_uint(data + offset, endian);
      unsigned int b = get_exif_uint(data + offset + 4, endian);
      hres = a / b;
    }
    if (tagnum == 0x11b) { /* ydpi */
      unsigned int offset = get_exif_uint(data + firstoffset + 8, endian);
      unsigned int a = get_exif_uint(data + offset, endian);
      unsigned int b = get_exif_uint(data + offset + 4, endian);
      vres = a / b;
    }
    if (tagnum == 0x128) { /* unit */
      int unit = get_exif_uint(data + firstoffset + 8, endian);
      res_unit = exif_pix_unit(unit);
    }

    /* TODO more exif filed */

    if (--number_of_tags == 0) break;
    firstoffset += 12;
  }
  exif->res_unit(res_unit);
  exif->hres(hres);
  exif->vres(vres);
  /* TODO save exif */

  return true;
}


inline bool process_photoshop(JOCTET FAR *data, int datalen, double *xdpi, double *ydpi)
{
  int i;
  for (i = 14; i < datalen; )
  {
    long len;
    unsigned int type;

    if (!(GETJOCTET(data[i]  ) == 0x38 &&
          GETJOCTET(data[i+1]) == 0x42 &&
          GETJOCTET(data[i+2]) == 0x49 &&
          GETJOCTET(data[i+3]) == 0x4D)) break; // Not recognized
    i += 4; // identifying string 

    type = (unsigned int) (GETJOCTET(data[i]<<8) + GETJOCTET(data[i+1]));

    i += 2; // resource type 

    i += GETJOCTET(data[i]) + ((GETJOCTET(data[i]) & 1) ? 1 : 2);   // resource name 

    len = ((((GETJOCTET(data[i]<<8) + GETJOCTET(data[i+1]))<<8) + 
          GETJOCTET(data[i+2]))<<8) + GETJOCTET(data[i+3]);
    i += 4; // Size 

    if (type == 0x03ED && len >= 16) {

      *xdpi = PS_FIXED_TO_FLOAT(GETJOCTET(data[i]<<8) + GETJOCTET(data[i+1]),
          GETJOCTET(data[i+2]<<8) + GETJOCTET(data[i+3]));
      *ydpi = PS_FIXED_TO_FLOAT(GETJOCTET(data[i+8]<<8) + GETJOCTET(data[i+9]),
          GETJOCTET(data[i+10]<<8) + GETJOCTET(data[i+11]));

      return true;
    }
    i += len + ((len & 1) ? 1 : 0);   // Alignment  
  }
  return false;
} 


typedef struct {
  struct jpeg_error_mgr super;
  jmp_buf jmp;
  char msg[JMSG_LENGTH_MAX];
} jpg_err;

typedef struct {
  jpeg_source_mgr super;
  Stream *stm;
  pix_len len;
} jpg_src;

typedef struct {
  jpeg_destination_mgr super;
  Stream *stm;
  pix_len len;
} jpg_dst;

static void error_exit(j_common_ptr cinfo)
{
  jpg_err *err = (jpg_err *)cinfo->err;
  char msgbuf[JMSG_LENGTH_MAX];
  err->super.format_message(cinfo, msgbuf);
//  Error("JpegLib error");
//  Error(msgbuf);
  longjmp(err->jmp, 1);
  return; 
}
static void output_message(j_common_ptr cinfo)
{
  struct error *err = (struct error *)cinfo->err;
//  char msgbuf[JMSG_LENGTH_MAX];
  //err->super.format_message(cinfo, msgbuf);
  return; 
}

static void init_source(j_decompress_ptr cinfo) 
{
  jpg_src *src = (jpg_src *)cinfo->src;
  Stream *stm = src->stm;
  pix_len len = 0;
  const char *rp = stm->rp(len, pix_sys_fbuf());
  src->len = len;
  src->super.bytes_in_buffer = len;
  src->super.next_input_byte = (const JOCTET*) rp;
}

static boolean fill_input_buffer(j_decompress_ptr cinfo) 
{
  jpg_src *src = (jpg_src *)cinfo->src;
  Stream *stm = src->stm;
  pix_len len = src->len;
  const char *rp = stm->rp(len, pix_sys_fbuf());
  src->len = len;
  src->super.bytes_in_buffer = len;
  src->super.next_input_byte = (const JOCTET*)rp;
  if (len) return TRUE;
  else return FALSE; 
}

static void skip_input_data(j_decompress_ptr cinfo, long n)
{
  jpg_src *src = (jpg_src *)cinfo->src;
  Stream *stm = src->stm;
  pix_len len = src->len;
  const char * rp = stm->rp(len, 0);
  stm->seek(n, PIX_SEEK_CUR);
  len = 0;
  rp = stm->rp(len, pix_sys_fbuf());
  src->len = len;
  src->super.bytes_in_buffer = len;
  src->super.next_input_byte = (const JOCTET*) rp;
}

static boolean resync_to_restart(j_decompress_ptr cinfo, int desired)
{
  return jpeg_resync_to_restart(cinfo , desired);
}

static void term_source(j_decompress_ptr cinfo) 
{
  jpg_src *src = (jpg_src *)cinfo->src;
}

static void init_destination(j_compress_ptr cinfo) 
{
  jpg_dst *dst = (jpg_dst *)cinfo->dest;
  Stream *stm = dst->stm;
  pix_len len = 0;
  char *wp = stm->wp(len, pix_sys_gran());
  dst->len = len;
  dst->super.free_in_buffer = len;
  dst->super.next_output_byte = (JOCTET*) wp;
}

static boolean empty_output_buffer(j_compress_ptr cinfo) 
{
  jpg_dst *dst = (jpg_dst *)cinfo->dest;
  Stream *stm = dst->stm;
  pix_len len = dst->len;
  char *wp = stm->wp(len, pix_sys_gran());
  dst->len = len;
  dst->super.free_in_buffer = len;
  dst->super.next_output_byte = (JOCTET*) wp;
  if (len == 0) return FALSE;
  return TRUE;
}

static void term_destination(j_compress_ptr cinfo) 
{
  //myflush(cinfo);
  jpg_dst *dst = (jpg_dst *)cinfo->dest;
  Stream *stm = dst->stm;
  pix_len len = dst->len - dst->super.free_in_buffer;
  char *wp = stm->wp(len, 0);
}


/* ------------------------------------------------------------ */

class Jpg {
public:

public:
  int m_type;
  union {
    jpeg_decompress_struct m_jpgd;
    jpeg_compress_struct m_jpgc;
  };
  jpg_err m_err;
  jpg_src m_src;
  jpg_dst m_dst;
  pix_img_pm m_pm;
  pix_pos m_line;
  pix_len m_rsize;
  int m_quality;

public:
  Jpg(int type) : m_type(0), m_line(-1),m_rsize(0), m_quality(85)
  {
    if (type == 1) {
      m_type = 1;
      m_src.stm = 0;
      m_src.super.init_source = init_source;
      m_src.super.fill_input_buffer = fill_input_buffer;
      m_src.super.skip_input_data = skip_input_data;
      m_src.super.resync_to_restart = resync_to_restart;
      m_src.super.term_source = term_source;
      m_src.super.bytes_in_buffer = 0;
      m_src.super.next_input_byte = 0;
      m_jpgd.err = (jpeg_error_mgr *) &m_err;
      jpeg_std_error(&m_err.super);
      m_err.super.error_exit = error_exit;
      m_err.super.output_message = output_message;

    } else if (type == 2) {
      m_type = 2;
      m_dst.stm = 0;
      m_dst.super.init_destination = init_destination;
      m_dst.super.empty_output_buffer = empty_output_buffer;
      m_dst.super.term_destination = term_destination;
      m_dst.super.free_in_buffer = 0;
      m_dst.super.next_output_byte = 0;
      m_jpgc.err = (jpeg_error_mgr*) &m_err;
      jpeg_std_error(&m_err.super);
      m_err.super.error_exit = error_exit;
      m_err.super.output_message = output_message;
    }
  }
  ~Jpg()
  {
    close();
  }
public:
  pix_img_pm& param()
  {
    return m_pm;
  }


  bool open(Stream & stm) {
    if (m_type == 1) {
      if (m_src.stm) return false;
      jpeg_decompress_struct &cinfo = m_jpgd;
      if (!stm.is_read()) return false;

      if (setjmp(m_err.jmp)) {
        return false;
      }
      jpeg_create_decompress(&cinfo);
      cinfo.src = (jpeg_source_mgr *)&m_src;
      m_src.stm = &stm;
      if (!open_read()) return false;
      return true;
    } else if (m_type == 2) {
      if (m_dst.stm) return false;
      jpeg_compress_struct &cinfo = m_jpgc;
      if (!stm.is_write()) return false;

      if (setjmp(m_err.jmp)) {
        return false;
      }

      jpeg_create_compress(&cinfo);
      cinfo.dest = (jpeg_destination_mgr *)&m_dst;
      m_dst.stm = &stm;
      return true;
    }

    return false;
  }
  bool close() {
    if (m_type == 1) {
      if (!m_src.stm) return true;
      jpeg_decompress_struct &cinfo = m_jpgd;
      m_src.stm = 0;
      m_line = -1;
      
      if (setjmp(m_err.jmp)) {
        jpeg_destroy_decompress(&cinfo);
        return true;
      }
      jpeg_abort_decompress(&cinfo);
      jpeg_destroy_decompress(&cinfo);
      return true;
    } else if (m_type == 2) {
      if (!m_dst.stm) return true;
      jpeg_compress_struct &cinfo = m_jpgc;

      if (setjmp(m_err.jmp)) {
        jpeg_destroy_compress(&cinfo);
        m_line = -1;
        return true;
      }

      jpeg_finish_compress(&cinfo);
      jpeg_destroy_compress(&cinfo);
      m_dst.stm = 0;
      m_line = -1;
      return true;
    }
    return false;
  }

  bool ready()
  {
    if (m_line >= 0) return true;
    if (m_type == 1) {
      if (!m_src.stm) return false;
      if (!start_decompress()) return false;
    } else if (m_type == 2) {
      if (!m_dst.stm) return false;
      if (!start_compress()) return false;
    }
    m_line = 0;
    return true;
  }
  pix_pos read(char *buf, pix_pos rows)
  {
    pix_pos r;
    for (r = 0; r < rows; r++)
      if (read_scanline(buf)) {
        buf += m_rsize;
        m_line ++;
      } else return r;
    return r;
  }
  pix_pos write(const char *buf, pix_pos rows)
  {
    pix_pos r;
    for (r = 0; r < rows; r++)
      if (write_scanline(buf)) {
        buf += m_rsize;
        m_line ++;
      } else return r;
    return r;
  }
  void quality(int q)
  {
    m_quality = q;
  }



public:
  void init_param(pix_cs cs)
  {
    if (cs == PIX_CS_RGB) {
    }
    switch (cs) {
    case PIX_CS_RGB:
    case PIX_CS_YCbCr:
    case PIX_CS_CMYK:
      pix_img_pm_init_cs(&m_pm, cs);
      break;
    default:
      pix_img_pm_init(&m_pm);
    }

    m_pm.type = PIX_IMG_JPG;
  }


public: /* reader functions */
  void set_rparam()
  {
    calc();
    pix_cs cs = get_out_color_space();
    init_param(cs); 
    m_pm.cols = get_output_width();
    m_pm.rows = get_output_height();
    m_pm.unit = get_density_unit();
    m_pm.hres = get_x_density();
    m_pm.vres = get_y_density();
  }


  bool open_read()
  {
    save_markers();

    if (!read_header()) return false;
    bool fax = mark_fax();
    bool ps = mark_ps();
    int icc = read_icc(0);

    Exif ex;
    bool exif = read_exif(&ex);
    
    if (exif && (ex.res_unit() != PIX_UNIT)) {
      rset_density_unit(ex.res_unit());
      rset_x_density(ex.hres());
      rset_y_density(ex.vres());
    }

    pix_cs cs = get_jpeg_color_space();
    rset_out_color_space(cs);
    if (cs == PIX_CS_YCbCr) {
      rset_out_color_space(PIX_CS_RGB);
    } else if ((cs == PIX_CS_YCCK) || (cs == PIX_CS_CMYK)) {
      rset_out_color_space(PIX_CS_CMYK);
    }
    set_rparam();
    m_pm.jpg.cs = cs;
    m_pm.jpg.fax = fax;
    m_pm.jpg.ps = ps;
    m_pm.jpg.exif = exif;
    m_pm.jpg.icc = icc;

    return true;
  }

  bool read_header()
  {
    jpeg_decompress_struct &cinfo = m_jpgd;

    if (setjmp(m_err.jmp)) {
      return false;
    }
    int ret = jpeg_read_header(&cinfo, TRUE);
    if (ret == JPEG_SUSPENDED) return false;
    return true;
  }
  void calc()
  {
    jpeg_decompress_struct &cinfo = m_jpgd;

    jpeg_calc_output_dimensions(&cinfo);
  }

  bool start_decompress()
  {
    jpeg_decompress_struct &cinfo = m_jpgd;
    if (setjmp(m_err.jmp)) {
      return false;
    }
    if (jpeg_start_decompress(&cinfo)) {
      m_rsize = cinfo.output_width * cinfo.output_components;
      return true;

    }
    m_rsize = 0;
    pix_log_error("jpg: decomp fail", PIX_ERR_JPG);
    return false;
  }

  pix_pos read_scanline(char *buf)
  {
    jpeg_decompress_struct &cinfo = m_jpgd;

    if (setjmp(m_err.jmp)) {
      return false;
    }
    char * rowp[1];
    rowp[0] = buf;

    return jpeg_read_scanlines(&cinfo, (JSAMPARRAY)rowp, 1);
  }
  void save_markers()
  {
    jpeg_decompress_struct &cinfo = m_jpgd;

    int m;
    for (m = 0; m < 16; m++)
      jpeg_save_markers(&cinfo, JPEG_APP0 + m, 0xFFFF);
  }


  bool mark_fax()
  {
    jpeg_decompress_struct &cinfo = m_jpgd;

    jpeg_saved_marker_ptr ptr = cinfo.marker_list;
    while (ptr) {

      if (ptr -> marker == (JPEG_APP0 + 1) && ptr -> data_length > 5)
      {
        JOCTET FAR* data = ptr -> data;

        if (GETJOCTET(data[0]) == 0x47 &&
            GETJOCTET(data[1]) == 0x33 &&
            GETJOCTET(data[2]) == 0x46 &&
            GETJOCTET(data[3]) == 0x41 &&
            GETJOCTET(data[4]) == 0x58)
          return true;
      }

      ptr = ptr -> next;
    }
    return false;
  }

  bool mark_ps()
  { /* APP13 */
    jpeg_decompress_struct &cinfo = m_jpgd;

    jpeg_saved_marker_ptr ptr = cinfo.marker_list;
    while (ptr) {

      if (ptr -> marker == (JPEG_APP0 + 13) && ptr -> data_length > 9)
      {    
        JOCTET FAR* data = ptr -> data;    

        if(GETJOCTET(data[0]) == 0x50 &&
            GETJOCTET(data[1]) == 0x68 &&
            GETJOCTET(data[2]) == 0x6F &&
            GETJOCTET(data[3]) == 0x74 &&
            GETJOCTET(data[4]) == 0x6F &&
            GETJOCTET(data[5]) == 0x73 &&
            GETJOCTET(data[6]) == 0x68 &&
            GETJOCTET(data[7]) == 0x6F &&
            GETJOCTET(data[8]) == 0x70) {
          double xdpi, ydpi;

          if (process_photoshop(data, ptr -> data_length, &xdpi, &ydpi)) {
            cinfo.X_density = (UINT16) xdpi;
            cinfo.Y_density = (UINT16) ydpi;

            /*
             * Set the density unit to 1 since the
             * Vertical and Horizontal resolution
             * are specified in pixels per inch
             */
            m_jpgd.density_unit = 1;
          }
          return true;
        }
      }

      ptr = ptr -> next;
    }

    return false;    
  }

  bool read_exif(Exif *ex)
  {
    jpeg_decompress_struct &cinfo = m_jpgd;

    jpeg_saved_marker_ptr ptr = cinfo.marker_list;
    while (ptr) {
      if (ptr -> marker == (JPEG_APP0 + 1) && ptr -> data_length > 5)
      {
        JOCTET FAR* data = ptr -> data;
        if (GETJOCTET(data[0]) == 0x45 &&
            GETJOCTET(data[1]) == 0x78 &&
            GETJOCTET(data[2]) == 0x69 &&
            GETJOCTET(data[3]) == 0x66 &&
            GETJOCTET(data[4]) == 0 &&
            GETJOCTET(data[5]) == 0)
          return process_exif(data + 6 , ptr -> data_length - 6 , ex);
      }
      ptr = ptr -> next;
    }
    return false;
  }


  int read_icc(Buffer *buf)
  {
    jpeg_decompress_struct &cinfo = m_jpgd;
    jpeg_saved_marker_ptr marker;
    int num_markers = 0;
    int seq_no;
    JOCTET *icc_data;
    unsigned int total_length =0;
    const int MAX_SEQ_NO = 255;   /*  sufficient since marker numbers are bytes */
    char marker_present[MAX_SEQ_NO+1];    /*  1 if marker found */
    unsigned int data_length[MAX_SEQ_NO+1]; /*  size of profile data in marker */
    unsigned int data_offset[MAX_SEQ_NO+1]; /*  offset for data in marker */

    /*  This first pass over the saved markers discovers whether there are
     * any ICC markers and verifies the consistency of the marker numbering.
     */

    for (seq_no = 1; seq_no <= MAX_SEQ_NO; seq_no++)
      marker_present[seq_no] = 0;

    for (marker = cinfo.marker_list; marker != NULL; marker = marker->next) {
      if (marker_is_icc(marker)) {
        if (num_markers == 0)
          num_markers = GETJOCTET(marker->data[13]);
        else if (num_markers != GETJOCTET(marker->data[13]))
          return 0;   /*  inconsistent num_markers fields */
        seq_no = GETJOCTET(marker->data[12]);
        if (seq_no <= 0 || seq_no > num_markers)
          return 0;   /*  bogus sequence number */
        if (marker_present[seq_no])
          return 0;   /*  duplicate sequence numbers */
        marker_present[seq_no] = 1;
        data_length[seq_no] = marker->data_length - ICC_OVERHEAD_LEN;
      }
    }
    if (num_markers == 0) return 0;
    
    /*  Check for missing markers, count total space needed,
     *  compute offset of each marker's part of the data.
     */

    total_length = 0;
    for (seq_no = 1; seq_no <= num_markers; seq_no++) {
      if (marker_present[seq_no] == 0)
        return 0;   /*  missing sequence number */
      data_offset[seq_no] = total_length;
      total_length += data_length[seq_no];
    }

    if (total_length <= 0)
      return 0;   /*  found only empty markers? */

    if (!buf) return total_length;

    /*  Allocate space for assembled data */
    buf->reset();
    buf->expand(total_length);

    icc_data = (JOCTET *)buf->rp();

    /*  and fill it in */
    for (marker = cinfo.marker_list; marker != NULL; marker = marker->next) {
      if (marker_is_icc(marker)) {
        JOCTET FAR *src_ptr;
        JOCTET *dst_ptr;
        unsigned int length;
        seq_no = GETJOCTET(marker->data[12]);
        dst_ptr = icc_data + data_offset[seq_no];
        src_ptr = marker->data + ICC_OVERHEAD_LEN;
        length = data_length[seq_no];
        while (length--) {
          *dst_ptr++ = *src_ptr++;
        }
      }
    }
    buf->wp(total_length);

    return total_length;
  }

  pix_cs get_out_color_space() const
  {
    const jpeg_decompress_struct &cinfo = m_jpgd;

    return jpg_pix_cs(cinfo.out_color_space);
  }
  void rset_out_color_space(pix_cs cs)
  {
    jpeg_decompress_struct &cinfo = m_jpgd;
    cinfo.out_color_space = jpg_jcs(cs);
  }
  pix_cs get_jpeg_color_space() const
  {
    const jpeg_decompress_struct &cinfo = m_jpgd;
    return jpg_pix_cs(cinfo.jpeg_color_space);
  }
  pix_pos get_image_width() const
  {
    const jpeg_decompress_struct &cinfo = m_jpgd;
    return cinfo.image_width;
  }
  pix_pos get_image_height() const
  {
    const jpeg_decompress_struct &cinfo = m_jpgd;
    return cinfo.image_height;
  }
  pix_pos get_output_height() const
  {
    const jpeg_decompress_struct &cinfo = m_jpgd;
    return cinfo.output_height;
  }
  pix_pos get_output_width() const
  {
    const jpeg_decompress_struct &cinfo = m_jpgd;
    return cinfo.output_width;
  }
  double get_output_gamma() const
  {
    const jpeg_decompress_struct &cinfo = m_jpgd;
    return cinfo.output_gamma;
  }

  void rset_scale(int v)
  {
    jpeg_decompress_struct &cinfo = m_jpgd;
    cinfo.scale_num = 1;
    cinfo.scale_denom = v;
  }
  pix_unit get_density_unit() const
  {
    const jpeg_decompress_struct &cinfo = m_jpgd;
    return jpg_pix_unit(cinfo.density_unit);
  }
  void rset_density_unit(pix_unit unit)
  {
    jpeg_decompress_struct &cinfo = m_jpgd;
    cinfo.density_unit = jpg_unit(unit);
  }
  double get_x_density() const
  {
    const jpeg_decompress_struct &cinfo = m_jpgd;
    return cinfo.X_density;
  }
  void rset_x_density(double x)
  {
    jpeg_decompress_struct &cinfo = m_jpgd;
    cinfo.X_density = (UINT16) x;
  }
  double get_y_density() const
  {
    const jpeg_decompress_struct &cinfo = m_jpgd;
    return cinfo.Y_density;
  }
  void rset_y_density(double y)
  {
    jpeg_decompress_struct &cinfo = m_jpgd;
    cinfo.Y_density = (UINT16) y;
  }

public: /* Writer */
  void param(const pix_img_pm & pm)
  {
    m_pm = pm;
  }
  bool setwparam()
  {
    /* TODO other cs */
    if (m_pm.cs != PIX_CS_RGB ) return false;
    m_pm.type = PIX_IMG_JPG;
    m_pm.jpg.quality = m_quality;
    m_pm.jpg.fax = 0;
    m_pm.jpg.ps = 0;
    m_pm.jpg.exif = 0;
    m_pm.jpg.icc = 0;
    if (m_pm.cs == PIX_CS_RGB) m_pm.jpg.cs = PIX_CS_YCbCr;
    else m_pm.jpg.cs = m_pm.cs;

    set_image_width(m_pm.cols);
    set_image_height(m_pm.rows);
    set_in_color_space(m_pm.cs);
    set_jpeg_color_space(m_pm.jpg.cs);
    set_defaults();
    set_color_space(m_pm.jpg.cs);
    set_input_components(3);
    set_density_unit(m_pm.unit);
    set_x_density(m_pm.hres);
    set_y_density(m_pm.vres);
    set_quality(m_pm.jpg.quality, true);
    return true;
  }
  bool start_compress()
  {
    if (m_pm.cols == 0 || m_pm.rows == 0) return false;
    if (m_pm.cols > 65500 || m_pm.rows > 65500) {
      pix_log_error("jpg: out of range due to jpeglib limit", PIX_ERR_JPG);
      return false;
    }
    if (!setwparam()) return false;

    jpeg_compress_struct &cinfo = m_jpgc;
    jpeg_start_compress(&cinfo, TRUE);
    return true;
  }
  int write_scanline(const char *buf)
  {
    jpeg_compress_struct &cinfo = m_jpgc;
    if (setjmp(m_err.jmp)) {
      return 0;
    }
    return jpeg_write_scanlines(&cinfo, (JSAMPARRAY)&buf, 1);
  }

  bool write_icc_profile(char *buf, int size)
  {
    jpeg_compress_struct &cinfo = m_jpgc;

    if (setjmp(m_err.jmp)) {
      return false;
    }
    /*
     * This routine writes the given ICC profile data into a JPEG file.
     * It *must* be called AFTER calling jpeg_start_compress() and BEFORE
     * the first call to jpeg_write_scanlines().
     * (This ordering ensures that the APP2 marker(s) will appear after the
     * SOI and JFIF or Adobe markers, but before all else.)
     */
    JOCTET *icc_data_ptr = (JOCTET *)buf;
    unsigned int icc_data_len = size;
    unsigned int num_markers;  /*  total number of markers we'll write */
    int cur_marker = 1;    /*  per spec, counting starts at 1 */
    unsigned int length;   /*  number of bytes to write in this marker */

    /*  Calculate the number of markers we'll need, rounding up of course */
    num_markers = icc_data_len / MAX_DATA_BYTES_IN_MARKER;
    if (num_markers * MAX_DATA_BYTES_IN_MARKER != icc_data_len)
      num_markers++;
    while (icc_data_len > 0) {
      /*  length of profile to put in this marker */
      length = icc_data_len;
      if (length > MAX_DATA_BYTES_IN_MARKER)
        length = MAX_DATA_BYTES_IN_MARKER;
      icc_data_len -= length;

      /*  Write the JPEG marker header (APP2 code and marker length) */
      jpeg_write_m_header(&cinfo, ICC_MARKER,
          (unsigned int) (length + ICC_OVERHEAD_LEN));
      /* Write the marker identifying string "ICC_PROFILE" (null-terminated).
       * We code it in this less-than-transparent way so that the code works
       * even if the local character set is not ASCII.
       */

      jpeg_write_m_byte(&cinfo, 0x49);
      jpeg_write_m_byte(&cinfo, 0x43);
      jpeg_write_m_byte(&cinfo, 0x43);
      jpeg_write_m_byte(&cinfo, 0x5F);
      jpeg_write_m_byte(&cinfo, 0x50);
      jpeg_write_m_byte(&cinfo, 0x52);
      jpeg_write_m_byte(&cinfo, 0x4F);
      jpeg_write_m_byte(&cinfo, 0x46);
      jpeg_write_m_byte(&cinfo, 0x49);
      jpeg_write_m_byte(&cinfo, 0x4C);
      jpeg_write_m_byte(&cinfo, 0x45);
      jpeg_write_m_byte(&cinfo, 0x0);

      /*  Add the sequencing info */

      jpeg_write_m_byte(&cinfo, cur_marker);
      jpeg_write_m_byte(&cinfo, (int) num_markers);

      /*  Add the profile data */
      while (length--) {
        jpeg_write_m_byte(&cinfo, *icc_data_ptr);
        icc_data_ptr++;
      }
      cur_marker++;
    }

    return true;
  }


     	

  void set_image_width(pix_pos v)
  {
    jpeg_compress_struct &cinfo = m_jpgc;
    cinfo.image_width = v;
  }
  void set_image_height(pix_pos v)
  {
    jpeg_compress_struct &cinfo = m_jpgc;
    cinfo.image_height = v;
  }
  void set_in_color_space(pix_cs cs)
  {
    jpeg_compress_struct &cinfo = m_jpgc;
    cinfo.in_color_space = jpg_jcs(cs);
  }
  void set_jpeg_color_space(pix_cs cs)
  {
    jpeg_compress_struct &cinfo = m_jpgc;
    cinfo.jpeg_color_space = jpg_jcs(cs);
  }
  void set_input_components(int v)
  {
    jpeg_compress_struct &cinfo = m_jpgc;
    cinfo.input_components = v;

  }
  void set_num_components(int v)
  {
    jpeg_compress_struct &cinfo = m_jpgc;
    cinfo.num_components = v;
  }
  void set_defaults()
  {
    jpeg_compress_struct &cinfo = m_jpgc;
    jpeg_set_defaults(&cinfo);
  }
  void set_color_space(pix_cs cs)
  {
    jpeg_compress_struct &cinfo = m_jpgc;
    jpeg_set_colorspace(&cinfo, jpg_jcs(cs));
  }
  void set_quality(int qual, bool baseline = true)
  {
    jpeg_compress_struct &cinfo = m_jpgc;
    jpeg_set_quality(&cinfo, qual, baseline);
    // avoid subsampling on high quality factor
    if (qual >= 70) {
      int i;
      for (i = 0; i < cinfo.num_components; i++) {
        m_jpgc.comp_info[i].h_samp_factor = 1;
        m_jpgc.comp_info[i].v_samp_factor = 1;
      }
    }
  }

  void set_density_unit(pix_unit u)
  {
    jpeg_compress_struct &cinfo = m_jpgc;
    int t = 0;
    if (u == PIX_UNIT_INCH) t = 1;
    else if (u == PIX_UNIT_METER) t = 2;
    cinfo.density_unit = t;
  }
  void set_write_jfif(bool b)
  {
    jpeg_compress_struct &cinfo = m_jpgc;
    cinfo.write_JFIF_header = b;
  }
  void set_write_adobe(bool b)
  {
    jpeg_compress_struct &cinfo = m_jpgc;
    cinfo.write_Adobe_marker = b;
  }
  void set_x_density(double v)
  {
    jpeg_compress_struct &cinfo = m_jpgc;
    cinfo.X_density = (UINT16) v;
  }
  void set_y_density(double v)
  {
    jpeg_compress_struct &cinfo = m_jpgc;
    cinfo.Y_density = (UINT16) v;
  }






};





PIXIMPL_END
PIXAPI_CPP_END

#endif

