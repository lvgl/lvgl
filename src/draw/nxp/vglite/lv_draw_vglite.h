/**
 * @file lv_draw_vglite.h
 *
 */

/**
 * Copyright 2023 NXP
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
#include "../../sw/lv_draw_sw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef lv_layer_t lv_vglite_layer_t;

typedef struct {
    lv_draw_unit_t base_unit;
    struct _lv_draw_task_t * task_act;
#if LV_USE_OS
    lv_thread_sync_t sync;
    lv_thread_t thread;
#endif
} lv_draw_vglite_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_layer_t * lv_draw_vglite_layer_init(lv_disp_t * disp);

void lv_draw_vglite_layer_deinit(lv_disp_t * disp, lv_layer_t * layer);

void lv_draw_vglite_init(void);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_DRAW_VGLITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_VGLITE_H*/
