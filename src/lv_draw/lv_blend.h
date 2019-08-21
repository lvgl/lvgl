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

void lv_blend_color(lv_color_t * dest_buf, lv_img_cf_t dest_cf, lv_coord_t len,
                   lv_color_t color, lv_img_cf_t src_cf,
                   lv_opa_t * mask, lv_mask_res_t mask_res, lv_opa_t opa, lv_blend_mode_t mode);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BLIT_H*/
