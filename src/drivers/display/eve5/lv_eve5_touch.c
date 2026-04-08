/**
 * @file lv_eve5_touch.c
 *
 * EVE5 (BT820) Touch Driver for LVGL
 *
 * Touch Modes:
 * - Compatibility (REG_CTOUCH_EXTENDED=1): Single touch via REG_TOUCH_SCREEN_XY
 * - Extended (REG_CTOUCH_EXTENDED=0): Multi-touch via REG_CTOUCH_TOUCHx_XY
 *
 * Calibration (REG_TOUCH_TRANSFORM_A-F) applies to both modes and must be
 * performed in compatibility mode before enabling extended mode.
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

#include "../../../stdlib/lv_mem.h"
#if LV_USE_OS
    #include "../../../osal/lv_os.h"
#endif

/*********************
 * DEFINES
 *********************/

/* No-touch indicator: both X and Y read 0x8000 when not touched */
#define TOUCH_NOT_DETECTED 0x8000

/**********************
 * TYPEDEFS
 **********************/

typedef struct {
    lv_display_t * disp;
    uint8_t touch_index;
} lv_eve5_touch_data_t;

/**********************
 * STATIC PROTOTYPES
 **********************/

static void touch_read_cb(lv_indev_t * indev, lv_indev_data_t * data);
static void multitouch_read_cb(lv_indev_t * indev, lv_indev_data_t * data);
static uint32_t get_touch_register(EVE_HalContext *phost, uint8_t index);
static bool read_touch_xy(EVE_HalContext *phost, uint8_t index, int16_t * x, int16_t * y);

/**********************
 * GLOBAL FUNCTIONS
 **********************/

lv_indev_t * lv_eve5_touch_create(lv_display_t * disp)
{
    if(disp == NULL) return NULL;

    EVE_HalContext *phost = lv_eve5_get_hal(disp);
    if(phost == NULL) return NULL;

    lv_eve5_touch_data_t * touch_data = lv_malloc_zeroed(sizeof(lv_eve5_touch_data_t));
    if(touch_data == NULL) return NULL;

    touch_data->disp = disp;
    touch_data->touch_index = 0;

    lv_indev_t * indev = lv_indev_create();
    if(indev == NULL) {
        lv_free(touch_data);
        return NULL;
    }

#if LV_USE_OS
    lv_eve5_hal_lock(disp);
#endif
    EVE_Hal_wr8(phost, REG_CTOUCH_EXTENDED, 1);
#if LV_USE_OS
    lv_eve5_hal_unlock(disp);
#endif

    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touch_read_cb);
    lv_indev_set_display(indev, disp);
    lv_indev_set_user_data(indev, touch_data);

    LV_LOG_INFO("EVE5: Single-touch created (compatibility mode)");
    return indev;
}

lv_eve5_multitouch_t * lv_eve5_multitouch_create(lv_display_t * disp, uint8_t num_points)
{
    if(disp == NULL) return NULL;
    if(num_points == 0 || num_points > LV_EVE5_TOUCH_POINTS_MAX) {
        LV_LOG_WARN("EVE5: num_points must be 1-%d, got %d",
                    LV_EVE5_TOUCH_POINTS_MAX, num_points);
        return NULL;
    }

    EVE_HalContext *phost = lv_eve5_get_hal(disp);
    if(phost == NULL) return NULL;

    lv_eve5_multitouch_t * mt = lv_malloc_zeroed(sizeof(lv_eve5_multitouch_t));
    if(mt == NULL) return NULL;

    mt->disp = disp;
    mt->num_points = num_points;

#if LV_USE_OS
    lv_eve5_hal_lock(disp);
#endif
    EVE_Hal_wr8(phost, REG_CTOUCH_EXTENDED, 0);
#if LV_USE_OS
    lv_eve5_hal_unlock(disp);
#endif

    for(uint8_t i = 0; i < num_points; i++) {
        lv_eve5_touch_data_t * touch_data = lv_malloc_zeroed(sizeof(lv_eve5_touch_data_t));
        if(touch_data == NULL) {
            for(uint8_t j = 0; j < i; j++) {
                lv_free(lv_indev_get_user_data(mt->indev[j]));
                lv_indev_delete(mt->indev[j]);
            }
            EVE_Hal_wr8(phost, REG_CTOUCH_EXTENDED, 1);
            lv_free(mt);
            return NULL;
        }

        touch_data->disp = disp;
        touch_data->touch_index = i;

        lv_indev_t * indev = lv_indev_create();
        if(indev == NULL) {
            lv_free(touch_data);
            for(uint8_t j = 0; j < i; j++) {
                lv_free(lv_indev_get_user_data(mt->indev[j]));
                lv_indev_delete(mt->indev[j]);
            }
            EVE_Hal_wr8(phost, REG_CTOUCH_EXTENDED, 1);
            lv_free(mt);
            return NULL;
        }

        lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(indev, multitouch_read_cb);
        lv_indev_set_display(indev, disp);
        lv_indev_set_user_data(indev, touch_data);

        mt->indev[i] = indev;
    }

    LV_LOG_INFO("EVE5: Multi-touch created with %d points (extended mode)", num_points);
    return mt;
}

void lv_eve5_multitouch_delete(lv_eve5_multitouch_t * mt)
{
    if(mt == NULL) return;

    EVE_HalContext *phost = lv_eve5_get_hal(mt->disp);
    if(phost != NULL) {
        EVE_Hal_wr8(phost, REG_CTOUCH_EXTENDED, 1);
    }

    for(uint8_t i = 0; i < mt->num_points; i++) {
        if(mt->indev[i] != NULL) {
            lv_eve5_touch_data_t * touch_data = lv_indev_get_user_data(mt->indev[i]);
            if(touch_data != NULL) {
                lv_free(touch_data);
            }
            lv_indev_delete(mt->indev[i]);
        }
    }

    lv_free(mt);
    LV_LOG_INFO("EVE5: Multi-touch deleted, restored compatibility mode");
}

lv_indev_t * lv_eve5_multitouch_get_indev(lv_eve5_multitouch_t * mt, uint8_t index)
{
    if(mt == NULL || index >= mt->num_points) return NULL;
    return mt->indev[index];
}

bool lv_eve5_touch_calibrate(lv_display_t * disp)
{
    if(disp == NULL) return false;

    EVE_HalContext *phost = lv_eve5_get_hal(disp);
    if(phost == NULL) return false;

    /* Calibration requires compatibility mode */
    uint8_t prev_mode = EVE_Hal_rd8(phost, REG_CTOUCH_EXTENDED);
    EVE_Hal_wr8(phost, REG_CTOUCH_EXTENDED, 1);

    LV_LOG_INFO("EVE5: Starting touch calibration...");

    EVE_CoCmd_dlStart(phost);
    EVE_CoDl_clearColorRgb(phost, 0, 0, 0);
    EVE_CoDl_clear(phost, true, true, true);
    EVE_CoDl_colorRgb(phost, 255, 255, 255);
    (void)EVE_CoCmd_calibrate(phost);
    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);
    EVE_Cmd_waitFlush(phost);

    bool success = (phost->Status == EVE_STATUS_OPENED);

    EVE_Hal_wr8(phost, REG_CTOUCH_EXTENDED, prev_mode);

    if(success) {
        LV_LOG_INFO("EVE5: Touch calibration complete");
    }
    else {
        LV_LOG_ERROR("EVE5: Touch calibration failed");
    }

    return success;
}

void lv_eve5_touch_set_calibration(lv_display_t * disp, const int32_t matrix[6])
{
    if(disp == NULL || matrix == NULL) return;

    EVE_HalContext *phost = lv_eve5_get_hal(disp);
    if(phost == NULL) return;

    EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_A, (uint32_t)matrix[0]);
    EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_B, (uint32_t)matrix[1]);
    EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_C, (uint32_t)matrix[2]);
    EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_D, (uint32_t)matrix[3]);
    EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_E, (uint32_t)matrix[4]);
    EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_F, (uint32_t)matrix[5]);

    LV_LOG_INFO("EVE5: Touch calibration matrix set");
}

void lv_eve5_touch_get_calibration(lv_display_t * disp, int32_t matrix[6])
{
    if(disp == NULL || matrix == NULL) return;

    EVE_HalContext *phost = lv_eve5_get_hal(disp);
    if(phost == NULL) return;

    matrix[0] = (int32_t)EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_A);
    matrix[1] = (int32_t)EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_B);
    matrix[2] = (int32_t)EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_C);
    matrix[3] = (int32_t)EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_D);
    matrix[4] = (int32_t)EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_E);
    matrix[5] = (int32_t)EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_F);
}

void lv_eve5_touch_set_mode(lv_display_t * disp, uint8_t mode)
{
    if(disp == NULL) return;
    if(mode > 3) return;

    EVE_HalContext *phost = lv_eve5_get_hal(disp);
    if(phost == NULL) return;

    EVE_Hal_wr8(phost, REG_TOUCH_MODE, mode);
}

/**********************
 * STATIC FUNCTIONS
 **********************/

/**
 * Get the register address for a touch point in extended mode.
 * Touch points 0-4 map to non-contiguous registers.
 */
static uint32_t get_touch_register(EVE_HalContext *phost, uint8_t index)
{
    LV_UNUSED(phost);
    switch(index) {
        case 0:
            return REG_CTOUCH_TOUCH0_XY;
        case 1:
            return REG_CTOUCH_TOUCHA_XY;
        case 2:
            return REG_CTOUCH_TOUCHB_XY;
        case 3:
            return REG_CTOUCH_TOUCHC_XY;
        case 4:
            return REG_CTOUCH_TOUCH4_XY;
        default:
            return REG_CTOUCH_TOUCH0_XY;
    }
}

/**
 * Read touch coordinates. All touch registers are 32-bit: X in upper 16, Y in lower 16.
 * Returns false if no touch (both coordinates read 0x8000).
 */
static bool read_touch_xy(EVE_HalContext *phost, uint8_t index, int16_t * x, int16_t * y)
{
    uint32_t reg = get_touch_register(phost, index);
    uint32_t xy = EVE_Hal_rd32(phost, reg);

    *x = (int16_t)(xy >> 16);
    *y = (int16_t)(xy & 0xFFFF);

    if((xy >> 16) == TOUCH_NOT_DETECTED || (xy & 0xFFFF) == TOUCH_NOT_DETECTED) {
        return false;
    }

    return true;
}

static void touch_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_eve5_touch_data_t * touch_data = lv_indev_get_user_data(indev);
    if(touch_data == NULL || touch_data->disp == NULL) {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }

    EVE_HalContext *phost = lv_eve5_get_hal(touch_data->disp);
    if(phost == NULL) {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }

#if LV_USE_OS
    lv_eve5_hal_lock(touch_data->disp);
#endif
    uint32_t xy = EVE_Hal_rd32(phost, REG_TOUCH_SCREEN_XY);
#if LV_USE_OS
    lv_eve5_hal_unlock(touch_data->disp);
#endif

    uint16_t x_raw = (xy >> 16) & 0xFFFF;
    uint16_t y_raw = xy & 0xFFFF;

    if(x_raw == TOUCH_NOT_DETECTED || y_raw == TOUCH_NOT_DETECTED) {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }

    int16_t x = (int16_t)x_raw;
    int16_t y = (int16_t)y_raw;

    if(x >= 0 && y >= 0 && (uint32_t)x < phost->Width && (uint32_t)y < phost->Height) {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = x;
        data->point.y = y;
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

static void multitouch_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_eve5_touch_data_t * touch_data = lv_indev_get_user_data(indev);
    if(touch_data == NULL || touch_data->disp == NULL) {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }

    EVE_HalContext *phost = lv_eve5_get_hal(touch_data->disp);
    if(phost == NULL) {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }

    int16_t x, y;

#if LV_USE_OS
    lv_eve5_hal_lock(touch_data->disp);
#endif
    bool touched = read_touch_xy(phost, touch_data->touch_index, &x, &y);
#if LV_USE_OS
    lv_eve5_hal_unlock(touch_data->disp);
#endif

    if(touched && x >= 0 && y >= 0 && (uint32_t)x < phost->Width && (uint32_t)y < phost->Height) {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = x;
        data->point.y = y;
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

#endif /* LV_USE_EVE5 */
