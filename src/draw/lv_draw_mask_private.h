/**
 * @file lv_draw_mask_private.h
 *
 */

#ifndef LV_DRAW_MASK_PRIVATE_H
#define LV_DRAW_MASK_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_mask.h"
#include "lv_draw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_draw_mask_rect_dsc_t {
    lv_draw_dsc_base_t base;

    /**The area t mask.*/
    lv_area_t area;

    /**The radius of masking*/
    int32_t radius;

    /**0: clear the content out of the `area`.
     * 1: don't touch the area out of `area`*/
    uint32_t keep_outside : 1;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_MASK_PRIVATE_H*/
