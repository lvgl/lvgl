/**
 * @file lv_draw_eve5_hal.c
 *
 * EVE5 (BT820) Hardware Abstraction Layer for LVGL Draw Unit
 *
 * Ported from BT81x EVE implementation with adaptations for:
 * - Render-to-texture architecture
 * - EVE5 coprocessor command interface
 * - Esd_GpuAlloc memory management
 *
 * This file contains:
 * - Layer initialization and finalization
 * - Software fallback buffer compositing
 * - Texture upload/draw utilities
 * - Mask rectangle drawing
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../lv_draw.h"
#include "../lv_draw_image.h"
#include "../lv_draw_mask_private.h"

/**********************
 * HELPER FUNCTIONS
 **********************/

/**
 * Set scissor region for clipping (used by all drawing functions)
 * Non-static so primitives.c can use it
 */
void lv_draw_eve5_set_scissor(lv_draw_eve5_unit_t *u, const lv_area_t *clip, const lv_area_t *layer_area)
{
    int32_t x = clip->x1 - layer_area->x1;
    int32_t y = clip->y1 - layer_area->y1;
    int32_t w = lv_area_get_width(clip);
    int32_t h = lv_area_get_height(clip);

    if(x < 0) { w += x; x = 0; }
    if(y < 0) { h += y; y = 0; }
    if(w <= 0 || h <= 0) { w = 0; h = 0; }

    EVE_CoDl_scissorXY(u->hal, x, y);
    EVE_CoDl_scissorSize(u->hal, w, h);
}

void lv_draw_eve5_clear_stencil(lv_draw_eve5_unit_t *u,
                                 int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                                 const lv_area_t *clip, const lv_area_t *layer_area)
{
    lv_area_t bbox = { x1 + layer_area->x1, y1 + layer_area->y1,
                       x2 + layer_area->x1, y2 + layer_area->y1 };
    lv_area_t clear_area;
    if(!lv_area_intersect(&clear_area, &bbox, clip)) return;
    lv_draw_eve5_set_scissor(u, &clear_area, layer_area);
    /* clearStencil defaults to 0 and nothing changes it */
    EVE_CoDl_clear(u->hal, 0, 1, 0);
    lv_draw_eve5_set_scissor(u, clip, layer_area);
}

Esd_GpuHandle track_frame_alloc(lv_draw_eve5_unit_t *u, Esd_GpuHandle handle)
{
    if(handle.Id == GA_HANDLE_INVALID.Id) {
        return handle;
    }

    if(u->frame_alloc_count >= EVE5_MAX_FRAME_ALLOCS) {
        LV_LOG_WARN("EVE5: Frame allocation tracking overflow");
        return handle;
    }

    u->frame_allocs[u->frame_alloc_count++] = handle;
    return handle;
}

/**
 * Map LVGL color format to EVE render target format and bytes per pixel.
 * Returns true if the format is supported for direct rendering.
 *
 * CMD_RENDERTARGET supports: L8, LA8, RGB565, RGB6, RGB8, ARGB4, ARGB1555, ARGB6, ARGB8, YCBCR
 */
bool lv_draw_eve5_get_render_target_format(lv_color_format_t lv_cf, uint16_t *eve_fmt, uint8_t *bpp)
{
    switch(lv_cf) {
        /* Non-alpha formats */
        case LV_COLOR_FORMAT_RGB565:
            *eve_fmt = RGB565;
            *bpp = 2;
            return true;

        case LV_COLOR_FORMAT_RGB888:
            *eve_fmt = RGB8;
            *bpp = 3;
            return true;

        case LV_COLOR_FORMAT_XRGB8888:
            /* XRGB has ignored alpha - render to RGB8 to save memory */
            *eve_fmt = RGB8;
            *bpp = 3;
            return true;

        /* Alpha formats - all render to ARGB8 (premultiplied result) */
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
            *eve_fmt = ARGB8;
            *bpp = 4;
            return true;

        case LV_COLOR_FORMAT_ARGB1555:
            /* Could render to ARGB1555 but alpha precision is poor.
             * Use ARGB8 for proper alpha handling, then user sees ARGB8. */
            *eve_fmt = ARGB8;
            *bpp = 4;
            return true;

        case LV_COLOR_FORMAT_ARGB4444:
            /* Same - use ARGB8 for better alpha precision */
            *eve_fmt = ARGB8;
            *bpp = 4;
            return true;

        /* Luminance formats */
        case LV_COLOR_FORMAT_L8:
            *eve_fmt = L8;
            *bpp = 1;
            return true;

        /* Unsupported - fall back based on whether format has alpha */
        default:
            if(lv_color_format_has_alpha(lv_cf)) {
                *eve_fmt = ARGB8;
                *bpp = 4;
            }
            else {
                *eve_fmt = RGB565;
                *bpp = 2;
            }
            return false;
    }
}

/**********************
 * LAYER MANAGEMENT
 **********************/

void lv_draw_eve5_hal_init_layer(lv_draw_eve5_unit_t *u, lv_layer_t *layer,
                                  bool is_screen, bool is_canvas)
{
    Esd_GpuHandle handle = { 0 };
    uint32_t ram_g_addr = GA_INVALID;
    bool canvas_has_content = false;  /* True if canvas has existing GPU content to preserve */
    bool canvas_is_premultiplied = false;  /* True if existing canvas content is premultiplied */
    uint16_t canvas_format = ARGB8;  /* EVE format of existing canvas content */
    uint32_t canvas_stride = 0;  /* Stride of existing canvas content */
    bool canvas_needs_conversion = false;  /* True if existing content needs format upgrade */

    int32_t w = lv_area_get_width(&layer->buf_area);
    int32_t h = lv_area_get_height(&layer->buf_area);
    int32_t aligned_w = ALIGN_UP(w, 16);
    int32_t aligned_h = ALIGN_UP(h, 16);

    /* Determine the target render format based on layer type:
     * - Screen layer: ALWAYS ARGB8 (display driver expects HW_BITMAP_FORMAT=ARGB8)
     * - Canvas layers: use draw_buf->header.cf (what user requested)
     * - Child layers: use color_format (set by LVGL for compositing - typically ARGB8888)
     * Alpha formats → ARGB8 (need proper alpha for compositing).
     * Non-alpha formats → native format (RGB565, RGB8) for efficiency. */
    uint16_t target_eve_fmt = ARGB8;
    uint8_t target_bpp = 4;
    if(!is_screen) {
        lv_color_format_t target_lv_cf;
        if(is_canvas && layer->draw_buf) {
            target_lv_cf = layer->draw_buf->header.cf;
        }
        else {
            target_lv_cf = layer->color_format;
        }
        lv_draw_eve5_get_render_target_format(target_lv_cf, &target_eve_fmt, &target_bpp);

#if LV_DRAW_EVE5_OPAQUE_LAYER_RGB8
        /* Force RGB8 for opaque layers (non-alpha formats like RGB565) */
        if(!lv_color_format_has_alpha(target_lv_cf) && target_eve_fmt != ARGB8) {
            target_eve_fmt = RGB8;
            target_bpp = 3;
        }
#endif
    }
    /* Screen layer: keep default ARGB8/4bpp - display driver expects this */

    /* Canvas layers use the persistent canvas cache instead of layer->user_data.
     * This allows the GPU allocation to persist after the stack-allocated lv_layer_t
     * is destroyed, and to be reused when the draw_buf is displayed as an image.
     * However, user_data MUST also be set so parent LAYER tasks can find the texture
     * (the LAYER dispatch checks user_data != NULL to decide whether to composite). */
    if(is_canvas) {
        uint32_t cached_aw;
        bool is_new;
        ram_g_addr = lv_draw_eve5_canvas_cache_get_or_create(u, layer,
                                                               target_eve_fmt, target_bpp,
                                                               &cached_aw,
                                                               &canvas_format, &canvas_stride,
                                                               &is_new, &canvas_is_premultiplied);
        if(ram_g_addr != GA_INVALID) {
            aligned_w = cached_aw;
            canvas_has_content = !is_new;  /* Existing allocation = has content to preserve */

            /* Set user_data to the canvas cache handle so parent LAYER tasks can
             * find this texture. user_data is nulled after each LAYER composite,
             * so it must be re-set each time the canvas is rendered. */
            lv_draw_eve5_canvas_cache_t *cache = &u->canvas_cache;
            for(uint32_t i = 0; i < EVE5_CANVAS_CACHE_CAPACITY; i++) {
                lv_draw_eve5_canvas_cache_entry_t *e = &cache->entries[i];
                if(e->valid && e->data_ptr == layer->draw_buf->data) {
                    layer->user_data = Esd_GpuHandle_ToPtrType(e->gpu_handle);
                    break;
                }
            }

            /* If existing content format differs from target format, we need conversion.
             * E.g., RGB565 canvas drawn to with ARGB8 content, or vice versa. */
            if(canvas_has_content && canvas_format != target_eve_fmt) {
                canvas_needs_conversion = true;
                LV_LOG_INFO("EVE5: Canvas %p needs format conversion from %d to %d",
                            (void *)layer, canvas_format, target_eve_fmt);
            }

            LV_LOG_INFO("EVE5: Using canvas cache for layer %p at RAM_G 0x%08X (%s, %s, fmt=%d target=%d)",
                        (void *)layer, ram_g_addr, is_new ? "new" : "existing",
                        canvas_is_premultiplied ? "premul" : "straight", canvas_format, target_eve_fmt);
        }
    }
    else {
        /* Check if layer already has allocation */
        if(layer->user_data != NULL) {
            handle = Esd_GpuHandle_FromPtrType(layer->user_data);
            ram_g_addr = Esd_GpuAlloc_Get(u->allocator, handle);
        }
    }

    /* Allocate texture if needed (non-canvas path) */
    if(ram_g_addr == GA_INVALID && !is_canvas) {
        uint32_t size = aligned_w * aligned_h * target_bpp;
        handle = Esd_GpuAlloc_Alloc(u->allocator, size, GA_ALIGN_128);
        layer->user_data = Esd_GpuHandle_ToPtrType(handle);

        ram_g_addr = Esd_GpuAlloc_Get(u->allocator, handle);
        if(ram_g_addr == GA_INVALID) {
            LV_LOG_ERROR("EVE5: Failed to allocate layer texture (%"PRId32"x%"PRId32" fmt=%d)",
                         aligned_w, aligned_h, target_eve_fmt);
            layer->user_data = NULL;
            return;
        }

        LV_LOG_INFO("EVE5: Allocated layer %p at RAM_G 0x%08X (%"PRId32"x%"PRId32" fmt=%d)",
                    (void *)layer, ram_g_addr, aligned_w, aligned_h, target_eve_fmt);
    }

    if(ram_g_addr == GA_INVALID) {
        LV_LOG_ERROR("EVE5: Failed to get layer texture");
        return;
    }

    /* Tag non-screen alpha layers as premultiplied ARGB8888 — EVE5 always
     * renders to ARGB8 render targets with SRC_ALPHA blending, producing
     * premultiplied content. Any alpha format (ARGB8888, ARGB4444, ARGB1555,
     * etc.) becomes ARGB8 premultiplied in GPU memory.
     * Don't tag the screen layer — its color_format is used by refr_area
     * to size the partial CPU buffer. Don't tag non-alpha formats (RGB565,
     * etc.) — they get alpha=255 fill and don't need premultiplied handling. */
    if(lv_color_format_has_alpha(layer->color_format) &&
       layer->color_format != LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED) {
        layer->color_format = LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED;
    }

    /* Set render target with appropriate format */
    EVE_CoCmd_renderTarget(u->hal, ram_g_addr, target_eve_fmt, aligned_w, aligned_h);
    LV_LOG_INFO("EVE5: Set render target for layer %p at RAM_G 0x%08X (%"PRId32"x%"PRId32" fmt=%d)",
                (void *)layer, ram_g_addr, aligned_w, aligned_h, target_eve_fmt);

    /* Start display list */
    EVE_CoCmd_dlStart(u->hal);

    /* Set scissor to full aligned texture */
    EVE_CoDl_scissorXY(u->hal, 0, 0);
    EVE_CoDl_scissorSize(u->hal, aligned_w, aligned_h);

    /* Reset canvas original content tracking */
    u->canvas_orig_addr = GA_INVALID;
    u->canvas_orig_palette = GA_INVALID;

    if(canvas_has_content) {
        /* Canvas with existing content: allocate a new buffer and blit content.
         *
         * We MUST allocate a new buffer even for same-format ARGB8 content because:
         * 1. We can't clear-then-blit from the same buffer (clear destroys content)
         * 2. The alpha pass needs access to the original content to incorporate
         *    existing alpha as the "base layer" before compositing new tasks
         *
         * The original buffer is kept alive (deferred free) until after the alpha
         * pass completes, so it can be read during alpha compositing. */
        uint32_t src_addr = ram_g_addr;
        uint16_t src_format = canvas_format;
        uint32_t src_stride = canvas_stride;

        Esd_GpuHandle old_handle = GA_HANDLE_INVALID;
        bool have_old_handle = false;

        /* Allocate new buffer for rendering in target format */
        uint32_t new_size = aligned_w * aligned_h * target_bpp;
        Esd_GpuHandle new_handle = Esd_GpuAlloc_Alloc(u->allocator, new_size, GA_ALIGN_128);
        uint32_t new_addr = Esd_GpuAlloc_Get(u->allocator, new_handle);

        if(new_addr == GA_INVALID) {
            LV_LOG_ERROR("EVE5: Failed to allocate buffer for canvas incremental render");
            /* Fall back to clearing without preserving content */
            EVE_CoDl_clearColorRgb(u->hal, 0, 0, 0);
            EVE_CoDl_clearColorA(u->hal, 0);
            EVE_CoDl_clear(u->hal, 1, 1, 1);
            goto canvas_cleared;
        }

        /* Get the old handle, capture palette, and update cache to point to new buffer */
        uint32_t src_palette = GA_INVALID;
        lv_draw_eve5_canvas_cache_t *cache = &u->canvas_cache;
        for(uint32_t i = 0; i < EVE5_CANVAS_CACHE_CAPACITY; i++) {
            lv_draw_eve5_canvas_cache_entry_t *e = &cache->entries[i];
            if(e->valid && e->data_ptr == layer->draw_buf->data) {
                old_handle = e->gpu_handle;
                have_old_handle = true;
                /* Capture palette address BEFORE zeroing it */
                src_palette = e->palette_addr;
                /* Update cache entry to new buffer with target format */
                e->gpu_handle = new_handle;
                e->eve_format = target_eve_fmt;
                e->stride = aligned_w * target_bpp;
                e->palette_addr = GA_INVALID;
                e->source_offset = 0;
                /* Update user_data to the new handle */
                layer->user_data = Esd_GpuHandle_ToPtrType(new_handle);
                break;
            }
        }

        /* Store original content reference for alpha pass.
         * The alpha pass will blit this as the "base layer" before compositing
         * new tasks' alpha on top. */
        u->canvas_orig_addr = src_addr;
        u->canvas_orig_format = src_format;
        u->canvas_orig_stride = src_stride;
        u->canvas_orig_palette = src_palette;
        u->canvas_orig_w = w;
        u->canvas_orig_h = h;

        /* Update render target to the new buffer */
        ram_g_addr = new_addr;
        EVE_CoCmd_renderTarget(u->hal, ram_g_addr, target_eve_fmt, aligned_w, aligned_h);

        if(canvas_needs_conversion) {
            LV_LOG_INFO("EVE5: Converting canvas from format %d to %d", src_format, target_eve_fmt);
        }

        /* Clear the new buffer, then blit existing content from old buffer */
        EVE_CoDl_clearColorRgb(u->hal, 0, 0, 0);
        EVE_CoDl_clearColorA(u->hal, 0);
        EVE_CoDl_clear(u->hal, 1, 1, 1);

        /* Blit existing content. For format conversion from non-alpha formats (RGB565),
         * use blend(ONE, ZERO) since there's no alpha to premultiply.
         * For ARGB8 content, blend depends on whether it's premultiplied. */
        EVE_CoDl_saveContext(u->hal);
        if(canvas_needs_conversion || canvas_is_premultiplied) {
            EVE_CoDl_blendFunc(u->hal, ONE, ZERO);  /* Direct copy */
        }
        else {
            /* Blit with standard alpha blending to convert straight → premultiplied.
             * After this blit, the render target contains premultiplied content.
             * Mark the canvas as premultiplied so subsequent renders know. */
            EVE_CoDl_blendFunc_default(u->hal);  /* SRC_ALPHA, ONE_MINUS_SRC_ALPHA */
            if(layer->draw_buf && layer->draw_buf->data) {
                lv_draw_eve5_canvas_cache_set_premultiplied(u, layer->draw_buf->data, true);
            }
            LV_LOG_INFO("EVE5: Converting canvas to premultiplied alpha");
        }
        EVE_CoDl_colorArgb_ex(u->hal, 0xFFFFFFFF);
        EVE_CoCmd_setBitmap(u->hal, src_addr, (uint8_t)src_format, w, h);
        EVE_CoDl_bitmapLayout(u->hal, (uint8_t)src_format, src_stride, h);
        EVE_CoDl_bitmapSize(u->hal, NEAREST, BORDER, BORDER, w, h);
        /* Set palette source for paletted formats (PALETTEDARGB8, etc.) */
        if(src_palette != GA_INVALID) {
            EVE_CoDl_paletteSource(u->hal, src_palette);
        }
        EVE_CoDl_begin(u->hal, BITMAPS);
        EVE_CoDl_vertex2f_0(u->hal, 0, 0);
        EVE_CoDl_end(u->hal);
        EVE_CoDl_restoreContext(u->hal);

        /* Track old handle for deferred free AFTER the render completes.
         * The alpha pass still needs to read from it. */
        if(have_old_handle) {
            track_frame_alloc(u, old_handle);
        }

        LV_LOG_INFO("EVE5: Blitting existing canvas content (%"PRId32"x%"PRId32" fmt=%d) from 0x%08X to 0x%08X",
                    w, h, src_format, src_addr, ram_g_addr);
    }
    else {
        /* Fresh layer or canvas: clear to transparent black (or opaque for screen) */
        EVE_CoDl_clearColorRgb(u->hal, 0, 0, 0);
        if(is_screen) {
            /* EVE_CoDl_clearColorRgb(u->hal, 0, 128, 0); */ /* TEST COLOR */
            EVE_CoDl_clearColorA(u->hal, 255);
        } else {
            /* EVE_CoDl_clearColorRgb(u->hal, 0, 0, 128); */ /* TEST COLOR */
            EVE_CoDl_clearColorA(u->hal, 0);
        }
        EVE_CoDl_clear(u->hal, 1, 1, 1);
    }

canvas_cleared:
    /* Restrict scissor to actual content area */
    EVE_CoDl_scissorXY(u->hal, 0, 0);
    EVE_CoDl_scissorSize(u->hal, w, h);

    /* Set default state */
    EVE_CoDl_colorArgb_ex(u->hal, 0xFFFFFFFF);
    EVE_CoDl_blendFunc(u->hal, SRC_ALPHA, ONE_MINUS_SRC_ALPHA);

}

void lv_draw_eve5_hal_finish_layer(lv_draw_eve5_unit_t *u, lv_layer_t *layer,
                                    bool is_screen, bool is_canvas)
{
    /* Canvas layers rendered through the pipeline are now premultiplied.
     * Mark them as such so the image display path uses correct blending. */
    if(is_canvas && layer->draw_buf && layer->draw_buf->data) {
        lv_draw_eve5_canvas_cache_set_premultiplied(u, layer->draw_buf->data, true);
    }

    EVE_CoDl_display(u->hal);
    EVE_CoCmd_swap(u->hal);
    EVE_CoCmd_graphicsFinish(u->hal);

    /* Get sync marker for deferred free */
    EVE_CmdSync sync = EVE_Cmd_sync(u->hal);

    /* Queue all tracked allocations for deferred free.
     * Note: Canvas allocations are NOT in frame_allocs - they're persistent
     * and managed separately by the canvas cache. */
    for(uint16_t i = 0; i < u->frame_alloc_count; i++) {
        Esd_GpuAlloc_DeferredFree(u->allocator, u->frame_allocs[i], sync);
    }
    u->frame_alloc_count = 0;

    // EVE_Cmd_waitFlush(u->hal);
}

/**********************
 * L8 ALPHA RENDER TARGET
 **********************/

/**
 * Allocate an L8 texture and start a DL cycle for alpha rendering.
 * Returns the GpuHandle for the L8 allocation (GA_HANDLE_INVALID on failure).
 * Caller should dispatch tasks into the DL, then call hal_finish_l8_rendertarget.
 */
Esd_GpuHandle lv_draw_eve5_hal_init_l8_rendertarget(lv_draw_eve5_unit_t *u,
                                                      int32_t aligned_w, int32_t aligned_h,
                                                      int32_t w, int32_t h)
{
    EVE_HalContext *phost = u->hal;

    /* Allocate L8 texture: 1 byte per pixel */
    uint32_t l8_size = (uint32_t)aligned_w * aligned_h;
    Esd_GpuHandle l8_handle = Esd_GpuAlloc_Alloc(u->allocator, l8_size, GA_ALIGN_128);
    uint32_t l8_addr = Esd_GpuAlloc_Get(u->allocator, l8_handle);

    if(l8_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Failed to allocate L8 alpha render target (%"PRId32"x%"PRId32")",
                    aligned_w, aligned_h);
        return GA_HANDLE_INVALID;
    }

    LV_LOG_INFO("EVE5: L8 alpha RT allocated at 0x%08X (%"PRId32"x%"PRId32")",
                l8_addr, aligned_w, aligned_h);

    /* Start L8 DL cycle */
    EVE_CoCmd_renderTarget(phost, l8_addr, L8, aligned_w, aligned_h);
    EVE_CoCmd_dlStart(phost);

    /* Clear full aligned area to black (alpha=0) */
    EVE_CoDl_scissorXY(phost, 0, 0);
    EVE_CoDl_scissorSize(phost, aligned_w, aligned_h);
    EVE_CoDl_clearColorRgb(phost, 0, 0, 0);
    EVE_CoDl_clear(phost, 1, 1, 1);

    /* Restrict scissor to content area */
    EVE_CoDl_scissorXY(phost, 0, 0);
    EVE_CoDl_scissorSize(phost, w, h);

    /* If canvas has existing content, blit its alpha as the base luminance.
     * L8 render target writes the R channel as luminance. Use BITMAP_SWIZZLE
     * to route the source bitmap's alpha channel to R, so existing alpha
     * becomes the starting L8 value before new tasks are composited on top. */
    if(u->canvas_orig_addr != GA_INVALID) {
        EVE_CoDl_saveContext(phost);
        EVE_CoDl_blendFunc(phost, ONE, ZERO);  /* Overwrite (just cleared to 0) */
        EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
        EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
        EVE_CoDl_bitmapSource(phost, u->canvas_orig_addr);
        EVE_CoDl_bitmapLayout(phost, (uint8_t)u->canvas_orig_format, u->canvas_orig_stride, u->canvas_orig_h);
        EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, u->canvas_orig_w, u->canvas_orig_h);
        /* Set palette source for paletted formats */
        if(u->canvas_orig_palette != GA_INVALID) {
            EVE_CoDl_paletteSource(phost, u->canvas_orig_palette);
        }
        /* Swizzle alpha to RGB so the L8 render target captures it as luminance.
         * L8 target writes the R channel; swizzle A→R makes L = src.A.
         * For non-alpha formats (RGB565), hardware returns A=255, giving L=255. */
        EVE_CoDl_bitmapSwizzle(phost, ALPHA, ALPHA, ALPHA, ONE);
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, 0, 0);
        EVE_CoDl_end(phost);
        EVE_CoDl_restoreContext(phost);
    }

    /* Default state */
    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
    EVE_CoDl_blendFunc(phost, SRC_ALPHA, ONE_MINUS_SRC_ALPHA);

    return l8_handle;
}

/**
 * Finish the L8 DL cycle and flush frame allocations.
 */
void lv_draw_eve5_hal_finish_l8_rendertarget(lv_draw_eve5_unit_t *u)
{
    EVE_CoDl_display(u->hal);
    EVE_CoCmd_swap(u->hal);
    EVE_CoCmd_graphicsFinish(u->hal);

    /* Flush frame allocs from L8 pass (synced to L8 graphicsFinish) */
    EVE_CmdSync l8_sync = EVE_Cmd_sync(u->hal);
    for(uint16_t i = 0; i < u->frame_alloc_count; i++) {
        Esd_GpuAlloc_DeferredFree(u->allocator, u->frame_allocs[i], l8_sync);
    }
    u->frame_alloc_count = 0;
}

/**
 * Blit L8 luminance into the current ARGB8 layer's alpha channel.
 *
 * BT820 L-format natively decodes as (R=255, G=255, B=255, A=L) — luminance
 * is already in the alpha channel.  colorMask(0,0,0,1) writes only alpha.
 * blend(ONE, ZERO) overwrites.  Result: dst.a = L8_luminance.
 */
void lv_draw_eve5_hal_blit_l8_to_alpha(lv_draw_eve5_unit_t *u, uint32_t l8_addr,
                                        int32_t aligned_w, int32_t aligned_h,
                                        int32_t w, int32_t h)
{
    EVE_HalContext *phost = u->hal;

    EVE_CoDl_scissorXY(phost, 0, 0);
    EVE_CoDl_scissorSize(phost, w, h);
    EVE_CoDl_bitmapTransform_identity(phost);
    EVE_CoDl_vertexFormat(phost, 0);
    EVE_CoDl_saveContext(phost);

    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
    EVE_CoDl_bitmapSource(phost, l8_addr);
    EVE_CoDl_bitmapLayout(phost, L8, aligned_w, aligned_h);
    EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, w, h);
    /* BT820 L8 natively decodes as (255,255,255,L) — alpha=L, no swizzle needed.
     * BITMAP_SWIZZLE only applies in GLFORMAT mode on BT820. */
    /* EVE_CoDl_bitmapSwizzle(phost, ZERO, ZERO, ZERO, ALPHA); */

    EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
    EVE_CoDl_blendFunc(phost, ONE, ZERO);
    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);

    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, 0, 0);
    EVE_CoDl_end(phost);

    EVE_CoDl_restoreContext(phost);
}

/**********************
 * TEXTURE DRAWING (for cached SW content)
 **********************/

Esd_GpuHandle lv_draw_eve5_hal_upload_texture(lv_draw_eve5_unit_t *u,
                                               const uint8_t *buf_data,
                                               int32_t buf_w, int32_t buf_h,
                                               uint32_t *out_stride)
{
    uint32_t eve_stride = ALIGN_UP(buf_w * 4, 4);
    uint32_t eve_size = eve_stride * buf_h;

    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(u->allocator, eve_size, GA_ALIGN_4);
    uint32_t ram_g_addr = Esd_GpuAlloc_Get(u->allocator, handle);

    if(ram_g_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Failed to allocate texture (%"PRIu32" bytes)", eve_size);
        return GA_HANDLE_INVALID;
    }

    uint32_t src_stride = buf_w * 4;
    if(eve_stride == src_stride) {
        EVE_Hal_wrMem(u->hal, ram_g_addr, buf_data, eve_size);
    }
    else {
        for(int32_t y = 0; y < buf_h; y++) {
            EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride,
                          buf_data + y * src_stride, src_stride);
        }
    }

    *out_stride = eve_stride;

    LV_LOG_TRACE("EVE5: Uploaded texture %"PRId32"x%"PRId32" to 0x%08X",
                 buf_w, buf_h, ram_g_addr);

    return handle;
}

void lv_draw_eve5_hal_draw_texture(lv_draw_eve5_unit_t *u,
                                    const lv_draw_task_t *t,
                                    uint32_t ram_g_addr,
                                    int32_t tex_w, int32_t tex_h,
                                    uint32_t eve_stride,
                                    const lv_area_t *draw_area)
{
    lv_layer_t *layer = t->target_layer;

    if(ram_g_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Invalid RAM_G address for texture draw");
        return;
    }

    int32_t x = draw_area->x1 - layer->buf_area.x1;
    int32_t y = draw_area->y1 - layer->buf_area.y1;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorArgb_ex(u->hal, 0xFFFFFFFF);
    /* Bitmap transform may be non-identity from a previous draw */
    EVE_CoDl_bitmapTransform_identity(u->hal);

    EVE_CoDl_bitmapHandle(u->hal, u->hal->CoScratchHandle);
    EVE_CoDl_bitmapSource(u->hal, ram_g_addr);
    EVE_CoDl_bitmapLayout(u->hal, ARGB8, eve_stride, tex_h);
    EVE_CoDl_bitmapSize(u->hal, NEAREST, BORDER, BORDER, tex_w, tex_h);
    /* BT820: BITMAP_SWIZZLE only applies in GLFORMAT mode — identity swizzle is a no-op. */
    /* EVE_CoDl_bitmapSwizzle(u->hal, RED, GREEN, BLUE, ALPHA); */

    EVE_CoDl_begin(u->hal, BITMAPS);
    EVE_CoDl_vertex2f_0(u->hal, x, y);
    EVE_CoDl_end(u->hal);
}

/**********************
 * UTILITIES
 **********************/

bool lv_draw_eve5_hal_check_texture(lv_draw_eve5_unit_t *u, Esd_GpuHandle handle)
{
    return (Esd_GpuAlloc_Get(u->allocator, handle) != GA_INVALID);
}

/**********************
 * MASK RECTANGLE
 **********************/

void lv_draw_eve5_hal_draw_mask_rect(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    EVE_HalContext *phost = u->hal;
    lv_layer_t *layer = t->target_layer;
    const lv_draw_mask_rect_dsc_t *dsc = t->draw_dsc;

    /* Convert mask area from absolute to layer-local coordinates */
    int32_t mask_x1 = dsc->area.x1 - layer->buf_area.x1;
    int32_t mask_y1 = dsc->area.y1 - layer->buf_area.y1;
    int32_t mask_x2 = dsc->area.x2 - layer->buf_area.x1;
    int32_t mask_y2 = dsc->area.y2 - layer->buf_area.y1;

    /* Layer bounds in local coordinates */
    int32_t layer_w = lv_area_get_width(&layer->buf_area);
    int32_t layer_h = lv_area_get_height(&layer->buf_area);

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_vertexFormat(phost, 0);
    EVE_CoDl_saveContext(phost);

    /* Step 1: Clear stencil (clearStencil defaults to 0) */
    EVE_CoDl_clear(phost, 0, 1, 0);

    /* Step 2: Draw rounded rect mask — multiply existing RGBA AND flag stencil.
     * This runs after both the RGB pass and the alpha correction pass, so all
     * four channels have their correct premultiplied values at this point.
     * We must scale ALL channels (not just alpha) because the layer content is
     * premultiplied — scaling only alpha would leave RGB too bright at partially
     * masked edges, causing white fringing when composited onto the parent.
     * blend(ZERO, SRC_ALPHA): dst.rgba = dst.rgba * src.a / 255
     * stencilOp(KEEP, INCR): stencil++ where the primitive draws. */
    EVE_CoDl_blendFunc(phost, ZERO, SRC_ALPHA);
    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
    EVE_CoDl_stencilFunc(phost, ALWAYS, 0, 0xFF);
    EVE_CoDl_stencilOp(phost, KEEP, INCR);

    lv_draw_eve5_draw_rect(u, mask_x1, mask_y1, mask_x2, mask_y2,
                            dsc->radius, &t->clip_area, &layer->buf_area);

    /* Step 3: Zero RGBA everywhere the mask primitive didn't draw.
     * stencilFunc(EQUAL, 0): only affect pixels where stencil is still 0.
     * blend(ZERO, ZERO): result = 0 — clears all channels (correct for
     * premultiplied: fully transparent is 0,0,0,0). */
    if(!dsc->keep_outside) {
        EVE_CoDl_stencilFunc(phost, EQUAL, 0, 0xFF);
        EVE_CoDl_stencilOp(phost, KEEP, KEEP);
        EVE_CoDl_colorA(phost, 0);
        EVE_CoDl_blendFunc(phost, ZERO, ZERO);
        EVE_CoDl_lineWidth(phost, 16);
        EVE_CoDl_begin(phost, RECTS);
        EVE_CoDl_vertex2f_0(phost, 0, 0);
        EVE_CoDl_vertex2f_0(phost, layer_w, layer_h);
        EVE_CoDl_end(phost);
    }

    EVE_CoDl_restoreContext(phost);
}

/**********************
 * BITMAP MASK (LAYER)
 *
 * Applied at the end of a child layer's render (same point as mask_rect).
 * Scales all premultiplied RGBA channels by the mask bitmap, producing
 * correctly masked content that composites with standard premultiplied
 * blend(ONE, ONE_MINUS_SRC_ALPHA) — no multi-phase masking needed.
 *
 * The caller clears bitmap_mask_src on the parent's LAYER task after
 * calling this, so no draw unit double-applies the mask.
 **********************/

void lv_draw_eve5_apply_bitmap_mask(lv_draw_eve5_unit_t *u, lv_layer_t *layer,
                                     const lv_draw_image_dsc_t *layer_dsc)
{
    EVE_HalContext *phost = u->hal;

    int32_t layer_w = lv_area_get_width(&layer->buf_area);
    int32_t layer_h = lv_area_get_height(&layer->buf_area);

    /* Load mask bitmap */
    eve5_gpu_image_t mask_img;
    if(!lv_draw_eve5_resolve_to_gpu(u, layer_dsc->bitmap_mask_src, &mask_img)) {
        LV_LOG_WARN("EVE5: Failed to load bitmap mask for layer %p", (void *)layer);
        return;
    }
    uint32_t mask_addr, mask_palette;
    eve5_gpu_image_resolve(u->allocator, &mask_img, &mask_addr, &mask_palette);
    if(mask_addr == GA_INVALID) return;

    /* Center-align mask on image_area (same convention as image draw) */
    int32_t img_w = lv_area_get_width(&layer_dsc->image_area);
    int32_t img_h = lv_area_get_height(&layer_dsc->image_area);
    int32_t mask_x = (layer_dsc->image_area.x1 - layer->buf_area.x1) + (img_w - mask_img.width) / 2;
    int32_t mask_y = (layer_dsc->image_area.y1 - layer->buf_area.y1) + (img_h - mask_img.height) / 2;

    EVE_CoDl_scissorXY(phost, 0, 0);
    EVE_CoDl_scissorSize(phost, layer_w, layer_h);
    EVE_CoDl_bitmapTransform_identity(phost);
    EVE_CoDl_vertexFormat(phost, 0);
    EVE_CoDl_saveContext(phost);

    /* Set up mask bitmap.
     * For ARGB8/PALETTEDARGB8 masks (grayscale PNGs): use GLFORMAT + swizzle
     * to route RED channel to ALPHA (R=G=B=gray, A=255 → we want gray as alpha).
     * For L8/A8: BT820 natively decodes as (255,255,255,L) — alpha=L, no swizzle. */
    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
    EVE_CoDl_bitmapSource(phost, mask_addr);
    set_palette_if_needed(phost, mask_img.eve_format, mask_palette);
    if(mask_img.eve_format == ARGB8 || mask_img.eve_format == PALETTEDARGB8) {
        EVE_CoDl_bitmapLayout(phost, GLFORMAT, mask_img.eve_stride, mask_img.height);
        EVE_CoDl_bitmapExtFormat(phost, mask_img.eve_format);
        EVE_CoDl_bitmapSwizzle(phost, ZERO, ZERO, ZERO, RED);
    }
    else {
        EVE_CoDl_bitmapLayout(phost, (uint8_t)mask_img.eve_format,
                              mask_img.eve_stride, mask_img.height);
    }

    /* Draw mask covering the full layer area. Use bitmap transform offset
     * to position the mask at (mask_x, mask_y) within the layer.
     * BORDER mode returns 0 for out-of-bounds texels, so:
     *   - Inside mask bitmap: dst *= mask_value (scales RGBA by mask)
     *   - Outside mask bitmap: dst *= 0 (zeroes premultiplied content) */
    EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, layer_w, layer_h);
    EVE_CoDl_bitmapTransformC(phost, -mask_x * 256);
    EVE_CoDl_bitmapTransformF(phost, -mask_y * 256);

    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
    EVE_CoDl_blendFunc(phost, ZERO, SRC_ALPHA);

    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, 0, 0);
    EVE_CoDl_end(phost);

    EVE_CoDl_restoreContext(phost);

    LV_LOG_INFO("EVE5: Applied bitmap mask to layer %p (%dx%d mask at (%d,%d))",
                (void *)layer, (int)mask_img.width, (int)mask_img.height,
                (int)mask_x, (int)mask_y);
}

#endif /* LV_USE_DRAW_EVE5 */
