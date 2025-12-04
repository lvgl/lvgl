/**
 * @file lv_vg_lite_bitmap_font_cache.h
 *
 */

#ifndef LV_VG_LITE_BITMAP_FONT_CACHE_H
#define LV_VG_LITE_BITMAP_FONT_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_VG_LITE

#include "../../font/lv_font.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_vg_lite_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Initialize the bitmap font cache for VG-Lite draw unit
 * @param unit pointer to the VG-Lite draw unit
 * @param cache_cnt number of cache entries to allocate
 */
void lv_vg_lite_bitmap_font_cache_init(struct _lv_draw_vg_lite_unit_t * unit, uint32_t cache_cnt);

/**
 * @brief Deinitialize the bitmap font cache for VG-Lite draw unit
 * @param unit pointer to the VG-Lite draw unit
 */
void lv_vg_lite_bitmap_font_cache_deinit(struct _lv_draw_vg_lite_unit_t * unit);

/**
 * @brief Get the bitmap font cache entry for a given font and letter
 * @param unit pointer to the VG-Lite draw unit
 * @param g_dsc pointer to the glyph descriptor
 * @return pointer to the draw buffer containing the cached bitmap font glyph, or NULL if the glyph ID is 0 or if cache creation fails
 */
lv_draw_buf_t * lv_vg_lite_bitmap_font_cache_get(struct _lv_draw_vg_lite_unit_t * unit,
                                                 const lv_font_glyph_dsc_t * g_dsc);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_VG_LITE_BITMAP_FONT_CACHE_H*/
