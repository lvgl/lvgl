/**
 * @file lv_wayland_touch.c
 *
 */

#include "lv_wayland_private.h"

#if LV_USE_WAYLAND

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

static void touch_read(lv_indev_t * drv, lv_indev_data_t * data);

static void touch_handle_down(void * data, struct wl_touch * wl_touch, uint32_t serial, uint32_t time,
                              struct wl_surface * surface, int32_t id, wl_fixed_t x_w, wl_fixed_t y_w);

static void touch_handle_up(void * data, struct wl_touch * wl_touch, uint32_t serial, uint32_t time, int32_t id);

static void touch_handle_motion(void * data, struct wl_touch * wl_touch, uint32_t time, int32_t id, wl_fixed_t x_w,
                                wl_fixed_t y_w);

static void touch_handle_frame(void * data, struct wl_touch * wl_touch);

static void touch_handle_cancel(void * data, struct wl_touch * wl_touch);

#if !LV_USE_GESTURE_RECOGNITION
    static lv_wl_touch_point_t * get_pressed_touch_point(lv_wl_seat_touch_t * tdata, int32_t id);
    static void touch_point_delete(lv_wl_seat_touch_t * tdata, lv_wl_touch_point_t * touch_point);
#endif

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
    lv_indev_set_read_cb(indev, touch_read);
    lv_indev_set_driver_data(indev, lv_wl_ctx.seat.touch);

    return indev;
}

lv_indev_t * lv_wayland_get_touchscreen(lv_display_t * display)
{
    LV_CHECK_ARG(display != NULL, return NULL);
    lv_wl_window_t * window = lv_display_get_driver_data(display);
    LV_CHECK_ARG_MSG(window != NULL, return NULL, "Invalid display");
    return window->lv_indev_touch;
}

/**********************
 *   PRIVATE FUNCTIONS
 **********************/

lv_wl_seat_touch_t * lv_wayland_seat_touch_create(struct wl_seat * seat)
{
    LV_ASSERT(seat != NULL);
    struct wl_touch * touch = wl_seat_get_touch(seat);
    if(!touch) {
        LV_LOG_WARN("Failed to get seat touch");
        return NULL;
    }
    lv_wl_seat_touch_t * wl_seat_touch = lv_zalloc(sizeof(*wl_seat_touch));
    LV_ASSERT_MALLOC(wl_seat_touch);
    if(!wl_seat_touch) {
        LV_LOG_WARN("Failed to allocate memory for wayland touch");
        wl_touch_destroy(touch);
        return NULL;
    }
    wl_touch_add_listener(touch, &touch_listener, NULL);
    wl_touch_set_user_data(touch, wl_seat_touch);

#if !LV_USE_GESTURE_RECOGNITION
    lv_ll_init(&wl_seat_touch->touch_point_ll, sizeof(lv_wl_touch_point_t));
#endif

    wl_seat_touch->wl_touch = touch;
    lv_wayland_update_indevs(touch_read, wl_seat_touch);

    return wl_seat_touch;
}

void lv_wayland_seat_touch_delete(lv_wl_seat_touch_t * seat_touch)
{
    lv_wayland_update_indevs(touch_read, NULL);
    wl_touch_destroy(seat_touch->wl_touch);
#if !LV_USE_GESTURE_RECOGNITION
    lv_ll_clear(&seat_touch->touch_point_ll);
#endif
    lv_free(seat_touch);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void touch_read(lv_indev_t * indev, lv_indev_data_t * data)
{

    LV_ASSERT(indev != NULL);
    LV_ASSERT(data != NULL);
    lv_wl_seat_touch_t * tdata = lv_indev_get_driver_data(indev);

    if(!tdata) {
        return;
    }
#if LV_USE_GESTURE_RECOGNITION
    /* Collect touches if there are any - send them to the gesture recognizer */
    lv_indev_gesture_recognizers_update(indev, tdata->touches, tdata->event_cnt);

    LV_LOG_TRACE("collected touch events: %d", tdata->event_cnt);

    if(tdata->event_cnt > 0) {
        data->point = tdata->touches[0].point;
    }
    else {
        data->point.x = data->point.y = 0;
    }

    tdata->event_cnt = 0;

    /* Set the gesture information, before returning to LVGL */
    lv_indev_gesture_recognizers_set_data(indev, data);

#else
    data->state = LV_INDEV_STATE_RELEASED;
    lv_display_t * disp = lv_indev_get_display(indev);
    if(!disp) {
        return;
    }
    lv_wl_window_t * window = lv_display_get_driver_data(disp);
    LV_ASSERT(window != NULL);

    lv_wl_touch_point_t * touch_point;
    LV_LL_READ(&tdata->touch_point_ll, touch_point) {
        /* does this touch belong to the this indev's display */
        if(touch_point->surface != window->body) {
            continue;
        }

        data->point = touch_point->point;
        data->state = touch_point->state;

        if(touch_point->state == LV_INDEV_STATE_RELEASED) {
            touch_point_delete(tdata, touch_point);
        }
        return;
    }
#endif
}

static void touch_handle_down(void * data, struct wl_touch * wl_touch, uint32_t serial, uint32_t time,
                              struct wl_surface * surface, int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
    LV_UNUSED(data);
    LV_UNUSED(id);
    LV_UNUSED(time);
    LV_UNUSED(serial);

    lv_wl_seat_touch_t * tdata = wl_touch_get_user_data(wl_touch);

    if(!surface) {
        return;
    }

#if LV_USE_GESTURE_RECOGNITION
    uint8_t i = tdata->event_cnt;

    tdata->touches[i].point.x   = wl_fixed_to_int(x_w);
    tdata->touches[i].point.y   = wl_fixed_to_int(y_w);
    tdata->touches[i].id        = id;
    tdata->touches[i].timestamp = time;
    tdata->touches[i].state     = LV_INDEV_STATE_PRESSED;
    tdata->event_cnt++;
#else
    lv_wl_touch_point_t * touch_point = lv_ll_ins_tail(&tdata->touch_point_ll);
    LV_ASSERT_MALLOC(touch_point);
    if(!touch_point) {
        LV_LOG_ERROR("Failed to allocate memory for a touch point");
        return;
    }

    touch_point->id = id;
    touch_point->surface = surface;
    touch_point->point.x = wl_fixed_to_int(x_w);
    touch_point->point.y = wl_fixed_to_int(y_w);
    touch_point->state = LV_INDEV_STATE_PRESSED;
#endif
}

static void touch_handle_up(void * data, struct wl_touch * wl_touch, uint32_t serial, uint32_t time, int32_t id)
{
    LV_UNUSED(serial);
    LV_UNUSED(time);
    LV_UNUSED(id);
    LV_UNUSED(data);
    lv_wl_seat_touch_t * tdata = wl_touch_get_user_data(wl_touch);

    /* Create a released event */
#if LV_USE_GESTURE_RECOGNITION
    uint8_t i = tdata->event_cnt;

    tdata->touches[i].point.x   = 0;
    tdata->touches[i].point.y   = 0;
    tdata->touches[i].id        = id;
    tdata->touches[i].timestamp = time;
    tdata->touches[i].state     = LV_INDEV_STATE_RELEASED;

    tdata->event_cnt++;
#else
    lv_wl_touch_point_t * touch_point = get_pressed_touch_point(tdata, id);
    if(touch_point) {
        /* deleted after LVGL reads it in touch_read */
        touch_point->state = LV_INDEV_STATE_RELEASED;
    }
#endif
}

static void touch_handle_motion(void * data, struct wl_touch * wl_touch, uint32_t time, int32_t id, wl_fixed_t x_w,
                                wl_fixed_t y_w)
{

    LV_UNUSED(id);
    LV_UNUSED(time);
    LV_UNUSED(data);
    lv_wl_seat_touch_t * tdata = wl_touch_get_user_data(wl_touch);

#if LV_USE_GESTURE_RECOGNITION
    /* Update the contact point of the corresponding id with the latest coordinate */
    lv_indev_touch_data_t * touch = &tdata->touches[0];
    lv_indev_touch_data_t * cur = NULL;

    for(uint8_t i = 0; i < tdata->event_cnt; i++) {
        if(touch->id == id) {
            cur = touch;
        }
        touch++;
    }

    if(cur == NULL) {
        uint8_t i = tdata->event_cnt;
        tdata->touches[i].point.x   = wl_fixed_to_int(x_w);
        tdata->touches[i].point.y   = wl_fixed_to_int(y_w);
        tdata->touches[i].id        = id;
        tdata->touches[i].timestamp = time;
        tdata->touches[i].state     = LV_INDEV_STATE_PRESSED;
        tdata->event_cnt++;
    }
    else {
        cur->point.x   = wl_fixed_to_int(x_w);
        cur->point.y   = wl_fixed_to_int(y_w);
        cur->id        = id;
        cur->timestamp = time;
    }
#else
    lv_wl_touch_point_t * touch_point = get_pressed_touch_point(tdata, id);
    if(touch_point) {
        touch_point->point.x = wl_fixed_to_int(x_w);
        touch_point->point.y = wl_fixed_to_int(y_w);
    }
#endif
}

static void touch_handle_frame(void * data, struct wl_touch * wl_touch)
{
    LV_UNUSED(wl_touch);
    LV_UNUSED(data);
    lv_wayland_indevs_ready(touch_read);
}

static void touch_handle_cancel(void * data, struct wl_touch * wl_touch)
{
    LV_UNUSED(data);

#if !LV_USE_GESTURE_RECOGNITION
    lv_wl_seat_touch_t * tdata = wl_touch_get_user_data(wl_touch);
    lv_wl_touch_point_t * touch_point;
    LV_LL_READ(&tdata->touch_point_ll, touch_point) {
        touch_point->state = LV_INDEV_STATE_RELEASED;
    }
    lv_wayland_indevs_ready(touch_read);
#else
    LV_UNUSED(wl_touch);
#endif
}

#if !LV_USE_GESTURE_RECOGNITION

static lv_wl_touch_point_t * get_pressed_touch_point(lv_wl_seat_touch_t * tdata, int32_t id)
{
    LV_ASSERT(tdata != NULL);
    lv_wl_touch_point_t * touch_point;
    LV_LL_READ(&tdata->touch_point_ll, touch_point) {
        if(touch_point->id == id && touch_point->state == LV_INDEV_STATE_PRESSED) {
            return touch_point;
        }
    }
    return NULL;
}

static void touch_point_delete(lv_wl_seat_touch_t * tdata, lv_wl_touch_point_t * touch_point)
{
    LV_ASSERT(tdata != NULL);
    lv_ll_remove(&tdata->touch_point_ll, touch_point);
    lv_free(touch_point);
}

#endif /*!LV_USE_GESTURE_RECOGNITION*/

#endif /* LV_USE_WAYLAND */
