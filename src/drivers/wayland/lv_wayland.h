/*******************************************************************
 *
 * @file lv_wayland.h - Public functions of the LVGL Wayland client
 *
 * Based on the original file from the repository.
 *
 * Porting to LVGL 9.1
 * 2024 EDGEMTech Ltd.
 *
 * See LICENCE.txt for details
 *
 * Author(s): EDGEMTech Ltd, Erik Tagirov (erik.tagirov@edgemtech.ch)
 *
 ******************************************************************/
#ifndef LV_WAYLAND_H
#define LV_WAYLAND_H

#ifndef _WIN32

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"
#include "../../indev/lv_indev_gesture.h"

#if LV_USE_WAYLAND

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef bool (*lv_wayland_display_close_f_t)(lv_display_t * disp);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Retrieves the file descriptor of the wayland socket
 */
int lv_wayland_get_fd(void);

/**
 * Creates a window
 * @param hor_res The width of the window in pixels
 * @param ver_res The height of the window in pixels
 * @param title The title of the window
 * @param close_cb The callback that will be execute when the user closes the window
 * @return The LVGL display associated to the window
 */
lv_display_t * lv_wayland_window_create(uint32_t hor_res, uint32_t ver_res, char * title,
                                        lv_wayland_display_close_f_t close_cb);

/**
 * Closes the window programmatically
 * @param disp Reference to the LVGL display associated to the window
 */
void lv_wayland_window_close(lv_display_t * disp);

/**
 * Check if the window is open
 * @param disp Reference to the LVGL display associated to the window
 * @return true: The window is open
 */
bool lv_wayland_window_is_open(lv_display_t * disp);

/**
 * Sets the fullscreen state of the window
 * @param disp Reference to the LVGL display associated to the window
 * @param fullscreen If true the window enters fullscreen
 */
void lv_wayland_window_set_fullscreen(lv_display_t * disp, bool fullscreen);

/**
 * Sets the maximized state of the window
 * @param disp Reference to the LVGL display associated to the window
 * @param fullscreen If true the window is maximized
 */
void lv_wayland_window_set_maximized(lv_display_t * disp, bool maximize);

/**
 * Obtains the input device of the mouse pointer
 * @note It is used to create an input group on application start
 * @param disp Reference to the LVGL display associated to the window
 * @return The input device
 */
lv_indev_t * lv_wayland_get_pointer(lv_display_t * disp);

/**
 * Obtains the input device of the encoder
 * @note It is used to create an input group on application start
 * @param disp Reference to the LVGL display associated to the window
 * @return The input device
 */
lv_indev_t * lv_wayland_get_pointeraxis(lv_display_t * disp);

/**
 * Obtains the input device of the keyboard
 * @note It is used to create an input group on application start
 * @param disp Reference to the LVGL display associated to the window
 * @return The input device
 */
lv_indev_t * lv_wayland_get_keyboard(lv_display_t * disp);

/**
 * Obtains the input device of the touch screen
 * @note It is used to create an input group on application start
 * @param disp Reference to the LVGL display associated to the window
 * @return The input device
 */
lv_indev_t * lv_wayland_get_touchscreen(lv_display_t * disp);

/**
 * Wrapper around lv_timer_handler
 * @note Must be called in the application run loop instead of the
 * regular lv_timer_handler provided by LVGL
 * @return true: if the cycle was completed, false if the application
 * went to sleep because the last frame wasn't completed
 */
bool lv_wayland_timer_handler(void);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_WAYLAND */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _WIN32 */
#endif /* WAYLAND_H */
