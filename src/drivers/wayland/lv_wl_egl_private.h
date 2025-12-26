/**
 * @file lv_wl_egl_private.h
 *
 */

/**
 * Modified by Renesas in 2025
 */

#ifndef LV_WL_EGL_PRIVATE_H
#define LV_WL_EGL_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "../../../lvgl.h"

#if LV_USE_WAYLAND && LV_WAYLAND_USE_EGL

#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "lv_wayland_private.h"
#include <xkbcommon/xkbcommon.h>
#include "../../indev/lv_indev.h"
#include "../../indev/lv_indev_gesture.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#define LV_WL_EGL_INDEX_COUNT  6

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

#ifndef GL_UNPACK_ROW_LENGTH
#define GL_UNPACK_ROW_LENGTH 0x0CF2
#endif

/*****************************************************************************
 * TYPEDEFS
 *****************************************************************************/

typedef struct lv_wl_egl_window lv_wl_egl_window_t;

/* OpenGL ES rendering context structure */
typedef struct {
    /* OpenGL resources */
    GLuint texture_id;
    GLuint shader_program;
    GLuint vbo;
    GLuint ebo;

    /* Framebuffer for software rendering */
    uint8_t * fb1;
    uint8_t * fb2;
} lv_wl_egl_context_t;

/* Input device structures */
typedef struct {
    struct wl_pointer * wl_pointer;
    struct wl_keyboard * wl_keyboard;
    struct wl_touch * wl_touch;

    struct {
        struct xkb_keymap * keymap;
        struct xkb_state * state;
    } xkb;

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

} lv_wl_egl_indev_t;

/* Wayland window structure */
struct lv_wl_egl_window {
    /* Wayland objects */
    struct wl_display * display;
    struct wl_registry * registry;
    struct wl_compositor * compositor;
    struct wl_surface * surface;
    struct wl_egl_window * egl_window;
    struct wl_region * region;
    struct wl_seat * wl_seat;
    struct wl_shm * shm;
    struct wl_cursor_theme * cursor_theme;
    struct wl_surface * cursor_surface;

    struct xdg_wm_base * xdg_wm_base;
    struct xdg_surface * xdg_surface;
    struct xdg_toplevel * xdg_toplevel;

    /* Input device */
    lv_wl_egl_indev_t indev;
    struct xkb_context * xkb_context;

    /* EGL objects */
    EGLDisplay egl_display;
    EGLContext egl_context;
    EGLSurface egl_surface;
    EGLConfig egl_config;

    /* OpenGL ES context */
    lv_wl_egl_context_t * gl_context;

#if LV_WAYLAND_USE_EGL_DMABUF
    dmabuf_ctx_t * dmabuf_ctx;
    bool use_dmabuf;
    struct gbm_device * gbm_device;
    int drm_fd;
    int current_buffer;
#endif

    /* LVGL display */
    lv_display_t * lv_display;
    lv_indev_t * lv_indev_pointer;
    lv_indev_t * lv_indev_pointeraxis;
    lv_indev_t * lv_indev_touch;
    lv_indev_t * lv_indev_keyboard;

    /* Window properties */
    int32_t width;
    int32_t height;
    char * title;

    bool resize_pending;
    int resize_width;
    int resize_height;

    bool maximized;
    bool fullscreen;
    bool resizing;

    bool cursor_flush_pending;
};

/*****************************************************************************
 * GLOBAL PROTOTYPES
 *****************************************************************************/

/**
 * Handle Wayland registry for egl seat
 *
 * @return pointer to wl_seat_listener structure
 */
const struct wl_seat_listener * lv_wl_egl_seat_get_listener(void);

/**
 * Register input devices keyboard, mouse, and touchpad into the Wayland window
 * @param window Pointer to window structure
 * @return true when input devices initialization success
 */
bool lv_wl_egl_indev_init(lv_wl_egl_window_t * window);

#ifdef __cplusplus
}
#endif

#endif

#endif /* LV_WL_EGL_PRIVATE_H */
