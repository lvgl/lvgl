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

#include "display/lcd/lv_lcd_generic_mipi.h"
#include "display/ili9341/lv_ili9341.h"
#include "display/st7735/lv_st7735.h"
#include "display/st7789/lv_st7789.h"
#include "display/st7796/lv_st7796.h"

#include "display/renesas_glcdc/lv_renesas_glcdc.h"
#include "display/st_ltdc/lv_st_ltdc.h"

#include "nuttx/lv_nuttx_entry.h"
#include "nuttx/lv_nuttx_fbdev.h"
#include "nuttx/lv_nuttx_touchscreen.h"
#include "nuttx/lv_nuttx_lcd.h"
#include "nuttx/lv_nuttx_libuv.h"

#include "evdev/lv_evdev.h"
#include "libinput/lv_libinput.h"

#include "windows/lv_windows_input.h"
#include "windows/lv_windows_display.h"

#include "glfw/lv_glfw_window.h"
#include "glfw/lv_opengles_texture.h"
#include "glfw/lv_opengles_driver.h"

#include "qnx/lv_qnx.h"

#include "wayland/lv_wayland.h"

#include "uefi/lv_uefi_context.h"
#include "uefi/lv_uefi_indev.h"
#include "uefi/lv_uefi_display.h"

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
