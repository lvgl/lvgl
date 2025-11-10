/**
 * @file lv_wayland_private.h
 *
 */

#ifndef LV_WAYLAND_PRIVATE_H
#define LV_WAYLAND_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_WAYLAND


#include <sys/poll.h>
#include <wayland-client-protocol.h>
#include <wayland_xdg_shell.h>
#include "../../misc/lv_types.h"
#include "lv_wl_backend_private.h"

/*********************
 *      DEFINES
 *********************/

#define LV_WAYLAND_DEFAULT_CURSOR_NAME "left_ptr"
#define LV_WAYLAND_MAX_OUTPUTS 8

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_wl_window_t;

typedef struct {
    struct wl_pointer * wl_pointer;
    struct wl_surface * cursor_surface;
    lv_point_t point;
    lv_indev_state_t left_btn_state;
    lv_indev_state_t right_btn_state;
    lv_indev_state_t wheel_btn_state;
    int16_t wheel_diff;
} lv_wl_seat_pointer_t;

typedef struct {
    struct wl_touch * wl_touch;

#if LV_USE_GESTURE_RECOGNITION
    lv_indev_touch_data_t touches[10];
    uint8_t event_cnt;
    uint8_t primary_id;
#else
    lv_point_t point;
    lv_indev_state_t state;
#endif /*LV_USE_GESTURE_RECOGNITION*/
} lv_wl_seat_touch_t;

typedef struct {
    struct wl_keyboard * wl_keyboard;
    struct xkb_keymap * xkb_keymap;
    struct xkb_state * xkb_state;

    lv_key_t key;
    lv_indev_state_t state;
    bool is_pressed;
} lv_wl_seat_keyboard_t;


typedef struct {
    struct wl_seat * wl_seat;

    lv_wl_seat_pointer_t * pointer;
    lv_wl_seat_touch_t * touch;
    lv_wl_seat_keyboard_t * keyboard;
} lv_wl_seat_t;



typedef struct {
    struct wl_output * wl_output;
    char name[64];
    int width;
    int height;
    int refresh;
    int scale;
    int flags;
} lv_wl_output_info_t;

typedef struct {
    struct wl_display * wl_display;
    struct wl_registry * wl_registry;
    struct wl_compositor * wl_compositor;
    struct wl_shm * wl_shm;
    lv_wl_seat_t seat;

    void * backend_data;
    lv_wl_output_info_t physical_outputs[LV_WAYLAND_MAX_OUTPUTS];
    uint8_t wl_output_count;

    struct xdg_wm_base * xdg_wm;

    lv_ll_t window_ll;
    lv_timer_t * cycle_timer;
} lv_wl_ctx_t;

typedef struct {
    struct xdg_surface * xdg_surface;
    uint32_t serial;
    int32_t width;
    int32_t height;
    bool requested;
    bool pending;
} lv_wl_resize_event_t;

typedef struct {
    struct xdg_surface * xdg_surface;
    struct xdg_toplevel * xdg_toplevel;
    bool configured;
} lv_wl_window_xdg_t;


typedef struct _lv_wl_window_t {
    lv_display_t * lv_disp;
    void * backend_display_data;
    lv_indev_t * lv_indev_pointer;
    lv_indev_t * lv_indev_pointeraxis;
    lv_indev_t * lv_indev_touch;
    lv_indev_t * lv_indev_keyboard;
    lv_wayland_display_close_cb_t close_cb;
    lv_wl_window_xdg_t xdg;

    /* The current physical assigned output */
    struct wl_output * physical_output;

    /* The current body surface */
    struct wl_surface * body;

    lv_wl_resize_event_t resize_event;

    bool maximized;
    bool fullscreen;

} lv_wl_window_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

extern lv_wl_ctx_t lv_wl_ctx;

/**********************
 *      Driver
 **********************/

lv_result_t lv_wayland_init(void);
void lv_wayland_deinit(void);

/**********************
 *      Window
 **********************/

int32_t lv_wayland_window_get_width(lv_wl_window_t * window);
int32_t lv_wayland_window_get_height(lv_wl_window_t * window);

void lv_wayland_window_delete(lv_wl_window_t * window);

const struct xdg_wm_base_listener * lv_wayland_xdg_get_wm_base_listener(void);

void lv_wayland_xdg_set_maximized(lv_wl_window_xdg_t * xdg, bool maximized);
void lv_wayland_xdg_set_minimized(lv_wl_window_xdg_t * xdg);
void lv_wayland_xdg_set_fullscreen(lv_wl_window_xdg_t * xdg, bool fullscreen,
                                   struct wl_output * output);
lv_result_t lv_wl_xdg_create_window(struct xdg_wm_base * xdg_wm, lv_wl_window_t * window,
                                    const char * title);
bool lv_wayland_xdg_is_resize_pending(lv_wl_window_t * window);
void lv_wayland_xdg_configure_surface(lv_wl_window_t * window);
void lv_wayland_xdg_resize(lv_wl_window_t * window);
void lv_wayland_xdg_delete_window(lv_wl_window_xdg_t * xdg);
void lv_wayland_xdg_deinit(void);

/**********************
 *      Input
 **********************/

void lv_wayland_seat_init(lv_wl_seat_t * seat, struct wl_registry * registry, uint32_t name, uint32_t version);
void lv_wayland_seat_deinit(lv_wl_seat_t * seat);

lv_wl_seat_pointer_t * lv_wayland_seat_pointer_create(struct wl_seat * seat, struct wl_surface * surface);
void lv_wayland_seat_pointer_delete(lv_wl_seat_pointer_t * seat_pointer);

lv_wl_seat_touch_t * lv_wayland_seat_touch_create(struct wl_seat * seat);
void lv_wayland_seat_touch_delete(lv_wl_seat_touch_t * seat_touch);

lv_wl_seat_keyboard_t * lv_wayland_seat_keyboard_create(struct wl_seat * seat);
void lv_wayland_seat_keyboard_delete(lv_wl_seat_keyboard_t * seat_keyboard);

/* Updates indev's driver data with the given 'read_cb' to 'new_driver_data' */
void lv_wayland_update_indevs(lv_indev_read_cb_t read_cb, void * new_driver_data);

#endif /* LV_USE_WAYLAND */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_WAYLAND_PRIVATE_H*/
