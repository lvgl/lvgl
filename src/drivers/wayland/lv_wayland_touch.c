/**
 * @file lv_wayland_touch.c
 *
 */

#include "lv_wayland_touch.h"

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

static void touch_read(lv_indev_t * drv, lv_indev_data_t * data);

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
static lv_ll_t g_touch_list;
bool g_touch_initialized = false;

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
	if (!g_touch_initialized)
	{
		lv_ll_init(&g_touch_list, sizeof(lv_wl_touch_node_t));
		g_touch_initialized = true;
	}
	return indev;
}

lv_indev_t * lv_wayland_get_touchscreen(lv_display_t * display)
{
    lv_wl_window_t * window = lv_display_get_driver_data(display);
    if(!window) {
        return NULL;
    }
    return window->lv_indev_touch;
}

/**********************
 *   PRIVATE FUNCTIONS
 **********************/

lv_wl_seat_touch_t * lv_wayland_seat_touch_create(struct wl_seat * seat)
{

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

    wl_seat_touch->wl_touch = touch;
    lv_wayland_update_indevs(touch_read, wl_seat_touch);

    return wl_seat_touch;
}

void lv_wayland_seat_touch_delete(lv_wl_seat_touch_t * seat_touch)
{
    lv_wayland_update_indevs(touch_read, NULL);
    wl_touch_destroy(seat_touch->wl_touch);
    lv_free(seat_touch);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void touch_read(lv_indev_t * indev, lv_indev_data_t * data)
{

    lv_wl_seat_touch_t * tdata = lv_indev_get_driver_data(indev);

    if(!tdata) {
        return;
    }
	
#if LV_USE_GESTURE_RECOGNITION
	/* Collect touches if there are any - send them to the gesture recognizer */
	lv_indev_gesture_recognizers_update(indev, tdata->touches, tdata->event_cnt);

	LV_LOG_TRACE("collected touch events: %d", tdata->event_cnt);

	if (tdata->event_cnt > 0)
	{
		data->point = tdata->touches[0].point;
	}
	else
	{
		data->point.x = data->point.y = 0;
	}

	tdata->event_cnt = 0;

	/* Set the gesture information, before returning to LVGL */
	lv_indev_gesture_recognizers_set_data(indev, data);

#else
	
	lv_display_t *disp = lv_indev_get_display(indev);
	lv_wl_window_t *window = lv_display_get_driver_data(disp);
	if (!window)
	{
		data->state = LV_INDEV_STATE_RELEASED;
		return;
	}

	bool found_match = false;
	lv_wl_touch_node_t *node;
	

	LV_LL_READ(&g_touch_list, node) {
		if (node->surface == window->body) {
			found_match = true;

			// COPY DATA FROM NODE
			data->point.x = node->last_point.x;
			data->point.y = node->last_point.y;
			data->state = node->state; // Uses the stored PRESSED state
			break;
		}
	}
	
	if (found_match)
	{
		int32_t raw_x = node->last_point.x;
		int32_t raw_y = node->last_point.y;

		// Get current display properties
		lv_display_rotation_t rotation = lv_display_get_rotation(disp);
		int32_t hor_res = lv_display_get_horizontal_resolution(disp);
		int32_t ver_res = lv_display_get_vertical_resolution(disp);

		int32_t transformed_x = raw_x;
		int32_t transformed_y = raw_y;

		// Apply Transformation based on Rotation
		data->point.x = transformed_x;
		data->point.y = transformed_y;
		data->state = node->state;
	}
	
	if (!found_match)
	{
		// No active touch found for this window
		data->state = LV_INDEV_STATE_RELEASED;
		data->point.x = 0;
		data->point.y = 0;
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

    if(!surface) {
        return;
    }
	
#if LV_USE_GESTURE_RECOGNITION
	lv_wl_seat_touch_t *tdata = wl_touch_get_user_data(wl_touch);
	
	uint8_t i = tdata->event_cnt;

	tdata->touches[i].point.x = wl_fixed_to_int(x_w);
	tdata->touches[i].point.y = wl_fixed_to_int(y_w);
	tdata->touches[i].id = id;
	tdata->touches[i].timestamp = time;
	tdata->touches[i].state = LV_INDEV_STATE_PRESSED;
	tdata->event_cnt++;
#else
	lv_wl_touch_node_t *new_node = lv_ll_ins_tail(&g_touch_list);
	if (!new_node) {
		LV_LOG_ERROR("Failed to allocate touch node");
		return;
	}
	new_node->id = id;
	new_node->surface = surface;
	new_node->last_point.x = wl_fixed_to_int(x_w);
	new_node->last_point.y = wl_fixed_to_int(y_w);
	new_node->state = LV_INDEV_STATE_PRESSED;
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
    tdata->state = LV_INDEV_STATE_RELEASED;

	lv_wl_touch_node_t *node;

	/* Iterate through the list to find the matching ID */
	LV_LL_READ(&g_touch_list, node)
	{
		if (node->id == id)
		{
			lv_ll_remove(&g_touch_list, node);
			lv_free(node);
			return;
		}
	}
#endif
}

static void touch_handle_motion(void * data, struct wl_touch * wl_touch, uint32_t time, int32_t id, wl_fixed_t x_w,
                                wl_fixed_t y_w)
{

    LV_UNUSED(id);
    LV_UNUSED(time);
    LV_UNUSED(data);

#if LV_USE_GESTURE_RECOGNITION
	lv_wl_seat_touch_t *tdata = wl_touch_get_user_data(wl_touch);
	
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
	lv_wl_touch_node_t *node;

	LV_LL_READ(&g_touch_list, node)
	{
		if (node->id == id)
		{
			node->last_point.x = wl_fixed_to_int(x_w);
			node->last_point.y = wl_fixed_to_int(y_w);
			node->state = LV_INDEV_STATE_PRESSED;
			return;
		}
	}
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
