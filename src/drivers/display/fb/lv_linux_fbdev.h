/**
 * @file lv_linux_fbdev.h
 *
 */

#ifndef LV_LINUX_FBDEV_H
#define LV_LINUX_FBDEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../display/lv_display.h"

#if LV_USE_LINUX_FBDEV

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_display_t * lv_linux_fbdev_create(void);

lv_result_t lv_linux_fbdev_set_file(lv_display_t * disp, const char * file);

/**
 * Skip the FBIOBLANK unblank ioctl in lv_linux_fbdev_set_file().
 * Use when another process (e.g. splash screen) already owns the framebuffer
 * and calling FBIOBLANK would cause visible flicker.
 * Must be called BEFORE lv_linux_fbdev_set_file().
 */
void lv_linux_fbdev_set_skip_unblank(lv_display_t * disp, bool skip);

/**
 * Force the display to be refreshed on every change.
 * Expected to be used with LV_DISPLAY_RENDER_MODE_DIRECT or LV_DISPLAY_RENDER_MODE_FULL.
 */
void lv_linux_fbdev_set_force_refresh(lv_display_t * disp, bool enabled);

/**
 * Enable red/blue channel swap in the flush callback.
 * Use for framebuffers with BGR pixel order (e.g., some Allwinner SoCs).
 * On Linux (non-BSD) builds, auto-detected from fb_var_screeninfo if not called explicitly.
 * When LV_LINUX_FBDEV_BSD is enabled, auto-detection is disabled and this must be set manually.
 * Must be called AFTER lv_linux_fbdev_set_file().
 */
void lv_linux_fbdev_set_swap_rb(lv_display_t * disp, bool enabled);

/**
 * Query whether red/blue channel swap is active.
 */
bool lv_linux_fbdev_get_swap_rb(lv_display_t * disp);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_LINUX_FBDEV */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_LINUX_FBDEV_H */
