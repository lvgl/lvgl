/**
 * @file lv_circ.h
 * 
 */

#ifndef LV_CIRC_H
#define LV_CIRC_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include "lv_area.h"

/*********************
 *      DEFINES
 *********************/
#define LV_CIRC_OCT1_X(p) (p.x)
#define LV_CIRC_OCT1_Y(p) (p.y)
#define LV_CIRC_OCT2_X(p) (p.y)
#define LV_CIRC_OCT2_Y(p) (p.x)
#define LV_CIRC_OCT3_X(p) (-p.y)
#define LV_CIRC_OCT3_Y(p) (p.x)
#define LV_CIRC_OCT4_X(p) (-p.x)
#define LV_CIRC_OCT4_Y(p) (p.y)
#define LV_CIRC_OCT5_X(p) (-p.x)
#define LV_CIRC_OCT5_Y(p) (-p.y)
#define LV_CIRC_OCT6_X(p) (-p.y)
#define LV_CIRC_OCT6_Y(p) (-p.x)
#define LV_CIRC_OCT7_X(p) (p.y)
#define LV_CIRC_OCT7_Y(p) (-p.x)
#define LV_CIRC_OCT8_X(p) (p.x)
#define LV_CIRC_OCT8_Y(p) (-p.y)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the circle drawing
 * @param c pointer to a point. The coordinates will be calculated here
 * @param tmp point to a variable. It will store temporary data
 * @param radius radius of the circle
 */
void lv_circ_init(lv_point_t * c, lv_coord_t * tmp, lv_coord_t radius);

/**
 * Test the circle drawing is ready or not
 * @param c same as in circ_init
 * @return true if the circle is not ready yet
 */
bool lv_circ_cont(lv_point_t * c);

/**
 * Get the next point from the circle
 * @param c same as in circ_init. The next point stored here.
 * @param tmp same as in circ_init.
 */
void lv_circ_next(lv_point_t * c, lv_coord_t * tmp);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
