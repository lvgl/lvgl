/**
 * @file lv_draw_sw_blend_to_i4.h
 *
 */

#ifndef LV_DRAW_SW_BLEND_TO_I4_H
#define LV_DRAW_SW_BLEND_TO_I4_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_draw_sw.h"
#if LV_USE_DRAW_SW

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
 * Solid color fill into an I4 (4-bit indexed) destination buffer.
 *
 * Pixel packing in the destination follows the wire convention used by
 * indexed-color AMOLED panels (e.g. CO5300): two pixels per byte, the
 * lower-x pixel in the upper nibble (D[7:4]), the higher-x pixel in the
 * lower nibble (D[3:0]). Stride is in bytes; for an N-pixel-wide row
 * the natural stride is `(N + 1) / 2`.
 *
 * The palette used to quantize `dsc->color` is taken from the active
 * display via `lv_display_get_palette()`. If none is set, a default
 * 16-entry grayscale palette is used.
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_sw_blend_color_to_i4(lv_draw_sw_blend_fill_dsc_t * dsc);

/**
 * Image (per-pixel) blend into an I4 destination buffer. Supports the
 * same set of source color formats as the I1/L8 blenders. Non-normal
 * blend modes (additive / subtractive / multiply / difference) round-trip
 * each destination pixel through the palette: index → palette RGB → blend
 * → quantize back.
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_sw_blend_image_to_i4(lv_draw_sw_blend_image_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_SW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_BLEND_TO_I4_H*/
