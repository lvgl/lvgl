/**
 * @file lv_draw_sw.h
 *
 */

#ifndef LV_DRAW_SW_H
#define LV_DRAW_SW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_draw.h"
#if LV_USE_DRAW_SW

#include "../../misc/lv_area.h"
#include "../../misc/lv_color.h"
#include "../../disp/lv_disp.h"
#include "../../osal/lv_os.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_unit_t base_unit;
    struct _lv_draw_task_t * task_act;
#if LV_USE_OS
    lv_thread_sync_t sync;
    lv_thread_t thread;
#endif
    uint32_t idx;
} lv_draw_sw_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_sw_init(void);

lv_layer_t * lv_draw_sw_layer_init(lv_disp_t * disp);

void lv_draw_sw_layer_deinit(struct _lv_disp_t * disp, lv_layer_t * layer);

void lv_draw_unit_sw_create(lv_disp_t * disp, uint32_t cnt);

LV_ATTRIBUTE_FAST_MEM void lv_draw_sw_img(lv_draw_unit_t * draw_unit, const lv_draw_img_dsc_t * draw_dsc,
                                          const lv_area_t * coords);

void lv_draw_sw_fill(lv_draw_unit_t * draw_unit, const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_sw_border(lv_draw_unit_t * draw_unit, const lv_draw_border_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_sw_box_shadow(lv_draw_unit_t * draw_unit, const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_sw_bg_img(lv_draw_unit_t * draw_unit, const lv_draw_bg_img_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_sw_label(lv_draw_unit_t * draw_unit, const lv_draw_label_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_sw_arc(lv_draw_unit_t * draw_unit, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords);

LV_ATTRIBUTE_FAST_MEM void lv_draw_sw_line(lv_draw_unit_t * draw_unit, const lv_draw_line_dsc_t * dsc);

void lv_draw_sw_layer(lv_draw_unit_t * draw_unit, const lv_draw_img_dsc_t * draw_dsc, const lv_area_t * coords);

void lv_draw_sw_triangle(lv_draw_unit_t * draw_unit, const lv_draw_triangle_dsc_t * dsc);

void lv_draw_sw_mask_rect(lv_draw_unit_t * draw_unit, const lv_draw_mask_rect_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_sw_transform(lv_draw_unit_t * draw_unit, const lv_area_t * dest_area, const void * src_buf,
                          lv_coord_t src_w, lv_coord_t src_h, lv_coord_t src_stride,
                          const lv_draw_img_dsc_t * draw_dsc, const lv_draw_img_sup_t * sup, lv_color_format_t cf, void * dest_buf);

/***********************
 * GLOBAL VARIABLES
 ***********************/

/**********************
 *      MACROS
 **********************/

#include "blend/lv_draw_sw_blend.h"

#endif /*LV_USE_DRAW_SW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_H*/
