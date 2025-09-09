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

#define lv_glfw_window_t                 lv_opengles_window_t
#define lv_glfw_texture_t                lv_opengles_window_texture_t
#define lv_glfw_window_create            lv_opengles_glfw_window_create
#define lv_glfw_window_create_ex         lv_opengles_glfw_window_create_ex
#define lv_glfw_window_set_title         lv_opengles_glfw_window_set_title
#define lv_glfw_window_delete            lv_opengles_window_delete
#define lv_glfw_window_set_flip          lv_opengles_glfw_window_set_flip
#define lv_glfw_window_add_texture       lv_opengles_window_add_texture
#define lv_glfw_texture_remove           lv_opengles_window_texture_remove
#define lv_glfw_texture_set_x            lv_opengles_window_texture_set_x
#define lv_glfw_texture_set_y            lv_opengles_window_texture_set_y
#define lv_glfw_texture_set_opa          lv_opengles_window_texture_set_opa
#define lv_glfw_texture_get_mouse_indev  lv_opengles_window_texture_get_mouse_indev
#define lv_glfw_window_get_glfw_window   lv_opengles_glfw_window_get_glfw_window


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_API_MAP_V9_3_H */
