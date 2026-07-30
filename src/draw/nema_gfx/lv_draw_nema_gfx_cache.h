/**
 * @file lv_draw_nema_gfx_cache.h
 * Copyright (c) 2026 STMicroelectronics.
 */

#ifndef LV_DRAW_NEMA_GFX_CACHE_H
#define LV_DRAW_NEMA_GFX_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl_public.h"

#if LV_NEMA_USE_CACHE

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_nema_gfx_invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area);
void lv_draw_nema_gfx_flush_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area);

/**********************
 *      MACROS
 **********************/

#endif /*LV_NEMA_USE_CACHE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_NEMA_GFX_CACHE_H*/
