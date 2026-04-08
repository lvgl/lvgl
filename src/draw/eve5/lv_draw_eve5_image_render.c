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

#include "../lv_draw.h"
#include "../lv_draw_image.h"

/**********************
 * IMAGE DRAWING
 **********************/

/**
 * Draw image or layer to the target layer.
 *
 * @param alpha_to_rgb  When true, renders alpha contribution as grayscale luminance
 *                      for later copying into a layer's alpha channel. Caller must
 *                      use default blend mode with colorMask(1,1,1,1). The A channel
 *                      is scratch space in this mode.
 */
void lv_draw_eve5_hal_draw_image(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t, bool alpha_to_rgb)
{
    EVE_HalContext *phost = u->hal;
    lv_layer_t *layer = t->target_layer;
    lv_draw_image_dsc_t *dsc = t->draw_dsc;

    uint32_t ram_g_addr;
    uint16_t eve_format;
    int32_t eve_stride;
    int32_t src_w, src_h;
    int32_t layout_h;
    uint32_t palette_addr = GA_INVALID;
    Esd_GpuHandle child_handle = GA_HANDLE_INVALID;

    /* Resolve bitmap source */
    if(t->type == LV_DRAW_TASK_TYPE_LAYER) {
        lv_layer_t *child_layer = (lv_layer_t *)dsc->src;
        src_w = lv_area_get_width(&child_layer->buf_area);
        src_h = lv_area_get_height(&child_layer->buf_area);

        lv_eve5_vram_res_t *child_vr = eve5_get_vram_res(child_layer);
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
        ram_g_addr = Esd_GpuAlloc_Get(u->allocator, child_handle);
        if(ram_g_addr == GA_INVALID) {
            LV_LOG_WARN("EVE5: Child layer %p texture invalid", (void *)child_layer);
            return;
        }
        ram_g_addr += child_vr->source_offset;
        eve_format = child_vr->eve_format;
        eve_stride = (int32_t)child_vr->stride;
        layout_h = src_h;
        if(child_vr->palette_offset != GA_INVALID) {
            uint32_t base = Esd_GpuAlloc_Get(u->allocator, child_handle);
            palette_addr = base + child_vr->palette_offset;
        }
    }
    else {
        lv_eve5_vram_res_t *img = lv_draw_eve5_resolve_to_gpu(u, dsc->src);
        if(!img) return;
        eve5_vram_res_resolve(u->allocator, img, &ram_g_addr, &palette_addr);
        if(ram_g_addr == GA_INVALID) return;
        eve_format = img->eve_format;
        eve_stride = (int32_t)img->stride;
        src_w = img->width;
        src_h = img->height;
        layout_h = src_h;
    }

    /* Load bitmap mask if set */
    bool has_bitmap_mask = false;
    uint32_t mask_ram_g_addr = GA_INVALID;
    int32_t mask_w = 0, mask_h = 0;
    int32_t mask_eve_stride = 0;
    uint16_t mask_eve_format = L8;
    uint32_t mask_palette_addr = GA_INVALID;
    if(dsc->bitmap_mask_src != NULL) {
        lv_eve5_vram_res_t *mask_img = lv_draw_eve5_resolve_to_gpu(u, dsc->bitmap_mask_src);
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

    /* Determine if content is premultiplied (RGB already scaled by alpha).
     * Premultiplied content uses blend(ONE, ONE_MINUS_SRC_ALPHA) to avoid
     * double-applying alpha. Vertex color is scaled by opa for attenuation. */
    bool is_layer = (t->type == LV_DRAW_TASK_TYPE_LAYER);
    bool is_premultiplied;
    if(is_layer) {
        lv_layer_t *child_layer = (lv_layer_t *)dsc->src;
        lv_eve5_vram_res_t *pvr = eve5_get_vram_res(child_layer);
        if(pvr != NULL) {
            is_premultiplied = pvr->is_premultiplied;
        }
        else if(child_layer->draw_buf != NULL) {
            is_premultiplied = lv_draw_buf_has_flag(child_layer->draw_buf, LV_IMAGE_FLAGS_PREMULTIPLIED);
        }
        else {
            is_premultiplied = false;
        }
    }
    else if(lv_image_src_get_type(dsc->src) == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t *img_dsc = (const lv_image_dsc_t *)dsc->src;
        lv_eve5_vram_res_t *ivr = eve5_get_image_vram_res(img_dsc);
        if(ivr != NULL) {
            is_premultiplied = ivr->is_premultiplied;
        }
        else {
            is_premultiplied = (img_dsc->header.flags & LV_IMAGE_FLAGS_PREMULTIPLIED) != 0
                            || img_dsc->header.cf == LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED;
        }
    }
    else {
        is_premultiplied = false;
    }

    if(is_premultiplied) {
        uint8_t opa = dsc->opa;
        if(dsc->recolor_opa > LV_OPA_MIN) {
            lv_color_t mixed = lv_color_mix(dsc->recolor, lv_color_white(), dsc->recolor_opa);
            EVE_CoDl_colorRgb(u->hal,
                (uint8_t)(mixed.red * opa / 255),
                (uint8_t)(mixed.green * opa / 255),
                (uint8_t)(mixed.blue * opa / 255));
        }
        else {
            EVE_CoDl_colorRgb(u->hal, opa, opa, opa);
        }
        EVE_CoDl_colorA(u->hal, opa);
    }
    else {
        EVE_CoDl_colorA(u->hal, dsc->opa);
        if(dsc->recolor_opa > LV_OPA_MIN) {
            lv_color_t mixed = lv_color_mix(dsc->recolor, lv_color_white(), dsc->recolor_opa);
            EVE_CoDl_colorRgb(u->hal, mixed.red, mixed.green, mixed.blue);
        }
        else {
            EVE_CoDl_colorRgb(u->hal, 255, 255, 255);
        }
    }

    /* Set up bitmap */
    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
    EVE_CoDl_bitmapSource(u->hal, ram_g_addr);
    set_palette_if_needed(u->hal, eve_format, palette_addr);
    eve5_set_bitmap_layout(u->hal, eve_format, eve_stride, layout_h);
    uint8_t bmp_filter = dsc->antialias ? BILINEAR : NEAREST;
    if(dsc->tile) {
        int32_t tile_w = lv_area_get_width(&dsc->image_area);
        int32_t tile_h = lv_area_get_height(&dsc->image_area);
        EVE_CoDl_bitmapSize(u->hal, bmp_filter, REPEAT, REPEAT, tile_w, tile_h);
    }
    else {
        EVE_CoDl_bitmapSize(u->hal, bmp_filter, BORDER, BORDER, src_w, src_h);
    }

    /* Alpha-channel masking path: clip_radius, bitmap_mask_src, alpha_to_rgb,
     * or colorkey+recolor. Uses multi-phase approach:
     * Phase 1a: clear bbox alpha
     * Phase 1b: write rounded rect mask (if clip_radius)
     * Phase 1b2: apply bitmap mask (if bitmap_mask_src)
     * Phase 1c: multiply mask by image alpha
     * Phase 2: draw image through mask */
    if(dsc->clip_radius > 0 || has_bitmap_mask || alpha_to_rgb
       || (dsc->colorkey != NULL && dsc->recolor_opa > LV_OPA_MIN)) {
        int32_t mask_x1 = dsc->image_area.x1 - layer->buf_area.x1;
        int32_t mask_y1 = dsc->image_area.y1 - layer->buf_area.y1;
        int32_t mask_x2 = dsc->image_area.x2 - layer->buf_area.x1;
        int32_t mask_y2 = dsc->image_area.y2 - layer->buf_area.y1;

        EVE_CoDl_bitmapTransform_identity(phost);
        EVE_CoDl_vertexFormat(phost, 0);
        EVE_CoDl_saveContext(phost);

        /* Phase 1a: Clear bbox alpha to 0 */
        EVE_CoDl_colorArgb_ex(phost, 0x00000000);
        EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
        EVE_CoDl_blendFunc(phost, ONE, ZERO);
        lv_draw_eve5_draw_rect(u, mask_x1, mask_y1, mask_x2, mask_y2, 0,
                                &t->clip_area, &layer->buf_area);

        /* Phase 1b: Write rounded rect mask (alpha=255 inside) */
        if(dsc->clip_radius > 0) {
            EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
            lv_draw_eve5_draw_rect(u, mask_x1, mask_y1, mask_x2, mask_y2,
                                    dsc->clip_radius, &t->clip_area, &layer->buf_area);
        }
        else if((alpha_to_rgb || dsc->colorkey != NULL) && !has_bitmap_mask) {
            /* No clip or mask: fill bbox A=255 so phase 1c multiply works */
            EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
            lv_draw_eve5_draw_rect(u, mask_x1, mask_y1, mask_x2, mask_y2, 0,
                                    &t->clip_area, &layer->buf_area);
        }

        /* Phase 1b2: Apply bitmap mask */
        if(has_bitmap_mask) {
            if(dsc->clip_radius > 0) {
                EVE_CoDl_blendFunc(phost, ZERO, SRC_ALPHA);
            }

            int32_t img_w = lv_area_get_width(&dsc->image_area);
            int32_t img_h = lv_area_get_height(&dsc->image_area);
            int32_t mask_draw_x = mask_x1 + (img_w - mask_w) / 2;
            int32_t mask_draw_y = mask_y1 + (img_h - mask_h) / 2;

            EVE_CoDl_saveContext(phost);
            EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
            EVE_CoDl_bitmapSource(phost, mask_ram_g_addr);
            set_palette_if_needed(phost, mask_eve_format, mask_palette_addr);
            /* ARGB8/PALETTEDARGB8: extract RED as alpha (grayscale PNGs decode with R=G=B=gray) */
            if(mask_eve_format == ARGB8 || mask_eve_format == PALETTEDARGB8) {
                EVE_CoDl_bitmapLayout(phost, GLFORMAT, mask_eve_stride, mask_h);
                EVE_CoDl_bitmapExtFormat(phost, mask_eve_format);
                EVE_CoDl_bitmapSwizzle(phost, ZERO, ZERO, ZERO, RED);
            }
            else {
                EVE_CoDl_bitmapLayout(phost, (uint8_t)mask_eve_format, mask_eve_stride, mask_h);
            }
            EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, mask_w, mask_h);
            EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
            EVE_CoDl_begin(phost, BITMAPS);
            EVE_CoDl_vertex2f_0(phost, mask_draw_x, mask_draw_y);
            EVE_CoDl_end(phost);
            EVE_CoDl_restoreContext(phost);

            /* Re-setup main image bitmap */
            EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
            EVE_CoDl_bitmapSource(phost, ram_g_addr);
            set_palette_if_needed(phost, eve_format, palette_addr);
            eve5_set_bitmap_layout(phost, eve_format, eve_stride, layout_h);
            if(dsc->tile) {
                int32_t tile_w = lv_area_get_width(&dsc->image_area);
                int32_t tile_h = lv_area_get_height(&dsc->image_area);
                EVE_CoDl_bitmapSize(phost, bmp_filter, REPEAT, REPEAT, tile_w, tile_h);
            }
            else {
                EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER, src_w, src_h);
            }
        }

        /* Set up image transform (if any) */
        int32_t draw_x = dsc->tile ? (dsc->image_area.x1 - layer->buf_area.x1) : x;
        int32_t draw_y = dsc->tile ? (dsc->image_area.y1 - layer->buf_area.y1) : y;
        if(dsc->rotation != 0 || dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE
           || dsc->skew_x != 0 || dsc->skew_y != 0) {
            int32_t clip_x = t->clip_area.x1 - layer->buf_area.x1;
            int32_t clip_y = t->clip_area.y1 - layer->buf_area.y1;
            draw_x = clip_x;
            draw_y = clip_y;

            if(dsc->skew_x != 0 || dsc->skew_y != 0) {
                image_skew_t skew;
                if(!compute_image_skew(&skew, dsc->rotation, dsc->scale_x, dsc->scale_y,
                                       dsc->skew_x, dsc->skew_y, dsc->pivot.x, dsc->pivot.y,
                                       src_w, src_h, x, y, clip_x, clip_y)) {
                    EVE_CoDl_restoreContext(phost);
                    goto cleanup;
                }
                apply_image_skew(phost, &skew, bmp_filter,
                    dsc->tile ? lv_area_get_width(&dsc->image_area) : 0,
                    dsc->tile ? lv_area_get_height(&dsc->image_area) : 0);
            }
            else {
                EVE_CoCmd_loadIdentity(u->hal);
                EVE_CoCmd_translate(u->hal, F16(x - clip_x + dsc->pivot.x), F16(y - clip_y + dsc->pivot.y));
                if(dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE) {
                    EVE_CoCmd_scale(u->hal, F16_SCALE_DIV_256(dsc->scale_x), F16_SCALE_DIV_256(dsc->scale_y));
                }
                if(dsc->rotation != 0) {
                    EVE_CoCmd_rotate(u->hal, DEGREES(dsc->rotation));
                }
                EVE_CoCmd_translate(u->hal, -F16(dsc->pivot.x), -F16(dsc->pivot.y));
                EVE_CoCmd_setMatrix(u->hal);
                EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER,
                    LV_MIN(lv_area_get_width(&t->clip_area), 2048),
                    LV_MIN(lv_area_get_height(&t->clip_area), 2048));
            }
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
            EVE_CoDl_vertex2f_0(phost, mask_x1, mask_y1);
            EVE_CoDl_vertex2f_0(phost, mask_x2, mask_y2);
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
                EVE_CoDl_vertex2f_0(phost, mask_x1, mask_y1);
                EVE_CoDl_vertex2f_0(phost, mask_x2, mask_y2);
                EVE_CoDl_end(phost);
            }
        }
        else {
            EVE_CoDl_colorA(phost, dsc->opa);
            EVE_CoDl_blendFunc(phost, ZERO, SRC_ALPHA);
            EVE_CoDl_begin(phost, BITMAPS);
            EVE_CoDl_vertex2f_0(phost, draw_x, draw_y);
            EVE_CoDl_end(phost);
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
            EVE_CoDl_vertex2f_0(phost, mask_x1, mask_y1);
            EVE_CoDl_vertex2f_0(phost, mask_x2, mask_y2);
            EVE_CoDl_end(phost);
        }
        else if(is_premultiplied) {
            if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE)
                EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE);
            else
                EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE_MINUS_DST_ALPHA);
            if(dsc->recolor_opa > LV_OPA_MIN) {
                lv_color_t mixed = lv_color_mix(dsc->recolor, lv_color_white(), dsc->recolor_opa);
                EVE_CoDl_colorRgb(phost, mixed.red, mixed.green, mixed.blue);
            }
            else {
                EVE_CoDl_colorRgb(phost, 255, 255, 255);
            }
            EVE_CoDl_colorA(phost, 255);
            EVE_CoDl_begin(phost, BITMAPS);
            EVE_CoDl_vertex2f_0(phost, draw_x, draw_y);
            EVE_CoDl_end(phost);
        }
        else if(dsc->recolor_opa > LV_OPA_MIN) {
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
                EVE_CoDl_begin(phost, BITMAPS);
                EVE_CoDl_vertex2f_0(phost, draw_x, draw_y);
                EVE_CoDl_end(phost);
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
            EVE_CoDl_vertex2f_0(phost, mask_x1, mask_y1);
            EVE_CoDl_vertex2f_0(phost, mask_x2, mask_y2);
            EVE_CoDl_end(phost);
        }
        else {
            /* No recolor: standard compositing through mask */
            if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE)
                EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE);
            else
                EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE_MINUS_DST_ALPHA);
            EVE_CoDl_colorRgb(phost, 255, 255, 255);
            EVE_CoDl_colorA(phost, 255);
            EVE_CoDl_begin(phost, BITMAPS);
            EVE_CoDl_vertex2f_0(phost, draw_x, draw_y);
            EVE_CoDl_end(phost);
        }

        EVE_CoDl_restoreContext(phost);

        if(!alpha_to_rgb) {
            lv_draw_eve5_track_alpha_trashed(u, mask_x1, mask_y1, mask_x2, mask_y2);
        }
    }
    else if(!is_premultiplied && dsc->recolor_opa > LV_OPA_MIN) {
        /* Unified recolor: out = src * (1-mix) + recolor * mix */
        uint8_t mix = dsc->recolor_opa;

        int32_t mask_x1 = t->clip_area.x1 - layer->buf_area.x1;
        int32_t mask_y1 = t->clip_area.y1 - layer->buf_area.y1;
        int32_t mask_x2 = t->clip_area.x2 - layer->buf_area.x1;
        int32_t mask_y2 = t->clip_area.y2 - layer->buf_area.y1;

        int32_t draw_vx = x;
        int32_t draw_vy = y;

        EVE_CoDl_bitmapTransform_identity(phost);
        EVE_CoDl_vertexFormat(phost, 0);

        /* Dim pass: render image with colorRgb(1-mix) for partial recolor */
        if(mix < LV_OPA_COVER) {
            uint8_t dim = 255 - mix;
            EVE_CoDl_colorRgb(phost, dim, dim, dim);
            EVE_CoDl_colorA(phost, dsc->opa);
            if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE)
                EVE_CoDl_blendFunc(phost, SRC_ALPHA, ONE);
            EVE_CoDl_begin(phost, BITMAPS);
            EVE_CoDl_vertex2f_0(phost, draw_vx, draw_vy);
            EVE_CoDl_end(phost);
            if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE)
                EVE_CoDl_blendFunc_default(phost);
        }

        /* Add pass: stamp image alpha, fill with recolor */
        EVE_CoDl_saveContext(phost);

        EVE_CoDl_colorArgb_ex(phost, 0x00000000);
        EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
        EVE_CoDl_blendFunc(phost, ONE, ZERO);
        lv_draw_eve5_draw_rect(u, mask_x1, mask_y1, mask_x2, mask_y2, 0,
                                &t->clip_area, &layer->buf_area);

        EVE_CoDl_colorA(phost, (uint8_t)(mix < LV_OPA_COVER
            ? ((uint32_t)mix * dsc->opa / 255) : dsc->opa));
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, draw_vx, draw_vy);
        EVE_CoDl_end(phost);

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
        bool has_transform = (dsc->rotation != 0 || dsc->scale_x != LV_SCALE_NONE
                             || dsc->scale_y != LV_SCALE_NONE
                             || dsc->skew_x != 0 || dsc->skew_y != 0);
        bool has_skew = (dsc->skew_x != 0 || dsc->skew_y != 0);

        int32_t draw_vx, draw_vy;
        if(has_transform) {
            draw_vx = t->clip_area.x1 - layer->buf_area.x1;
            draw_vy = t->clip_area.y1 - layer->buf_area.y1;
        }
        else {
            draw_vx = dsc->tile ? (dsc->image_area.x1 - layer->buf_area.x1) : x;
            draw_vy = dsc->tile ? (dsc->image_area.y1 - layer->buf_area.y1) : y;
        }

        image_skew_t skew;
        if(has_skew) {
            if(!compute_image_skew(&skew, dsc->rotation, dsc->scale_x, dsc->scale_y,
                                   dsc->skew_x, dsc->skew_y, dsc->pivot.x, dsc->pivot.y,
                                   src_w, src_h, x, y, draw_vx, draw_vy))
                goto cleanup;
        }

        if(!has_skew && !has_transform) {
            EVE_CoDl_bitmapTransform_identity(phost);
        }

        if(has_skew || has_colorkey || has_transform) {
            EVE_CoDl_vertexFormat(phost, 0);
            EVE_CoDl_saveContext(phost);
        }

        if(has_skew) {
            apply_image_skew(phost, &skew, bmp_filter,
                dsc->tile ? lv_area_get_width(&dsc->image_area) : 0,
                dsc->tile ? lv_area_get_height(&dsc->image_area) : 0);
        }
        else if(has_transform) {
            EVE_CoCmd_loadIdentity(u->hal);
            EVE_CoCmd_translate(u->hal, F16(x - draw_vx + dsc->pivot.x), F16(y - draw_vy + dsc->pivot.y));
            if(dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE) {
                EVE_CoCmd_scale(u->hal, F16_SCALE_DIV_256(dsc->scale_x), F16_SCALE_DIV_256(dsc->scale_y));
            }
            if(dsc->rotation != 0) {
                EVE_CoCmd_rotate(u->hal, DEGREES(dsc->rotation));
            }
            EVE_CoCmd_translate(u->hal, -F16(dsc->pivot.x), -F16(dsc->pivot.y));
            EVE_CoCmd_setMatrix(u->hal);
            EVE_CoCmd_loadIdentity(u->hal);
            EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER,
                LV_MIN(lv_area_get_width(&t->clip_area), 2048),
                LV_MIN(lv_area_get_height(&t->clip_area), 2048));
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
                if(dsc->recolor_opa > LV_OPA_MIN) {
                    lv_color_t mixed = lv_color_mix(dsc->recolor, lv_color_white(), dsc->recolor_opa);
                    EVE_CoDl_colorRgb(phost,
                        (uint8_t)(mixed.red * opa / 255),
                        (uint8_t)(mixed.green * opa / 255),
                        (uint8_t)(mixed.blue * opa / 255));
                }
                else {
                    EVE_CoDl_colorRgb(phost, opa, opa, opa);
                }
                EVE_CoDl_colorA(phost, opa);
            }
            else {
                EVE_CoDl_colorA(phost, dsc->opa);
                if(dsc->recolor_opa > LV_OPA_MIN) {
                    lv_color_t mixed = lv_color_mix(dsc->recolor, lv_color_white(), dsc->recolor_opa);
                    EVE_CoDl_colorRgb(phost, mixed.red, mixed.green, mixed.blue);
                }
                else {
                    EVE_CoDl_colorRgb(phost, 255, 255, 255);
                }
            }
        }

        if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE) {
            EVE_CoDl_blendFunc(phost, is_premultiplied ? ONE : SRC_ALPHA, ONE);
        }
        else if(is_premultiplied) {
            EVE_CoDl_blendFunc(phost, ONE, ONE_MINUS_SRC_ALPHA);
        }

        EVE_CoDl_begin(u->hal, BITMAPS);
        EVE_CoDl_vertex2f_0(u->hal, draw_vx, draw_vy);
        EVE_CoDl_end(u->hal);

        if(has_skew || has_colorkey || has_transform) {
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
