/**
 * @file lv_windows_input_private.h
 *
 */

#ifndef LV_WINDOWS_INPUT_PRIVATE_H
#define LV_WINDOWS_INPUT_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#if LV_USE_WINDOWS

#include <stdbool.h>
#include <windows.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

bool lv_windows_pointer_device_window_message_handler(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    LRESULT * plResult);

bool lv_windows_keypad_device_window_message_handler(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    LRESULT * plResult);

bool lv_windows_encoder_device_window_message_handler(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    LRESULT * plResult);

/**********************
 *      MACROS
 **********************/

#endif // LV_USE_WINDOWS

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_WINDOWS_INPUT_PRIVATE_H*/
