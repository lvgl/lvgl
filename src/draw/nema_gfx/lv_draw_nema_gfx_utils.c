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

uint32_t nemagfx_blending_mode(lv_blend_mode_t lv_blend_mode)
{
    uint32_t blending_mode;
    switch(lv_blend_mode) {
        case LV_BLEND_MODE_NORMAL:
            blending_mode = NEMA_BL_SRC_OVER;
            break;
        case LV_BLEND_MODE_ADDITIVE:
            blending_mode = NEMA_BL_ADD;
            break;
        default:
            blending_mode = NEMA_BL_SRC_OVER;
            break;
    }
    return blending_mode;
}

void nemagfx_grad_set(NEMA_VG_GRAD_HANDLE gradient, lv_grad_dsc_t lv_grad, lv_opa_t opa)
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

#endif /*LV_USE_NEMA_GFX*/