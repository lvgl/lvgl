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
#include "lv_types.h"
#include "lv_math.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Represents a point on the screen.
 */
typedef struct {
    int32_t x;
    int32_t y;
} lv_point_t;

typedef struct {
    lv_value_precise_t x;
    lv_value_precise_t y;
} lv_point_precise_t;

/** Represents an area of the screen.*/
typedef struct {
    int32_t x1;
    int32_t y1;
    int32_t x2;
    int32_t y2;
} lv_area_t;

/** Alignments*/

typedef enum {
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
} lv_align_t;

typedef enum {
    LV_DIR_NONE     = 0x00,
    LV_DIR_LEFT     = (1 << 0),
    LV_DIR_RIGHT    = (1 << 1),
    LV_DIR_TOP      = (1 << 2),
    LV_DIR_BOTTOM   = (1 << 3),
    LV_DIR_HOR      = LV_DIR_LEFT | LV_DIR_RIGHT,
    LV_DIR_VER      = LV_DIR_TOP | LV_DIR_BOTTOM,
    LV_DIR_ALL      = LV_DIR_HOR | LV_DIR_VER,
} lv_dir_t;

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
void lv_area_set(lv_area_t * area_p, int32_t x1, int32_t y1, int32_t x2, int32_t y2);

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
int32_t lv_area_get_width(const lv_area_t * area_p);

/**
 * Get the height of an area
 * @param area_p pointer to an area
 * @return the height of the area (if y1 == y2 -> height = 1)
 */
int32_t lv_area_get_height(const lv_area_t * area_p);

/**
 * Set the width of an area
 * @param area_p pointer to an area
 * @param w the new width of the area (w == 1 makes x1 == x2)
 */
void lv_area_set_width(lv_area_t * area_p, int32_t w);

/**
 * Set the height of an area
 * @param area_p pointer to an area
 * @param h the new height of the area (h == 1 makes y1 == y2)
 */
void lv_area_set_height(lv_area_t * area_p, int32_t h);

/**
 * Return with area of an area (x * y)
 * @param area_p pointer to an area
 * @return size of area
 */
uint32_t lv_area_get_size(const lv_area_t * area_p);

void lv_area_increase(lv_area_t * area, int32_t w_extra, int32_t h_extra);

void lv_area_move(lv_area_t * area, int32_t x_ofs, int32_t y_ofs);

/**
 * Align an area to another
 * @param base an area where the other will be aligned
 * @param to_align the area to align
 * @param align `LV_ALIGN_...`
 * @param ofs_x X offset
 * @param ofs_y Y offset
 */
void lv_area_align(const lv_area_t * base, lv_area_t * to_align, lv_align_t align, int32_t ofs_x, int32_t ofs_y);

/**
 * Transform a point
 * @param point         pointer to a point
 * @param angle         angle with 0.1 resolutions (123 means 12.3°)
 * @param scale_x       horizontal zoom, 256 means 100%
 * @param scale_y       vertical zoom, 256 means 100%
 * @param pivot         pointer to the pivot point of the transformation
 * @param zoom_first    true: zoom first and rotate after that; else: opposite order
 */
void lv_point_transform(lv_point_t * point, int32_t angle, int32_t scale_x, int32_t scale_y, const lv_point_t * pivot,
                        bool zoom_first);

/**
 * Transform an array of points
 * @param points        pointer to an array of points
 * @param count         number of points in the array
 * @param angle         angle with 0.1 resolutions (123 means 12.3°)
 * @param scale_x       horizontal zoom, 256 means 100%
 * @param scale_y       vertical zoom, 256 means 100%
 * @param pivot         pointer to the pivot point of the transformation
 * @param zoom_first    true: zoom first and rotate after that; else: opposite order
 */
void lv_point_array_transform(lv_point_t * points, size_t count, int32_t angle, int32_t scale_x, int32_t scale_y,
                              const lv_point_t * pivot,
                              bool zoom_first);

lv_point_t lv_point_from_precise(const lv_point_precise_t * p);

lv_point_precise_t lv_point_to_precise(const lv_point_t * p);

void lv_point_set(lv_point_t * p, int32_t x, int32_t y);

void lv_point_precise_set(lv_point_precise_t * p, lv_value_precise_t x, lv_value_precise_t y);

void lv_point_swap(lv_point_t * p1, lv_point_t * p2);

void lv_point_precise_swap(lv_point_precise_t * p1, lv_point_precise_t * p2);

/**********************
 *      MACROS
 **********************/

#define LV_COORD_TYPE_SHIFT    (29U)

#define LV_COORD_TYPE_MASK     (3 << LV_COORD_TYPE_SHIFT)
#define LV_COORD_TYPE(x)       ((x) & LV_COORD_TYPE_MASK)  /*Extract type specifiers*/
#define LV_COORD_PLAIN(x)      ((x) & ~LV_COORD_TYPE_MASK) /*Remove type specifiers*/

#define LV_COORD_TYPE_PX       (0 << LV_COORD_TYPE_SHIFT)
#define LV_COORD_TYPE_SPEC     (1 << LV_COORD_TYPE_SHIFT)
#define LV_COORD_TYPE_PX_NEG   (3 << LV_COORD_TYPE_SHIFT)

#define LV_COORD_IS_PX(x)       (LV_COORD_TYPE(x) == LV_COORD_TYPE_PX || LV_COORD_TYPE(x) == LV_COORD_TYPE_PX_NEG)
#define LV_COORD_IS_SPEC(x)     (LV_COORD_TYPE(x) == LV_COORD_TYPE_SPEC)

#define LV_COORD_SET_SPEC(x)    ((x) | LV_COORD_TYPE_SPEC)

/** Max coordinate value */
#define LV_COORD_MAX            ((1 << LV_COORD_TYPE_SHIFT) - 1)
#define LV_COORD_MIN            (-LV_COORD_MAX)

/*Special coordinates*/
#define LV_SIZE_CONTENT         LV_COORD_SET_SPEC(LV_COORD_MAX)
#define LV_PCT_STORED_MAX       (LV_COORD_MAX - 1)
#if LV_PCT_STORED_MAX % 2 != 0
#error LV_PCT_STORED_MAX should be an even number
#endif
#define LV_PCT_POS_MAX          (LV_PCT_STORED_MAX / 2)
#define LV_PCT(x)               (LV_COORD_SET_SPEC(((x) < 0 ? (LV_PCT_POS_MAX - LV_MAX((x), -LV_PCT_POS_MAX)) : LV_MIN((x), LV_PCT_POS_MAX))))
#define LV_COORD_IS_PCT(x)      ((LV_COORD_IS_SPEC(x) && LV_COORD_PLAIN(x) <= LV_PCT_STORED_MAX))
#define LV_COORD_GET_PCT(x)     (LV_COORD_PLAIN(x) > LV_PCT_POS_MAX ? LV_PCT_POS_MAX - LV_COORD_PLAIN(x) : LV_COORD_PLAIN(x))

LV_EXPORT_CONST_INT(LV_COORD_MAX);
LV_EXPORT_CONST_INT(LV_COORD_MIN);
LV_EXPORT_CONST_INT(LV_SIZE_CONTENT);

/**
 * Convert a percentage value to `int32_t`.
 * Percentage values are stored in special range
 * @param x the percentage (0..1000)
 * @return a coordinate that stores the percentage
 */
int32_t lv_pct(int32_t x);

int32_t lv_pct_to_px(int32_t v, int32_t base);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
