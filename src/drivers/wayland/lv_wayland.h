/**
 * @file lv_wayland.h
 */

#ifndef LV_WAYLAND_H
#define LV_WAYLAND_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_WAYLAND

#include "lv_wayland_keyboard.h"
#include "lv_wayland_pointer.h"
#include "lv_wayland_touch.h"
#include "lv_wayland_window.h"
#include "lv_wayland_pointer_axis.h"

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
 * Wrapper around lv_timer_handler
 * @note Must be called in the application run loop instead of the
 * regular lv_timer_handler provided by LVGL
 * @return time till it needs to be run next (in ms)
 */
uint32_t lv_wayland_timer_handler(void);

/**
 * Retrieves the file descriptor of the wayland socket
 */
int lv_wayland_get_fd(void);

void lv_wayland_deinit(void);

lv_result_t lv_wayland_init(void);
void lv_wayland_flush(void);
int lv_wayland_get_display_size(const char *name, int32_t *width, int32_t *height);
/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_WAYLAND */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_WAYLAND_H */
