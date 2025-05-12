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

#if LV_USE_DRAW_G2D
#include <stdlib.h>
#include "g2d.h"
#include "../../../misc/lv_area_private.h"
#include "lv_g2d_buf_map.h"
#include "lv_g2d_utils.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/* Blit simple w/ opa and alpha channel */
static void _g2d_fill(void * g2d_handle, struct g2d_buf * dst_buf, struct g2d_surface * dst_surf);
static void _g2d_fill_with_opa(void * g2d_handle, struct g2d_buf * dst_buf, struct g2d_surface * dst_surf,
                               struct g2d_buf * src_buf, struct g2d_surface * src_surf);

static void _g2d_set_src_surf(struct g2d_surface * src_surf, struct g2d_buf * buf, const lv_area_t * area,
                              int32_t stride, lv_color_t color, lv_opa_t opa);

static void _g2d_set_dst_surf(struct g2d_surface * dst_surf, struct g2d_buf * buf, const lv_area_t * area,
                              int32_t stride, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_g2d_fill(lv_draw_task_t * t)
{
    lv_draw_fill_dsc_t * dsc = t->draw_dsc;

    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    lv_draw_g2d_unit_t * u = (lv_draw_g2d_unit_t *)t->draw_unit;
    lv_layer_t * layer = t->target_layer;
    lv_draw_buf_t * draw_buf = layer->draw_buf;
    lv_area_t * coords = &t->area;

    lv_area_t rel_coords;
    lv_area_copy(&rel_coords, coords);
    lv_area_move(&rel_coords, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, &t->clip_area);
    lv_area_move(&rel_clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t blend_area;
    if(!lv_area_intersect(&blend_area, &rel_coords, &rel_clip_area))
        return; /*Fully clipped, nothing to do*/

    /* G2D takes stride in pixels. */
    int32_t stride = draw_buf->header.stride / (lv_color_format_get_bpp(draw_buf->header.cf) / 8);

    /* Destination buffer */
    struct g2d_buf * dst_buf = g2d_search_buf_map(draw_buf->data);

    bool has_opa = (dsc->opa < (lv_opa_t)LV_OPA_MAX);
    struct g2d_surface * dst_surf = lv_malloc(sizeof(struct g2d_surface));
    G2D_ASSERT_MSG(dst_surf, "Failed to alloc destination surface.");
    _g2d_set_dst_surf(dst_surf, dst_buf, &blend_area, stride, dsc->color);

    if(has_opa) {
        struct g2d_buf * tmp_buf = g2d_alloc(lv_area_get_width(&blend_area) * lv_area_get_height(&blend_area) * sizeof(
                                                 lv_color32_t), 1);
        G2D_ASSERT_MSG(tmp_buf, "Failed to alloc temporary buffer.");
        struct g2d_surface * src_surf = lv_malloc(sizeof(struct g2d_surface));
        G2D_ASSERT_MSG(src_surf, "Failed to alloc source surface.");
        _g2d_set_src_surf(src_surf, tmp_buf, &blend_area, stride, dsc->color, dsc->opa);
        _g2d_fill_with_opa(u->g2d_handle, dst_buf, dst_surf, tmp_buf, src_surf);
        g2d_free(tmp_buf);
        lv_free(src_surf);
    }
    else {
        _g2d_fill(u->g2d_handle, dst_buf, dst_surf);
    }

    lv_free(dst_surf);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _g2d_set_src_surf(struct g2d_surface * src_surf, struct g2d_buf * buf, const lv_area_t * area,
                              int32_t stride, lv_color_t color, lv_opa_t opa)
{
    int32_t width  = lv_area_get_width(area);
    int32_t height = lv_area_get_height(area);

    src_surf->format = G2D_RGBA8888;

    src_surf->left   = 0;
    src_surf->top    = 0;
    src_surf->right  = width;
    src_surf->bottom = height;
    src_surf->stride = stride;
    src_surf->width  = width;
    src_surf->height = height;

    src_surf->planes[0] = buf->buf_paddr;
    src_surf->rot = G2D_ROTATION_0;

    src_surf->clrcolor = g2d_rgba_to_u32(color);
    src_surf->global_alpha = opa;
    src_surf->blendfunc = G2D_ONE | G2D_PRE_MULTIPLIED_ALPHA;
}

static void _g2d_set_dst_surf(struct g2d_surface * dst_surf, struct g2d_buf * buf, const lv_area_t * area,
                              int32_t stride, lv_color_t color)
{
    int32_t width  = lv_area_get_width(area);
    int32_t height = lv_area_get_height(area);

    dst_surf->format = G2D_RGBA8888;

    dst_surf->left   = area->x1;
    dst_surf->top    = area->y1;
    dst_surf->right  = area->x1 + width;
    dst_surf->bottom = area->y1 + height;
    dst_surf->stride = stride;
    dst_surf->width  = width;
    dst_surf->height = height;

    dst_surf->planes[0] = buf->buf_paddr;
    dst_surf->rot = G2D_ROTATION_0;

    dst_surf->clrcolor = g2d_rgba_to_u32(color);
    dst_surf->global_alpha = 0xff;
    dst_surf->blendfunc = G2D_ONE_MINUS_SRC_ALPHA | G2D_PRE_MULTIPLIED_ALPHA;
}

static void _g2d_fill_with_opa(void * g2d_handle, struct g2d_buf * dst_buf, struct g2d_surface * dst_surf,
                               struct g2d_buf * src_buf, struct g2d_surface * src_surf)
{
    g2d_cache_op(src_buf, G2D_CACHE_FLUSH);

    g2d_clear(g2d_handle, src_surf);
    g2d_flush(g2d_handle);

    g2d_enable(g2d_handle, G2D_BLEND);
    g2d_enable(g2d_handle, G2D_GLOBAL_ALPHA);
    g2d_blit(g2d_handle, src_surf, dst_surf);
    g2d_finish(g2d_handle);
    g2d_disable(g2d_handle, G2D_GLOBAL_ALPHA);
    g2d_disable(g2d_handle, G2D_BLEND);
}

static void _g2d_fill(void * g2d_handle, struct g2d_buf * dst_buf, struct g2d_surface * dst_surf)
{
    g2d_clear(g2d_handle, dst_surf);
    g2d_flush(g2d_handle);

    g2d_finish(g2d_handle);
}

#endif /*LV_USE_DRAW_G2D*/