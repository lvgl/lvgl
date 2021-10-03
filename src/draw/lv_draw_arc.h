/**
 * @file lv_draw_arc.h
 *
 */

#ifndef LV_DRAW_ARC_H
#define LV_DRAW_ARC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_line.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_color_t color;
    lv_coord_t width;
    const void * img_src;
    lv_opa_t opa;
    lv_blend_mode_t blend_mode  : 2;
    uint8_t rounded : 1;
} lv_draw_arc_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

LV_ATTRIBUTE_FAST_MEM void lv_draw_arc_dsc_init(lv_draw_arc_dsc_t * dsc);

/**
 * Draw an arc. (Can draw pie too with great thickness.)
 * @param center_x      the x coordinate of the center of the arc
 * @param center_y      the y coordinate of the center of the arc
 * @param radius        the radius of the arc
 * @param mask          the arc will be drawn only in this mask
 * @param start_angle   the start angle of the arc (0 deg on the bottom, 90 deg on the right)
 * @param end_angle     the end angle of the arc
 * @param clip_area     the arc will be drawn only in this area
 * @param dsc           pointer to an initialized `lv_draw_line_dsc_t` variable
 */
void lv_draw_arc(lv_coord_t center_x, lv_coord_t center_y, uint16_t radius,  uint16_t start_angle, uint16_t end_angle,
                 const lv_area_t * clip_area, const lv_draw_arc_dsc_t * dsc);

/**
 * Get an area the should be invalidated when the arcs angle changed between start_angle and end_ange
 * @param x             the x coordinate of the center of the arc
 * @param y             the y coordinate of the center of the arc
 * @param radius        the radius of the arc
 * @param start_angle   the start angle of the arc (0 deg on the bottom, 90 deg on the right)
 * @param end_angle     the end angle of the arc
 * @param w             width of the arc
 * @param rounded       true: the arc is rounded
 * @param area          store the area to invalidate here
 */
void lv_draw_arc_get_area(lv_coord_t x, lv_coord_t y, uint16_t radius,  uint16_t start_angle, uint16_t end_angle,
                          lv_coord_t w, bool rounded, lv_area_t * area);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_ARC*/
