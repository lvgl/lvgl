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
#include "../../misc/lv_area.h"
#include "../../misc/lv_color.h"
#include "lv_draw_sw_mask.h"
#include "lv_draw_sw_blend.h"
#include "../lv_draw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_t base_draw;

    /** Fill an area of the destination buffer with a color*/
    void (*blend)(lv_draw_t * draw, const lv_draw_sw_blend_dsc_t * dsc);
}lv_draw_sw_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_sw_init(void);
lv_draw_t * lv_draw_sw_create(void);

void lv_draw_sw_arc(lv_coord_t center_x, lv_coord_t center_y, uint16_t radius,  uint16_t start_angle,
        uint16_t end_angle,
        const lv_area_t * clip_area, const lv_draw_arc_dsc_t * dsc);

void lv_draw_sw_rect(lv_draw_t * draw, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_sw_letter(lv_draw_t * draw, const lv_draw_label_dsc_t * dsc,  const lv_point_t * pos_p, uint32_t letter);


LV_ATTRIBUTE_FAST_MEM void lv_draw_sw_img(struct _lv_draw_t * draw, const lv_draw_img_dsc_t * draw_dsc,
        const lv_area_t * coords, const uint8_t * src_buf, lv_img_cf_t cf);

void lv_draw_sw_line(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * clip, const lv_draw_line_dsc_t * dsc);

void lv_draw_sw_polygon(const lv_point_t points[], uint16_t point_cnt, const lv_area_t * clip_area, const lv_draw_rect_dsc_t * draw_dsc);


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
