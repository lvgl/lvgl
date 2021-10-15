#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t *active_screen = NULL;
static lv_obj_t *arc = NULL;

void setUp(void)
{
    active_screen = lv_scr_act();
    arc = lv_arc_create(active_screen);
}

void tearDown(void)
{
    active_screen = NULL;
    arc = NULL;
}

void test_arc_creation_successfull(void)
{
    TEST_ASSERT_NOT_NULL(arc);
}

void test_arc_should_truncate_to_max_range_when_new_value_exceeds_it(void)
{
    /* Default max range is 100 */
    int16_t value_after_truncation = 100;
    
    lv_arc_set_value(arc, 200);

    TEST_ASSERT_EQUAL_INT16(value_after_truncation, lv_arc_get_value(arc));
}

void test_arc_should_truncate_to_min_range_when_new_value_is_inferior(void)
{
    /* Default min range is 100 */
    int16_t value_after_truncation = 0;

    lv_arc_set_value(arc, 0);

    TEST_ASSERT_EQUAL_INT16(value_after_truncation, lv_arc_get_value(arc));
}

void test_arc_should_update_value_after_updating_range(void)
{
    int16_t value_after_updating_max_range = 50;
    int16_t value_after_updating_min_range = 30;

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
    lv_arc_set_mode(arc, LV_ARC_MODE_SYMMETRICAL);

    TEST_ASSERT_EQUAL_INT16(expected_angle_start, lv_arc_get_angle_start(arc));
    TEST_ASSERT_EQUAL_INT16(expected_angle_end, lv_arc_get_angle_end(arc));
}

void test_arc_should_update_angles_when_changing_to_symmetrical_mode_value_more_than_middle_range(void)
{
    int16_t expected_angle_start = 270;
    int16_t expected_angle_end = 45;

    /* start angle is 135, end angle is 45 at creation */
    lv_arc_set_value(arc, 100);
    lv_arc_set_mode(arc, LV_ARC_MODE_SYMMETRICAL);

    TEST_ASSERT_EQUAL_INT16(expected_angle_start, lv_arc_get_angle_start(arc));
    TEST_ASSERT_EQUAL_INT16(expected_angle_end, lv_arc_get_angle_end(arc));
}

/* See #2522 for more information */
void test_arc_angles_when_reversed(void)
{
    uint16_t expected_start_angle = 36;
    uint16_t expected_end_angle = 90;
    int16_t expected_value = 40;

    lv_arc_set_mode(arc, LV_ARC_MODE_REVERSE);

    lv_arc_set_bg_angles(arc, 0, 90);
    
    lv_arc_set_value(arc, expected_value);

    TEST_ASSERT_EQUAL_UINT16(expected_start_angle, lv_arc_get_angle_start(arc));
    TEST_ASSERT_EQUAL_UINT16(expected_end_angle, lv_arc_get_angle_end(arc));
    TEST_ASSERT_EQUAL_INT16(expected_value, lv_arc_get_value(arc));
}

#endif
