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

#define lv_vector_dsc_create                        lv_draw_vector_dsc_create
#define lv_vector_dsc_delete                        lv_draw_vector_dsc_delete
#define lv_vector_dsc_set_transform                 lv_draw_vector_dsc_set_transform
#define lv_vector_dsc_set_blend_mode                lv_draw_vector_dsc_set_blend_mode
#define lv_vector_dsc_set_fill_color32              lv_draw_vector_dsc_set_fill_color32
#define lv_vector_dsc_set_fill_color                lv_draw_vector_dsc_set_fill_color
#define lv_vector_dsc_set_fill_opa                  lv_draw_vector_dsc_set_fill_opa
#define lv_vector_dsc_set_fill_rule                 lv_draw_vector_dsc_set_fill_rule
#define lv_vector_dsc_set_fill_units                lv_draw_vector_dsc_set_fill_units
#define lv_vector_dsc_set_fill_image                lv_draw_vector_dsc_set_fill_image
#define lv_vector_dsc_set_fill_linear_gradient      lv_draw_vector_dsc_set_fill_linear_gradient
#define lv_vector_dsc_set_fill_radial_gradient      lv_draw_vector_dsc_set_fill_radial_gradient
#define lv_vector_dsc_set_fill_gradient_spread      lv_draw_vector_dsc_set_fill_gradient_spread
#define lv_vector_dsc_set_fill_gradient_color_stops lv_draw_vector_dsc_set_fill_gradient_color_stops
#define lv_vector_dsc_set_fill_transform            lv_draw_vector_dsc_set_fill_transform
#define lv_vector_dsc_set_stroke_color32            lv_draw_vector_dsc_set_stroke_color32
#define lv_vector_dsc_set_stroke_color              lv_draw_vector_dsc_set_stroke_color
#define lv_vector_dsc_set_stroke_opa                lv_draw_vector_dsc_set_stroke_opa
#define lv_vector_dsc_set_stroke_width              lv_draw_vector_dsc_set_stroke_width
#define lv_vector_dsc_set_stroke_dash               lv_draw_vector_dsc_set_stroke_dash
#define lv_vector_dsc_set_stroke_cap                lv_draw_vector_dsc_set_stroke_cap
#define lv_vector_dsc_set_stroke_join               lv_draw_vector_dsc_set_stroke_join
#define lv_vector_dsc_set_stroke_miter_limit        lv_draw_vector_dsc_set_stroke_miter_limit
#define lv_vector_dsc_set_stroke_linear_gradient    lv_draw_vector_dsc_set_stroke_linear_gradient
#define lv_vector_dsc_set_stroke_radial_gradient    lv_draw_vector_dsc_set_stroke_radial_gradient
#define lv_vector_dsc_set_stroke_gradient_spread    lv_draw_vector_dsc_set_stroke_gradient_spread
#define lv_vector_dsc_set_stroke_gradient_color_stops lv_draw_vector_dsc_set_stroke_gradient_color_stops
#define lv_vector_dsc_set_stroke_transform          lv_draw_vector_dsc_set_stroke_transform
#define lv_vector_dsc_identity                      lv_draw_vector_dsc_identity
#define lv_vector_dsc_scale                         lv_draw_vector_dsc_scale
#define lv_vector_dsc_rotate                        lv_draw_vector_dsc_rotate
#define lv_vector_dsc_translate                     lv_draw_vector_dsc_translate
#define lv_vector_dsc_skew                          lv_draw_vector_dsc_skew
#define lv_vector_dsc_add_path                      lv_draw_vector_dsc_add_path
#define lv_vector_dsc_clear_area                    lv_draw_vector_dsc_clear_area


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_API_MAP_V9_3_H */
