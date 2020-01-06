/**
 * @file lv_draw_line.h
 *
 */

#ifndef LV_DRAW_LINE_H
#define LV_DRAW_LINE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_color_t color;
    lv_style_int_t width;
    lv_opa_t opa;
    lv_blend_mode_t blend_mode;
    uint8_t round_start :1;
    uint8_t round_end   :1;
}lv_draw_line_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Draw a line
 * @param point1 first point of the line
 * @param point2 second point of the line
 * @param mask the line will be drawn only on this area
 * @param style pointer to a line's style
 * @param opa_scale scale down all opacities by the factor
 */
void lv_draw_line(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * mask, lv_draw_line_dsc_t * dsc);

void lv_draw_line_dsc_init(lv_draw_line_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DRAW_LINE_H*/
