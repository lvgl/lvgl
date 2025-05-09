/**
 * @file lv_draw_vglite.h
 *
 */

/**
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_DRAW_VGLITE_H
#define LV_DRAW_VGLITE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_DRAW_VGLITE
#include "../../lv_draw_private.h"
#include "../../../display/lv_display_private.h"
#include "../../../misc/lv_area_private.h"

#include "../../lv_draw_triangle.h"
#include "../../lv_draw_label.h"
#include "../../lv_draw_image.h"
#include "../../lv_draw_line.h"
#include "../../lv_draw_arc.h"
#include "vg_lite.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct vglite_draw_task {
    lv_draw_task_t * t;
    vg_lite_path_t * path;
    vg_lite_linear_gradient_t * gradient;
    int32_t * path_data;
} vglite_draw_task_t;

typedef struct lv_draw_vglite_unit {
    lv_draw_unit_t base_unit;
    vglite_draw_task_t * task_act;
#if LV_USE_OS
    lv_thread_sync_t sync;
    lv_thread_t thread;
    volatile bool inited;
    volatile bool exit_status;
#endif
#if LV_USE_VGLITE_DRAW_ASYNC
    volatile bool wait_for_finish;
#endif
} lv_draw_vglite_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_buf_vglite_init_handlers(void);

void lv_draw_vglite_init(void);

void lv_draw_vglite_deinit(void);

void lv_draw_vglite_arc(vglite_draw_task_t * vglite_task);

void lv_draw_vglite_border(vglite_draw_task_t * vglite_task);

void lv_draw_vglite_fill(vglite_draw_task_t * vglite_task);

void lv_draw_vglite_img(vglite_draw_task_t * vglite_task);

void lv_draw_vglite_label(vglite_draw_task_t * vglite_task);

void lv_draw_vglite_layer(vglite_draw_task_t * vglite_task);

void lv_draw_vglite_line(vglite_draw_task_t * vglite_task);

void lv_draw_vglite_triangle(vglite_draw_task_t * vglite_task);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_DRAW_VGLITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_VGLITE_H*/
