/**
 * @file circ.h
 * 
 */

#ifndef CIRC_H
#define CIRC_H

/*********************
 *      INCLUDES
 *********************/

#include <lvgl/lv_misc/area.h>
#include <stddef.h>

/*********************
 *      DEFINES
 *********************/
#define CIRC_OCT1_X(p) (p.x)
#define CIRC_OCT1_Y(p) (p.y)
#define CIRC_OCT2_X(p) (p.y)
#define CIRC_OCT2_Y(p) (p.x)
#define CIRC_OCT3_X(p) (-p.y)
#define CIRC_OCT3_Y(p) (p.x)
#define CIRC_OCT4_X(p) (-p.x)
#define CIRC_OCT4_Y(p) (p.y)
#define CIRC_OCT5_X(p) (-p.x)
#define CIRC_OCT5_Y(p) (-p.y)
#define CIRC_OCT6_X(p) (-p.y)
#define CIRC_OCT6_Y(p) (-p.x)
#define CIRC_OCT7_X(p) (p.y)
#define CIRC_OCT7_Y(p) (-p.x)
#define CIRC_OCT8_X(p) (p.x)
#define CIRC_OCT8_Y(p) (-p.y)

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
void circ_init(point_t * c, cord_t * tmp, cord_t radius);

/**
 * Test the circle drawing is ready or not
 * @param c same as in circ_init
 * @return true if the circle is not ready yet
 */
bool circ_cont(point_t * c);

/**
 * Get the next point from the circle
 * @param c same as in circ_init. The next point stored here.
 * @param tmp same as in circ_init.
 */
void circ_next(point_t * c, cord_t * tmp);

/**********************
 *      MACROS
 **********************/

#endif
