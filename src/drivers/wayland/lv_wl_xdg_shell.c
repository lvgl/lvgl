/**
 * @file lv_wl_xdg_shell.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland_private.h"

#if LV_USE_WAYLAND

#include <linux/input-event-codes.h>
#include "wayland_xdg_shell.h"

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

/**********************
 *   Shell
 **********************/

void lv_wayland_xdg_deinit(void)
{
    if(lv_wl_ctx.xdg_wm) {
        xdg_wm_base_destroy(lv_wl_ctx.xdg_wm);
    }
}

/**********************
 *   Listeners
 **********************/

const struct xdg_wm_base_listener * lv_wayland_xdg_get_wm_base_listener(void)
{
    return &xdg_wm_base_listener;
}

/**********************
 *   Shell Window
 **********************/

void lv_wayland_xdg_set_fullscreen(lv_wl_window_xdg_t * xdg, bool fullscreen,
                                   struct wl_output * output)
{
    LV_ASSERT_NULL(xdg);
    LV_ASSERT_NULL(xdg->xdg_toplevel);
    if(fullscreen) {
        xdg_toplevel_set_fullscreen(xdg->xdg_toplevel, output);
    }
    else {
        xdg_toplevel_unset_fullscreen(xdg->xdg_toplevel);
    }
}

void lv_wayland_xdg_set_maximized(lv_wl_window_xdg_t * xdg, bool maximized)
{
    LV_ASSERT_NULL(xdg);
    LV_ASSERT_NULL(xdg->xdg_toplevel);
    if(maximized) {
        xdg_toplevel_set_maximized(xdg->xdg_toplevel);
    }
    else {
        xdg_toplevel_unset_maximized(xdg->xdg_toplevel);
    }
}

void lv_wayland_xdg_set_minimized(lv_wl_window_xdg_t * xdg)
{
    LV_ASSERT_NULL(xdg);
    LV_ASSERT_NULL(xdg->xdg_toplevel);
    xdg_toplevel_set_minimized(xdg->xdg_toplevel);
}

lv_result_t lv_wl_xdg_create_window(struct xdg_wm_base * xdg_wm, lv_wl_window_t * window, const char * title)
{
    LV_ASSERT_NULL(xdg_wm);

    window->xdg.xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm, window->body);
    if(!window->xdg.xdg_surface) {
        LV_LOG_ERROR("Failed to create XDG surface");
        return LV_RESULT_INVALID;
    }

    window->xdg.xdg_toplevel = xdg_surface_get_toplevel(window->xdg.xdg_surface);
    if(!window->xdg.xdg_toplevel) {
        xdg_surface_destroy(window->xdg.xdg_surface);
        window->xdg.xdg_surface = NULL;
        LV_LOG_ERROR("Failed to acquire XDG toplevel surface");
        return LV_RESULT_INVALID;
    }
    xdg_surface_add_listener(window->xdg.xdg_surface, &xdg_surface_listener, window);
    xdg_toplevel_add_listener(window->xdg.xdg_toplevel, &xdg_toplevel_listener, window);
    xdg_toplevel_set_title(window->xdg.xdg_toplevel, title);
    xdg_toplevel_set_app_id(window->xdg.xdg_toplevel, title);
    return LV_RESULT_OK;
}

void lv_wayland_xdg_configure_surface(lv_wl_window_t * window)
{
    /* XDG surfaces need to be configured before a buffer can be attached.
     * An (XDG) surface commit (without an attached buffer) triggers this
     * configure event */
    wl_surface_commit(window->body);
    wl_display_roundtrip(lv_wl_ctx.wl_display);
    LV_ASSERT_MSG(window->resize_event.pending, "Failed to receive the xdg_surface configuration event");
}
bool lv_wayland_xdg_is_resize_pending(lv_wl_window_t * window)
{
    return window->resize_event.pending;
}

void lv_wayland_xdg_resize(lv_wl_window_t * window)
{
    if(!window->resize_event.pending) {
        return;
    }


    lv_display_set_resolution(window->lv_disp,
                              window->resize_event.width,
                              window->resize_event.height);
    xdg_surface_ack_configure(window->resize_event.xdg_surface, window->resize_event.serial);
    window->resize_event.pending = false;
    window->xdg.configured = true;
}

void lv_wayland_xdg_delete_window(lv_wl_window_xdg_t * xdg)
{
    if(xdg->xdg_toplevel) {
        xdg_toplevel_destroy(xdg->xdg_toplevel);
    }
    if(xdg->xdg_surface) {
        xdg_surface_destroy(xdg->xdg_surface);
    }
    xdg->xdg_surface = NULL;
    xdg->xdg_toplevel = NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void xdg_surface_handle_configure(void * data, struct xdg_surface * xdg_surface, uint32_t serial)
{
    lv_wl_window_t * window = (lv_wl_window_t *)data;

    if(!window->resize_event.requested) {
        LV_LOG_TRACE("resize event not requested. ignoring it");
        window->xdg.configured = true;
        xdg_surface_ack_configure(xdg_surface, serial);
        return;
    }

    LV_LOG_TRACE("resize event requested and now pending");
    window->resize_event.pending = true;
    window->resize_event.requested = false;
    window->resize_event.xdg_surface = xdg_surface;
    window->resize_event.serial = serial;
}

static void xdg_toplevel_handle_configure(void * data, struct xdg_toplevel * xdg_toplevel, int32_t width,
                                          int32_t height, struct wl_array * states)
{
    lv_wl_window_t * window = (lv_wl_window_t *)data;

    LV_UNUSED(xdg_toplevel);
    LV_UNUSED(states);
    LV_LOG_TRACE("XDG toplevel configure: w=%d h=%d (current: %dx%d)",
                 width, height, lv_wayland_window_get_width(window), lv_wayland_window_get_height(window));

    if((width < 0) || (height < 0)) {
        LV_LOG_TRACE("will not resize to w:%d h:%d", width, height);
        return;
    }

    window->xdg.configured = false;
    /* Width and height are already ok, don't resize*/
    if(width == lv_wayland_window_get_width(window) &&
       height == lv_wayland_window_get_height(window)) {
        LV_LOG_TRACE("Window's size is already correct. Ignore resize request");
        return;
    }
    window->resize_event.requested = true;
    window->resize_event.width = width ? width : lv_display_get_horizontal_resolution(window->lv_disp);
    window->resize_event.height = height ? height : lv_display_get_vertical_resolution(window->lv_disp);
}

static void xdg_toplevel_handle_close(void * data, struct xdg_toplevel * xdg_toplevel)
{
    LV_UNUSED(xdg_toplevel);
    lv_wl_window_t * window = (lv_wl_window_t *)data;
    lv_wayland_window_delete(window);
}

static void xdg_wm_base_ping(void * data, struct xdg_wm_base * xdg_wm_base, uint32_t serial)
{
    LV_UNUSED(data);

    xdg_wm_base_pong(xdg_wm_base, serial);

    return;
}

#endif /* LV_USE_WAYLAND */
