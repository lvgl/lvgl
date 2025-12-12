
/**
 * @file lv_wl_window.h
 *
 */

#ifndef LV_WL_WINDOW_H
#define LV_WL_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"

#if LV_USE_WAYLAND

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef bool (*lv_wayland_display_close_cb_t)(lv_display_t * disp);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Creates a window
 * @param hor_res The width of the window in pixels
 * @param ver_res The height of the window in pixels
 * @param title The title of the window
 * @param close_cb The callback that will be execute when the user closes the window
 * @return The LVGL display associated to the window
 */
lv_display_t * lv_wayland_window_create(uint32_t hor_res, uint32_t ver_res, char * title,
                                        lv_wayland_display_close_cb_t close_cb);

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
 * Assigns the window to a specific physical display
 * @param disp Reference to the LVGL display associated to the window
 * @param display Physical display number
 */
void lv_wayland_assign_physical_display(lv_display_t * disp, uint8_t display);

/**
 * Unassigns the current physical display attached to the window
 * @param disp Reference to the LVGL display associated to the window
 */
void lv_wayland_unassign_physical_display(lv_display_t * disp);

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
 * Minimizes the window
 * @param disp Reference to the LVGL display associated to the window
 */
void lv_wayland_window_set_minimized(lv_display_t * disp);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_WAYLAND */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_WL_WINDOW_H*/
