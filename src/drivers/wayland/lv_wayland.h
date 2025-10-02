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

#include "lv_wl_keyboard.h"
#include "lv_wl_pointer.h"
#include "lv_wl_touch.h"
#include "lv_wl_window.h"
#include "lv_wl_pointer_axis.h"

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
 * Retrieves the file descriptor of the wayland socket
 */
int lv_wayland_get_fd(void);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_WAYLAND */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_WAYLAND_H */
