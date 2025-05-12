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

typedef struct {
    uint32_t num_gesture;
    float scale;
} click_counts_t;

static void gesture_event_cb(lv_event_t * e)
{
    click_counts_t * counts = lv_event_get_user_data(e);
    counts->scale = lv_event_get_pinch_scale(e);
    ++counts->num_gesture;
}

void test_gesture_pinch(void)
{
    /*Setup button that counts events.*/

    click_counts_t counts;
    lv_memzero(&counts, sizeof(counts));

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(label, 320, 320);
    lv_obj_add_event_cb(label, gesture_event_cb, LV_EVENT_GESTURE, &counts);

    /*Simple multi-touch.*/

    lv_point_t point_begin_0 = {120, 200};
    lv_point_t point_begin_1 = {200, 120};
    lv_point_t point_end_0 = {80, 250};
    lv_point_t point_end_1 = {250, 80};
    lv_test_gesture_pinch(point_begin_0, point_begin_1, point_end_0, point_end_1);
    TEST_ASSERT_GREATER_THAN(0, counts.num_gesture);
}

#endif
