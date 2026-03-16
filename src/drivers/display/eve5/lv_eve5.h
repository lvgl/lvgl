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
#include "../../../indev/lv_indev.h"
#include "EVE_Hal.h"
#include "Esd_GpuAlloc.h"

/*********************
 *      DEFINES
 *********************/

/** Maximum number of simultaneous touch points supported by BT820 */
#define LV_EVE5_TOUCH_POINTS_MAX 5

/**********************
 *      TYPEDEFS
 **********************/

/** Multi-touch context returned by lv_eve5_multitouch_create() */
typedef struct {
    lv_indev_t *indev[LV_EVE5_TOUCH_POINTS_MAX];
    lv_display_t *disp;
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
lv_display_t *lv_eve5_create(EVE_HalContext *hal, Esd_GpuAlloc *allocator);

/**
 * Get the EVE HAL context from the display
 * @param disp pointer to an EVE5 display
 * @return     pointer to EVE HAL context, or NULL if invalid
 */
EVE_HalContext *lv_eve5_get_hal(lv_display_t *disp);

/**
 * Get the GPU allocator from the display
 * @param disp pointer to an EVE5 display
 * @return     pointer to GPU allocator, or NULL if invalid
 */
Esd_GpuAlloc *lv_eve5_get_allocator(lv_display_t *disp);

/*--------------------
 * Single Touch
 *--------------------*/

/**
 * Create a single-point touch input device for the EVE5 display.
 * Uses REG_TOUCH_SCREEN_XY in compatibility mode (REG_CTOUCH_EXTENDED=1).
 * Suitable for resistive touch or when only primary touch point is needed.
 *
 * @param disp pointer to the EVE5 display created by lv_eve5_create()
 * @return     pointer to the created input device, or NULL on failure
 */
lv_indev_t *lv_eve5_touch_create(lv_display_t *disp);

/*--------------------
 * Multi-Touch
 *--------------------*/

/**
 * Create multi-touch input devices for the EVE5 display.
 * Enables extended mode (REG_CTOUCH_EXTENDED=0) and creates up to 5 
 * separate input devices for capacitive multi-touch.
 * 
 * BT820 touch registers in extended mode:
 * - Touch 0: REG_CTOUCH_TOUCH0_XY (0x160)
 * - Touch 1: REG_CTOUCH_TOUCHA_XY (0x164)
 * - Touch 2: REG_CTOUCH_TOUCHB_XY (0x168)
 * - Touch 3: REG_CTOUCH_TOUCHC_XY (0x16C)
 * - Touch 4: REG_CTOUCH_TOUCH4_XY (0x170)
 *
 * Note: Calibration should be performed BEFORE enabling extended mode,
 * as the calibration UI uses single-touch. The calibration matrix
 * applies to both modes.
 *
 * @param disp       pointer to the EVE5 display created by lv_eve5_create()
 * @param num_points number of touch points to support (1-5)
 * @return           pointer to multi-touch context, or NULL on failure
 *                   Caller should store this and pass to lv_eve5_multitouch_delete()
 */
lv_eve5_multitouch_t *lv_eve5_multitouch_create(lv_display_t *disp, uint8_t num_points);

/**
 * Delete multi-touch input devices and free resources.
 * Also restores compatibility mode (REG_CTOUCH_EXTENDED=1).
 * @param mt pointer to multi-touch context from lv_eve5_multitouch_create()
 */
void lv_eve5_multitouch_delete(lv_eve5_multitouch_t *mt);

/**
 * Get a specific touch point input device from multi-touch context
 * @param mt    pointer to multi-touch context
 * @param index touch point index (0 to num_points-1)
 * @return      pointer to the input device, or NULL if invalid
 */
lv_indev_t *lv_eve5_multitouch_get_indev(lv_eve5_multitouch_t *mt, uint8_t index);

/*--------------------
 * Touch Calibration
 *--------------------*/

/**
 * Run interactive touch calibration using EVE's CMD_CALIBRATE.
 * Blocks until calibration is complete. Results are stored in 
 * REG_TOUCH_TRANSFORM_A through REG_TOUCH_TRANSFORM_F.
 *
 * Note: Must be called in compatibility mode (single-touch).
 * If multi-touch is active, temporarily disables it during calibration.
 * The calibration matrix applies to both compatibility and extended modes.
 *
 * @param disp pointer to the EVE5 display
 * @return     true if calibration completed successfully
 */
bool lv_eve5_touch_calibrate(lv_display_t *disp);

/**
 * Set touch calibration matrix values directly.
 * Use this to restore previously saved calibration.
 * Values are 16.16 fixed-point numbers.
 *
 * @param disp   pointer to the EVE5 display
 * @param matrix array of 6 calibration values (A, B, C, D, E, F)
 */
void lv_eve5_touch_set_calibration(lv_display_t *disp, const int32_t matrix[6]);

/**
 * Get current touch calibration matrix values.
 * Use this to save calibration for later restoration.
 * Values are 16.16 fixed-point numbers where 0x10000 = 1.0.
 *
 * @param disp   pointer to the EVE5 display
 * @param matrix array to receive 6 calibration values (A, B, C, D, E, F)
 */
void lv_eve5_touch_get_calibration(lv_display_t *disp, int32_t matrix[6]);

/**
 * Set touch sampling mode
 * @param disp pointer to the EVE5 display
 * @param mode one of:
 *             0 = Off (no sampling)
 *             1 = One-shot (single sample)
 *             2 = Frame (sample at start of each frame)
 *             3 = Continuous (up to 1000 samples/sec, default)
 */
void lv_eve5_touch_set_mode(lv_display_t *disp, uint8_t mode);

/**********************
 *      MACROS
 **********************/

/* Touch mode constants */
#define LV_EVE5_TOUCHMODE_OFF        0
#define LV_EVE5_TOUCHMODE_ONESHOT    1
#define LV_EVE5_TOUCHMODE_FRAME      2
#define LV_EVE5_TOUCHMODE_CONTINUOUS 3

/*--------------------
 * HAL Thread Safety
 *--------------------*/

#if LV_USE_OS
/**
 * Lock the EVE HAL mutex.
 * Must be held while accessing the EVE HAL from any LVGL thread.
 * All EVE5 driver components (display, touch, draw unit, SD card)
 * use this to serialize HAL access when multi-threading is enabled.
 *
 * @param disp pointer to the EVE5 display
 */
void lv_eve5_hal_lock(lv_display_t *disp);

/**
 * Unlock the EVE HAL mutex.
 * @param disp pointer to the EVE5 display
 */
void lv_eve5_hal_unlock(lv_display_t *disp);
#endif

/*--------------------
 * SD Card Filesystem
 *--------------------*/

/* Include the SD card filesystem driver header */
#include "lv_eve5_sdcard.h"

#endif /* LV_USE_EVE5 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_EVE5_H */
