/**
 * @file lv_blit.h
 *
 */

#ifndef LV_BLIT_H
#define LV_BLIT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_misc/lv_color.h"
#include "../lv_misc/lv_area.h"
#include "lv_img_decoder.h"
#include "lv_mask.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_BLIT_MODE_NORMAL,
    LV_BLIT_MODE_ADDITIVE,
}lv_blend_mode_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_blend_fill(const lv_area_t * clip_area, const lv_area_t * fill_area,
        lv_color_t color, lv_opa_t * mask, lv_mask_res_t mask_res, lv_opa_t opa,
        lv_blend_mode_t mode);


void lv_blend_map(const lv_area_t * clip_area, const lv_area_t * map_area, const lv_color_t * map_buf,
                  const lv_opa_t * mask, lv_mask_res_t mask_res,
                  lv_opa_t opa, lv_blend_mode_t mode);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BLIT_H*/
