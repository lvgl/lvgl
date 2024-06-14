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

#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"

#if LV_USE_WAYLAND

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef bool (*lv_window_close_cb_t)(lv_display_t * disp);
typedef int32_t lw_coord_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
bool lv_wayland_init(void);
void lv_wayland_deinit(void);
uint32_t lv_wayland_timer_handler(void);
int lv_get_wayland_fd(void);
lv_display_t * lv_create_wayland_window(lw_coord_t hor_res, lw_coord_t ver_res, char *title,
                                     lv_window_close_cb_t close_cb);
bool lv_close_wayland_window(lv_display_t * disp);
void lv_set_wayland_window_fullscreen(lv_display_t * disp, bool fullscreen);
lv_indev_t * lv_get_wayland_pointer(lv_display_t * disp);
lv_indev_t * lv_get_wayland_pointeraxis(lv_display_t * disp);
lv_indev_t * lv_get_wayland_keyboard(lv_display_t * disp);
lv_indev_t * lv_get_wayland_touchscreen(lv_display_t * disp);

bool lv_is_window_open(lv_display_t *disp);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_WAYLAND */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WAYLAND_H */
