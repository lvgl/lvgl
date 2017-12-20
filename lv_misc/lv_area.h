/**
 * @file lv_area.h
 * 
 */

#ifndef LV_AREA_H
#define LV_AREA_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/
#define LV_COORD_MAX     (16383)    /*To avoid overflow don't let the max [-32,32k] range */
#define LV_COORD_MIN     (-16384)

/**********************
 *      TYPEDEFS
 **********************/
typedef int16_t lv_coord_t;

typedef struct
{
    lv_coord_t x;
    lv_coord_t y;
}lv_point_t;

typedef struct
{
    lv_coord_t x1;
    lv_coord_t y1;        
    lv_coord_t x2;
    lv_coord_t y2;        
}lv_area_t;

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
void lv_area_set(lv_area_t * area_p, lv_coord_t x1, lv_coord_t y1, lv_coord_t x2, lv_coord_t y2);

/**
 * Copy an area
 * @param dest pointer to the destination area
 * @param src pointer to the source area
 */
inline static void lv_area_copy(lv_area_t * dest, const lv_area_t * src)
{
    memcpy(dest, src, sizeof(lv_area_t));
}

/**
 * Get the width of an area
 * @param area_p pointer to an area
 * @return the width of the area (if x1 == x2 -> width = 1)
 */
static inline lv_coord_t lv_area_get_width(const lv_area_t * area_p)
{
    return area_p->x2 - area_p->x1 + 1;
}

/**
 * Get the height of an area
 * @param area_p pointer to an area
 * @return the height of the area (if y1 == y2 -> height = 1)
 */
static inline lv_coord_t lv_area_get_height(const lv_area_t * area_p)
{
    return area_p->y2 - area_p->y1 + 1;
}

/**
 * Set the width of an area
 * @param area_p pointer to an area
 * @param w the new width of the area (w == 1 makes x1 == x2)
 */
void lv_area_set_width(lv_area_t * area_p, lv_coord_t w);

/**
 * Set the height of an area
 * @param area_p pointer to an area
 * @param h the new height of the area (h == 1 makes y1 == y2)
 */
void lv_area_set_height(lv_area_t * area_p, lv_coord_t h);

/**
 * Set the position of an area (width and height will be kept)
 * @param area_p pointer to an area
 * @param x the new x coordinate of the area
 * @param y the new y coordinate of the area
 */
void lv_area_set_pos(lv_area_t * area_p, lv_coord_t x, lv_coord_t y);

/**
 * Return with area of an area (x * y)
 * @param area_p pointer to an area
 * @return size of area
 */
uint32_t lv_area_get_size(const lv_area_t * area_p);

/**
 * Get the common parts of two areas
 * @param res_p pointer to an area, the result will be stored her
 * @param a1_p pointer to the first area
 * @param a2_p pointer to the second area
 * @return false: the two area has NO common parts, res_p is invalid
 */
bool lv_area_union(lv_area_t * res_p, const lv_area_t * a1_p, const lv_area_t * a2_p);

/**
 * Join two areas into a third which involves the other two
 * @param res_p pointer to an area, the result will be stored here
 * @param a1_p pointer to the first area
 * @param a2_p pointer to the second area
 */
void lv_area_join(lv_area_t * a_res_p, const lv_area_t * a1_p, const lv_area_t * a2_p);

/**
 * Check if a point is on an area
 * @param a_p pointer to an area
 * @param p_p pointer to a point
 * @return false:the point is out of the area
 */
bool lv_area_is_point_on(const lv_area_t * a_p, const lv_point_t * p_p);

/**
 * Check if two area has common parts
 * @param a1_p pointer to an area.
 * @param a2_p pointer to an other area
 * @return false: a1_p and a2_p has no common parts
 */
bool lv_area_is_on(const lv_area_t * a1_p, const lv_area_t * a2_p);

/**
 * Check if an area is fully on an other
 * @param ain_p pointer to an area which could be on aholder_p
 * @param aholder pointer to an area which could involve ain_p
 * @return
 */
bool lv_area_is_in(const lv_area_t * ain_p, const lv_area_t * aholder_p);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
