/**
 * @file lv_wl_shell.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland.h"

#if LV_WAYLAND_WL_SHELL

/* WL_SHELL has been deprecated for 3 years now */
#warning LV_WAYLAND_WL_SHELL is deprecated and will be removed in a future release

#include "lv_wayland_private.h"
#include <linux/input-event-codes.h>

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

/**********************
 *   PRIVATE FUNCTIONS
 **********************/

void lv_wayland_wl_shell_deinit(void)
{
    if(lv_wl_ctx.wl_shell) {
        wl_shell_destroy(lv_wl_ctx.wl_shell);
    }
}

const struct wl_shell_surface_listener * lv_wayland_wl_shell_get_listener(void)
{
    return &shell_surface_listener;
}

lv_result_t lv_wayland_wl_shell_create_window(struct lv_wayland_context * ctx, struct window * window,
                                              const char * title)
{
    if(!ctx->wl_shell) {
        return LV_RESULT_INVALID;
    }

    window->wl_shell_surface = wl_shell_get_shell_surface(ctx->wl_shell, window->body->surface);
    if(!window->wl_shell_surface) {
        LV_LOG_ERROR("cannot create WL shell surface");
        return LV_RESULT_INVALID;
    }

    wl_shell_surface_add_listener(window->wl_shell_surface, lv_wayland_wl_shell_get_listener(), window);
    wl_shell_surface_set_toplevel(window->wl_shell_surface);
    wl_shell_surface_set_title(window->wl_shell_surface, title);

    /* For wl_shell, just draw the window, weston doesn't send it */
    lv_wayland_window_draw(window, window->width, window->height);
    return LV_RESULT_OK;
}

lv_result_t lv_wayland_wl_shell_set_maximized(struct window * window, bool maximized)
{

    if(!window->wl_shell_surface) {
        return LV_RESULT_INVALID;
    }
    if(maximized) {
        LV_LOG_ERROR("WL_SHELL - Unsupported operation - Maximization");
        return LV_RESULT_INVALID;
    }
    else {
        wl_shell_surface_set_toplevel(window->wl_shell_surface);
    }
    return LV_RESULT_OK;
}

lv_result_t lv_wayland_wl_shell_set_minimized(struct window * window)
{
    LV_LOG_ERROR("WL_SHELL - Unsupported operation - Minization");
    return LV_RESULT_INVALID;
}
lv_result_t lv_wayland_wl_shell_set_fullscreen(struct window * window, bool fullscreen)
{
    if(!window->wl_shell_surface) {
        return LV_RESULT_INVALID;
    }
    if(fullscreen) {
        wl_shell_surface_set_fullscreen(window->wl_shell_surface, WL_SHELL_SURFACE_FULLSCREEN_METHOD_SCALE, 0, NULL);
    }
    else {
        wl_shell_surface_set_toplevel(window->wl_shell_surface);
    }
    return LV_RESULT_OK;
}

lv_result_t lv_wayland_wl_shell_destroy_window(struct window * window)
{
    if(!window->wl_shell_surface) {
        return LV_RESULT_INVALID;
    }
    wl_shell_surface_destroy(window->wl_shell_surface);
    return LV_RESULT_OK;
}

void lv_wayland_wl_shell_handle_pointer_event(struct lv_wayland_context * ctx, uint32_t serial, uint32_t button,
                                              uint32_t state)
{
    struct window * window = ctx->pointer_obj->window;
    switch(ctx->pointer_obj->type) {
        case OBJECT_TITLEBAR:
            if((button == BTN_LEFT) && (state == WL_POINTER_BUTTON_STATE_PRESSED)) {
                if(window->wl_shell_surface) {
                    wl_shell_surface_move(window->wl_shell_surface, ctx->wl_seat, serial);
                    window->flush_pending = true;
                }
            }
            break;
        case OBJECT_BUTTON_CLOSE:
        case OBJECT_BORDER_TOP:
        case OBJECT_BORDER_BOTTOM:
        case OBJECT_BORDER_LEFT:
        case OBJECT_BORDER_RIGHT:
        case OBJECT_WINDOW:
            break;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

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

#endif /* LV_WAYLAND_WL_SHELL */
