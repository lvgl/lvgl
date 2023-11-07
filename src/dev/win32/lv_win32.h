/**
 * @file lv_win32.h
 *
 */

#ifndef LV_WIN32_H
#define LV_WIN32_H

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"

#if LV_USE_WIN32

#include <windows.h>

#if _MSC_VER >= 1200
    // Disable compilation warnings.
    #pragma warning(push)
    // nonstandard extension used : bit field types other than int
    #pragma warning(disable:4214)
    // 'conversion' conversion from 'type1' to 'type2', possible loss of data
    #pragma warning(disable:4244)
#endif

#if _MSC_VER >= 1200
    // Restore compilation warnings.
    #pragma warning(pop)
#endif

/*********************
 *      DEFINES
 *********************/

#define LVGL_SIMULATOR_WINDOW_CLASS L"LVGL.SimulatorWindow"

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_win32_keyboard_queue_item_t {
    SLIST_ENTRY ItemEntry;
    uint32_t key;
    lv_indev_state_t state;
} lv_win32_keyboard_queue_item_t;

typedef struct _lv_win32_window_context_t {
    lv_display_t * display_device_object;
    lv_indev_t * mouse_device_object;
    lv_indev_t * mousewheel_device_object;
    lv_indev_t * keyboard_device_object;

    int32_t display_hor_res;
    int32_t display_ver_res;
    uint32_t display_dpi;
    void * display_draw_buffer_base;
    size_t display_draw_buffer_size;
    volatile bool display_refreshing;
    HDC display_framebuffer_context_handle;
    uint32_t * display_framebuffer_base;
    size_t display_framebuffer_size;

    lv_indev_state_t mouse_state;
    lv_point_t mouse_point;

    lv_indev_state_t mousewheel_state;
    int16_t mousewheel_enc_diff;

    CRITICAL_SECTION keyboard_mutex;
    PSLIST_HEADER keyboard_queue;
    uint16_t keyboard_utf16_high_surrogate;
    uint16_t keyboard_utf16_low_surrogate;
} lv_win32_window_context_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

EXTERN_C bool lv_win32_quit_signal;

void lv_win32_add_all_input_devices_to_group(lv_group_t * group);

lv_win32_window_context_t * lv_win32_get_window_context(HWND window_handle);

HWND lv_win32_create_display_window(
    const wchar_t * window_title,
    int32_t hor_res,
    int32_t ver_res,
    HINSTANCE instance_handle,
    HICON icon_handle,
    int show_window_mode);

bool lv_win32_init(
    HINSTANCE instance_handle,
    int show_window_mode,
    int32_t hor_res,
    int32_t ver_res,
    HICON icon_handle);

bool lv_win32_create(int32_t hor_res, int32_t ver_res);

#endif /*LV_USE_WIN32*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_WIN32_H*/
