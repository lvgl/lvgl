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

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FONT_PRIVATE_H*/
