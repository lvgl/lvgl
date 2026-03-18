/**
 * @file lv_draw_eve5_image_alpha.c
 *
 * EVE5 (BT820) Image/Layer Alpha Correction Pass
 *
 * Separated from lv_draw_eve5_image.c. Contains:
 * - Alpha pass colorkey gate helper (builds stencil for colorkey exclusion)
 * - Alpha correction pass for IMAGE and LAYER tasks
 *   (clip_radius, bitmap_mask, colorkey, transforms)
 *
 * The RGB draw function (lv_draw_eve5_hal_draw_image) and shared helpers
 * (compute_image_skew, apply_image_skew, build_colorkey_stencil) remain
 * in lv_draw_eve5_image.c.
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
 * ALPHA PASS IMAGE/LAYER
 **********************/

/**
 * Alpha pass helper: build colorkey stencil and configure state so subsequent
 * draws are gated by stencilFunc(NOTEQUAL, 6) — colorkey-matching pixels
 * are excluded from drawing, preserving previously accumulated alpha.
 *
 * Requires a saveContext before calling. The caller draws their shape
 * (gated by the stencil), then calls restoreContext.
 *
 * On return:
 *   colorMask(0,0,0,1), blend(ONE, ONE_MINUS_SRC_ALPHA),
 *   stencilFunc(NOTEQUAL, 6), stencilOp(KEEP, KEEP).
 *   Bitmap state is dirty — caller must set up their own bitmap if needed.
 *
 * Returns false if skew computation fails (caller should restoreContext).
 */
static bool alpha_pass_build_colorkey_gate(lv_draw_eve5_unit_t *u,
                                            const lv_draw_task_t *t,
                                            const lv_draw_image_dsc_t *dsc,
                                            uint32_t ram_g_addr,
                                            uint16_t eve_format,
                                            int32_t eve_stride,
                                            int32_t layout_h,
                                            int32_t src_w, int32_t src_h,
                                            int32_t img_x, int32_t img_y,
                                            uint32_t palette_addr)
{
    EVE_HalContext *phost = u->hal;
    lv_layer_t *layer = t->target_layer;

    bool has_transform = (dsc->rotation != 0 || dsc->scale_x != LV_SCALE_NONE
                         || dsc->scale_y != LV_SCALE_NONE
                         || dsc->skew_x != 0 || dsc->skew_y != 0);
    bool has_skew = (dsc->skew_x != 0 || dsc->skew_y != 0);

    /* Compute draw vertex position (same as standard path) */
    int32_t draw_vx, draw_vy;
    if(has_transform) {
        draw_vx = t->clip_area.x1 - layer->buf_area.x1;
        draw_vy = t->clip_area.y1 - layer->buf_area.y1;
    }
    else {
        draw_vx = dsc->tile ? (dsc->image_area.x1 - layer->buf_area.x1) : img_x;
        draw_vy = dsc->tile ? (dsc->image_area.y1 - layer->buf_area.y1) : img_y;
    }

    /* Compute skew transform (must be done before saveContext so
     * degenerate matrix can return cleanly via the caller) */
    image_skew_t skew;
    if(has_skew) {
        if(!compute_image_skew(&skew, dsc->rotation, dsc->scale_x, dsc->scale_y,
                               dsc->skew_x, dsc->skew_y, dsc->pivot.x, dsc->pivot.y,
                               src_w, src_h, img_x, img_y, draw_vx, draw_vy))
            return false;
    }

    /* Set up source bitmap for stencil sampling */
    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
    EVE_CoDl_bitmapSource(phost, ram_g_addr);
    set_palette_if_needed(phost, eve_format, palette_addr);
    EVE_CoDl_bitmapLayout(phost, (uint8_t)eve_format, eve_stride, layout_h);
    uint8_t bmp_filter = dsc->antialias ? BILINEAR : NEAREST;
    if(dsc->tile) {
        int32_t tile_w = lv_area_get_width(&dsc->image_area);
        int32_t tile_h = lv_area_get_height(&dsc->image_area);
        EVE_CoDl_bitmapSize(phost, bmp_filter, REPEAT, REPEAT, tile_w, tile_h);
    }
    else {
        EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER, src_w, src_h);
    }
    if(!has_skew && !has_transform)
        EVE_CoDl_bitmapTransform_identity(phost);

    /* Apply bitmap transform */
    if(has_skew) {
        apply_image_skew(phost, &skew, bmp_filter,
            dsc->tile ? lv_area_get_width(&dsc->image_area) : 0,
            dsc->tile ? lv_area_get_height(&dsc->image_area) : 0);
    }
    else if(has_transform) {
        EVE_CoCmd_loadIdentity(phost);
        EVE_CoCmd_translate(phost, F16(img_x - draw_vx + dsc->pivot.x),
                            F16(img_y - draw_vy + dsc->pivot.y));
        if(dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE) {
            EVE_CoCmd_scale(phost, F16_SCALE_DIV_256(dsc->scale_x),
                            F16_SCALE_DIV_256(dsc->scale_y));
        }
        if(dsc->rotation != 0) {
            EVE_CoCmd_rotate(phost, DEGREES(dsc->rotation));
        }
        EVE_CoCmd_translate(phost, -F16(dsc->pivot.x), -F16(dsc->pivot.y));
        EVE_CoCmd_setMatrix(phost);
        EVE_CoCmd_loadIdentity(phost);
        EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER,
            LV_MIN(lv_area_get_width(&t->clip_area), 2048),
            LV_MIN(lv_area_get_height(&t->clip_area), 2048));
    }

    /* Build 6-pass colorkey stencil */
    EVE_CoDl_clear(phost, 0, 1, 0);
    build_colorkey_stencil(phost, dsc->colorkey, eve_format, eve_stride, layout_h, draw_vx, draw_vy);

    /* Restore alpha pass drawing state with stencil gate.
     * Subsequent draws are gated by NOTEQUAL 6 — colorkey pixels excluded.
     * BT820: build_colorkey_stencil restores original layout on return,
     * no swizzle restore needed (swizzle only active in GLFORMAT mode). */
    EVE_CoDl_alphaFunc(phost, ALWAYS, 0);
    EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
    EVE_CoDl_blendFunc(phost, ONE, ONE_MINUS_SRC_ALPHA);
    EVE_CoDl_stencilFunc(phost, NOTEQUAL, 6, 0xFF);
    EVE_CoDl_stencilOp(phost, KEEP, KEEP);

    return true;
}

/**
 * Alpha correction pass for IMAGE and LAYER tasks.
 *
 * Re-draws the bitmap with correct transforms, colorkey, and tiling
 * into the alpha channel only. The outer context already has
 * colorMask(0,0,0,1) and blend(ONE, ONE_MINUS_SRC_ALPHA) set.
 *
 * For clip_radius or bitmap_mask_src, draws the clip shape at opa
 * (exact for opaque formats, approximate for ARGB with per-pixel alpha).
 * For the standard path, draws the actual bitmap — exact for all formats.
 */
void lv_draw_eve5_hal_alpha_draw_image(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    EVE_HalContext *phost = u->hal;
    lv_layer_t *layer = t->target_layer;
    const lv_draw_image_dsc_t *dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;

    uint32_t ram_g_addr;
    uint16_t eve_format;
    int32_t eve_stride;
    int32_t src_w, src_h;
    int32_t layout_h;
    uint32_t palette_addr = GA_INVALID;

    /* Resolve bitmap source (same as normal draw) */
    if(t->type == LV_DRAW_TASK_TYPE_LAYER) {
        lv_layer_t *child_layer = (lv_layer_t *)dsc->src;
        Esd_GpuHandle child_handle = Esd_GpuHandle_FromPtrType(child_layer->user_data);
        ram_g_addr = Esd_GpuAlloc_Get(u->allocator, child_handle);
        if(ram_g_addr == GA_INVALID) return;
        src_w = lv_area_get_width(&child_layer->buf_area);
        src_h = lv_area_get_height(&child_layer->buf_area);

        /* Check canvas cache for actual format (canvas direct image may use RGB565).
         * Validate address match to avoid false hits from draw_buf->data reuse. */
        uint16_t cached_format;
        uint32_t cached_stride;
        uint32_t cached_palette_addr;
        uint32_t cached_addr = GA_INVALID;
        if(child_layer->draw_buf && child_layer->draw_buf->data) {
            cached_addr = lv_draw_eve5_canvas_cache_lookup(u, child_layer->draw_buf->data,
                                                            NULL, NULL, NULL, &cached_format, &cached_stride,
                                                            &cached_palette_addr);
        }
        if(cached_addr != GA_INVALID) {
            ram_g_addr = cached_addr;
            eve_format = cached_format;
            eve_stride = (int32_t)cached_stride;
            palette_addr = cached_palette_addr;
            layout_h = src_h;
        }
        else {
            /* Standard render target - format derived from layer's actual format.
             * Canvas layers (draw_buf != NULL, parent == NULL): use draw_buf->header.cf.
             * Child layers (parent != NULL): use color_format set by LVGL. */
            uint8_t bpp;
            bool is_child_canvas = (child_layer->draw_buf != NULL && child_layer->parent == NULL);
            lv_color_format_t layer_cf;
            if(is_child_canvas) {
                layer_cf = child_layer->draw_buf->header.cf;
            }
            else {
                layer_cf = child_layer->color_format;
            }
            if(layer_cf == LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED) {
                layer_cf = LV_COLOR_FORMAT_ARGB8888;
            }
            /* Function sets appropriate fallback even if unsupported */
            lv_draw_eve5_get_render_target_format(layer_cf, &eve_format, &bpp);
#if LV_DRAW_EVE5_OPAQUE_LAYER_RGB8
            /* Force RGB8 for opaque layers (must match hal_init_layer) */
            if(!lv_color_format_has_alpha(layer_cf) && eve_format != ARGB8) {
                eve_format = RGB8;
                bpp = 3;
            }
#endif
            eve_stride = ALIGN_UP(src_w, 16) * bpp;
            layout_h = ALIGN_UP(src_h, 16);
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

    int32_t x = t->area.x1 - layer->buf_area.x1;
    int32_t y = t->area.y1 - layer->buf_area.y1;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    /* Masking path: clip_radius or bitmap_mask — draw clip shape at opa.
     * Exact for opaque formats, approximate for ARGB with per-pixel alpha. */
    if(dsc->clip_radius > 0 || dsc->bitmap_mask_src != NULL) {
#if EVE5_ALPHA_STENCIL_APPROX
        /* Stencil-based clip: build clip shape in stencil, then draw through it.
         * Handles two cases:
         *   clip + mask: draw mask bitmap through stencil (exact for opaque, approx for ARGB)
         *   clip + ARGB (no mask): draw source bitmap through stencil (per-pixel alpha)
         * Colorkey excluded — its own 6-pass stencil would conflict. */
        if(dsc->clip_radius > 0 && dsc->colorkey == NULL) {
            /* Try to load mask bitmap if present */
            uint32_t mask_bmp_addr = GA_INVALID;
            int32_t mask_bmp_stride = 0;
            int32_t mask_bmp_w = 0, mask_bmp_h = 0;
            uint16_t mask_bmp_format = L8;
            uint32_t mask_bmp_palette_addr = GA_INVALID;
            if(dsc->bitmap_mask_src != NULL) {
                eve5_gpu_image_t mask_img;
                if(lv_draw_eve5_resolve_to_gpu(u, dsc->bitmap_mask_src, &mask_img)) {
                    eve5_gpu_image_resolve(u->allocator, &mask_img, &mask_bmp_addr, &mask_bmp_palette_addr);
                    if(mask_bmp_addr != GA_INVALID) {
                        mask_bmp_format = mask_img.eve_format;
                        mask_bmp_stride = mask_img.eve_stride;
                        mask_bmp_w = mask_img.width;
                        mask_bmp_h = mask_img.height;
                    }
                }
            }

            if(mask_bmp_addr != GA_INVALID || eve_format == ARGB8) {
                int32_t clip_x1 = dsc->image_area.x1 - layer->buf_area.x1;
                int32_t clip_y1 = dsc->image_area.y1 - layer->buf_area.y1;
                int32_t clip_x2 = dsc->image_area.x2 - layer->buf_area.x1;
                int32_t clip_y2 = dsc->image_area.y2 - layer->buf_area.y1;
                int32_t clip_w = clip_x2 - clip_x1 + 1;
                int32_t clip_h = clip_y2 - clip_y1 + 1;
                int32_t real_radius = LV_MIN3(clip_w / 2, clip_h / 2, (int32_t)dsc->clip_radius);

                lv_draw_eve5_clear_stencil(u, clip_x1, clip_y1, clip_x2, clip_y2,
                                            &t->clip_area, &layer->buf_area);

                /* Bitmap transform may be non-identity from a previous draw —
                 * bitmap draws inside this scope need identity, so set before save. */
                EVE_CoDl_bitmapTransform_identity(phost);
                EVE_CoDl_vertexFormat(phost, 0);
                EVE_CoDl_saveContext(phost);

                /* Phase 1: Draw clip shape into stencil (no color output) */
                EVE_CoDl_colorMask(phost, 0, 0, 0, 0);
                EVE_CoDl_stencilOp(phost, KEEP, INCR);
                if(clip_w == clip_h && real_radius >= clip_w / 2) {
                    EVE_CoDl_pointSize(phost, clip_w * 8 - 8);
                    EVE_CoDl_begin(phost, POINTS);
                    EVE_CoDl_vertex2f_1(phost, clip_x1 * 2 + (clip_w - 1), clip_y1 * 2 + (clip_h - 1));
                    EVE_CoDl_end(phost);
                }
                else {
                    EVE_CoDl_lineWidth(phost, real_radius * 16);
                    EVE_CoDl_begin(phost, RECTS);
                    EVE_CoDl_vertex2f_0(phost, clip_x1 + real_radius, clip_y1 + real_radius);
                    EVE_CoDl_vertex2f_0(phost, clip_x2 - real_radius, clip_y2 - real_radius);
                    EVE_CoDl_end(phost);
                }

                /* Phase 2: Draw through stencil (alpha-only) */
                EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
                EVE_CoDl_stencilFunc(phost, NOTEQUAL, 0, 0xFF);
                EVE_CoDl_stencilOp(phost, KEEP, KEEP);
                EVE_CoDl_colorA(phost, dsc->opa);

                if(mask_bmp_addr != GA_INVALID) {
                    /* Draw mask bitmap through clip stencil.
                     * Exact for opaque formats; for ARGB, preserves mask + clip shape
                     * but loses source bitmap per-pixel alpha. */
                    int32_t img_w = lv_area_get_width(&dsc->image_area);
                    int32_t img_h = lv_area_get_height(&dsc->image_area);
                    int32_t mask_draw_x = clip_x1 + (img_w - mask_bmp_w) / 2;
                    int32_t mask_draw_y = clip_y1 + (img_h - mask_bmp_h) / 2;

                    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
                    EVE_CoDl_bitmapSource(phost, mask_bmp_addr);
                    set_palette_if_needed(phost, mask_bmp_format, mask_bmp_palette_addr);
                    /* For ARGB8 or PALETTEDARGB8: use GLFORMAT + swizzle to extract RED channel as alpha
                     * (grayscale PNGs decode as ARGB8/PALETTEDARGB8 with R=G=B=gray, A=255).
                     * For L8/A8: BT820 natively decodes as (255,255,255,L) — no swizzle needed. */
                    if(mask_bmp_format == ARGB8 || mask_bmp_format == PALETTEDARGB8) {
                        EVE_CoDl_bitmapLayout(phost, GLFORMAT, mask_bmp_stride, mask_bmp_h);
                        EVE_CoDl_bitmapExtFormat(phost, mask_bmp_format);
                        EVE_CoDl_bitmapSwizzle(phost, ZERO, ZERO, ZERO, RED);
                    }
                    else {
                        EVE_CoDl_bitmapLayout(phost, (uint8_t)mask_bmp_format, mask_bmp_stride, mask_bmp_h);
                    }
                    EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, mask_bmp_w, mask_bmp_h);
                    EVE_CoDl_begin(phost, BITMAPS);
                    EVE_CoDl_vertex2f_0(phost, mask_draw_x, mask_draw_y);
                    EVE_CoDl_end(phost);
                }
                else {
                    /* Draw ARGB source bitmap through clip stencil.
                     * Reproduces per-pixel alpha inside the clip region. */
                    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
                    EVE_CoDl_bitmapSource(phost, ram_g_addr);
                    set_palette_if_needed(phost, eve_format, palette_addr);
                    EVE_CoDl_bitmapLayout(phost, (uint8_t)eve_format, eve_stride, layout_h);
                    uint8_t bmp_filter = dsc->antialias ? BILINEAR : NEAREST;
                    if(dsc->tile) {
                        int32_t tile_w = lv_area_get_width(&dsc->image_area);
                        int32_t tile_h = lv_area_get_height(&dsc->image_area);
                        EVE_CoDl_bitmapSize(phost, bmp_filter, REPEAT, REPEAT, tile_w, tile_h);
                    }
                    else {
                        EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER, src_w, src_h);
                    }
                    /* BT820: BITMAP_SWIZZLE only applies in GLFORMAT mode — no-op here. */
                    /* if(is_alpha_only)
                        EVE_CoDl_bitmapSwizzle(phost, ONE, ONE, ONE, ALPHA);
                    else
                        EVE_CoDl_bitmapSwizzle(phost, RED, GREEN, BLUE, ALPHA); */

                    /* Compute draw vertex and apply transforms (same as standard path) */
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

                    if(has_skew) {
                        image_skew_t skew;
                        if(!compute_image_skew(&skew, dsc->rotation, dsc->scale_x, dsc->scale_y,
                                               dsc->skew_x, dsc->skew_y, dsc->pivot.x, dsc->pivot.y,
                                               src_w, src_h, x, y, draw_vx, draw_vy)) {
                            EVE_CoDl_restoreContext(phost);
                            return;
                        }
                        apply_image_skew(phost, &skew, bmp_filter,
                            dsc->tile ? lv_area_get_width(&dsc->image_area) : 0,
                            dsc->tile ? lv_area_get_height(&dsc->image_area) : 0);
                    }
                    else if(has_transform) {
                        EVE_CoCmd_loadIdentity(phost);
                        EVE_CoCmd_translate(phost, F16(x - draw_vx + dsc->pivot.x),
                                            F16(y - draw_vy + dsc->pivot.y));
                        if(dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE) {
                            EVE_CoCmd_scale(phost, F16_SCALE_DIV_256(dsc->scale_x),
                                            F16_SCALE_DIV_256(dsc->scale_y));
                        }
                        if(dsc->rotation != 0) {
                            EVE_CoCmd_rotate(phost, DEGREES(dsc->rotation));
                        }
                        EVE_CoCmd_translate(phost, -F16(dsc->pivot.x), -F16(dsc->pivot.y));
                        EVE_CoCmd_setMatrix(phost);
                        EVE_CoCmd_loadIdentity(phost);
                        /* Expand bitmapSize to cover rotated/scaled output */
                        EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER,
                            LV_MIN(lv_area_get_width(&t->clip_area), 2048),
                            LV_MIN(lv_area_get_height(&t->clip_area), 2048));
                    }

                    EVE_CoDl_begin(phost, BITMAPS);
                    EVE_CoDl_vertex2f_0(phost, draw_vx, draw_vy);
                    EVE_CoDl_end(phost);
                }

                EVE_CoDl_restoreContext(phost);
                return;
            }
        }
#endif
        /* Colorkey gate: when colorkey is set, build the 6-pass stencil on
         * the source image so subsequent shape draws are gated by
         * stencilFunc(NOTEQUAL, 6) — colorkey pixels receive no alpha,
         * preserving previously accumulated alpha from earlier tasks. */
        bool has_colorkey_gate = false;
        if(dsc->colorkey != NULL) {
            EVE_CoDl_vertexFormat(phost, 0);
            EVE_CoDl_saveContext(phost);
            has_colorkey_gate = alpha_pass_build_colorkey_gate(
                u, t, dsc, ram_g_addr, eve_format, eve_stride,
                layout_h, src_w, src_h, x, y, palette_addr);
            if(!has_colorkey_gate) {
                EVE_CoDl_restoreContext(phost);
                return;
            }
        }

        if(dsc->clip_radius > 0 && dsc->bitmap_mask_src == NULL) {
            /* clip_radius only — draw rounded rect at opa (exact for all formats).
             * When colorkey gate is active, colorkey pixels are excluded. */
            int32_t mask_x1 = dsc->image_area.x1 - layer->buf_area.x1;
            int32_t mask_y1 = dsc->image_area.y1 - layer->buf_area.y1;
            int32_t mask_x2 = dsc->image_area.x2 - layer->buf_area.x1;
            int32_t mask_y2 = dsc->image_area.y2 - layer->buf_area.y1;
            EVE_CoDl_colorA(phost, dsc->opa);
            lv_draw_eve5_draw_rect(u, mask_x1, mask_y1, mask_x2, mask_y2,
                                    dsc->clip_radius, &t->clip_area, &layer->buf_area);
        }
        else {
            /* bitmap_mask (with or without clip_radius) — draw mask bitmap at opa.
             * For opaque formats without clip_radius: exact.
             * For opaque + clip_radius (stencil unavailable): loses clip rounding.
             * For ARGB: mask shape preserved, per-pixel bitmap alpha lost.
             * When colorkey gate is active, colorkey pixels are excluded. */
            if(dsc->bitmap_mask_src != NULL) {
                uint16_t mask_eve_format = L8;
                int32_t mask_eve_stride = 0;
                int32_t mask_w = 0, mask_h = 0;
                uint32_t mask_palette_addr = GA_INVALID;
                uint32_t mask_addr = GA_INVALID;

                {
                    eve5_gpu_image_t mask_img;
                    if(lv_draw_eve5_resolve_to_gpu(u, dsc->bitmap_mask_src, &mask_img)) {
                        eve5_gpu_image_resolve(u->allocator, &mask_img, &mask_addr, &mask_palette_addr);
                        if(mask_addr != GA_INVALID) {
                            mask_eve_format = mask_img.eve_format;
                            mask_eve_stride = mask_img.eve_stride;
                            mask_w = mask_img.width;
                            mask_h = mask_img.height;
                        }
                    }
                }
                if(mask_addr != GA_INVALID) {
                    /* Center-align mask on image_area (same as normal draw) */
                    int32_t img_w = lv_area_get_width(&dsc->image_area);
                    int32_t img_h = lv_area_get_height(&dsc->image_area);
                    int32_t mask_x1 = dsc->image_area.x1 - layer->buf_area.x1;
                    int32_t mask_y1 = dsc->image_area.y1 - layer->buf_area.y1;
                    int32_t mask_draw_x = mask_x1 + (img_w - mask_w) / 2;
                    int32_t mask_draw_y = mask_y1 + (img_h - mask_h) / 2;

                    /* Bitmap transform may be non-identity from a previous draw —
                     * mask bitmap draws at identity. */
                    EVE_CoDl_bitmapTransform_identity(phost);
                    EVE_CoDl_vertexFormat(phost, 0);
                    EVE_CoDl_saveContext(phost);
                    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
                    EVE_CoDl_bitmapSource(phost, mask_addr);
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
                    EVE_CoDl_colorA(phost, dsc->opa);
                    EVE_CoDl_begin(phost, BITMAPS);
                    EVE_CoDl_vertex2f_0(phost, mask_draw_x, mask_draw_y);
                    EVE_CoDl_end(phost);
                    EVE_CoDl_restoreContext(phost);
                }
            }
        }

        if(has_colorkey_gate) {
            EVE_CoDl_restoreContext(phost);
        }
        return;
    }

    /* Standard path: re-draw bitmap with transforms for exact per-pixel alpha.
     * Mirrors the normal draw's standard path but skips color/blend setup
     * since the outer context handles colorMask and blendFunc. */
    bool has_colorkey = (dsc->colorkey != NULL);
    bool has_transform = (dsc->rotation != 0 || dsc->scale_x != LV_SCALE_NONE
                         || dsc->scale_y != LV_SCALE_NONE
                         || dsc->skew_x != 0 || dsc->skew_y != 0);
    bool has_skew = (dsc->skew_x != 0 || dsc->skew_y != 0);

    /* Set up bitmap (same as normal draw) */
    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
    EVE_CoDl_bitmapSource(phost, ram_g_addr);
    set_palette_if_needed(phost, eve_format, palette_addr);
    EVE_CoDl_bitmapLayout(phost, (uint8_t)eve_format, eve_stride, layout_h);
    uint8_t bmp_filter = dsc->antialias ? BILINEAR : NEAREST;
    if(dsc->tile) {
        int32_t tile_w = lv_area_get_width(&dsc->image_area);
        int32_t tile_h = lv_area_get_height(&dsc->image_area);
        EVE_CoDl_bitmapSize(phost, bmp_filter, REPEAT, REPEAT, tile_w, tile_h);
    }
    else {
        EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER, src_w, src_h);
    }
    /* BT820: BITMAP_SWIZZLE only applies in GLFORMAT mode — no-op here. */
    /* if(is_alpha_only)
        EVE_CoDl_bitmapSwizzle(phost, ONE, ONE, ONE, ALPHA);
    else
        EVE_CoDl_bitmapSwizzle(phost, RED, GREEN, BLUE, ALPHA); */

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

    /* Compute skew transform (before saveContext so degenerate can return) */
    image_skew_t skew;
    if(has_skew) {
        if(!compute_image_skew(&skew, dsc->rotation, dsc->scale_x, dsc->scale_y,
                               dsc->skew_x, dsc->skew_y, dsc->pivot.x, dsc->pivot.y,
                               src_w, src_h, x, y, draw_vx, draw_vy))
            return;
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
        EVE_CoCmd_loadIdentity(phost);
        EVE_CoCmd_translate(phost, F16(x - draw_vx + dsc->pivot.x), F16(y - draw_vy + dsc->pivot.y));
        if(dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE) {
            EVE_CoCmd_scale(phost, F16_SCALE_DIV_256(dsc->scale_x), F16_SCALE_DIV_256(dsc->scale_y));
        }
        if(dsc->rotation != 0) {
            EVE_CoCmd_rotate(phost, DEGREES(dsc->rotation));
        }
        EVE_CoCmd_translate(phost, -F16(dsc->pivot.x), -F16(dsc->pivot.y));
        EVE_CoCmd_setMatrix(phost);
        EVE_CoCmd_loadIdentity(phost);
        /* Expand bitmapSize to cover rotated/scaled output —
         * source dimensions would clip the transformed bounding box. */
        EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER,
            LV_MIN(lv_area_get_width(&t->clip_area), 2048),
            LV_MIN(lv_area_get_height(&t->clip_area), 2048));
    }

    /* Build colorkey stencil mask */
    if(has_colorkey) {
        /* clearStencil defaults to 0 and nothing changes it */
        EVE_CoDl_clear(phost, 0, 1, 0);
        build_colorkey_stencil(phost, dsc->colorkey, eve_format, eve_stride, layout_h, draw_vx, draw_vy);
        /* Restore drawing state after stencil passes.
         * BT820: build_colorkey_stencil restores original layout on return,
         * no swizzle restore needed (swizzle only active in GLFORMAT mode). */
        EVE_CoDl_alphaFunc(phost, ALWAYS, 0);
        EVE_CoDl_colorMask(phost, 0, 0, 0, 1);  /* Restore alpha-only from outer context */
        EVE_CoDl_stencilFunc(phost, NOTEQUAL, 6, 0xFF);
        EVE_CoDl_stencilOp(phost, KEEP, KEEP);
    }

    /* Set alpha — the only "color" setup needed for the alpha pass */
    EVE_CoDl_colorA(phost, dsc->opa);

    /* Draw bitmap */
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, draw_vx, draw_vy);
    EVE_CoDl_end(phost);

    /* Restore state */
    if(has_skew || has_colorkey || has_transform) {
        EVE_CoDl_restoreContext(phost);
    }
}

#endif /* LV_USE_DRAW_EVE5 */
