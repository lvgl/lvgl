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
    /* Source format and stride captured at flush time. The HW path's tile VRAM
     * format follows the layer's color format (typically the display's natural
     * cf — RGB565 for LV_COLOR_DEPTH=16, RGB8 for 24, ARGB8 for 32). The SW
     * path goes through px_map in EVE_SW_BITMAP_FORMAT. The compositor must
     * sample each region with its actual format/stride or it'll read garbage. */
    uint16_t eve_format;
    uint32_t eve_stride;
    bool is_gpu_rendered;
} rendered_region_t;

typedef struct {
    EVE_HalContext * hal;
    Esd_GpuAlloc * allocator;
    /* frame_buffer_0/1 mirror the *active* swapchain config (what the
     * coprocessor will sample on the next CMD_SWAP). In FULL mode they're the
     * two original HAL-boot buffers (proper double buffer). In PARTIAL mode
     * they're equal, both pointing at frame_buffer_0_orig (single-buffer
     * workaround). The compositor reads from one and writes the back via
     * SWAPCHAIN_0 — keeping these in sync with REG_SC0_PTR1 ensures the read
     * targets a buffer the swapchain actually scans out from. */
    uint32_t frame_buffer_0;
    uint32_t frame_buffer_1;
    /* Original HAL-boot PTR1 — preserved so FULL mode can restore the
     * double-buffered configuration after a partial-mode override. */
    uint32_t frame_buffer_1_orig;
    uint32_t current_fb;
    rendered_region_t pending_regions[MAX_REGIONS];
    int pending_count;
    EVE_CmdSync last_frame_sync;
    /* Set by lv_eve5_record_frame_sync when the EVE5 draw unit's finish_layer
     * runs on the swapchain (FULL HW path). flush_cb FULL-mode reads + clears
     * it to decide between HW (already swapped — just reclaim deferred frees)
     * and SW (need to upload px_map and CMD_SWAP ourselves). Stays false when
     * the draw unit is disabled at runtime, so the screen layer falls through
     * to LVGL's SW renderer instead. */
    bool full_frame_hw_rendered;
    /* Set by lv_eve5_request_invalidate when a deferred screen invalidate is
     * needed (typically because frame-time alloc failures became unblocked by
     * a post-frame GC sweep). Consumed and cleared by the LV_EVENT_REFR_READY
     * handler once LVGL is out of its rendering pass and lv_obj_invalidate is
     * legal again. */
    bool invalidate_pending;
    lv_eve5_render_mode_t render_mode;
    /* Both draw buffers are created at init and kept alive for the display's
     * lifetime so mode switching is allocation-free. Only one is bound to the
     * display via lv_display_set_draw_buffers at any given time. */
    lv_draw_buf_t * tile_buf;       /**< Partial mode: small W×64 tile, RGB565 (or LV_COLOR_DEPTH-derived) */
    lv_draw_buf_t * full_buf;       /**< Full mode: virtual W×H, RGB8, vram_res pre-attached with is_swapchain=true */
    lv_color_format_t partial_cf;   /**< Color format used in partial mode (display's natural format) */
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
static void refr_ready_cb(lv_event_t * e);
static void composite_to_framebuffer(lv_eve5_driver_t * drvr);
static void full_mode_sw_present(lv_eve5_driver_t * drvr, const lv_area_t * area, const uint8_t * px_map);
static lv_draw_buf_t * create_tile_buf(EVE_HalContext * phost, lv_color_format_t cf);
static lv_draw_buf_t * create_full_buf(EVE_HalContext * phost);
static void apply_render_mode(lv_display_t * disp, lv_eve5_render_mode_t mode);

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

/**********************
 * GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_eve5_create(EVE_HalContext *hal, Esd_GpuAlloc *allocator)
{
    return lv_eve5_create_ex(hal, allocator, LV_EVE5_RENDER_MODE_PARTIAL);
}

lv_display_t * lv_eve5_create_ex(EVE_HalContext *hal, Esd_GpuAlloc *allocator,
                                 lv_eve5_render_mode_t mode)
{
    EVE_HalContext *phost = hal;

    lv_display_t * disp = lv_display_create(phost->Width, phost->Height);
    if(disp == NULL)
        return NULL;

    /* Chips without render-target support can't run the partial-mode tile
     * compositor (no SWAPCHAIN_0 / no CMD_RENDERTARGET). Lock to FULL mode so
     * the EVE5 draw unit's non-RT path renders the screen as a single DL. */
    if(!EVE_Hal_supportRenderTarget(hal) && mode != LV_EVE5_RENDER_MODE_FULL) {
        LV_LOG_INFO("EVE5: Forcing FULL render mode (no render-target support)");
        mode = LV_EVE5_RENDER_MODE_FULL;
    }

    lv_eve5_driver_t * drvr = lv_malloc_zeroed(sizeof(lv_eve5_driver_t));
    if(drvr == NULL) {
        lv_display_delete(disp);
        return NULL;
    }
    drvr->hal = hal;
    drvr->allocator = allocator;
    drvr->pending_count = 0;
    drvr->last_frame_sync = EVE_CMD_SYNC_INVALID;
    drvr->render_mode = LV_EVE5_RENDER_MODE_PARTIAL; /* set by apply_render_mode below */
    drvr->partial_cf = lv_display_get_color_format(disp);
#if LV_USE_OS
    lv_mutex_init(&drvr->hal_mutex);
#endif

    lv_display_set_driver_data(disp, drvr);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_flush_wait_cb(disp, wait_cb);

    /* Expand invalidated areas by 1px to cover EVE's AA fringe bleed */
    lv_display_add_event_cb(disp, invalidate_area_cb, LV_EVENT_INVALIDATE_AREA, NULL);

    /* Drain deferred-invalidate requests from the draw unit after each refresh */
    lv_display_add_event_cb(disp, refr_ready_cb, LV_EVENT_REFR_READY, NULL);

    /* Create both draw buffers up front. Only one is active at a time (selected
     * by render mode). Keeping both around avoids alloc/free churn on mode switch. */
    drvr->tile_buf = create_tile_buf(phost, drvr->partial_cf);
    drvr->full_buf = create_full_buf(phost);
    if(drvr->tile_buf == NULL || drvr->full_buf == NULL) {
        if(drvr->tile_buf) lv_free(drvr->tile_buf);
        if(drvr->full_buf) {
            if(drvr->full_buf->vram_res) lv_free(drvr->full_buf->vram_res);
            lv_free(drvr->full_buf);
        }
        lv_free(drvr);
        lv_display_delete(disp);
        return NULL;
    }

    /* Read the swapchain buffer addresses set up by HAL bootup. frame_buffer_0
     * and frame_buffer_1_orig are the canonical double-buffered values;
     * frame_buffer_1 mirrors the *active* PTR1 (which apply_render_mode may
     * override to PTR0 in partial mode). The compositor reads frame_buffer_0/1
     * to find the previous front buffer, so they must reflect what the
     * swapchain actually points at — otherwise the compositor reads from a
     * buffer the scanout never sources from.
     *
     * Mode-specific swapchain config:
     *   - FULL: leaves PTR1 at frame_buffer_1_orig (proper double buffering — tear-free).
     *   - PARTIAL: overrides PTR1 = PTR0 to force single-buffer scanout. This is
     *     because the partial-mode compositor needs to read the "previous front"
     *     buffer as the base for compositing the next frame, but EVE provides no
     *     reliable way to identify which swapchain buffer is the live scanout
     *     after a CMD_SWAP. The natural alternative — pointing REG_SO_SOURCE at
     *     a fixed memory address rather than SWAPCHAIN_0 — would sidestep the
     *     detection problem, but in the emulator that path does not correctly
     *     trigger the render, so we emulate the fixed-address behavior through
     *     the swapchain by making PTR0 and PTR1 alias the same buffer. Proper
     *     double-buffered partial mode would require encoding a per-frame
     *     sentinel into the rendered output so the compositor can detect which
     *     buffer was last rendered (and thus which is the previous front).
     */
    /* SC0 swapchain registers only exist on chips with render-target support.
     * On EVE1-EVE4 the swap mechanism is just CMD_SWAP rotating DL banks;
     * no per-buffer pointers to track. Leave frame_buffer_* at zero (already
     * zeroed by lv_malloc_zeroed) so the partial-mode compositor paths —
     * which we never enter without RT support — can't read anything
     * meaningful from them. */
#ifdef EVE_SUPPORT_RENDERTARGET
    if(EVE_Hal_supportRenderTarget(phost)) {
        drvr->frame_buffer_0 = EVE_Hal_rd32(phost, REG_SC0_PTR0);
        drvr->frame_buffer_1_orig = EVE_Hal_rd32(phost, REG_SC0_PTR1);
        drvr->frame_buffer_1 = drvr->frame_buffer_1_orig; /* updated by apply_render_mode below */
    }
#endif
    drvr->current_fb = 0;

    /* Bind initial buffer + apply swapchain register config for the requested mode */
    apply_render_mode(disp, mode);

    /* Stash the display on the HAL's user context so lv_draw_eve5_init can reach
     * back to the display when given just the HAL pointer. The EVE5 driver
     * assumes a 1:1 hal:display:draw_unit relationship — UserContext is owned by
     * lv_eve5 for that purpose. */
    if(phost->UserContext != NULL && phost->UserContext != disp) {
        LV_LOG_WARN("EVE5: phost->UserContext already set to %p; overwriting with display %p",
                    phost->UserContext, (void *)disp);
    }
    phost->UserContext = disp;

    /* Initialize GPU allocator */
    allocator->TotalMemorySize = RAM_G_SIZE;
    allocator->TotalReserved = RAM_G_SIZE - RAM_G_AVAILABLE;
    Esd_GpuAlloc_SetGen(allocator, EVE_GEN);
    Esd_GpuAlloc_Reset(allocator);

    /* TODO: Handle REG_SC0_RESET on coprocessor error to avoid missed SWAPs */

    /* Clear both framebuffers with distinct colors to detect buffer order.
     * CMD_GRAPHICSFINISH is BT820-only; on previous gens CMD_SWAP itself
     * is blocking, so the followup waitFlush is sufficient sync. */
    EVE_CoCmd_dlStart(phost);
    EVE_CoDl_clearColorRgb_ex(phost, 0x008000);
    EVE_CoDl_clearTag(phost, EVE_TAG_NONE);
    EVE_CoDl_clear(phost, true, true, true);
    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);
    if(EVE_Hal_supportRenderTarget(phost)) EVE_CoCmd_graphicsFinish(phost);
    EVE_Cmd_waitFlush(phost);

    EVE_CoCmd_dlStart(phost);
    EVE_CoDl_clearColorRgb_ex(phost, 0x000080);
    EVE_CoDl_clearTag(phost, EVE_TAG_NONE);
    EVE_CoDl_clear(phost, true, true, true);
    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);
    if(EVE_Hal_supportRenderTarget(phost)) EVE_CoCmd_graphicsFinish(phost);
    EVE_Cmd_waitFlush(phost);

    /* Detect which buffer is front by reading back the test color. Only
     * meaningful on RT-capable chips with a real swapchain — on EVE3/EVE4
     * frame_buffer_0 is zero and the read would be garbage. */
    if(EVE_Hal_supportRenderTarget(phost)) {
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

bool lv_eve5_detach_gpu_handle(lv_draw_buf_t * buf, Esd_GpuHandle *out_handle,
                               uint16_t *out_format, uint32_t *out_stride)
{
    if(buf == NULL || buf->vram_res == NULL) return false;
    lv_eve5_vram_res_t * vr = (lv_eve5_vram_res_t *)buf->vram_res;
    /* Swapchain vram_res is owned by the driver (full_buf); refuse to detach.
     * Returning false routes the caller (e.g., partial-mode flush_cb) to its
     * SW path, which is the safe degraded behavior if a flush ever fires for
     * the full_buf. */
    if(vr->is_swapchain) return false;
    *out_handle = vr->gpu_handle;
    if(out_format) *out_format = vr->eve_format;
    if(out_stride) *out_stride = vr->stride;
    lv_free(vr);
    buf->vram_res = NULL;
    return true;
}

bool lv_eve5_set_render_mode(lv_display_t * disp, lv_eve5_render_mode_t mode)
{
    if(disp == NULL) return false;
    lv_eve5_driver_t * drvr = lv_display_get_driver_data(disp);
    if(drvr == NULL) return false;
    if(drvr->render_mode == mode) return true;

    /* PARTIAL mode requires CMD_RENDERTARGET for tile composition. Refuse the
     * switch on chips that don't support it — the caller should stick to FULL. */
    if(mode != LV_EVE5_RENDER_MODE_FULL && !EVE_Hal_supportRenderTarget(drvr->hal)) {
        LV_LOG_WARN("EVE5: PARTIAL render mode unsupported (no render-target capability)");
        return false;
    }

#if LV_USE_OS
    /* HAL mutex serializes us against the EVE5 draw unit. */
    lv_mutex_lock(&drvr->hal_mutex);
#endif
    /* Drain the entire command FIFO before reconfiguring swapchain registers.
     * waitFlush blocks until the coprocessor has processed every queued command,
     * including any in-flight CMD_SWAP / scanout pipeline state — stronger than
     * waiting on a specific sync marker, and the right tool for a one-off
     * config change like a mode toggle. */
    EVE_Cmd_waitFlush(drvr->hal);
    apply_render_mode(disp, mode);
#if LV_USE_OS
    lv_mutex_unlock(&drvr->hal_mutex);
#endif
    return true;
}

lv_eve5_render_mode_t lv_eve5_get_render_mode(lv_display_t * disp)
{
    if(disp == NULL) return LV_EVE5_RENDER_MODE_PARTIAL;
    lv_eve5_driver_t * drvr = lv_display_get_driver_data(disp);
    return drvr ? drvr->render_mode : LV_EVE5_RENDER_MODE_PARTIAL;
}

void lv_eve5_link_draw_unit(lv_display_t * disp, struct _lv_draw_unit_t * draw_unit)
{
    if(disp == NULL || draw_unit == NULL) return;
    lv_eve5_driver_t * drvr = lv_display_get_driver_data(disp);
    if(drvr == NULL) return;
    if(drvr->full_buf != NULL && drvr->full_buf->vram_res != NULL) {
        drvr->full_buf->vram_res->unit = (lv_draw_unit_t *)draw_unit;
    }
}

void lv_eve5_record_frame_sync(lv_display_t * disp, EVE_CmdSync sync)
{
    if(disp == NULL) return;
    lv_eve5_driver_t * drvr = lv_display_get_driver_data(disp);
    if(drvr == NULL) return;
    /* Already serialized by the draw unit's hal_lock around eve5_render_layer
     * (which contains finish_layer); no additional locking needed. */
    drvr->last_frame_sync = sync;
    drvr->full_frame_hw_rendered = true;
}

void lv_eve5_request_invalidate(lv_display_t * disp)
{
    if(disp == NULL) return;
    lv_eve5_driver_t * drvr = lv_display_get_driver_data(disp);
    if(drvr == NULL) return;
    drvr->invalidate_pending = true;
}

/**********************
 * STATIC FUNCTIONS
 **********************/

/**
 * Build the partial-mode tile draw buffer.
 *
 * Header-only initially; backing allocated on first dispatch:
 *   - EVE5 draw unit: VRAM only (via vram_alloc_cb)
 *   - SW fallback: CPU memory (lazy-allocated by LVGL)
 * DISCARDABLE flag set after each flush prevents stale uploads when the
 * tile is re-used across frames or after a mode switch.
 */
static lv_draw_buf_t * create_tile_buf(EVE_HalContext * phost, lv_color_format_t cf)
{
    uint32_t stride = lv_draw_buf_width_to_stride(phost->Width, cf);
    lv_draw_buf_t * buf = lv_malloc_zeroed(sizeof(lv_draw_buf_t));
    if(buf == NULL) return NULL;

    buf->header.magic = LV_IMAGE_HEADER_MAGIC;
    buf->header.w = phost->Width;
    buf->header.h = 64;
    buf->header.cf = cf;
    buf->header.stride = stride;
    buf->header.flags = LV_IMAGE_FLAGS_MODIFIABLE | LV_IMAGE_FLAGS_ALLOCATED;
    buf->data_size = stride * 64;
    buf->handlers = lv_draw_buf_get_handlers();
    return buf;
}

/**
 * Build the full-mode screen draw buffer.
 *
 * vram_res is pre-attached with is_swapchain=true and gpu_handle=GA_HANDLE_INVALID:
 *   - HW path: EVE5 draw unit's init_layer detects is_swapchain on the screen
 *     layer and issues EVE_CoCmd_renderTarget with SWAPCHAIN_0. The render
 *     engine resolves it to the current back buffer; finish_layer's CMD_SWAP
 *     flips scanout. data stays NULL.
 *   - SW path (set_enabled(false) at runtime): LVGL's ensure_resident takes
 *     the "different unit" migration branch, allocates CPU memory through
 *     LVGL's normal mechanism, calls our vram_download_cb (no-op for
 *     swapchain), and our vram_free_cb (no-op for swapchain — the swapchain
 *     descriptor is owned by the driver). The SW renderer renders into the
 *     allocated CPU buffer; flush_cb's FULL-mode SW branch uploads it to a
 *     temp VRAM and presents via SWAPCHAIN_0 + CMD_SWAP.
 *
 * Format is RGB8 to match the HAL-reserved swapchain. With LV_COLOR_DEPTH != 24,
 * applying this mode causes lv_display_set_color_format to retag the display
 * and screen layer to RGB888.
 */
static lv_draw_buf_t * create_full_buf(EVE_HalContext * phost)
{
    lv_draw_buf_t * buf = lv_malloc_zeroed(sizeof(lv_draw_buf_t));
    if(buf == NULL) return NULL;

    lv_eve5_vram_res_t * vr = lv_malloc_zeroed(sizeof(lv_eve5_vram_res_t));
    if(vr == NULL) {
        lv_free(buf);
        return NULL;
    }

    /* Stride matches what a CPU-side draw buffer would have (LVGL's canonical
     * RGB888 stride, including its alignment rules) so lv_draw_buf_reshape's
     * size check accepts the screen-sized layer and SW migration can allocate
     * a compatible CPU buffer. The physical SC0 buffer set up by HAL bootup
     * uses Width × 3 with no row alignment, but we never read the swapchain
     * via this stride — the render engine writes through SWAPCHAIN_0 and
     * intermediate prev_handle blits carry their own stride. */
    int32_t W = phost->Width;
    int32_t H = phost->Height;
    uint32_t stride = lv_draw_buf_width_to_stride((uint32_t)W, LV_COLOR_FORMAT_RGB888);
    uint32_t cpu_size = stride * (uint32_t)H;

    vr->base.unit = NULL; /* Set by lv_eve5_link_draw_unit on EVE5 draw unit init */
    vr->base.size = 0;    /* No GPU allocation — virtual */
    vr->gpu_handle = GA_HANDLE_INVALID;
    /* eve_format describes the swapchain/framebuffer pixel format. RGB8 is
     * BT820-only — pre-BT820 single-target builds don't have the symbol, and
     * the non-RT dispatch path renders directly via CMD_SWAP without
     * sampling this vram_res, so the value is just inert metadata. */
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
    vr->eve_format = RGB8;
#else
    vr->eve_format = RGB565;
#endif
    vr->stride = stride;
    vr->width = W;
    vr->height = H;
    vr->source_offset = 0;
    vr->palette_offset = GA_INVALID;
    vr->is_premultiplied = false;
    vr->has_content = false;
    vr->is_swapchain = true;

    buf->header.magic = LV_IMAGE_HEADER_MAGIC;
    buf->header.w = W;
    buf->header.h = H;
    buf->header.cf = LV_COLOR_FORMAT_RGB888;
    buf->header.stride = stride;
    /* MODIFIABLE so SW migration can allocate writable CPU memory.
     * ALLOCATED tells LVGL not to bypass our buffer in default paths. */
    buf->header.flags = LV_IMAGE_FLAGS_MODIFIABLE | LV_IMAGE_FLAGS_ALLOCATED;
    /* data_size mirrors what the CPU buffer would occupy. Required because
     * lv_draw_buf_reshape rejects shapes whose computed size exceeds data_size,
     * and lv_refr's layer_reshape_draw_buf calls reshape on every refresh.
     * For HW path, no CPU memory is actually allocated (data stays NULL). For
     * SW path, ensure_resident allocates CPU memory matching this size. */
    buf->data_size = cpu_size;
    buf->data = NULL;
    buf->handlers = lv_draw_buf_get_handlers();
    buf->vram_res = (lv_draw_buf_vram_res_t *)vr;
    return buf;
}

/**
 * Activate the requested render mode on the display.
 *
 * Updates the swapchain buffer config (REG_SC0_PTR1) and LVGL state (active
 * draw buffer, color format, render mode). When called at runtime, the caller
 * must already have synced past the last frame and held the HAL mutex so the
 * register write doesn't race in-flight rendering.
 *
 * Idempotent — setting the same mode twice rewrites the same values (cheap).
 */
static void apply_render_mode(lv_display_t * disp, lv_eve5_render_mode_t mode)
{
    lv_eve5_driver_t * drvr = lv_display_get_driver_data(disp);
    if(drvr == NULL) return;
    EVE_HalContext * phost = drvr->hal;

    if(mode == LV_EVE5_RENDER_MODE_FULL) {
        /* Proper double-buffered scanout on BT820+: PTR0 and PTR1 distinct.
         * SWAPCHAIN_0 magic-resolves to whichever is currently back; CMD_SWAP
         * rotates them. No tearing because scanout reads the front while
         * rendering writes the back.
         *
         * Non-RT chips (EVE1-EVE4) have no swapchain registers; CMD_SWAP just
         * rotates the DL bank that the GPU replays. Skip the register write. */
#ifdef EVE_SUPPORT_RENDERTARGET
        if(EVE_Hal_supportRenderTarget(phost)) {
            EVE_Hal_wr32(phost, REG_SC0_PTR1, drvr->frame_buffer_1_orig);
            drvr->frame_buffer_1 = drvr->frame_buffer_1_orig;
        }
#else
        (void)phost;
#endif

        lv_display_set_draw_buffers(disp, drvr->full_buf, NULL);
        /* set_color_format propagates to disp, layer_head, and the active
         * draw_buf's header.cf — sets things up for FULL render mode. */
        lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB888);
        lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_FULL);
    }
#ifdef EVE_SUPPORT_RENDERTARGET
    else {
        /* Single-buffered scanout: PTR1 = PTR0. The partial-mode compositor has
         * no reliable way to identify the live scanout buffer post-swap, so it
         * cannot safely pick "previous front" to read as base in a true
         * double-buffered swapchain. We emulate the fixed-scanout-address path
         * here (which would otherwise be REG_SO_SOURCE = some_addr — but that
         * path doesn't trigger the render correctly in the emulator). See
         * create_ex for the full explanation and the path forward.
         * frame_buffer_1 is mirrored to frame_buffer_0 so the compositor's
         * read_fb selection always lands on the active scanout buffer
         * regardless of which side of the toggle current_fb is on.
         *
         * PARTIAL mode requires render-target support — non-RT builds force
         * FULL in lv_eve5_create_ex, and REG_SC0_PTR1 isn't defined there. */
        EVE_Hal_wr32(phost, REG_SC0_PTR1, drvr->frame_buffer_0);
        drvr->frame_buffer_1 = drvr->frame_buffer_0;

        lv_display_set_draw_buffers(disp, drvr->tile_buf, NULL);
        lv_display_set_color_format(disp, drvr->partial_cf);
        lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_PARTIAL);

        /* Force a full-screen invalidation so the next refresh re-renders the
         * whole screen via partial tiles. After a FULL→PARTIAL switch the
         * scanout buffer holds the last full-mode frame; without this LVGL
         * would only re-render whatever happens to be invalidated next, leaving
         * stale full-mode pixels visible everywhere else. (At create time the
         * screen is empty, so this is effectively a no-op.) */
        lv_obj_t * scr = lv_display_get_screen_active(disp);
        if(scr != NULL) lv_obj_invalidate(scr);
    }
#endif
    drvr->render_mode = mode;
}

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

static void refr_ready_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_target(e);
    if(disp == NULL) return;
    lv_eve5_driver_t * drvr = lv_display_get_driver_data(disp);
    if(drvr == NULL || !drvr->invalidate_pending) return;

    drvr->invalidate_pending = false;

    /* LVGL has cleared rendering_in_progress by the time REFR_READY fires,
     * so lv_obj_invalidate is legal here. The screen will be marked dirty
     * for the next refresh cycle. */
    lv_obj_t * scr = lv_display_get_screen_active(disp);
    if(scr != NULL) {
        LV_LOG_INFO("EVE5: deferred screen invalidate (post-GC retry)");
        lv_obj_invalidate(scr);
    }
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    lv_eve5_driver_t * drvr = lv_display_get_driver_data(disp);

    if(drvr == NULL || drvr->hal == NULL) {
        lv_display_flush_ready(disp);
        return;
    }

    /* Full mode dispatches based on which path produced the frame:
     *   HW: lv_draw_eve5_hal_finish_layer rendered to SWAPCHAIN_0 and CMD_SWAP'd.
     *       lv_eve5_record_frame_sync set full_frame_hw_rendered. We just need
     *       to reclaim deferred VRAM frees the GPU has caught up with.
     *   SW: the EVE5 draw unit was disabled (or absent), the LVGL SW renderer
     *       wrote pixels into full_buf->data == px_map. We upload that bitmap
     *       to a temp VRAM, draw it into SWAPCHAIN_0, CMD_SWAP, and defer-free
     *       the temp.
     *
     * Sub-layer (opa_layered) draw_bufs go vram_free_cb → PendingFree →
     * FlushPending (via finish_layer's sync) → DeferredFree. The actual handle
     * reclamation only happens in UpdateFree, which PARTIAL does inside
     * composite_to_framebuffer; FULL has no compositor, so we call it here on
     * both HW and SW paths or VRAM exhausts after a few seconds of layered
     * content. */
    if(drvr->render_mode == LV_EVE5_RENDER_MODE_FULL) {
#if LV_USE_OS
        lv_mutex_lock(&drvr->hal_mutex);
#endif
        bool hw_rendered = drvr->full_frame_hw_rendered;
        drvr->full_frame_hw_rendered = false;

#ifdef EVE_SUPPORT_RENDERTARGET
        if(!hw_rendered && px_map != NULL) {
            /* SW path: upload the rendered bitmap and present it. */
            full_mode_sw_present(drvr, area, px_map);
        }
#else
        /* Non-RT chips: the EVE5 draw unit's non-RT dispatch always renders
         * directly to the implicit framebuffer via CMD_SWAP, so flush_cb
         * never sees a SW-rendered px_map needing upload. */
        (void)hw_rendered;
        (void)area;
        (void)px_map;
#endif

        EVE_CmdSync completed = EVE_Cmd_syncCompleted(drvr->hal);
        Esd_GpuAlloc_UpdateFree(drvr->allocator, completed);
#if LV_USE_OS
        lv_mutex_unlock(&drvr->hal_mutex);
#endif
        lv_display_flush_ready(disp);
        return;
    }

#ifdef EVE_SUPPORT_RENDERTARGET
    /* Partial-mode tile collection — render-target only. On non-RT chips
     * lv_eve5_create_ex forces FULL mode, so this code path is unreachable;
     * the symbols it uses (SWAPCHAIN_0, RGB8 framebuffer format, etc.) also
     * don't exist on those builds. */

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
     * so lv_draw_buf_destroy doesn't double-free. Capture the actual EVE
     * format and stride too — tile VRAM is allocated in the layer's color
     * format (RGB565 / RGB8 / ARGB8 depending on LV_COLOR_DEPTH and any
     * promotion flags), and the compositor must sample it with that format. */
    Esd_GpuHandle handle = GA_HANDLE_INVALID;
    uint16_t region_format = 0;
    uint32_t region_stride = 0;
    bool is_gpu_rendered = false;

    if(layer && layer->draw_buf) {
        is_gpu_rendered = lv_eve5_detach_gpu_handle(layer->draw_buf, &handle,
                                                    &region_format, &region_stride);
    }

    if(!is_gpu_rendered) {
        /* SW path: copy px_map (system RAM) to VRAM. The pixel format is the
         * display's native LVGL format (EVE_SW_BITMAP_FORMAT) packed tight. */
        uint32_t size = (w * h) * SW_BYTES_PER_PIXEL;

        handle = Esd_GpuAlloc_Alloc(drvr->allocator, size, GA_ALIGN_4);
        uint32_t gpu_addr = Esd_GpuAlloc_Get(drvr->allocator, handle);

        if(gpu_addr != GA_INVALID) {
            EVE_Hal_wrMem(phost, gpu_addr, px_map, size);
            EVE_Hal_requestFenceBeforeSwap(phost);
            region_format = EVE_SW_BITMAP_FORMAT;
            region_stride = (uint32_t)w * SW_BYTES_PER_PIXEL;
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
        drvr->pending_regions[drvr->pending_count].eve_format = region_format;
        drvr->pending_regions[drvr->pending_count].eve_stride = region_stride;
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

#endif /* EVE_SUPPORT_RENDERTARGET — partial-mode tile collection */

    lv_display_flush_ready(disp);
}

#ifdef EVE_SUPPORT_RENDERTARGET
/* Partial-mode tile compositor — render-target only. Uses CMD_RENDERTARGET,
 * SWAPCHAIN_0, CMD_GRAPHICSFINISH, and the BT820+ RGB8 framebuffer format,
 * none of which exist on non-RT builds. The PARTIAL render mode itself is
 * forced to FULL on those chips by lv_eve5_create_ex, so this function
 * would never be reached at runtime anyway. */
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

        /* Sample the region in its actual format/stride captured at flush time.
         * HW: layer-cf-derived (RGB565 for LV_COLOR_DEPTH=16, RGB8 for 24, ARGB8
         * for 32 or alpha-promoted layers), 16-px aligned stride from the EVE5
         * draw unit's vram_alloc_cb. SW: EVE_SW_BITMAP_FORMAT tightly packed. */
        EVE_CoDl_bitmapLayout(phost, (uint8_t)region->eve_format,
                              region->eve_stride, h);
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
#endif /* EVE_SUPPORT_RENDERTARGET — composite_to_framebuffer */

/**
 * Present a SW-rendered full-screen frame.
 *
 * Allocates a temp VRAM block, uploads px_map (full_buf->data, RGB888 packed),
 * draws it into SWAPCHAIN_0 with CMD_SWAP, and queues the temp for deferred
 * free. Caller holds the HAL mutex.
 *
 * Used when the EVE5 draw unit is disabled at runtime — the LVGL SW renderer
 * fills full_buf->data each frame and we hand the bitmap to the GPU here.
 *
 * Render-target gated: uses SWAPCHAIN_0 and the BT820+ RGB8 framebuffer
 * format. On non-RT chips the EVE5 draw unit's non-RT dispatch handles the
 * screen render directly via CMD_SWAP, so this SW present path is not
 * needed (and the symbols don't exist).
 */
#ifdef EVE_SUPPORT_RENDERTARGET
static void full_mode_sw_present(lv_eve5_driver_t * drvr, const lv_area_t * area, const uint8_t * px_map)
{
    EVE_HalContext * phost = drvr->hal;
    int32_t W = phost->Width;
    int32_t H = phost->Height;

    /* SW always renders the full screen in FULL render mode; warn if not. */
    if(area->x1 != 0 || area->y1 != 0
       || lv_area_get_width(area) != W || lv_area_get_height(area) != H) {
        LV_LOG_WARN("EVE5 FULL SW: unexpected partial area (%d,%d)-(%d,%d)",
                    area->x1, area->y1, area->x2, area->y2);
    }

    uint32_t stride = (uint32_t)W * FB_BYTES_PER_PIXEL;
    uint32_t size = stride * (uint32_t)H;

    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(drvr->allocator, size, GA_ALIGN_4);
    uint32_t gpu_addr = Esd_GpuAlloc_Get(drvr->allocator, handle);
    if(gpu_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5 FULL SW: temp VRAM alloc failed (%u bytes)", size);
        return;
    }

    EVE_Hal_wrMem(phost, gpu_addr, px_map, size);
    EVE_Hal_requestFenceBeforeSwap(phost);

    /* SWAPCHAIN_0 is a BT820+ render-target sentinel. On non-RT chips, just
     * open a DL — CMD_SWAP at the end rotates the implicit framebuffer. */
    if(EVE_Hal_supportRenderTarget(phost)) {
        EVE_CoCmd_renderTarget(phost, SWAPCHAIN_0, FB_BITMAP_FORMAT, W, H);
    }
    EVE_CoCmd_dlStart(phost);
    EVE_CoDl_bitmapHandle(phost, EVE_CO_SCRATCH_HANDLE);
    EVE_CoDl_blendFunc(phost, ONE, ZERO);
    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
    EVE_CoDl_scissorXY(phost, 0, 0);
    EVE_CoDl_scissorSize(phost, W, H);
    EVE_CoDl_vertexTranslateX(phost, 0);
    EVE_CoDl_vertexTranslateY(phost, 0);

    EVE_CoDl_bitmapSource(phost, gpu_addr);
    EVE_CoDl_bitmapLayout(phost, FB_BITMAP_FORMAT, stride, H);
    EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, W, H);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, 0, 0);
    EVE_CoDl_end(phost);

    EVE_CoDl_blendFunc_default(phost);
    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);
    /* CMD_GRAPHICSFINISH is BT820-only; previous gens have a blocking CMD_SWAP. */
    if(EVE_Hal_supportRenderTarget(phost)) EVE_CoCmd_graphicsFinish(phost);

    EVE_CmdSync sync = EVE_Cmd_sync(phost);

    /* Wait for previous frame's resources to drain before reusing them, then
     * track this frame's sync for the next iteration / mode switch. */
    if(drvr->last_frame_sync != EVE_CMD_SYNC_INVALID) {
        EVE_Cmd_waitSync(phost, drvr->last_frame_sync);
    }
    drvr->last_frame_sync = sync;

    Esd_GpuAlloc_DeferredFree(drvr->allocator, handle, sync);
}
#endif /* EVE_SUPPORT_RENDERTARGET — full_mode_sw_present */

static void wait_cb(lv_display_t * disp)
{
    LV_UNUSED(disp);
    /* Async rendering, no blocking wait needed */
}

#endif /* LV_USE_EVE5 */
