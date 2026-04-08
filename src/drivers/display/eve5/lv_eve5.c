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
#if LV_USE_DRAW_EVE5
#include "../../../draw/eve5/lv_draw_eve5.h"
#endif

/*********************
 * DEFINES
 *********************/

#define MAX_REGIONS 128

/**********************
 * TYPEDEFS
 **********************/

typedef struct
{
    Esd_GpuHandle handle; // GPU memory handle for this region
    lv_area_t area;       // Screen coordinates
    bool is_gpu_rendered; // Rendered by GPU (ARGB8) vs CPU (RGB565/RGB8/ARGB8)
} rendered_region_t;

typedef struct
{
    EVE_HalContext *hal;
    Esd_GpuAlloc *allocator;
    uint32_t frame_buffer_0;
    uint32_t frame_buffer_1;
    uint32_t current_fb; // 0 or 1 - which buffer we're rendering to
    rendered_region_t pending_regions[MAX_REGIONS];
    int pending_count;
    EVE_CmdSync last_frame_sync;
#if LV_USE_OS
    lv_mutex_t hal_mutex; // Serializes all SPI bus access (HAL reads/writes, coprocessor commands)
#endif
} lv_eve5_driver_t;

/**********************
 * STATIC PROTOTYPES
 **********************/

/* Display callbacks */
static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
static void wait_cb(lv_display_t *disp);
static void invalidate_area_cb(lv_event_t *e);
static void composite_to_framebuffer(lv_eve5_driver_t *drvr);

/**********************
 * STATIC VARIABLES
 **********************/

/**********************
 * MACROS
 **********************/

/* Determine bitmap format and bytes per pixel based on LVGL color depth for SW Render */
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

/* Framebuffer format is RGB8 (3 bytes per pixel) - The "Screen" memory */
#define FB_BYTES_PER_PIXEL 3
#define FB_BITMAP_FORMAT RGB8

/* Hardware renders (From Draw Unit) are always ARGB8 */
#define HW_BYTES_PER_PIXEL 4
#define HW_BITMAP_FORMAT ARGB8

/**********************
 * GLOBAL FUNCTIONS
 **********************/

/*--------------------
 * Display
 *--------------------*/

lv_display_t *lv_eve5_create(EVE_HalContext *hal, Esd_GpuAlloc *allocator)
{
    EVE_HalContext *phost = hal;

    lv_display_t *disp = lv_display_create(phost->Width, phost->Height);
    if (disp == NULL)
        return NULL;

    /* Allocate driver data */
    lv_eve5_driver_t *drvr = lv_malloc_zeroed(sizeof(lv_eve5_driver_t));
    if (drvr == NULL)
    {
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

    /* Expand invalidated areas by 1px to account for EVE AA fringe bleed.
     * Without this, sub-pixel AA at primitive edges can leave ghost pixels
     * outside the redrawn area when objects disappear (e.g., blinking cursor). */
    lv_display_add_event_cb(disp, invalidate_area_cb, LV_EVENT_INVALIDATE_AREA, NULL);

    /* Allocate render buffer for LVGL - use partial rendering */
    /* 64 lines buffer is a good balance for RAM usage vs Draw Call overhead */
    uint32_t buf_size = phost->Width * 64; 
    void *buf1 = lv_malloc(buf_size * sizeof(lv_color_t));

    if (buf1 == NULL)
    {
        lv_free(drvr);
        lv_display_delete(disp);
        return NULL;
    }

    lv_display_set_buffers(disp, buf1, NULL, buf_size * sizeof(lv_color_t),
        LV_DISPLAY_RENDER_MODE_PARTIAL);

    /* The HAL has already allocated two framebuffers for the swapchain */
    drvr->frame_buffer_0 = EVE_Hal_rd32(phost, REG_SC0_PTR0);
    EVE_Hal_wr32(phost, REG_SC0_PTR1, drvr->frame_buffer_0); /* TEMPORARY: Force single buffer for testing */
    drvr->frame_buffer_1 = EVE_Hal_rd32(phost, REG_SC0_PTR1);
    drvr->current_fb = 0;

    /* Initialize allocator */
    allocator->TotalMemorySize = RAM_G_SIZE;
    allocator->TotalReserved = RAM_G_SIZE - RAM_G_AVAILABLE;
    Esd_GpuAlloc_SetGen(allocator, EVE_GEN);
    Esd_GpuAlloc_Reset(allocator);

    // TODO: Maybe REG_SC0_RESET
    // ... need to probably reset swapchain on coprocessor error in case a SWAP gets missed...

    /* Initial clear of BOTH framebuffers */
    EVE_CoCmd_dlStart(phost);
    EVE_CoDl_clearColorRgb_ex(phost, 0x008000); // Dark green
    EVE_CoDl_clearTag(phost, EVE_TAG_NONE);
    EVE_CoDl_clear(phost, true, true, true);
    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);
    EVE_CoCmd_graphicsFinish(phost);
    EVE_Cmd_waitFlush(phost);

    EVE_CoCmd_dlStart(phost);
    EVE_CoDl_clearColorRgb_ex(phost, 0x000080); // Dark blue
    EVE_CoDl_clearTag(phost, EVE_TAG_NONE);
    EVE_CoDl_clear(phost, true, true, true);
    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);
    EVE_CoCmd_graphicsFinish(phost);
    EVE_CoCmd_sync(phost);
    EVE_Cmd_waitFlush(phost);

    // We can track a pixel to check which buffer is which
    uint32_t test_pixel = EVE_Hal_rd32(phost, drvr->frame_buffer_0);
    if ((test_pixel & 0xFFFFFF) == 0x008000)
    {
        drvr->current_fb = 1;
    }
    else if ((test_pixel & 0xFFFFFF) == 0x000080)
    {
        drvr->current_fb = 0;
    }
    else
    {
        eve_printf("Warning: Unable to determine current framebuffer state\n");
    }

    return disp;
}

EVE_HalContext *lv_eve5_get_hal(lv_display_t *disp)
{
    lv_eve5_driver_t *drvr = lv_display_get_driver_data(disp);
    return drvr ? drvr->hal : NULL;
}

Esd_GpuAlloc *lv_eve5_get_allocator(lv_display_t *disp)
{
    lv_eve5_driver_t *drvr = lv_display_get_driver_data(disp);
    return drvr ? drvr->allocator : NULL;
}

#if LV_USE_OS
void lv_eve5_hal_lock(lv_display_t *disp)
{
    lv_eve5_driver_t *drvr = lv_display_get_driver_data(disp);
    if(drvr) lv_mutex_lock(&drvr->hal_mutex);
}

void lv_eve5_hal_unlock(lv_display_t *disp)
{
    lv_eve5_driver_t *drvr = lv_display_get_driver_data(disp);
    if(drvr) lv_mutex_unlock(&drvr->hal_mutex);
}
#endif

/**********************
 * STATIC FUNCTIONS
 **********************/

/*--------------------
 * Display Callbacks
 *--------------------*/

static void invalidate_area_cb(lv_event_t *e)
{
    lv_area_t *area = lv_event_get_invalidated_area(e);
    if(area == NULL) return;

    /* Expand by 1px in each direction to cover EVE AA fringe */
    lv_display_t *disp = lv_event_get_target(e);
    area->x1 = LV_MAX(area->x1 - 1, 0);
    area->y1 = LV_MAX(area->y1 - 1, 0);
    area->x2 = LV_MIN(area->x2 + 1, lv_display_get_horizontal_resolution(disp) - 1);
    area->y2 = LV_MIN(area->y2 + 1, lv_display_get_vertical_resolution(disp) - 1);
}

static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    lv_eve5_driver_t *drvr = lv_display_get_driver_data(disp);

    if (drvr == NULL || drvr->hal == NULL)
    {
        lv_display_flush_ready(disp);
        return;
    }

#if LV_USE_OS
    lv_mutex_lock(&drvr->hal_mutex);
#endif

    EVE_HalContext *phost = drvr->hal;
    lv_display_t *disp_refr = lv_refr_get_disp_refreshing();
    lv_layer_t *layer = disp_refr ? disp_refr->layer_head : NULL;

	// LOG_DEBUG_MEMORY: printf("Flush layer 0x%p area (%d,%d)-(%d,%d)\n",
	// LOG_DEBUG_MEMORY:        layer, area->x1, area->y1, area->x2, area->y2);

    /* Calculate area dimensions */
    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);
    
    /* Check if we have room for more regions */
    if (drvr->pending_count >= MAX_REGIONS)
    {
        LV_LOG_WARN("EVE5: Max regions reached in flush, dropping frame segment");
#if LV_USE_OS
        lv_mutex_unlock(&drvr->hal_mutex);
#endif
        lv_display_flush_ready(disp);
        return;
    }

    /* TRANSFER OF OWNERSHIP:
     * If the layer's draw_buf has VRAM backing, the Draw Unit produced a GPU texture.
     * Detach the handle so lv_draw_buf_destroy doesn't double-free.
     */
    Esd_GpuHandle handle = GA_HANDLE_INVALID;
    bool is_gpu_rendered = false;

#if LV_USE_DRAW_EVE5
    if(layer && layer->draw_buf) {
        is_gpu_rendered = lv_draw_eve5_detach_gpu_handle(layer->draw_buf, &handle);
    }
#endif
    else
    {
        /* Software path: LVGL rendered to px_map (System RAM), copy to VRAM */
        uint32_t size = (w * h) * SW_BYTES_PER_PIXEL;
        
        handle = Esd_GpuAlloc_Alloc(drvr->allocator, size, GA_ALIGN_4);
        uint32_t gpu_addr = Esd_GpuAlloc_Get(drvr->allocator, handle);
        
        if (gpu_addr != GA_INVALID)
        {
            /* Copy LVGL rendered data to GPU memory */
            EVE_Hal_wrMem(phost, gpu_addr, px_map, size);
        }
        else
        {
            LV_LOG_ERROR("EVE5: OOM in flush_cb SW path");
#if LV_USE_OS
            lv_mutex_unlock(&drvr->hal_mutex);
#endif
            lv_display_flush_ready(disp);
            return;
        }
    }

    /* Store this region for compositing */
    if (handle.Id != GA_HANDLE_INVALID.Id)
    {
        drvr->pending_regions[drvr->pending_count].handle = handle;
        drvr->pending_regions[drvr->pending_count].area = *area;
        drvr->pending_regions[drvr->pending_count].is_gpu_rendered = is_gpu_rendered;
        drvr->pending_count++;
    }

    /* Check if this is the last region for this frame */
    if (lv_display_flush_is_last(disp))
    {
        composite_to_framebuffer(drvr);
    }

#if LV_USE_OS
    lv_mutex_unlock(&drvr->hal_mutex);
#endif

    /* Tell LVGL we're done with this buffer slice */
    lv_display_flush_ready(disp);
}

static void composite_to_framebuffer(lv_eve5_driver_t *drvr)
{
    EVE_HalContext *phost = drvr->hal;

    /* Determine which buffer to read from (previous) and write to (current) */
    uint32_t read_fb = drvr->current_fb == 0 ? drvr->frame_buffer_1 : drvr->frame_buffer_0;

    /* Restore render target to the current swapchain buffer */
    EVE_CoCmd_renderTarget(phost, SWAPCHAIN_0, FB_BITMAP_FORMAT, phost->Width, phost->Height);

    /* Start a new display list for the final composition */
    EVE_CoCmd_dlStart(phost);

    /* Use scratch bitmap handle for compositing operations */
    EVE_CoDl_bitmapHandle(phost, EVE_CO_SCRATCH_HANDLE);

    /* Hard blend: We just want to overwrite pixels, not mix with background */
    EVE_CoDl_blendFunc(phost, ONE, ZERO);

    /* Reset context */
    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF); /* Full opacity white */
    EVE_CoDl_scissorXY(phost, 0, 0);
    EVE_CoDl_scissorSize(phost, phost->Width, phost->Height);
    EVE_CoDl_vertexTranslateX(phost, 0);
    EVE_CoDl_vertexTranslateY(phost, 0);

    /* 1. Blit the PREVIOUS framebuffer as the base */
    /* This ensures partial updates work by preserving unchanged areas */
    EVE_CoDl_bitmapSource(phost, read_fb);
    EVE_CoDl_bitmapLayout(phost, FB_BITMAP_FORMAT,
        phost->Width * FB_BYTES_PER_PIXEL, phost->Height);
    EVE_CoDl_bitmapSize(phost, NEAREST,
        BORDER, BORDER,
        phost->Width, phost->Height);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, 0, 0);
    EVE_CoDl_end(phost);

    /* 2. Composite each updated region on top */
    for (int i = 0; i < drvr->pending_count; i++)
    {
        rendered_region_t *region = &drvr->pending_regions[i];
        uint32_t gpu_addr = Esd_GpuAlloc_Get(drvr->allocator, region->handle);

        if (gpu_addr == GA_INVALID) continue;

        int32_t w = lv_area_get_width(&region->area);
        int32_t h = lv_area_get_height(&region->area);

        /* Reset color to white (draw texture as-is) */
        EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);

        EVE_CoDl_bitmapSource(phost, gpu_addr);

        if (region->is_gpu_rendered)
        {
            // printf("Render from GPU\n");
            /* HW Path: ARGB8, 16-pixel aligned width */
            /* Note: Draw Unit creates stride = ALIGN_UP(w, 16) * 4 */
            int32_t aligned_w = (w + 15) & ~15;  // Must be 16-pixel aligned for BT820
            EVE_CoDl_bitmapLayout(phost, HW_BITMAP_FORMAT,
                aligned_w * HW_BYTES_PER_PIXEL, h);
        }
        else
        {
            // printf("Render from software\n");
            /* SW Path: Format depends on LV_COLOR_DEPTH, tightly packed */
            EVE_CoDl_bitmapLayout(phost, EVE_SW_BITMAP_FORMAT,
                w * SW_BYTES_PER_PIXEL, h);
        }

        EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, w, h);

        /* Draw at absolute screen coordinates */
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, region->area.x1, region->area.y1);
        EVE_CoDl_end(phost);
    }

    /* Restore default blend func */
    EVE_CoDl_blendFunc_default(phost);
    
    /* Finish display list and swap */
    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);
    EVE_CoCmd_graphicsFinish(phost);

    /* Get sync marker for deferred free */
    EVE_CmdSync sync = EVE_Cmd_sync(phost);

    /* Toggle current buffer index */
    drvr->current_fb = (drvr->current_fb == 0) ? 1 : 0;

    /* Queue all pending region textures for deferred free */
    for (int i = 0; i < drvr->pending_count; i++)
    {
        Esd_GpuAlloc_DeferredFree(drvr->allocator,
            drvr->pending_regions[i].handle,
            sync);
    }
    drvr->pending_count = 0;

    /* Wait for composition to complete (optional - can remove now for async) */
    // EVE_Cmd_waitFlush(phost);

    /* Wait for previous frame to complete */
    if (drvr->last_frame_sync != EVE_CMD_SYNC_INVALID) {
		EVE_Cmd_waitSync(phost, drvr->last_frame_sync);
    }
	drvr->last_frame_sync = sync;

    /* Process completed deferred frees (frame end) */
	EVE_CmdSync completed = EVE_Cmd_syncCompleted(phost); /* This triggers UpdateFree in the Idle callback if applicable */
	Esd_GpuAlloc_UpdateFree(drvr->allocator, completed); /* FIXME: Check why Idle in EVE_Cmd_syncCompleted is not calling this consistently */
}

static void wait_cb(lv_display_t *disp)
{
    lv_eve5_driver_t *drvr = lv_display_get_driver_data(disp);

    if (drvr == NULL || drvr->hal == NULL)
    {
        return;
    }

    /* Wait for any pending EVE rendering operations to complete */
    // EVE_Cmd_waitFlush(drvr->hal);
}

#endif /* LV_USE_EVE5 */
