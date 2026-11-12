#if LV_BUILD_TEST
#include "unity/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

void test_spinbox_step_rejects_invalid_values(void)
{
    lv_obj_t * spinbox = lv_spinbox_create(lv_screen_active());
    lv_spinbox_set_step(spinbox, 10);

    lv_spinbox_set_step(spinbox, 0);
    TEST_ASSERT_EQUAL_INT32(10, lv_spinbox_get_step(spinbox));

    lv_spinbox_set_step(spinbox, (uint32_t)INT32_MAX + 1U);
    TEST_ASSERT_EQUAL_INT32(10, lv_spinbox_get_step(spinbox));

    lv_spinbox_set_step(spinbox, UINT32_MAX);
    TEST_ASSERT_EQUAL_INT32(10, lv_spinbox_get_step(spinbox));

    lv_spinbox_step_prev(spinbox);
    TEST_ASSERT_EQUAL_INT32(100, lv_spinbox_get_step(spinbox));
    lv_obj_delete(spinbox);
}

void test_spinbox_cursor_rejects_invalid_positions(void)
{
    lv_obj_t * spinbox = lv_spinbox_create(lv_screen_active());
    lv_spinbox_set_digit_format(spinbox, 10, 0);
    lv_spinbox_set_range(spinbox, 0, INT32_MAX);
    lv_spinbox_set_step(spinbox, 10);

    const uint32_t positions[] = {10, 11, 12, 128, 256, UINT32_MAX};
    for(uint32_t i = 0; i < sizeof(positions) / sizeof(positions[0]); i++) {
        lv_spinbox_set_cursor_pos(spinbox, positions[i]);
        TEST_ASSERT_EQUAL_INT32(10, lv_spinbox_get_step(spinbox));
    }
    lv_obj_delete(spinbox);
}

void test_spinbox_cursor_accepts_supported_positions(void)
{
    lv_obj_t * spinbox = lv_spinbox_create(lv_screen_active());
    lv_spinbox_set_digit_format(spinbox, 10, 0);
    lv_spinbox_set_range(spinbox, 0, INT32_MAX);

    int32_t expected_step = 1;
    for(uint32_t pos = 0; pos < 10; pos++) {
        lv_spinbox_set_cursor_pos(spinbox, pos);
        TEST_ASSERT_EQUAL_INT32(expected_step, lv_spinbox_get_step(spinbox));
        if(pos < 9) expected_step *= 10;
    }

    lv_spinbox_set_cursor_pos(spinbox, 0);
    TEST_ASSERT_EQUAL_INT32(1, lv_spinbox_get_step(spinbox));
    lv_spinbox_set_range(spinbox, 0, 99);
    lv_spinbox_set_cursor_pos(spinbox, 1);
    lv_spinbox_set_cursor_pos(spinbox, 2);
    TEST_ASSERT_EQUAL_INT32(10, lv_spinbox_get_step(spinbox));
    lv_obj_delete(spinbox);
}

#endif /*LV_BUILD_TEST*/
