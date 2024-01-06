/**
 * @file lv_draw_mask_rect.h
 *
 */

#ifndef LV_DRAW_MASK_RECT_H
#define LV_DRAW_MASK_RECT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "../misc/lv_color.h"
#include "../misc/lv_area.h"
#include "../misc/lv_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_draw_dsc_base_t base;

    lv_area_t area;
    int32_t radius;
} lv_draw_mask_rect_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a rectangle mask draw descriptor.
 * @param dsc       pointer to a draw descriptor
 */
LV_ATTRIBUTE_FAST_MEM void lv_draw_mask_rect_dsc_init(lv_draw_mask_rect_dsc_t * dsc);

/**
 * Create a draw task to mask a rectangle from the buffer
 * @param layer     pointer to a layer
 * @param dsc       pointer to a draw descriptor
 */
void lv_draw_mask_rect(lv_layer_t * layer, const lv_draw_mask_rect_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_MASK_RECT_H*/
