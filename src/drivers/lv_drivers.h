/**
 * @file lv_drivers.h
 *
 */

#ifndef LV_DRIVERS_H
#define LV_DRIVERS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "sdl/lv_sdl_window.h"
#include "sdl/lv_sdl_mouse.h"
#include "sdl/lv_sdl_mousewheel.h"
#include "sdl/lv_sdl_keyboard.h"

#include "x11/lv_x11.h"

#include "display/drm/lv_linux_drm.h"
#include "display/fb/lv_linux_fbdev.h"

#include "display/tft_espi/lv_tft_espi.h"

#include "nuttx/lv_nuttx_entry.h"
#include "nuttx/lv_nuttx_fbdev.h"
#include "nuttx/lv_nuttx_touchscreen.h"
#include "nuttx/lv_nuttx_lcd.h"
#include "nuttx/lv_nuttx_libuv.h"

#include "evdev/lv_evdev.h"

#include "windows/lv_windows_input.h"
#include "windows/lv_windows_display.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRIVERS_H*/
