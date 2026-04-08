#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * slider = NULL;
static lv_obj_t * sliderRangeMode = NULL;
static lv_obj_t * sliderNormalMode = NULL;
static lv_obj_t * sliderSymmetricalMode = NULL;
static lv_group_t * g = NULL;

void setUp(void)
{
    active_screen = lv_screen_active();
    slider = lv_slider_create(active_screen);
    sliderRangeMode = lv_slider_create(active_screen);
    sliderNormalMode = lv_slider_create(active_screen);
    sliderSymmetricalMode = lv_slider_create(active_screen);

    lv_slider_set_mode(sliderRangeMode, LV_SLIDER_MODE_RANGE);
    lv_slider_set_mode(sliderNormalMode, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_mode(sliderSymmetricalMode, LV_SLIDER_MODE_SYMMETRICAL);

    g = lv_group_create();
    lv_indev_set_group(lv_test_indev_get_indev(LV_INDEV_TYPE_ENCODER), g);
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_textarea_should_have_valid_documented_default_values(void)
{
    int32_t objw = lv_obj_get_width(slider);
    int32_t objh = lv_obj_get_height(slider);

    /* Horizontal slider */
    TEST_ASSERT_TRUE(objw >= objh);
    TEST_ASSERT_FALSE(lv_obj_has_flag(slider, LV_OBJ_FLAG_SCROLL_CHAIN));
    TEST_ASSERT_FALSE(lv_obj_has_flag(slider, LV_OBJ_FLAG_SCROLLABLE));
}

void test_slider_event_keys_right_and_up_increment_value_by_one(void)
{
    uint32_t key = LV_KEY_RIGHT;
    lv_slider_set_value(slider, 10, LV_ANIM_OFF);
    int32_t value = lv_slider_get_value(slider);

    lv_obj_send_event(slider, LV_EVENT_KEY, (void *) &key);

    int32_t new_value = lv_slider_get_value(slider);
    TEST_ASSERT_EQUAL_INT32(value + 1, new_value);

    key = LV_KEY_UP;
    lv_obj_send_event(slider, LV_EVENT_KEY, (void *) &key);
    TEST_ASSERT_EQUAL_INT32(new_value + 1, lv_slider_get_value(slider));
}

void test_slider_event_keys_left_and_down_decrement_value_by_one(void)
{
    uint32_t key = LV_KEY_LEFT;
    lv_slider_set_value(slider, 10, LV_ANIM_OFF);
    int32_t value = lv_slider_get_value(slider);

    lv_obj_send_event(slider, LV_EVENT_KEY, (void *) &key);

    int32_t new_value = lv_slider_get_value(slider);
    TEST_ASSERT_EQUAL_INT32(value - 1, new_value);

    key = LV_KEY_DOWN;
    lv_obj_send_event(slider, LV_EVENT_KEY, (void *) &key);
    TEST_ASSERT_EQUAL_INT32(new_value - 1, lv_slider_get_value(slider));
}

void test_slider_event_invalid_key_should_not_change_values(void)
{
    uint32_t key = LV_KEY_ENTER;
    lv_slider_set_value(slider, 10, LV_ANIM_OFF);
    int32_t value = lv_slider_get_value(slider);

    lv_obj_send_event(slider, LV_EVENT_KEY, (void *) &key);

    TEST_ASSERT_EQUAL_INT32(value, lv_slider_get_value(slider));
}

void test_slider_range_mode_should_leave_edit_mode_if_released(void)
{
    lv_slider_t * ptr = (lv_slider_t *) sliderRangeMode;

    /* Setup group and encoder indev */
    lv_group_add_obj(g, sliderNormalMode);
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
    lv_slider_t * ptr = (lv_slider_t *) sliderRangeMode;

    /* Setup group and encoder indev */
    lv_group_add_obj(g, sliderRangeMode);
    lv_group_set_editing(g, true);

    lv_test_encoder_release();
    lv_test_wait(50);

    /* Always executed when handling LV_EVENT_RELEASED or
     * LV_EVENT_PRESS_LOST */
    TEST_ASSERT_FALSE(ptr->dragging);
    TEST_ASSERT_NULL(ptr->value_to_set);

    TEST_ASSERT(lv_group_get_editing(g));
}

void test_slider_normal_mode_should_leave_edit_mode_if_released(void)
{
    lv_slider_t * ptr = (lv_slider_t *) sliderNormalMode;
    ptr->left_knob_focus = 1;

    /* Setup group and encoder indev */
    lv_group_add_obj(g, sliderNormalMode);
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

void test_ranged_mode_adjust_with_encoder(void)
{
    lv_slider_set_value(sliderRangeMode, 90, LV_ANIM_OFF);
    lv_slider_set_start_value(sliderRangeMode, 10, LV_ANIM_OFF);

    /* Setup group and encoder indev */
    lv_group_add_obj(g, sliderRangeMode);
    lv_group_set_editing(g, false);

    /*Go the edit mode*/
    lv_test_encoder_click();

    /*Adjust the right knob*/
    lv_test_encoder_turn(-10);
    TEST_ASSERT_EQUAL(80, lv_slider_get_value(sliderRangeMode));  /*Updated?*/
    TEST_ASSERT_EQUAL(10, lv_slider_get_left_value(sliderRangeMode));     /*Maintained?*/

    /*Focus the left knob*/
    lv_test_encoder_click();

    /*Adjust the left knob*/
    lv_test_encoder_turn(5);
    TEST_ASSERT_EQUAL(80, lv_slider_get_value(sliderRangeMode));  /*Maintained?*/
    TEST_ASSERT_EQUAL(15, lv_slider_get_left_value(sliderRangeMode));  /*Updated?*/

}

void test_normal_mode_slider_hit_test(void)
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

    lv_slider_set_value(sliderNormalMode, 100, LV_ANIM_OFF);
    lv_obj_send_event(sliderNormalMode, LV_EVENT_HIT_TEST, (void *) &info);

    /* point can click slider */
    TEST_ASSERT(info.res);
}

void test_slider_range_event_hit_test(void)
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
    lv_obj_send_event(sliderRangeMode, LV_EVENT_HIT_TEST, (void *) &info);

    /* point can click slider in the left knob */
    TEST_ASSERT(info.res);
}

void test_slider_properties(void)
{
#if LV_USE_OBJ_PROPERTY
    lv_obj_t * obj = lv_slider_create(lv_screen_active());
    lv_property_t prop = { };

    prop.id = LV_PROPERTY_SLIDER_RANGE;
    prop.arg1.num = 10;
    prop.arg2.num = 100;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(lv_slider_get_min_value(obj), 10);
    TEST_ASSERT_EQUAL_INT(lv_slider_get_max_value(obj), 100);
    TEST_ASSERT_EQUAL_INT(10, lv_obj_get_property(obj, LV_PROPERTY_SLIDER_MIN_VALUE).num);
    TEST_ASSERT_EQUAL_INT(100, lv_obj_get_property(obj, LV_PROPERTY_SLIDER_MAX_VALUE).num);

    prop.id = LV_PROPERTY_SLIDER_VALUE;
    prop.arg1.num = 50;
    prop.arg2.enable = false;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(lv_slider_get_value(obj), 50);
    TEST_ASSERT_EQUAL_INT(50, lv_obj_get_property(obj, LV_PROPERTY_SLIDER_VALUE).num);

    prop.id = LV_PROPERTY_SLIDER_MODE;
    prop.num = LV_SLIDER_MODE_RANGE;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(LV_SLIDER_MODE_RANGE, lv_obj_get_property(obj, LV_PROPERTY_SLIDER_MODE).num);

    prop.id = LV_PROPERTY_SLIDER_LEFT_VALUE;
    prop.arg1.num = 40;
    prop.arg2.enable = false;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(40, lv_slider_get_left_value(obj));
    TEST_ASSERT_EQUAL_INT(40, lv_obj_get_property(obj, LV_PROPERTY_SLIDER_LEFT_VALUE).num);

    prop.id = LV_PROPERTY_SLIDER_IS_DRAGGED;
    TEST_ASSERT_FALSE(lv_slider_is_dragged(obj));
    TEST_ASSERT_FALSE(lv_obj_get_property(obj, LV_PROPERTY_SLIDER_IS_DRAGGED).enable);

    prop.id = LV_PROPERTY_SLIDER_IS_SYMMETRICAL;
    TEST_ASSERT_FALSE(lv_slider_is_dragged(obj));
    TEST_ASSERT_FALSE(lv_obj_get_property(obj, LV_PROPERTY_SLIDER_IS_SYMMETRICAL).enable);

#endif
}

void test_slider_return_true_when_dragged(void)
{
    lv_obj_set_size(slider, 200, 20);
    lv_obj_center(slider);
    lv_obj_update_layout(slider);

    TEST_ASSERT_FALSE(lv_slider_is_dragged(slider));

    lv_area_t coords;
    lv_obj_get_coords(slider, &coords);
    lv_test_mouse_move_to(coords.x1 + 10, coords.y1 + 10);
    lv_test_mouse_press();
    lv_test_wait(50);

    lv_test_mouse_move_by(10, 0);
    lv_test_wait(50);

    TEST_ASSERT_TRUE(lv_slider_is_dragged(slider));

    lv_test_mouse_release();
    lv_test_wait(50);

    TEST_ASSERT_FALSE(lv_slider_is_dragged(slider));
}

void test_slider_range_mode_hit_test_left_knob(void)
{
    lv_obj_set_size(sliderRangeMode, 200, 20);
    lv_obj_center(sliderRangeMode);

    /* Set values to ensure separation between knobs */
    lv_slider_set_value(sliderRangeMode, 90, LV_ANIM_OFF);
    lv_slider_set_start_value(sliderRangeMode, 10, LV_ANIM_OFF);
    lv_obj_update_layout(sliderRangeMode);

    /* Force redraw to update knob areas */
    lv_obj_invalidate(sliderRangeMode);
    lv_refr_now(NULL);

    lv_slider_t * ptr = (lv_slider_t *) sliderRangeMode;

    /* Point on the left knob */
    lv_point_t point;
    point.x = (ptr->left_knob_area.x1 + ptr->left_knob_area.x2) / 2;
    point.y = (ptr->left_knob_area.y1 + ptr->left_knob_area.y2) / 2;

    lv_hit_test_info_t info = {
        .res = false,
        .point = &point
    };

    lv_obj_send_event(sliderRangeMode, LV_EVENT_HIT_TEST, &info);

    TEST_ASSERT_TRUE(info.res);
}

void test_slider_scroll_chain_horizontal(void)
{
    lv_obj_set_size(slider, 200, 20);
    lv_obj_update_layout(slider);

    /* Simulate POINTER release */
    lv_test_mouse_release();
    lv_obj_send_event(slider, LV_EVENT_RELEASED, NULL);

    /* Horizontal ptr should allow vertical scroll chain */
    TEST_ASSERT_TRUE(lv_obj_has_flag(slider, LV_OBJ_FLAG_SCROLL_CHAIN_VER));
}

void test_slider_scroll_chain_vertical(void)
{
    lv_obj_set_size(slider, 20, 200);
    lv_obj_update_layout(slider);

    /* Simulate POINTER release */
    lv_test_mouse_release();
    lv_obj_send_event(slider, LV_EVENT_RELEASED, NULL);

    /* Vertical ptr should allow horizontal scroll chain */
    TEST_ASSERT_TRUE(lv_obj_has_flag(slider, LV_OBJ_FLAG_SCROLL_CHAIN_HOR));
}

void test_slider_range_mode_key_decrement_left_value(void)
{
    /* Set range mode and initial values */
    lv_slider_set_mode(sliderRangeMode, LV_SLIDER_MODE_RANGE);
    lv_slider_set_value(sliderRangeMode, 80, LV_ANIM_OFF);
    lv_slider_set_start_value(sliderRangeMode, 20, LV_ANIM_OFF);

    /* Focus the left knob */
    lv_slider_t * ptr = (lv_slider_t *) sliderRangeMode;
    ptr->left_knob_focus = 1;

    /* Simulate LEFT key press */
    uint32_t key = LV_KEY_LEFT;
    lv_obj_send_event(sliderRangeMode, LV_EVENT_KEY, &key);

    /* Left value should decrement by 1 */
    TEST_ASSERT_EQUAL_INT32(19, lv_slider_get_left_value(sliderRangeMode));

    /* Simulate DOWN key press */
    key = LV_KEY_DOWN;
    lv_obj_send_event(sliderRangeMode, LV_EVENT_KEY, &key);

    /* Left value should decrement by 1 again */
    TEST_ASSERT_EQUAL_INT32(18, lv_slider_get_left_value(sliderRangeMode));
}

void test_slider_rotary_event_handling(void)
{
    /* Setup group and encoder indev */
    lv_group_add_obj(g, sliderNormalMode);
    lv_group_set_editing(g, true);

    /* Normal mode (right knob focus) */
    lv_slider_set_value(sliderNormalMode, 50, LV_ANIM_OFF);

    /* Simulate rotary right (+1) */
    lv_test_encoder_turn(1);
    TEST_ASSERT_EQUAL_INT32(51, lv_slider_get_value(sliderNormalMode));

    /* Simulate rotary left (-1) */
    lv_test_encoder_turn(-1);
    TEST_ASSERT_EQUAL_INT32(50, lv_slider_get_value(sliderNormalMode));

    /* Range mode (left knob focus) */
    lv_slider_set_mode(sliderRangeMode, LV_SLIDER_MODE_RANGE);
    lv_group_add_obj(g, sliderRangeMode);

    /* Manually focus left knob to hit the else branch */
    lv_slider_t * ptr = (lv_slider_t *) sliderRangeMode;
    ptr->left_knob_focus = 1;

    lv_slider_set_value(sliderRangeMode, 80, LV_ANIM_OFF);
    lv_slider_set_start_value(sliderRangeMode, 20, LV_ANIM_OFF);

    /* Simulate rotary event directly to ensure coverage of the specific branch */
    int32_t diff = 1;
    lv_obj_send_event(sliderRangeMode, LV_EVENT_ROTARY, &diff);
    TEST_ASSERT_EQUAL_INT32(21, lv_slider_get_left_value(sliderRangeMode));
}

static void assert_symmetrical_slider_knob_drawn(bool is_hor, int32_t value)
{
    lv_obj_clean(lv_screen_active());
    lv_obj_t * obj = lv_slider_create(lv_screen_active());
    lv_obj_center(obj);
    if(is_hor) lv_obj_set_size(obj, 200, 20);
    else lv_obj_set_size(obj, 20, 200);

    lv_slider_set_mode(obj, LV_SLIDER_MODE_SYMMETRICAL);
    lv_slider_set_range(obj, -100, 100);
    lv_slider_set_value(obj, value, LV_ANIM_OFF);
    lv_obj_update_layout(obj);

    char ref_screenshot_name[64];
    lv_snprintf(ref_screenshot_name, sizeof(ref_screenshot_name),
                "widgets/slider_sym_%s_value_%s_%d.png",
                is_hor ? "hor" : "ver",
                value >= 0 ? "positive" : "negative",
                LV_ABS(value));
    TEST_ASSERT_EQUAL_SCREENSHOT(ref_screenshot_name);

    lv_slider_t * ptr = (lv_slider_t *) obj;
    if(is_hor) {
        TEST_ASSERT_NOT_EQUAL_INT32(0, ptr->right_knob_area.x1);
        TEST_ASSERT_NOT_EQUAL_INT32(0, ptr->right_knob_area.x2);
    }
    else {
        TEST_ASSERT_NOT_EQUAL_INT32(0, ptr->right_knob_area.y1);
        TEST_ASSERT_NOT_EQUAL_INT32(0, ptr->right_knob_area.y2);
    }
}

void test_slider_symmetrical_mode_knob_drawing(void)
{
    /* Horizontal Case (Negative) */
    assert_symmetrical_slider_knob_drawn(true, -50);

    /* Vertical Case (Negative) */
    assert_symmetrical_slider_knob_drawn(false, -50);

    /* Vertical Case (Positive) */
    assert_symmetrical_slider_knob_drawn(false, 50);
}

void test_slider_range_mode_encoder_exit_edit_with_left_knob_focus(void)
{
    lv_slider_set_mode(sliderRangeMode, LV_SLIDER_MODE_RANGE);

    /* Setup group and encoder indev */
    lv_group_add_obj(g, sliderRangeMode);
    lv_group_set_editing(g, true);

    /* Manually focus left knob */
    lv_slider_t * ptr = (lv_slider_t *) sliderRangeMode;
    ptr->left_knob_focus = 1;

    /* Simulate encoder click (Release event) */
    lv_test_encoder_click();

    TEST_ASSERT_EQUAL_UINT8(0, ptr->left_knob_focus);
    TEST_ASSERT_FALSE(lv_group_get_editing(g));
}

static void assert_slider_drag_start_selection(lv_obj_t * obj, int32_t x, int32_t y,
                                               const int32_t * expected_value_ptr,
                                               int32_t expected_focus)
{
    lv_slider_t * ptr = (lv_slider_t *) obj;

    lv_test_mouse_move_to(x, y);
    lv_test_mouse_press();
    lv_test_wait(50);

    /* Move slightly to trigger drag (exceed scroll_limit) */
    lv_test_mouse_move_by(10, 10);
    lv_test_wait(50);

    TEST_ASSERT_TRUE(ptr->dragging);
    TEST_ASSERT_EQUAL_PTR(expected_value_ptr, ptr->value_to_set);
    if(expected_focus >= 0) {
        TEST_ASSERT_EQUAL_INT32(expected_focus, ptr->left_knob_focus);
    }

    lv_test_mouse_release();
    lv_test_wait(50);
}

void test_slider_range_mode_drag_start_value_selection(void)
{
    lv_obj_set_size(sliderRangeMode, 200, 20);
    lv_obj_center(sliderRangeMode);
    lv_slider_set_mode(sliderRangeMode, LV_SLIDER_MODE_RANGE);
    lv_slider_set_value(sliderRangeMode, 80, LV_ANIM_OFF);      /* Right knob at ~80% */
    lv_slider_set_start_value(sliderRangeMode, 20, LV_ANIM_OFF); /* Left knob at ~20% */
    lv_obj_update_layout(sliderRangeMode);

    /* Force redraw to update knob areas */
    lv_obj_invalidate(sliderRangeMode);
    lv_refr_now(NULL);

    lv_slider_t * ptr = (lv_slider_t *) sliderRangeMode;
    lv_area_t right_knob = ptr->right_knob_area;
    lv_area_t left_knob = ptr->left_knob_area;

    /* Click to the right of the right knob (Horizontal) */
    assert_slider_drag_start_selection(sliderRangeMode,
                                       right_knob.x2 + 10, (right_knob.y1 + right_knob.y2) / 2,
                                       &ptr->bar.cur_value, -1);

    /* Click to the left of the left knob (Horizontal) */
    assert_slider_drag_start_selection(sliderRangeMode,
                                       left_knob.x1 - 10, (left_knob.y1 + left_knob.y2) / 2,
                                       &ptr->bar.start_value, -1);

    /* Click between knobs, closer to right knob (Horizontal) */
    assert_slider_drag_start_selection(sliderRangeMode,
                                       right_knob.x1 - 5, (right_knob.y1 + right_knob.y2) / 2,
                                       &ptr->bar.cur_value, 0);

    /* Click between knobs, closer to left knob (Horizontal) */
    assert_slider_drag_start_selection(sliderRangeMode,
                                       left_knob.x2 + 5, (left_knob.y1 + left_knob.y2) / 2,
                                       &ptr->bar.start_value, 1);
}

void test_slider_vertical_range_mode_drag_start_value_selection(void)
{
    lv_obj_t * slider_ver = lv_slider_create(lv_screen_active());
    lv_obj_set_size(slider_ver, 20, 200);
    lv_obj_center(slider_ver);
    lv_slider_set_mode(slider_ver, LV_SLIDER_MODE_RANGE);
    lv_slider_set_value(slider_ver, 80, LV_ANIM_OFF);
    lv_slider_set_start_value(slider_ver, 20, LV_ANIM_OFF);
    lv_obj_update_layout(slider_ver);

    /* Force redraw to update knob areas */
    lv_obj_invalidate(slider_ver);
    lv_refr_now(NULL);

    lv_slider_t * ptr_ver = (lv_slider_t *) slider_ver;
    lv_area_t top_knob = ptr_ver->right_knob_area;
    lv_area_t bottom_knob = ptr_ver->left_knob_area;

    /* Click above the top knob (Vertical) */
    assert_slider_drag_start_selection(slider_ver,
                                       (top_knob.x1 + top_knob.x2) / 2, top_knob.y1 - 10,
                                       &ptr_ver->bar.cur_value, -1);

    /* Click below the bottom knob (Vertical) */
    assert_slider_drag_start_selection(slider_ver,
                                       (bottom_knob.x1 + bottom_knob.x2) / 2, bottom_knob.y2 + 10,
                                       &ptr_ver->bar.start_value, -1);

    /* Click between knobs, closer to top/right knob (Vertical) */
    assert_slider_drag_start_selection(slider_ver,
                                       (top_knob.x1 + top_knob.x2) / 2, top_knob.y2 + 5,
                                       &ptr_ver->bar.cur_value, 0);

    /* Click between knobs, closer to bottom/left knob (Vertical) */
    assert_slider_drag_start_selection(slider_ver,
                                       (bottom_knob.x1 + bottom_knob.x2) / 2, bottom_knob.y1 - 5,
                                       &ptr_ver->bar.start_value, 1);
}

void test_slider_range_mode_horizontal_rtl_drag_start_value_selection(void)
{
    lv_obj_set_size(sliderRangeMode, 200, 20);
    lv_obj_center(sliderRangeMode);
    lv_slider_set_mode(sliderRangeMode, LV_SLIDER_MODE_RANGE);
    lv_obj_set_style_base_dir(sliderRangeMode, LV_BASE_DIR_RTL, 0);

    lv_slider_set_value(sliderRangeMode, 80, LV_ANIM_OFF);
    lv_slider_set_start_value(sliderRangeMode, 20, LV_ANIM_OFF);
    lv_obj_update_layout(sliderRangeMode);

    /* Force redraw to update knob areas */
    lv_obj_invalidate(sliderRangeMode);
    lv_refr_now(NULL);

    lv_slider_t * ptr = (lv_slider_t *) sliderRangeMode;
    lv_area_t left_knob = ptr->left_knob_area;

    assert_slider_drag_start_selection(sliderRangeMode,
                                       left_knob.x2 + 10, (left_knob.y1 + left_knob.y2) / 2,
                                       &ptr->bar.start_value, -1);
}

void test_slider_range_mode_vertical_rtl_drag_start_value_selection(void)
{
    lv_obj_t * slider_ver = lv_slider_create(lv_screen_active());
    lv_obj_set_size(slider_ver, 20, 200);
    lv_obj_center(slider_ver);
    lv_slider_set_mode(slider_ver, LV_SLIDER_MODE_RANGE);
    lv_obj_set_style_base_dir(slider_ver, LV_BASE_DIR_RTL, 0);

    lv_slider_set_value(slider_ver, 80, LV_ANIM_OFF);
    lv_slider_set_start_value(slider_ver, 20, LV_ANIM_OFF);
    lv_obj_update_layout(slider_ver);

    /* Force redraw to update knob areas */
    lv_obj_invalidate(slider_ver);
    lv_refr_now(NULL);

    lv_slider_t * ptr_ver = (lv_slider_t *) slider_ver;
    lv_area_t right_knob = ptr_ver->right_knob_area;

    /* Click above the top/right knob */
    assert_slider_drag_start_selection(slider_ver,
                                       (right_knob.x1 + right_knob.x2) / 2, right_knob.y1 - 20,
                                       &ptr_ver->bar.cur_value, -1);
}

#endif
