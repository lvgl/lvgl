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
 * @file lv_nema_gfx_path.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_refr.h"

#if LV_USE_NEMA_GFX
#if LV_USE_NEMA_VG

#include "lv_nema_gfx_path.h"

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static int data_point = 0;
static int seg_point  = 0;

lv_nema_gfx_path_t * lv_nema_gfx_path_create(void)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_nema_gfx_path_t * nema_gfx_path = lv_malloc_zeroed(sizeof(lv_nema_gfx_path_t));
    LV_ASSERT_MALLOC(nema_gfx_path);
    nema_gfx_path->seg = NULL;
    nema_gfx_path->data = NULL;
    nema_gfx_path->seg_size = 0;
    nema_gfx_path->data_size = 0;
    data_point = 0;
    seg_point  = 0;
    LV_PROFILER_DRAW_END;
    return nema_gfx_path;
}

void lv_nema_gfx_path_alloc(lv_nema_gfx_path_t * nema_gfx_path)
{
    LV_PROFILER_DRAW_BEGIN;
    nema_gfx_path->path = nema_vg_path_create();
    nema_gfx_path->paint = nema_vg_paint_create();
    nema_gfx_path->data = (float *) lv_malloc(nema_gfx_path->data_size * sizeof(float));
    LV_ASSERT_MALLOC(nema_gfx_path->data);
    nema_gfx_path->seg = (uint8_t *) lv_malloc(nema_gfx_path->seg_size * sizeof(uint8_t));
    LV_ASSERT_MALLOC(nema_gfx_path->seg);
    LV_PROFILER_DRAW_END;
}

void lv_nema_gfx_path_destroy(lv_nema_gfx_path_t * nema_gfx_path)
{
    LV_PROFILER_DRAW_BEGIN;
    LV_ASSERT_NULL(nema_gfx_path);

    if(nema_gfx_path->path != NULL) {
        nema_vg_path_destroy(nema_gfx_path->path);
        nema_gfx_path->path = NULL;
    }

    if(nema_gfx_path->paint != NULL) {
        nema_vg_paint_destroy(nema_gfx_path->paint);
        nema_gfx_path->paint = NULL;
    }

    if(nema_gfx_path->data != NULL) {
        lv_free(nema_gfx_path->data);
        nema_gfx_path->data = NULL;
    }
    if(nema_gfx_path->seg != NULL) {
        lv_free(nema_gfx_path->seg);
        nema_gfx_path->seg = NULL;
    }
    lv_free(nema_gfx_path);
    LV_PROFILER_DRAW_END;
}

void lv_nema_gfx_path_move_to(lv_nema_gfx_path_t * path, float x, float y)
{
    LV_ASSERT_NULL(path);
    LV_ASSERT(path->data_size > data_point + 1);
    LV_ASSERT(path->seg_size > seg_point);
    path->seg[seg_point++] = NEMA_VG_PRIM_MOVE;
    path->data[data_point++] = x;
    path->data[data_point++] = y;
}

void lv_nema_gfx_path_line_to(lv_nema_gfx_path_t * path, float x, float y)
{
    LV_ASSERT_NULL(path);
    LV_ASSERT(path->data_size > data_point + 1);
    LV_ASSERT(path->seg_size > seg_point);
    path->seg[seg_point++] = NEMA_VG_PRIM_LINE;
    path->data[data_point++] = x;
    path->data[data_point++] = y;

}

void lv_nema_gfx_path_quad_to(lv_nema_gfx_path_t * path, float cx, float cy, float x, float y)
{
    LV_ASSERT_NULL(path);
    LV_ASSERT(path->data_size > data_point + 3);
    LV_ASSERT(path->seg_size > seg_point);
    path->seg[seg_point++] = NEMA_VG_PRIM_BEZIER_QUAD;
    path->data[data_point++] = cx;
    path->data[data_point++] = cy;
    path->data[data_point++] = x;
    path->data[data_point++] = y;
}

void lv_nema_gfx_path_cubic_to(lv_nema_gfx_path_t * path, float cx1, float cy1, float cx2, float cy2, float x, float y)
{
    LV_ASSERT_NULL(path);
    LV_ASSERT(path->data_size > data_point + 5);
    LV_ASSERT(path->seg_size > seg_point);
    path->seg[seg_point++] = NEMA_VG_PRIM_BEZIER_CUBIC;
    path->data[data_point++] = cx1;
    path->data[data_point++] = cy1;
    path->data[data_point++] = cx2;
    path->data[data_point++] = cy2;
    path->data[data_point++] = x;
    path->data[data_point++] = y;
}

void lv_nema_gfx_path_end(lv_nema_gfx_path_t * path)
{
    /* Do Path end jobs....whatever*/
    seg_point = 0;
    data_point = 0;

}

#endif  /*LV_USE_NEMA_VG*/
#endif  /*LV_USE_NEMA_GFX*/
