/**
 * @file lv_draw_g2d.h
 *
 */

/**
 * Copyright 2022, 2023，2024 G2D
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_DRAW_G2D_H
#define LV_DRAW_G2D_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_G2D
#include "../sw/lv_draw_sw.h"
#include "lv_g2d_utils.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef lv_layer_t lv_g2d_layer_t;

typedef struct {
    lv_draw_unit_t base_unit;
    struct _lv_draw_task_t * task_act;
#if LV_USE_OS
    lv_thread_sync_t sync;
    lv_thread_t thread;
#endif
} lv_draw_g2d_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_buf_g2d_init_handlers(void);

void lv_draw_g2d_init(void);

void lv_draw_g2d_deinit(void);

void lv_draw_g2d_fill(lv_draw_unit_t * draw_unit, const lv_draw_fill_dsc_t * dsc,
                      const lv_area_t * coords);

void lv_draw_g2d_img(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * dsc,
                     const lv_area_t * coords);

void lv_draw_g2d_layer(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                       const lv_area_t * coords);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_DRAW_G2D*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_G2D_H*/
