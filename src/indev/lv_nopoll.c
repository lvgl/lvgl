#include "lvgl.h"
#include "lv_indev.h"
#include "lv_indev_private.h"
#include "lv_nopoll.h"

static lv_indev_t     *indev = NULL;
static void            indev_read(lv_indev_t *indev, lv_indev_data_t *data);
static lv_obj_t       *pointer_search_obj(int32_t x, int32_t y);
static lv_obj_t       *gesture_bubble_up(lv_obj_t *obj);
static lv_obj_t       *_obj   = NULL;
static lv_event_code_t _event = LV_EVENT_ALL;
static lv_dir_t        _dir   = LV_DIR_NONE;

void lv_nopoll_create()
{
    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_mode(indev, LV_INDEV_MODE_EVENT);
    lv_indev_set_read_cb(indev, indev_read);
}

void lv_nopoll_click(int32_t x, int32_t y)
{
    _obj   = pointer_search_obj(x, y);
    _event = LV_EVENT_CLICKED;
    _dir   = LV_DIR_NONE;
    if (indev == NULL) return;
    lv_indev_read(indev);
}

void lv_nopoll_long_press(int32_t x, int32_t y)
{
    _obj   = pointer_search_obj(x, y);
    _event = LV_EVENT_LONG_PRESSED;
    _dir   = LV_DIR_NONE;
    if (indev == NULL) return;
    lv_indev_read(indev);
}

static void lv_nopoll_swipe(int32_t x, int32_t y, lv_dir_t dir)
{
    _obj   = gesture_bubble_up(pointer_search_obj(x, y));
    _event = LV_EVENT_GESTURE;
    _dir   = dir;
    if (indev == NULL) return;
    lv_indev_read(indev);
}

void lv_nopoll_swipe_up(int32_t x, int32_t y)
{
    lv_nopoll_swipe(x, y, LV_DIR_TOP);
}

void lv_nopoll_swipe_down(int32_t x, int32_t y)
{
    lv_nopoll_swipe(x, y, LV_DIR_BOTTOM);
}

void lv_nopoll_swipe_left(int32_t x, int32_t y)
{
    lv_nopoll_swipe(x, y, LV_DIR_LEFT);
}

void lv_nopoll_swipe_right(int32_t x, int32_t y)
{
    lv_nopoll_swipe(x, y, LV_DIR_RIGHT);
}

/* return object at coordinates (x, y) */
static lv_obj_t *pointer_search_obj(int32_t x, int32_t y)
{
    lv_obj_t  *obj = NULL;
    lv_point_t p;

    p.x = x;
    p.y = y;

    /* code from lv_indev.c */
    obj = lv_indev_search_obj(lv_layer_sys(), &p);
    if (obj) return obj;

    obj = lv_indev_search_obj(lv_layer_top(), &p);
    if (obj) return obj;

    /* Search the object in the active screen */
    obj = lv_indev_search_obj(lv_screen_active(), &p);
    if (obj) return obj;

    obj = lv_indev_search_obj(lv_layer_bottom(), &p);
    return obj;
}

/* gesture bubble up */
static lv_obj_t *gesture_bubble_up(lv_obj_t *obj)
{
    lv_obj_t *gesture_obj = obj;

    /* if object has gesture bubble flag set, send event to parent */
    while (gesture_obj && lv_obj_has_flag(gesture_obj, LV_OBJ_FLAG_GESTURE_BUBBLE))
        gesture_obj = lv_obj_get_parent(gesture_obj);

    /* if no object found, send gesture to screen */
    if (gesture_obj == NULL) gesture_obj = lv_screen_active();

    return gesture_obj;
}

/* called by lvgl to read input device */
static void indev_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    if (indev == NULL) return;
    indev->pointer.gesture_dir = _dir;
    lv_obj_send_event(_obj, _event, indev);
    data->state = LV_INDEV_STATE_RELEASED;
}
