#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

/* STATIC VARIABLES */
static uint32_t test_tick_value = 0;
static uint32_t test_delay_called = 0;

/* STATIC FUNCTIONS */
static uint32_t test_tick_cb(void)
{
    return test_tick_value;
}

static void test_delay_cb(uint32_t ms)
{
    test_delay_called = ms;
}

static uint32_t test_tick_inc_cb(void)
{
    return test_tick_value ++;
}

/* TEST FUNCTIONS */
void setUp(void)
{
    /* Function run before every test */
    /* Reset test variables */
    test_tick_value = 0;
    test_delay_called = 0;

    /* Reset tick module */
    lv_tick_set_cb(NULL);
    lv_delay_set_cb(NULL);
}

void tearDown(void)
{
    /* Function run after every test */
}

void test_tick_inc_and_get(void)
{
    /* Initial tick should be 0 */
    TEST_ASSERT_EQUAL(0, lv_tick_get());

    /* Increment tick by 10 */
    lv_tick_inc(10);
    TEST_ASSERT_EQUAL(10, lv_tick_get());

    /* Increment tick by 100 */
    lv_tick_inc(100);
    TEST_ASSERT_EQUAL(110, lv_tick_get());
}

void test_tick_get_with_callback(void)
{
    /* Set callback */
    lv_tick_set_cb(test_tick_cb);
    TEST_ASSERT_EQUAL(test_tick_cb, lv_tick_get_cb());

    /* Test callback value */
    test_tick_value = 1234;
    TEST_ASSERT_EQUAL(1234, lv_tick_get());

    test_tick_value = 5678;
    uint32_t tmp_tick = lv_tick_get();
    TEST_ASSERT_EQUAL(5678, tmp_tick);
}

void test_tick_elaps_normal(void)
{
    uint32_t start = lv_tick_get();
    lv_tick_inc(50);
    TEST_ASSERT_EQUAL(50, lv_tick_elaps(start));

    lv_tick_inc(100);
    TEST_ASSERT_EQUAL(150, lv_tick_elaps(start));
}

void test_tick_elaps_overflow(void)
{
    /* Simulate near overflow */
    test_tick_value = UINT32_MAX - 10;
    lv_tick_set_cb(test_tick_cb);
    uint32_t start = lv_tick_get();

    /* Simulate overflow */
    test_tick_value = 20;
    TEST_ASSERT_EQUAL(31, lv_tick_elaps(start));
}

void test_delay_ms_with_callback(void)
{
    lv_delay_set_cb(test_delay_cb);
    lv_delay_ms(100);
    TEST_ASSERT_EQUAL(100, test_delay_called);
}

void test_delay_ms_without_callback(void)
{
    lv_tick_set_cb(test_tick_inc_cb);
    uint32_t start = lv_tick_get();
    lv_delay_ms(10);
    TEST_ASSERT_EQUAL(0, test_delay_called);
    TEST_ASSERT_GREATER_OR_EQUAL(10, lv_tick_elaps(start));
}

#endif
