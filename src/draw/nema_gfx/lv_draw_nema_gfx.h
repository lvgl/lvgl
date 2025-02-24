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

#ifndef LV_DRAW_NEMA_GFX_H
#define LV_DRAW_NEMA_GFX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_NEMA_GFX

#include "lv_draw_nema_gfx_utils.h"

#include "../lv_draw_private.h"
#include "../lv_draw_buf_private.h"
#include "../lv_draw_image_private.h"
#include "../lv_image_decoder_private.h"
#include "../lv_draw_label_private.h"
#include "../lv_draw_mask_private.h"
#include "../lv_draw_rect_private.h"
#include "../lv_draw_triangle_private.h"
#include "../lv_draw_vector_private.h"

#include "../../misc/lv_area_private.h"

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_draw_unit_t base_unit;
    lv_draw_task_t * task_act;
#if LV_USE_OS
    lv_thread_sync_t sync;
    lv_thread_t thread;
    volatile bool inited;
    volatile bool exit_status;
#endif
    uint32_t idx;
    nema_cmdlist_t cl;
#if LV_USE_NEMA_VG
    NEMA_VG_PAINT_HANDLE paint;
    NEMA_VG_GRAD_HANDLE gradient;
    NEMA_VG_PATH_HANDLE path;
#endif
} lv_draw_nema_gfx_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_nema_gfx_init(void);

void lv_draw_nema_gfx_deinit(void);

void lv_draw_nema_gfx_fill(lv_draw_task_t * t,
                           const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_nema_gfx_triangle(lv_draw_task_t * t, const lv_draw_triangle_dsc_t * dsc);

void lv_draw_nema_gfx_img(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc,
                          const lv_area_t * coords);

void lv_draw_nema_gfx_label(lv_draw_task_t * t, const lv_draw_label_dsc_t * dsc,
                            const lv_area_t * coords);

void lv_draw_nema_gfx_label_init(lv_draw_unit_t * draw_unit);

void lv_draw_nema_gfx_layer(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                            const lv_area_t * coords);

void lv_draw_nema_gfx_line(lv_draw_task_t * t, const lv_draw_line_dsc_t * dsc);

void lv_draw_nema_gfx_border(lv_draw_task_t * t, const lv_draw_border_dsc_t * dsc,
                             const lv_area_t * coords);

void lv_draw_nema_gfx_arc(lv_draw_task_t * t, const lv_draw_arc_dsc_t * dsc,
                          const lv_area_t * coords);


/**********************
 *      MACROS
 **********************/

#endif  /*LV_USE_NEMA_GFX*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_NEMA_GFX_H*/
