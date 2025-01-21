/**
 * @file lv_draw_g2d.h
 *
 */

/**
 * Copyright 2024 NXP
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

#include "../../../lv_conf_internal.h"

#if LV_USE_DRAW_G2D
#include "../../sw/lv_draw_sw_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct lv_draw_g2d_unit {
    lv_draw_sw_unit_t;

    void * g2d_handle;
} lv_draw_g2d_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_g2d_init(void);

void lv_draw_g2d_deinit(void);

void lv_draw_buf_g2d_init_handlers(void);

void lv_draw_g2d_fill(lv_draw_g2d_unit_t * draw_g2d_unit, const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_g2d_img(lv_draw_g2d_unit_t * draw_g2d_unit, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_G2D*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_G2D_H*/