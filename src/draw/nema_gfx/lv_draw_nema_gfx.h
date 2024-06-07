/**
 * @file lv_draw_nema_gfx.h
 *
 */

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

#ifndef LV_DRAW_NEMA_GFX_H
#define LV_DRAW_NEMA_GFX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_color.h"
#include "../../hal/lv_hal_disp.h"
#include "../sw/lv_draw_sw.h"

#if LV_USE_NEMA_GFX

#include "nema_core.h"
#include "nema_utils.h"
#include "nema_error.h"
#include "nema_provisional.h"


/*********************
 *      DEFINES
 *********************/

#ifndef NEMA_VIRT2PHYS
#define NEMA_VIRT2PHYS
#endif

/*Color depth: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888)*/
#if LV_COLOR_DEPTH == 8
#define LV_NEMA_GFX_COLOR_FORMAT NEMA_RGB332
#define LV_NEMA_GFX_FORMAT_MULTIPLIER 1
#elif LV_COLOR_DEPTH == 16
#define LV_NEMA_GFX_COLOR_FORMAT NEMA_RGB565
#define LV_NEMA_GFX_FORMAT_MULTIPLIER 2
#elif LV_COLOR_DEPTH == 32
#define LV_NEMA_GFX_COLOR_FORMAT NEMA_BGRA8888
#define LV_NEMA_GFX_FORMAT_MULTIPLIER 4
#else
/*Can't use GPU with other formats*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_sw_ctx_t base_sw_ctx;
    /*Add other needed fields here*/
    nema_cmdlist_t cl;
} lv_draw_nema_gfx_ctx_t;

struct _lv_disp_drv_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_nema_gfx_init(void);

void lv_draw_nema_gfx_ctx_init(struct _lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx);

void lv_draw_nema_gfx_ctx_deinit(struct _lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx);

void lv_draw_nema_gfx_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc);

void lv_draw_nema_gfx_bg(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_nema_gfx_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_nema_gfx_img_decoded(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                                  const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t cf);

void lv_draw_nema_gfx_letter(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,  const lv_point_t * pos_p,
                             uint32_t letter);

void lv_draw_nema_gfx_arc(lv_draw_ctx_t * draw_ctx, const lv_draw_arc_dsc_t * dsc, const lv_point_t * center,
                          uint16_t radius,
                          uint16_t start_angle, uint16_t end_angle);

void lv_draw_nema_gfx_line(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_line_dsc_t * dsc,
                           const lv_point_t * point1, const lv_point_t * point2);

void lv_draw_nema_gfx_polygon(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * draw_dsc,
                              const lv_point_t * points, uint16_t point_cnt);

lv_res_t  lv_draw_nema_gfx_img(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                               const lv_area_t * coords,
                               const void * src);

/**********************
 *      MACROS
 **********************/

#endif  /*LV_USE_NEMA_GFX*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_NEMA_GFX_H*/
