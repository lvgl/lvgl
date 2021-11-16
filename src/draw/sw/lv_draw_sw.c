/**
 * @file lv_draw_sw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_draw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

void lv_draw_sw_arc(lv_coord_t center_x, lv_coord_t center_y, uint16_t radius,  uint16_t start_angle, uint16_t end_angle,
                 const lv_area_t * clip_area, const lv_draw_arc_dsc_t * dsc);

void lv_draw_sw_rect(const lv_area_t * coords, const lv_area_t * clip, const lv_draw_rect_dsc_t * dsc);

LV_ATTRIBUTE_FAST_MEM void lv_draw_sw_letter(const lv_point_t * pos_p, const lv_area_t * clip_area,
                                          const lv_font_t * font_p,
                                          uint32_t letter,
                                          lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode);

LV_ATTRIBUTE_FAST_MEM void lv_draw_sw_img(const lv_area_t * map_area, const lv_area_t * clip_area,
                                              const uint8_t * map_p,
                                              const lv_draw_img_dsc_t * draw_dsc,
                                              bool chroma_key, bool alpha_byte);

void lv_draw_sw_line(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * clip,
                                        const lv_draw_line_dsc_t * dsc);

void lv_draw_sw_polygon(const lv_point_t points[], uint16_t point_cnt, const lv_area_t * clip_area,
                        const lv_draw_rect_dsc_t * draw_dsc);


LV_ATTRIBUTE_FAST_MEM void lv_blend_sw_fill(const lv_area_t * clip_area, const lv_area_t * fill_area,
                                          lv_color_t color, lv_opa_t * mask, lv_draw_mask_res_t mask_res, lv_opa_t opa,
                                          lv_blend_mode_t mode);

LV_ATTRIBUTE_FAST_MEM void lv_blend_sw_map(const lv_area_t * clip_area, const lv_area_t * map_area,
                                         const lv_color_t * map_buf,
                                         lv_opa_t * mask, lv_draw_mask_res_t mask_res,
                                         lv_opa_t opa, lv_blend_mode_t mode);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sw_init(void)
{
    static lv_draw_backend_t backend;
    lv_draw_backend_init(&backend);

    backend.draw_arc = lv_draw_sw_arc;
    backend.draw_rect = lv_draw_sw_rect;
    backend.draw_letter = lv_draw_sw_letter;
    backend.draw_img = lv_draw_sw_img;
    backend.draw_line = lv_draw_sw_line;
    backend.draw_polygon = lv_draw_sw_polygon;
    backend.blend_fill = lv_blend_sw_fill;
    backend.blend_map  = lv_blend_sw_map;

    lv_draw_backend_add(&backend);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
