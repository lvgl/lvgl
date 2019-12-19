/**
 * @file lv_templ.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_point.h"

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
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_point_within_ellipse(const lv_area_t * area, const lv_point_t * p) {
    /* Ellipse radius */
    lv_coord_t a = (area->x2 - area->x1) / 2;
    lv_coord_t b = (area->y2 - area->y1) / 2;

    /* Ellipse center */
    lv_coord_t cx = area->x1 + a;
    lv_coord_t cy = area->y1 + b;
    
    /*Simplify the code by moving everything to (0, 0) */
    /* Point, recentered as though the ellipse is at (0, 0) */
    lv_coord_t px = p->x - cx;
    lv_coord_t py = p->y - cy;

    /* A point is within an ellipse if (x^2)(b^2) + (y^2)(a^2) <= 1 */

    int32_t a_sqrd = a*a;
    int32_t b_sqrd = b*b;
    int32_t dist = ((px*px)*(b_sqrd)) + ((py*py)*(a_sqrd));
    int32_t extrema = a_sqrd*b_sqrd;
    
    if(dist <= extrema)
        return true;
    else
        return false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
