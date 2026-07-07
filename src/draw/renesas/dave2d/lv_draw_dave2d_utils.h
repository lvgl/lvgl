/**
 * @file lv_draw_dave2d_utils.h
 *
 */

#ifndef LV_DRAW_DAVE2D_UTILS_H
#define LV_DRAW_DAVE2D_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lv_draw_dave2d.h"
#if LV_USE_DRAW_DAVE2D

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

d2_color lv_draw_dave2d_lv_colour_to_d2_colour(lv_color_t color);

d2_u32 lv_draw_dave2d_lv_colour_fmt_to_d2_fmt(lv_color_format_t colour_format);

void d2_framebuffer_from_layer(d2_device * handle, lv_layer_t * layer);

/**
 * Clamp a clip rectangle (given in layer-buffer-local coordinates, i.e. with the
 * origin at the layer's buf_area top-left) to the actual framebuffer extent.
 * The D/AVE 2D renderers derive their scissor from the draw task's clip_area,
 * which can be larger than the current (possibly chunked) layer buffer; without
 * this clamp the GPU can write past the buffer and corrupt the heap.
 */
bool lv_draw_dave2d_clip_to_layer_buf(lv_area_t * clip, const lv_layer_t * layer);

bool lv_draw_dave2d_is_dest_cf_supported(lv_color_format_t cf);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_DAVE2D*/
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_DAVE2D_UTILS_H*/
