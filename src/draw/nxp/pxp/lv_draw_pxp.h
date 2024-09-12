/**
 * @file lv_draw_pxp.h
 *
 */

/**
 * Copyright 2022-2024 NXP
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_DRAW_PXP_H
#define LV_DRAW_PXP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_PXP
#if LV_USE_DRAW_PXP || LV_USE_ROTATE_PXP
#include "../../sw/lv_draw_sw_private.h"
#include "../../../misc/lv_area_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef lv_draw_sw_unit_t lv_draw_pxp_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_pxp_init(void);

void lv_draw_pxp_deinit(void);

void lv_draw_pxp_rotate(const void * src_buf, void * dest_buf, int32_t src_width, int32_t src_height,
                        int32_t src_stride, int32_t dest_stride, lv_display_rotation_t rotation,
                        lv_color_format_t cf);

#if LV_USE_DRAW_PXP
void lv_draw_buf_pxp_init_handlers(void);

void lv_draw_pxp_fill(lv_draw_unit_t * draw_unit, const lv_draw_fill_dsc_t * dsc,
                      const lv_area_t * coords);

void lv_draw_pxp_img(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * dsc,
                     const lv_area_t * coords);

void lv_draw_pxp_layer(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                       const lv_area_t * coords);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_DRAW_PXP*/
#endif /*LV_USE_DRAW_PXP || LV_USE_ROTATE_PXP*/
#endif /*LV_USE_PXP*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_PXP_H*/
