/**
 * @file lv_api_map_v9_3.h
 *
 */

#ifndef LV_API_MAP_V9_3_H
#define LV_API_MAP_V9_3_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#define lv_draw_buf_malloc_cb               lv_draw_buf_malloc_cb_t
#define lv_draw_buf_free_cb                 lv_draw_buf_free_cb_t
#define lv_draw_buf_copy_cb                 lv_draw_buf_copy_cb_t
#define lv_draw_buf_align_cb                lv_draw_buf_align_cb_t
#define lv_draw_buf_cache_operation_cb      lv_draw_buf_cache_operation_cb_t
#define lv_draw_buf_width_to_stride_cb      lv_draw_buf_width_to_stride_cb_t


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_API_MAP_V9_3_H */
