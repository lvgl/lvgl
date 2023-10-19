/**
 * @file lv_draw_mask_rect.h
 *
 */

#ifndef LV_DRAW_MASK_RECT_H
#define LV_DRAW_MASK_RECT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
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

    lv_area_t area;
    lv_coord_t radius;
} lv_draw_mask_rect_dsc_t;

struct _lv_layer_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

LV_ATTRIBUTE_FAST_MEM void lv_draw_mask_rect_dsc_init(lv_draw_mask_rect_dsc_t * dsc);

/**
 * Draw a line
 * @param point1 first point of the line
 * @param point2 second point of the line
 * @param clip the line will be drawn only in this area
 * @param dsc pointer to an initialized `lv_draw_line_dsc_t` variable
 */
void lv_draw_mask_rect(struct _lv_layer_t * layer, const lv_draw_mask_rect_dsc_t * dsc);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_MASK_RECT_H*/
