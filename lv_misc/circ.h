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
void circ_init(point_t * c, cord_t * tmp, cord_t radius);
bool circ_cont(point_t * c);
void circ_next(point_t * c, cord_t * tmp);

/**********************
 *      MACROS
 **********************/

#endif
