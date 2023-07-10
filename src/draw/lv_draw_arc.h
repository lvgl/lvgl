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
#include "../lv_conf_internal.h"
#include "../misc/lv_color.h"
#include "../misc/lv_area.h"
#include "../misc/lv_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_draw_dsc_base_t base;

    lv_color_t color;
    lv_coord_t width;
    uint16_t start_angle;
    uint16_t end_angle;
    lv_point_t center;
    uint16_t radius;
    const void * img_src;
    lv_opa_t opa;
    uint8_t rounded : 1;
} lv_draw_arc_dsc_t;

struct _lv_layer_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_arc_dsc_init(lv_draw_arc_dsc_t * dsc);

void lv_draw_arc(struct _lv_layer_t * layer, const lv_draw_arc_dsc_t * dsc);

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

#endif /*LV_DRAW_ARC_H*/
