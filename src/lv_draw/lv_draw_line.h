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
#include "../lv_core/lv_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_color_t color;
    lv_style_int_t width;
    lv_style_int_t dash_width;
    lv_style_int_t dash_gap;
    lv_opa_t opa;
    lv_blend_mode_t blend_mode  : 2;
    uint8_t round_start : 1;
    uint8_t round_end   : 1;
    uint8_t raw_end     : 1;    /*Do not bother with perpendicular line ending is it's not visible for any reason*/
} lv_draw_line_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

//! @cond Doxygen_Suppress
/**
 * Draw a line
 * @param point1 first point of the line
 * @param point2 second point of the line
 * @param clip the line will be drawn only in this area
 * @param dsc pointer to an initialized `lv_draw_line_dsc_t` variable
 */
LV_ATTRIBUTE_FAST_MEM void lv_draw_line(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * clip,
                                        const lv_draw_line_dsc_t * dsc);

LV_ATTRIBUTE_FAST_MEM void lv_draw_line_dsc_init(lv_draw_line_dsc_t * dsc);


//! @endcond

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DRAW_LINE_H*/
