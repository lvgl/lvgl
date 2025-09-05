/**
 * @file lv_test_indev_gesture.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test.h"
#if LV_USE_TEST && LV_USE_GESTURE_RECOGNITION

#include "../../core/lv_global.h"
#include "../../lvgl_private.h"

/*********************
 *      DEFINES
 *********************/

#define MAX_TOUCH_CNT 2

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_test_gesture_read_cb(lv_indev_t * indev, lv_indev_data_t * data);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#define _state LV_GLOBAL_DEFAULT()->test_state

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_test_indev_gesture_create(void)
{
    _state.max_touch_cnt = MAX_TOUCH_CNT;
    _state.touch_data =
        lv_malloc_zeroed(sizeof(lv_indev_touch_data_t) * _state.max_touch_cnt);
    if(_state.touch_data == NULL) {
        LV_LOG_ERROR("lv_indev_touch_data_t malloc failed");
    }

    _state.gesture_indev = lv_indev_create();
    lv_indev_set_type(_state.gesture_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(_state.gesture_indev,  lv_test_gesture_read_cb);
}

void lv_test_indev_gesture_delete(void)
{
    if(_state.gesture_indev) {
        lv_indev_delete(_state.gesture_indev);
        _state.gesture_indev = NULL;
    }

    if(_state.touch_data) {
        lv_free(_state.touch_data);
        _state.touch_data = NULL;
    }

    _state.max_touch_cnt = 0;
}

lv_indev_t * lv_test_indev_get_gesture_indev(lv_indev_type_t type)
{
    switch(type) {
        case LV_INDEV_TYPE_POINTER:
            return _state.gesture_indev;
        default:
            return NULL;
    }
}

void lv_test_gesture_set_pinch_data(lv_point_t point_0, lv_point_t point_1)
{
    _state.touch_data[0].id = 0;
    _state.touch_data[0].point = point_0;
    _state.touch_data[1].id = 1;
    _state.touch_data[1].point = point_1;
}

void lv_test_gesture_pinch_press(void)
{
    _state.touch_data[0].state = LV_INDEV_STATE_PRESSED;
    _state.touch_data[1].state = LV_INDEV_STATE_PRESSED;
}

void lv_test_gesture_pinch_release(void)
{
    _state.touch_data[0].state = LV_INDEV_STATE_RELEASED;
    _state.touch_data[1].state = LV_INDEV_STATE_RELEASED;
}

void lv_test_gesture_pinch(lv_point_t point_begin_0, lv_point_t point_begin_1,
                           lv_point_t point_end_0, lv_point_t point_end_1)
{
    lv_test_gesture_pinch_release();
    lv_test_wait(50);
    lv_test_gesture_set_pinch_data(point_begin_0, point_begin_1);
    lv_test_gesture_pinch_press();
    lv_test_wait(50);
    lv_test_gesture_set_pinch_data(point_end_0, point_end_1);
    lv_test_wait(80);
    lv_test_gesture_set_pinch_data(point_end_0, point_end_1);
    lv_test_wait(80);
    lv_test_gesture_pinch_release();
    lv_test_wait(50);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_test_gesture_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    LV_UNUSED(indev);

    lv_indev_gesture_recognizers_update(indev,
                                        _state.touch_data,
                                        _state.max_touch_cnt);
    lv_indev_gesture_recognizers_set_data(indev, data);
    if(_state.touch_data != NULL && _state.max_touch_cnt > 0) {
        data->point.x = _state.touch_data[0].point.x;
        data->point.y = _state.touch_data[0].point.y;
    }
    else {
        LV_LOG_ERROR("Invalid touch data or max touch count");
        data->point.x = 0;
        data->point.y = 0;
    }
}

#endif /*LV_USE_TEST*/
