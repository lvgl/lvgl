/**
 * @file lv_draw_pxp.h
 *
 */

/**
 * Copyright 2022, 2023 NXP
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

#if LV_USE_DRAW_PXP
#include "../../sw/lv_draw_sw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef lv_layer_t lv_pxp_layer_t;

typedef struct {
    lv_draw_unit_t base_unit;
    struct _lv_draw_task_t * task_act;
#if LV_USE_OS
    lv_thread_sync_t sync;
    lv_thread_t thread;
#endif
} lv_draw_pxp_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_layer_t * lv_draw_pxp_layer_init(lv_disp_t * disp);

void lv_draw_pxp_layer_deinit(lv_disp_t * disp, lv_layer_t * layer);

void lv_draw_pxp_init(void);

void lv_draw_pxp_buffer_copy(lv_layer_t * layer,
                             void * dest_buf, lv_coord_t dest_stride, const lv_area_t * dest_area,
                             void * src_buf, lv_coord_t src_stride, const lv_area_t * src_area);

void lv_draw_pxp_bg_img(lv_draw_unit_t * draw_unit, const lv_draw_bg_img_dsc_t * dsc,
                        const lv_area_t * coords);

void lv_draw_pxp_fill(lv_draw_unit_t * draw_unit, const lv_draw_fill_dsc_t * dsc,
                      const lv_area_t * coords);

void lv_draw_pxp_img(lv_draw_unit_t * draw_unit, const lv_draw_img_dsc_t * dsc,
                     const lv_area_t * coords);

void lv_draw_pxp_layer(lv_draw_unit_t * draw_unit, const lv_draw_img_dsc_t * draw_dsc,
                       const lv_area_t * coords);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_DRAW_PXP*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_PXP_H*/
