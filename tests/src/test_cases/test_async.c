#if LV_BUILD_TEST
#include "../lvgl.h"
#include "unity/unity.h"

static void * test_cb_data = NULL;
static void * test_user_data = (void *)0x11223344;
static unsigned int async_calls;
static lv_result_t schedule_result;

static void test_async_cb(void * user_data)
{
    test_cb_data = user_data;
}

static void reschedule_async_cb(void * user_data)
{
    async_calls++;
    /* Bound the chain so this test fails without hanging on older code. */
    if(async_calls < 3) schedule_result = lv_async_call(reschedule_async_cb, user_data);
}

static void schedule_from_timer(lv_timer_t * timer)
{
    LV_UNUSED(timer);
    schedule_result = lv_async_call(test_async_cb, test_user_data);
}

void setUp(void)
{
    lv_async_call_cancel(test_async_cb, test_user_data);
}

void tearDown(void)
{
    lv_async_call_cancel(test_async_cb, test_user_data);
    lv_async_call_cancel(reschedule_async_cb, NULL);
    test_cb_data = NULL;
}

void test_async_call_cancel_match(void)
{
    TEST_ASSERT_EQUAL_INT32(LV_RESULT_OK, lv_async_call(test_async_cb, test_user_data));
    TEST_ASSERT_EQUAL_INT32(LV_RESULT_OK, lv_async_call_cancel(test_async_cb, test_user_data));
    TEST_ASSERT_EQUAL_INT32(LV_RESULT_INVALID, lv_async_call_cancel(test_async_cb, test_user_data));
}

void test_async_call_basic_functionality(void)
{
    test_cb_data = NULL;
    TEST_ASSERT_EQUAL_INT32(LV_RESULT_OK, lv_async_call(test_async_cb, test_user_data));
    lv_timer_handler();
    TEST_ASSERT_EQUAL(test_user_data, test_cb_data);
}

void test_async_call_reschedules_on_the_next_handler(void)
{
    async_calls = 0;
    schedule_result = LV_RESULT_INVALID;
    TEST_ASSERT_EQUAL_INT32(LV_RESULT_OK, lv_async_call(reschedule_async_cb, NULL));

    for(unsigned int count = 1; count <= 3; count++) {
        uint32_t time_to_next = lv_timer_handler();
        TEST_ASSERT_EQUAL_UINT(count, async_calls);
        TEST_ASSERT_EQUAL_INT32(LV_RESULT_OK, schedule_result);
        if(count < 3) TEST_ASSERT_EQUAL_UINT32(0, time_to_next);
    }
    lv_timer_handler();
    TEST_ASSERT_EQUAL_UINT(3, async_calls);
}

void test_async_call_from_timer_waits_for_the_next_handler(void)
{
    schedule_result = LV_RESULT_INVALID;
    lv_timer_t * timer = lv_timer_create(schedule_from_timer, 0, NULL);
    TEST_ASSERT_NOT_NULL(timer);
    lv_timer_set_repeat_count(timer, 1);

    lv_timer_handler();
    TEST_ASSERT_EQUAL_INT32(LV_RESULT_OK, schedule_result);
    TEST_ASSERT_NULL(test_cb_data);

    lv_timer_handler();
    TEST_ASSERT_EQUAL_PTR(test_user_data, test_cb_data);
}

void test_async_call_can_cancel_deferred_callback(void)
{
    async_calls = 0;
    schedule_result = LV_RESULT_INVALID;
    TEST_ASSERT_EQUAL_INT32(LV_RESULT_OK, lv_async_call(reschedule_async_cb, NULL));
    lv_timer_handler();

    TEST_ASSERT_EQUAL_INT32(LV_RESULT_OK, schedule_result);
    TEST_ASSERT_EQUAL_INT32(LV_RESULT_OK, lv_async_call_cancel(reschedule_async_cb, NULL));
    lv_timer_handler();
    TEST_ASSERT_EQUAL_UINT(1, async_calls);
}

#endif
