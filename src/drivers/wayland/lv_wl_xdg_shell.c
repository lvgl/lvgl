/**
 * @file lv_wl_xdg_shell.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_wayland.h"
#include "wayland_xdg_shell.h"

#if LV_USE_WAYLAND
/*
 * LV_WAYLAND_XDG_SHELL is automatically defined if LV_WAYLAND_WL_SHELL is not set
 * inside lv_wayland_private.h so we need include this header file before checking
 * for LV_WAYLAND_XDG_SHELL
 */
#include "lv_wayland_private.h"

#if LV_WAYLAND_XDG_SHELL
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void xdg_surface_handle_configure(void * data, struct xdg_surface * xdg_surface, uint32_t serial);
static void xdg_toplevel_handle_configure(void * data, struct xdg_toplevel * xdg_toplevel, int32_t width,
                                          int32_t height, struct wl_array * states);
static void xdg_toplevel_handle_close(void * data, struct xdg_toplevel * xdg_toplevel);
static void xdg_wm_base_ping(void * data, struct xdg_wm_base * xdg_wm_base, uint32_t serial);

/**********************
 *  STATIC VARIABLES
 **********************/

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_handle_configure,
};

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = xdg_toplevel_handle_configure,
    .close     = xdg_toplevel_handle_close,
};

static const struct xdg_wm_base_listener xdg_wm_base_listener = {.ping = xdg_wm_base_ping};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   PRIVATE FUNCTIONS
 **********************/

const struct xdg_wm_base_listener * lv_wayland_xdg_shell_get_wm_base_listener(void)
{
    return &xdg_wm_base_listener;
}

const struct xdg_surface_listener * lv_wayland_xdg_shell_get_surface_listener(void)
{
    return &xdg_surface_listener;
}

const struct xdg_toplevel_listener * lv_wayland_xdg_shell_get_toplevel_listener(void)
{
    return &xdg_toplevel_listener;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void xdg_surface_handle_configure(void * data, struct xdg_surface * xdg_surface, uint32_t serial)
{
    struct window * window = (struct window *)data;

    xdg_surface_ack_configure(xdg_surface, serial);

    if(window->body->surface_configured == false) {
        /* This branch is executed at launch */
        if(window->resize_pending == false) {
            /* Use the size passed to the create_window function */
            lv_wayland_window_draw(window, window->width, window->height);
        }
        else {

            /* Handle early maximization or fullscreen, */
            /* by using the size communicated by the compositor */
            /* when the initial xdg configure event arrives  */
            lv_wayland_window_draw(window, window->resize_width, window->resize_height);
            window->width          = window->resize_width;
            window->height         = window->resize_height;
            window->resize_pending = false;
        }
    }
    window->body->surface_configured = true;
}

static void xdg_toplevel_handle_configure(void * data, struct xdg_toplevel * xdg_toplevel, int32_t width,
                                          int32_t height, struct wl_array * states)
{
    struct window * window = (struct window *)data;

    LV_UNUSED(xdg_toplevel);
    LV_UNUSED(states);
    LV_UNUSED(width);
    LV_UNUSED(height);

    LV_LOG_TRACE("w:%d h:%d", width, height);
    LV_LOG_TRACE("current body w:%d h:%d", window->body->width, window->body->height);
    LV_LOG_TRACE("window w:%d h:%d", window->width, window->height);

    if((width <= 0) || (height <= 0)) {
        LV_LOG_TRACE("will not resize to w:%d h:%d", width, height);
        return;
    }

    if((width != window->width) || (height != window->height)) {
        window->resize_width   = width;
        window->resize_height  = height;
        window->resize_pending = true;
        LV_LOG_TRACE("resize_pending is set, will resize to w:%d h:%d", width, height);
    }
    else {
        LV_LOG_TRACE("resize_pending not set w:%d h:%d", width, height);
    }
}

static void xdg_toplevel_handle_close(void * data, struct xdg_toplevel * xdg_toplevel)
{
    struct window * window = (struct window *)data;
    window->shall_close    = true;

    LV_UNUSED(xdg_toplevel);
}

static void xdg_wm_base_ping(void * data, struct xdg_wm_base * xdg_wm_base, uint32_t serial)
{
    LV_UNUSED(data);

    xdg_wm_base_pong(xdg_wm_base, serial);

    return;
}

#endif /* LV_WAYLAND_XDG_SHELL */
#endif /* LV_USE_WAYLAND */
