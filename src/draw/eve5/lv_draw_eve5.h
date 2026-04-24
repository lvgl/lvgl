/**
 * @file lv_draw_eve5.h
 *
 * EVE5 (BT820) Draw Unit Public Header
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */
#ifndef LV_DRAW_EVE5_H
#define LV_DRAW_EVE5_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 * INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_EVE5

#include "EVE_Hal.h"
#include "Esd_GpuAlloc.h"

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the EVE5 draw unit and register it with LVGL.
 * @param hal       pointer to initialized EVE HAL context
 * @param allocator pointer to GPU memory allocator
 */
void lv_draw_eve5_init(EVE_HalContext *hal, Esd_GpuAlloc *allocator);

/**
 * Deinitialize the EVE5 draw unit.
 */
void lv_draw_eve5_deinit(void);

/**
 * Enable or disable the EVE5 draw unit.
 * When disabled, all tasks fall through to the SW renderer.
 * The EVE5 display driver still composites the result to screen.
 * @param enabled  true to accept tasks (default), false to decline all
 */
void lv_draw_eve5_set_enabled(bool enabled);

/**
 * Check whether the EVE5 draw unit is accepting tasks.
 * @return true if enabled
 */
bool lv_draw_eve5_get_enabled(void);

#endif /* LV_USE_DRAW_EVE5 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_DRAW_EVE5_H */
