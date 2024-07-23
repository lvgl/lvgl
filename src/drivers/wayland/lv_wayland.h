/*******************************************************************
 *
 * lv_wayland.h - Public functions of the LVGL Wayland client
 *
 * Based on the original file from the repository.
 *
 * Porting to LVGL 9.1
 * Copyright (c) 2024 EDGEMTech Ltd.
 *
 * See LICENSE.txt for details
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

#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"

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

void lv_wayland_init(void);
void lv_wayland_deinit(void);
int lv_wayland_get_fd(void);

lv_display_t * lv_wayland_window_create(uint32_t hor_res, uint32_t ver_res, char * title,
                                     lv_wayland_display_close_f_t close_cb);

void lv_wayland_window_close(lv_display_t * disp);
bool lv_wayland_window_is_open(lv_display_t * disp);
void lv_wayland_window_set_fullscreen(lv_display_t * disp, bool fullscreen);
void lv_wayland_window_set_maximized(lv_display_t * disp, bool maximize);

lv_indev_t * lv_wayland_get_pointer(lv_display_t * disp);
lv_indev_t * lv_wayland_get_pointeraxis(lv_display_t * disp);
lv_indev_t * lv_wayland_get_keyboard(lv_display_t * disp);
lv_indev_t * lv_wayland_get_touchscreen(lv_display_t * disp);

uint32_t lv_wayland_timer_handler(void);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_WAYLAND */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WAYLAND_H */
