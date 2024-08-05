/**
 * @file lv_line.h
 *
 */

#ifndef LV_LINE_H
#define LV_LINE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"
#if LV_USE_LINE != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_line_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a line object
 * @param parent pointer to an object, it will be the parent of the new line
 * @return pointer to the created line
 */
lv_obj_t * lv_line_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set an array of points. The line object will connect these points.
 * @param obj           pointer to a line object
 * @param points        an array of points. Only the address is saved, so the array needs to be alive while the line exists
 * @param point_num     number of points in 'point_a'
 */
void lv_line_set_points(lv_obj_t * obj, const lv_point_precise_t points[], uint32_t point_num);

/**
 * Set a non-const array of points. Identical to `lv_line_set_points` except the array may be retrieved by `lv_line_get_points_mutable`.
 * @param obj           pointer to a line object
 * @param points        a non-const array of points. Only the address is saved, so the array needs to be alive while the line exists.
 * @param point_num     number of points in 'point_a'
 */
void lv_line_set_points_mutable(lv_obj_t * obj, lv_point_precise_t points[], uint32_t point_num);

/**
 * Enable (or disable) the y coordinate inversion.
 * If enabled then y will be subtracted from the height of the object,
 * therefore the y = 0 coordinate will be on the bottom.
 * @param obj       pointer to a line object
 * @param en        true: enable the y inversion, false:disable the y inversion
 */
void lv_line_set_y_invert(lv_obj_t * obj, bool en);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the pointer to the array of points.
 * @param obj           pointer to a line object
 * @return              const pointer to the array of points
 */
const lv_point_precise_t * lv_line_get_points(lv_obj_t * obj);

/**
 * Get the number of points in the array of points.
 * @param obj           pointer to a line object
 * @return              number of points in array of points
 */
uint32_t lv_line_get_point_count(lv_obj_t * obj);

/**
 * Check the mutability of the stored point array pointer.
 * @param obj           pointer to a line object
 * @return              true: the point array pointer is mutable, false: constant
 */
bool lv_line_is_point_array_mutable(lv_obj_t * obj);

/**
 * Get a pointer to the mutable array of points or NULL if it is not mutable
 * @param obj           pointer to a line object
 * @return              pointer to the array of points. NULL if not mutable.
 */
lv_point_precise_t * lv_line_get_points_mutable(lv_obj_t * obj);

/**
 * Get the y inversion attribute
 * @param obj       pointer to a line object
 * @return          true: y inversion is enabled, false: disabled
 */
bool lv_line_get_y_invert(const lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LINE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LINE_H*/
