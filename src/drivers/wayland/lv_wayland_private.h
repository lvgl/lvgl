/**
 * @file lv_templ.h
 *
 */

#ifndef LV_WAYLAND_PRIVATE_H
#define LV_WAYLAND_PRIVATE_H

#include <sys/poll.h>
#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland.h"
#include "lv_wayland_smm.h"

#if LV_USE_WAYLAND

#if !LV_WAYLAND_WL_SHELL
#include "wayland_xdg_shell.h"
#define LV_WAYLAND_XDG_SHELL 1
#else
#define LV_WAYLAND_XDG_SHELL 0
#endif

/*********************
 *      DEFINES
 *********************/

#define LVGL_DRAW_BUFFER_DIV (8)
#define DMG_CACHE_CAPACITY (32)
#define TAG_LOCAL (0)
#define TAG_BUFFER_DAMAGE (1)

#if LV_WAYLAND_WINDOW_DECORATIONS
#define TITLE_BAR_HEIGHT 24
#define BORDER_SIZE 2
#define BUTTON_MARGIN LV_MAX((TITLE_BAR_HEIGHT / 6), BORDER_SIZE)
#define BUTTON_PADDING LV_MAX((TITLE_BAR_HEIGHT / 8), BORDER_SIZE)
#define BUTTON_SIZE (TITLE_BAR_HEIGHT - (2 * BUTTON_MARGIN))

#endif

#ifndef LV_WAYLAND_CYCLE_PERIOD
#define LV_WAYLAND_CYCLE_PERIOD LV_MIN(LV_DEF_REFR_PERIOD, 1)
#endif



/**********************
 *      TYPEDEFS
 **********************/

enum object_type {
    OBJECT_TITLEBAR = 0,
    OBJECT_BUTTON_CLOSE,
#if LV_WAYLAND_XDG_SHELL
    OBJECT_BUTTON_MAXIMIZE,
    OBJECT_BUTTON_MINIMIZE,
#endif
    OBJECT_BORDER_TOP,
    OBJECT_BORDER_BOTTOM,
    OBJECT_BORDER_LEFT,
    OBJECT_BORDER_RIGHT,
    OBJECT_WINDOW,
};

#define FIRST_DECORATION (OBJECT_TITLEBAR)
#define LAST_DECORATION (OBJECT_BORDER_RIGHT)
#define NUM_DECORATIONS (LAST_DECORATION-FIRST_DECORATION+1)

struct window;
struct input {
    struct {
        uint32_t x;
        uint32_t y;
        lv_indev_state_t left_button;
        lv_indev_state_t right_button;
        lv_indev_state_t wheel_button;
        int16_t wheel_diff;
    } pointer;

    struct {
        lv_key_t key;
        lv_indev_state_t state;
    } keyboard;

#if LV_USE_GESTURE_RECOGNITION
    lv_indev_touch_data_t touches[10];
    uint8_t touch_event_cnt;
    uint8_t primary_id;
#endif
};

struct seat {
    struct wl_touch * wl_touch;
    struct wl_pointer * wl_pointer;
    struct wl_keyboard * wl_keyboard;

    struct {
        struct xkb_keymap * keymap;
        struct xkb_state * state;
    } xkb;
};

struct graphic_object {
    struct window * window;

    struct wl_surface * surface;
    bool surface_configured;
    smm_buffer_t * pending_buffer;
    smm_group_t * buffer_group;
    struct wl_subsurface * subsurface;

    enum object_type type;
    int width;
    int height;

    struct input input;
};

struct application {
    struct wl_display * display;
    struct wl_registry * registry;
    struct wl_compositor * compositor;
    struct wl_subcompositor * subcompositor;
    struct wl_shm * shm;
    struct wl_seat * wl_seat;

    struct wl_cursor_theme * cursor_theme;
    struct wl_surface * cursor_surface;

#if LV_WAYLAND_WL_SHELL
    struct wl_shell * wl_shell;
#endif

#if LV_WAYLAND_XDG_SHELL
    struct xdg_wm_base * xdg_wm;
#endif

#ifdef LV_WAYLAND_WINDOW_DECORATIONS
    bool opt_disable_decorations;
#endif

    uint32_t shm_format;

    struct xkb_context * xkb_context;

    struct seat seat;

    struct graphic_object * touch_obj;
    struct graphic_object * pointer_obj;
    struct graphic_object * keyboard_obj;

    lv_ll_t window_ll;
    lv_timer_t * cycle_timer;

    bool cursor_flush_pending;
    struct pollfd wayland_pfd;
};

struct window {
    lv_display_t * lv_disp;
    lv_draw_buf_t * lv_disp_draw_buf;

    lv_indev_t * lv_indev_pointer;
    lv_indev_t * lv_indev_pointeraxis;
    lv_indev_t * lv_indev_touch;
    lv_indev_t * lv_indev_keyboard;

    lv_wayland_display_close_f_t close_cb;

    struct application * application;

#if LV_WAYLAND_WL_SHELL
    struct wl_shell_surface * wl_shell_surface;
#endif

#if LV_WAYLAND_XDG_SHELL
    struct xdg_surface * xdg_surface;
    struct xdg_toplevel * xdg_toplevel;
    uint32_t wm_capabilities;
#endif

    struct graphic_object * body;
    struct {
        lv_area_t cache[DMG_CACHE_CAPACITY];
        unsigned char start;
        unsigned char end;
        unsigned size;
    } dmg_cache;

#if LV_WAYLAND_WINDOW_DECORATIONS
    struct graphic_object * decoration[NUM_DECORATIONS];
#endif

    int width;
    int height;

    bool resize_pending;
    int resize_width;
    int resize_height;

    bool flush_pending;
    bool shall_close;
    bool closed;
    bool maximized;
    bool fullscreen;
    uint32_t frame_counter;
    bool frame_done;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

const struct wl_keyboard_listener * lv_wayland_keyboard_get_listener(void);
const struct wl_pointer_listener * lv_wayland_pointer_get_listener(void);
const struct wl_touch_listener * lv_wayland_touch_get_listener(void);

#endif /* LV_USE_WAYLAND */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_WAYLAND_PRIVATE_H*/
