/**
 * @file lv_wl_touch.c
 *
 */

#include "lv_wl_touch.h"

#if LV_USE_WAYLAND

#include "lv_wayland_private.h"

#include <wayland-client-protocol.h>
#include <wayland-cursor.h>

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

static void _lv_wayland_touch_read(lv_indev_t * drv, lv_indev_data_t * data);

static void touch_handle_down(void * data, struct wl_touch * wl_touch, uint32_t serial, uint32_t time,
                              struct wl_surface * surface, int32_t id, wl_fixed_t x_w, wl_fixed_t y_w);

static void touch_handle_up(void * data, struct wl_touch * wl_touch, uint32_t serial, uint32_t time, int32_t id);

static void touch_handle_motion(void * data, struct wl_touch * wl_touch, uint32_t time, int32_t id, wl_fixed_t x_w,
                                wl_fixed_t y_w);

static void touch_handle_frame(void * data, struct wl_touch * wl_touch);

static void touch_handle_cancel(void * data, struct wl_touch * wl_touch);

/**********************
 *  STATIC VARIABLES
 **********************/

static const struct wl_touch_listener touch_listener = {
    .down   = touch_handle_down,
    .up     = touch_handle_up,
    .motion = touch_handle_motion,
    .frame  = touch_handle_frame,
    .cancel = touch_handle_cancel,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_indev_t * lv_wayland_touch_create(void)
{

    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, _lv_wayland_touch_read);

    return indev;
}

lv_indev_t * lv_wayland_get_touchscreen(lv_display_t * display)
{
    struct window * window = lv_display_get_user_data(display);
    if(!window) {
        return NULL;
    }
    return window->lv_indev_touch;
}

/**********************
 *   PRIVATE FUNCTIONS
 **********************/

const struct wl_touch_listener * lv_wayland_touch_get_listener(void)
{
    return &touch_listener;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _lv_wayland_touch_read(lv_indev_t * drv, lv_indev_data_t * data)
{

    struct window * window = lv_display_get_user_data(lv_indev_get_display(drv));

    if(!window || window->closed) {
        return;
    }

#if LV_USE_GESTURE_RECOGNITION
    /* Collect touches if there are any - send them to the gesture recognizer */
    lv_indev_gesture_recognizers_update(drv, &window->body->input.touches[0], window->body->input.touch_event_cnt);

    LV_LOG_TRACE("collected touch events: %d", window->body->input.touch_event_cnt);

    window->body->input.touch_event_cnt = 0;

    /* Set the gesture information, before returning to LVGL */
    lv_indev_gesture_recognizers_set_data(drv, data);

    if(window->body->input.touch_event_cnt > 0) {
        data->point.x = window->body->input.touches[0].point.x;
        data->point.y = window->body->input.touches[0].point.y;
    }
    else {
        data->point.x = 0;
        data->point.y = 0;
    }
#else
    data->point.x = window->body->input.touch.point.x;
    data->point.y = window->body->input.touch.point.y;
    data->state = window->body->input.touch.state;
#endif
}

static void touch_handle_down(void * data, struct wl_touch * wl_touch, uint32_t serial, uint32_t time,
                              struct wl_surface * surface, int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
    struct lv_wayland_context * app = data;
#if LV_USE_GESTURE_RECOGNITION
    uint8_t i;
#endif

    LV_UNUSED(id);
    LV_UNUSED(time);
    LV_UNUSED(serial);
    LV_UNUSED(wl_touch);

    if(!surface) {
        app->touch_obj = NULL;
        return;
    }

    /* Create the touch down event */
    app->touch_obj = wl_surface_get_user_data(surface);

#if LV_USE_GESTURE_RECOGNITION
    i              = app->touch_obj->input.touch_event_cnt;

    app->touch_obj->input.touches[i].point.x   = wl_fixed_to_int(x_w);
    app->touch_obj->input.touches[i].point.y   = wl_fixed_to_int(y_w);
    app->touch_obj->input.touches[i].id        = id;
    app->touch_obj->input.touches[i].timestamp = time;
    app->touch_obj->input.touches[i].state     = LV_INDEV_STATE_PRESSED;
    app->touch_obj->input.touch_event_cnt++;
#else
    app->touch_obj->input.touch.point.x = wl_fixed_to_int(x_w);
    app->touch_obj->input.touch.point.y = wl_fixed_to_int(y_w);
    app->touch_obj->input.touch.state = LV_INDEV_STATE_PRESSED;
#endif

#if LV_WAYLAND_WINDOW_DECORATIONS
    struct window * window = app->touch_obj->window;
    switch(app->touch_obj->type) {
        case OBJECT_TITLEBAR:
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
            break;
        default:
            break;
    }
#endif
}

static void touch_handle_up(void * data, struct wl_touch * wl_touch, uint32_t serial, uint32_t time, int32_t id)
{
    struct lv_wayland_context * app = data;
#if LV_USE_GESTURE_RECOGNITION
    uint8_t i;
#endif

    LV_UNUSED(serial);
    LV_UNUSED(time);
    LV_UNUSED(id);
    LV_UNUSED(wl_touch);

    /* Create a released event */
#if LV_USE_GESTURE_RECOGNITION
    i = app->touch_obj->input.touch_event_cnt;

    app->touch_obj->input.touches[i].point.x   = 0;
    app->touch_obj->input.touches[i].point.y   = 0;
    app->touch_obj->input.touches[i].id        = id;
    app->touch_obj->input.touches[i].timestamp = time;
    app->touch_obj->input.touches[i].state     = LV_INDEV_STATE_RELEASED;

    app->touch_obj->input.touch_event_cnt++;
#else
    app->touch_obj->input.touch.state = LV_INDEV_STATE_RELEASED;
#endif

#if LV_WAYLAND_WINDOW_DECORATIONS
    struct window * window = app->touch_obj->window;
    switch(app->touch_obj->type) {
        case OBJECT_BUTTON_CLOSE:
            window->shall_close = true;
            break;
#if LV_WAYLAND_XDG_SHELL
        case OBJECT_BUTTON_MAXIMIZE:
            if(window->xdg_toplevel) {
                if(window->maximized) {
                    xdg_toplevel_unset_maximized(window->xdg_toplevel);
                }
                else {
                    xdg_toplevel_set_maximized(window->xdg_toplevel);
                }
                window->maximized ^= true;
            }
            break;
        case OBJECT_BUTTON_MINIMIZE:
            if(window->xdg_toplevel) {
                xdg_toplevel_set_minimized(window->xdg_toplevel);
                window->flush_pending = true;
            }
#endif /* LV_WAYLAND_XDG_SHELL */
        default:
            break;
    }
#endif /* LV_WAYLAND_WINDOW_DECORATIONS */
}

static void touch_handle_motion(void * data, struct wl_touch * wl_touch, uint32_t time, int32_t id, wl_fixed_t x_w,
                                wl_fixed_t y_w)
{
    struct lv_wayland_context * app = data;
#if LV_USE_GESTURE_RECOGNITION
    lv_indev_touch_data_t * touch;
    lv_indev_touch_data_t * cur;
    uint8_t i;
#endif

    LV_UNUSED(time);
    LV_UNUSED(id);
    LV_UNUSED(wl_touch);

#if LV_USE_GESTURE_RECOGNITION
    /* Update the contact point of the corresponding id with the latest coordinate */
    touch = &app->touch_obj->input.touches[0];
    cur   = NULL;

    for(i = 0; i < app->touch_obj->input.touch_event_cnt; i++) {
        if(touch->id == id) {
            cur = touch;
        }
        touch++;
    }

    if(cur == NULL) {

        i                                          = app->touch_obj->input.touch_event_cnt;
        app->touch_obj->input.touches[i].point.x   = wl_fixed_to_int(x_w);
        app->touch_obj->input.touches[i].point.y   = wl_fixed_to_int(y_w);
        app->touch_obj->input.touches[i].id        = id;
        app->touch_obj->input.touches[i].timestamp = time;
        app->touch_obj->input.touches[i].state     = LV_INDEV_STATE_PRESSED;
        app->touch_obj->input.touch_event_cnt++;

    }
    else {

        cur->point.x   = wl_fixed_to_int(x_w);
        cur->point.y   = wl_fixed_to_int(y_w);
        cur->id        = id;
        cur->timestamp = time;
    }
#else
    app->touch_obj->input.touch.point.x = wl_fixed_to_int(x_w);
    app->touch_obj->input.touch.point.y = wl_fixed_to_int(y_w);
#endif
}

static void touch_handle_frame(void * data, struct wl_touch * wl_touch)
{
    LV_UNUSED(wl_touch);
    LV_UNUSED(data);
}

static void touch_handle_cancel(void * data, struct wl_touch * wl_touch)
{
    LV_UNUSED(wl_touch);
    LV_UNUSED(data);
}

#endif /* LV_USE_WAYLAND */
