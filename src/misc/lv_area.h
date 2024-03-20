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
#include "../lv_conf_internal.h"
#include <stdbool.h>
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

#if LV_USE_LARGE_COORD
typedef int32_t lv_coord_t;
#else
typedef int16_t lv_coord_t;
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Represents a point on the screen.
 */
typedef struct {
    lv_coord_t x;
    lv_coord_t y;
} lv_point_t;

/** Represents an area of the screen.*/
typedef struct {
    lv_coord_t x1;
    lv_coord_t y1;
    lv_coord_t x2;
    lv_coord_t y2;
} lv_area_t;

/** Alignments*/
enum {
    LV_ALIGN_DEFAULT = 0,
    LV_ALIGN_TOP_LEFT,
    LV_ALIGN_TOP_MID,
    LV_ALIGN_TOP_RIGHT,
    LV_ALIGN_BOTTOM_LEFT,
    LV_ALIGN_BOTTOM_MID,
    LV_ALIGN_BOTTOM_RIGHT,
    LV_ALIGN_LEFT_MID,
    LV_ALIGN_RIGHT_MID,
    LV_ALIGN_CENTER,

    LV_ALIGN_OUT_TOP_LEFT,
    LV_ALIGN_OUT_TOP_MID,
    LV_ALIGN_OUT_TOP_RIGHT,
    LV_ALIGN_OUT_BOTTOM_LEFT,
    LV_ALIGN_OUT_BOTTOM_MID,
    LV_ALIGN_OUT_BOTTOM_RIGHT,
    LV_ALIGN_OUT_LEFT_TOP,
    LV_ALIGN_OUT_LEFT_MID,
    LV_ALIGN_OUT_LEFT_BOTTOM,
    LV_ALIGN_OUT_RIGHT_TOP,
    LV_ALIGN_OUT_RIGHT_MID,
    LV_ALIGN_OUT_RIGHT_BOTTOM,
};
typedef uint8_t lv_align_t;

enum {
    LV_DIR_NONE     = 0x00,
    LV_DIR_LEFT     = (1 << 0),
    LV_DIR_RIGHT    = (1 << 1),
    LV_DIR_TOP      = (1 << 2),
    LV_DIR_BOTTOM   = (1 << 3),
    LV_DIR_HOR      = LV_DIR_LEFT | LV_DIR_RIGHT,
    LV_DIR_VER      = LV_DIR_TOP | LV_DIR_BOTTOM,
    LV_DIR_ALL      = LV_DIR_HOR | LV_DIR_VER,
};

typedef uint8_t lv_dir_t;

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
    dest->x1 = src->x1;
    dest->y1 = src->y1;
    dest->x2 = src->x2;
    dest->y2 = src->y2;
}

/**
 * Get the width of an area
 * @param area_p pointer to an area
 * @return the width of the area (if x1 == x2 -> width = 1)
 */
static inline lv_coord_t lv_area_get_width(const lv_area_t * area_p)
{
    return (lv_coord_t)(area_p->x2 - area_p->x1 + 1);
}

/**
 * Get the height of an area
 * @param area_p pointer to an area
 * @return the height of the area (if y1 == y2 -> height = 1)
 */
static inline lv_coord_t lv_area_get_height(const lv_area_t * area_p)
{
    return (lv_coord_t)(area_p->y2 - area_p->y1 + 1);
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
void _lv_area_set_pos(lv_area_t * area_p, lv_coord_t x, lv_coord_t y);

/**
 * Return with area of an area (x * y)
 * @param area_p pointer to an area
 * @return size of area
 */
uint32_t lv_area_get_size(const lv_area_t * area_p);

void lv_area_increase(lv_area_t * area, lv_coord_t w_extra, lv_coord_t h_extra);

void lv_area_move(lv_area_t * area, lv_coord_t x_ofs, lv_coord_t y_ofs);

/**
 * Get the common parts of two areas
 * @param res_p pointer to an area, the result will be stored her
 * @param a1_p pointer to the first area
 * @param a2_p pointer to the second area
 * @return false: the two area has NO common parts, res_p is invalid
 */
bool _lv_area_intersect(lv_area_t * res_p, const lv_area_t * a1_p, const lv_area_t * a2_p);

/**
 * Get resulting sub areas after removing the common parts of two areas from the first area
 * @param res_p pointer to an array of areas with a count of 4, the resulting areas will be stored here
 * @param a1_p pointer to the first area
 * @param a2_p pointer to the second area
 * @return number of results (max 4) or -1 if no intersect
 */
int8_t _lv_area_diff(lv_area_t * res_p, const lv_area_t * a1_p, const lv_area_t * a2_p);

/**
 * Join two areas into a third which involves the other two
 * @param res_p pointer to an area, the result will be stored here
 * @param a1_p pointer to the first area
 * @param a2_p pointer to the second area
 */
void _lv_area_join(lv_area_t * a_res_p, const lv_area_t * a1_p, const lv_area_t * a2_p);

/**
 * Check if a point is on an area
 * @param a_p pointer to an area
 * @param p_p pointer to a point
 * @param radius radius of area (e.g. for rounded rectangle)
 * @return false:the point is out of the area
 */
bool _lv_area_is_point_on(const lv_area_t * a_p, const lv_point_t * p_p, lv_coord_t radius);

/**
 * Check if two area has common parts
 * @param a1_p pointer to an area.
 * @param a2_p pointer to an other area
 * @return false: a1_p and a2_p has no common parts
 */
bool _lv_area_is_on(const lv_area_t * a1_p, const lv_area_t * a2_p);

/**
 * Check if an area is fully on an other
 * @param ain_p pointer to an area which could be in 'aholder_p'
 * @param aholder_p pointer to an area which could involve 'ain_p'
 * @param radius radius of `aholder_p` (e.g. for rounded rectangle)
 * @return true: `ain_p` is fully inside `aholder_p`
 */
bool _lv_area_is_in(const lv_area_t * ain_p, const lv_area_t * aholder_p, lv_coord_t radius);

/**
 * Check if an area is fully out of an other
 * @param aout_p pointer to an area which could be in 'aholder_p'
 * @param aholder_p pointer to an area which could involve 'ain_p'
 * @param radius radius of `aholder_p` (e.g. for rounded rectangle)
 * @return true: `aout_p` is fully outside `aholder_p`
 */
bool _lv_area_is_out(const lv_area_t * aout_p, const lv_area_t * aholder_p, lv_coord_t radius);

/**
 * Check if 2 area is the same
 * @param a pointer to an area
 * @param b pointer to another area
 */
bool _lv_area_is_equal(const lv_area_t * a, const lv_area_t * b);

/**
 * Align an area to an other
 * @param base an are where the other will be aligned
 * @param to_align the area to align
 * @param align `LV_ALIGN_...`
 */
void lv_area_align(const lv_area_t * base, lv_area_t * to_align, lv_align_t align, lv_coord_t ofs_x, lv_coord_t ofs_y);

void lv_point_transform(lv_point_t * p, int32_t angle, int32_t zoom, const lv_point_t * pivot);

/**********************
 *      MACROS
 **********************/

#if LV_USE_LARGE_COORD
#define _LV_COORD_TYPE_SHIFT    (29U)
#else
#define _LV_COORD_TYPE_SHIFT    (13U)
#endif

#define _LV_COORD_TYPE_MASK     (3 << _LV_COORD_TYPE_SHIFT)
#define _LV_COORD_TYPE(x)       ((x) & _LV_COORD_TYPE_MASK)  /*Extract type specifiers*/
#define _LV_COORD_PLAIN(x)      ((x) & ~_LV_COORD_TYPE_MASK) /*Remove type specifiers*/

#define _LV_COORD_TYPE_PX       (0 << _LV_COORD_TYPE_SHIFT)
#define _LV_COORD_TYPE_SPEC     (1 << _LV_COORD_TYPE_SHIFT)
#define _LV_COORD_TYPE_PX_NEG   (3 << _LV_COORD_TYPE_SHIFT)

#define LV_COORD_IS_PX(x)       (_LV_COORD_TYPE(x) == _LV_COORD_TYPE_PX || \
                                 _LV_COORD_TYPE(x) == _LV_COORD_TYPE_PX_NEG ? true : false)
#define LV_COORD_IS_SPEC(x)     (_LV_COORD_TYPE(x) == _LV_COORD_TYPE_SPEC ? true : false)

#define LV_COORD_SET_SPEC(x)    ((x) | _LV_COORD_TYPE_SPEC)

/*Special coordinates*/
#define LV_PCT(x)               (x < 0 ? LV_COORD_SET_SPEC(1000 - (x)) : LV_COORD_SET_SPEC(x))
#define LV_COORD_IS_PCT(x)      ((LV_COORD_IS_SPEC(x) && _LV_COORD_PLAIN(x) <= 2000) ? true : false)
#define LV_COORD_GET_PCT(x)     (_LV_COORD_PLAIN(x) > 1000 ? 1000 - _LV_COORD_PLAIN(x) : _LV_COORD_PLAIN(x))
#define LV_SIZE_CONTENT         LV_COORD_SET_SPEC(2001)

LV_EXPORT_CONST_INT(LV_SIZE_CONTENT);

/*Max coordinate value*/
#define LV_COORD_MAX            ((1 << _LV_COORD_TYPE_SHIFT) - 1)
#define LV_COORD_MIN            (-LV_COORD_MAX)

LV_EXPORT_CONST_INT(LV_COORD_MAX);
LV_EXPORT_CONST_INT(LV_COORD_MIN);

/**
 * Convert a percentage value to `lv_coord_t`.
 * Percentage values are stored in special range
 * @param x the percentage (0..1000)
 * @return a coordinate that stores the percentage
 */
static inline lv_coord_t lv_pct(lv_coord_t x)
{
    return LV_PCT(x);
}

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
