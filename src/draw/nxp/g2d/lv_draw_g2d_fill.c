/**
 * @file lv_draw_g2d_fill.c
 *
 */

/**
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_g2d.h"
#include <stdlib.h>

#if LV_USE_DRAW_G2D
#include "g2d.h"
#include "../../../misc/lv_area_private.h"
#include "lv_g2d_utils.h"

/*********************
 *      DEFINES
 *********************/
extern void * g2d_handle;
extern struct g2d_buf * ctx_buf1;
extern struct g2d_buf * ctx_buf2;

extern struct g2d_buf * temp_buf;

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/* Blit simple w/ opa and alpha channel */
static void _g2d_fill(struct g2d_buf * dest_buf, struct g2d_surface dst_surf);
static void _g2d_fill_with_opa(struct g2d_buf * dest_buf, struct g2d_surface dst_surf, struct g2d_surface src_surf);

static struct g2d_surface _g2d_set_src_surf(struct g2d_buf * buf, const lv_area_t * area, int32_t stride,
                                            lv_color_t color, lv_opa_t opa);

static struct g2d_surface _g2d_set_dst_surf(struct g2d_buf * buf, const lv_area_t * area, int32_t stride,
                                            lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_g2d_fill(lv_draw_unit_t * draw_unit, const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    lv_layer_t * layer = draw_unit->target_layer;
    lv_draw_buf_t * draw_buf = layer->draw_buf;

    lv_area_t rel_coords;
    lv_area_copy(&rel_coords, coords);
    lv_area_move(&rel_coords, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, draw_unit->clip_area);
    lv_area_move(&rel_clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t blend_area;
    if(!lv_area_intersect(&blend_area, &rel_coords, &rel_clip_area))
        return; /*Fully clipped, nothing to do*/

    /* G2D takes stride in pixels. */
    int32_t stride = draw_buf->header.stride / (lv_color_format_get_bpp(draw_buf->header.cf) / 8);

    /* Workaround for destination buffer */
    struct g2d_buf * dest_buf = (draw_buf->data == ctx_buf1->buf_vaddr) ? ctx_buf1 : ctx_buf2;

    bool has_opa = (dsc->opa < (lv_opa_t)LV_OPA_MAX);

    if(has_opa) {
        struct g2d_surface src_surf = _g2d_set_src_surf(temp_buf, &blend_area, stride, dsc->color, dsc->opa);
        struct g2d_surface dst_surf = _g2d_set_dst_surf(dest_buf, &blend_area, stride, dsc->color);
        _g2d_fill_with_opa(dest_buf, dst_surf, src_surf);
    }
    else {
        struct g2d_surface dst_surf = _g2d_set_dst_surf(dest_buf, &blend_area, stride, dsc->color);
        _g2d_fill(dest_buf, dst_surf);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static struct g2d_surface _g2d_set_src_surf(struct g2d_buf * buf, const lv_area_t * area, int32_t stride,
                                            lv_color_t color, lv_opa_t opa)
{
    int32_t width  = lv_area_get_width(area);
    int32_t height = lv_area_get_height(area);

    struct g2d_surface src_surf;

    src_surf.format = G2D_RGBA8888;

    src_surf.left   = 0;
    src_surf.top    = 0;
    src_surf.right  = width;
    src_surf.bottom = height;
    src_surf.stride = stride;
    src_surf.width  = width;
    src_surf.height = height;

    src_surf.planes[0] = buf->buf_paddr;
    src_surf.rot = G2D_ROTATION_0;

    src_surf.clrcolor = g2d_rgba_to_u32(color);
    src_surf.global_alpha = opa;
    src_surf.blendfunc = G2D_ONE | G2D_PRE_MULTIPLIED_ALPHA;

    return src_surf;
}

static struct g2d_surface _g2d_set_dst_surf(struct g2d_buf * buf, const lv_area_t * area, int32_t stride,
                                            lv_color_t color)
{
    int32_t width  = lv_area_get_width(area);
    int32_t height = lv_area_get_height(area);

    struct g2d_surface dst_surf;

    dst_surf.format = G2D_RGBA8888;

    dst_surf.left   = area->x1;
    dst_surf.top    = area->y1;
    dst_surf.right  = area->x1 + width;
    dst_surf.bottom = area->y1 + height;
    dst_surf.stride = stride;
    dst_surf.width  = width;
    dst_surf.height = height;

    dst_surf.planes[0] = buf->buf_paddr;
    dst_surf.rot = G2D_ROTATION_0;

    dst_surf.clrcolor = g2d_rgba_to_u32(color);
    dst_surf.global_alpha = 0xff;
    dst_surf.blendfunc = G2D_ONE_MINUS_SRC_ALPHA | G2D_PRE_MULTIPLIED_ALPHA;

    return dst_surf;
}

static void _g2d_fill_with_opa(struct g2d_buf * dest_buf, struct g2d_surface dst_surf, struct g2d_surface src_surf)
{
    g2d_clear(g2d_handle, &src_surf);
    g2d_flush(g2d_handle);

    g2d_cache_op(dest_buf, G2D_CACHE_FLUSH);

    g2d_enable(g2d_handle, G2D_BLEND);
    g2d_enable(g2d_handle, G2D_GLOBAL_ALPHA);
    g2d_blit(g2d_handle, &src_surf, &dst_surf);
    g2d_finish(g2d_handle);
    g2d_disable(g2d_handle, G2D_GLOBAL_ALPHA);
    g2d_disable(g2d_handle, G2D_BLEND);
}

static void _g2d_fill(struct g2d_buf * dest_buf, struct g2d_surface dst_surf)
{
    g2d_clear(g2d_handle, &dst_surf);
    g2d_flush(g2d_handle);

    g2d_cache_op(dest_buf, G2D_CACHE_FLUSH);

    g2d_finish(g2d_handle);
}
#endif /*LV_USE_DRAW_G2D*/