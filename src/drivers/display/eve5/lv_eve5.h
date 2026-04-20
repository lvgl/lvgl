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
#include "../../../lv_conf_internal.h"

#if LV_USE_EVE5

#include "../../../display/lv_display.h"
#include "../../../draw/lv_draw_buf.h"
#include "../../../indev/lv_indev.h"
#include "EVE_Hal.h"
#include "Esd_GpuAlloc.h"

/*********************
 *      DEFINES
 *********************/

#define LV_EVE5_TOUCH_POINTS_MAX 5

/**********************
 *      TYPEDEFS
 **********************/

/**
 * EVE5 VRAM residency descriptor that extends lv_draw_buf_vram_res_t with
 * GPU handle and EVE-specific metadata. Attached to draw_buf->vram_res
 * for buffers backed by EVE5 RAM_G allocations.
 */
typedef struct {
    lv_draw_buf_vram_res_t base;       /**< Must be first member */
    Esd_GpuHandle gpu_handle;          /**< RAM_G allocation handle */
    uint16_t eve_format;               /**< EVE bitmap format (ARGB8, RGB565, etc.) */
    uint32_t stride;                   /**< Bytes per row in RAM_G */
    int32_t width;                     /**< Image width in pixels */
    int32_t height;                    /**< Image height in pixels */
    uint32_t source_offset;            /**< Offset from alloc base to bitmap data */
    uint32_t palette_offset;           /**< Offset from alloc base to palette LUT (GA_INVALID if none) */
    bool is_premultiplied;             /**< True if GPU content is premultiplied alpha */
    bool has_content;                  /**< True after first render; incremental renders must preserve existing content */
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
 * Create an EVE5 (BT820) display
 * @param hal       pointer to initialized EVE HAL context
 * @param allocator pointer to GPU memory allocator
 * @return          pointer to the created display, or NULL on failure
 */
lv_display_t * lv_eve5_create(EVE_HalContext *hal, Esd_GpuAlloc *allocator);

/**
 * Get the EVE HAL context from the display
 * @param disp pointer to an EVE5 display
 * @return     pointer to EVE HAL context, or NULL if invalid
 */
EVE_HalContext * lv_eve5_get_hal(lv_display_t * disp);

/**
 * Get the GPU allocator from the display
 * @param disp pointer to an EVE5 display
 * @return     pointer to GPU allocator, or NULL if invalid
 */
Esd_GpuAlloc * lv_eve5_get_allocator(lv_display_t * disp);

/**
 * Detach the GPU handle from a draw_buf's vram_res, transferring ownership
 * to the caller. The vram_res is freed and set to NULL.
 * @param buf        pointer to draw buffer with EVE5 VRAM residency
 * @param out_handle receives the GPU allocation handle
 * @return           true if a handle was detached, false if no VRAM residency
 */
bool lv_eve5_detach_gpu_handle(lv_draw_buf_t * buf, Esd_GpuHandle *out_handle);

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

#endif /* LV_USE_EVE5 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_EVE5_H */
