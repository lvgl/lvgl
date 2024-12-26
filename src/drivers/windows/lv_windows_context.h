/**
 * @file lv_windows_context.h
 *
 */

#ifndef LV_WINDOWS_CONTEXT_H
#define LV_WINDOWS_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"

#if LV_USE_WINDOWS

#if LV_USE_OS != LV_OS_WINDOWS
#error [lv_windows] LV_OS_WINDOWS is required. Enable it in lv_conf.h (LV_USE_OS LV_OS_WINDOWS)
#endif

#include <windows.h>

#ifndef CREATE_WAITABLE_TIMER_MANUAL_RESET
#define CREATE_WAITABLE_TIMER_MANUAL_RESET  0x00000001
#endif

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_windows_pointer_context_t {
    lv_indev_state_t state;
    lv_point_t point;
    lv_indev_t * indev;
} lv_windows_pointer_context_t;

typedef struct _lv_windows_keypad_queue_item_t {
    uint32_t key;
    lv_indev_state_t state;
} lv_windows_keypad_queue_item_t;

typedef struct _lv_windows_keypad_context_t {
    lv_ll_t queue;
    uint16_t utf16_high_surrogate;
    uint16_t utf16_low_surrogate;
    lv_indev_t * indev;
} lv_windows_keypad_context_t;

typedef struct _lv_windows_encoder_context_t {
    lv_indev_state_t state;
    int16_t enc_diff;
    lv_indev_t * indev;
} lv_windows_encoder_context_t;

typedef struct _lv_windows_window_context_t {
    lv_display_t * display_device_object;
    lv_timer_t * display_timer_object;

    int32_t window_dpi;
    int32_t zoom_level;
    bool allow_dpi_override;
    bool simulator_mode;
    bool display_resolution_changed;
    lv_point_t requested_display_resolution;

    HDC display_framebuffer_context_handle;
    uint32_t * display_framebuffer_base;
    size_t display_framebuffer_size;

    lv_windows_pointer_context_t pointer;
    lv_windows_keypad_context_t keypad;
    lv_windows_encoder_context_t encoder;

} lv_windows_window_context_t;

typedef struct _lv_windows_create_display_data_t {
    const wchar_t * title;
    int32_t hor_res;
    int32_t ver_res;
    int32_t zoom_level;
    bool allow_dpi_override;
    bool simulator_mode;
    HANDLE mutex;
    lv_display_t * display;
} lv_windows_create_display_data_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Initialize the LVGL Windows backend.
 * @remark This is a private API which is used for LVGL Windows backend
 *         implementation. LVGL users shouldn't use that because the
 *         LVGL has already used it in lv_init.
*/
void lv_windows_platform_init(void);

/**
 * @brief Get the window context from specific LVGL display window.
 * @param window_handle The window handle of specific LVGL display window.
 * @return The window context from specific LVGL display window.
 * @remark This is a private API which is used for LVGL Windows backend
 *         implementation. LVGL users shouldn't use that because the
 *         maintainer doesn't promise the application binary interface
 *         compatibility for this API.
*/
lv_windows_window_context_t * lv_windows_get_window_context(
    HWND window_handle);

/**********************
 *      MACROS
 **********************/

#endif // LV_USE_WINDOWS

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_WINDOWS_CONTEXT_H*/
