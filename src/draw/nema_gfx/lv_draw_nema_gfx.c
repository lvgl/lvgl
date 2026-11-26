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
 * @file lv_draw_nema_gfx.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_nema_gfx.h"
#include "../sw/lv_draw_sw_private.h"

#if LV_USE_NEMA_GFX

/*********************
 *      DEFINES
 *********************/

#define DRAW_UNIT_ID_NEMA_GFX 7

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void draw_begin(lv_layer_t * layer, const lv_area_t * clip_area);
static void draw_end(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area);

static bool nema_gfx_can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc);
static bool nema_gfx_can_draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_border_dsc_t * dsc);
static bool nema_gfx_can_draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_label_dsc_t * dsc);
static bool nema_gfx_can_draw_img(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc);
static bool nema_gfx_can_draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_line_dsc_t * dsc);
static bool nema_gfx_can_draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer,
                                       const lv_draw_triangle_dsc_t * dsc);

static int32_t nema_gfx_delete(lv_draw_unit_t * draw_unit);

static int32_t nema_gfx_wait_for_finish(lv_draw_unit_t * draw_unit);

static void nema_gfx_init_handlers(lv_draw_buf_handlers_t * handlers);

static void lv_draw_buf_nema_gfx_init_handlers(void);

static void draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);
static void draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_border_dsc_t * dsc, const lv_area_t * coords);
static void draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_label_dsc_t * dsc, const lv_area_t * coords);
static void draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);
static void draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);
#if LV_USE_NEMA_VG
static void draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                     const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords);
#endif
static void draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_line_dsc_t * dsc);
static void draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_triangle_dsc_t * dsc);
#if LV_USE_VECTOR_GRAPHIC && LV_USE_NEMA_VG
static void draw_vector(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_vector_dsc_t * dsc);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

static const lv_draw_unit_draw_cbs_t draw_cbs = {
    .draw_fill_cb = draw_fill,
    .can_draw_fill_cb = nema_gfx_can_draw_fill,
    .draw_border_cb = draw_border,
    .can_draw_border_cb = nema_gfx_can_draw_border,
    .draw_label_cb = draw_label,
    .can_draw_label_cb = nema_gfx_can_draw_label,
    .draw_image_cb = draw_image,
    .can_draw_image_cb = nema_gfx_can_draw_img,
    .draw_layer_cb = draw_layer,
#if LV_USE_NEMA_VG
    .draw_arc_cb = draw_arc,
#endif
    .draw_line_cb = draw_line,
    .can_draw_line_cb = nema_gfx_can_draw_line,
    .draw_triangle_cb = draw_triangle,
    .can_draw_triangle_cb = nema_gfx_can_draw_triangle,
#if LV_USE_VECTOR_GRAPHIC && LV_USE_NEMA_VG
    .draw_vector_cb = draw_vector,
#endif
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_nema_gfx_init(void)
{
    lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = lv_draw_create_unit(sizeof(lv_draw_nema_gfx_unit_t));
    /*Initialize NemaGFX*/
    nema_init();

    lv_draw_buf_nema_gfx_init_handlers();

    draw_nema_gfx_unit->base_unit.delete_cb = nema_gfx_delete;
    draw_nema_gfx_unit->base_unit.wait_for_finish_cb = nema_gfx_wait_for_finish;
    draw_nema_gfx_unit->base_unit.name = "NEMA_GFX";
    draw_nema_gfx_unit->base_unit.draw_cbs = &draw_cbs;
    draw_nema_gfx_unit->base_unit.fallback = lv_draw_sw_get_draw_unit();
    draw_nema_gfx_unit->base_unit.id = DRAW_UNIT_ID_NEMA_GFX;

#if LV_USE_NEMA_VG
    /*Initialize NemaVG */
    nema_vg_init(LV_NEMA_GFX_MAX_RESX, LV_NEMA_GFX_MAX_RESY);
    /* Allocate VG Buffers*/
    draw_nema_gfx_unit->paint = nema_vg_paint_create();
    draw_nema_gfx_unit->gradient = nema_vg_grad_create();
    draw_nema_gfx_unit->path = nema_vg_path_create();
    /*Initialize Freetype Support*/
    lv_draw_nema_gfx_label_init(&(draw_nema_gfx_unit->base_unit));
#endif
    /*Create GPU Command List*/
    draw_nema_gfx_unit->cl = nema_cl_create();
    /*Bind Command List*/
    nema_cl_bind_circular(&(draw_nema_gfx_unit->cl));

}

void lv_draw_nema_gfx_deinit(void)
{
    return;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void nema_gfx_init_handlers(lv_draw_buf_handlers_t * handlers)
{
#if LV_NEMA_USE_CACHE
    handlers->invalidate_cache_cb = lv_draw_nema_gfx_invalidate_cache;
    handlers->flush_cache_cb = lv_draw_nema_gfx_flush_cache;
#endif
}

static void lv_draw_buf_nema_gfx_init_handlers(void)
{
    nema_gfx_init_handlers(lv_draw_buf_get_handlers());
    nema_gfx_init_handlers(lv_draw_buf_get_font_handlers());
    nema_gfx_init_handlers(lv_draw_buf_get_image_handlers());
}

static int32_t nema_gfx_wait_for_finish(lv_draw_unit_t * draw_unit)
{
    lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = (lv_draw_nema_gfx_unit_t *)draw_unit;
    nema_cl_submit(&(draw_nema_gfx_unit->cl));
    nema_cl_wait(&(draw_nema_gfx_unit->cl));
    return 1;
}

static bool nema_gfx_can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(layer);
#if LV_USE_NEMA_VG
    LV_UNUSED(dsc);
    return true;
#else
    return dsc->grad.dir == (lv_grad_dir_t)LV_GRAD_DIR_NONE;
#endif
}

static bool nema_gfx_can_draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_border_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(layer);
    return (!(dsc->side != (lv_border_side_t)LV_BORDER_SIDE_FULL && dsc->radius > 0)) &&
           (dsc->radius > dsc->width);
}

static bool nema_gfx_can_draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_label_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(layer);
    const lv_font_fmt_txt_dsc_t * fdsc = (const lv_font_fmt_txt_dsc_t *)(dsc->font->dsc);
    return fdsc->bitmap_format != LV_FONT_FMT_TXT_COMPRESSED;
}

static bool nema_gfx_can_draw_img(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(layer);
    /*Guard for previous NemaGFX Version*/
#ifndef NEMA_BLOP_RECOLOR
    if(dsc->recolor_opa > LV_OPA_MIN)
        return false;
#endif
    const lv_image_dsc_t * img_dsc = dsc->src;
    if(!lv_nemagfx_is_cf_supported(img_dsc->header.cf))
        return false;

    return dsc->blend_mode != LV_BLEND_MODE_SUBTRACTIVE;
}

static bool nema_gfx_can_draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_line_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(layer);
    bool is_dashed = (dsc->dash_width && dsc->dash_gap);
    return !is_dashed && !(dsc->round_end || dsc->round_start);
}

static bool nema_gfx_can_draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer,
                                       const lv_draw_triangle_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(layer);
#if LV_USE_NEMA_VG
    LV_UNUSED(dsc);
    return true;
#else
    return dsc->grad.dir == (lv_grad_dir_t)LV_GRAD_DIR_NONE;
#endif
}

/**
 * Write the CPU cache of the clip area back before the GPU draws
 */
static void draw_begin(lv_layer_t * layer, const lv_area_t * clip_area)
{
    lv_area_t buf_area = *clip_area;
    lv_area_move(&buf_area, -layer->buf_area.x1, -layer->buf_area.y1);
    lv_draw_buf_flush_cache(layer->draw_buf, &buf_area);
}

/**
 * Wait until the GPU has stopped accessing the buffers, then invalidate the CPU cache of the clip area
 */
static void draw_end(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area)
{
    lv_draw_nema_gfx_unit_t * u = (lv_draw_nema_gfx_unit_t *)draw_unit;
    nema_cl_wait(&(u->cl));

    lv_area_t buf_area = *clip_area;
    lv_area_move(&buf_area, -layer->buf_area.x1, -layer->buf_area.y1);
    lv_draw_buf_invalidate_cache(layer->draw_buf, &buf_area);
}

static void draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin(layer, clip_area);
    lv_draw_nema_gfx_fill(draw_unit, layer, clip_area, dsc, coords);
    draw_end(draw_unit, layer, clip_area);
}

static void draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_border_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin(layer, clip_area);
    lv_draw_nema_gfx_border(draw_unit, layer, clip_area, dsc, coords);
    draw_end(draw_unit, layer, clip_area);
}

static void draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_label_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin(layer, clip_area);
    lv_draw_nema_gfx_label(draw_unit, layer, clip_area, dsc, coords);
    draw_end(draw_unit, layer, clip_area);
}

static void draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin(layer, clip_area);
    lv_draw_nema_gfx_img(draw_unit, layer, clip_area, dsc, coords);
    draw_end(draw_unit, layer, clip_area);
}

static void draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin(layer, clip_area);
    lv_draw_nema_gfx_layer(draw_unit, layer, clip_area, dsc, coords);
    draw_end(draw_unit, layer, clip_area);
}

#if LV_USE_NEMA_VG
static void draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                     const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin(layer, clip_area);
    lv_draw_nema_gfx_arc(draw_unit, layer, clip_area, dsc, coords);
    draw_end(draw_unit, layer, clip_area);
}
#endif

static void draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_line_dsc_t * dsc)
{
    draw_begin(layer, clip_area);
    lv_draw_line_iterate(layer, clip_area, dsc, lv_draw_nema_gfx_line, draw_unit);
    draw_end(draw_unit, layer, clip_area);
}

static void draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_triangle_dsc_t * dsc)
{
    draw_begin(layer, clip_area);
    lv_draw_nema_gfx_triangle(draw_unit, layer, clip_area, dsc);
    draw_end(draw_unit, layer, clip_area);
}

#if LV_USE_VECTOR_GRAPHIC && LV_USE_NEMA_VG
static void draw_vector(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_vector_dsc_t * dsc)
{
    draw_begin(layer, clip_area);
    lv_draw_nema_gfx_vector(draw_unit, layer, clip_area, dsc);
    draw_end(draw_unit, layer, clip_area);
}
#endif

static int32_t nema_gfx_delete(lv_draw_unit_t * draw_unit)
{
#if LV_USE_NEMA_VG
    /*Free VG Buffers*/
    lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = (lv_draw_nema_gfx_unit_t *) draw_unit;
    nema_vg_paint_destroy(draw_nema_gfx_unit->paint);
    nema_vg_path_destroy(draw_nema_gfx_unit->path);
    nema_vg_grad_destroy(draw_nema_gfx_unit->gradient);
#endif

#if LV_USE_NEMA_VG == 0
    LV_UNUSED(draw_unit);
#endif
    return 0;
}

#endif
