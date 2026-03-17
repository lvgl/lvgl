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

static lv_wl_seat_pointer_t * create_pointer(struct wl_seat * wl_seat);
static void delete_pointer(lv_wl_seat_pointer_t * seat_pointer);

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

void lv_wayland_seat_init(lv_wl_seat_t * seat, struct wl_registry * registry, uint32_t name, uint32_t version)
{
    LV_ASSERT_NULL(seat);
    LV_UNUSED(version);
    seat->wl_seat = wl_registry_bind(registry, name, &wl_seat_interface, 1);
    wl_seat_add_listener(seat->wl_seat, &seat_listener, seat);
}

void lv_wayland_seat_deinit(lv_wl_seat_t * seat)
{
    if(seat->pointer) {
        delete_pointer(seat->pointer);
        seat->pointer = NULL;
    }
    if(seat->keyboard) {
        lv_wayland_seat_keyboard_delete(seat->keyboard);
        seat->keyboard = NULL;
    }
    if(seat->touch) {
        lv_wayland_seat_touch_delete(seat->touch);
        seat->touch = NULL;
    }
    wl_seat_destroy(seat->wl_seat);
}

void lv_wayland_update_indevs(lv_indev_read_cb_t read_cb, void * new_driver_data)
{
    lv_indev_t * indev = NULL;
    while((indev = lv_indev_get_next(indev))) {
        if(lv_indev_get_read_cb(indev) != read_cb) {
            continue;
        }
        lv_indev_set_driver_data(indev, new_driver_data);
    }
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_wl_seat_pointer_t * create_pointer(struct wl_seat * wl_seat)
{
    struct wl_surface * surface = wl_compositor_create_surface(lv_wl_ctx.wl_compositor);
    if(!surface) {
        LV_LOG_WARN("Failed to get surface for pointer");
        return NULL;
    }
    lv_wl_seat_pointer_t * seat_pointer = lv_wayland_seat_pointer_create(wl_seat, surface);
    if(!seat_pointer) {
        LV_LOG_WARN("Failed to create seat pointer");
        wl_surface_destroy(surface);
        return NULL;
    }
    return seat_pointer;
}

static void delete_pointer(lv_wl_seat_pointer_t * seat_pointer)
{
    wl_surface_destroy(seat_pointer->cursor_surface);
    lv_wayland_seat_pointer_delete(seat_pointer);
}

static void seat_handle_capabilities(void * data, struct wl_seat * wl_seat, enum wl_seat_capability caps)
{
    lv_wl_seat_t * seat = (lv_wl_seat_t *) data;

    if((caps & WL_SEAT_CAPABILITY_POINTER) && !seat->pointer) {
        seat->pointer = create_pointer(wl_seat);
    }
    else if(!(caps & WL_SEAT_CAPABILITY_POINTER) && seat->pointer) {
        delete_pointer(seat->pointer);
        seat->pointer = NULL;
    }

    if((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !seat->keyboard) {
        seat->keyboard = lv_wayland_seat_keyboard_create(wl_seat);
    }
    else if(!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && seat->keyboard) {
        lv_wayland_seat_keyboard_delete(seat->keyboard);
        seat->keyboard = NULL;
    }

    if((caps & WL_SEAT_CAPABILITY_TOUCH) && !seat->touch) {
        seat->touch = lv_wayland_seat_touch_create(wl_seat);
    }
    else if(!(caps & WL_SEAT_CAPABILITY_TOUCH) && seat->touch) {
        lv_wayland_seat_touch_delete(seat->touch);
        seat->touch = NULL;
    }
}

#endif /* LV_USE_WAYLAND */
