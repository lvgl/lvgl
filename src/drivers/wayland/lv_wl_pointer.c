/**
 * @file lv_wl_pointer.c
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


static void pointer_read(lv_indev_t * indev, lv_indev_data_t * data);
static void pointeraxis_read(lv_indev_t * indev, lv_indev_data_t * data);

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

static struct wl_cursor_theme * cursor_theme = NULL;

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
    if(!indev) {
        return NULL;
    }
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, pointer_read);
    lv_indev_set_driver_data(indev, lv_wl_ctx.seat.pointer);
    return indev;
}

lv_indev_t * lv_wayland_get_pointer(lv_display_t * disp)
{
    lv_wl_window_t * window = lv_display_get_driver_data(disp);
    if(!window) {
        return NULL;
    }
    return window->lv_indev_pointer;
}

lv_indev_t * lv_wayland_pointer_axis_create(void)
{
    lv_indev_t * indev = lv_indev_create();
    if(!indev) {
        return NULL;
    }
    lv_indev_set_type(indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(indev, pointeraxis_read);
    lv_indev_set_driver_data(indev, lv_wl_ctx.seat.pointer);
    return indev;
}

lv_indev_t * lv_wayland_get_pointeraxis(lv_display_t * display)
{
    lv_wl_window_t * window = lv_display_get_driver_data(display);
    if(!window) {
        return NULL;
    }
    return window->lv_indev_pointeraxis;
}

lv_wl_seat_pointer_t * lv_wayland_seat_pointer_create(struct wl_seat * seat, struct wl_surface * surface)
{
    LV_ASSERT_NULL(seat);
    LV_ASSERT_NULL(surface);
    if(!cursor_theme && !(cursor_theme = wl_cursor_theme_load(NULL, 32, lv_wl_ctx.wl_shm))) {
        LV_LOG_WARN("Failed to load cursor theme for pointer");
        return NULL;
    }

    struct wl_pointer * pointer = wl_seat_get_pointer(seat);
    if(!pointer) {
        LV_LOG_WARN("Failed to get seat pointer");
        return NULL;
    }

    lv_wl_seat_pointer_t * wl_seat_pointer = lv_zalloc(sizeof(*wl_seat_pointer));
    LV_ASSERT_MALLOC(wl_seat_pointer);
    if(!wl_seat_pointer) {
        LV_LOG_WARN("Failed to allocate memory for wayland pointer");
        return NULL;
    }
    wl_pointer_add_listener(pointer, &pointer_listener, NULL);
    wl_pointer_set_user_data(pointer, wl_seat_pointer);

    wl_seat_pointer->cursor_surface = surface;
    wl_seat_pointer->wl_pointer = pointer;
    lv_wayland_update_indevs(pointer_read, wl_seat_pointer);
    lv_wayland_update_indevs(pointeraxis_read, wl_seat_pointer);

    return wl_seat_pointer;
}

void lv_wayland_seat_pointer_delete(lv_wl_seat_pointer_t * seat_pointer)
{
    lv_wayland_update_indevs(pointer_read, NULL);
    lv_wayland_update_indevs(pointeraxis_read, NULL);
    wl_pointer_destroy(seat_pointer->wl_pointer);
    lv_free(seat_pointer);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void pointeraxis_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_wl_seat_pointer_t * seat_pointer = lv_indev_get_driver_data(indev);
    if(!seat_pointer) {
        return;
    }

    data->state = seat_pointer->wheel_btn_state;
    data->enc_diff = seat_pointer->wheel_diff;
    seat_pointer->wheel_diff = 0;
}
static void pointer_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_wl_seat_pointer_t * seat_pointer = lv_indev_get_driver_data(indev);

    if(!seat_pointer) {
        return;
    }
    data->point = seat_pointer->point;
    data->state = seat_pointer->left_btn_state;
}

static void pointer_handle_enter(void * data, struct wl_pointer * pointer, uint32_t serial, struct wl_surface * surface,
                                 wl_fixed_t sx, wl_fixed_t sy)
{
    LV_UNUSED(data);
    LV_UNUSED(surface);
    lv_wl_seat_pointer_t * seat_pointer = wl_pointer_get_user_data(pointer);
    int pos_x = wl_fixed_to_int(sx);
    int pos_y = wl_fixed_to_int(sy);

    seat_pointer->point.x = pos_x;
    seat_pointer->point.y = pos_y;

    struct wl_cursor * wl_cursor = wl_cursor_theme_get_cursor(cursor_theme, LV_WAYLAND_DEFAULT_CURSOR_NAME);
    struct wl_cursor_image * cursor_image = wl_cursor->images[0];

    wl_pointer_set_cursor(pointer, serial, seat_pointer->cursor_surface, cursor_image->hotspot_x, cursor_image->hotspot_y);

    wl_surface_attach(seat_pointer->cursor_surface, wl_cursor_image_get_buffer(cursor_image), 0, 0);
    wl_surface_damage(seat_pointer->cursor_surface, 0, 0, cursor_image->width, cursor_image->height);
    wl_surface_commit(seat_pointer->cursor_surface);
}


static void pointer_handle_leave(void * data, struct wl_pointer * pointer, uint32_t serial, struct wl_surface * surface)
{

    LV_UNUSED(data);
    LV_UNUSED(serial);
    LV_UNUSED(surface);
    LV_UNUSED(pointer);
}

static void pointer_handle_motion(void * data, struct wl_pointer * pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
    LV_UNUSED(data);
    LV_UNUSED(time);

    lv_wl_seat_pointer_t * seat_pointer = wl_pointer_get_user_data(pointer);
    LV_ASSERT_NULL(seat_pointer);

    seat_pointer->point.x = wl_fixed_to_int(sx);
    seat_pointer->point.y = wl_fixed_to_int(sy);
}

static void pointer_handle_button(void * data, struct wl_pointer * pointer, uint32_t serial, uint32_t time,
                                  uint32_t button, uint32_t state)
{
    LV_UNUSED(data);
    LV_UNUSED(serial);
    LV_UNUSED(time);
    lv_wl_seat_pointer_t * seat_pointer = wl_pointer_get_user_data(pointer);
    LV_ASSERT_NULL(seat_pointer);
    const lv_indev_state_t lv_state = (state == WL_POINTER_BUTTON_STATE_PRESSED) ?
                                      LV_INDEV_STATE_PRESSED :
                                      LV_INDEV_STATE_RELEASED;

    if(button == BTN_LEFT) {
        seat_pointer->left_btn_state = lv_state;
    }
    else if(button == BTN_RIGHT) {
        seat_pointer->right_btn_state = lv_state;
    }
    else if(button == BTN_MIDDLE) {
        seat_pointer->wheel_btn_state = lv_state;
    }


}

static void pointer_handle_axis(void * data, struct wl_pointer * pointer, uint32_t time, uint32_t axis,
                                wl_fixed_t value)
{
    LV_UNUSED(data);
    LV_UNUSED(time);
    lv_wl_seat_pointer_t * seat_pointer = wl_pointer_get_user_data(pointer);
    const int diff = wl_fixed_to_int(value);
    if(axis != 0) {
        return;
    }
    seat_pointer->wheel_diff += diff;
}

#endif /* LV_USE_WAYLAND */
