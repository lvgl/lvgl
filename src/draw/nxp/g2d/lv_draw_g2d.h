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

#if LV_USE_G2D
#if LV_USE_DRAW_G2D || LV_USE_ROTATE_G2D
#include "../../sw/lv_draw_sw_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct lv_draw_g2d_unit {
    lv_draw_unit_t base_unit;
#if LV_USE_OS
    lv_draw_sw_thread_dsc_t thread_dsc;
#else
    lv_draw_task_t * task_act;
#endif
} lv_draw_g2d_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_g2d_init(void);

void lv_draw_g2d_deinit(void);

void lv_draw_buf_g2d_init_handlers(void);

void lv_draw_g2d_fill(lv_draw_task_t * t);

void lv_draw_g2d_img(lv_draw_task_t * t);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_G2D || LV_USE_ROTATE_G2D*/
#endif /*LV_USE_G2D*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_G2D_H*/
