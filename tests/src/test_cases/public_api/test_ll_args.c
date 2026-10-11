#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_ll_t test_ll;

#define NODE_SIZE sizeof(int32_t)

void setUp(void)
{
    lv_ll_init(&test_ll, NODE_SIZE);
}

void tearDown(void)
{
    lv_ll_clear(&test_ll);
}

void test_ll_args_is_empty_null_list(void)
{
    TEST_ASSERT_TRUE(lv_ll_is_empty(NULL));
}

void test_ll_args_null_list_handling(void)
{
    TEST_ASSERT_NULL(lv_ll_ins_prev(NULL, NULL));
    TEST_ASSERT_NULL(lv_ll_ins_prev(&test_ll, NULL));

    lv_ll_remove(NULL, NULL);

    TEST_ASSERT_NULL(lv_ll_get_head(NULL));
    TEST_ASSERT_NULL(lv_ll_get_tail(NULL));
}

#endif /*LV_BUILD_TEST*/
