/**
 * @file lv_font_cache.h
 *
 */

#ifndef LV_FONT_CACHE_H
#define LV_FONT_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_font.h"

#if LV_FONT_CACHE_GLYPH_CNT > 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the built-in fonts
 */
void lv_font_cache_init(uint32_t max_glyph_cnt);

/**
 * Deinitialize the built-in fonts
 */
void lv_font_cache_deinit(void);

const void * lv_font_cache_get_glyph_bitmap(lv_font_glyph_dsc_t * g_dsc, lv_draw_buf_t * draw_buf);

/**
 * Release reference to the glyph bitmap and descriptor.
 * @param font pointer to font
 * @param g_dsc pointer to glyph descriptor
 */
void lv_font_cache_release_glyph(const lv_font_t * font, lv_font_glyph_dsc_t * g_dsc);

/**********************
 *      MACROS
 **********************/

#endif /*LV_FONT_CACHE_GLYPH_CNT */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FONT_CACHE_H*/
