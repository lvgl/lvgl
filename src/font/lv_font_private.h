/**
 * @file lv_font_private.h
 *
 */

#ifndef LV_FONT_PRIVATE_H
#define LV_FONT_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl_public.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

int32_t lv_font_glyph_dsc_compare(const lv_font_glyph_dsc_t * lhs, const lv_font_glyph_dsc_t * rhs);

/*
 * Internal implementation of @ref lv_font_get_glyph_bitmap
 */
const void * lv_font_get_glyph_bitmap_internal(lv_font_glyph_dsc_t * g_dsc, lv_draw_buf_t * draw_buf);

/*
 * Internal implementation of @ref lv_font_get_glyph_static_bitmap
 */
const void * lv_font_get_glyph_static_bitmap_internal(lv_font_glyph_dsc_t * g_dsc);

/*
 * Internal implementation of @ref lv_font_glyph_release_draw_data
 */
void lv_font_glyph_release_draw_data_internal(lv_font_glyph_dsc_t * g_dsc);

/*
 * Internal implementation of @ref lv_font_get_glyph_dsc
 */
bool lv_font_get_glyph_dsc_internal(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t letter,
                                    uint32_t letter_next);

/*
 * Internal implementation of @ref lv_font_get_glyph_width
 */
uint16_t lv_font_get_glyph_width_internal(const lv_font_t * font, uint32_t letter, uint32_t letter_next);

/*
 * Internal implementation of @ref lv_font_get_line_height
 */
static inline int32_t lv_font_get_line_height_internal(const lv_font_t * font)
{
    LV_ASSERT(font);
    return font->line_height;
}

/*
 * Internal implementation of @ref lv_font_has_static_bitmap_internal
 */
static inline bool lv_font_has_static_bitmap_internal(const lv_font_t * font)
{
    LV_ASSERT(font);
    return font->static_bitmap;
}

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FONT_PRIVATE_H*/

