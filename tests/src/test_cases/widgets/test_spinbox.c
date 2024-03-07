#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include "lv_test_indev.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * spinbox_negative_min_range = NULL;
static lv_obj_t * spinbox_zero_min_range = NULL;
static lv_obj_t * spinbox_events = NULL;
static lv_group_t * g = NULL;

static const int32_t SPINBOX_NEGATIVE_MIN_RANGE_VALUE = -11;
static const int32_t SPINBOX_ZERO_MIN_RANGE_VALUE = 0;
static const int32_t SPINBOX_NEGATIVE_MAX_RANGE_VALUE = 12;
static const uint8_t SPINBOX_DECIMAL_POSITION = 1U;

void setUp(void)
{
    active_screen = lv_screen_active();
    spinbox_negative_min_range = lv_spinbox_create(active_screen);
    spinbox_zero_min_range = lv_spinbox_create(active_screen);
    spinbox_events = lv_spinbox_create(active_screen);

    lv_spinbox_set_range(spinbox_negative_min_range, SPINBOX_NEGATIVE_MIN_RANGE_VALUE, SPINBOX_NEGATIVE_MAX_RANGE_VALUE);
    lv_spinbox_set_range(spinbox_zero_min_range, SPINBOX_ZERO_MIN_RANGE_VALUE, SPINBOX_NEGATIVE_MAX_RANGE_VALUE);

    g = lv_group_create();
    lv_indev_set_group(lv_test_encoder_indev, g);
}

void tearDown(void)
{
    lv_group_remove_obj(spinbox_events);

    lv_obj_delete(spinbox_negative_min_range);
    lv_obj_delete(spinbox_zero_min_range);
    lv_obj_delete(spinbox_events);

    lv_obj_clean(active_screen);
}

/* See issue #3559 for more info */
void test_spinbox_decrement_when_min_range_is_negative(void)
{
    /* Current spinbox value is 2 */
    const int32_t expected_value = -11;
    lv_spinbox_set_value(spinbox_negative_min_range, 2);

    /* Change cursor position of spinbox to 10 */
    lv_spinbox_set_cursor_pos(spinbox_negative_min_range, SPINBOX_DECIMAL_POSITION);
    lv_spinbox_decrement(spinbox_negative_min_range);
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

    /* Change cursor position of spinbox to 10 */
    lv_spinbox_set_cursor_pos(spinbox_zero_min_range, SPINBOX_DECIMAL_POSITION);
    lv_spinbox_decrement(spinbox_zero_min_range);
    lv_spinbox_decrement(spinbox_zero_min_range);

    /* We expect value now being 0 */
    int32_t actual_value = lv_spinbox_get_value(spinbox_zero_min_range);

    TEST_ASSERT_EQUAL_INT32(expected_value, actual_value);
}

void test_spinbox_position_selection(void)
{
    /* Assert step is 1 when selecting the lowest possible position */
    lv_spinbox_set_cursor_pos(spinbox_zero_min_range, 0);
    TEST_ASSERT_EQUAL(1, lv_spinbox_get_step(spinbox_zero_min_range));

    /* The other branch in the if */
    lv_spinbox_set_cursor_pos(spinbox_zero_min_range, 1);
    TEST_ASSERT_EQUAL(10, lv_spinbox_get_step(spinbox_zero_min_range));

    /* When not possible to select the indicated position */
    lv_obj_t * tmp;
    tmp = lv_spinbox_create(active_screen);
    lv_spinbox_set_range(tmp, 0, 10);
    lv_spinbox_set_cursor_pos(tmp, 2);
    TEST_ASSERT_EQUAL(1, lv_spinbox_get_step(tmp));
    lv_obj_clean(tmp);
}

void test_spinbox_set_range(void)
{
    int32_t range_max = 40;
    int32_t range_min = 20;

    lv_obj_t * tmp;
    tmp = lv_spinbox_create(active_screen);
    lv_spinbox_set_range(tmp, 0, 100);
    lv_spinbox_set_value(tmp, 50);

    /* Validate value gets updated when range_max is smaller */
    lv_spinbox_set_range(tmp, 0, range_max);

    TEST_ASSERT_EQUAL(range_max, lv_spinbox_get_value(tmp));

    /* Validate value gets updated when range_min is bigger */
    lv_spinbox_set_value(tmp, 5);
    lv_spinbox_set_range(tmp, range_min, range_max);

    TEST_ASSERT_EQUAL(range_min, lv_spinbox_get_value(tmp));

    lv_obj_clean(tmp);
}

void test_spinbox_step_prev(void)
{
    lv_obj_t * tmp = lv_spinbox_create(active_screen);

    /* When next step is bigger than biggest range */
    lv_spinbox_set_range(tmp, 0, 5);
    lv_spinbox_step_prev(tmp);
    TEST_ASSERT_EQUAL(1, lv_spinbox_get_step(tmp));

    lv_spinbox_step_next(tmp);
    /* When next step is smaller than range_max */
    lv_spinbox_set_range(tmp, 0, 20);
    lv_spinbox_step_prev(tmp);
    TEST_ASSERT_EQUAL(10, lv_spinbox_get_step(tmp));

    lv_spinbox_step_next(tmp);
    /* When next step is smaller than abs(range_min) */
    lv_spinbox_set_range(tmp, -25, 5);
    lv_spinbox_step_prev(tmp);
    TEST_ASSERT_EQUAL(10, lv_spinbox_get_step(tmp));

    lv_obj_clean(tmp);
}

void test_spinbox_rollover(void)
{
    lv_obj_t * tmp = lv_spinbox_create(active_screen);

    lv_spinbox_set_rollover(tmp, true);
    TEST_ASSERT_TRUE(lv_spinbox_get_rollover(tmp));

    lv_spinbox_set_rollover(tmp, false);
    TEST_ASSERT_FALSE(lv_spinbox_get_rollover(tmp));

    lv_obj_clean(tmp);
}

void test_spinbox_event_key(void)
{
    /* Spinbox should increment it's value by one after receiving the LV_KEY_UP event */
    lv_spinbox_set_value(spinbox_events, 0);
    uint32_t key = LV_KEY_UP;
    lv_obj_send_event(spinbox_events, LV_EVENT_KEY, (void *) &key);

    TEST_ASSERT_EQUAL(1, lv_spinbox_get_value(spinbox_events));

    /* Spinbox should decrement it's value by one after receiving the LV_KEY_DOWN event */
    key = LV_KEY_DOWN;
    lv_obj_send_event(spinbox_events, LV_EVENT_KEY, (void *) &key);

    TEST_ASSERT_EQUAL(0, lv_spinbox_get_value(spinbox_events));

    /* Spinbox should multiply it's step vale by 10 after receiving the LV_KEY_LEFT event */
    int32_t step = lv_spinbox_get_step(spinbox_events);
    key = LV_KEY_LEFT;
    lv_obj_send_event(spinbox_events, LV_EVENT_KEY, (void *) &key);

    TEST_ASSERT_EQUAL(step * 10, lv_spinbox_get_step(spinbox_events));

    /* Spinbox should divide it's step vale by 10 after receiving the LV_KEY_RIGHT event */
    step = lv_spinbox_get_step(spinbox_events);
    key = LV_KEY_RIGHT;
    lv_obj_send_event(spinbox_events, LV_EVENT_KEY, (void *) &key);

    TEST_ASSERT_EQUAL(step / 10, lv_spinbox_get_step(spinbox_events));
}

void test_spinbox_event_key_encoder_indev_turn_right(void)
{
    /* Setup group and encoder indev */
    lv_group_add_obj(g, spinbox_events);

    /* Spinbox should increment it's value by one step after receiving the LV_KEY_UP event */
    lv_spinbox_set_value(spinbox_events, 0);

    lv_test_encoder_click();
    lv_test_encoder_turn(1);

    TEST_ASSERT_EQUAL(1, lv_spinbox_get_value(spinbox_events));
}

void test_spinbox_event_key_encoder_indev_turn_left(void)
{
    int32_t value = 10;
    /* Setup group and encoder indev */
    lv_group_add_obj(g, spinbox_events);

    /* Spinbox should decrement it's value by one step after receiving the LV_KEY_UP event */
    lv_spinbox_set_value(spinbox_events, value);
    lv_spinbox_set_cursor_pos(spinbox_events, 0);

    lv_test_encoder_click();
    lv_test_encoder_turn(-1);
    TEST_ASSERT_EQUAL(value - 1, lv_spinbox_get_value(spinbox_events));
}

void test_spinbox_event_key_encoder_indev_editing_group(void)
{
    int32_t value = 10;
    /* Setup group and encoder indev */
    lv_spinbox_set_range(spinbox_events, 0, 20);
    lv_group_add_obj(g, spinbox_events);
    lv_group_set_editing(g, true);

    lv_spinbox_set_value(spinbox_events, value);
    lv_spinbox_set_cursor_pos(spinbox_events, 0);

    lv_test_encoder_click();
    lv_test_encoder_turn(-1);
    TEST_ASSERT_EQUAL(0, lv_spinbox_get_value(spinbox_events));
    /* digit_count is 5, so we expect to be in the position of the MSB digit */
    TEST_ASSERT_EQUAL(1000, lv_spinbox_get_step(spinbox_events));

    /* Test with digit_count == 1 */
    lv_spinbox_set_digit_format(spinbox_events, 1, 2);
    lv_spinbox_set_cursor_pos(spinbox_events, 0);

    lv_test_encoder_click();
    lv_test_encoder_turn(-1);
    TEST_ASSERT_EQUAL(0, lv_spinbox_get_value(spinbox_events));
    /* digit_count is 1, so we expect to be in the same position */
    TEST_ASSERT_EQUAL(1, lv_spinbox_get_step(spinbox_events));
}

void test_spinbox_event_key_encoder_indev_editing_group_left_step_direction(void)
{
    int32_t value = 10;
    /* Setup group and encoder indev */
    lv_spinbox_set_digit_step_direction(spinbox_events, LV_DIR_LEFT);
    lv_spinbox_set_range(spinbox_events, 0, 20);
    lv_group_add_obj(g, spinbox_events);
    lv_group_set_editing(g, true);

    lv_spinbox_set_value(spinbox_events, value);
    lv_spinbox_set_cursor_pos(spinbox_events, 0);

    lv_test_encoder_click();
    lv_test_encoder_turn(-1);
    TEST_ASSERT_EQUAL(0, lv_spinbox_get_value(spinbox_events));
    /* digit_count is 5, we expect to be in the position next to the left */
    TEST_ASSERT_EQUAL(10, lv_spinbox_get_step(spinbox_events));

    /* Test with digit_count == 1 */
    lv_spinbox_set_digit_format(spinbox_events, 2, 2);
    lv_spinbox_set_cursor_pos(spinbox_events, 1);

    lv_test_encoder_click();
    lv_test_encoder_turn(-1);
    TEST_ASSERT_EQUAL(0, lv_spinbox_get_value(spinbox_events));
    /* digit_count is 1, so we expect to be in the same position */
    TEST_ASSERT_EQUAL(1, lv_spinbox_get_step(spinbox_events));
}

void test_spinbox_event_release(void)
{
    lv_spinbox_set_value(spinbox_events, 0);
    lv_spinbox_set_digit_format(spinbox_events, 5, 2);

    /* Set cursor in least significant decimal digit */
    lv_spinbox_set_cursor_pos(spinbox_events, 0);
    lv_obj_send_event(spinbox_events, LV_EVENT_RELEASED, NULL);

    TEST_ASSERT_EQUAL(1, lv_spinbox_get_step(spinbox_events));
}

void test_spinbox_zero_crossing(void)
{
    int32_t value = -13;
    /* Setup group and encoder indev */
    lv_spinbox_set_digit_step_direction(spinbox_events, LV_DIR_LEFT);
    lv_spinbox_set_range(spinbox_events, -20, 20);
    lv_group_add_obj(g, spinbox_events);

    lv_spinbox_set_value(spinbox_events, value);
    lv_spinbox_set_cursor_pos(spinbox_events, 1);

    lv_test_encoder_click();
    lv_test_encoder_turn(1);
    TEST_ASSERT_EQUAL(-3, lv_spinbox_get_value(spinbox_events));

    lv_test_encoder_turn(1);
    TEST_ASSERT_EQUAL(3, lv_spinbox_get_value(spinbox_events));

    lv_test_encoder_turn(1);
    TEST_ASSERT_EQUAL(13, lv_spinbox_get_value(spinbox_events));
}

#endif
