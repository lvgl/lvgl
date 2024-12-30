/**
 * MIT License
 *
 * -----------------------------------------------------------------------------
 * Copyright (c) 2008-24 Think Silicon Single Member PC
 * -----------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next paragraph)
 * shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/**
 * @file lv_draw_nema_gfx.h
 *
 */

#ifndef LV_DRAW_NEMA_GFX_UTILS_H
#define LV_DRAW_NEMA_GFX_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_NEMA_GFX
#include "../sw/lv_draw_sw.h"

#include "nema_core.h"
#include "nema_utils.h"
#include "nema_error.h"
#include "nema_provisional.h"
#include "nema_vg.h"

/*********************
 *      DEFINES
 *********************/

#ifndef NEMA_VIRT2PHYS
#define NEMA_VIRT2PHYS
#else
uintptr_t NEMA_VIRT2PHYS(void * addr);
#endif

/*Color depth: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888)*/
#if LV_COLOR_DEPTH == 8
#define LV_NEMA_GFX_COLOR_FORMAT NEMA_L8
#define LV_NEMA_GFX_FORMAT_MULTIPLIER 1
#elif LV_COLOR_DEPTH == 16
#define LV_NEMA_GFX_COLOR_FORMAT NEMA_RGB565
#define LV_NEMA_GFX_FORMAT_MULTIPLIER 2
#elif LV_COLOR_DEPTH == 24
#define LV_NEMA_GFX_COLOR_FORMAT NEMA_BGR24
#define LV_NEMA_GFX_FORMAT_MULTIPLIER 3
#elif LV_COLOR_DEPTH == 32
#define LV_NEMA_GFX_COLOR_FORMAT NEMA_BGRA8888
#define LV_NEMA_GFX_FORMAT_MULTIPLIER 4
#else
/*Can't use GPU with other formats*/
#error Selected Color Depth Not Supported
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Check if  `lv_color_format_t` is supported.
 * @param     cf  The LVGL color format
 * @return        True/false
 */
bool lv_nemagfx_is_cf_supported(lv_color_format_t cf);

/**
 * Convert a `lv_color_format_t` to a Nema color format.
 * @param     cf  The LVGL color format
 * @return        The Nema color format
 */
uint32_t lv_nemagfx_cf_to_nema(lv_color_format_t cf);

/**
 * Get NemaGFX blending mode
 *
 * @param[in] lv_blend_mode The LVGL blend mode
 *
 * @return NemaGFX blending mode
 *
 */
uint32_t lv_nemagfx_blending_mode(lv_blend_mode_t lv_blend_mode);


/**
 * Get NemaGFX blending mode
 *
 * @param[in] gradient NemaGFX Gradient Buffer
 *
 * @param[in] lv_grad Gradient descriptor
 *
 * @param[in] opa Descriptor's opacity
 *
*/
void lv_nemagfx_grad_set(NEMA_VG_GRAD_HANDLE gradient, lv_grad_dsc_t lv_grad, lv_opa_t opa);

/**********************
 *      MACROS
 **********************/

#endif  /*LV_USE_NEMA_GFX*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_NEMA_GFX_UTILS_H*/
