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

#endif /*LV_BUILD_TEST*/
