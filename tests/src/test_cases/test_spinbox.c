#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * spinbox_negative_min_range = NULL;
static lv_obj_t * spinbox_zero_min_range = NULL;

static const int32_t SPINBOX_NEGATIVE_MIN_RANGE_VALUE = -11;
static const int32_t SPINBOX_ZERO_MIN_RANGE_VALUE = 0;
static const int32_t SPINBOX_NEGATIVE_MAX_RANGE_VALUE = 12;
static const uint8_t SPINBOX_DECIMAL_POSITION = 1U;

void setUp(void)
{
    active_screen = lv_scr_act();
    spinbox_negative_min_range = lv_spinbox_create(active_screen);
    spinbox_zero_min_range = lv_spinbox_create(active_screen);

    lv_spinbox_set_range(spinbox_negative_min_range, SPINBOX_NEGATIVE_MIN_RANGE_VALUE, SPINBOX_NEGATIVE_MAX_RANGE_VALUE);
    lv_spinbox_set_range(spinbox_zero_min_range, SPINBOX_ZERO_MIN_RANGE_VALUE, SPINBOX_NEGATIVE_MAX_RANGE_VALUE);
}

void tearDown(void)
{
    lv_obj_del(spinbox_negative_min_range);
    lv_obj_del(spinbox_zero_min_range);
}

/* See issue #3559 for more info */
void test_spinbox_decrement_when_min_range_is_negative(void)
{
    /* Current spinbox value is 2 */
    const int32_t expected_value = -11;
    lv_spinbox_set_value(spinbox_negative_min_range, 2);

    /* Move to decimal position and decrement */
    lv_spinbox_set_pos(spinbox_negative_min_range, SPINBOX_DECIMAL_POSITION);
    lv_spinbox_decrement(spinbox_negative_min_range);

    /* We expect value now being -11 */
    int32_t actual_value = lv_spinbox_get_value(spinbox_negative_min_range);

    TEST_ASSERT_EQUAL_INT32(expected_value, actual_value);
}

void test_spinbox_decrement_when_min_range_is_zero(void)
{
    /* Current spinbox value is 2 */
    const int32_t expected_value = 0;
    lv_spinbox_set_value(spinbox_zero_min_range, 2);

    /* Move to decimal position and decrement */
    lv_spinbox_set_pos(spinbox_zero_min_range, SPINBOX_DECIMAL_POSITION);
    lv_spinbox_decrement(spinbox_zero_min_range);

    /* We expect value now being 0 */
    int32_t actual_value = lv_spinbox_get_value(spinbox_zero_min_range);

    TEST_ASSERT_EQUAL_INT32(expected_value, actual_value);
}

#endif
