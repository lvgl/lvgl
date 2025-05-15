/**
 * @file lv_wl_shell.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland.h"

#if LV_WAYLAND_WL_SHELL

#include "lv_wayland_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void wl_shell_handle_ping(void * data, struct wl_shell_surface * shell_surface, uint32_t serial);
static void wl_shell_handle_configure(void * data, struct wl_shell_surface * shell_surface, uint32_t edges,
                                      int32_t width, int32_t height);

/**********************
 *  STATIC VARIABLES
 **********************/

static const struct wl_shell_surface_listener shell_surface_listener = {
    .ping      = wl_shell_handle_ping,
    .configure = wl_shell_handle_configure,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

const struct wl_shell_surface_listener * lv_wayland_wl_shell_get_listener(void)
{
    return &shell_surface_listener;
}

static void wl_shell_handle_ping(void * data, struct wl_shell_surface * shell_surface, uint32_t serial)
{
    return wl_shell_surface_pong(shell_surface, serial);
}

static void wl_shell_handle_configure(void * data, struct wl_shell_surface * shell_surface, uint32_t edges,
                                      int32_t width, int32_t height)
{
    struct window * window = (struct window *)data;

    LV_UNUSED(edges);

    if((width <= 0) || (height <= 0)) {
        return;
    }
    else if((width != window->width) || (height != window->height)) {
        window->resize_width   = width;
        window->resize_height  = height;
        window->resize_pending = true;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_WAYLAND_WL_SHELL */
