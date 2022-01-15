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
#include "lv_draw_sw_blend.h"
#include "../lv_draw.h"
#include "../../misc/lv_area.h"
#include "../../misc/lv_color.h"
#include "../../hal/lv_hal_disp.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_disp_drv_t;

typedef struct {
    lv_draw_ctx_t base_draw;

    /** Fill an area of the destination buffer with a color*/
    void (*blend)(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc);
} lv_draw_sw_ctx_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_sw_init_ctx(struct _lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx);
void lv_draw_sw_deinit_ctx(struct _lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx);

void lv_draw_sw_wait_for_finish(lv_draw_ctx_t * draw_ctx);

void lv_draw_sw_arc(lv_draw_ctx_t * draw_ctx, const lv_draw_arc_dsc_t * dsc, const lv_point_t * center, uint16_t radius,
                    uint16_t start_angle, uint16_t end_angle);

void lv_draw_sw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_sw_bg(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);
void lv_draw_sw_letter(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc, const lv_point_t * pos_p,
                       uint32_t letter);

LV_ATTRIBUTE_FAST_MEM void lv_draw_sw_img_decoded(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * draw_dsc,
                                                  const lv_area_t * coords, const uint8_t * src_buf, lv_img_cf_t cf);

LV_ATTRIBUTE_FAST_MEM void lv_draw_sw_line(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_line_dsc_t * dsc,
                                           const lv_point_t * point1, const lv_point_t * point2);

void lv_draw_sw_polygon(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * draw_dsc,
                        const lv_point_t * points, uint16_t point_cnt);

/***********************
 * GLOBAL VARIABLES
 ***********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_H*/
