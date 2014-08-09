/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  libpix.h
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *        License:  LGPL
 *
 *    Description:  libpix header
 *
 * =============================================================
 */

/**
 * @file
 * @brief %libpix header
 */

#ifndef __LIBPIX_H__
#define __LIBPIX_H__

#include <pix/config.h>

#ifdef PIXAPI_C

PIXAPI_C_BEG
/**
 * @defgroup capi CAPI
 * @brief C API functions
 * @{
 */
typedef struct _pix_buf * pix_buf;

typedef struct _pix_str * pix_str;

typedef struct _pix_stm * pix_stm;

typedef struct _pix_img * pix_img;

typedef struct _pix_inf * pix_inf;

/* Buffer */

pix_buf
PIXAPI pix_buf_create();

void
PIXAPI pix_buf_destroy(pix_buf *buf);

void 
PIXAPI pix_buf_resize(pix_buf buf, pix_len size);

void 
PIXAPI pix_buf_expand(pix_buf buf, pix_len wsize);

void 
PIXAPI pix_buf_move(pix_buf buf, pix_pos pos, pix_len wsize);

void 
PIXAPI pix_buf_release(pix_buf buf);

char *
PIXAPI pix_buf_bp(pix_buf buf);

char *
PIXAPI pix_buf_rp(pix_buf buf);

char *
PIXAPI pix_buf_wp(pix_buf buf);

char *
PIXAPI pix_buf_ep(pix_buf buf);

char *
PIXAPI pix_buf_bp_set(pix_buf buf, char *bp);

char *
PIXAPI pix_buf_rp_set(pix_buf buf, char *rp);

char *
PIXAPI pix_buf_wp_set(pix_buf buf, char *wp);

char *
PIXAPI pix_buf_ep_set(pix_buf buf, char *ep);

char *
PIXAPI pix_buf_rp_move(pix_buf buf, int off);

char *
PIXAPI pix_buf_wp_move(pix_buf buf, int off);

void 
PIXAPI pix_buf_reset(pix_buf buf);

void 
PIXAPI pix_buf_r_reset(pix_buf buf);

void 
PIXAPI pix_buf_w_reset(pix_buf buf);

pix_pos 
PIXAPI pix_buf_r_pos(pix_buf buf);

pix_pos 
PIXAPI pix_buf_w_pos(pix_buf buf);

pix_len 
PIXAPI pix_buf_avail(pix_buf buf);

pix_len 
PIXAPI pix_buf_p_avail(pix_buf buf);

pix_len 
PIXAPI pix_buf_r_avail(pix_buf buf);

pix_len 
PIXAPI pix_buf_w_avail(pix_buf buf);

pix_len 
PIXAPI pix_buf_length(pix_buf buf);

pix_len 
PIXAPI pix_buf_size(pix_buf buf);

void 
PIXAPI pix_buf_swap(pix_buf buf1, pix_buf buf2);


/* String */
pix_str
PIXAPI pix_str_create();

void
PIXAPI pix_str_destroy(pix_str *str);

void 
PIXAPI pix_str_swap(pix_str str1, pix_str str2);

void 
PIXAPI pix_str_swap_buf(pix_str str, pix_buf buf);

pix_len 
PIXAPI pix_str_length(pix_str str);

pix_len 
PIXAPI pix_str_size(pix_str str);

int 
PIXAPI pix_str_equal(pix_str str, pix_str str2);

int 
PIXAPI pix_str_empty(pix_str str);

const char * 
PIXAPI pix_str_chars(pix_str str);

void 
PIXAPI pix_str_clear(pix_str str);

void 
PIXAPI pix_str_assign(pix_str str, pix_str src);

void 
PIXAPI pix_str_assign_char(pix_str str, 
    char ch, pix_len count);

void 
PIXAPI pix_str_assign_chars(pix_str str, 
    const char *buf, pix_len len, int encode);

void 
PIXAPI pix_str_append(pix_str str, pix_str sub);

void 
PIXAPI pix_str_append_char(pix_str str, 
    char ch, pix_len count);

void 
PIXAPI pix_str_append_chars(pix_str str, 
    const char *buf, pix_len len, int encode);

void 
PIXAPI pix_str_insert(pix_str str, pix_pos pos,
    pix_str sub);

void 
PIXAPI pix_str_insert_char(pix_str str, pix_pos pos,
    char ch, int count);

void 
PIXAPI pix_str_insert_chars(pix_str str, pix_pos pos, 
    const char *buf, pix_len len, int encode);

void 
PIXAPI pix_str_replace(pix_str str, pix_pos beg, pix_pos end, 
    pix_str sub);

void 
PIXAPI pix_str_replace_char(pix_str str, pix_pos beg, pix_pos end, 
    char ch, int count);

void 
PIXAPI pix_str_replace_chars(pix_str str, pix_pos beg, pix_pos end, 
    const char *buf, pix_len len, int encode);

pix_pos 
PIXAPI pix_str_find(pix_str str, 
    pix_str sub, pix_pos beg, pix_pos end);

pix_pos 
PIXAPI pix_str_find_char(pix_str str,
    char ch, pix_len count, pix_pos beg, pix_pos end);

pix_pos 
PIXAPI pix_str_find_chars(pix_str str, 
    const char *buf, pix_len len, pix_pos beg, pix_pos end);

void 
PIXAPI pix_str_remove(pix_str str, pix_pos beg, pix_pos end);

void 
PIXAPI pix_str_sub(pix_str str, pix_pos beg, pix_pos end);

void 
PIXAPI pix_str_trim(pix_str str);

void 
PIXAPI pix_str_reverse(pix_str str);

void 
PIXAPI pix_str_repeat(pix_str str, pix_len count);

int 
PIXAPI pix_str_keep(pix_str str);

int 
PIXAPI pix_str_keep_set(pix_str str, pix_len val);

void 
PIXAPI pix_str_get_sub(pix_str str, pix_str result, pix_pos beg, pix_pos end);

pix_pos 
PIXAPI pix_str_get_line(pix_str str, pix_str result, pix_pos pos, const char *eol);

void 
PIXAPI pix_str_get_indent(pix_str str, pix_str result, pix_len count, const char *indent, const char *neweol, const char *eol);

void 
PIXAPI pix_str_get_escape(pix_str str, pix_str result, int escape);

void 
PIXAPI pix_str_int(pix_str str, int val);

void 
PIXAPI pix_str_long(pix_str str, long val);

void 
PIXAPI pix_str_float(pix_str str, double val);

void 
PIXAPI pix_str_inf(pix_str str, pix_inf info);


/* Stream */
pix_stm
PIXAPI pix_stm_create();

void 
PIXAPI pix_stm_destroy(pix_stm *stm);

int 
PIXAPI pix_stm_open_file(pix_stm stm, const char *path, int flag);

int 
PIXAPI pix_stm_open_mem(pix_stm stm, int flag);

int 
PIXAPI pix_stm_close(pix_stm stm);

int 
PIXAPI pix_stm_eof(pix_stm stm);

pix_pos 
PIXAPI pix_stm_seek(pix_stm stm, pix_pos pos, int whence);

pix_pos 
PIXAPI pix_stm_tell(pix_stm stm);

pix_len 
PIXAPI pix_stm_length(pix_stm stm);

pix_len 
PIXAPI pix_stm_trunc(pix_stm stm, pix_len len);

pix_len 
PIXAPI pix_stm_read(pix_stm stm, char * buf, pix_len len);

pix_len 
PIXAPI pix_stm_write(pix_stm stm, const char *buf, pix_len len);

const char * 
PIXAPI pix_stm_name(pix_stm stm);

int 
PIXAPI pix_stm_flag_set(pix_stm stm, int flag);

int 
PIXAPI pix_stm_flag_get(pix_stm stm);

int 
PIXAPI pix_stm_is_read(pix_stm stm);

int 
PIXAPI pix_stm_is_write(pix_stm stm);

int 
PIXAPI pix_stm_is_append(pix_stm stm);

int 
PIXAPI pix_stm_mem_swap(pix_stm stm, pix_buf buf);

int 
PIXAPI pix_stm_mem_reserve(pix_stm stm, int len);

int 
PIXAPI pix_stm_mem_size(pix_stm stm);



/* Info */
pix_inf
PIXAPI pix_inf_create();

void 
PIXAPI pix_inf_destroy(pix_inf *inf);

int 
PIXAPI pix_inf_empty(pix_inf inf);

void 
PIXAPI pix_inf_new_node(pix_inf inf);

void 
PIXAPI pix_inf_new_node_content(pix_inf inf, pix_str content); 

void 
PIXAPI pix_inf_new_child(pix_inf inf);

void 
PIXAPI pix_inf_new_child_content(pix_inf inf, pix_str content);

void 
PIXAPI pix_inf_top(pix_inf inf);

void 
PIXAPI pix_inf_first(pix_inf inf);

int 
PIXAPI pix_inf_child(pix_inf inf);

int 
PIXAPI pix_inf_parent(pix_inf inf);

int 
PIXAPI pix_inf_next(pix_inf inf);

int 
PIXAPI pix_inf_prev(pix_inf inf);

void 
PIXAPI pix_inf_data_set(pix_inf inf, pix_str key, pix_str val);

int 
PIXAPI pix_inf_data_get(pix_inf inf, pix_str key, pix_str val);

int 
PIXAPI pix_inf_data_first(pix_inf inf, pix_str key , pix_str val);

int 
PIXAPI pix_inf_data_next(pix_inf inf, pix_str key, pix_str val);

void 
PIXAPI pix_inf_content_set(pix_inf inf, pix_str content);

int 
PIXAPI pix_inf_content_get(pix_inf inf, pix_str content);

const char *
PIXAPI pix_inf_chars(pix_inf inf);

void 
PIXAPI pix_inf_push_str(pix_inf inf, pix_str str);

void 
PIXAPI pix_inf_push_stm(pix_inf inf, pix_stm stm, int width, int row);

/**
 * @}
 */

PIXAPI_C_END

#endif /* PIXAPI_C */

#ifdef PIXAPI_CPP

#include <pix/utils.hpp>
#include <pix/memory.hpp>
#include <pix/string.hpp>
#include <pix/info.hpp>
#include <pix/stream.hpp>
#include <pix/image.hpp>
#include <pix/image_jpg.hpp>
#include <pix/filter_resize.hpp>
#include <pix/system.hpp>
#include <pix/view.hpp>


#ifdef PIXAPI_C /* C++ function using C interface */
pix_buf pix_buf_create(Pix::Buffer &buffer );
pix_str pix_str_create(Pix::String &string);
pix_stm pix_stm_create(Pix::FileStream &fstm);
pix_stm pix_stm_create(Pix::MemStream &mstm);
pix_inf pix_inf_create(Pix::Info &info);
#endif /* PIXAPI_C */
#endif /* PIXAPI_CPP */


#endif

