/**
 * @file lv_area_private.h
 *
 */

#ifndef LV_AREA_PRIVATE_H
#define LV_AREA_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_area.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Set the position of an area (width and height will be kept)
 * @param area_p pointer to an area
 * @param x the new x coordinate of the area
 * @param y the new y coordinate of the area
 */
void lv_area_set_pos(lv_area_t * area_p, int32_t x, int32_t y);

/**
 * Get the common parts of two areas
 * @param res_p pointer to an area, the result will be stored her
 * @param a1_p pointer to the first area
 * @param a2_p pointer to the second area
 * @return false: the two area has NO common parts, res_p is invalid
 */
bool lv_area_intersect(lv_area_t * res_p, const lv_area_t * a1_p, const lv_area_t * a2_p);

/**
 * Get resulting sub areas after removing the common parts of two areas from the first area
 * @param res_p pointer to an array of areas with a count of 4, the resulting areas will be stored here
 * @param a1_p pointer to the first area
 * @param a2_p pointer to the second area
 * @return number of results (max 4) or -1 if no intersect
 */
int8_t lv_area_diff(lv_area_t res_p[], const lv_area_t * a1_p, const lv_area_t * a2_p);

/**
 * Join two areas into a third which involves the other two
 * @param a_res_p pointer to an area, the result will be stored here
 * @param a1_p pointer to the first area
 * @param a2_p pointer to the second area
 */
void lv_area_join(lv_area_t * a_res_p, const lv_area_t * a1_p, const lv_area_t * a2_p);

/**
 * Check if a point is on an area
 * @param a_p pointer to an area
 * @param p_p pointer to a point
 * @param radius radius of area (e.g. for rounded rectangle)
 * @return false:the point is out of the area
 */
bool lv_area_is_point_on(const lv_area_t * a_p, const lv_point_t * p_p, int32_t radius);

/**
 * Check if two area has common parts
 * @param a1_p pointer to an area.
 * @param a2_p pointer to another area
 * @return false: a1_p and a2_p has no common parts
 */
bool lv_area_is_on(const lv_area_t * a1_p, const lv_area_t * a2_p);

/**
 * Check if an area is fully on another
 * @param ain_p pointer to an area which could be in 'aholder_p'
 * @param aholder_p pointer to an area which could involve 'ain_p'
 * @param radius radius of `aholder_p` (e.g. for rounded rectangle)
 * @return true: `ain_p` is fully inside `aholder_p`
 */
bool lv_area_is_in(const lv_area_t * ain_p, const lv_area_t * aholder_p, int32_t radius);

/**
 * Check if an area is fully out of another
 * @param aout_p pointer to an area which could be in 'aholder_p'
 * @param aholder_p pointer to an area which could involve 'ain_p'
 * @param radius radius of `aholder_p` (e.g. for rounded rectangle)
 * @return true: `aout_p` is fully outside `aholder_p`
 */
bool lv_area_is_out(const lv_area_t * aout_p, const lv_area_t * aholder_p, int32_t radius);

/**
 * Check if 2 area is the same
 * @param a pointer to an area
 * @param b pointer to another area
 */
bool lv_area_is_equal(const lv_area_t * a, const lv_area_t * b);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_AREA_PRIVATE_H*/
