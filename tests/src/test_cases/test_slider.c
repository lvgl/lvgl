#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include "lv_test_indev.h"

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

void test_slider_range_mode_should_leave_edit_mode_if_released(void)
{
    lv_slider_t * ptr = (lv_slider_t *) slider;
    ptr->left_knob_focus = 1;
    lv_slider_set_mode(slider, LV_SLIDER_MODE_RANGE);

    /* Setup group and encoder indev */
    lv_group_t * g = lv_group_create();
    lv_indev_set_group(lv_test_encoder_indev, g);
    lv_group_add_obj(g, slider);
    lv_group_set_editing(g, true);

    lv_test_encoder_click();

    /* Always executed when handling LV_EVENT_RELEASED or
     * LV_EVENT_PRESS_LOST */
    TEST_ASSERT_FALSE(ptr->dragging);
    TEST_ASSERT_NULL(ptr->value_to_set);
    TEST_ASSERT_EQUAL(0U, ptr->left_knob_focus);

    /* Group leaved edit mode */
    TEST_ASSERT_FALSE(lv_group_get_editing(g));
}

void test_slider_range_mode_should_not_leave_edit_mode_if_released_with_no_left_knob_focus(void)
{
    lv_slider_t * ptr = (lv_slider_t *) slider;
    ptr->left_knob_focus = 0;
    lv_slider_set_mode(slider, LV_SLIDER_MODE_RANGE);

    /* Setup group and encoder indev */
    lv_group_t * g = lv_group_create();
    lv_indev_set_group(lv_test_encoder_indev, g);
    lv_group_add_obj(g, slider);
    lv_group_set_editing(g, true);

    lv_test_encoder_release();
    lv_test_indev_wait(50);

    /* Always executed when handling LV_EVENT_RELEASED or
     * LV_EVENT_PRESS_LOST */
    TEST_ASSERT_FALSE(ptr->dragging);
    TEST_ASSERT_NULL(ptr->value_to_set);

    /* Group leaved edit mode */

    /* NOTE: This assert should be checking against 1U instead of 0U
     * The reason why left_knob_focus is being set to 0U remains unknown
     * at the lv_slider event handler level. */
    TEST_ASSERT_EQUAL(0U, ptr->left_knob_focus);
    TEST_ASSERT(lv_group_get_editing(g));
}

void test_slider_normal_mode_should_leave_edit_mode_if_released(void)
{
    lv_slider_t * ptr = (lv_slider_t *) slider;
    ptr->left_knob_focus = 1;
    lv_slider_set_mode(slider, LV_SLIDER_MODE_NORMAL);

    /* Setup group and encoder indev */
    lv_group_t * g = lv_group_create();
    lv_indev_set_group(lv_test_encoder_indev, g);
    lv_group_add_obj(g, slider);
    lv_group_set_editing(g, true);

    lv_test_encoder_click();

    /* Always executed when handling LV_EVENT_RELEASED or
     * LV_EVENT_PRESS_LOST */
    TEST_ASSERT_FALSE(ptr->dragging);
    TEST_ASSERT_NULL(ptr->value_to_set);
    TEST_ASSERT_EQUAL(0U, ptr->left_knob_focus);

    /* Group leaved edit mode */
    TEST_ASSERT_FALSE(lv_group_get_editing(g));
}

void test_slider_event_hit_test(void)
{
    /* Validate if point 0,0 can click in the slider */
    lv_point_t point = {
        .x = 0,
        .y = 0
    };

    lv_hit_test_info_t info = {
        .res = false,
        .point = &point
    };
    lv_event_send(slider, LV_EVENT_HIT_TEST, (void *) &info);

    /* Point was hit */
    TEST_ASSERT(info.res);
}

#endif
