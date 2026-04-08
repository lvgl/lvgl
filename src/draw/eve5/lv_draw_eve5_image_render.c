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
#if LV_USE_OS
#include "../../drivers/display/eve5/lv_eve5.h"
#endif

/**********************
 * IMAGE DRAWING
 **********************/

/* alpha_to_rgb: renders the task's alpha contribution as grayscale luminance
 * into RGB, for later copying into a layer's alpha channel. Requires the
 * caller to use default blend mode: blend(SRC_ALPHA, ONE_MINUS_SRC_ALPHA)
 * with colorMask(1,1,1,1). The A channel is scratch space in this mode. */
void lv_draw_eve5_hal_draw_image(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t, bool alpha_to_rgb)
{
    EVE_HalContext *phost = u->hal;
    lv_layer_t *layer = t->target_layer;
    lv_draw_image_dsc_t *dsc = t->draw_dsc;

    uint32_t ram_g_addr;
    uint16_t eve_format;
    int32_t eve_stride;
    int32_t src_w, src_h;
    int32_t layout_h;  /* Height for bitmapLayout (may differ from src_h for render targets) */
    uint32_t palette_addr = GA_INVALID;  /* RAM_G address of palette LUT (GA_INVALID = non-paletted) */
    Esd_GpuHandle child_handle = GA_HANDLE_INVALID;

    /* Resolve bitmap source.
     * LAYER: GPU-rendered child layer in RAM_G.
     * May be ARGB8 render target or native format from canvas direct image optimization.
     * NOTE: Only GPU-rendered layers are supported (vram_res holds GPU handle).
     * CPU-rendered layers (draw_buf only) are filtered out by the dispatcher. */
    if(t->type == LV_DRAW_TASK_TYPE_LAYER) {
        lv_layer_t *child_layer = (lv_layer_t *)dsc->src;
        src_w = lv_area_get_width(&child_layer->buf_area);
        src_h = lv_area_get_height(&child_layer->buf_area);

        /* Read GPU handle and format directly from vram_res */
        lv_draw_eve5_vram_res_t *child_vr = eve5_get_vram_res(child_layer);
        if(child_vr == NULL) {
            LV_LOG_WARN("EVE5: Child layer %p has no vram_res", (void *)child_layer);
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
        /* Regular image — resolve to GPU via unified loading chain */
        eve5_gpu_image_t img;
        if(!lv_draw_eve5_resolve_to_gpu(u, dsc->src, &img)) return;
        eve5_gpu_image_resolve(u->allocator, &img, &ram_g_addr, &palette_addr);
        if(ram_g_addr == GA_INVALID) return;
        eve_format = img.eve_format;
        eve_stride = img.eve_stride;
        src_w = img.width;
        src_h = img.height;
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
        eve5_gpu_image_t mask_img;
        if(lv_draw_eve5_resolve_to_gpu(u, dsc->bitmap_mask_src, &mask_img)) {
            eve5_gpu_image_resolve(u->allocator, &mask_img, &mask_ram_g_addr, &mask_palette_addr);
            if(mask_ram_g_addr != GA_INVALID) {
                mask_eve_format = mask_img.eve_format;
                mask_eve_stride = mask_img.eve_stride;
                mask_w = mask_img.width;
                mask_h = mask_img.height;
                has_bitmap_mask = true;
            }
        }
        if(!has_bitmap_mask) {
            LV_LOG_WARN("EVE5: Failed to load bitmap mask");
        }
    }

    /* Calculate position in layer coordinates */
    int32_t x = t->area.x1 - layer->buf_area.x1;
    int32_t y = t->area.y1 - layer->buf_area.y1;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    /* Premultiplied content (layers rendered with SRC_ALPHA, or ARGB8888_PREMULTIPLIED images):
     * pixel.rgb = actual_color * alpha, pixel.a = alpha (squared for layers).
     * For compositing, use blend(ONE, ONE_MINUS_SRC_ALPHA) to avoid
     * double-applying alpha to RGB. Scale vertex color by opa so the
     * premultiplied content is properly attenuated by opacity.
     * Regular images use standard SRC_ALPHA blend with unscaled colors. */
    bool is_layer = (t->type == LV_DRAW_TASK_TYPE_LAYER);
    bool is_premultiplied;
    if(is_layer) {
        lv_layer_t *child_layer = (lv_layer_t *)dsc->src;
        lv_draw_eve5_vram_res_t *pvr = eve5_get_vram_res(child_layer);
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
        lv_draw_eve5_vram_res_t *ivr = eve5_get_image_vram_res(img_dsc);
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
    EVE_CoDl_bitmapLayout(u->hal, (uint8_t)eve_format, eve_stride, layout_h);
    uint8_t bmp_filter = dsc->antialias ? BILINEAR : NEAREST;
    if(dsc->tile) {
        int32_t tile_w = lv_area_get_width(&dsc->image_area);
        int32_t tile_h = lv_area_get_height(&dsc->image_area);
        EVE_CoDl_bitmapSize(u->hal, bmp_filter, REPEAT, REPEAT, tile_w, tile_h);
    }
    else {
        EVE_CoDl_bitmapSize(u->hal, bmp_filter, BORDER, BORDER, src_w, src_h);
    }
    /* BT820 L-format natively decodes as (255,255,255,L) — alpha-only formats
     * (A1/A2/A4/A8 mapped to L1/L2/L4/L8) already have luminance in alpha and
     * RGB=white for colorRgb modulation.  No swizzle needed for either path.
     * BITMAP_SWIZZLE only applies in GLFORMAT mode on BT820. */
    /* if(is_alpha_only)
        EVE_CoDl_bitmapSwizzle(phost, ONE, ONE, ONE, ALPHA);
    else
        EVE_CoDl_bitmapSwizzle(phost, RED, GREEN, BLUE, ALPHA); */

    /* Alpha-channel masking for clip_radius, bitmap_mask_src, alpha_to_rgb,
     * or colorkey+recolor (the non-premultiplied recolor path below doesn't
     * handle colorkey stencil, so route it through here instead).
     * Uses the same multi-phase approach as gradient fill masking:
     * Phase 1a: clear bbox alpha, 1b: write rounded rect mask (if clip_radius),
     * 1b2: apply bitmap mask (if bitmap_mask_src),
     * 1c: multiply mask by image alpha, 2: draw image through mask.
     * alpha_to_rgb routes through this path to reuse the transform/colorkey
     * handling; phase 2 draws a white RECT instead of the image bitmap. */
    if(dsc->clip_radius > 0 || has_bitmap_mask || alpha_to_rgb
       || (dsc->colorkey != NULL && dsc->recolor_opa > LV_OPA_MIN)) {
        /* Convert image_area from absolute to layer-local coordinates */
        int32_t mask_x1 = dsc->image_area.x1 - layer->buf_area.x1;
        int32_t mask_y1 = dsc->image_area.y1 - layer->buf_area.y1;
        int32_t mask_x2 = dsc->image_area.x2 - layer->buf_area.x1;
        int32_t mask_y2 = dsc->image_area.y2 - layer->buf_area.y1;

        /* Bitmap transform may be non-identity from a previous draw —
         * mask bitmap phases below need identity, so set it before save. */
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
        else if(alpha_to_rgb && !has_bitmap_mask) {
            /* alpha_to_rgb without clip or mask: fill bbox A=255 so phase 1c
             * multiply produces image_alpha (instead of 0 × image_alpha = 0). */
            EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
            lv_draw_eve5_draw_rect(u, mask_x1, mask_y1, mask_x2, mask_y2, 0,
                                    &t->clip_area, &layer->buf_area);
        }
        else if(dsc->colorkey != NULL && !has_bitmap_mask) {
            /* Colorkey + recolor (no clip_radius): fill bbox A=255 so the
             * colorkey punch and image alpha multiply produce correct mask. */
            EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
            lv_draw_eve5_draw_rect(u, mask_x1, mask_y1, mask_x2, mask_y2, 0,
                                    &t->clip_area, &layer->buf_area);
        }

        /* Phase 1b2: Apply bitmap mask (A8/L8 texture as per-pixel alpha).
         * Combined with clip_radius: multiplies existing rounded rect alpha.
         * Standalone: overwrites alpha directly (ONE,ZERO carries from 1a). */
        if(has_bitmap_mask) {
            if(dsc->clip_radius > 0) {
                EVE_CoDl_blendFunc(phost, ZERO, SRC_ALPHA);
            }
            /* else: ONE,ZERO from Phase 1a overwrites alpha with mask values */

            /* Center-align mask on image_area */
            int32_t img_w = lv_area_get_width(&dsc->image_area);
            int32_t img_h = lv_area_get_height(&dsc->image_area);
            int32_t mask_draw_x = mask_x1 + (img_w - mask_w) / 2;
            int32_t mask_draw_y = mask_y1 + (img_h - mask_h) / 2;

            /* Nested save/restore to preserve main image bitmap config */
            EVE_CoDl_saveContext(phost);
            EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
            EVE_CoDl_bitmapSource(phost, mask_ram_g_addr);
            set_palette_if_needed(phost, mask_eve_format, mask_palette_addr);
            /* For ARGB8 or PALETTEDARGB8: use GLFORMAT + swizzle to extract RED channel as alpha
             * (grayscale PNGs decode as ARGB8/PALETTEDARGB8 with R=G=B=gray, A=255).
             * For L8/A8: BT820 natively decodes as (255,255,255,L) — no swizzle needed. */
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

            /* Re-setup main image bitmap — restoreContext doesn't restore per-handle
             * bitmap state (source, layout, size), only graphics context. The scratch
             * handle still has mask config after restoreContext; we must re-configure
             * it with the main image before Phase 1c and Phase 2 draw. */
            EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
            EVE_CoDl_bitmapSource(phost, ram_g_addr);
            set_palette_if_needed(phost, eve_format, palette_addr);
            EVE_CoDl_bitmapLayout(phost, (uint8_t)eve_format, eve_stride, layout_h);
            if(dsc->tile) {
                int32_t tile_w = lv_area_get_width(&dsc->image_area);
                int32_t tile_h = lv_area_get_height(&dsc->image_area);
                EVE_CoDl_bitmapSize(phost, bmp_filter, REPEAT, REPEAT, tile_w, tile_h);
            }
            else {
                EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER, src_w, src_h);
            }
        }

        /* Set up image transform (if any) — stays active for phases 1c and 2.
         * For tiled images, use image_area origin so tile pattern aligns. */
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
                /* Rotation and/or scaling only — CoCmd matrix pipeline */
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
                /* Expand bitmapSize to cover rotated/scaled output —
                 * source dimensions would clip the transformed bounding box. */
                EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER,
                    LV_MIN(lv_area_get_width(&t->clip_area), 2048),
                    LV_MIN(lv_area_get_height(&t->clip_area), 2048));
            }
        }

        /* Colorkey + clip_radius: build stencil mask and punch alpha holes.
         * After the 6-pass stencil (stencil==6 at colorkey pixels), zero the
         * alpha mask at those positions so phases 1c and 2 naturally skip them. */
        if(dsc->colorkey != NULL) {
            /* clearStencil defaults to 0 and nothing changes it */
            EVE_CoDl_clear(phost, 0, 1, 0);
            build_colorkey_stencil(phost, dsc->colorkey, eve_format, eve_stride, layout_h, draw_x, draw_y);
            /* BT820: build_colorkey_stencil restores original layout on return,
             * no swizzle restore needed (swizzle only active in GLFORMAT mode). */
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
            /* Disable stencil for subsequent phases */
            EVE_CoDl_stencilFunc(phost, ALWAYS, 0, 0);
            EVE_CoDl_stencilOp(phost, KEEP, KEEP);
        }

        /* Phase 1c: Multiply mask by image alpha.
         *
         * Non-premultiplied / alpha_to_rgb: multiply mask by bitmap alpha and opa.
         * The bitmap's per-pixel alpha modulates the mask so transparent parts
         * of the source don't contribute.
         *
         * Premultiplied: scale mask by opa only (full-coverage RECT, not bitmap).
         * Premultiplied content has alpha baked into RGB — multiplying the mask
         * by bitmap alpha would double-apply it. Phase 2 uses DST_ALPHA blend
         * to gate the source by the mask, so the premultiplied alpha naturally
         * modulates the result without an explicit mask multiply. */
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

        /* Phase 2: Draw through the alpha mask. */
        EVE_CoDl_colorMask(phost, 1, 1, 1, 1);
        if(alpha_to_rgb) {
            /* alpha_to_rgb: fill white RECT through mask — equivalent to full
             * recolor to white. result.r = mask + prev.r * (1 - mask/255). */
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
            /* Premultiplied compositing through mask.
             * DST_ALPHA = mask * opa (spatial mask scaled by opacity).
             * blend(DST_ALPHA, ONE_MINUS_DST_ALPHA) gates the source by the mask
             * and fades the destination. Opa is already in DST_ALPHA from phase 1c,
             * so colorRgb carries only the recolor tint (or white = no tint). */
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
            /* Correct per-pixel recolor: out = image*(1-mix) + recolor*mix.
             * Phase 2a draws dimmed image through mask; phase 2b adds
             * recolor contribution additively. Exact at all coverage levels.
             * Replaces the old colorRgb modulation which was only approximate. */
            uint8_t mix = dsc->recolor_opa;

            /* Phase 2a: Draw image dimmed by (1-mix) through the mask.
             * colorMask(1,1,1,0) preserves mask in A for additive recolor pass.
             * Skipped for full recolor (dim=0, image contributes nothing). */
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

            /* Phase 2b: Add recolor*mix through the mask.
             * Partial: additive blend onto the dimmed image from 2a.
             * Full: standard compositing (no 2a, replace through mask). */
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
            /* No recolor — standard compositing through mask. */
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
            /* Image masking uses alpha-as-scratch */
            lv_draw_eve5_track_alpha_trashed(u, mask_x1, mask_y1, mask_x2, mask_y2);
        }
    }
    else if(!is_premultiplied && dsc->recolor_opa > LV_OPA_MIN) {
        /* Unified recolor via dim + alpha-masked add.
         * Correct per-pixel RGB lerp: out = src * (1-mix) + recolor * mix.
         * Partial recolor (two-pass): dim pass renders image with colorRgb(1-mix),
         * then add pass stamps image alpha and fills with recolor additively.
         * Full recolor (single-pass): skip dim (would be black), replace with recolor. */
        uint8_t mix = dsc->recolor_opa;

        int32_t mask_x1 = t->clip_area.x1 - layer->buf_area.x1;
        int32_t mask_y1 = t->clip_area.y1 - layer->buf_area.y1;
        int32_t mask_x2 = t->clip_area.x2 - layer->buf_area.x1;
        int32_t mask_y2 = t->clip_area.y2 - layer->buf_area.y1;

        int32_t draw_vx = x;
        int32_t draw_vy = y;

        EVE_CoDl_bitmapTransform_identity(phost);
        EVE_CoDl_vertexFormat(phost, 0);

        /* Dim pass: render image with colorRgb(1-mix) for partial recolor.
         * Produces src * (1-mix) with correct alpha via standard blending.
         * Skipped for full recolor (mix==255) since it would render black. */
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

        /* Add pass: stamp image alpha, fill with recolor.
         * For partial recolor: additive blend adds recolor * mix * img_alpha.
         * For full recolor: replace blend fills recolor * img_alpha. */
        EVE_CoDl_saveContext(phost);

        /* Phase 1a: Clear bbox alpha to 0 */
        EVE_CoDl_colorArgb_ex(phost, 0x00000000);
        EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
        EVE_CoDl_blendFunc(phost, ONE, ZERO);
        lv_draw_eve5_draw_rect(u, mask_x1, mask_y1, mask_x2, mask_y2, 0,
                                &t->clip_area, &layer->buf_area);

        /* Phase 1b: Stamp image alpha into render target alpha.
         * blend(ONE, ZERO) overwrites dest alpha with texel.a * colorA.
         * colorA = mix * opa / 255 for partial (scales recolor contribution),
         * colorA = opa for full recolor (stamp full image alpha). */
        EVE_CoDl_colorA(phost, (uint8_t)(mix < LV_OPA_COVER
            ? ((uint32_t)mix * dsc->opa / 255) : dsc->opa));
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, draw_vx, draw_vy);
        EVE_CoDl_end(phost);

        /* Phase 2: Fill with recolor through the alpha mask */
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

        /* Alpha channel used as scratch — mark for repair */
        lv_draw_eve5_track_alpha_trashed(u, mask_x1, mask_y1, mask_x2, mask_y2);
    }
    else {
        /* No clip radius, bitmap mask, or recolor — standard draw path.
         * Structured as linear phases: compute → save → transform → colorkey → draw → restore.
         * Colorkey uses stencil test directly (no alpha masking needed).
         * Non-layer recolor is handled entirely by the unified recolor block above.
         * Layer recolor and masking-path recolor still use colorRgb modulation. */
        bool has_colorkey = (dsc->colorkey != NULL);
        bool has_transform = (dsc->rotation != 0 || dsc->scale_x != LV_SCALE_NONE
                             || dsc->scale_y != LV_SCALE_NONE
                             || dsc->skew_x != 0 || dsc->skew_y != 0);
        bool has_skew = (dsc->skew_x != 0 || dsc->skew_y != 0);

        /* Compute draw vertex position */
        int32_t draw_vx, draw_vy;
        if(has_transform) {
            draw_vx = t->clip_area.x1 - layer->buf_area.x1;
            draw_vy = t->clip_area.y1 - layer->buf_area.y1;
        }
        else {
            draw_vx = dsc->tile ? (dsc->image_area.x1 - layer->buf_area.x1) : x;
            draw_vy = dsc->tile ? (dsc->image_area.y1 - layer->buf_area.y1) : y;
        }

        /* Skew: compute inverse affine transform.
         * Done before saveContext so degenerate matrix can early-return cleanly. */
        image_skew_t skew;
        if(has_skew) {
            if(!compute_image_skew(&skew, dsc->rotation, dsc->scale_x, dsc->scale_y,
                                   dsc->skew_x, dsc->skew_y, dsc->pivot.x, dsc->pivot.y,
                                   src_w, src_h, x, y, draw_vx, draw_vy))
                goto cleanup;
        }

        /* Bitmap transform may be non-identity from a previous draw —
         * when not applying our own transform, reset it before use. */
        if(!has_skew && !has_transform) {
            EVE_CoDl_bitmapTransform_identity(phost);
        }

        /* Save context for non-optimized state (transform, colorkey stencil) */
        if(has_skew || has_colorkey || has_transform) {
            EVE_CoDl_vertexFormat(phost, 0);
            EVE_CoDl_saveContext(phost);
        }

        /* Apply bitmap transform */
        if(has_skew) {
            apply_image_skew(phost, &skew, bmp_filter,
                dsc->tile ? lv_area_get_width(&dsc->image_area) : 0,
                dsc->tile ? lv_area_get_height(&dsc->image_area) : 0);
        }
        else if(has_transform) {
            /* Rotation and/or scaling only — CoCmd matrix pipeline */
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
            /* Expand bitmapSize to cover rotated/scaled output —
             * source dimensions would clip the transformed bounding box. */
            EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER,
                LV_MIN(lv_area_get_width(&t->clip_area), 2048),
                LV_MIN(lv_area_get_height(&t->clip_area), 2048));
        }

        /* Build colorkey stencil mask (transform is active, stencil samples match draw) */
        if(has_colorkey) {
            /* clearStencil defaults to 0 and nothing changes it */
            EVE_CoDl_clear(phost, 0, 1, 0);
            build_colorkey_stencil(phost, dsc->colorkey, eve_format, eve_stride, layout_h, draw_vx, draw_vy);
            /* BT820: build_colorkey_stencil restores original layout on return,
             * no swizzle restore needed (swizzle only active in GLFORMAT mode). */
            EVE_CoDl_alphaFunc(phost, ALWAYS, 0);
            EVE_CoDl_colorMask(phost, 1, 1, 1, 1);
            EVE_CoDl_stencilFunc(phost, NOTEQUAL, 6, 0xFF);
            EVE_CoDl_stencilOp(phost, KEEP, KEEP);
            /* Restore colors overwritten by stencil passes */
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

        /* Handle blend mode */
        if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE) {
            EVE_CoDl_blendFunc(phost, is_premultiplied ? ONE : SRC_ALPHA, ONE);
        }
        else if(is_premultiplied) {
            /* Premultiplied compositing: RGB already contains color*alpha,
             * so source factor = ONE (don't re-multiply by alpha).
             * Destination fades out via ONE_MINUS_SRC_ALPHA. */
            EVE_CoDl_blendFunc(phost, ONE, ONE_MINUS_SRC_ALPHA);
        }

        /* Draw bitmap */
        EVE_CoDl_begin(u->hal, BITMAPS);
        EVE_CoDl_vertex2f_0(u->hal, draw_vx, draw_vy);
        EVE_CoDl_end(u->hal);

        /* Restore state */
        if(has_skew || has_colorkey || has_transform) {
            EVE_CoDl_restoreContext(phost);
        }
        else if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE || is_premultiplied) {
            EVE_CoDl_blendFunc_default(phost);
        }
    }

cleanup:
    /* Child layer GPU memory lifecycle is managed by lv_draw_buf_destroy
     * via vram_free_cb — do NOT track_frame_alloc here (would double-free). */
    LV_UNUSED(child_handle);
}

#endif /* LV_USE_DRAW_EVE5 */
