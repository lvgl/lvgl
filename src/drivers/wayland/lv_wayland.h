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
 * Wrapper around lv_timer_handler
 * @note Must be called in the application run loop instead of the
 * regular lv_timer_handler provided by LVGL
 * @return true: if the cycle was completed, false if the application
 * went to sleep because the last frame wasn't completed
 */
bool lv_wayland_timer_handler(void);

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

#endif /* WAYLAND_H */
