/**
 * @file lv_draw_sifli_epic.h
 *
 */

/**
 * Copyright 2024 SiFli Technologies
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_DRAW_SIFLI_EPIC_H
#define LV_DRAW_SIFLI_EPIC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_SIFLI_EPIC
#include "../../lv_draw_private.h"
#include "../../../display/lv_display_private.h"
#include "../../../misc/lv_area_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * SiFli EPIC draw unit structure.
 * Similar to NXP PXP's lv_draw_pxp_unit_t.
 */
typedef struct _lv_draw_sifli_epic_unit_t {
    lv_draw_unit_t base_unit;       /**< Base draw unit */
    lv_draw_task_t * task_act;      /**< Currently active task */

#if LV_USE_SIFLI_EPIC_DRAW_THREAD
    lv_thread_sync_t sync;          /**< Thread synchronization primitive */
    lv_thread_t thread;             /**< Render thread */
    volatile bool inited;           /**< Initialization status */
    volatile bool exit_status;      /**< Exit status for thread */
#endif
} lv_draw_sifli_epic_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize SiFli EPIC draw unit.
 * This function should be called during display initialization.
 */
void lv_draw_sifli_epic_init(void);

/**
 * Deinitialize SiFli EPIC draw unit.
 * This function should be called during display deinitialization.
 */
void lv_draw_sifli_epic_deinit(void);

/**
 * Initialize draw buffer handlers for EPIC.
 */
void lv_draw_buf_sifli_epic_init_handlers(void);

/**
 * EPIC fill operation.
 * @param t Draw task
 */
void lv_draw_sifli_epic_fill(lv_draw_task_t * t);

/**
 * EPIC border draw operation.
 * @param t Draw task
 */
void lv_draw_sifli_epic_border(lv_draw_task_t * t);

/**
 * EPIC label draw operation.
 * @param t Draw task
 */
void lv_draw_sifli_epic_label(lv_draw_task_t * t);

/**
 * EPIC image draw operation.
 * @param t Draw task
 */
void lv_draw_sifli_epic_img(lv_draw_task_t * t);

/**
 * EPIC layer blend operation.
 * @param t Draw task
 */
void lv_draw_sifli_epic_layer(lv_draw_task_t * t);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SIFLI_EPIC*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SIFLI_EPIC_H*/
