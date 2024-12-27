/**
 * @file lv_windows_display.h
 *
 */

#ifndef LV_WINDOWS_DISPLAY_H
#define LV_WINDOWS_DISPLAY_H

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

#define LV_WINDOWS_ZOOM_BASE_LEVEL 100

#ifndef USER_DEFAULT_SCREEN_DPI
#define USER_DEFAULT_SCREEN_DPI 96
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Create a LVGL display object.
 * @param title The window title of LVGL display.
 * @param hor_res The horizontal resolution value of LVGL display.
 * @param ver_res The vertical resolution value of LVGL display.
 * @param zoom_level The zoom level value. Base value is 100 a.k.a 100%.
 * @param allow_dpi_override Allow DPI override if true, or follow the
 *                           Windows DPI scaling setting dynamically.
 * @param simulator_mode Create simulator mode display if true, or create
 *                       application mode display.
 * @return The created LVGL display object.
 * @remark The display window is positioned at the center of the screen by
 *         default to enhance user experience.
*/
lv_display_t * lv_windows_create_display(
    const wchar_t * title,
    int32_t hor_res,
    int32_t ver_res,
    int32_t zoom_level,
    bool allow_dpi_override,
    bool simulator_mode);

/**
 * @brief Get the window handle from specific LVGL display object.
 * @param display The specific LVGL display object.
 * @return The window handle from specific LVGL display object.
*/
HWND lv_windows_get_display_window_handle(lv_display_t * display);

/**
 * @brief Get logical pixel value from physical pixel value taken account
 *        with zoom level.
 * @param physical The physical pixel value taken account with zoom level.
 * @param zoom_level The zoom level value. Base value is 100 a.k.a 100%.
 * @return The logical pixel value.
 * @remark It uses the same calculation style as Windows OS implementation.
 *         It will be useful for integrate LVGL Windows backend to other
 *         Windows applications.
*/
int32_t lv_windows_zoom_to_logical(int32_t physical, int32_t zoom_level);

/**
 * @brief Get physical pixel value taken account with zoom level from
 *        logical pixel value.
 * @param logical The logical pixel value.
 * @param zoom_level The zoom level value. Base value is 100 a.k.a 100%.
 * @return The physical pixel value taken account with zoom level.
 * @remark It uses the same calculation style as Windows OS implementation.
 *         It will be useful for integrate LVGL Windows backend to other
 *         Windows applications.
*/
int32_t lv_windows_zoom_to_physical(int32_t logical, int32_t zoom_level);

/**
 * @brief Get logical pixel value from physical pixel value taken account
 *        with DPI scaling.
 * @param physical The physical pixel value taken account with DPI scaling.
 * @param dpi The DPI scaling value. Base value is USER_DEFAULT_SCREEN_DPI.
 * @return The logical pixel value.
 * @remark It uses the same calculation style as Windows OS implementation.
 *         It will be useful for integrate LVGL Windows backend to other
 *         Windows applications.
*/
int32_t lv_windows_dpi_to_logical(int32_t physical, int32_t dpi);

/**
 * @brief Get physical pixel value taken account with DPI scaling from
 *        logical pixel value.
 * @param logical The logical pixel value.
 * @param dpi The DPI scaling value. Base value is USER_DEFAULT_SCREEN_DPI.
 * @return The physical pixel value taken account with DPI scaling.
 * @remark It uses the same calculation style as Windows OS implementation.
 *         It will be useful for integrate LVGL Windows backend to other
 *         Windows applications.
*/
int32_t lv_windows_dpi_to_physical(int32_t logical, int32_t dpi);

/**
 * @brief Set the window to be a top-level or normal window.
 * @param display The specific LVGL display object.
 * @param top_level If true, sets the window as a top-level window;
 *                  otherwise, restores it to a normal window.
 * @remark A top-level window is placed above all non-top-level windows
 *         and typically used for dialogs or notifications.
 *         This function uses Windows API to adjust the `WS_EX_TOPMOST`
 *         and `WS_EX_APPWINDOW` extended styles of the window.
 * @note Ensure that the LVGL display object is initialized properly
 *       before calling this function.
 */
void lv_windows_set_top_level(lv_display_t * display, bool top_level);

/**
 * @brief Set the window to be frameless or restore the default frame.
 * @param display The specific LVGL display object.
 * @param frameless If true, removes the window's frame (including
 *                  title bar and borders); otherwise, restores the
 *                  default frame.
 * @remark This function modifies the `WS_OVERLAPPEDWINDOW` and `WS_POPUPWINDOW`
 *         styles of the window using the Windows API.
 *         A frameless window is often used for splash screens or custom-styled windows.
 * @note Adjusting the frameless property may affect the user's ability to resize,
 *       move, or close the window using standard system controls.
 */
void lv_windows_set_frameless(lv_display_t * display, bool frameless);

/**********************
 *      MACROS
 **********************/

#endif // LV_USE_WINDOWS

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_WINDOWS_DISPLAY_H*/
