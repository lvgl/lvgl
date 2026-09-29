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
#include "../../lvgl_public.h"

#if LV_USE_DRAW_EVE5

#include "EVE_Hal.h"
#include "EVE_GpuAlloc.h"

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the EVE5 draw unit and register it with LVGL.
 * @param hal       pointer to initialized EVE HAL context
 * @param allocator pointer to GPU memory allocator
 */
void lv_draw_eve5_init(EVE_HalContext *hal, EVE_GpuAlloc *allocator);

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

struct _lv_draw_unit_t;
/**
 * Coprocessor-reset hook: invalidates cached bitmap-handle bindings so the
 * EVE5 draw unit re-issues CMD_ROMFONT / CMD_SETFONT2 on next text render.
 * Called by lv_eve5_reset_coprocessor after EVE_Util_resetCoprocessor;
 * safe to pass any lv_draw_unit_t (no-op when @p draw_unit is not the EVE5
 * draw unit). Public so the display driver can dispatch without knowing
 * the internal lv_draw_eve5_unit_t layout.
 */
void lv_draw_eve5_handle_coprocessor_reset(struct _lv_draw_unit_t * draw_unit);

/**
 * What the draw unit moved between CPU memory and EVE memory since the program
 * started, besides display lists: the pixels it wrote into EVE memory (images,
 * glyphs, the textures of the software renderer), the images the coprocessor
 * decoded or loaded (files, SD card, flash), the pixels it read back into CPU
 * memory, and the CPU memory it hashed to tell whether its EVE copies are
 * still current (without LV_USE_DRAW_VRAM).
 */
typedef struct {
    uint64_t upload_bytes;
    uint32_t uploads;
    uint32_t hw_decodes;
    uint64_t download_bytes;
    uint32_t downloads;
    uint64_t hash_bytes;
    uint32_t hashes;
} lv_draw_eve5_stats_t;

const lv_draw_eve5_stats_t * lv_draw_eve5_get_stats(void);

#endif /* LV_USE_DRAW_EVE5 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_DRAW_EVE5_H */
