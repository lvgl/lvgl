/**
 * @file lv_wl_seat.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_wayland.h"

#if LV_USE_WAYLAND

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

static void seat_handle_capabilities(void * data, struct wl_seat * wl_seat, enum wl_seat_capability caps);

/**********************
 *  STATIC VARIABLES
 **********************/

static const struct wl_seat_listener seat_listener = {
    .capabilities = seat_handle_capabilities,
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

const struct wl_seat_listener * lv_wayland_seat_get_listener(void)
{
    return &seat_listener;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void seat_handle_capabilities(void * data, struct wl_seat * wl_seat, enum wl_seat_capability caps)
{
    struct lv_wayland_context * app = data;
    struct seat * seat       = &app->seat;

    if((caps & WL_SEAT_CAPABILITY_POINTER) && !seat->wl_pointer) {
        seat->wl_pointer = wl_seat_get_pointer(wl_seat);
        wl_pointer_add_listener(seat->wl_pointer, lv_wayland_pointer_get_listener(), app);
        app->cursor_surface = wl_compositor_create_surface(app->compositor);
        if(!app->cursor_surface) {
            LV_LOG_WARN("failed to create cursor surface");
        }
    }
    else if(!(caps & WL_SEAT_CAPABILITY_POINTER) && seat->wl_pointer) {
        wl_pointer_destroy(seat->wl_pointer);
        if(app->cursor_surface) {
            wl_surface_destroy(app->cursor_surface);
        }
        seat->wl_pointer = NULL;
    }

    if((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !seat->wl_keyboard) {
        seat->wl_keyboard = wl_seat_get_keyboard(wl_seat);
        wl_keyboard_add_listener(seat->wl_keyboard, lv_wayland_keyboard_get_listener(), app);
    }
    else if(!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && seat->wl_keyboard) {
        wl_keyboard_destroy(seat->wl_keyboard);
        seat->wl_keyboard = NULL;
    }

    if((caps & WL_SEAT_CAPABILITY_TOUCH) && !seat->wl_touch) {
        seat->wl_touch = wl_seat_get_touch(wl_seat);
        wl_touch_add_listener(seat->wl_touch, lv_wayland_touch_get_listener(), app);
    }
    else if(!(caps & WL_SEAT_CAPABILITY_TOUCH) && seat->wl_touch) {
        wl_touch_destroy(seat->wl_touch);
        seat->wl_touch = NULL;
    }
}

#endif /* LV_USE_WAYLAND */
