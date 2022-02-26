#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * slider = NULL;

void setUp(void)
{
    active_screen = lv_scr_act();
    slider = lv_slider_create(active_screen);
}

void tearDown(void)
{
}

void test_textarea_should_have_valid_documented_default_values(void)
{
    lv_coord_t objw = lv_obj_get_width(slider);
    lv_coord_t objh = lv_obj_get_height(slider);

    /* Horizontal slider */
    TEST_ASSERT_TRUE(objw >= objh);
    TEST_ASSERT_FALSE(lv_obj_has_flag(slider, LV_OBJ_FLAG_SCROLL_CHAIN));
    TEST_ASSERT_FALSE(lv_obj_has_flag(slider, LV_OBJ_FLAG_SCROLLABLE));
}

void test_slider_event_keys_right_and_up_increment_value_by_one(void)
{
    char key = LV_KEY_RIGHT;
    lv_slider_set_value(slider, 10, LV_ANIM_OFF);
    int32_t value = lv_slider_get_value(slider);

    lv_event_send(slider, LV_EVENT_KEY, (void *) &key);

    int32_t new_value = lv_slider_get_value(slider);
    TEST_ASSERT_EQUAL_INT32(value + 1, new_value);

    key = LV_KEY_UP;
    lv_event_send(slider, LV_EVENT_KEY, (void *) &key);
    TEST_ASSERT_EQUAL_INT32(new_value + 1, lv_slider_get_value(slider));
}

void test_slider_event_keys_left_and_down_decrement_value_by_one(void)
{
    char key = LV_KEY_LEFT;
    lv_slider_set_value(slider, 10, LV_ANIM_OFF);
    int32_t value = lv_slider_get_value(slider);

    lv_event_send(slider, LV_EVENT_KEY, (void *) &key);

    int32_t new_value = lv_slider_get_value(slider);
    TEST_ASSERT_EQUAL_INT32(value - 1, new_value);

    key = LV_KEY_DOWN;
    lv_event_send(slider, LV_EVENT_KEY, (void *) &key);
    TEST_ASSERT_EQUAL_INT32(new_value - 1, lv_slider_get_value(slider));
}

void test_slider_event_invalid_key_should_not_change_values(void)
{
    char key = LV_KEY_ENTER;
    lv_slider_set_value(slider, 10, LV_ANIM_OFF);
    int32_t value = lv_slider_get_value(slider);

    lv_event_send(slider, LV_EVENT_KEY, (void *) &key);

    TEST_ASSERT_EQUAL_INT32(value, lv_slider_get_value(slider));
}

#endif
