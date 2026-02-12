#if LV_BUILD_TEST
#include "../lvgl.h"
#include "unity/unity.h"

static void * test_cb_data = NULL;
static void * test_user_data = (void *)0x11223344;

static void test_async_cb(void * user_data)
{
    test_cb_data = user_data;
}

void setUp(void)
{
    lv_async_call_cancel(test_async_cb, test_user_data);
}

void tearDown(void)
{
    lv_async_call_cancel(test_async_cb, test_user_data);
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

#endif
