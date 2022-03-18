/**
 * @file lv_draw_gles_img.h
 *
 */

#ifndef LV_DRAW_GLES_IMG_H
#define LV_DRAW_GLES_IMG_H


#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_GPU_GLES
#include "../lv_draw.h"



/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_res_t lv_draw_gles_draw_img(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * draw_dsc,
                     const lv_area_t * coords, const void * src);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_GLES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_GLES_IMG_H*/
