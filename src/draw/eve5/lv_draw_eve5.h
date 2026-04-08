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

/*********************
 * DEFINES
 *********************/

/**********************
 * TYPEDEFS
 **********************/

typedef struct _lv_draw_buf_t lv_draw_buf_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_eve5_init(EVE_HalContext *hal, Esd_GpuAlloc *allocator);
void lv_draw_eve5_deinit(void);

/**
 * Detach the GPU handle from a draw_buf's vram_res, transferring ownership
 * to the caller. Used by the display driver to take screen texture ownership.
 */
bool lv_draw_eve5_detach_gpu_handle(lv_draw_buf_t *buf, Esd_GpuHandle *out_handle);

#endif /* LV_USE_DRAW_EVE5 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_DRAW_EVE5_H */
