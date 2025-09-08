/**
 * @file lv_draw_buf_convert_neon.h
 *
 */

#ifndef LV_DRAW_BUF_CONVERT_NEON_H
#define LV_DRAW_BUF_CONVERT_NEON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../misc/lv_color.h"
#include "../../lv_draw_buf.h"

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

#ifndef LV_DRAW_CONVERT_PREMULTIPLY_INDEXED
#define LV_DRAW_CONVERT_PREMULTIPLY_INDEXED(buf) \
    _lv_draw_buf_convert_premultiply_indexed_neon(buf)
#endif

#ifndef LV_DRAW_CONVERT_PREMULTIPLY_ARGB8888
#define LV_DRAW_CONVERT_PREMULTIPLY_ARGB8888(buf) \
    _lv_draw_buf_convert_premultiply_argb8888_neon(buf)
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Convert indexed draw_buf to premultiplied format with neon specific optimizations
 * @param buf     pointer to a draw buf
 */

lv_result_t _lv_draw_buf_convert_premultiply_indexed_neon(lv_draw_buf_t * buf);

/**
 * Convert argb8888 draw_buf to premultiplied format with neon specific optimizations
 * @param buf     pointer to a draw buf
 */
lv_result_t _lv_draw_buf_convert_premultiply_argb8888_neon(lv_draw_buf_t * buf);
#endif /*LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_DRAW_BUF_CONVERT_NEON_H */
