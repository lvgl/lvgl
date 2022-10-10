#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include "lv_test_indev.h"
#include <stdio.h>

void setUp(void);
void tearDown(void);
void test_colorwheel_create_successful_recolor_true(void);
void test_colorwheel_set_hsv_changed(void);
void test_colorwheel_set_hsv_not_changed(void);
void test_colorwheel_set_rgb_changed(void);
void test_colorwheel_set_rgb_not_changed(void);
void test_colorwheel_set_mode_hue(void);
void test_colorwheel_set_mode_saturation(void);
void test_colorwheel_set_mode_value(void);
void test_colorwheel_event_keys_right_and_up_increment_value_by_one(void);
void test_colorwheel_event_keys_left_and_down_decrement_value_by_one(void);
void test_colorwheel_set_mode_fixed_true(void);
void test_colorwheel_set_mode_fixed_false(void);
void test_colorwheel_angle_to_mode_color_fast_in_mode_value(void);
void test_colorwheel_angle_to_mode_color_fast_in_mode_saturation(void);
void test_colorwheel_event_pressing(void);
void test_colorwheel_event_pressed_mode_hue(void);
void test_colorwheel_event_pressed_mode_saturation(void);
void test_colorwheel_event_pressed_mode_value(void);

static lv_obj_t * active_screen = NULL;
static lv_obj_t * colorwheel = NULL;

void setUp(void)
{
    active_screen = lv_scr_act();

    colorwheel = lv_colorwheel_create(active_screen, true);
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_colorwheel_create_successful_recolor_true(void)
{
    TEST_ASSERT_NOT_NULL(colorwheel);

    TEST_ASSERT_EQUAL_SCREENSHOT("colorwheel_01.png");
}

void test_colorwheel_set_hsv_changed(void)
{
    // color black (0x000000) is different from default
    lv_color_hsv_t hsv_to_set = lv_color_rgb_to_hsv(0, 0, 0);

    bool hsv_changed = lv_colorwheel_set_hsv(colorwheel, hsv_to_set);

    TEST_ASSERT_TRUE(hsv_changed);
}

void test_colorwheel_set_hsv_not_changed(void)
{
    lv_color_hsv_t default_hsv = lv_colorwheel_get_hsv(colorwheel);

    bool hsv_changed = lv_colorwheel_set_hsv(colorwheel, default_hsv);

    TEST_ASSERT_FALSE(hsv_changed);
}

void test_colorwheel_set_rgb_changed(void)
{
    // color black (0x000000) is different from default
    lv_color_t rgb_to_set = lv_color_make(0, 0, 0);

    bool rgb_changed = lv_colorwheel_set_rgb(colorwheel, rgb_to_set);

    TEST_ASSERT_TRUE(rgb_changed);
}

void test_colorwheel_set_rgb_not_changed(void)
{
    lv_color_t default_rgb = lv_colorwheel_get_rgb(colorwheel);

    bool rgb_changed = lv_colorwheel_set_rgb(colorwheel, default_rgb);

    TEST_ASSERT_FALSE(rgb_changed);
}

void test_colorwheel_set_mode_hue(void)
{
    lv_colorwheel_set_mode(colorwheel, LV_COLORWHEEL_MODE_HUE);

    TEST_ASSERT_EQUAL(LV_COLORWHEEL_MODE_HUE, lv_colorwheel_get_color_mode(colorwheel));
}

void test_colorwheel_set_mode_saturation(void)
{
    lv_colorwheel_set_mode(colorwheel, LV_COLORWHEEL_MODE_SATURATION);

    TEST_ASSERT_EQUAL(LV_COLORWHEEL_MODE_SATURATION, lv_colorwheel_get_color_mode(colorwheel));
}

void test_colorwheel_set_mode_value(void)
{
    lv_colorwheel_set_mode(colorwheel, LV_COLORWHEEL_MODE_VALUE);

    TEST_ASSERT_EQUAL(LV_COLORWHEEL_MODE_VALUE, lv_colorwheel_get_color_mode(colorwheel));
}

void test_colorwheel_event_keys_right_and_up_increment_value_by_one(void)
{
    uint32_t keyCode = LV_KEY_RIGHT;

    // start with a known color to avoid carry over (i.e. %360 / %100)
    lv_colorwheel_set_hsv(colorwheel, lv_color_rgb_to_hsv(0, 0, 0));

    lv_color_hsv_t hsv_cur = lv_colorwheel_get_hsv(colorwheel);

    lv_event_send(colorwheel, LV_EVENT_KEY, &keyCode);

    lv_color_hsv_t hsv_new = lv_colorwheel_get_hsv(colorwheel);

    TEST_ASSERT_EQUAL(hsv_cur.h + 1, hsv_new.h);

    // Change mode to test increase in saturation
    lv_colorwheel_set_mode(colorwheel, LV_COLORWHEEL_MODE_SATURATION);
    hsv_cur = hsv_new;

    keyCode = LV_KEY_UP;
    lv_event_send(colorwheel, LV_EVENT_KEY, &keyCode);

    hsv_new = lv_colorwheel_get_hsv(colorwheel);

    TEST_ASSERT_EQUAL(hsv_cur.s + 1, hsv_new.s);

    // Change mode to test increase in value
    lv_colorwheel_set_mode(colorwheel, LV_COLORWHEEL_MODE_VALUE);
    hsv_cur = hsv_new;

    keyCode = LV_KEY_UP;
    lv_event_send(colorwheel, LV_EVENT_KEY, &keyCode);

    hsv_new = lv_colorwheel_get_hsv(colorwheel);

    TEST_ASSERT_EQUAL(hsv_cur.v + 1, hsv_new.v);
}

void test_colorwheel_event_keys_left_and_down_decrement_value_by_one(void)
{
    uint32_t keyCode = LV_KEY_LEFT;

    // start with a known color to avoid carry over (i.e. %360 / %100)
    lv_colorwheel_set_hsv(colorwheel, lv_color_rgb_to_hsv(0, 255, 0));

    lv_color_hsv_t hsv_cur = lv_colorwheel_get_hsv(colorwheel);

    lv_event_send(colorwheel, LV_EVENT_KEY, &keyCode);

    lv_color_hsv_t hsv_new = lv_colorwheel_get_hsv(colorwheel);

    TEST_ASSERT_EQUAL(hsv_cur.h - 1, hsv_new.h);

    // Change mode to test decrease in saturation
    lv_colorwheel_set_mode(colorwheel, LV_COLORWHEEL_MODE_SATURATION);
    hsv_cur = hsv_new;

    keyCode = LV_KEY_DOWN;
    lv_event_send(colorwheel, LV_EVENT_KEY, &keyCode);

    hsv_new = lv_colorwheel_get_hsv(colorwheel);

    TEST_ASSERT_EQUAL(hsv_cur.s - 1, hsv_new.s);

    // Change mode to test decrease in value
    lv_colorwheel_set_mode(colorwheel, LV_COLORWHEEL_MODE_VALUE);
    hsv_cur = hsv_new;

    keyCode = LV_KEY_DOWN;
    lv_event_send(colorwheel, LV_EVENT_KEY, &keyCode);

    hsv_new = lv_colorwheel_get_hsv(colorwheel);

    TEST_ASSERT_EQUAL(hsv_cur.v - 1, hsv_new.v);
}

void test_colorwheel_set_mode_fixed_true(void)
{
    lv_colorwheel_set_mode_fixed(colorwheel, true);

    TEST_ASSERT_TRUE(lv_colorwheel_get_color_mode_fixed(colorwheel));
}

void test_colorwheel_set_mode_fixed_false(void)
{
    lv_colorwheel_set_mode_fixed(colorwheel, false);

    TEST_ASSERT_FALSE(lv_colorwheel_get_color_mode_fixed(colorwheel));
}

void test_colorwheel_angle_to_mode_color_fast_in_mode_value(void)
{
    lv_colorwheel_set_mode(colorwheel, LV_COLORWHEEL_MODE_VALUE);

    lv_colorwheel_set_hsv(colorwheel, lv_color_rgb_to_hsv(64, 255, 128));

    TEST_ASSERT_EQUAL_SCREENSHOT("colorwheel_02.png");
}

void test_colorwheel_angle_to_mode_color_fast_in_mode_saturation(void)
{
    lv_colorwheel_set_mode(colorwheel, LV_COLORWHEEL_MODE_SATURATION);

    lv_colorwheel_set_hsv(colorwheel, lv_color_rgb_to_hsv(64, 255, 128));

    TEST_ASSERT_EQUAL_SCREENSHOT("colorwheel_03.png");
}

void test_colorwheel_event_pressing(void)
{
    lv_group_t * g = lv_group_create();
    lv_indev_set_group(lv_test_encoder_indev, g);

    lv_group_add_obj(g, colorwheel);

    lv_test_encoder_turn(1);
    lv_test_encoder_click();

    lv_indev_set_group(lv_test_encoder_indev, NULL);
    lv_group_del(g);
}

void test_colorwheel_event_pressed_mode_hue(void)
{
    lv_test_mouse_click_at(50, 50);

    // the effect of that event is reset of value
    lv_color_hsv_t hsv_cur = lv_colorwheel_get_hsv(colorwheel);

    TEST_ASSERT_EQUAL_INT16(0, hsv_cur.h);
}

void test_colorwheel_event_pressed_mode_saturation(void)
{
    lv_colorwheel_set_mode(colorwheel, LV_COLORWHEEL_MODE_SATURATION);

    lv_test_mouse_click_at(50, 50);

    // the effect of that event is reset of value
    lv_color_hsv_t hsv_cur = lv_colorwheel_get_hsv(colorwheel);

    TEST_ASSERT_EQUAL_INT16(100, hsv_cur.s);
}

void test_colorwheel_event_pressed_mode_value(void)
{
    lv_colorwheel_set_mode(colorwheel, LV_COLORWHEEL_MODE_VALUE);

    lv_test_mouse_click_at(50, 50);

    // the effect of that event is reset of value
    lv_color_hsv_t hsv_cur = lv_colorwheel_get_hsv(colorwheel);

    TEST_ASSERT_EQUAL_INT16(100, hsv_cur.v);
}

#endif
