/**
 * @file lv_eve5.h
 *
 * EVE5 (BT820) Display and Touch Driver for LVGL
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */
#ifndef LV_EVE5_H
#define LV_EVE5_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl_public.h"

#if LV_USE_EVE5

#include "EVE_Hal.h"
#include "EVE_GpuAlloc.h"

/*********************
 *      DEFINES
 *********************/

#define LV_EVE5_TOUCH_POINTS_MAX 5

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Display rendering mode.
 *
 * PARTIAL: tile-based rendering — LVGL renders invalidated regions into a small
 *   tile buffer; the EVE5 driver composites tiles onto SWAPCHAIN_0 (reading the
 *   previous front buffer as base) and swaps. Currently runs the swapchain in
 *   single-buffered mode (REG_SC0_PTR1 = PTR0) because the compositor cannot
 *   reliably identify which swapchain buffer is the live scanout source after
 *   a swap — the natural alternative would be to set REG_SO_SOURCE to a fixed
 *   memory address, but that path does not correctly trigger the render in the
 *   emulator, so we fake the same single-target effect through the swapchain
 *   instead. A proper double-buffered partial mode would require encoding a
 *   per-frame sentinel into the rendered output so the compositor can detect
 *   which buffer was last rendered. Display list usage is bounded per-tile, so
 *   complex screens never overflow.
 *
 * FULL: full-screen rendering — LVGL re-renders the entire screen each frame
 *   into a virtual screen-sized buffer; the EVE5 draw unit renders the screen
 *   layer directly to SWAPCHAIN_0 (no intermediate composite). The swapchain
 *   runs proper double buffering (PTR0 and PTR1 distinct), so scanout is
 *   tear-free. Caveat: a complex screen may hit the per-frame display list
 *   limit (4096 entries / 16 KB on BT820); partial mode is preferred for very
 *   dense UIs. Mode is selectable at create time and switchable at runtime
 *   (e.g., to enable double buffering during video).
 */
typedef enum {
    LV_EVE5_RENDER_MODE_PARTIAL = 0,
    LV_EVE5_RENDER_MODE_FULL = 1,
} lv_eve5_render_mode_t;

/**
 * EVE5 VRAM residency descriptor that extends lv_draw_buf_vram_res_t with
 * GPU handle and EVE-specific metadata. Attached to draw_buf->vram_res
 * for buffers backed by EVE5 RAM_G allocations.
 */
typedef struct {
    lv_draw_buf_vram_res_t base;       /**< Must be first member */
    EVE_GpuHandle gpu_handle;          /**< RAM_G allocation handle (GA_HANDLE_INVALID when is_swapchain) */
    uint16_t eve_format;               /**< EVE bitmap format (ARGB8, RGB565, etc.) */
    uint32_t stride;                   /**< Bytes per row in RAM_G */
    int32_t width;                     /**< Image width in pixels */
    int32_t height;                    /**< Image height in pixels */
    uint32_t source_offset;            /**< Offset from alloc base to bitmap data */
    uint32_t palette_offset;           /**< Offset from alloc base to palette LUT (GA_INVALID if none) */
    bool is_premultiplied;             /**< True if GPU content is premultiplied alpha */
    bool has_content;                  /**< True after first render; incremental renders must preserve existing content */
    bool sample_as_luminance;          /**< Source semantics are luminance-as-RGB (LVGL LV_COLOR_FORMAT_L8 today;
                                            sub-byte luminance assets in the future). EVE samples L1/L2/L4/L8 as
                                            (R=255, G=255, B=255, A=value) — alpha-only with white RGB; luminance
                                            semantics need (R=G=B=value, A=255). When this flag is set the image
                                            draw paths put the chip in GLFORMAT mode with
                                            BITMAP_SWIZZLE(ALPHA,ALPHA,ALPHA,ONE) — the stored value lives in the
                                            sample's ALPHA channel, so ALPHA gets routed to all RGB outputs while
                                            alpha is forced to 1. Distinguishes from LVGL A1-A8 sources
                                            (sample_as_luminance=false) which map to EVE L1-L8 and render
                                            correctly as alpha by default. Requires BT815+ (BITMAP_SWIZZLE). */
    bool is_swapchain;                 /**< True for the full-mode screen draw_buf: render target is SWAPCHAIN_0
                                            (gpu_handle is GA_HANDLE_INVALID). SWAPCHAIN_0 is resolved by the
                                            render engine to the current back buffer (one of REG_SC0_PTR0/PTR1).
                                            Backbuffer lifetime is tied to scanout (not render-engine sync), so
                                            this vram_res must not be ScopedFree'd while it's still scanout source. */
} lv_eve5_vram_res_t;

typedef struct {
    lv_indev_t * indev[LV_EVE5_TOUCH_POINTS_MAX];
    lv_display_t * disp;
    uint8_t num_points;
} lv_eve5_multitouch_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*--------------------
 * Display
 *--------------------*/

/**
 * Create an EVE5 (BT820) display in PARTIAL render mode (default).
 * @param hal       pointer to initialized EVE HAL context
 * @param allocator pointer to GPU memory allocator
 * @return          pointer to the created display, or NULL on failure
 */
lv_display_t * lv_eve5_create(EVE_HalContext *hal, EVE_GpuAlloc *allocator);

/**
 * Create an EVE5 (BT820) display with a specific render mode.
 *
 * Both PARTIAL and FULL modes use the HAL-reserved swapchain framebuffers;
 * mode selection determines whether LVGL renders in tiles + composite (PARTIAL)
 * or full-frame directly to the back buffer (FULL).
 *
 * @param hal       pointer to initialized EVE HAL context
 * @param allocator pointer to GPU memory allocator
 * @param mode      initial rendering mode
 * @return          pointer to the created display, or NULL on failure
 */
lv_display_t * lv_eve5_create_ex(EVE_HalContext *hal, EVE_GpuAlloc *allocator,
                                 lv_eve5_render_mode_t mode);

/**
 * Switch the rendering mode at runtime. Safe to call between frames; the driver
 * waits for the previous frame's render-engine sync before reconfiguring LVGL.
 *
 * The same swapchain framebuffers serve both modes — no allocation churn. The
 * only visible side effect of switching modes is that the next 1–2 frames may
 * tear briefly, since switching changes how the back buffer is produced.
 *
 * @param disp pointer to the EVE5 display
 * @param mode new render mode
 * @return     true on success
 */
bool lv_eve5_set_render_mode(lv_display_t * disp, lv_eve5_render_mode_t mode);

/**
 * Get the current render mode.
 */
lv_eve5_render_mode_t lv_eve5_get_render_mode(lv_display_t * disp);

/**
 * Set the EVE5 draw unit pointer on the display's full_buf vram_res, so LVGL
 * can dispatch vram callbacks (vram_check, vram_free, etc.) on the swapchain
 * draw buffer. Called automatically by lv_draw_eve5_init() — the draw unit
 * looks up the display through hal->UserContext (set by lv_eve5_create*).
 *
 * Safe to call multiple times. No-op if the display has no full_buf or if
 * draw_unit is NULL.
 */
struct _lv_draw_unit_t;
void lv_eve5_link_draw_unit(lv_display_t * disp, struct _lv_draw_unit_t * draw_unit);

/**
 * Record the sync marker that follows the screen-swapping CMD_SWAP. Called by
 * the EVE5 draw unit after the screen layer's finish (FULL mode) so that
 * lv_eve5_set_render_mode can drain the in-flight scanout pipeline before
 * reconfiguring the swapchain registers.
 *
 * In PARTIAL mode, the screen layer's CMD_SWAP doesn't flip scanout (the layer
 * renders into a tile-sized RAM_G handle); the real frame swap happens in the
 * compositor, which updates last_frame_sync directly. So the draw unit only
 * calls this for swapchain-targeted layers (vram_res->is_swapchain).
 */
void lv_eve5_record_frame_sync(lv_display_t * disp, EVE_CmdSync sync);

/**
 * Queue a full-screen invalidate for after the current refresh completes.
 * Direct lv_obj_invalidate during rendering trips an assert in lv_inv_area;
 * this flag is consumed by a LV_EVENT_REFR_READY handler in the driver.
 */
void lv_eve5_request_invalidate(lv_display_t * disp);

/**
 * Reset the coprocessor narrowly after an isolated asset-load fault, and
 * invalidate cached EVE5 state that no longer matches hardware.
 *
 * Asset loads (CMD_LOADIMAGE / CMD_LOADASSET / CMD_INFLATE / CMD_FSREAD /
 * CMD_FLASHREAD) can fault on malformed or unsupported content. The load
 * call sites flush the FIFO before each attempt so the fault is
 * attributable to the just-issued command, then call this on a detected
 * fault so later renders aren't blocked by a stale fault state.
 *
 * Calls EVE_Util_resetCoprocessor, dispatches the optional draw-unit
 * cache-invalidation hook registered via
 * lv_eve5_set_coprocessor_reset_handler (so bitmap handle pool / rom font
 * cache / asset font bindings drop their now-stale state), then runs a
 * SyncBarrier so deferred frees / open scopes whose close syncs never
 * landed are released. Mirrors Esd_ResetCoprocessor.
 */
void lv_eve5_reset_coprocessor(lv_display_t * disp);

/**
 * Register a hook the display driver calls during lv_eve5_reset_coprocessor
 * so a connected draw unit can drop cached bitmap-handle bindings (rom
 * font cache, asset font slots) that no longer match the chip state after
 * a narrow reset. The hook is invoked with the draw unit pointer stashed
 * via lv_eve5_link_draw_unit. Pass NULL to clear. Safe to call multiple
 * times.
 *
 * Kept separate from lv_eve5_link_draw_unit so the display side has no
 * link-time dependency on draw-unit symbols — the draw unit's init
 * function registers itself.
 */
void lv_eve5_set_coprocessor_reset_handler(lv_display_t * disp,
                                           void (*handler)(struct _lv_draw_unit_t * draw_unit));

/**
 * Get the EVE HAL context from the display
 * @param disp pointer to an EVE5 display
 * @return     pointer to EVE HAL context, or NULL if invalid
 */
EVE_HalContext * lv_eve5_get_hal(lv_display_t * disp);

/**
 * Get the lv_display_t associated with an EVE HAL context. lv_eve5_create*
 * stashes the display on hal->UserContext so the draw unit (which owns only
 * a HAL pointer) can reach back to the display without resorting to
 * lv_display_get_default(). One display per HAL context is assumed.
 *
 * @param hal pointer to an EVE HAL context
 * @return    pointer to the associated display, or NULL if none has been set
 */
static inline lv_display_t * lv_eve5_disp_from_hal(EVE_HalContext * hal)
{
    return hal ? (lv_display_t *)hal->UserContext : NULL;
}

/**
 * Get the GPU allocator from the display
 * @param disp pointer to an EVE5 display
 * @return     pointer to GPU allocator, or NULL if invalid
 */
EVE_GpuAlloc * lv_eve5_get_allocator(lv_display_t * disp);

/**
 * Detach the GPU handle from a draw_buf's vram_res, transferring ownership
 * to the caller. The vram_res is freed and set to NULL.
 *
 * Optionally also returns the EVE bitmap format and stride that the buffer
 * was rendered in — the compositor needs these to sample each tile correctly,
 * since tile VRAM is allocated in the layer's color format (not always ARGB8).
 * Pass NULL for either out param if you don't need that piece of metadata.
 *
 * Refuses (returns false) when the vram_res is the driver-owned swapchain
 * descriptor — that one must not be torn apart.
 *
 * @param buf        pointer to draw buffer with EVE5 VRAM residency
 * @param out_handle receives the GPU allocation handle
 * @param out_format optional: receives the EVE bitmap format (RGB565, RGB8, ARGB8, ...)
 * @param out_stride optional: receives the bytes-per-row stride
 * @return           true if a handle was detached, false if no VRAM residency
 *                   (or it was the swapchain descriptor)
 */
bool lv_eve5_detach_gpu_handle(lv_draw_buf_t * buf, EVE_GpuHandle *out_handle,
                               uint16_t *out_format, uint32_t *out_stride);

/*--------------------
 * Single Touch
 *--------------------*/

/**
 * Create a single-point touch input device.
 * Uses REG_TOUCH_SCREEN_XY in compatibility mode (REG_CTOUCH_EXTENDED=1).
 * Suitable for resistive touch or when only the primary touch point is needed.
 *
 * @param disp pointer to the EVE5 display
 * @return     pointer to the created input device, or NULL on failure
 */
lv_indev_t * lv_eve5_touch_create(lv_display_t * disp);

/*--------------------
 * Multi-Touch
 *--------------------*/

/**
 * Create multi-touch input devices for capacitive touch panels.
 * Enables extended mode (REG_CTOUCH_EXTENDED=0) and creates up to 5
 * separate input devices.
 *
 * Touch calibration must be performed BEFORE calling this function,
 * as the calibration UI requires single-touch mode. The calibration
 * matrix applies to both modes.
 *
 * @param disp       pointer to the EVE5 display
 * @param num_points number of touch points to support (1-5)
 * @return           multi-touch context (caller must pass to lv_eve5_multitouch_delete)
 */
lv_eve5_multitouch_t * lv_eve5_multitouch_create(lv_display_t * disp, uint8_t num_points);

/**
 * Delete multi-touch input devices and restore single-touch mode.
 * @param mt pointer to multi-touch context
 */
void lv_eve5_multitouch_delete(lv_eve5_multitouch_t * mt);

/**
 * Get a specific touch point input device
 * @param mt    pointer to multi-touch context
 * @param index touch point index (0 to num_points-1)
 * @return      pointer to the input device, or NULL if invalid
 */
lv_indev_t * lv_eve5_multitouch_get_indev(lv_eve5_multitouch_t * mt, uint8_t index);

/*--------------------
 * Touch Calibration
 *--------------------*/

/**
 * Run interactive touch calibration using CMD_CALIBRATE.
 * Blocks until complete. Results are stored in REG_TOUCH_TRANSFORM_A-F.
 *
 * Requires single-touch mode. If multi-touch is active, temporarily
 * disables it during calibration.
 *
 * @param disp pointer to the EVE5 display
 * @return     true if calibration completed successfully
 */
bool lv_eve5_touch_calibrate(lv_display_t * disp);
bool lv_eve5_touch_calibrate_hal(EVE_HalContext * phost);

/**
 * Set touch calibration matrix directly (restore saved calibration).
 * Values are 16.16 fixed-point.
 *
 * @param disp   pointer to the EVE5 display
 * @param matrix array of 6 calibration values (A, B, C, D, E, F)
 */
void lv_eve5_touch_set_calibration(lv_display_t * disp, const int32_t matrix[6]);
void lv_eve5_touch_set_calibration_hal(EVE_HalContext * phost, const int32_t matrix[6]);

/**
 * Get current touch calibration matrix (save for later restoration).
 * Values are 16.16 fixed-point (0x10000 = 1.0).
 *
 * @param disp   pointer to the EVE5 display
 * @param matrix array to receive 6 calibration values (A, B, C, D, E, F)
 */
void lv_eve5_touch_get_calibration(lv_display_t * disp, int32_t matrix[6]);
void lv_eve5_touch_get_calibration_hal(EVE_HalContext * phost, int32_t matrix[6]);

/**
 * Set touch sampling mode
 * @param disp pointer to the EVE5 display
 * @param mode LV_EVE5_TOUCHMODE_OFF (0), _ONESHOT (1), _FRAME (2), or _CONTINUOUS (3, default)
 */
void lv_eve5_touch_set_mode(lv_display_t * disp, uint8_t mode);

/**********************
 *      MACROS
 **********************/

#define LV_EVE5_TOUCHMODE_OFF        0
#define LV_EVE5_TOUCHMODE_ONESHOT    1
#define LV_EVE5_TOUCHMODE_FRAME      2
#define LV_EVE5_TOUCHMODE_CONTINUOUS 3

/*--------------------
 * HAL Thread Safety
 *--------------------*/

#if LV_USE_OS
/**
 * Lock the EVE HAL mutex for thread-safe access.
 * All EVE5 driver components use this to serialize HAL access.
 * @param disp pointer to the EVE5 display
 */
void lv_eve5_hal_lock(lv_display_t * disp);

/**
 * Unlock the EVE HAL mutex.
 * @param disp pointer to the EVE5 display
 */
void lv_eve5_hal_unlock(lv_display_t * disp);
#endif

/*--------------------
 * Filesystems
 *--------------------*/

#include "lv_eve5_sdcard.h"
#include "lv_eve5_flash.h"
#include "lv_eve5_rom_font.h"
#include "lv_eve5_asset_font.h"

#endif /* LV_USE_EVE5 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_EVE5_H */
