/**
 * @file lv_mask.h
 *
 */

#ifndef LV_MASK_H
#define LV_MASK_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
enum {
    LV_LINE_MASK_SIDE_LEFT = 0,
    LV_LINE_MASK_SIDE_RIGHT,
    LV_LINE_MASK_SIDE_TOP,
    LV_LINE_MASK_SIDE_BOTTOM,
};

typedef uint8_t lv_line_mask_side_t;

typedef struct {
    lv_point_t origo;
    /* X / (1024*Y) steepness (X is 0..1023 range). What is the change of X in 1024 Y?*/
    lv_coord_t xy_steep;

    /* Y / (1024*X) steepness (Y is 0..1023 range). What is the change of Y in 1024 X?*/
    lv_coord_t yx_steep;

    /*Helper which stores yx_steep for flat lines and xy_steep for steep (non flat) lines */
    lv_coord_t steep;

    /*Steepness in 1 px in 0..255 range. Used only by flat lines. */
    int32_t spx;

    /*1: It's a flat line? (Neare to horizontal)*/
    uint8_t flat :1;

    /*Which side to keep?*/
    lv_line_mask_side_t side :2;

    /* Invert the mask. The default is: Keep the left part.
     * It is used to select left/right/top/bottom*/
    uint8_t inv:1;
}lv_mask_line_param_t;

typedef struct {
    lv_area_t rect;
    lv_coord_t radius;
}lv_mask_radius_param_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_mask_apply(lv_color_t * dest_buf, lv_color_t * src_buf, lv_opa_t * mask_buf, lv_coord_t len);

void lv_mask_line_points_init(lv_mask_line_param_t * p, lv_coord_t p1x, lv_coord_t p1y, lv_coord_t p2x, lv_coord_t p2y, lv_line_mask_side_t side);
void lv_mask_line_angle_init(lv_mask_line_param_t * p, lv_coord_t p1x, lv_coord_t p1y, int16_t deg, lv_line_mask_side_t side);
void lv_mask_line(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, void * param);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_MASK_H*/
