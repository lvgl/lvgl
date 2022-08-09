/**
 * @file lv_draw_transform.h
 *
 */

#ifndef LV_DRAW_TRANSFORM_H
#define LV_DRAW_TRANSFORM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "../misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_draw_ctx_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_transform(struct _lv_draw_ctx_t * draw_ctx, const lv_area_t * dest_area, const void * src_buf,
                       lv_coord_t src_w, lv_coord_t src_h,
                       lv_coord_t src_stride, const lv_draw_img_dsc_t * draw_dsc, lv_img_cf_t cf, lv_color_t * cbuf, lv_opa_t * abuf);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_TRANSFORM_H*/
