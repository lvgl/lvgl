/**
 * @file lv_freetype_harfbuzz.h
 *
 */

#ifndef LV_FREETYPE_HARFBUZZ_H
#define LV_FREETYPE_HARFBUZZ_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_FREETYPE && LV_USE_HARFBUZZ

#include "../../font/lv_font.h"
#include "../../misc/lv_bidi.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint32_t glyph_id;
    int32_t x_offset;
    int32_t y_offset;
    int32_t x_advance;
    int32_t y_advance;
    uint32_t cluster;
} lv_hb_glyph_info_t;

typedef struct {
    lv_hb_glyph_info_t * glyphs;
    uint32_t count;
} lv_hb_shaped_text_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Shape a UTF-8 text string using HarfBuzz.
 * Caller must free the result with lv_hb_shaped_text_destroy().
 * @param font pointer to an LVGL font (must be a FreeType font)
 * @param text UTF-8 encoded text
 * @param byte_len length of text in bytes
 * @param dir_hint text direction hint: LV_BASE_DIR_AUTO to let HarfBuzz detect direction,
 *                 LV_BASE_DIR_LTR or LV_BASE_DIR_RTL to force a direction (use when text
 *                 has already been BIDI-reordered to prevent double-reordering)
 * @return pointer to shaped text result, or NULL on failure
 */
lv_hb_shaped_text_t * lv_hb_shape_text(const lv_font_t * font, const char * text, uint32_t byte_len,
                                        lv_base_dir_t dir_hint);

/**
 * Free shaped text result.
 * @param shaped pointer to shaped text to free
 */
void lv_hb_shaped_text_destroy(lv_hb_shaped_text_t * shaped);

/**
 * Check if a font is a FreeType font that can use HarfBuzz shaping.
 * @param font pointer to an LVGL font
 * @return true if the font is a FreeType font
 */
bool lv_freetype_is_harfbuzz_font(const lv_font_t * font);

/**
 * Get the width of a shaped text string in pixels.
 * Shapes the text using HarfBuzz and sums up the x_advance values.
 * @param font pointer to an LVGL font (must be a FreeType font)
 * @param text UTF-8 encoded text
 * @param byte_len length of text in bytes
 * @param letter_space extra spacing between glyphs
 * @return width of the shaped text in pixels, or -1 on failure
 */
int32_t lv_hb_get_text_width(const lv_font_t * font, const char * text, uint32_t byte_len, int32_t letter_space);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_FREETYPE && LV_USE_HARFBUZZ */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_FREETYPE_HARFBUZZ_H */
