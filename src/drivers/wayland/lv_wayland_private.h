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

#include "lv_wayland.h"
#if LV_USE_WAYLAND


#include "lv_wayland_smm.h"
#include <sys/poll.h>
#include <wayland-client-protocol.h>
#if !LV_WAYLAND_WL_SHELL
#include "wayland_xdg_shell.h"
#define LV_WAYLAND_XDG_SHELL 1
#else
#define LV_WAYLAND_XDG_SHELL 0
#endif

/*********************
 *      DEFINES
 *********************/

#define LV_WAYLAND_DEFAULT_CURSOR_NAME "left_ptr"

#define LVGL_DRAW_BUFFER_DIV (8)
#define DMG_CACHE_CAPACITY (32)
#define TAG_LOCAL (0)
#define TAG_BUFFER_DAMAGE (1)

#if LV_WAYLAND_WINDOW_DECORATIONS
#define TITLE_BAR_HEIGHT 24
#define BORDER_SIZE 2
#else
#define TITLE_BAR_HEIGHT 0
#define BORDER_SIZE 0
#endif

#define BUTTON_MARGIN LV_MAX((TITLE_BAR_HEIGHT / 6), BORDER_SIZE)
#define BUTTON_PADDING LV_MAX((TITLE_BAR_HEIGHT / 8), BORDER_SIZE)
#define BUTTON_SIZE (TITLE_BAR_HEIGHT - (2 * BUTTON_MARGIN))

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
#define NUM_DECORATIONS (LAST_DECORATION - FIRST_DECORATION + 1)

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
    struct wl_subsurface * subsurface;
    smm_buffer_t * pending_buffer;
    smm_group_t * buffer_group;
    struct input input;
    enum object_type type;
    int width;
    int height;
};

typedef struct {
    struct buffer * buffers;
    struct zwp_linux_dmabuf_v1 * handler;
    uint32_t format;
} dmabuf_ctx_t;

typedef struct {
    lv_draw_buf_t * lv_draw_buf;
    struct wl_shm * handler;
    uint32_t format;
} shm_ctx_t;

struct lv_wayland_context {
    struct wl_display * display;
    struct wl_registry * registry;
    struct wl_compositor * compositor;
    struct wl_subcompositor * subcompositor;
    struct wl_seat * wl_seat;
    struct wl_cursor_theme * cursor_theme;
    struct wl_surface * cursor_surface;
    shm_ctx_t shm_ctx;

#if LV_WAYLAND_USE_DMABUF
    dmabuf_ctx_t dmabuf_ctx;
#endif

#if LV_WAYLAND_WL_SHELL
    struct wl_shell * wl_shell;
#endif

#if LV_WAYLAND_XDG_SHELL
    struct xdg_wm_base * xdg_wm;
#endif

#ifdef LV_WAYLAND_WINDOW_DECORATIONS
    bool opt_disable_decorations;
#endif

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
    lv_indev_t * lv_indev_pointer;
    lv_indev_t * lv_indev_pointeraxis;
    lv_indev_t * lv_indev_touch;
    lv_indev_t * lv_indev_keyboard;

    lv_wayland_display_close_f_t close_cb;

    struct lv_wayland_context * wl_ctx;

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
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

extern struct lv_wayland_context lv_wl_ctx;

/**********************
 *      Driver
 **********************/

void lv_wayland_init(void);
void lv_wayland_deinit(void);
void lv_wayland_wait_flush_cb(lv_display_t * disp);
/**********************
 *      Window
 **********************/

const struct wl_callback_listener * lv_wayland_window_get_wl_surface_frame_listener(void);

void lv_wayland_window_draw(struct window * window, uint32_t width, uint32_t height);
lv_result_t lv_wayland_window_resize(struct window * window, int width, int height);
void lv_wayland_window_destroy(struct window * window);
#if LV_WAYLAND_WINDOW_DECORATIONS
uint32_t lv_wayland_window_decoration_create_all(struct window * window);
void lv_wayland_window_decoration_detach_all(struct window * window);
bool lv_wayland_window_decoration_create(struct window * window, struct graphic_object * decoration, int window_width,
                                         int window_height);
bool lv_wayland_window_decoration_attach(struct window * window, struct graphic_object * decoration,
                                         smm_buffer_t * decoration_buffer, struct graphic_object * parent);
void lv_wayland_window_decoration_detach(struct window * window, struct graphic_object * decoration);
#endif

/**********************
 *  Window Management
 **********************/

#if LV_WAYLAND_WL_SHELL
lv_result_t lv_wayland_wl_shell_create_window(struct lv_wayland_context * app, struct window * window,
                                              const char * title);
const struct wl_shell_surface_listener * lv_wayland_wl_shell_get_listener(void);
void lv_wayland_wl_shell_handle_pointer_event(struct lv_wayland_context * app, uint32_t serial, uint32_t button,
                                              uint32_t state);
lv_result_t lv_wayland_wl_shell_set_maximized(struct window * window, bool maximized);
lv_result_t lv_wayland_wl_shell_set_minimized(struct window * window);
lv_result_t lv_wayland_wl_shell_set_fullscreen(struct window * window, bool fullscreen);
lv_result_t lv_wayland_wl_shell_destroy_window(struct window * window);
void lv_wayland_wl_shell_deinit(void);
#elif LV_WAYLAND_XDG_SHELL

const struct xdg_surface_listener * lv_wayland_xdg_shell_get_surface_listener(void);
const struct xdg_toplevel_listener * lv_wayland_xdg_shell_get_toplevel_listener(void);
const struct xdg_wm_base_listener * lv_wayland_xdg_shell_get_wm_base_listener(void);
lv_result_t lv_wayland_xdg_shell_set_maximized(struct window * window, bool maximized);
lv_result_t lv_wayland_xdg_shell_set_minimized(struct window * window);
lv_result_t lv_wayland_xdg_shell_set_fullscreen(struct window * window, bool fullscreen);
lv_result_t lv_wayland_xdg_shell_create_window(struct lv_wayland_context * app, struct window * window,
                                               const char * title);
lv_result_t lv_wayland_xdg_shell_destroy_window_toplevel(struct window * window);
lv_result_t lv_wayland_xdg_shell_destroy_window_surface(struct window * window);
void lv_wayland_xdg_shell_handle_pointer_event(struct lv_wayland_context * app, uint32_t serial, uint32_t button,
                                               uint32_t state);

const char * lv_wayland_xdg_shell_get_cursor_name(const struct lv_wayland_context * app);
void lv_wayland_xdg_shell_deinit(void);
#endif

/**********************
 *      SHM
 **********************/

void lv_wayland_shm_set_interface(shm_ctx_t * context, struct wl_registry * registry, uint32_t name,
                                  const char * interface, uint32_t version);

struct graphic_object * lv_wayland_shm_on_graphical_object_creation(shm_ctx_t * context, struct graphic_object * obj);
void lv_wayland_shm_on_graphical_object_destruction(shm_ctx_t * context, struct graphic_object * obj);
lv_result_t lv_wayland_shm_set_draw_buffers(shm_ctx_t * context, lv_display_t * display);
lv_result_t lv_wayland_shm_create_draw_buffers(shm_ctx_t * context, struct window * window);
lv_result_t lv_wayland_shm_resize_window(shm_ctx_t * context, struct window * window, int32_t width, int32_t height);
lv_result_t lv_wayland_shm_is_ready(shm_ctx_t * context);

void lv_wayland_shm_delete_draw_buffers(shm_ctx_t * context, struct window * window);
void lv_wayland_shm_initalize_context(shm_ctx_t * context);
void lv_wayland_shm_deinit(shm_ctx_t * context);
void lv_wayland_shm_flush_partial_mode(lv_display_t * disp, const lv_area_t * area, unsigned char * color_p);

struct wl_cursor_theme * lv_wayland_shm_load_cursor_theme(shm_ctx_t * context);

/**********************
 *      DMABUF
 **********************/

void lv_wayland_dmabuf_set_interface(dmabuf_ctx_t * context, struct wl_registry * registry, uint32_t name,
                                     const char * interface, uint32_t version);

struct graphic_object * lv_wayland_dmabuf_on_graphical_object_creation(dmabuf_ctx_t * context,
                                                                       struct graphic_object * obj);

void lv_wayland_dmabuf_on_graphical_object_destruction(dmabuf_ctx_t * context, struct graphic_object * obj);
lv_result_t lv_wayland_dmabuf_set_draw_buffers(dmabuf_ctx_t * context, lv_display_t * display);
lv_result_t lv_wayland_dmabuf_create_draw_buffers(dmabuf_ctx_t * context, struct window * window);
lv_result_t lv_wayland_dmabuf_resize_window(dmabuf_ctx_t * context, struct window * window);
lv_result_t lv_wayland_dmabuf_is_ready(dmabuf_ctx_t * context);

void lv_wayland_dmabuf_destroy_draw_buffers(dmabuf_ctx_t * context, struct window * window);
void lv_wayland_dmabuf_initalize_context(dmabuf_ctx_t * context);
void lv_wayland_dmabuf_deinit(dmabuf_ctx_t * context);
void lv_wayland_dmabuf_flush_full_mode(lv_display_t * disp, const lv_area_t * area, unsigned char * color_p);

/**********************
 *      SME
 **********************/

const struct smm_events * lv_wayland_sme_get_events(void);

/**********************
 *      Seat
 **********************/

const struct wl_seat_listener * lv_wayland_seat_get_listener(void);

/**********************
 *      Input
 **********************/

const struct wl_keyboard_listener * lv_wayland_keyboard_get_listener(void);
const struct wl_pointer_listener * lv_wayland_pointer_get_listener(void);
const struct wl_touch_listener * lv_wayland_touch_get_listener(void);

/**********************
 *      Cache
 **********************/

void lv_wayland_cache_add_area(struct window * window, smm_buffer_t * buf, const lv_area_t * area);
void lv_wayland_cache_clear(struct window * window);
void lv_wayland_cache_apply_areas(struct window * window, void * dest, void * src, smm_buffer_t * src_buf);
void lv_wayland_cache_purge(struct window * window, smm_buffer_t * buf);

#endif /* LV_USE_WAYLAND */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_WAYLAND_PRIVATE_H*/
