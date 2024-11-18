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
 * @file lv_draw_nema_gfx_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_nema_gfx.h"

#if LV_USE_NEMA_GFX

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

uint32_t lv_nemagfx_cf_to_nema(lv_color_format_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_A1:
            return NEMA_A1;
        case LV_COLOR_FORMAT_A2:
            return NEMA_A2;
        case LV_COLOR_FORMAT_A4:
            return NEMA_A4;
        case LV_COLOR_FORMAT_A8:
            return NEMA_A8;
        case LV_COLOR_FORMAT_I1:
            return NEMA_L1;
        case LV_COLOR_FORMAT_I2:
            return NEMA_L2;
        case LV_COLOR_FORMAT_I4:
            return NEMA_L4;
        case LV_COLOR_FORMAT_L8:
        case LV_COLOR_FORMAT_I8:
            return NEMA_L8;
        case LV_COLOR_FORMAT_RGB565:
            return NEMA_RGB565;
        case LV_COLOR_FORMAT_RGB888:
            return NEMA_BGR24;
        case LV_COLOR_FORMAT_ARGB8888:
            return NEMA_BGRA8888;
        case LV_COLOR_FORMAT_XRGB8888:
            return NEMA_BGRX8888;
        case LV_COLOR_FORMAT_NEMA_TSC4:
            return NEMA_TSC4;
        case LV_COLOR_FORMAT_NEMA_TSC6:
            return NEMA_TSC6;
        case LV_COLOR_FORMAT_NEMA_TSC6A:
            return NEMA_TSC6A;
        case LV_COLOR_FORMAT_NEMA_TSC12:
            return NEMA_TSC12;
        case LV_COLOR_FORMAT_NEMA_TSC12A:
            return NEMA_TSC12A;
            /*Guard for previous NemaGFX Version*/
#ifdef NEMA_TSC6AP
        case LV_COLOR_FORMAT_NEMA_TSC6AP:
            return NEMA_TSC6AP;
#endif
        default:
            return LV_NEMA_GFX_COLOR_FORMAT;
    }
}

uint32_t lv_nemagfx_blending_mode(lv_blend_mode_t lv_blend_mode)
{
    uint32_t blending_mode;
    switch(lv_blend_mode) {
        case LV_BLEND_MODE_NORMAL:
            blending_mode = NEMA_BL_SIMPLE;
            break;
        case LV_BLEND_MODE_ADDITIVE:
            blending_mode = NEMA_BL_ADD;
            break;
        case LV_BLEND_MODE_MULTIPLY:
            blending_mode = nema_blending_mode(NEMA_BF_DESTCOLOR, NEMA_BF_INVSRCALPHA, NEMA_BLOP_SRC_PREMULT);
            break;
        default:
            blending_mode = NEMA_BL_SIMPLE;
            break;
    }
    return blending_mode;
}

void lv_nemagfx_grad_set(NEMA_VG_GRAD_HANDLE gradient, lv_grad_dsc_t lv_grad, lv_opa_t opa)
{

    float stops[LV_GRADIENT_MAX_STOPS];
    color_var_t colors[LV_GRADIENT_MAX_STOPS];

    uint32_t cnt = LV_MAX(lv_grad.stops_count, LV_GRADIENT_MAX_STOPS);

    for(uint8_t i = 0; i < cnt; i++) {
        stops[i] = (float)(lv_grad.stops[i].frac) / 255.f;
        colors[i].a = LV_OPA_MIX2(lv_grad.stops[i].opa, opa);
        colors[i].r = lv_grad.stops[i].color.red;
        colors[i].g = lv_grad.stops[i].color.green;
        colors[i].b = lv_grad.stops[i].color.blue;
    }

    nema_vg_grad_set(gradient, cnt, stops, colors);
}

bool lv_nemagfx_is_cf_supported(lv_color_format_t cf)
{

    switch(cf) {
        case LV_COLOR_FORMAT_ARGB8565:
        case LV_COLOR_FORMAT_RGB565A8:
            return false;
        default:
            break;
    }
    return true;
}

#endif /*LV_USE_NEMA_GFX*/
