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

typedef struct lv_draw_g2d_unit_t {
    lv_draw_unit_t base_unit;
    lv_draw_task_t * task_act;
#if LV_USE_OS
    lv_thread_sync_t sync;
    lv_thread_t thread;
    volatile bool inited;
    volatile bool exit_status;
#endif
} lv_draw_g2d_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_g2d_init(void);

void lv_draw_g2d_deinit(void);

void lv_draw_g2d_rotate(const void * src_buf, void * dest_buf, int32_t src_width, int32_t src_height,
                        int32_t src_stride, int32_t dest_stride, lv_display_rotation_t rotation,
                        lv_color_format_t cf);

void lv_draw_buf_g2d_init_handlers(void);

void lv_draw_g2d_fill(lv_draw_task_t * t);

void lv_draw_g2d_img(lv_draw_task_t * t);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_G2D || LV_USE_ROTATE_G2D*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_G2D_H*/
#endif /*LV_USE_G2D*/
