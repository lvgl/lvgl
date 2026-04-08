/**
 * @file lv_eve5.c
 *
 * EVE5 (BT820) Display Driver for LVGL
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

/*********************
 * INCLUDES
 *********************/
#include "lv_eve5.h"

#if LV_USE_EVE5

#include "../../../core/lv_refr.h"
#include "../../../stdlib/lv_mem.h"
#include "../../../display/lv_display_private.h"
#include "../../../core/lv_refr_private.h"
#if LV_USE_OS
    #include "../../../osal/lv_os_private.h"
#endif

/*********************
 * DEFINES
 *********************/

#define MAX_REGIONS 128

/**********************
 * TYPEDEFS
 **********************/

typedef struct {
    Esd_GpuHandle handle;
    lv_area_t area;
    bool is_gpu_rendered;  /* GPU path (ARGB8) vs CPU path (RGB565/RGB8/ARGB8) */
} rendered_region_t;

typedef struct {
    EVE_HalContext * hal;
    Esd_GpuAlloc * allocator;
    uint32_t frame_buffer_0;
    uint32_t frame_buffer_1;
    uint32_t current_fb;
    rendered_region_t pending_regions[MAX_REGIONS];
    int pending_count;
    EVE_CmdSync last_frame_sync;
#if LV_USE_OS
    lv_mutex_t hal_mutex;
#endif
} lv_eve5_driver_t;

/**********************
 * STATIC PROTOTYPES
 **********************/

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static void wait_cb(lv_display_t * disp);
static void invalidate_area_cb(lv_event_t * e);
static void composite_to_framebuffer(lv_eve5_driver_t * drvr);

/**********************
 * MACROS
 **********************/

/* SW render format depends on LVGL color depth */
#if LV_COLOR_DEPTH == 16
    #define EVE_SW_BITMAP_FORMAT RGB565
    #define SW_BYTES_PER_PIXEL 2
#elif LV_COLOR_DEPTH == 24
    #define EVE_SW_BITMAP_FORMAT RGB8
    #define SW_BYTES_PER_PIXEL 3
#elif LV_COLOR_DEPTH == 32
    #define EVE_SW_BITMAP_FORMAT ARGB8
    #define SW_BYTES_PER_PIXEL 4
#else
    #error "Unsupported LV_COLOR_DEPTH - must be 16, 24, or 32"
#endif

/* Framebuffer (screen memory) is always RGB8 */
#define FB_BYTES_PER_PIXEL 3
#define FB_BITMAP_FORMAT RGB8

/* HW draw unit always produces ARGB8 */
#define HW_BYTES_PER_PIXEL 4
#define HW_BITMAP_FORMAT ARGB8

/**********************
 * GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_eve5_create(EVE_HalContext *hal, Esd_GpuAlloc *allocator)
{
    EVE_HalContext *phost = hal;

    lv_display_t * disp = lv_display_create(phost->Width, phost->Height);
    if(disp == NULL)
        return NULL;

    lv_eve5_driver_t * drvr = lv_malloc_zeroed(sizeof(lv_eve5_driver_t));
    if(drvr == NULL) {
        lv_display_delete(disp);
        return NULL;
    }
    drvr->hal = hal;
    drvr->allocator = allocator;
    drvr->pending_count = 0;
    drvr->last_frame_sync = EVE_CMD_SYNC_INVALID;
#if LV_USE_OS
    lv_mutex_init(&drvr->hal_mutex);
#endif

    lv_display_set_driver_data(disp, drvr);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_flush_wait_cb(disp, wait_cb);

    /* Expand invalidated areas by 1px to cover EVE's AA fringe bleed */
    lv_display_add_event_cb(disp, invalidate_area_cb, LV_EVENT_INVALIDATE_AREA, NULL);

    /* Lazy-allocated tile buffer for partial rendering.
     * Header-only initially; backing allocated on first dispatch:
     * - EVE5 draw unit: VRAM only
     * - SW fallback: CPU memory
     * DISCARDABLE flag set after each flush prevents stale uploads. */
    {
        lv_color_format_t cf = lv_display_get_color_format(disp);
        uint32_t stride = lv_draw_buf_width_to_stride(phost->Width, cf);
        lv_draw_buf_t * buf1 = lv_malloc_zeroed(sizeof(lv_draw_buf_t));
        if(buf1 == NULL) {
            lv_free(drvr);
            lv_display_delete(disp);
            return NULL;
        }
        buf1->header.magic = LV_IMAGE_HEADER_MAGIC;
        buf1->header.w = phost->Width;
        buf1->header.h = 64;
        buf1->header.cf = cf;
        buf1->header.stride = stride;
        buf1->header.flags = LV_IMAGE_FLAGS_MODIFIABLE | LV_IMAGE_FLAGS_ALLOCATED;
        buf1->data_size = stride * 64;
        buf1->handlers = lv_draw_buf_get_handlers();

        lv_display_set_draw_buffers(disp, buf1, NULL);
        lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_PARTIAL);
    }

    /* Get swapchain buffer addresses from HAL */
    drvr->frame_buffer_0 = EVE_Hal_rd32(phost, REG_SC0_PTR0);
    EVE_Hal_wr32(phost, REG_SC0_PTR1, drvr->frame_buffer_0); /* TODO: Remove, forces single buffer for testing */
    drvr->frame_buffer_1 = EVE_Hal_rd32(phost, REG_SC0_PTR1);
    drvr->current_fb = 0;

    /* Initialize GPU allocator */
    allocator->TotalMemorySize = RAM_G_SIZE;
    allocator->TotalReserved = RAM_G_SIZE - RAM_G_AVAILABLE;
    Esd_GpuAlloc_SetGen(allocator, EVE_GEN);
    Esd_GpuAlloc_Reset(allocator);

    /* TODO: Handle REG_SC0_RESET on coprocessor error to avoid missed SWAPs */

    /* Clear both framebuffers with distinct colors to detect buffer order */
    EVE_CoCmd_dlStart(phost);
    EVE_CoDl_clearColorRgb_ex(phost, 0x008000);
    EVE_CoDl_clearTag(phost, EVE_TAG_NONE);
    EVE_CoDl_clear(phost, true, true, true);
    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);
    EVE_CoCmd_graphicsFinish(phost);
    EVE_Cmd_waitFlush(phost);

    EVE_CoCmd_dlStart(phost);
    EVE_CoDl_clearColorRgb_ex(phost, 0x000080);
    EVE_CoDl_clearTag(phost, EVE_TAG_NONE);
    EVE_CoDl_clear(phost, true, true, true);
    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);
    EVE_CoCmd_graphicsFinish(phost);
    EVE_CoCmd_sync(phost);
    EVE_Cmd_waitFlush(phost);

    /* Detect which buffer is front by reading back the test color */
    uint32_t test_pixel = EVE_Hal_rd32(phost, drvr->frame_buffer_0);
    if((test_pixel & 0xFFFFFF) == 0x008000) {
        drvr->current_fb = 1;
    }
    else if((test_pixel & 0xFFFFFF) == 0x000080) {
        drvr->current_fb = 0;
    }
    else {
        eve_printf("Warning: Unable to determine current framebuffer state\n");
    }

    return disp;
}

EVE_HalContext * lv_eve5_get_hal(lv_display_t * disp)
{
    lv_eve5_driver_t * drvr = lv_display_get_driver_data(disp);
    return drvr ? drvr->hal : NULL;
}

Esd_GpuAlloc * lv_eve5_get_allocator(lv_display_t * disp)
{
    lv_eve5_driver_t * drvr = lv_display_get_driver_data(disp);
    return drvr ? drvr->allocator : NULL;
}

#if LV_USE_OS
void lv_eve5_hal_lock(lv_display_t * disp)
{
    lv_eve5_driver_t * drvr = lv_display_get_driver_data(disp);
    if(drvr) lv_mutex_lock(&drvr->hal_mutex);
}

void lv_eve5_hal_unlock(lv_display_t * disp)
{
    lv_eve5_driver_t * drvr = lv_display_get_driver_data(disp);
    if(drvr) lv_mutex_unlock(&drvr->hal_mutex);
}
#endif

bool lv_eve5_detach_gpu_handle(lv_draw_buf_t * buf, Esd_GpuHandle *out_handle)
{
    if(buf == NULL || buf->vram_res == NULL) return false;
    lv_eve5_vram_res_t * vr = (lv_eve5_vram_res_t *)buf->vram_res;
    *out_handle = vr->gpu_handle;
    lv_free(vr);
    buf->vram_res = NULL;
    return true;
}

/**********************
 * STATIC FUNCTIONS
 **********************/

static void invalidate_area_cb(lv_event_t * e)
{
    lv_area_t * area = lv_event_get_invalidated_area(e);
    if(area == NULL) return;

    lv_display_t * disp = lv_event_get_target(e);
    area->x1 = LV_MAX(area->x1 - 1, 0);
    area->y1 = LV_MAX(area->y1 - 1, 0);
    area->x2 = LV_MIN(area->x2 + 1, lv_display_get_horizontal_resolution(disp) - 1);
    area->y2 = LV_MIN(area->y2 + 1, lv_display_get_vertical_resolution(disp) - 1);
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    lv_eve5_driver_t * drvr = lv_display_get_driver_data(disp);

    if(drvr == NULL || drvr->hal == NULL) {
        lv_display_flush_ready(disp);
        return;
    }

#if LV_USE_OS
    lv_mutex_lock(&drvr->hal_mutex);
#endif

    EVE_HalContext *phost = drvr->hal;
    lv_display_t * disp_refr = lv_refr_get_disp_refreshing();
    lv_layer_t * layer = disp_refr ? disp_refr->layer_head : NULL;

    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);

    if(drvr->pending_count >= MAX_REGIONS) {
        LV_LOG_WARN("EVE5: Max regions reached in flush, dropping frame segment");
#if LV_USE_OS
        lv_mutex_unlock(&drvr->hal_mutex);
#endif
        lv_display_flush_ready(disp);
        return;
    }

    /* Transfer ownership: if draw_buf has VRAM backing, detach the handle
     * so lv_draw_buf_destroy doesn't double-free. */
    Esd_GpuHandle handle = GA_HANDLE_INVALID;
    bool is_gpu_rendered = false;

    if(layer && layer->draw_buf) {
        is_gpu_rendered = lv_eve5_detach_gpu_handle(layer->draw_buf, &handle);
    }

    if(!is_gpu_rendered) {
        /* SW path: copy px_map (system RAM) to VRAM */
        uint32_t size = (w * h) * SW_BYTES_PER_PIXEL;

        handle = Esd_GpuAlloc_Alloc(drvr->allocator, size, GA_ALIGN_4);
        uint32_t gpu_addr = Esd_GpuAlloc_Get(drvr->allocator, handle);

        if(gpu_addr != GA_INVALID) {
            EVE_Hal_wrMem(phost, gpu_addr, px_map, size);
        }
        else {
            LV_LOG_ERROR("EVE5: OOM in flush_cb SW path");
#if LV_USE_OS
            lv_mutex_unlock(&drvr->hal_mutex);
#endif
            lv_display_flush_ready(disp);
            return;
        }
    }

    /* Sentinel: handle is GA_HANDLE_INVALID only if neither GPU nor SW path assigned it */
    if(handle.Id != GA_HANDLE_INVALID.Id) {
        drvr->pending_regions[drvr->pending_count].handle = handle;
        drvr->pending_regions[drvr->pending_count].area = *area;
        drvr->pending_regions[drvr->pending_count].is_gpu_rendered = is_gpu_rendered;
        drvr->pending_count++;
    }

    /* Mark tile buffer stale to prevent uploading old data on draw unit switch */
    if(layer && layer->draw_buf) {
        lv_draw_buf_set_flag(layer->draw_buf, LV_IMAGE_FLAGS_DISCARDABLE);
    }

    if(lv_display_flush_is_last(disp)) {
        composite_to_framebuffer(drvr);
    }

#if LV_USE_OS
    lv_mutex_unlock(&drvr->hal_mutex);
#endif

    lv_display_flush_ready(disp);
}

static void composite_to_framebuffer(lv_eve5_driver_t * drvr)
{
    EVE_HalContext *phost = drvr->hal;

    uint32_t read_fb = drvr->current_fb == 0 ? drvr->frame_buffer_1 : drvr->frame_buffer_0;

    EVE_CoCmd_renderTarget(phost, SWAPCHAIN_0, FB_BITMAP_FORMAT, phost->Width, phost->Height);
    EVE_CoCmd_dlStart(phost);
    EVE_CoDl_bitmapHandle(phost, EVE_CO_SCRATCH_HANDLE);
    EVE_CoDl_blendFunc(phost, ONE, ZERO);
    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
    EVE_CoDl_scissorXY(phost, 0, 0);
    EVE_CoDl_scissorSize(phost, phost->Width, phost->Height);
    EVE_CoDl_vertexTranslateX(phost, 0);
    EVE_CoDl_vertexTranslateY(phost, 0);

    /* Blit previous framebuffer as base (preserves unchanged areas) */
    EVE_CoDl_bitmapSource(phost, read_fb);
    EVE_CoDl_bitmapLayout(phost, FB_BITMAP_FORMAT,
                          phost->Width * FB_BYTES_PER_PIXEL, phost->Height);
    EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER,
                        phost->Width, phost->Height);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, 0, 0);
    EVE_CoDl_end(phost);

    /* Composite each updated region */
    for(int i = 0; i < drvr->pending_count; i++) {
        rendered_region_t * region = &drvr->pending_regions[i];
        uint32_t gpu_addr = Esd_GpuAlloc_Get(drvr->allocator, region->handle);

        if(gpu_addr == GA_INVALID) {
            LV_LOG_WARN("EVE5 composite: region %d handle invalid", i);
            continue;
        }

        int32_t w = lv_area_get_width(&region->area);
        int32_t h = lv_area_get_height(&region->area);

        EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
        EVE_CoDl_bitmapSource(phost, gpu_addr);

        if(region->is_gpu_rendered) {
            /* HW path: ARGB8 with 16-pixel aligned stride */
            int32_t aligned_w = (w + 15) & ~15;
            EVE_CoDl_bitmapLayout(phost, HW_BITMAP_FORMAT,
                                  aligned_w * HW_BYTES_PER_PIXEL, h);
        }
        else {
            /* SW path: tightly packed */
            EVE_CoDl_bitmapLayout(phost, EVE_SW_BITMAP_FORMAT,
                                  w * SW_BYTES_PER_PIXEL, h);
        }

        EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, w, h);
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, region->area.x1, region->area.y1);
        EVE_CoDl_end(phost);
    }

    EVE_CoDl_blendFunc_default(phost);
    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);
    EVE_CoCmd_graphicsFinish(phost);

    EVE_CmdSync sync = EVE_Cmd_sync(phost);

    drvr->current_fb = (drvr->current_fb == 0) ? 1 : 0;

    /* Queue region textures for deferred free */
    for(int i = 0; i < drvr->pending_count; i++) {
        Esd_GpuAlloc_DeferredFree(drvr->allocator,
                                  drvr->pending_regions[i].handle, sync);
    }
    drvr->pending_count = 0;

    /* Wait for previous frame before reusing its resources */
    if(drvr->last_frame_sync != EVE_CMD_SYNC_INVALID) {
        EVE_Cmd_waitSync(phost, drvr->last_frame_sync);
    }
    drvr->last_frame_sync = sync;

    /* Process completed deferred frees */
    EVE_CmdSync completed = EVE_Cmd_syncCompleted(phost);
    Esd_GpuAlloc_UpdateFree(drvr->allocator,
                            completed); /* FIXME: Idle callback in EVE_Cmd_syncCompleted should handle this (running out of sync ids?) */
}

static void wait_cb(lv_display_t * disp)
{
    LV_UNUSED(disp);
    /* Async rendering, no blocking wait needed */
}

#endif /* LV_USE_EVE5 */
