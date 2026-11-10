/**
 * @file lv_draw_ppa.h
 *
 */

#ifndef LV_DRAW_PPA_H
#define LV_DRAW_PPA_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl_public.h"

#if LV_USE_PPA

#include "../../lv_draw_private.h"
#include "../../../display/lv_display_private.h"
#include "../../../misc/lv_area_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_ppa_init(void);
void lv_draw_ppa_deinit(void);
void lv_draw_buf_ppa_init_handlers(void);

void lv_draw_ppa_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_ppa_img(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                     const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);

lv_result_t lv_draw_ppa_rotate(const void * src, void * dst, int32_t src_width, int32_t src_height,
                               int32_t src_stride, int32_t dst_stride, lv_rotation_t rotation,
                               lv_color_format_t cf);

/**********************
 *      MACROS
 **********************/

#define LV_DRAW_ROTATE90_RGB565(src, dst, w, h, src_stride, dst_stride) \
    lv_draw_ppa_rotate(src, dst, w, h, src_stride, dst_stride, LV_ROTATION_90, LV_COLOR_FORMAT_RGB565)
#define LV_DRAW_ROTATE180_RGB565(src, dst, w, h, src_stride, dst_stride) \
    lv_draw_ppa_rotate(src, dst, w, h, src_stride, dst_stride, LV_ROTATION_180, LV_COLOR_FORMAT_RGB565)
#define LV_DRAW_ROTATE270_RGB565(src, dst, w, h, src_stride, dst_stride) \
    lv_draw_ppa_rotate(src, dst, w, h, src_stride, dst_stride, LV_ROTATION_270, LV_COLOR_FORMAT_RGB565)

#endif /* LV_USE_PPA */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_DRAW_PPA_H */
