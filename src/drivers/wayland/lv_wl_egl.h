/**
 * @file lv_wl_egl.h
 *
 */

/**
 * Modified by Renesas in 2025
 */

#ifndef LV_WL_EGL_H
#define LV_WL_EGL_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "../../../lvgl.h"

#if LV_USE_WAYLAND && LV_WAYLAND_USE_EGL

#include "lv_wl_egl_private.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/


/*****************************************************************************
 * TYPEDEFS
 *****************************************************************************/


/*****************************************************************************
 * GLOBAL FUNCTION PROTOTYPES
 *****************************************************************************/

/**
 * Create a Wayland window with OpenGL ES backend for LVGL
 * @param width Window width in pixels
 * @param height Window height in pixels
 * @param title Window title (optional, can be NULL)
 * @return Pointer to LVGL display or NULL on failure
 */
lv_display_t * lv_wayland_egl_window_create(int32_t width, int32_t height, const char * title);

/**
 * Process Wayland events (non-blocking)
 * @return Number of events processed, or -1 on error
 */
int lv_wayland_egl_process_events(void);

/**
 * Destroy window and free all resources
 */
void lv_wayland_egl_window_destroy(void);

#ifdef __cplusplus
}
#endif

#endif

#endif /* LV_WL_EGL_H */
