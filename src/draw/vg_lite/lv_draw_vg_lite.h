/**
 * @file lv_draw_vg_lite.h
 *
 */

#ifndef LV_DRAW_VG_LITE_H
#define LV_DRAW_VG_LITE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl_public.h"

#if LV_USE_DRAW_VG_LITE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_vg_lite_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_draw_buf_vg_lite_init_handlers(void);

void lv_draw_vg_lite_init(void);

void lv_draw_vg_lite_deinit(void);

/**
 * Mark the draw tasks queued to the GPU as finished. Call it when the GPU has drawn everything.
 * Until then the draw tasks of other draw units that depend on them can't start.
 * @param u     pointer to the VG-Lite draw unit
 */
void lv_draw_vg_lite_finish_queued_tasks(struct _lv_draw_vg_lite_unit_t * u);

void lv_draw_vg_lite_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                         const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_vg_lite_box_shadow(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                                const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords);

/**
 * Check whether VG-Lite should draw a box shadow. It can't cut a non-covering object out of every shadow,
 * so those are left to SW, unless SW can't draw into the layer.
 * @param dsc       the box shadow descriptor
 * @param layer_cf  the color format of the target layer
 * @return          true: VG-Lite should draw it
 */
bool lv_draw_vg_lite_box_shadow_is_supported(const lv_draw_box_shadow_dsc_t * dsc, lv_color_format_t layer_cf);

void lv_draw_vg_lite_blur(lv_draw_unit_t * u, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_vg_lite_border(lv_draw_unit_t * u, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_draw_border_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_vg_lite_fill(lv_draw_unit_t * u, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_vg_lite_img(lv_draw_unit_t * u, lv_layer_t * layer, const lv_area_t * clip_area,
                         const lv_draw_image_dsc_t * dsc, const lv_area_t * coords, bool no_cache);

void lv_draw_vg_lite_label_init(struct _lv_draw_vg_lite_unit_t * u);

void lv_draw_vg_lite_label_deinit(struct _lv_draw_vg_lite_unit_t * u);

void lv_draw_vg_lite_letter(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_draw_letter_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_vg_lite_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                           const lv_draw_label_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_vg_lite_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                           const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords);

void lv_draw_vg_lite_line(lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_line_dsc_t * dsc, void * user_data);

void lv_draw_vg_lite_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                              const lv_draw_triangle_dsc_t * dsc);

void lv_draw_vg_lite_mask_rect(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                               const lv_draw_mask_rect_dsc_t * dsc);

#if LV_USE_VECTOR_GRAPHIC
void lv_draw_vg_lite_vector(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_draw_vector_dsc_t * dsc);
#endif

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_VG_LITE_H*/
