/**
 * @file lv_draw_eve5_image_render.c
 *
 * EVE5 (BT820) Image and Layer RGB Drawing
 *
 * Contains the main image/layer draw function (lv_draw_eve5_hal_draw_image).
 * Handles:
 * - Source resolution (layer vs regular image, SD card, HW decode, SW decode)
 * - Clip radius / bitmap mask alpha-channel masking
 * - Recolor (full and partial, premultiplied and straight)
 * - Colorkey stencil masking
 * - Affine transforms (rotation, scale, skew)
 * - Tiling, blend modes, premultiplied compositing
 *
 * Shared helpers (compute_image_skew, apply_image_skew, build_colorkey_stencil)
 * are in lv_draw_eve5_image.c.
 * Alpha correction pass is in lv_draw_eve5_image_alpha.c.
 * Image loading and upload are in lv_draw_eve5_image_load.c.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

/**********************
 * IMAGE DRAWING
 **********************/

/* Vertex color of an image: white, or with the recolor mixed in. LVGL draws
 * alpha-only images (A1..A8) in the recolor color; their texture is white, so
 * the vertex color is the recolor. */
static lv_color_t image_tint(const lv_draw_image_dsc_t * dsc, bool alpha_only)
{
    if(alpha_only) return dsc->recolor;
    if(dsc->recolor_opa > LV_OPA_MIN) return lv_color_mix(dsc->recolor, lv_color_white(), dsc->recolor_opa);
    return lv_color_white();
}

/* Bitmap layout of an image source. An alpha layer, such as the A8 layer of
 * a drop shadow, is rendered in a color format: it samples as white with its
 * alpha, as an L8 image does. An alpha-only source drawn premultiplied
 * (alpha_premultiplied) samples its alpha in every channel. */
static void set_image_layout(EVE_HalContext * phost, uint16_t eve_format, int32_t stride, int32_t height,
                             bool sample_as_luminance, bool alpha_layer, bool alpha_premultiplied)
{
#if (EVE_SUPPORT_CHIPID >= EVE_BT815) || defined(EVE_MULTI_GRAPHICS_TARGET)
    if(alpha_premultiplied) {
        EVE_CoDl_bitmapLayout(phost, GLFORMAT, stride, height);
        EVE_CoDl_bitmapExtFormat(phost, eve_format);
        EVE_CoDl_bitmapSwizzle(phost, ALPHA, ALPHA, ALPHA, ALPHA);
        return;
    }
#else
    LV_UNUSED(alpha_premultiplied);
#endif
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
    if(alpha_layer) {
        EVE_CoDl_bitmapLayout(phost, GLFORMAT, stride, height);
        EVE_CoDl_bitmapExtFormat(phost, eve_format);
        EVE_CoDl_bitmapSwizzle(phost, ONE, ONE, ONE, ALPHA);
        return;
    }
#else
    LV_UNUSED(alpha_layer);
#endif
    eve5_set_image_bitmap_layout(phost, eve_format, stride, height, sample_as_luminance);
}

/* Set up the bitmap transform of an image draw: rotation, scale and skew
 * about the pivot, or none. Gives the vertex to draw the bitmap at, and sets
 * the bitmap size of a transformed image. Returns false when the transform
 * is degenerate. */
static bool setup_image_transform(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t,
                                  int32_t src_w, int32_t src_h, int32_t x, int32_t y,
                                  uint8_t bmp_filter, int32_t * draw_vx, int32_t * draw_vy)
{
    EVE_HalContext * phost = u->hal;
    const lv_draw_image_dsc_t * dsc = t->draw_dsc;
    const lv_layer_t * layer = t->target_layer;
    bool has_skew = dsc->skew_x != 0 || dsc->skew_y != 0;
    bool has_transform = has_skew || dsc->rotation != 0
                         || dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE;

    if(!has_transform) {
        *draw_vx = dsc->tile ? (dsc->image_area.x1 - layer->buf_area.x1) : x;
        *draw_vy = dsc->tile ? (dsc->image_area.y1 - layer->buf_area.y1) : y;
        EVE_CoDl_bitmapTransform_identity(phost);
        return true;
    }

    const lv_area_t * transform_area = eve5_transform_area(t);
    *draw_vx = transform_area->x1 - layer->buf_area.x1;
    *draw_vy = transform_area->y1 - layer->buf_area.y1;

    if(has_skew) {
        image_skew_t skew;
        if(!compute_image_skew(&skew, dsc->rotation, dsc->scale_x, dsc->scale_y,
                               dsc->skew_x, dsc->skew_y, dsc->pivot.x, dsc->pivot.y,
                               src_w, src_h, x, y, *draw_vx, *draw_vy)) {
            return false;
        }
        apply_image_skew(phost, &skew, bmp_filter,
                         dsc->tile ? lv_area_get_width(&dsc->image_area) : 0,
                         dsc->tile ? lv_area_get_height(&dsc->image_area) : 0);
        return true;
    }

    EVE_CoCmd_loadIdentity(phost);
    EVE_CoCmd_translate(phost, F16(x - *draw_vx + dsc->pivot.x), F16(y - *draw_vy + dsc->pivot.y));
    /* Rotate after scaling: LVGL scales along the image axes */
    if(dsc->rotation != 0) {
        EVE_CoCmd_rotate(phost, DEGREES(dsc->rotation));
    }
    if(dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE) {
        EVE_CoCmd_scale(phost, F16_SCALE_DIV_256(dsc->scale_x), F16_SCALE_DIV_256(dsc->scale_y));
    }
    EVE_CoCmd_translate(phost, -F16(dsc->pivot.x), -F16(dsc->pivot.y));
    EVE_CoCmd_setMatrix(phost);
    EVE_CoCmd_loadIdentity(phost);
    EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER,
                        LV_MIN(lv_area_get_width(transform_area), 2048),
                        LV_MIN(lv_area_get_height(transform_area), 2048));
    return true;
}

/* Draw the image bitmap at its vertex, or stamped over the tile extent when
 * stamp_w is set */
static void draw_image_bitmap(EVE_HalContext * phost, int32_t vx, int32_t vy,
                              int32_t stamp_w, int32_t stamp_h, int32_t src_w, int32_t src_h)
{
    EVE_CoDl_begin(phost, BITMAPS);
    if(stamp_w > 0) {
        eve5_image_tile_emit_stamps(phost, vx, vy, stamp_w, stamp_h, src_w, src_h);
    }
    else {
        EVE_CoDl_vertex2f_0(phost, vx, vy);
    }
    EVE_CoDl_end(phost);
}

/**
 * Draw image or layer to the target layer.
 *
 * @param alpha_to_rgb  When true, renders alpha contribution as grayscale luminance
 *                      for later copying into a layer's alpha channel. Caller must
 *                      use default blend mode with colorMask(1,1,1,1). The A channel
 *                      is scratch space in this mode.
 */
void lv_draw_eve5_hal_draw_image(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, bool alpha_to_rgb)
{
    EVE_HalContext *phost = u->hal;
    lv_layer_t * layer = t->target_layer;
    lv_draw_image_dsc_t * dsc = t->draw_dsc;

    uint32_t ram_g_addr;
    uint16_t eve_format;
    int32_t eve_stride;
    int32_t src_w, src_h;
    int32_t layout_h;
    uint32_t palette_addr = GA_INVALID;
    bool sample_as_luminance = false;
    bool alpha_layer = false;
    bool src_premultiplied = false;
    EVE_GpuHandle child_handle = GA_HANDLE_INVALID;

    /* Resolve bitmap source */
    if(t->type == LV_DRAW_TASK_TYPE_LAYER) {
        lv_layer_t * child_layer = (lv_layer_t *)dsc->src;
        src_w = lv_area_get_width(&child_layer->buf_area);
        src_h = lv_area_get_height(&child_layer->buf_area);

        lv_eve5_vram_res_t * child_vr = eve5_get_vram_res(child_layer);
        if(child_vr == NULL) {
            LV_LOG_WARN("EVE5: Child layer %p has no vram_res", (void *)child_layer);
            return;
        }
        /* Skip empty layers (cleared but no tasks rendered) — content is
         * transparent black so compositing would be a visual no-op. */
        if(!child_vr->has_content) {
            return;
        }
        child_handle = child_vr->gpu_handle;
        ram_g_addr = EVE_GpuAlloc_Get(u->allocator, child_handle);
        if(ram_g_addr == GA_INVALID) {
            LV_LOG_WARN("EVE5: Child layer %p texture invalid", (void *)child_layer);
            return;
        }
        ram_g_addr += child_vr->source_offset;
        eve_format = child_vr->eve_format;
        eve_stride = (int32_t)child_vr->stride;
        layout_h = src_h;
        sample_as_luminance = child_vr->sample_as_luminance;
        src_premultiplied = child_vr->is_premultiplied;
        if(child_vr->palette_offset != GA_INVALID) {
            uint32_t base = EVE_GpuAlloc_Get(u->allocator, child_handle);
            palette_addr = base + child_vr->palette_offset;
        }
        lv_color_format_t child_cf = child_layer->color_format;
        alpha_layer = (child_cf == LV_COLOR_FORMAT_A1 || child_cf == LV_COLOR_FORMAT_A2
                       || child_cf == LV_COLOR_FORMAT_A4 || child_cf == LV_COLOR_FORMAT_A8)
                      && eve_format != L8;
    }
    else {
        lv_eve5_vram_res_t * img = lv_draw_eve5_resolve_to_gpu(u, dsc->src);
        if(!img) return;
        eve5_vram_res_resolve(u->allocator, img, &ram_g_addr, &palette_addr);
        if(ram_g_addr == GA_INVALID) return;
        eve_format = img->eve_format;
        eve_stride = (int32_t)img->stride;
        src_w = img->width;
        src_h = img->height;
        layout_h = src_h;
        sample_as_luminance = img->sample_as_luminance;
        src_premultiplied = img->is_premultiplied;
    }

    /* L formats sample as (255, 255, 255, L): alpha, unless the source is a
     * luminance image */
    bool alpha_only = alpha_layer || (!sample_as_luminance
                                      && (eve_format == L1 || eve_format == L2 || eve_format == L4 || eve_format == L8));

    /* Load bitmap mask if set */
    bool has_bitmap_mask = false;
    uint32_t mask_ram_g_addr = GA_INVALID;
    int32_t mask_w = 0, mask_h = 0;
    int32_t mask_eve_stride = 0;
    uint16_t mask_eve_format = L8;
    uint32_t mask_palette_addr = GA_INVALID;
    if(dsc->bitmap_mask_src != NULL) {
        lv_eve5_vram_res_t * mask_img = lv_draw_eve5_resolve_to_gpu(u, dsc->bitmap_mask_src);
        if(mask_img) {
            eve5_vram_res_resolve(u->allocator, mask_img, &mask_ram_g_addr, &mask_palette_addr);
            if(mask_ram_g_addr != GA_INVALID) {
                mask_eve_format = mask_img->eve_format;
                mask_eve_stride = (int32_t)mask_img->stride;
                mask_w = mask_img->width;
                mask_h = mask_img->height;
                has_bitmap_mask = true;
            }
        }
        if(!has_bitmap_mask) {
            LV_LOG_WARN("EVE5: Failed to load bitmap mask");
        }
    }

    int32_t x = t->area.x1 - layer->buf_area.x1;
    int32_t y = t->area.y1 - layer->buf_area.y1;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    bool has_any_transform = (dsc->rotation != 0 || dsc->scale_x != LV_SCALE_NONE
                              || dsc->scale_y != LV_SCALE_NONE
                              || dsc->skew_x != 0 || dsc->skew_y != 0);
    /* Masked: drawn through a mask in the alpha channel, which holds the
     * image's alpha (clip radius, bitmap mask, colorkey with recolor, and the
     * L8 alpha pass) */
    bool masked = eve5_image_clip_radius(dsc) > 0 || has_bitmap_mask || alpha_to_rgb
                  || (dsc->colorkey != NULL && dsc->recolor_opa > LV_OPA_MIN);

    /* An alpha-only image filtered by a transform is drawn premultiplied, its
     * alpha sampled in every channel: it samples as white with its alpha, and
     * outside the bitmap BORDER gives (0, 0, 0, 0), which pulls the white
     * toward black at the edges, darkened again by the SRC_ALPHA blend.
     * Premultiplied, BORDER's zero is right. Through a mask, the image's alpha
     * is in the mask and its color is drawn flat instead. */
    bool alpha_premultiplied = false;
#if (EVE_SUPPORT_CHIPID >= EVE_BT815) || defined(EVE_MULTI_GRAPHICS_TARGET)
    alpha_premultiplied = alpha_only && has_any_transform && dsc->antialias && !masked && dsc->colorkey == NULL
                          && EVE_CHIPID >= EVE_BT815;
#endif

    /* Premultiplied content (RGB already scaled by alpha) uses blend(ONE,
     * ONE_MINUS_SRC_ALPHA) to avoid double-applying alpha. Vertex color is
     * scaled by opa for attenuation. The flag comes from the resolved VRAM
     * resource: for a file or decoded source that is the decoded image, not
     * the source descriptor. Only the alpha of an alpha layer is drawn. */
    bool is_premultiplied = (src_premultiplied && !alpha_layer) || alpha_premultiplied;

    lv_color_t tint = image_tint(dsc, alpha_only);
    if(is_premultiplied) {
        uint8_t opa = dsc->opa;
        EVE_CoDl_colorRgb(u->hal,
                          (uint8_t)(tint.red * opa / 255),
                          (uint8_t)(tint.green * opa / 255),
                          (uint8_t)(tint.blue * opa / 255));
        EVE_CoDl_colorA(u->hal, opa);
    }
    else {
        EVE_CoDl_colorA(u->hal, dsc->opa);
        EVE_CoDl_colorRgb(u->hal, tint.red, tint.green, tint.blue);
    }

    /* Set up bitmap */
    EVE_CoDl_bitmapHandle(phost, EVE_CO_SCRATCH_HANDLE);
    EVE_CoDl_bitmapSource(u->hal, ram_g_addr);
    set_palette_if_needed(u->hal, eve_format, palette_addr);
    set_image_layout(u->hal, eve_format, eve_stride, layout_h, sample_as_luminance, alpha_layer, alpha_premultiplied);
    uint8_t bmp_filter = dsc->antialias ? BILINEAR : NEAREST;
    bool tile_stamps = dsc->tile && eve5_image_tile_needs_stamps(src_w, src_h);
    /* dsc->image_area is the first-tile origin (src-sized); t->area is the full
     * fill region. Extent runs from image_area corner to t->area's far edge. */
    int32_t tile_extent_w = dsc->tile ? (t->area.x2 - dsc->image_area.x1 + 1) : 0;
    int32_t tile_extent_h = dsc->tile ? (t->area.y2 - dsc->image_area.y1 + 1) : 0;
    if(dsc->tile && !tile_stamps) {
        EVE_CoDl_bitmapSize(u->hal, bmp_filter, REPEAT, REPEAT,
                            LV_MIN(tile_extent_w, 2048), LV_MIN(tile_extent_h, 2048));
    }
    else {
        EVE_CoDl_bitmapSize(u->hal, bmp_filter, BORDER, BORDER, src_w, src_h);
    }
    /* Tile extent to stamp the bitmap over, 0 to draw it once */
    int32_t stamp_w = (tile_stamps && !has_any_transform) ? tile_extent_w : 0;
    int32_t stamp_h = (tile_stamps && !has_any_transform) ? tile_extent_h : 0;

    /* Alpha-channel masking path: clip_radius, bitmap_mask_src, alpha_to_rgb,
     * or colorkey+recolor. Uses multi-phase approach:
     * Phase 1a: clear bbox alpha
     * Phase 1b: write rounded rect mask (if clip_radius)
     * Phase 1b2: apply bitmap mask (if bitmap_mask_src)
     * Phase 1c: multiply mask by image alpha
     * Phase 2: draw image through mask */
    if(masked) {
        int32_t mask_x1 = dsc->image_area.x1 - layer->buf_area.x1;
        int32_t mask_y1 = dsc->image_area.y1 - layer->buf_area.y1;
        int32_t mask_x2 = dsc->image_area.x2 - layer->buf_area.x1;
        int32_t mask_y2 = dsc->image_area.y2 - layer->buf_area.y1;

        /* Area the image draws over, which the mask covers: where the
         * transform puts it, or the tile extent */
        const lv_area_t * box_area = has_any_transform ? eve5_transform_area(t)
                                     : dsc->tile ? &t->area : &dsc->image_area;
        int32_t box_x1 = box_area->x1 - layer->buf_area.x1;
        int32_t box_y1 = box_area->y1 - layer->buf_area.y1;
        int32_t box_x2 = box_area->x2 - layer->buf_area.x1;
        int32_t box_y2 = box_area->y2 - layer->buf_area.y1;

        EVE_CoDl_bitmapTransform_identity(phost);
        EVE_CoDl_vertexFormat(phost, 0);
        EVE_CoDl_saveContext(phost);

        /* Phase 1a: Clear bbox alpha to 0 */
        EVE_CoDl_colorArgb_ex(phost, 0x00000000);
        EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
        EVE_CoDl_blendFunc(phost, ONE, ZERO);
        lv_draw_eve5_draw_rect(u, box_x1, box_y1, box_x2, box_y2, 0,
                               &t->clip_area, &layer->buf_area);

        /* Phase 1b: Write rounded rect mask (alpha=255 inside) */
        if(eve5_image_clip_radius(dsc) > 0) {
            EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
            lv_draw_eve5_draw_rect(u, mask_x1, mask_y1, mask_x2, mask_y2,
                                   eve5_image_clip_radius(dsc), &t->clip_area, &layer->buf_area);
        }
        else if((alpha_to_rgb || dsc->colorkey != NULL) && !has_bitmap_mask) {
            /* No clip or mask: fill bbox A=255 so phase 1c multiply works */
            EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
            lv_draw_eve5_draw_rect(u, box_x1, box_y1, box_x2, box_y2, 0,
                                   &t->clip_area, &layer->buf_area);
        }

        /* Phase 1b2: Apply bitmap mask */
        if(has_bitmap_mask) {
            if(eve5_image_clip_radius(dsc) > 0) {
                EVE_CoDl_blendFunc(phost, ZERO, SRC_ALPHA);
            }

            int32_t img_w = lv_area_get_width(&dsc->image_area);
            int32_t img_h = lv_area_get_height(&dsc->image_area);
            int32_t mask_draw_x = mask_x1 + (img_w - mask_w) / 2;
            int32_t mask_draw_y = mask_y1 + (img_h - mask_h) / 2;

            EVE_CoDl_saveContext(phost);
            EVE_CoDl_bitmapHandle(phost, EVE_CO_SCRATCH_HANDLE);
            EVE_CoDl_bitmapSource(phost, mask_ram_g_addr);
            set_palette_if_needed(phost, mask_eve_format, mask_palette_addr);
            /* ARGB8/PALETTEDARGB8: extract RED as alpha (grayscale PNGs decode with R=G=B=gray) */
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
            if(mask_eve_format == ARGB8 || mask_eve_format == PALETTEDARGB8) {
                EVE_CoDl_bitmapLayout(phost, GLFORMAT, mask_eve_stride, mask_h);
                EVE_CoDl_bitmapExtFormat(phost, mask_eve_format);
                EVE_CoDl_bitmapSwizzle(phost, ZERO, ZERO, ZERO, RED);
            }
            else
#endif
            {
                EVE_CoDl_bitmapLayout(phost, (uint8_t)mask_eve_format, mask_eve_stride, mask_h);
            }
            EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, mask_w, mask_h);
            EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
            EVE_CoDl_begin(phost, BITMAPS);
            EVE_CoDl_vertex2f_0(phost, mask_draw_x, mask_draw_y);
            EVE_CoDl_end(phost);
            EVE_CoDl_restoreContext(phost);

            /* Re-setup main image bitmap */
            EVE_CoDl_bitmapHandle(phost, EVE_CO_SCRATCH_HANDLE);
            EVE_CoDl_bitmapSource(phost, ram_g_addr);
            set_palette_if_needed(phost, eve_format, palette_addr);
            set_image_layout(phost, eve_format, eve_stride, layout_h, sample_as_luminance, alpha_layer, false);
            if(dsc->tile && !tile_stamps) {
                EVE_CoDl_bitmapSize(phost, bmp_filter, REPEAT, REPEAT,
                                    LV_MIN(tile_extent_w, 2048), LV_MIN(tile_extent_h, 2048));
            }
            else {
                EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER, src_w, src_h);
            }
        }

        /* Set up image transform (if any) */
        int32_t draw_x, draw_y;
        if(!setup_image_transform(u, t, src_w, src_h, x, y, bmp_filter, &draw_x, &draw_y)) {
            EVE_CoDl_restoreContext(phost);
            goto cleanup;
        }

        /* Colorkey: build stencil mask and punch alpha holes */
        if(dsc->colorkey != NULL) {
            EVE_CoDl_clear(phost, 0, 1, 0);
            build_colorkey_stencil(phost, dsc->colorkey, eve_format, eve_stride, layout_h, draw_x, draw_y);
            EVE_CoDl_alphaFunc(phost, ALWAYS, 0);
            EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
            EVE_CoDl_stencilFunc(phost, EQUAL, 6, 0xFF);
            EVE_CoDl_stencilOp(phost, KEEP, KEEP);
            EVE_CoDl_colorA(phost, 0);
            EVE_CoDl_blendFunc(phost, ONE, ZERO);
            EVE_CoDl_begin(phost, RECTS);
            EVE_CoDl_lineWidth(phost, 16);
            EVE_CoDl_vertex2f_0(phost, box_x1, box_y1);
            EVE_CoDl_vertex2f_0(phost, box_x2, box_y2);
            EVE_CoDl_end(phost);
            EVE_CoDl_stencilFunc(phost, ALWAYS, 0, 0);
            EVE_CoDl_stencilOp(phost, KEEP, KEEP);
        }

        /* Phase 1c: Multiply mask by image alpha.
         * Premultiplied: scale mask by opa only (alpha is baked into RGB).
         * Non-premultiplied/alpha_to_rgb: multiply mask by bitmap alpha and opa. */
        if(is_premultiplied && !alpha_to_rgb) {
            if(dsc->opa < LV_OPA_MAX) {
                EVE_CoDl_colorA(phost, dsc->opa);
                EVE_CoDl_blendFunc(phost, ZERO, SRC_ALPHA);
                EVE_CoDl_lineWidth(phost, 16);
                EVE_CoDl_begin(phost, RECTS);
                EVE_CoDl_vertex2f_0(phost, box_x1, box_y1);
                EVE_CoDl_vertex2f_0(phost, box_x2, box_y2);
                EVE_CoDl_end(phost);
            }
        }
        else {
            EVE_CoDl_colorA(phost, dsc->opa);
            EVE_CoDl_blendFunc(phost, ZERO, SRC_ALPHA);
            draw_image_bitmap(phost, draw_x, draw_y, stamp_w, stamp_h, src_w, src_h);
        }

        /* Phase 2: Draw through the alpha mask */
        EVE_CoDl_colorMask(phost, 1, 1, 1, 1);
        if(alpha_to_rgb) {
            /* Fill white RECT through mask for L8 alpha capture */
            EVE_CoDl_colorRgb(phost, 255, 255, 255);
            EVE_CoDl_colorA(phost, 255);
            EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE_MINUS_DST_ALPHA);
            EVE_CoDl_lineWidth(phost, 16);
            EVE_CoDl_begin(phost, RECTS);
            EVE_CoDl_vertex2f_0(phost, box_x1, box_y1);
            EVE_CoDl_vertex2f_0(phost, box_x2, box_y2);
            EVE_CoDl_end(phost);
        }
        else if(is_premultiplied) {
            if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE)
                EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE);
            else
                EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE_MINUS_DST_ALPHA);
            EVE_CoDl_colorRgb(phost, tint.red, tint.green, tint.blue);
            EVE_CoDl_colorA(phost, 255);
            draw_image_bitmap(phost, draw_x, draw_y, stamp_w, stamp_h, src_w, src_h);
        }
        else if(!alpha_only && dsc->recolor_opa > LV_OPA_MIN) {
            /* Per-pixel recolor: out = image*(1-mix) + recolor*mix */
            uint8_t mix = dsc->recolor_opa;

            /* Phase 2a: Draw image dimmed by (1-mix) */
            if(mix < LV_OPA_COVER) {
                uint8_t dim = 255 - mix;
                EVE_CoDl_colorMask(phost, 1, 1, 1, 0);
                EVE_CoDl_colorRgb(phost, dim, dim, dim);
                EVE_CoDl_colorA(phost, 255);
                if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE)
                    EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE);
                else
                    EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE_MINUS_DST_ALPHA);
                draw_image_bitmap(phost, draw_x, draw_y, stamp_w, stamp_h, src_w, src_h);
            }

            /* Phase 2b: Add recolor*mix */
            EVE_CoDl_colorMask(phost, 1, 1, 1, 0);
            EVE_CoDl_colorRgb(phost,
                              (uint8_t)((uint32_t)dsc->recolor.red * mix / 255),
                              (uint8_t)((uint32_t)dsc->recolor.green * mix / 255),
                              (uint8_t)((uint32_t)dsc->recolor.blue * mix / 255));
            EVE_CoDl_colorA(phost, 255);
            if(mix < LV_OPA_COVER || dsc->blend_mode == LV_BLEND_MODE_ADDITIVE)
                EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE);
            else
                EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE_MINUS_DST_ALPHA);
            EVE_CoDl_lineWidth(phost, 16);
            EVE_CoDl_begin(phost, RECTS);
            EVE_CoDl_vertex2f_0(phost, box_x1, box_y1);
            EVE_CoDl_vertex2f_0(phost, box_x2, box_y2);
            EVE_CoDl_end(phost);
        }
        else {
            /* No recolor: standard compositing through mask. An alpha-only
             * image is its color over the mask, which holds its alpha: drawn
             * flat, as its white would be darkened where filtering mixes in
             * the bitmap's border. */
            if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE)
                EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE);
            else
                EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE_MINUS_DST_ALPHA);
            EVE_CoDl_colorRgb(phost, tint.red, tint.green, tint.blue);
            EVE_CoDl_colorA(phost, 255);
            if(alpha_only) {
                EVE_CoDl_lineWidth(phost, 16);
                EVE_CoDl_begin(phost, RECTS);
                EVE_CoDl_vertex2f_0(phost, box_x1, box_y1);
                EVE_CoDl_vertex2f_0(phost, box_x2, box_y2);
                EVE_CoDl_end(phost);
            }
            else {
                draw_image_bitmap(phost, draw_x, draw_y, stamp_w, stamp_h, src_w, src_h);
            }
        }

        EVE_CoDl_restoreContext(phost);

        if(!alpha_to_rgb) {
            lv_draw_eve5_track_alpha_trashed(u, box_x1, box_y1, box_x2, box_y2);
        }
    }
    else if(!alpha_only && dsc->recolor_opa > LV_OPA_MIN) {
        /* Unified recolor: out = src * (1-mix) + recolor * mix */
        uint8_t mix = dsc->recolor_opa;

        int32_t mask_x1 = t->clip_area.x1 - layer->buf_area.x1;
        int32_t mask_y1 = t->clip_area.y1 - layer->buf_area.y1;
        int32_t mask_x2 = t->clip_area.x2 - layer->buf_area.x1;
        int32_t mask_y2 = t->clip_area.y2 - layer->buf_area.y1;

        EVE_CoDl_vertexFormat(phost, 0);
        EVE_CoDl_saveContext(phost);

        int32_t draw_vx, draw_vy;
        if(!setup_image_transform(u, t, src_w, src_h, x, y, bmp_filter, &draw_vx, &draw_vy)) {
            EVE_CoDl_restoreContext(phost);
            goto cleanup;
        }

        /* Dim pass: render image with colorRgb(1-mix) for partial recolor.
         * Premultiplied pixels are scaled by opa themselves. */
        if(mix < LV_OPA_COVER) {
            uint8_t dim = 255 - mix;
            if(is_premultiplied) dim = (uint8_t)((uint32_t)dim * dsc->opa / 255);
            EVE_CoDl_colorRgb(phost, dim, dim, dim);
            EVE_CoDl_colorA(phost, dsc->opa);
            if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE || is_premultiplied)
                EVE_CoDl_blendFunc(phost, is_premultiplied ? ONE : SRC_ALPHA,
                                   dsc->blend_mode == LV_BLEND_MODE_ADDITIVE ? ONE : ONE_MINUS_SRC_ALPHA);
            draw_image_bitmap(phost, draw_vx, draw_vy, stamp_w, stamp_h, src_w, src_h);
        }

        /* Add pass: stamp image alpha, fill with recolor */
        EVE_CoDl_colorArgb_ex(phost, 0x00000000);
        EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
        EVE_CoDl_blendFunc(phost, ONE, ZERO);
        lv_draw_eve5_draw_rect(u, mask_x1, mask_y1, mask_x2, mask_y2, 0,
                               &t->clip_area, &layer->buf_area);

        EVE_CoDl_colorA(phost, (uint8_t)(mix < LV_OPA_COVER
                                         ? ((uint32_t)mix * dsc->opa / 255) : dsc->opa));
        draw_image_bitmap(phost, draw_vx, draw_vy, stamp_w, stamp_h, src_w, src_h);

        EVE_CoDl_colorMask(phost, 1, 1, 1, 1);
        EVE_CoDl_colorRgb(phost, dsc->recolor.red, dsc->recolor.green, dsc->recolor.blue);
        EVE_CoDl_colorA(phost, 255);
        if(mix < LV_OPA_COVER || dsc->blend_mode == LV_BLEND_MODE_ADDITIVE)
            EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE);
        else
            EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE_MINUS_DST_ALPHA);
        lv_draw_eve5_draw_rect(u, mask_x1, mask_y1, mask_x2, mask_y2, 0,
                               &t->clip_area, &layer->buf_area);

        EVE_CoDl_restoreContext(phost);

        lv_draw_eve5_track_alpha_trashed(u, mask_x1, mask_y1, mask_x2, mask_y2);
    }
    else {
        /* Standard draw path: no clip radius, bitmap mask, or recolor */
        bool has_colorkey = (dsc->colorkey != NULL);
        bool scoped = has_colorkey || has_any_transform;

        if(scoped) {
            EVE_CoDl_vertexFormat(phost, 0);
            EVE_CoDl_saveContext(phost);
        }

        int32_t draw_vx, draw_vy;
        if(!setup_image_transform(u, t, src_w, src_h, x, y, bmp_filter, &draw_vx, &draw_vy)) {
            if(scoped) EVE_CoDl_restoreContext(phost);
            goto cleanup;
        }

        if(has_colorkey) {
            EVE_CoDl_clear(phost, 0, 1, 0);
            build_colorkey_stencil(phost, dsc->colorkey, eve_format, eve_stride, layout_h, draw_vx, draw_vy);
            EVE_CoDl_alphaFunc(phost, ALWAYS, 0);
            EVE_CoDl_colorMask(phost, 1, 1, 1, 1);
            EVE_CoDl_stencilFunc(phost, NOTEQUAL, 6, 0xFF);
            EVE_CoDl_stencilOp(phost, KEEP, KEEP);
            /* Restore colors */
            if(is_premultiplied) {
                uint8_t opa = dsc->opa;
                EVE_CoDl_colorRgb(phost,
                                  (uint8_t)(tint.red * opa / 255),
                                  (uint8_t)(tint.green * opa / 255),
                                  (uint8_t)(tint.blue * opa / 255));
                EVE_CoDl_colorA(phost, opa);
            }
            else {
                EVE_CoDl_colorA(phost, dsc->opa);
                EVE_CoDl_colorRgb(phost, tint.red, tint.green, tint.blue);
            }
        }

        if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE) {
            EVE_CoDl_blendFunc(phost, is_premultiplied ? ONE : SRC_ALPHA, ONE);
        }
        else if(is_premultiplied) {
            EVE_CoDl_blendFunc(phost, ONE, ONE_MINUS_SRC_ALPHA);
        }

        draw_image_bitmap(phost, draw_vx, draw_vy, stamp_w, stamp_h, src_w, src_h);

        if(scoped) {
            EVE_CoDl_restoreContext(phost);
        }
        else if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE || is_premultiplied) {
            EVE_CoDl_blendFunc_default(phost);
        }
    }

cleanup:
    LV_UNUSED(child_handle);
}

#endif /* LV_USE_DRAW_EVE5 */
