/**
 * @file lv_draw_g2d_img.c
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

#if LV_USE_G2D
#if LV_USE_DRAW_G2D
#include <math.h>
#include "g2d.h"
#include "../../../misc/lv_area_private.h"
#include "lv_g2d_utils.h"
#include "lv_g2d_buf_map.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static struct g2d_buf * _g2d_handle_src_buf(const lv_image_dsc_t * data);

static void _g2d_set_src_surf(struct g2d_surface * src_surf, struct g2d_buf * buf, const lv_area_t * area,
                              int32_t stride, lv_color_format_t cf, const lv_draw_image_dsc_t * dsc);

static void _g2d_set_tmp_surf(struct g2d_surface * tmp_surf, struct g2d_buf * buf, const lv_area_t * area,
                              lv_color_format_t cf);

static void _g2d_set_dst_surf(struct g2d_surface * dst_surf, struct g2d_buf * buf, const lv_area_t * area,
                              lv_draw_buf_t * draw_buf, const lv_draw_image_dsc_t * dsc);

/* Blit simple w/ opa and alpha channel */
static void _g2d_blit(void * handle, struct g2d_surface * dst_surf, struct g2d_surface * src_surf);

static void _g2d_blit_two_steps(void * handle, struct g2d_surface * dst_surf, struct g2d_surface * src_surf,
                                struct g2d_surface * tmp_surf);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_g2d_img(lv_draw_task_t * t)
{
    lv_draw_image_dsc_t * dsc = t->draw_dsc;

    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    lv_layer_t * layer = t->target_layer;
    lv_draw_buf_t * draw_buf = layer->draw_buf;
    const lv_image_dsc_t * img_dsc = dsc->src;
    lv_area_t * coords = &t->area;

    lv_area_t rel_coords;
    lv_area_copy(&rel_coords, coords);
    lv_area_move(&rel_coords, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t clip_area;
    lv_area_copy(&clip_area, &t->clip_area);
    lv_area_move(&clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t blend_area;
    bool has_transform = (dsc->rotation != 0 || dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE);
    if(has_transform)
        lv_area_copy(&blend_area, &rel_coords);
    else if(!lv_area_intersect(&blend_area, &rel_coords, &clip_area))
        return; /*Fully clipped, nothing to do*/

    lv_area_t src_area;
    src_area.x1 = blend_area.x1 - (coords->x1 - layer->buf_area.x1);
    src_area.y1 = blend_area.y1 - (coords->y1 - layer->buf_area.y1);
    src_area.x2 = src_area.x1 + lv_area_get_width(&blend_area);
    src_area.y2 = src_area.y1 + lv_area_get_height(&blend_area);
    lv_color_format_t src_cf = img_dsc->header.cf;

    /* G2D takes stride in pixels. */
    const uint8_t pixel_size = lv_color_format_get_size(src_cf);

    uint32_t src_stride = img_dsc->header.stride == 0 ?
                          lv_color_format_get_size(img_dsc->header.cf) * img_dsc->header.w :
                          img_dsc->header.stride;
    LV_ASSERT(pixel_size != 0);
    src_stride /= pixel_size;


    /* Source image */
    struct g2d_buf * src_buf = _g2d_handle_src_buf(img_dsc);

    /* Destination buffer */
    struct g2d_buf * dst_buf = g2d_search_buf_map(draw_buf->data);


    void * handle = g2d_get_handle();

    struct g2d_surface src_surf;
    struct g2d_surface dst_surf;

    _g2d_set_src_surf(&src_surf, src_buf, &src_area, src_stride, src_cf, dsc);
    _g2d_set_dst_surf(&dst_surf, dst_buf, &blend_area, draw_buf, dsc);

    bool has_rotation = (dsc->rotation != 0);

    if(has_rotation) {
        /** If the image has rotation, then blit in two steps:
         *   1. Source with rotation to temporary surface.
         *   2. Temporary with other transformations (if any) to destination (frame buffer).
         */
        struct g2d_buf * tmp_buf = g2d_alloc(lv_area_get_width(&src_area) * lv_area_get_height(
                                                 &src_area) *  lv_color_format_get_size(src_cf), 1);
        G2D_ASSERT_MSG(tmp_buf, "Failed to alloc temporary buffer.");
        struct g2d_surface tmp_surf;
        _g2d_set_tmp_surf(&tmp_surf, tmp_buf, &src_area, src_cf);
        _g2d_blit_two_steps(handle, &dst_surf, &src_surf, &tmp_surf);
        g2d_free(tmp_buf);
    }
    else {
        // If rotation is not involved, blit in one step.
        _g2d_blit(handle, &dst_surf, &src_surf);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static struct g2d_buf * _g2d_handle_src_buf(const lv_image_dsc_t * img_dsc)
{
    struct g2d_buf * src_buf = g2d_search_buf_map((void *)img_dsc->data);

    if(src_buf == NULL) {
        src_buf = g2d_alloc(img_dsc->data_size, 1);
        G2D_ASSERT_MSG(src_buf, "Failed to alloc source buffer.");
        memcpy((uint8_t *)src_buf->buf_vaddr, img_dsc->data, img_dsc->data_size);
        g2d_cache_op(src_buf, G2D_CACHE_FLUSH);
        g2d_insert_buf_map((void *)img_dsc->data, src_buf);
    }

    return src_buf;
}

static void _g2d_set_src_surf(struct g2d_surface * src_surf, struct g2d_buf * buf, const lv_area_t * area,
                              int32_t stride, lv_color_format_t cf, const lv_draw_image_dsc_t * dsc)
{
    src_surf->format = g2d_get_buf_format(cf);

    bool has_rotation = (dsc->rotation != 0);
    enum g2d_rotation g2d_angle = G2D_ROTATION_0;

    if(has_rotation) {
        switch(dsc->rotation) {
            case 0:
                g2d_angle = G2D_ROTATION_0;
                break;
            case 900:
                g2d_angle = G2D_ROTATION_270;
                break;
            case 1800:
                g2d_angle = G2D_ROTATION_180;
                break;
            case 2700:
                g2d_angle = G2D_ROTATION_90;
                break;
            default:
                g2d_angle = G2D_ROTATION_0;
        }
    }

    src_surf->left   = area->x1;
    src_surf->top    = area->y1;
    src_surf->right  = area->x2;
    src_surf->bottom = area->y2;
    src_surf->stride = stride;
    src_surf->width  = area->x2 - area->x1;
    src_surf->height = area->y2 - area->y1;

    src_surf->planes[0] = buf->buf_paddr;
    src_surf->rot = g2d_angle;

    src_surf->clrcolor = g2d_rgba_to_u32(lv_color_black());
    src_surf->global_alpha = dsc->opa;
    src_surf->blendfunc = G2D_ONE | G2D_PRE_MULTIPLIED_ALPHA;
}

static void _g2d_set_tmp_surf(struct g2d_surface * tmp_surf, struct g2d_buf * buf, const lv_area_t * area,
                              lv_color_format_t cf)
{
    tmp_surf->format = g2d_get_buf_format(cf);

    int32_t dest_w = area->x2 - area->x1;
    int32_t dest_h = area->y2 - area->y1;

    tmp_surf->left   = area->x1;
    tmp_surf->top    = area->y1;
    tmp_surf->right  = area->x1 + dest_w;
    tmp_surf->bottom = area->y1 + dest_h;
    tmp_surf->stride = dest_w;
    tmp_surf->width  = dest_w;
    tmp_surf->height = dest_h;

    tmp_surf->planes[0] = buf->buf_paddr;
    tmp_surf->rot = G2D_ROTATION_0;

    tmp_surf->clrcolor = g2d_rgba_to_u32(lv_color_black());
    tmp_surf->global_alpha = 0x00;
    tmp_surf->blendfunc = G2D_ONE_MINUS_SRC_ALPHA | G2D_PRE_MULTIPLIED_ALPHA;
}

static void _g2d_set_dst_surf(struct g2d_surface * dst_surf, struct g2d_buf * buf, const lv_area_t * area,
                              lv_draw_buf_t * draw_buf, const lv_draw_image_dsc_t * dsc)
{
    int32_t stride = draw_buf->header.stride / (lv_color_format_get_bpp(draw_buf->header.cf) / 8);
    lv_color_format_t cf = draw_buf->header.cf;
    uint32_t width = draw_buf->header.w;
    uint32_t height = draw_buf->header.h;

    int32_t blend_w  = lv_area_get_width(area);
    int32_t blend_h = lv_area_get_height(area);

    lv_point_t pivot = dsc->pivot;
    /*The offsets are now relative to the transformation result with pivot ULC*/
    int32_t piv_offset_x = 0;
    int32_t piv_offset_y = 0;
    int32_t trim_x = 0;
    int32_t trim_y = 0;
    int32_t dest_w;
    int32_t dest_h;

    bool has_rotation = (dsc->rotation != 0);

    if(has_rotation) {
        switch(dsc->rotation) {
            case 0:
                piv_offset_x = 0;
                piv_offset_y = 0;
                break;
            case 900:
                piv_offset_x = pivot.x + pivot.y - blend_h;
                piv_offset_y = pivot.y - pivot.x;
                break;
            case 1800:
                piv_offset_x = 2 * pivot.x - blend_w;
                piv_offset_y = 2 * pivot.y - blend_h;
                break;
            case 2700:
                piv_offset_x = pivot.x - pivot.y;
                piv_offset_y = pivot.x + pivot.y - blend_w;
                break;
            default:
                piv_offset_x = 0;
                piv_offset_y = 0;
        }
    }

    float fp_scale_x = (float)dsc->scale_x / LV_SCALE_NONE;
    float fp_scale_y = (float)dsc->scale_y / LV_SCALE_NONE;
    int32_t int_scale_x = (int32_t)fp_scale_x;
    int32_t int_scale_y = (int32_t)fp_scale_y;

    /*Any scale_factor in (k, k + 1] will result in a trim equal to k*/
    trim_x = (fp_scale_x == int_scale_x) ? int_scale_x - 1 : int_scale_x;
    trim_y = (fp_scale_y == int_scale_y) ? int_scale_y - 1 : int_scale_y;

    dest_w = blend_w * fp_scale_x + trim_x;
    dest_h = blend_h * fp_scale_y + trim_y;

    /*Final pivot offset = scale_factor * rotation_pivot_offset + scaling_pivot_offset*/
    piv_offset_x = floor(fp_scale_x * piv_offset_x) - floor((fp_scale_x - 1) * pivot.x);
    piv_offset_y = floor(fp_scale_y * piv_offset_y) - floor((fp_scale_y - 1) * pivot.y);

    dst_surf->format = g2d_get_buf_format(cf);

    dst_surf->left   = area->x1 + piv_offset_x;
    dst_surf->top    = area->y1 + piv_offset_y;
    dst_surf->right  = area->x1 + piv_offset_x + dest_w - trim_x;
    dst_surf->bottom = area->y1 + piv_offset_y + dest_h - trim_y;
    dst_surf->stride = stride;
    dst_surf->width  = width;
    dst_surf->height = height;

    dst_surf->planes[0] = buf->buf_paddr;
    dst_surf->rot = G2D_ROTATION_0;

    dst_surf->clrcolor = g2d_rgba_to_u32(lv_color_black());
    dst_surf->global_alpha = 0xff;
    dst_surf->blendfunc = G2D_ONE_MINUS_SRC_ALPHA | G2D_PRE_MULTIPLIED_ALPHA;
}

static void _g2d_blit(void * handle, struct g2d_surface * dst_surf, struct g2d_surface * src_surf)
{
    g2d_enable(handle, G2D_BLEND);
    g2d_enable(handle, G2D_GLOBAL_ALPHA);
    g2d_blit(handle, src_surf, dst_surf);
    g2d_finish(handle);
    g2d_disable(handle, G2D_GLOBAL_ALPHA);
    g2d_disable(handle, G2D_BLEND);
}

static void _g2d_blit_two_steps(void * handle, struct g2d_surface * dst_surf, struct g2d_surface * src_surf,
                                struct g2d_surface * tmp_surf)
{
    g2d_clear(handle, tmp_surf);

    g2d_enable(handle, G2D_BLEND);
    g2d_enable(handle, G2D_GLOBAL_ALPHA);
    g2d_blit(handle, src_surf, tmp_surf);
    g2d_finish(handle);
    g2d_disable(handle, G2D_GLOBAL_ALPHA);
    g2d_disable(handle, G2D_BLEND);

    /**After first blit, change blending and global alpha for temporary surface
     * since the surface now acts as source.
     */
    tmp_surf->blendfunc = G2D_ONE | G2D_PRE_MULTIPLIED_ALPHA;
    tmp_surf->global_alpha = 0xFF;

    g2d_enable(handle, G2D_BLEND);
    g2d_enable(handle, G2D_GLOBAL_ALPHA);
    g2d_blit(handle, tmp_surf, dst_surf);
    g2d_finish(handle);
    g2d_disable(handle, G2D_GLOBAL_ALPHA);
    g2d_disable(handle, G2D_BLEND);
}
#endif /*LV_USE_DRAW_G2D*/
#endif /*LV_USE_G2D*/
