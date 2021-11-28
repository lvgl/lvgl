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
#include "../lv_draw_arc.h"
#include "../lv_draw_rect.h"
#include "../lv_draw_mask.h"
#include "../lv_draw_line.h"
#include "../lv_draw_img.h"
#include "../lv_draw_mask.h"
#include "../lv_draw_blend.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_sw_init(void);

void lv_draw_sw_arc(lv_coord_t center_x, lv_coord_t center_y, uint16_t radius,  uint16_t start_angle,
                    uint16_t end_angle,
                    const lv_area_t * clip_area, const lv_draw_arc_dsc_t * dsc);

void lv_draw_sw_rect(const lv_area_t * coords, const lv_area_t * clip, const lv_draw_rect_dsc_t * dsc);

void lv_draw_sw_letter(const lv_point_t * pos_p, const lv_area_t * clip_area,
                       const lv_font_t * font_p,
                       uint32_t letter,
                       lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode);

void lv_draw_sw_img(const lv_area_t * map_area, const lv_area_t * clip_area,
                    const uint8_t * map_p,
                    const lv_draw_img_dsc_t * draw_dsc,
                    bool chroma_key, bool alpha_byte);

void lv_draw_sw_line(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * clip,
                     const lv_draw_line_dsc_t * dsc);

void lv_draw_sw_polygon(const lv_point_t points[], uint16_t point_cnt, const lv_area_t * clip_area,
                        const lv_draw_rect_dsc_t * draw_dsc);


void lv_blend_sw_fill(lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * fill_area,
                      lv_color_t color, lv_opa_t * mask, lv_opa_t opa, lv_blend_mode_t blend_mode);

void lv_blend_sw_map(lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * clip_area,
                     const lv_color_t * src_buf, const lv_area_t * src_area,
                     lv_opa_t * mask, lv_opa_t opa, lv_blend_mode_t mode);

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
