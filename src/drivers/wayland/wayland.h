/**
 * @file wayland
 *
 */

#ifndef WAYLAND_H
#define WAYLAND_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h"
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
bool lw_wayland_init(void);
void lw_wayland_deinit(void);
int lw_get_wayland_fd(void);
lv_disp_t * lw_create_window(lv_coord_t hor_res, lv_coord_t ver_res, char *title,
                                     lv_wayland_display_close_f_t close_cb);
void lw_close_wayland_window(lv_display_t * disp);
void lw_set_wayland_window_fullscreen(lv_display_t * disp, bool fullscreen);
lv_indev_t * lw_lvgl_get_pointer(lv_display_t * disp);
lv_indev_t * lw_lvgl_get_pointeraxis(lv_display_t * disp);
lv_indev_t * lw_lvgl_get_keyboard(lv_display_t * disp);
lv_indev_t * lw_lvgl_get_touchscreen(lv_display_t * disp);
uint32_t lw_wayland_timer_handler(void);
bool lw_window_is_open(lv_display_t *disp);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_WAYLAND */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WAYLAND_H */
