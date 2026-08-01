#if LV_BUILD_TEST
#include "../lvgl.h"
#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

/*
 * Track the center point across multiple gesture events.
 * center_valid and center are updated only when the function returns true
 * so that the ENDED event (which returns false) does not clobber a valid reading.
 */
typedef struct {
    bool center_valid;
    lv_point_t center;
    uint32_t num_gesture;
} gesture_center_data_t;

static void gesture_pinch_center_cb(lv_event_t * e)
{
    gesture_center_data_t * data = lv_event_get_user_data(e);
    lv_point_t point;
    bool valid = lv_event_get_gesture_center_point(e, LV_INDEV_GESTURE_PINCH, &point);

    if(valid) {
        data->center_valid = true;
        data->center = point;
    }
    ++data->num_gesture;
}

static void gesture_wrong_type_cb(lv_event_t * e)
{
    gesture_center_data_t * data = lv_event_get_user_data(e);
    lv_point_t point;

    /* Query ROTATE center during a PINCH gesture - must always return false */
    if(lv_event_get_gesture_center_point(e, LV_INDEV_GESTURE_ROTATE, &point)) {
        data->center_valid = true;
    }
    ++data->num_gesture;
}

/*
 * Capture the center point result exclusively during the ENDED state so that
 * we can assert what the function returns once all fingers are lifted.
 */
typedef struct {
    bool center_valid_on_ended;
    lv_point_t center_on_ended;
    uint32_t num_ended;
} gesture_ended_data_t;

static void gesture_ended_cb(lv_event_t * e)
{
    gesture_ended_data_t * data = lv_event_get_user_data(e);
    lv_indev_gesture_state_t state = lv_event_get_gesture_state(e, LV_INDEV_GESTURE_PINCH);

    if(state == LV_INDEV_GESTURE_STATE_ENDED) {
        lv_point_t point = {-1, -1};
        data->center_valid_on_ended = lv_event_get_gesture_center_point(e, LV_INDEV_GESTURE_PINCH, &point);
        data->center_on_ended = point;
        ++data->num_ended;
    }
}

/* Center point is the midpoint of the two initial touch positions */
void test_gesture_center_point_pinch_returns_midpoint(void)
{
    gesture_center_data_t data;
    lv_memzero(&data, sizeof(data));

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(label, 320, 320);
    lv_obj_add_event_cb(label, gesture_pinch_center_cb, LV_EVENT_GESTURE, &data);

    /* Begin midpoint: ((120+200)/2, (200+120)/2) = (160, 160) */
    lv_point_t point_begin_0 = {120, 200};
    lv_point_t point_begin_1 = {200, 120};
    lv_point_t point_end_0   = {80, 250};
    lv_point_t point_end_1   = {250, 80};
    lv_test_gesture_pinch(point_begin_0, point_begin_1, point_end_0, point_end_1);

    TEST_ASSERT_GREATER_THAN_UINT32(0, data.num_gesture);
    TEST_ASSERT_TRUE(data.center_valid);
    TEST_ASSERT_EQUAL_INT32((point_begin_0.x + point_begin_1.x) / 2, data.center.x);
    TEST_ASSERT_EQUAL_INT32((point_begin_0.y + point_begin_1.y) / 2, data.center.y);
}

/* Querying the center point of a gesture type not currently active returns false */
void test_gesture_center_point_wrong_type_returns_false(void)
{
    gesture_center_data_t data;
    lv_memzero(&data, sizeof(data));

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(label, 320, 320);
    lv_obj_add_event_cb(label, gesture_wrong_type_cb, LV_EVENT_GESTURE, &data);

    lv_point_t point_begin_0 = {120, 200};
    lv_point_t point_begin_1 = {200, 120};
    lv_point_t point_end_0   = {80, 250};
    lv_point_t point_end_1   = {250, 80};
    lv_test_gesture_pinch(point_begin_0, point_begin_1, point_end_0, point_end_1);

    TEST_ASSERT_GREATER_THAN_UINT32(0, data.num_gesture);
    TEST_ASSERT_FALSE(data.center_valid);
}

/* Once a gesture has ended the center point is no longer available */
void test_gesture_center_point_ended_returns_false(void)
{
    gesture_ended_data_t data;
    lv_memzero(&data, sizeof(data));

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(label, 320, 320);
    lv_obj_add_event_cb(label, gesture_ended_cb, LV_EVENT_GESTURE, &data);

    lv_point_t point_begin_0 = {120, 200};
    lv_point_t point_begin_1 = {200, 120};
    lv_point_t point_end_0   = {80, 250};
    lv_point_t point_end_1   = {250, 80};
    lv_test_gesture_pinch(point_begin_0, point_begin_1, point_end_0, point_end_1);

    TEST_ASSERT_GREATER_THAN_UINT32(0, data.num_ended);
    TEST_ASSERT_FALSE(data.center_valid_on_ended);
    TEST_ASSERT_EQUAL_INT32(0, data.center_on_ended.x);
    TEST_ASSERT_EQUAL_INT32(0, data.center_on_ended.y);
}

#endif
