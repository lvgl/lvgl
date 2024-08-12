/**
 * @file lv_line_private.h
 *
 */

#ifndef LV_LINE_PRIVATE_H
#define LV_LINE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj_private.h"
#include "lv_line.h"

#if LV_USE_LINE != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Data of line */
struct lv_line_t {
    lv_obj_t obj;
    union {
        const lv_point_precise_t * constant;
        lv_point_precise_t * mut;
    } point_array;                      /**< Pointer to an array with the points of the line*/
    uint32_t point_num;                 /**< Number of points in 'point_array'*/
    uint32_t y_inv : 1;                  /**< 1: y == 0 will be on the bottom*/
    uint32_t point_array_is_mutable : 1; /**< whether the point array is const or mutable*/
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_LINE != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LINE_PRIVATE_H*/
