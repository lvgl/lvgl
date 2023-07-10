#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include "lv_test_indev.h"

/* This function runs before each test */
void setUp(void);

void test_arc_creation_successfull(void);
void test_arc_should_truncate_to_max_range_when_new_value_exceeds_it(void);
void test_arc_should_truncate_to_min_range_when_new_value_is_inferior(void);
void test_arc_should_update_value_after_updating_range(void);
void test_arc_should_update_angles_when_changing_to_symmetrical_mode(void);
void test_arc_should_update_angles_when_changing_to_symmetrical_mode_value_more_than_middle_range(void);
void test_arc_angles_when_reversed(void);

static lv_obj_t * active_screen = NULL;
static lv_obj_t * arc = NULL;
static uint32_t event_cnt;

static void dummy_event_cb(lv_event_t * e);

void setUp(void)
{
    active_screen = lv_scr_act();
}

void test_arc_creation_successfull(void)
{
    arc = lv_arc_create(active_screen);

    TEST_ASSERT_NOT_NULL(arc);
}

void test_arc_should_truncate_to_max_range_when_new_value_exceeds_it(void)
{
    /* Default max range is 100 */
    int16_t value_after_truncation = 100;

    arc = lv_arc_create(active_screen);

    lv_arc_set_value(arc, 200);

    TEST_ASSERT_EQUAL_INT16(value_after_truncation, lv_arc_get_value(arc));
}

void test_arc_should_truncate_to_min_range_when_new_value_is_inferior(void)
{
    /* Default min range is 100 */
    int16_t value_after_truncation = 0;

    arc = lv_arc_create(active_screen);

    lv_arc_set_value(arc, 0);

    TEST_ASSERT_EQUAL_INT16(value_after_truncation, lv_arc_get_value(arc));
}

void test_arc_should_update_value_after_updating_range(void)
{
    int16_t value_after_updating_max_range = 50;
    int16_t value_after_updating_min_range = 30;

    arc = lv_arc_create(active_screen);

    lv_arc_set_value(arc, 80);
    lv_arc_set_range(arc, 1, 50);

    TEST_ASSERT_EQUAL_INT16(value_after_updating_max_range, lv_arc_get_value(arc));

    lv_arc_set_value(arc, 10);
    lv_arc_set_range(arc, 30, 50);

    TEST_ASSERT_EQUAL_INT16(value_after_updating_min_range, lv_arc_get_value(arc));
}

void test_arc_should_update_angles_when_changing_to_symmetrical_mode(void)
{
    int16_t expected_angle_start = 135;
    int16_t expected_angle_end = 270;

    /* start angle is 135, end angle is 45 at creation */
    arc = lv_arc_create(active_screen);
    lv_arc_set_mode(arc, LV_ARC_MODE_SYMMETRICAL);

    TEST_ASSERT_EQUAL_INT16(expected_angle_start, lv_arc_get_angle_start(arc));
    TEST_ASSERT_EQUAL_INT16(expected_angle_end, lv_arc_get_angle_end(arc));
}

void test_arc_should_update_angles_when_changing_to_symmetrical_mode_value_more_than_middle_range(void)
{
    int16_t expected_angle_start = 270;
    int16_t expected_angle_end = 45;

    /* start angle is 135, end angle is 45 at creation */
    arc = lv_arc_create(active_screen);
    lv_arc_set_value(arc, 100);
    lv_arc_set_mode(arc, LV_ARC_MODE_SYMMETRICAL);

    TEST_ASSERT_EQUAL_INT16(expected_angle_start, lv_arc_get_angle_start(arc));
    TEST_ASSERT_EQUAL_INT16(expected_angle_end, lv_arc_get_angle_end(arc));
}

/* See #2522 for more information */
void test_arc_angles_when_reversed(void)
{
    uint16_t expected_start_angle = 54;
    uint16_t expected_end_angle = 90;
    int16_t expected_value = 40;

    lv_obj_t * arcBlack;
    arcBlack = lv_arc_create(lv_scr_act());

    lv_arc_set_mode(arcBlack, LV_ARC_MODE_REVERSE);

    lv_arc_set_bg_angles(arcBlack, 0, 90);

    lv_arc_set_value(arcBlack, expected_value);

    TEST_ASSERT_EQUAL_UINT16(expected_start_angle, lv_arc_get_angle_start(arcBlack));
    TEST_ASSERT_EQUAL_UINT16(expected_end_angle, lv_arc_get_angle_end(arcBlack));
    TEST_ASSERT_EQUAL_INT16(expected_value, lv_arc_get_value(arcBlack));
}

void test_arc_click_area_with_adv_hittest(void)
{
    arc = lv_arc_create(lv_scr_act());
    lv_obj_set_size(arc, 100, 100);
    lv_obj_set_style_arc_width(arc, 10, 0);
    lv_obj_add_flag(arc, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_add_event_cb(arc, dummy_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_set_ext_click_area(arc, 5);

    /*No click detected at the middle*/
    event_cnt = 0;
    lv_test_mouse_click_at(50, 50);
    TEST_ASSERT_EQUAL_UINT32(0, event_cnt);

    /*No click close to the radius - bg_arc - ext_click_area*/
    event_cnt = 0;
    lv_test_mouse_click_at(83, 50);
    TEST_ASSERT_EQUAL_UINT32(0, event_cnt);

    /*Click on the radius - bg_arc - ext_click_area*/
    event_cnt = 0;
    lv_test_mouse_click_at(86, 50);
    TEST_ASSERT_GREATER_THAN(0, event_cnt);

    /*Click on the radius + ext_click_area*/
    event_cnt = 0;
    lv_test_mouse_click_at(104, 50);
    TEST_ASSERT_GREATER_THAN(0, event_cnt);

    /*No click beyond to the radius + ext_click_area*/
    event_cnt = 0;
    lv_test_mouse_click_at(106, 50);
    TEST_ASSERT_EQUAL_UINT32(0, event_cnt);
}

static void dummy_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    event_cnt++;
}

#endif
