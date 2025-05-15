/**
 * @file lv_wayland_pointer.c
 *
 */

#include "lv_wl_pointer.h"

#if LV_USE_WAYLAND

#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <wayland-cursor.h>
#include <xkbcommon/xkbcommon.h>
#include <linux/input-event-codes.h>
#include "lv_wayland_private.h"

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void _lv_wayland_pointer_read(lv_indev_t * drv, lv_indev_data_t * data);

static void pointer_handle_enter(void * data, struct wl_pointer * pointer, uint32_t serial, struct wl_surface * surface,
                                 wl_fixed_t sx, wl_fixed_t sy);

static void pointer_handle_leave(void * data, struct wl_pointer * pointer, uint32_t serial,
                                 struct wl_surface * surface);

static void pointer_handle_motion(void * data, struct wl_pointer * pointer, uint32_t time, wl_fixed_t sx,
                                  wl_fixed_t sy);

static void pointer_handle_button(void * data, struct wl_pointer * wl_pointer, uint32_t serial, uint32_t time,
                                  uint32_t button, uint32_t state);

static void pointer_handle_axis(void * data, struct wl_pointer * wl_pointer, uint32_t time, uint32_t axis,
                                wl_fixed_t value);

/**********************
 *  STATIC VARIABLES
 **********************/

static const struct wl_pointer_listener pointer_listener = {
    .enter  = pointer_handle_enter,
    .leave  = pointer_handle_leave,
    .motion = pointer_handle_motion,
    .button = pointer_handle_button,
    .axis   = pointer_handle_axis,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_indev_t * lv_wayland_pointer_create(void)
{
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, _lv_wayland_pointer_read);
    return indev;
}

lv_indev_t * lv_wayland_get_pointer(lv_display_t * disp)
{
    struct window * window = lv_display_get_user_data(disp);
    if(!window) {
        return NULL;
    }
    return window->lv_indev_pointer;
}

/**********************
 *   PRIVATE FUNCTIONS
 **********************/

const struct wl_pointer_listener * lv_wayland_pointer_get_listener(void)
{
    return &pointer_listener;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _lv_wayland_pointer_read(lv_indev_t * drv, lv_indev_data_t * data)
{
    struct window * window = lv_display_get_user_data(lv_indev_get_display(drv));

    if(!window || window->closed) {
        return;
    }

    data->point.x = window->body->input.pointer.x;
    data->point.y = window->body->input.pointer.y;
    data->state   = window->body->input.pointer.left_button;
}

static void pointer_handle_enter(void * data, struct wl_pointer * pointer, uint32_t serial, struct wl_surface * surface,
                                 wl_fixed_t sx, wl_fixed_t sy)
{
    struct application * app = data;
    const char * cursor      = "left_ptr";
    int pos_x                = wl_fixed_to_int(sx);
    int pos_y                = wl_fixed_to_int(sy);

    if(!surface) {
        app->pointer_obj = NULL;
        return;
    }

    app->pointer_obj = wl_surface_get_user_data(surface);

    app->pointer_obj->input.pointer.x = pos_x;
    app->pointer_obj->input.pointer.y = pos_y;

#if (LV_WAYLAND_WINDOW_DECORATIONS && LV_WAYLAND_XDG_SHELL)
    if(!app->pointer_obj->window->xdg_toplevel || app->opt_disable_decorations) {
        return;
    }

    struct window * window = app->pointer_obj->window;

    switch(app->pointer_obj->type) {
        case OBJECT_BORDER_TOP:
            if(window->maximized) {
                // do nothing
            }
            else if(pos_x < (BORDER_SIZE * 5)) {
                cursor = "top_left_corner";
            }
            else if(pos_x >= (window->width + BORDER_SIZE - (BORDER_SIZE * 5))) {
                cursor = "top_right_corner";
            }
            else {
                cursor = "top_side";
            }
            break;
        case OBJECT_BORDER_BOTTOM:
            if(window->maximized) {
                // do nothing
            }
            else if(pos_x < (BORDER_SIZE * 5)) {
                cursor = "bottom_left_corner";
            }
            else if(pos_x >= (window->width + BORDER_SIZE - (BORDER_SIZE * 5))) {
                cursor = "bottom_right_corner";
            }
            else {
                cursor = "bottom_side";
            }
            break;
        case OBJECT_BORDER_LEFT:
            if(window->maximized) {
                // do nothing
            }
            else if(pos_y < (BORDER_SIZE * 5)) {
                cursor = "top_left_corner";
            }
            else if(pos_y >= (window->height + BORDER_SIZE - (BORDER_SIZE * 5))) {
                cursor = "bottom_left_corner";
            }
            else {
                cursor = "left_side";
            }
            break;
        case OBJECT_BORDER_RIGHT:
            if(window->maximized) {
                // do nothing
            }
            else if(pos_y < (BORDER_SIZE * 5)) {
                cursor = "top_right_corner";
            }
            else if(pos_y >= (window->height + BORDER_SIZE - (BORDER_SIZE * 5))) {
                cursor = "bottom_right_corner";
            }
            else {
                cursor = "right_side";
            }
            break;
        default:
            break;
    }
#endif

    if(app->cursor_surface) {
        struct wl_cursor_image * cursor_image = wl_cursor_theme_get_cursor(app->cursor_theme, cursor)->images[0];
        wl_pointer_set_cursor(pointer, serial, app->cursor_surface, cursor_image->hotspot_x, cursor_image->hotspot_y);
        wl_surface_attach(app->cursor_surface, wl_cursor_image_get_buffer(cursor_image), 0, 0);
        wl_surface_damage(app->cursor_surface, 0, 0, cursor_image->width, cursor_image->height);
        wl_surface_commit(app->cursor_surface);
        app->cursor_flush_pending = true;
    }
}

static void pointer_handle_leave(void * data, struct wl_pointer * pointer, uint32_t serial, struct wl_surface * surface)
{
    struct application * app = data;

    LV_UNUSED(pointer);
    LV_UNUSED(serial);

    if(!surface || (app->pointer_obj == wl_surface_get_user_data(surface))) {
        app->pointer_obj = NULL;
    }
}

static void pointer_handle_motion(void * data, struct wl_pointer * pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
    struct application * app = data;

    LV_UNUSED(pointer);
    LV_UNUSED(time);

    if(!app->pointer_obj) {
        return;
    }

    app->pointer_obj->input.pointer.x = LV_MAX(0, LV_MIN(wl_fixed_to_int(sx), app->pointer_obj->width - 1));
    app->pointer_obj->input.pointer.y = LV_MAX(0, LV_MIN(wl_fixed_to_int(sy), app->pointer_obj->height - 1));
}

static void pointer_handle_button(void * data, struct wl_pointer * wl_pointer, uint32_t serial, uint32_t time,
                                  uint32_t button, uint32_t state)
{
    struct application * app = data;

    LV_UNUSED(serial);
    LV_UNUSED(wl_pointer);
    LV_UNUSED(time);

    const lv_indev_state_t lv_state =
        (state == WL_POINTER_BUTTON_STATE_PRESSED) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;

    if(!app->pointer_obj) {
        return;
    }

#if LV_WAYLAND_WINDOW_DECORATIONS
    struct window * window;
    window    = app->pointer_obj->window;
    int pos_x = app->pointer_obj->input.pointer.x;
    int pos_y = app->pointer_obj->input.pointer.y;
#endif

    switch(app->pointer_obj->type) {
        case OBJECT_WINDOW:
            switch(button) {
                case BTN_LEFT:
                    app->pointer_obj->input.pointer.left_button = lv_state;
                    break;
                case BTN_RIGHT:
                    app->pointer_obj->input.pointer.right_button = lv_state;
                    break;
                case BTN_MIDDLE:
                    app->pointer_obj->input.pointer.wheel_button = lv_state;
                    break;
                default:
                    break;
            }

            break;
#if LV_WAYLAND_WINDOW_DECORATIONS
        case OBJECT_TITLEBAR:
            if((button == BTN_LEFT) && (state == WL_POINTER_BUTTON_STATE_PRESSED)) {
#if LV_WAYLAND_XDG_SHELL
                if(window->xdg_toplevel) {
                    xdg_toplevel_move(window->xdg_toplevel, app->wl_seat, serial);
                    window->flush_pending = true;
                }
#endif
#if LV_WAYLAND_WL_SHELL
                if(window->wl_shell_surface) {
                    wl_shell_surface_move(window->wl_shell_surface, app->wl_seat, serial);
                    window->flush_pending = true;
                }
#endif
            }
            break;
        case OBJECT_BUTTON_CLOSE:
            if((button == BTN_LEFT) && (state == WL_POINTER_BUTTON_STATE_RELEASED)) {
                window->shall_close = true;
            }
            break;
#if LV_WAYLAND_XDG_SHELL
        case OBJECT_BUTTON_MAXIMIZE:
            if((button == BTN_LEFT) && (state == WL_POINTER_BUTTON_STATE_RELEASED)) {
                if(window->xdg_toplevel) {
                    if(window->maximized) {
                        xdg_toplevel_unset_maximized(window->xdg_toplevel);
                    }
                    else {
                        xdg_toplevel_set_maximized(window->xdg_toplevel);
                    }
                    window->maximized ^= true;
                    window->flush_pending = true;
                }
            }
            break;
        case OBJECT_BUTTON_MINIMIZE:
            if((button == BTN_LEFT) && (state == WL_POINTER_BUTTON_STATE_RELEASED)) {
                if(window->xdg_toplevel) {
                    xdg_toplevel_set_minimized(window->xdg_toplevel);
                    window->flush_pending = true;
                }
            }
            break;
        case OBJECT_BORDER_TOP:
            if((button == BTN_LEFT) && (state == WL_POINTER_BUTTON_STATE_PRESSED)) {
                if(window->xdg_toplevel && !window->maximized) {
                    uint32_t edge;
                    if(pos_x < (BORDER_SIZE * 5)) {
                        edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP_LEFT;
                    }
                    else if(pos_x >= (window->width + BORDER_SIZE - (BORDER_SIZE * 5))) {
                        edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT;
                    }
                    else {
                        edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP;
                    }
                    xdg_toplevel_resize(window->xdg_toplevel, window->application->wl_seat, serial, edge);
                    window->flush_pending = true;
                }
            }
            break;
        case OBJECT_BORDER_BOTTOM:
            if((button == BTN_LEFT) && (state == WL_POINTER_BUTTON_STATE_PRESSED)) {
                if(window->xdg_toplevel && !window->maximized) {
                    uint32_t edge;
                    if(pos_x < (BORDER_SIZE * 5)) {
                        edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT;
                    }
                    else if(pos_x >= (window->width + BORDER_SIZE - (BORDER_SIZE * 5))) {
                        edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT;
                    }
                    else {
                        edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM;
                    }
                    xdg_toplevel_resize(window->xdg_toplevel, window->application->wl_seat, serial, edge);
                    window->flush_pending = true;
                }
            }
            break;
        case OBJECT_BORDER_LEFT:
            if((button == BTN_LEFT) && (state == WL_POINTER_BUTTON_STATE_PRESSED)) {
                if(window->xdg_toplevel && !window->maximized) {
                    uint32_t edge;
                    if(pos_y < (BORDER_SIZE * 5)) {
                        edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP_LEFT;
                    }
                    else if(pos_y >= (window->height + BORDER_SIZE - (BORDER_SIZE * 5))) {
                        edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT;
                    }
                    else {
                        edge = XDG_TOPLEVEL_RESIZE_EDGE_LEFT;
                    }
                    xdg_toplevel_resize(window->xdg_toplevel, window->application->wl_seat, serial, edge);
                    window->flush_pending = true;
                }
            }
            break;
        case OBJECT_BORDER_RIGHT:
            if((button == BTN_LEFT) && (state == WL_POINTER_BUTTON_STATE_PRESSED)) {
                if(window->xdg_toplevel && !window->maximized) {
                    uint32_t edge;
                    if(pos_y < (BORDER_SIZE * 5)) {
                        edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT;
                    }
                    else if(pos_y >= (window->height + BORDER_SIZE - (BORDER_SIZE * 5))) {
                        edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT;
                    }
                    else {
                        edge = XDG_TOPLEVEL_RESIZE_EDGE_RIGHT;
                    }
                    xdg_toplevel_resize(window->xdg_toplevel, window->application->wl_seat, serial, edge);
                    window->flush_pending = true;
                }
            }
            break;
#endif // LV_WAYLAND_XDG_SHELL
#endif // LV_WAYLAND_WINDOW_DECORATIONS
        default:
            break;
    }
}

static void pointer_handle_axis(void * data, struct wl_pointer * wl_pointer, uint32_t time, uint32_t axis,
                                wl_fixed_t value)
{
    struct application * app = data;
    const int diff           = wl_fixed_to_int(value);

    LV_UNUSED(time);
    LV_UNUSED(wl_pointer);

    if(!app->pointer_obj) {
        return;
    }

    if(axis == 0) {
        if(diff > 0) {
            app->pointer_obj->input.pointer.wheel_diff++;
        }
        else if(diff < 0) {
            app->pointer_obj->input.pointer.wheel_diff--;
        }
    }
}

#endif /* LV_USE_WAYLAND */
