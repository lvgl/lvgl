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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct lv_draw_vglite_unit {
    lv_draw_unit_t base_unit;
    lv_draw_task_t * task_act;
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

void lv_draw_vglite_arc(lv_draw_unit_t * draw_unit, const lv_draw_arc_dsc_t * dsc,
                        const lv_area_t * coords);

void lv_draw_vglite_border(lv_draw_unit_t * draw_unit, const lv_draw_border_dsc_t * dsc,
                           const lv_area_t * coords);

void lv_draw_vglite_fill(lv_draw_unit_t * draw_unit, const lv_draw_fill_dsc_t * dsc,
                         const lv_area_t * coords);

void lv_draw_vglite_img(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * dsc,
                        const lv_area_t * coords);

void lv_draw_vglite_label(lv_draw_unit_t * draw_unit, const lv_draw_label_dsc_t * dsc,
                          const lv_area_t * coords);

void lv_draw_vglite_layer(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                          const lv_area_t * coords);

void lv_draw_vglite_line(lv_draw_unit_t * draw_unit, const lv_draw_line_dsc_t * dsc);

void lv_draw_vglite_triangle(lv_draw_unit_t * draw_unit, const lv_draw_triangle_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_DRAW_VGLITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_VGLITE_H*/
