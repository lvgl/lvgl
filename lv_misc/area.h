/**
 * @file area.h
 * 
 */

#ifndef AREA_H
#define AREA_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef LV_CORD_TYPE cord_t;

typedef struct
{
    cord_t x;
    cord_t y;
}point_t;

typedef struct
{
    cord_t x1;
    cord_t y1;        
    cord_t x2;
    cord_t y2;        
}area_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize an area
 * @param area_p pointer to an area
 * @param x1 left coordinate of the area
 * @param y1 top coordinate of the area
 * @param x2 right coordinate of the area
 * @param y2 bottom coordinate of the area
 */
void area_set(area_t * area_p, cord_t x1, cord_t y1, cord_t x2, cord_t y2);

/**
 * Copy an area
 * @param dest pointer to the destination area
 * @param src pointer to the source area
 */
static void inline area_cpy(area_t * dest, const area_t * src)
{
    memcpy(dest, src, sizeof(area_t));
}

/**
 * Get the width of an area
 * @param area_p pointer to an area
 * @return the width of the area (if x1 == x2 -> width = 1)
 */
static inline cord_t area_get_width(const area_t * area_p)
{
    return area_p->x2 - area_p->x1 + 1;
}

/**
 * Get the height of an area
 * @param area_p pointer to an area
 * @return the height of the area (if y1 == y2 -> height = 1)
 */
static inline cord_t area_get_height(const area_t * area_p)
{
    return area_p->y2 - area_p->y1 + 1;
}

/**
 * Set the width of an area
 * @param area_p pointer to an area
 * @param w the new width of the area (w == 1 makes x1 == x2)
 */
void area_set_width(area_t * area_p, cord_t w);

/**
 * Set the height of an area
 * @param area_p pointer to an area
 * @param h the new height of the area (h == 1 makes y1 == y2)
 */
void area_set_height(area_t * area_p, cord_t h);

/**
 * Set the position of an area (width and height will be kept)
 * @param area_p pointer to an area
 * @param x the new x coordinate of the area
 * @param y the new y coordinate of the area
 */
void area_set_pos(area_t * area_p, cord_t x, cord_t y);

/**
 * Return with area of an area (x * y)
 * @param area_p pointer to an area
 * @return size of area
 */
uint32_t area_get_size(const area_t * area_p);

/**
 * Get the common parts of two areas
 * @param res_p pointer to an area, the result will be stored her
 * @param a1_p pointer to the first area
 * @param a2_p pointer to the second area
 * @return false: the two area has NO common parts, res_p is invalid
 */
bool area_union(area_t * res_p, const area_t * a1_p, const area_t * a2_p);

/**
 * Join two areas into a third which involves the other two
 * @param res_p pointer to an area, the result will be stored here
 * @param a1_p pointer to the first area
 * @param a2_p pointer to the second area
 */
void area_join(area_t * a_res_p, const area_t * a1_p, const area_t * a2_p);

/**
 * Check if a point is on an area
 * @param a_p pointer to an area
 * @param p_p pointer to a point
 * @return false:the point is out of the area
 */
bool area_is_point_on(const area_t * a_p, const point_t * p_p);

/**
 * Check if two area has common parts
 * @param a1_p pointer to an area.
 * @param a2_p pointer to an other area
 * @return false: a1_p and a2_p has no common parts
 */
bool area_is_on(const area_t * a1_p, const area_t * a2_p);

/**
 * Check if an area is fully on an other
 * @param ain_p pointer to an area which could be on aholder_p
 * @param aholder pointer to an area which could involve ain_p
 * @return
 */
bool area_is_in(const area_t * ain_p, const area_t * aholder_p);

/**********************
 *      MACROS
 **********************/

#endif
