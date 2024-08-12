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
struct lv_draw_mask_rect_dsc_t {
    lv_draw_dsc_base_t base;

    lv_area_t area;
    int32_t radius;
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
