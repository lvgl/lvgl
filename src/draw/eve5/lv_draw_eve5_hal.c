/**
 * @file lv_draw_eve5_hal.c
 *
 * EVE5 (BT820) Hardware Abstraction Layer for LVGL Draw Unit
 *
 * This file contains:
 * - Layer initialization and finalization (render target setup)
 * - VRAM allocation/upload/download callbacks
 * - L8 alpha render target management
 * - Mask rectangle and bitmap mask drawing
 * - Texture upload/draw utilities for SW fallback compositing
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../lv_draw.h"
#include "../lv_draw_image.h"
#include "../lv_draw_mask.h"

/**********************
 * HELPER FUNCTIONS
 **********************/

void lv_draw_eve5_set_scissor(lv_draw_eve5_unit_t * u, const lv_area_t * clip, const lv_area_t * layer_area)
{
    int32_t x = clip->x1 - layer_area->x1;
    int32_t y = clip->y1 - layer_area->y1;
    int32_t w = lv_area_get_width(clip);
    int32_t h = lv_area_get_height(clip);

    if(x < 0) {
        w += x;
        x = 0;
    }
    if(y < 0) {
        h += y;
        y = 0;
    }
    if(w <= 0 || h <= 0) {
        w = 0;
        h = 0;
    }

    EVE_CoDl_scissorXY(u->hal, x, y);
    EVE_CoDl_scissorSize(u->hal, w, h);
}

void lv_draw_eve5_clear_stencil(lv_draw_eve5_unit_t * u,
                                int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                                const lv_area_t * clip, const lv_area_t * layer_area)
{
    lv_area_t bbox = { x1 + layer_area->x1, y1 + layer_area->y1,
                       x2 + layer_area->x1, y2 + layer_area->y1
                     };
    lv_area_t clear_area;
    if(!lv_area_intersect(&clear_area, &bbox, clip)) return;
    lv_draw_eve5_set_scissor(u, &clear_area, layer_area);
    EVE_CoDl_clear(u->hal, 0, 1, 0);
    lv_draw_eve5_set_scissor(u, clip, layer_area);
}



/**
 * Map LVGL color format to EVE render target format and bytes per pixel.
 * Returns true if the format is directly supported.
 */
bool lv_draw_eve5_get_render_target_format(lv_color_format_t lv_cf, uint16_t * eve_fmt, uint8_t * bpp)
{
    switch(lv_cf) {
        case LV_COLOR_FORMAT_RGB565:
            *eve_fmt = RGB565;
            *bpp = 2;
            return true;

        case LV_COLOR_FORMAT_RGB888:
            *eve_fmt = RGB8;
            *bpp = 3;
            return true;

        case LV_COLOR_FORMAT_XRGB8888:
            *eve_fmt = RGB8;
            *bpp = 3;
            return true;

        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
            *eve_fmt = ARGB8;
            *bpp = 4;
            return true;

        case LV_COLOR_FORMAT_ARGB1555:
        case LV_COLOR_FORMAT_ARGB4444:
            /* Use ARGB8 for better alpha precision */
            *eve_fmt = ARGB8;
            *bpp = 4;
            return true;

        case LV_COLOR_FORMAT_L8:
            *eve_fmt = L8;
            *bpp = 1;
            return true;

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
 * VRAM CALLBACKS
 **********************/

static bool eve5_vram_alloc_cb(lv_draw_unit_t * draw_unit, lv_draw_buf_t * buf)
{
    lv_draw_eve5_unit_t * u = (lv_draw_eve5_unit_t *)draw_unit;

    uint32_t w = buf->header.w;
    uint32_t h = buf->header.h;
    lv_color_format_t cf = (lv_color_format_t)buf->header.cf;

    uint16_t eve_fmt;
    uint8_t bpp;
    lv_draw_eve5_get_render_target_format(cf, &eve_fmt, &bpp);

#if LV_DRAW_EVE5_OPAQUE_LAYER_RGB8
    if(!lv_color_format_has_alpha(cf) && eve_fmt != ARGB8) {
        eve_fmt = RGB8;
        bpp = 3;
    }
#endif

    uint32_t aligned_w = ALIGN_UP(w, 16);
    uint32_t aligned_h = ALIGN_UP(h, 16);
    uint32_t size = aligned_w * aligned_h * bpp;

#if LV_USE_OS
    lv_eve5_hal_lock(lv_display_get_default());
#endif

    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(u->allocator, size, GA_ALIGN_128);
    if(Esd_GpuAlloc_Get(u->allocator, handle) == GA_INVALID) {
        LV_LOG_WARN("EVE5 VRAM alloc failed (%ux%u fmt=%d, %u bytes)", w, h, eve_fmt, size);
#if LV_USE_OS
        lv_eve5_hal_unlock(lv_display_get_default());
#endif
        return false;
    }

    lv_eve5_vram_res_t * vr = lv_malloc(sizeof(lv_eve5_vram_res_t));
    if(vr == NULL) {
        Esd_GpuAlloc_Free(u->allocator, handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(lv_display_get_default());
#endif
        return false;
    }

    vr->base.unit = draw_unit;
    vr->base.size = size;
    vr->gpu_handle = handle;
    vr->eve_format = eve_fmt;
    vr->stride = aligned_w * bpp;
    vr->source_offset = 0;
    vr->palette_offset = GA_INVALID;
    vr->is_premultiplied = false;
    vr->has_content = false;

    buf->vram_res = (lv_draw_buf_vram_res_t *)vr;

#if LV_USE_OS
    lv_eve5_hal_unlock(lv_display_get_default());
#endif

    LV_LOG_INFO("EVE5 VRAM alloc: %ux%u fmt=%d stride=%u -> handle %d",
                w, h, eve_fmt, vr->stride, handle.Id);
    return true;
}

static void eve5_vram_free_cb(lv_draw_unit_t * draw_unit, lv_draw_buf_t * buf)
{
    lv_draw_eve5_unit_t * u = (lv_draw_eve5_unit_t *)draw_unit;
    lv_eve5_vram_res_t * vr = (lv_eve5_vram_res_t *)buf->vram_res;

    if(vr == NULL) return;

#if LV_USE_OS
    lv_eve5_hal_lock(lv_display_get_default());
#endif

    /* PendingFree: the texture may still be referenced by an in-flight
     * display list. FlushPending at the next finish_layer will assign
     * the sync value and UpdateFree will reclaim the memory. */
    Esd_GpuAlloc_PendingFree(u->allocator, vr->gpu_handle);

#if LV_USE_OS
    lv_eve5_hal_unlock(lv_display_get_default());
#endif

    lv_free(vr);
    buf->vram_res = NULL;
}

static bool eve5_vram_upload_cb(lv_draw_unit_t * draw_unit, lv_draw_buf_t * buf)
{
    /* Non-ALLOCATED buffers with no data are render target placeholders; just allocate VRAM.
     * Image descriptors (handlers == NULL) also lack ALLOCATED but have data to upload. */
    if(!(buf->header.flags & LV_IMAGE_FLAGS_ALLOCATED) && buf->data == NULL) {
        return eve5_vram_alloc_cb(draw_unit, buf);
    }

    /* MODIFIABLE + ALLOCATED: render-target-capable buffer with pixel data.
     * Indexed formats need proper palette conversion, so route through read-only path. */
    if((buf->header.flags & LV_IMAGE_FLAGS_MODIFIABLE)
       && !LV_COLOR_FORMAT_IS_INDEXED(buf->header.cf)) {
        if(!eve5_vram_alloc_cb(draw_unit, buf)) return false;

        lv_draw_eve5_unit_t * u = (lv_draw_eve5_unit_t *)draw_unit;
        lv_eve5_vram_res_t * vr = (lv_eve5_vram_res_t *)buf->vram_res;

#if LV_USE_OS
        lv_eve5_hal_lock(lv_display_get_default());
#endif

        uint32_t gpu_addr = Esd_GpuAlloc_Get(u->allocator, vr->gpu_handle);
        if(gpu_addr == GA_INVALID) {
#if LV_USE_OS
            lv_eve5_hal_unlock(lv_display_get_default());
#endif
            return false;
        }

        uint32_t cpu_stride = buf->header.stride;
        if(cpu_stride == 0) cpu_stride = lv_draw_buf_width_to_stride(buf->header.w, buf->header.cf);
        uint32_t gpu_stride = vr->stride;
        uint32_t row_bytes = LV_MIN(cpu_stride, gpu_stride);
        uint8_t * src = buf->data;

        if(src != NULL) {
            if(gpu_stride == row_bytes) {
                for(int32_t y = 0; y < buf->header.h; y++) {
                    EVE_Hal_wrMem(u->hal, gpu_addr + y * gpu_stride, src + y * cpu_stride, row_bytes);
                }
            }
            else {
                /* GPU stride wider than source — zero-pad each row */
                uint8_t * row_buf = lv_malloc(gpu_stride);
                if(row_buf != NULL) {
                    for(int32_t y = 0; y < buf->header.h; y++) {
                        lv_memzero(row_buf, gpu_stride);
                        lv_memcpy(row_buf, src + y * cpu_stride, row_bytes);
                        EVE_Hal_wrMem(u->hal, gpu_addr + y * gpu_stride, row_buf, gpu_stride);
                    }
                    lv_free(row_buf);
                }
                else {
                    for(int32_t y = 0; y < buf->header.h; y++) {
                        EVE_Hal_wrMem(u->hal, gpu_addr + y * gpu_stride, src + y * cpu_stride, row_bytes);
                    }
                }
            }
            vr->is_premultiplied = lv_draw_buf_has_flag(buf, LV_IMAGE_FLAGS_PREMULTIPLIED)
                                   || buf->header.cf == LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED;
            vr->has_content = true;
            EVE_Hal_requestFenceBeforeSwap(u->hal);
        }

#if LV_USE_OS
        lv_eve5_hal_unlock(lv_display_get_default());
#endif
        return true;
    }

    /* Read-only image data: upload with image alignment.
     * upload_image_to_gpu checks existing vram_res, uploads if needed,
     * and attaches vram_res directly to the image descriptor. */
    lv_draw_eve5_unit_t * u = (lv_draw_eve5_unit_t *)draw_unit;

#if LV_USE_OS
    lv_eve5_hal_lock(lv_display_get_default());
#endif

    lv_eve5_vram_res_t * vr = lv_draw_eve5_upload_image_to_gpu(u, (LV_IMAGE_DSC_CONST lv_image_dsc_t *)buf);

#if LV_USE_OS
    lv_eve5_hal_unlock(lv_display_get_default());
#endif

    return vr != NULL;
}

static bool eve5_vram_download_cb(lv_draw_unit_t * draw_unit, lv_draw_buf_t * buf)
{
    lv_draw_eve5_unit_t * u = (lv_draw_eve5_unit_t *)draw_unit;
    lv_eve5_vram_res_t * vr = (lv_eve5_vram_res_t *)buf->vram_res;

    if(vr == NULL || buf->data == NULL) return false;

#if LV_USE_OS
    lv_eve5_hal_lock(lv_display_get_default());
#endif

    /* Flush coprocessor before reading since render target writes may be in flight.
     * TODO: Use EVE_CmdSync marker from hal_finish_layer for more precise sync. */
    EVE_Cmd_waitFlush(u->hal);

    bool ok = lv_draw_eve5_download_image(u, buf, vr);

#if LV_USE_OS
    lv_eve5_hal_unlock(lv_display_get_default());
#endif

    return ok;
}

static bool eve5_vram_check_cb(lv_draw_unit_t * draw_unit, lv_draw_buf_t * buf)
{
    lv_draw_eve5_unit_t * u = (lv_draw_eve5_unit_t *)draw_unit;
    lv_eve5_vram_res_t * vr = (lv_eve5_vram_res_t *)buf->vram_res;

    if(vr == NULL) return false;

    return Esd_GpuAlloc_Get(u->allocator, vr->gpu_handle) != GA_INVALID;
}

void lv_draw_eve5_register_vram_callbacks(lv_draw_eve5_unit_t * u)
{
    u->base_unit.vram_alloc_cb    = eve5_vram_alloc_cb;
    u->base_unit.vram_free_cb     = eve5_vram_free_cb;
    u->base_unit.vram_upload_cb   = eve5_vram_upload_cb;
    u->base_unit.vram_download_cb = eve5_vram_download_cb;
    u->base_unit.vram_check_cb    = eve5_vram_check_cb;
}

/**********************
 * LAYER MANAGEMENT
 **********************/

void lv_draw_eve5_hal_init_layer(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                 bool is_screen,
                                 const lv_draw_eve5_slice_t * slice)
{
    uint32_t ram_g_addr = GA_INVALID;
    bool existing_has_content = false;
    bool existing_is_premultiplied = false;
    uint16_t existing_format = ARGB8;
    uint32_t existing_stride = 0;
    bool existing_needs_conversion = false;

    int32_t w = lv_area_get_width(&layer->buf_area);
    int32_t h = lv_area_get_height(&layer->buf_area);
    int32_t aligned_w = ALIGN_UP(w, 16);
    int32_t aligned_h = ALIGN_UP(h, 16);

    /* Determine target render format */
    uint16_t target_eve_fmt = ARGB8;
    uint8_t target_bpp = 4;
    if(!is_screen) {
        lv_color_format_t target_lv_cf;
        if(layer->draw_buf != NULL) {
            target_lv_cf = layer->draw_buf->header.cf;
        }
        else {
            target_lv_cf = layer->color_format;
        }
        lv_draw_eve5_get_render_target_format(target_lv_cf, &target_eve_fmt, &target_bpp);

#if LV_DRAW_EVE5_OPAQUE_LAYER_RGB8
        if(!lv_color_format_has_alpha(target_lv_cf) && target_eve_fmt != ARGB8) {
            target_eve_fmt = RGB8;
            target_bpp = 3;
        }
#endif
    }

    lv_eve5_vram_res_t * vr = eve5_get_vram_res(layer);
    if(vr != NULL) {
        /* Reallocate if format or dimensions don't match current layer needs.
         * Format mismatch: e.g., display buffer format vs render target format.
         * Stride too narrow: layer wider than previous allocation.
         * Height overflow: existing stride × current aligned_h exceeds allocation.
         *   This catches the case where a reused buffer is wider (larger stride)
         *   but the current layer is taller — stride × height would overrun.
         * Any reallocation is an implicit discard (has_content = false). */
        uint32_t needed_stride = (uint32_t)aligned_w * target_bpp;
        bool realloc_needed = (vr->eve_format != target_eve_fmt)
                              || (vr->stride < needed_stride)
                              || (vr->stride * (uint32_t)aligned_h > vr->base.size);
        if(realloc_needed) {
            uint32_t needed_size = needed_stride * (uint32_t)aligned_h;
            /* PendingFree: previous buffer may be referenced by a prior frame's compositing DL */
            Esd_GpuAlloc_PendingFree(u->allocator, vr->gpu_handle);
            vr->gpu_handle = Esd_GpuAlloc_Alloc(u->allocator, needed_size, GA_ALIGN_128);
            vr->eve_format = target_eve_fmt;
            vr->stride = needed_stride;
            vr->source_offset = 0;
            vr->palette_offset = GA_INVALID;
            vr->base.size = needed_size;
            vr->has_content = false;
        }

        ram_g_addr = Esd_GpuAlloc_Get(u->allocator, vr->gpu_handle);
        if(ram_g_addr != GA_INVALID) {
            aligned_w = vr->stride / target_bpp;
            if(layer->draw_buf && lv_draw_buf_has_flag(layer->draw_buf,
                                                       LV_IMAGE_FLAGS_CLEARZERO | LV_IMAGE_FLAGS_DISCARDABLE)) {
                lv_draw_buf_clear_flag(layer->draw_buf,
                                       LV_IMAGE_FLAGS_CLEARZERO | LV_IMAGE_FLAGS_DISCARDABLE);
                vr->has_content = false;
            }
            if(vr->has_content) {
                existing_has_content = true;
                existing_is_premultiplied = vr->is_premultiplied;
                existing_format = vr->eve_format;
                existing_stride = vr->stride;
                if(existing_format != target_eve_fmt) {
                    existing_needs_conversion = true;
                }
            }
        }
        else {
            LV_LOG_ERROR("EVE5: VRAM handle invalid for layer %p", (void *)layer);
            return;
        }
    }
    else {
        LV_LOG_ERROR("EVE5: No vram_res on layer %p", (void *)layer);
        return;
    }

    /* EVE5 renders with SRC_ALPHA blending, producing premultiplied output */
    if(lv_color_format_has_alpha(layer->color_format)) {
        if(vr != NULL) vr->is_premultiplied = true;
        if(layer->draw_buf != NULL) {
            lv_draw_buf_set_flag(layer->draw_buf, LV_IMAGE_FLAGS_PREMULTIPLIED);
        }
    }

    EVE_CoCmd_renderTarget(u->hal, ram_g_addr, target_eve_fmt, aligned_w, aligned_h);
    EVE_CoCmd_dlStart(u->hal);
    EVE_CoDl_scissorXY(u->hal, 0, 0);
    EVE_CoDl_scissorSize(u->hal, aligned_w, aligned_h);

    u->canvas_orig_addr = GA_INVALID;
    u->canvas_orig_palette = GA_INVALID;

    if(slice->isolated) {
        /* Isolated slice: clear to transparent black, no content inheritance.
         * Used for rendering blend-mode tasks into a clean buffer. */
        EVE_CoDl_clearColorRgb(u->hal, 0, 0, 0);
        EVE_CoDl_clearColorA(u->hal, 0);
        EVE_CoDl_clear(u->hal, 1, 1, 1);
    }
    /* Sentinel check: prev_handle is GA_HANDLE_INVALID when no previous slice was assigned */
    else if(slice->prev_handle.Id != GA_HANDLE_INVALID.Id) {
        /* Previous slice output: blit as starting point (always premultiplied) */
        uint32_t prev_addr = Esd_GpuAlloc_Get(u->allocator, slice->prev_handle);
        if(prev_addr != GA_INVALID) {
            EVE_CoDl_clearColorRgb(u->hal, 0, 0, 0);
            EVE_CoDl_clearColorA(u->hal, is_screen ? 255 : 0);
            EVE_CoDl_clear(u->hal, 1, 1, 1);

            EVE_CoDl_saveContext(u->hal);
            EVE_CoDl_blendFunc(u->hal, ONE, ZERO);
            EVE_CoDl_colorArgb_ex(u->hal, 0xFFFFFFFF);
            EVE_CoDl_bitmapHandle(u->hal, u->hal->CoScratchHandle);
            EVE_CoDl_bitmapSource(u->hal, prev_addr);
            EVE_CoDl_bitmapLayout(u->hal, (uint8_t)target_eve_fmt, aligned_w * target_bpp, h);
            EVE_CoDl_bitmapSize(u->hal, NEAREST, BORDER, BORDER, w, h);
            EVE_CoDl_begin(u->hal, BITMAPS);
            EVE_CoDl_vertex2f_0(u->hal, 0, 0);
            EVE_CoDl_end(u->hal);
            EVE_CoDl_restoreContext(u->hal);

            u->canvas_orig_addr = prev_addr;
            u->canvas_orig_format = target_eve_fmt;
            u->canvas_orig_stride = aligned_w * target_bpp;
            u->canvas_orig_palette = GA_INVALID;
            u->canvas_orig_w = w;
            u->canvas_orig_h = h;

            Esd_GpuAlloc_PendingFree(u->allocator, slice->prev_handle);
        }
        else {
            EVE_CoDl_clearColorRgb(u->hal, 0, 0, 0);
            EVE_CoDl_clearColorA(u->hal, is_screen ? 255 : 0);
            EVE_CoDl_clear(u->hal, 1, 1, 1);
        }
    }
    else if(existing_has_content) {
        /* Incremental render: allocate new buffer and blit existing content.
         * Required because:
         * 1. Can't clear-then-blit from the same buffer (clear destroys content)
         * 2. Alpha pass needs original content to incorporate existing alpha as
         *    the "base layer" before compositing new tasks */
        uint32_t src_addr = ram_g_addr;
        uint16_t src_format = existing_format;
        uint32_t src_stride = existing_stride;

        Esd_GpuHandle old_handle = GA_HANDLE_INVALID;
        bool have_old_handle = false;

        uint32_t new_size = aligned_w * aligned_h * target_bpp;
        Esd_GpuHandle new_handle = Esd_GpuAlloc_Alloc(u->allocator, new_size, GA_ALIGN_128);
        uint32_t new_addr = Esd_GpuAlloc_Get(u->allocator, new_handle);

        if(new_addr == GA_INVALID) {
            LV_LOG_ERROR("EVE5: Failed to allocate buffer for canvas incremental render");
            EVE_CoDl_clearColorRgb(u->hal, 0, 0, 0);
            EVE_CoDl_clearColorA(u->hal, 0);
            EVE_CoDl_clear(u->hal, 1, 1, 1);
            goto canvas_cleared;
        }

        uint32_t src_palette = GA_INVALID;
        {
            old_handle = vr->gpu_handle;
            have_old_handle = true;
            if(vr->palette_offset != GA_INVALID) {
                uint32_t old_base = Esd_GpuAlloc_Get(u->allocator, old_handle);
                if(old_base != GA_INVALID) src_palette = old_base + vr->palette_offset;
            }
            vr->gpu_handle = new_handle;
            vr->eve_format = target_eve_fmt;
            vr->stride = aligned_w * target_bpp;
            vr->palette_offset = GA_INVALID;
            vr->source_offset = 0;
            vr->base.size = new_size;
        }

        /* Store original content reference for alpha pass */
        u->canvas_orig_addr = src_addr;
        u->canvas_orig_format = src_format;
        u->canvas_orig_stride = src_stride;
        u->canvas_orig_palette = src_palette;
        u->canvas_orig_w = w;
        u->canvas_orig_h = h;

        ram_g_addr = new_addr;
        EVE_CoCmd_renderTarget(u->hal, ram_g_addr, target_eve_fmt, aligned_w, aligned_h);

        if(existing_needs_conversion) {
            LV_LOG_INFO("EVE5: Converting layer from format %d to %d", src_format, target_eve_fmt);
        }

        EVE_CoDl_clearColorRgb(u->hal, 0, 0, 0);
        EVE_CoDl_clearColorA(u->hal, 0);
        EVE_CoDl_clear(u->hal, 1, 1, 1);

        /* Blit existing content, converting to premultiplied if needed */
        EVE_CoDl_saveContext(u->hal);
        if(existing_needs_conversion || existing_is_premultiplied) {
            EVE_CoDl_blendFunc(u->hal, ONE, ZERO);
        }
        else {
            EVE_CoDl_blendFunc_default(u->hal);
            if(vr != NULL) {
                vr->is_premultiplied = true;
            }
            LV_LOG_INFO("EVE5: Converting layer to premultiplied alpha");
        }
        EVE_CoDl_colorArgb_ex(u->hal, 0xFFFFFFFF);
        EVE_CoCmd_setBitmap(u->hal, src_addr, (uint8_t)src_format, w, h);
        EVE_CoDl_bitmapLayout(u->hal, (uint8_t)src_format, src_stride, h);
        EVE_CoDl_bitmapSize(u->hal, NEAREST, BORDER, BORDER, w, h);
        if(src_palette != GA_INVALID) {
            EVE_CoDl_paletteSource(u->hal, src_palette);
        }
        EVE_CoDl_begin(u->hal, BITMAPS);
        EVE_CoDl_vertex2f_0(u->hal, 0, 0);
        EVE_CoDl_end(u->hal);
        EVE_CoDl_restoreContext(u->hal);

        if(have_old_handle) {
            Esd_GpuAlloc_PendingFree(u->allocator, old_handle);
        }

        LV_LOG_INFO("EVE5: Blitting existing layer content (%"PRId32"x%"PRId32" fmt=%d) from 0x%08X to 0x%08X",
                    w, h, src_format, src_addr, ram_g_addr);
    }
    else {
        EVE_CoDl_clearColorRgb(u->hal, 0, 0, 0);
        if(is_screen) {
            /* EVE_CoDl_clearColorRgb(u->hal, 0, 128, 0); */ /* TEST COLOR */
            EVE_CoDl_clearColorA(u->hal, 255);
        }
        else {
            /* EVE_CoDl_clearColorRgb(u->hal, 0, 0, 128); */ /* TEST COLOR */
            EVE_CoDl_clearColorA(u->hal, 0);
        }
        EVE_CoDl_clear(u->hal, 1, 1, 1);
    }

canvas_cleared:
    EVE_CoDl_scissorXY(u->hal, 0, 0);
    EVE_CoDl_scissorSize(u->hal, w, h);
    EVE_CoDl_colorArgb_ex(u->hal, 0xFFFFFFFF);
    EVE_CoDl_blendFunc(u->hal, SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
}

void lv_draw_eve5_hal_finish_layer(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                   bool is_screen, int rendered_count)
{
    {
        lv_eve5_vram_res_t * finish_vr = eve5_get_vram_res(layer);
        if(finish_vr != NULL) {
            /* Only update state when tasks actually rendered.
             * A discarded/fresh layer that ran zero tasks should retain
             * its current has_content and is_premultiplied flags — the
             * content wasn't re-rendered so its state hasn't changed. */
            if(rendered_count > 0) {
                finish_vr->is_premultiplied = true;
                finish_vr->has_content = true;
            }
        }
    }

    EVE_CoDl_display(u->hal);
    EVE_CoCmd_swap(u->hal);
    EVE_CoCmd_graphicsFinish(u->hal);

    EVE_CmdSync sync = EVE_Cmd_sync(u->hal);
    Esd_GpuAlloc_FlushPending(u->allocator, sync);
}

/**********************
 * L8 ALPHA RENDER TARGET
 **********************/

/**
 * Allocate L8 texture and start DL cycle for alpha rendering.
 * Returns GpuHandle (GA_HANDLE_INVALID on failure).
 */
Esd_GpuHandle lv_draw_eve5_hal_init_l8_rendertarget(lv_draw_eve5_unit_t * u,
                                                    int32_t aligned_w, int32_t aligned_h,
                                                    int32_t w, int32_t h)
{
    EVE_HalContext *phost = u->hal;

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

    EVE_CoCmd_renderTarget(phost, l8_addr, L8, aligned_w, aligned_h);
    EVE_CoCmd_dlStart(phost);

    EVE_CoDl_scissorXY(phost, 0, 0);
    EVE_CoDl_scissorSize(phost, aligned_w, aligned_h);
    EVE_CoDl_clearColorRgb(phost, 0, 0, 0);
    EVE_CoDl_clear(phost, 1, 1, 1);

    EVE_CoDl_scissorXY(phost, 0, 0);
    EVE_CoDl_scissorSize(phost, w, h);

    /* Canvas: blit existing alpha as base luminance.
     * BITMAP_SWIZZLE routes alpha to RGB so L8 target captures it as luminance. */
    if(u->canvas_orig_addr != GA_INVALID) {
        EVE_CoDl_saveContext(phost);
        EVE_CoDl_blendFunc(phost, ONE, ZERO);
        EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
        EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
        EVE_CoDl_bitmapSource(phost, u->canvas_orig_addr);
        /* GLFORMAT mode required for BITMAP_SWIZZLE to take effect on BT820 */
        EVE_CoDl_bitmapLayout(phost, GLFORMAT, u->canvas_orig_stride, u->canvas_orig_h);
        EVE_CoDl_bitmapExtFormat(phost, u->canvas_orig_format);
        EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, u->canvas_orig_w, u->canvas_orig_h);
        if(u->canvas_orig_palette != GA_INVALID) {
            EVE_CoDl_paletteSource(phost, u->canvas_orig_palette);
        }
        EVE_CoDl_bitmapSwizzle(phost, ALPHA, ALPHA, ALPHA, ONE);
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, 0, 0);
        EVE_CoDl_end(phost);
        EVE_CoDl_restoreContext(phost);
    }

    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
    EVE_CoDl_blendFunc(phost, SRC_ALPHA, ONE_MINUS_SRC_ALPHA);

    return l8_handle;
}

void lv_draw_eve5_hal_finish_l8_rendertarget(lv_draw_eve5_unit_t * u)
{
    EVE_CoDl_display(u->hal);
    EVE_CoCmd_swap(u->hal);
    EVE_CoCmd_graphicsFinish(u->hal);

    EVE_CmdSync l8_sync = EVE_Cmd_sync(u->hal);
    Esd_GpuAlloc_FlushPending(u->allocator, l8_sync);
}

/**
 * Blit L8 luminance into the current ARGB8 layer's alpha channel.
 * BT820 L-format decodes as (R=255, G=255, B=255, A=L), so luminance is in alpha.
 */
void lv_draw_eve5_hal_blit_l8_to_alpha(lv_draw_eve5_unit_t * u, uint32_t l8_addr,
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

    EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
    EVE_CoDl_blendFunc(phost, ONE, ZERO);
    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);

    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, 0, 0);
    EVE_CoDl_end(phost);

    EVE_CoDl_restoreContext(phost);
}

#if LV_DRAW_EVE5_SW_FALLBACK
/**********************
 * TEXTURE DRAWING (SW fallback compositing)
 **********************/

Esd_GpuHandle lv_draw_eve5_hal_upload_texture(lv_draw_eve5_unit_t * u,
                                              const uint8_t * buf_data,
                                              int32_t buf_w, int32_t buf_h,
                                              uint32_t * out_stride)
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
        uint8_t * row_buf = lv_malloc(eve_stride);
        if(row_buf != NULL) {
            for(int32_t y = 0; y < buf_h; y++) {
                lv_memzero(row_buf, eve_stride);
                lv_memcpy(row_buf, buf_data + y * src_stride, src_stride);
                EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, row_buf, eve_stride);
            }
            lv_free(row_buf);
        }
        else {
            for(int32_t y = 0; y < buf_h; y++) {
                EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride,
                              buf_data + y * src_stride, src_stride);
            }
        }
    }

    *out_stride = eve_stride;

    EVE_Hal_requestFenceBeforeSwap(u->hal);

    LV_LOG_TRACE("EVE5: Uploaded texture %"PRId32"x%"PRId32" to 0x%08X",
                 buf_w, buf_h, ram_g_addr);

    return handle;
}

void lv_draw_eve5_hal_draw_texture(lv_draw_eve5_unit_t * u,
                                   const lv_draw_task_t * t,
                                   uint32_t ram_g_addr,
                                   int32_t tex_w, int32_t tex_h,
                                   uint32_t eve_stride,
                                   const lv_area_t * draw_area)
{
    lv_layer_t * layer = t->target_layer;

    if(ram_g_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Invalid RAM_G address for texture draw");
        return;
    }

    int32_t x = draw_area->x1 - layer->buf_area.x1;
    int32_t y = draw_area->y1 - layer->buf_area.y1;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorArgb_ex(u->hal, 0xFFFFFFFF);
    EVE_CoDl_bitmapTransform_identity(u->hal);

    EVE_CoDl_bitmapHandle(u->hal, u->hal->CoScratchHandle);
    EVE_CoDl_bitmapSource(u->hal, ram_g_addr);
    EVE_CoDl_bitmapLayout(u->hal, ARGB8, eve_stride, tex_h);
    EVE_CoDl_bitmapSize(u->hal, NEAREST, BORDER, BORDER, tex_w, tex_h);

    EVE_CoDl_begin(u->hal, BITMAPS);
    EVE_CoDl_vertex2f_0(u->hal, x, y);
    EVE_CoDl_end(u->hal);
}

bool lv_draw_eve5_hal_check_texture(lv_draw_eve5_unit_t * u, Esd_GpuHandle handle)
{
    return (Esd_GpuAlloc_Get(u->allocator, handle) != GA_INVALID);
}
#endif /* LV_DRAW_EVE5_SW_FALLBACK */

/**********************
 * MASK RECTANGLE
 **********************/

/**
 * Draw mask rectangle using stencil buffer.
 *
 * Runs after both RGB pass and alpha correction, so all four channels have
 * correct premultiplied values. Scales ALL channels (not just alpha) because
 * premultiplied content requires RGB to scale with alpha to avoid white
 * fringing at partially masked edges.
 */
void lv_draw_eve5_hal_draw_mask_rect(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t)
{
    EVE_HalContext *phost = u->hal;
    lv_layer_t * layer = t->target_layer;
    const lv_draw_mask_rect_dsc_t * dsc = t->draw_dsc;

    int32_t mask_x1 = dsc->area.x1 - layer->buf_area.x1;
    int32_t mask_y1 = dsc->area.y1 - layer->buf_area.y1;
    int32_t mask_x2 = dsc->area.x2 - layer->buf_area.x1;
    int32_t mask_y2 = dsc->area.y2 - layer->buf_area.y1;

    int32_t layer_w = lv_area_get_width(&layer->buf_area);
    int32_t layer_h = lv_area_get_height(&layer->buf_area);

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_vertexFormat(phost, 0);
    EVE_CoDl_saveContext(phost);

    EVE_CoDl_clear(phost, 0, 1, 0);

    /* Draw mask shape: multiply existing RGBA and flag stencil.
     * blend(ZERO, SRC_ALPHA): dst.rgba = dst.rgba * src.a / 255
     * stencilOp(KEEP, INCR): stencil++ where primitive draws */
    EVE_CoDl_blendFunc(phost, ZERO, SRC_ALPHA);
    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
    EVE_CoDl_stencilFunc(phost, ALWAYS, 0, 0xFF);
    EVE_CoDl_stencilOp(phost, KEEP, INCR);

    lv_draw_eve5_draw_rect(u, mask_x1, mask_y1, mask_x2, mask_y2,
                           dsc->radius, &t->clip_area, &layer->buf_area);

    /* Zero RGBA outside mask (stencil still 0).
     * blend(ZERO, ZERO): clears all channels (correct for premultiplied) */
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
 **********************/

/**
 * Apply bitmap mask to child layer.
 *
 * Scales all premultiplied RGBA channels by mask bitmap, producing correctly
 * masked content for compositing with blend(ONE, ONE_MINUS_SRC_ALPHA).
 * Caller clears bitmap_mask_src afterward to prevent double-application.
 */
void lv_draw_eve5_apply_bitmap_mask(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                    const lv_draw_image_dsc_t * layer_dsc)
{
    EVE_HalContext *phost = u->hal;

    int32_t layer_w = lv_area_get_width(&layer->buf_area);
    int32_t layer_h = lv_area_get_height(&layer->buf_area);

    lv_eve5_vram_res_t * mask_img = lv_draw_eve5_resolve_to_gpu(u, layer_dsc->bitmap_mask_src);
    if(!mask_img) {
        LV_LOG_WARN("EVE5: Failed to load bitmap mask for layer %p", (void *)layer);
        return;
    }
    uint32_t mask_addr, mask_palette;
    eve5_vram_res_resolve(u->allocator, mask_img, &mask_addr, &mask_palette);
    if(mask_addr == GA_INVALID) return;

    /* Center-align mask on image_area */
    int32_t img_w = lv_area_get_width(&layer_dsc->image_area);
    int32_t img_h = lv_area_get_height(&layer_dsc->image_area);
    int32_t mask_x = (layer_dsc->image_area.x1 - layer->buf_area.x1) + (img_w - mask_img->width) / 2;
    int32_t mask_y = (layer_dsc->image_area.y1 - layer->buf_area.y1) + (img_h - mask_img->height) / 2;

    EVE_CoDl_scissorXY(phost, 0, 0);
    EVE_CoDl_scissorSize(phost, layer_w, layer_h);
    EVE_CoDl_bitmapTransform_identity(phost);
    EVE_CoDl_vertexFormat(phost, 0);
    EVE_CoDl_saveContext(phost);

    /* Set up mask bitmap.
     * ARGB8/PALETTEDARGB8 grayscale masks: swizzle RED to ALPHA.
     * L8/A8: BT820 natively decodes as (255,255,255,L). */
    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
    EVE_CoDl_bitmapSource(phost, mask_addr);
    set_palette_if_needed(phost, mask_img->eve_format, mask_palette);
    if(mask_img->eve_format == ARGB8 || mask_img->eve_format == PALETTEDARGB8) {
        EVE_CoDl_bitmapLayout(phost, GLFORMAT, mask_img->stride, mask_img->height);
        EVE_CoDl_bitmapExtFormat(phost, mask_img->eve_format);
        EVE_CoDl_bitmapSwizzle(phost, ZERO, ZERO, ZERO, RED);
    }
    else {
        EVE_CoDl_bitmapLayout(phost, (uint8_t)mask_img->eve_format,
                              mask_img->stride, mask_img->height);
    }

    /* Draw mask over full layer. BORDER mode returns 0 outside bitmap bounds. */
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
                (void *)layer, (int)mask_img->width, (int)mask_img->height,
                (int)mask_x, (int)mask_y);
}

#endif /* LV_USE_DRAW_EVE5 */
