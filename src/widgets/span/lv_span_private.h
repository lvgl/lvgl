/**
 * @file lv_span_private.h
 *
 */

#ifndef LV_SPAN_PRIVATE_H
#define LV_SPAN_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj_private.h"
#include "lv_span.h"

#if LV_USE_SPAN != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_span_t {
    char * txt;                /**<  a pointer to display text */
    lv_obj_t * spangroup;      /**<  a pointer to spangroup */
    lv_style_t style;          /**<  display text style */
    uint32_t static_flag : 1;  /**<  the text is static flag */

    lv_point_t trailing_pos;
    int32_t trailing_height;
};

/** Data of label*/
struct _lv_spangroup_t {
    lv_obj_t obj;
    int32_t lines;
    int32_t indent;         /**<  first line indent */
    int32_t cache_w;        /**<  the cache automatically calculates the width */
    int32_t cache_h;        /**<  similar cache_w */
    lv_ll_t  child_ll;
    uint32_t mode : 2;      /**<  details see lv_span_mode_t */
    uint32_t overflow : 1;  /**<  details see lv_span_overflow_t */
    uint32_t refresh : 1;   /**<  the spangroup need refresh cache_w and cache_h */
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_SPAN != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SPAN_PRIVATE_H*/
