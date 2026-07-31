#if LV_BUILD_TEST
#include "../lvgl.h"
#include "unity/unity.h"

/* STATIC VARIABLES */
static uint32_t test_tick_value = 0;
static uint32_t test_async_cb_count = 0;
static lv_timer_t * test_timer = NULL;

/* STATIC FUNCTIONS */
static uint32_t test_tick_cb(void)
{
    return test_tick_value;
}

/* TEST FUNCTIONS */
void setUp(void)
{
    /* Function run before every test */
    /* Reset test variables */
    test_tick_value = 0;
    test_async_cb_count = 0;

    /* Reset tick module */
    lv_tick_set_cb(NULL);
}

void tearDown(void)
{
    /* Function run after every test */
    if(test_timer) {
        lv_timer_delete(test_timer);
        test_timer = NULL;
    }

    /* Reset tick module */
    lv_tick_set_cb(NULL);
}


static void timer_async_demo_cb(lv_timer_t * t)
{
    TEST_ASSERT_EQUAL(test_timer, t);
    lv_timer_delete(test_timer);
    test_timer = NULL;
    test_async_cb_count++;
}

void test_tickless(void)
{
    lv_tick_set_cb(test_tick_cb);

    const uint32_t MAX_PERIOD = 5;
    uint32_t period = MAX_PERIOD;
    for(unsigned i = 0; i < 100; ++i) {
        if(lv_timer_handler() > period) {
            break;
        }
    }
    TEST_ASSERT_GREATER_THAN(MAX_PERIOD, lv_timer_get_time_to_next());
    test_timer = lv_timer_create(timer_async_demo_cb, period, NULL);
    while(period >= 1) {
        TEST_ASSERT_EQUAL(period, lv_timer_get_time_to_next());
        TEST_ASSERT_EQUAL(period, lv_timer_handler());
        TEST_ASSERT_EQUAL(period, lv_timer_get_time_to_next());
        TEST_ASSERT_EQUAL(0, test_async_cb_count);
        period--;
        test_tick_value++;
    }

    /* It is time to trigger the callback */
    TEST_ASSERT_EQUAL(0, lv_timer_get_time_to_next());
    TEST_ASSERT_EQUAL(0, test_async_cb_count);

    /* Run the callback */
    TEST_ASSERT_GREATER_THAN(MAX_PERIOD, lv_timer_handler());
    TEST_ASSERT_GREATER_THAN(MAX_PERIOD, lv_timer_get_time_to_next());
    TEST_ASSERT_EQUAL(1, test_async_cb_count);
    test_tick_value++;

    /* Run a few more times */
    for(unsigned i = 0; i < 5; ++i) {
        uint32_t lhs = lv_timer_handler();
        uint32_t rhs = lv_timer_get_time_to_next();
        TEST_ASSERT_EQUAL(lhs, rhs);
        test_tick_value++;
    }

    TEST_ASSERT_EQUAL(1, test_async_cb_count);
}

#endif
