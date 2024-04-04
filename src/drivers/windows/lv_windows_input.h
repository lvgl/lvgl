/**
 * @file lv_windows_input.h
 *
 */

#ifndef LV_WINDOWS_INPUT_H
#define LV_WINDOWS_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"

#if LV_USE_WINDOWS

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

/**
 * @brief Get the window handle from specific LVGL input device object.
 * @param indev The specific LVGL input device object.
 * @return The window handle from specific LVGL input device object.
*/
HWND lv_windows_get_indev_window_handle(lv_indev_t * indev);

/**
 * @brief Open a LVGL pointer input device object for the specific LVGL
 *        display object, or create it if the LVGL pointer input device
 *        object is not created or removed before.
 * @param display The specific LVGL display object.
 * @return The LVGL pointer input device object for the specific LVGL
 *         display object.
*/
lv_indev_t * lv_windows_acquire_pointer_indev(lv_display_t * display);

/**
 * @brief Open a LVGL keypad input device object for the specific LVGL
 *        display object, or create it if the LVGL keypad input device
 *        object is not created or removed before.
 * @param display The specific LVGL display object.
 * @return The LVGL keypad input device object for the specific LVGL
 *         display object.
*/
lv_indev_t * lv_windows_acquire_keypad_indev(lv_display_t * display);

/**
 * @brief Open a LVGL encoder input device object for the specific LVGL
 *        display object, or create it if the LVGL encoder input device
 *        object is not created or removed before.
 * @param display The specific LVGL display object.
 * @return The LVGL encoder input device object for the specific LVGL
 *         display object.
*/
lv_indev_t * lv_windows_acquire_encoder_indev(lv_display_t * display);

/**********************
 *      MACROS
 **********************/

#endif // LV_USE_WINDOWS

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_WINDOWS_INPUT_H*/
