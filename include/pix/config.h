/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  pix/config.h
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *        License:  LGPL
 *
 *    Description:  basic types, constants, and configurations
 *
 * =============================================================
 */

/**
 * @file
 * @brief %basic types, constants and configurations.
 */

#ifndef __PIX_CONFIG_H__
#define __PIX_CONFIG_H__

#include <stddef.h>

/* ############################ Language ######################### */
#ifndef __cplusplus
/* --------------------- Pure C ------------------------  */
#define PIXAPI_C
#define PIXAPI_C_BEG
#define PIXAPI_C_END
#else
/* ---------------------- C++ ---------------------------- */
#define PIXAPI_C
#define PIXAPI_CPP
#define PIXAPI_C_BEG extern "C" {
#define PIXAPI_C_END }
#define PIXAPI_CPP_BEG namespace Pix {
#define PIXAPI_CPP_END }
#endif /* __cplusplus */

PIXAPI_C_BEG
/* ############################## OS ############################ */

#ifndef _WIN32
/* ------------------------ POSIX -------------------- */
#define PIX_POSIX
#define PIXAPI
#else
/* ----------------------- WINDOWS ------------------- */
#define PIX_WIN
#define PIXAPI __stdcall
#endif /* _WIN32 */

/* ########################### Compiler ########################## */
#ifndef _MSC_VER
/* ------------------------ GNU ------------------------ */
#define PIX_GNU
#include <libpix.config.h>
/* 8-bit type */
typedef char pix_int8;
typedef unsigned char pix_uint8;
/* 16-bit type */
typedef short pix_int16;
typedef unsigned short pix_uint16;
/* 32-bit type */
typedef int pix_int32;
typedef unsigned int pix_uint32;
/* 64-bit type */
typedef long long pix_int64;
typedef unsigned long long pix_uint64;

#else
/* ------------------------ VC -------------------------- */
#define PIX_VC

#define PIXAPI __stdcall
/* Multiple assignment operators specified. Yes, we're sure we want it */
#pragma warning(disable: 4522)
/* data convertion warning, usually in 64-bit */
#pragma warning(disable: 4244)
/* encoding warning */
#pragma warning(disable: 4819)

/* 8-bit type */
typedef __int8 pix_int8;
typedef unsigned __int8 pix_uint8;
/* 16-bit type */
typedef __int16 pix_int16;
typedef unsigned __int16 pix_uint16;
/* 32-bit type */
typedef __int32 pix_int32;
typedef unsigned __int32 pix_uint32;
/* 64-bit type */
typedef __int64 pix_int64;
typedef unsigned __int64 pix_uint64;

#endif /* _MSC_VER */

/**
 * @defgroup type Type
 * @brief %basic types and constants
 * @{
 */

/* ######################### Types and Const ######################### */

typedef pix_int8   pix_char;   /**< @brief %char type, 8 bit.*/
typedef pix_uint8  pix_uchar;  /**< @brief %unsigned char type, 8 bit. */
typedef pix_int16  pix_short;  /**< @brief %short int type, 16 bit. */
typedef pix_uint16 pix_ushort; /**< @brief %unsigned short int type, 16 bit. */
typedef pix_int32  pix_int;    /**< @brief %int type, 32 bit. */
typedef pix_uint32 pix_uint;   /**< @brief %unsigned int type, 32 bit. */
typedef pix_int64  pix_long;   /**< @brief %long int type, 64 bit. */
typedef pix_uint64 pix_ulong;  /**< @brief %unsigned long int type, 64 bit. */
typedef pix_long   pix_pos;    /**< @brief %position type, must be signed. */
typedef pix_long   pix_len;    /**< @brief %length type, must be signed.*/
typedef wchar_t    pix_wchar;  /**< @brief %wide char type, same as wchar_t. */
typedef pix_ushort pix_ucs2;   /**< @brief %ucs2 char type, 2 byte unicode. */
typedef pix_uint   pix_ucs4;   /**< @brief %ucs4 char type, 4 byte unicode. */

#define PIX_LF     "\n"
#define PIX_CR     "\r"
#define PIX_CRLF   "\r\n"
#define PIX_EOL    "|\r\n|\n|\r"
#define PIX_DELIM  "| |\v|\t|\r\n|\n|\r"

/** @brief %radix type*/
typedef enum {
  PIX_RADIX_AUTO,
  PIX_RADIX_DEC,
  PIX_RADIX_HEX,
  PIX_RADIX_OCT,
  PIX_RADIX_BIN,
} pix_radix;

/** @brief %encoding type*/
typedef enum {
  PIX_ENC = 0,              /* Binary bytes */
  PIX_ENC_AUTO = 1,         /* Use default or same with original */
  PIX_ENC_EBCDIC,           /* IBM EBCDIC */
  /* ANSI compatible */
  PIX_ENC_ANSI = 0x000101,  /* ISO 646 ECMA-6*/
  PIX_ENC_LATIN1,           /* ISO 8859-1  Western European */
  PIX_ENC_LATIN2,           /* ISO 8859-2  Central European */
  PIX_ENC_LATIN3,           /* ISO 8859-3  South European */
  PIX_ENC_LATIN4,           /* ISO 8859-4  North European */
  PIX_ENC_CYRILLIC,         /* ISO 8859-5  Cyrillic */
  PIX_ENC_ARABIC,           /* ISO 8859-6  Arabic */
  PIX_ENC_GREEK,            /* ISO 8859-7  Greek */
  PIX_ENC_HEBREW,           /* ISO 8859-8  Hebrew */
  PIX_ENC_LATIN5,           /* ISO 8859-9  Turkish */
  PIX_ENC_LATIN6,           /* ISO 8859-10 Nordic */
  PIX_ENC_THAI,             /* ISO 8859-11 Thai */
  PIX_ENC_LATIN7,           /* ISO 8859-13 Baltic Rim */
  PIX_ENC_LATIN8,           /* ISO 8859-14 Celtic */
  PIX_ENC_LATIN9,           /* ISO 8859-15  */
  PIX_ENC_LATIN10,          /* ISO 8859-16  */
  PIX_ENC_GB2312,           /* Simplified Chinese */
  PIX_ENC_GBK,              /* CP936 Simplified Chinese */
  PIX_ENC_GB18030,          /* Simplified Chinese */
  PIX_ENC_BIG5,             /* CP950 Traditional Chinese */
  PIX_ENC_JIS,              /* Japanese */
  PIX_ENC_SJIS,             /* CP932 Shift-JIS Japanese */
  PIX_ENC_KOREAN,           /* CP949 Korean */
  PIX_ENC_VIETNAM,          /* CP1258 Vietnam */
  PIX_ENC_VISCII,           /* Vietnam */ 
  PIX_ENC_EUCCN,
  PIX_ENC_EUCJP,
  PIX_ENC_EUCKR,
  PIX_ENC_EUCTW,
  /* ISO 10646 Unicode */
  PIX_ENC_UTF8 = 0x001001, /* UTF-8 Compatible with ANSI */ 
  PIX_ENC_UTF7,
  PIX_ENC_UTF16LE,         /* UTF-16LE Windows WCHAR*/
  PIX_ENC_UTF16BE,         /* UTF-16BE */
  PIX_ENC_UTF32LE,         /* UTF-32LE x86 Linux wchar_t */
  PIX_ENC_UTF32BE,         /* UTF-32BE */
} pix_enc;

/* @brief %escape type */
typedef enum {
  PIX_ESC = 0,
  PIX_ESC_C,
  PIX_ESC_OCT,
  PIX_ESC_HEX,
  PIX_ESC_DOT,
} pix_esc;

/** @brief %align type */
typedef enum {
  PIX_ALIGN = 0,
  PIX_ALIGN_LEFT,
  PIX_ALIGN_RIGHT,
  PIX_ALIGN_CENTER,
  PIX_ALIGN_AUTO,
} pix_align;

/** @brief %seek whence */
typedef enum {
  PIX_SEEK_SET = 0,
  PIX_SEEK_CUR = 1,
  PIX_SEEK_END = 2,
} pix_seek;

/** @brief %stream flags */
typedef enum {
  PIX_STM_READ   = 0x0001,
  PIX_STM_WRITE  = 0x0002,
  PIX_STM_APPEND = 0x0010,
  PIX_STM_CREATE = 0x0020,
  PIX_STM_TRUNC  = 0x0040,
  PIX_STM_RW     = PIX_STM_READ | PIX_STM_WRITE,
  PIX_STM_RWA    = PIX_STM_RW   | PIX_STM_APPEND,
  PIX_STM_RWC    = PIX_STM_RW   | PIX_STM_CREATE,
  PIX_STM_RWT    = PIX_STM_RW   | PIX_STM_TRUNC,
  PIX_STM_RWAC   = PIX_STM_RWA  | PIX_STM_CREATE,
  PIX_STM_RWAT   = PIX_STM_RWA  | PIX_STM_TRUNC,
  PIX_STM_RWCT   = PIX_STM_RWC  | PIX_STM_TRUNC,
  PIX_STM_RWACT  = PIX_STM_RWAC | PIX_STM_TRUNC,
  PIX_STM_WA     = PIX_STM_WRITE | PIX_STM_APPEND,
  PIX_STM_WC     = PIX_STM_WRITE | PIX_STM_CREATE,
  PIX_STM_WT     = PIX_STM_WRITE | PIX_STM_TRUNC,
  PIX_STM_WAC    = PIX_STM_WC    | PIX_STM_APPEND,
  PIX_STM_WAT    = PIX_STM_WA    | PIX_STM_TRUNC,
  PIX_STM_WCT    = PIX_STM_WC    | PIX_STM_TRUNC,
  PIX_STM_WACT   = PIX_STM_WAC   | PIX_STM_TRUNC,
} pix_stm_flag;

/** @brief %color space */
typedef enum {
  PIX_CS = 0,
  PIX_CS_PALETTE,
  PIX_CS_GRAY,
  PIX_CS_RGB,
  PIX_CS_CMY,
  PIX_CS_CMYK,
  PIX_CS_YCbCr,
  PIX_CS_YCCK,
  PIX_CS_YUV,
  PIX_CS_YUVK,
  PIX_CS_XYZ,
  PIX_CS_CIELab,
  PIX_CS_LogL,
  PIX_CS_LogLuv,
  PIX_CS_HSV,
  PIX_CS_HLS,
} pix_cs;


/** @brief %image flags */
typedef enum {
  PIX_IMG_FLAG   = 0,
  PIX_IMG_INVERT = 0x01,
  PIX_IMG_SWAP   = 0x02,
} pix_img_flag;

/** @brief %measurement unit */
typedef enum {
  PIX_UNIT = 0,
  PIX_UNIT_PIXEL ,
  PIX_UNIT_INCH ,
  PIX_UNIT_METER ,
  PIX_UNIT_CM ,
  PIX_UNIT_MM ,
} pix_unit;

/** @brief %compression */
typedef enum {
  PIX_CPRS,
  /* Frequent use compression */
  PIX_CPRS_NONE,
  PIX_CPRS_RLE,
  PIX_CPRS_RLEW,
  PIX_CPRS_FAX3,
  PIX_CPRS_FAX4,
  PIX_CPRS_LZW,
  PIX_CPRS_DEFLATE,
  PIX_CPRS_OJPG,
  PIX_CPRS_JPG,
  PIX_CPRS_JBIG,
  PIX_CPRS_JPG2K,
  /* Other tiff compression  */
  PIX_CPRS_ADOBE,
  PIX_CPRS_NEXT,
  PIX_CPRS_PACKBITS,
  PIX_CPRS_THUNDERSCAN,
  PIX_CPRS_IT8CTPAD,
  PIX_CPRS_IT8LW,
  PIX_CPRS_IT8MP,
  PIX_CPRS_IT8BL,
  PIX_CPRS_PIXARFILM,
  PIX_CPRS_PIXARLOG,
  PIX_CPRS_DCS,
  PIX_CPRS_SGILOG,
  PIX_CPRS_SGILOG24,
} pix_cprs;

/** @brief %orientation */
typedef enum {
  PIX_ORT = 0,
  PIX_ORT_TOPLEFT,
  PIX_ORT_TOPRIGHT,
  PIX_ORT_BOTLEFT,
  PIX_ORT_BOTRIGHT,
  PIX_ORT_LEFTTOP,
  PIX_ORT_LEFTBOT,
  PIX_ORT_RIGHTTOP,
  PIX_ORT_RIGHTBOT,
} pix_ort;

/** @brief %inter-polation type */
typedef enum {
  PIX_POL = 0,
  PIX_POL_NEAREST,
  PIX_POL_BILINEAR,
  PIX_POL_BICUBIC,

} pix_pol;

/** @brief %image type */
typedef enum {
  PIX_IMG = 0,
  PIX_IMG_FLT = 1,
  PIX_IMG_JPG,
  PIX_IMG_TIF,
  PIX_IMG_PNG,
  PIX_IMG_GIF,
  PIX_IMG_BMP,
  PIX_IMG_ICO,
} pix_img_type;

/** @brief %filter type */
typedef enum {
  PIX_FLT = 0,
  PIX_FLT_RESIZE,
  PIX_FLT_ICC,
  /* TODO */
} pix_flt_type;


/** @brief %filter parameter */
typedef struct {
  pix_flt_type type;
  union {
    pix_pol pol; /* interpolation type */
  };

} pix_flt_pm;

/** @brief %jpeg parameter */
typedef struct {
  pix_cs cs;
  int quality;
  char fax;
  char ps;
  char exif;
  int icc;
} pix_jpg_pm;

/** @brief %tiff parameter */
typedef struct {
  pix_cprs compress;
  pix_ort  orient;
  /* TODO */
} pix_tif_pm;

/** @brief %png parameter */
typedef struct {
  /* TODO */
} pix_png_pm;


/** @brief %gif parameter */
typedef struct {
  /* TODO */
} pix_gif_pm;

/** @brief %bmp parameter */
typedef struct {
  /* TODO */
} pix_bmp_pm;

/** @brief %ico parameter */
typedef struct {
  /* TODO */
} pix_ico_pm;

/** @brief %image parameter */
typedef struct {
  pix_cs cs;           /* color space */
  char bits[8];        /* bits of each component */
  pix_pos cols;        /* cols of pixel */
  pix_pos rows;        /* rows of pixel */
  double hres;         /* horizontal resolution */
  double vres;         /* vertical resolution */
  pix_unit unit;       /* resolution unit */
  int flag;            /* flags */
  pix_img_type type;
  union {
    pix_flt_pm flt;
    pix_jpg_pm jpg;
    pix_tif_pm tif;
    pix_png_pm png;
    pix_gif_pm gif;
    pix_bmp_pm bmp;
    pix_bmp_pm ico;
  };
} pix_img_pm;

/** @brief %raster operations */
typedef enum {
  PIX_ROP_CPY,
  PIX_ROP_AND,
  PIX_ROP_OR,
  PIX_ROP_XOR,
  PIX_ROP_NOT,
} pix_rop;

/* ########################  Type Functions ######################### */
/** @brief %get character length of an encoding.  */
int
PIXAPI pix_enc_width(int enc);

/** @brief %get max character length of an encoding.  */
int
PIXAPI pix_enc_max_width(int enc);

/** @brief %wchar encoding */
pix_enc
PIXAPI pix_enc_wchar();

/** @brief %use utf-16 to substitute ucs2. */
pix_enc
PIXAPI pix_enc_ucs2();
/** @brief %use utf32 to substitute ucs4. */
pix_enc
PIXAPI pix_enc_ucs4();

/** @brief %init image parameter. */
void 
PIXAPI pix_img_pm_init(pix_img_pm *pm);

/** @brief %init image parameter with color space. */
void 
PIXAPI pix_img_pm_init_cs(pix_img_pm *pm , pix_cs cs);


/* ########################### Constant Chars ########################## */
/** @brief %get radix string. */
const char * 
PIXAPI pix_chars_radix(int radix);

/** @brief %get align string. */
const char * 
PIXAPI pix_chars_align(int align);

/** @brief %get escape string. */
const char * 
PIXAPI pix_chars_esc(int esc);

/** @brief %get encoding string. */
const char *
PIXAPI pix_chars_enc(int enc);

/** @brief %get unit string. */
const char *
PIXAPI pix_chars_unit(int unit);

/** @brief %get color space string. */
const char *
PIXAPI pix_chars_cs(int cs);
/**
 * @}
 */


/* ########################## System Info ########################## */

/** @defgroup library Library 
 *  @brief %system config, log and error reporting
 *  @{ */

/** @brief %get library name. */
const char *
PIXAPI pix_sys_name();

/** @brief %get library version. */
const char *
PIXAPI pix_sys_ver();

/** @brief %get endian of the machine. */
int
PIXAPI pix_sys_endian();

/** @brief %get word length of the machine. */
int
PIXAPI pix_sys_word();

/** @brief %get page size of operating system. */
int
PIXAPI pix_sys_page();

/** @brief %get allocation granularity */
int
PIXAPI pix_sys_gran();

/** @brief %get default encoding. */
pix_enc
PIXAPI pix_sys_enc();

/** @brief %get file buf size */
pix_len
PIXAPI pix_sys_fbuf();

/** @brief %get file create umask, default 022 */
pix_uint
PIXAPI pix_sys_umask();

/** @brief %show system info to stdout. */
void
PIXAPI pix_sys_show();



/* ############################ Config ############################# */


/** @brief %set default encoding. */
pix_enc
PIXAPI pix_cfg_enc(pix_enc enc);

/** @brief %set file buffer size. */
int
PIXAPI pix_cfg_fbuf(int size);

/** @brief %set file create umask, default 0022 */
pix_uint
PIXAPI pix_cfg_umask(pix_uint umask);

/** @brief %set log level. */
void
PIXAPI pix_cfg_log(int level);

/**
 * @brief %log handler
 * @param type 1 message 2 warning 3 error
 */
typedef void (*pix_log_func)(const char *msg, int type);

/**
 * @brief %set log handler.
 * @param logfn 0 to use default handler, print messages and 
 * warnings to stdout, print error to stderr.
 */
void
PIXAPI pix_cfg_log_handler(pix_log_func logfn );

/* ############################ Log ############################ */
/** @brief %error code */
typedef enum {
  PIX_ERR_OK = 0,
  PIX_ERR_OVERFLOW,
  PIX_ERR_FILE,
  PIX_ERR_SEEK,
  PIX_ERR_ICONV,
  PIX_ERR_JPG,
  PIX_ERR_TIF,
  PIX_ERR_PNG,
  PIX_ERR_WIN

} pix_err;

/** 
 * @brief %get log level.
 * @ret
 * level 0 disable log
 * level 1 show error
 * level 2 show error, warning
 * level 3 show everything
 */
int
PIXAPI pix_log();

/**
 * @brief %log a message.
 * 
 */
void
PIXAPI pix_log_msg(const char *msg);

/**
 * @brief %log a warning.
 */
void
PIXAPI pix_log_warn(const char *msg);

/**
 * @brief %log an error.
 * TODO: errno
 * For now, this function uses the ansi c errno because we dont't 
 * want to deal with thread-safe problem by ourself at present.
 * So, using this library may affect errno.
 * Update later.
 *
 */
void
PIXAPI pix_log_error(const char *msg, int errid );

/**
 * @brief %get last error code.
 */
int
PIXAPI pix_log_errid();

/**
 * @brief %clear the error code.
 */
void
PIXAPI pix_log_clear();

/** @} */

PIXAPI_C_END

#endif /* __PIX_CONFIG_HPP__ */

/**
 * @namespace Pix
 * @brief %libpix classes and functions
 */

/**
 * @namespace Pix::Win
 * @brief %useful function and class for winows.
 * 
 */

