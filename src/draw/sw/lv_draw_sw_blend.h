/**
 * @file lv_draw_sw_blend.h
 *
 */

#ifndef LV_DRAW_SW_BLEND_H
#define LV_DRAW_SW_BLEND_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_color.h"
#include "../../misc/lv_area.h"
#include "../../misc/lv_style.h"
#include "../lv_draw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const lv_area_t * blend_area;    /**< the area with absolute coordinates to fill on `dest_buf`*/
    const lv_color_t * src_buf;     /**<  pointer to the destination buffer*/
    lv_color_t color;               /**< fill color*/
    lv_opa_t * mask;                /**< NULL if ignored, or an alpha mask to apply on `fill_area`*/
    lv_draw_mask_res_t mask_res;
    const lv_area_t * mask_area;    /**< the area with absolute coordinates to fill on `dest_buf`*/
    lv_opa_t opa;                   /**< overall opacity*/
    lv_blend_mode_t blend_mode;     /**< E.g. LV_BLEND_MODE_ADDITIVE*/
}lv_draw_sw_blend_dsc_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_draw_sw_blend(lv_draw_t * draw, const lv_draw_sw_blend_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_BLEND_H*/
