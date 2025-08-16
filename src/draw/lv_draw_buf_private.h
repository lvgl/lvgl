/**
 * @file lv_draw_buf_private.h
 *
 */

#ifndef LV_DRAW_BUF_PRIVATE_H
#define LV_DRAW_BUF_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_buf.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_buf_handlers_t {
    lv_draw_buf_malloc_cb_t buf_malloc_cb;
    lv_draw_buf_free_cb_t buf_free_cb;
    lv_draw_buf_copy_cb_t buf_copy_cb;
    lv_draw_buf_align_cb_t align_pointer_cb;
    lv_draw_buf_cache_operation_cb_t invalidate_cache_cb;
    lv_draw_buf_cache_operation_cb_t flush_cache_cb;
    lv_draw_buf_width_to_stride_cb_t width_to_stride_cb;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Called internally to initialize the draw_buf_handlers in lv_global
 */
void lv_draw_buf_init_handlers(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_BUF_PRIVATE_H*/
