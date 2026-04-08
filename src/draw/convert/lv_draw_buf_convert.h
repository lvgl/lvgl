/**
 * @file lv_draw_buf_convert.h
 *
 */

#ifndef LV_DRAW_BUF_CONVERT_H
#define LV_DRAW_BUF_CONVERT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../misc/lv_color.h"
#include "../lv_draw_buf.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Convert draw_buf  to premultiplied format
 * @param buf     pointer to a draw buf
 */
lv_result_t lv_draw_buf_convert_premultiply(lv_draw_buf_t * buf);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_DRAW_BUF_CONVERT_H */
