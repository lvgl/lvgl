#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_array_t array;

void setUp(void)
{
    LV_ARRAY_INIT(&array, int32_t);
}

void tearDown(void)
{
    lv_array_destroy(&array);
}

void test_array_append_values(void)
{
    for(int32_t i = 0; i < 10; i++) {
        LV_ARRAY_APPEND_VALUE(&array, i);
    }

    TEST_ASSERT_EQUAL_UINT32(8, lv_array_length(&array));
}

void test_array_set_get(void)
{
    uint32_t v = 100;
    LV_ARRAY_SET(&array, 0, &v, uint32_t);
    uint32_t * r = LV_ARRAY_GET(&array, 0, uint32_t);
    TEST_ASSERT_EQUAL_UINT32(100, *r);
}

void test_array_len(void)
{
    for(int32_t i = 0; i < 10; i++) {
        LV_ARRAY_APPEND_VALUE(&array, i);
    }

    TEST_ASSERT_EQUAL_UINT32(1, lv_array_is_full(&array) ? 1 : 0);
    TEST_ASSERT_EQUAL_UINT32(8, lv_array_length(&array));
    lv_array_clear(&array);
    TEST_ASSERT_EQUAL_UINT32(1, lv_array_is_empty(&array) ? 1 : 0);
    TEST_ASSERT_EQUAL_UINT32(0, lv_array_length(&array));
}

void test_array_resize(void)
{
    for(int32_t i = 0; i < 10; i++) {
        LV_ARRAY_APPEND_VALUE(&array, i);
    }

    lv_array_resize(&array, 12);
    TEST_ASSERT_EQUAL_UINT32(8, lv_array_length(&array));
    lv_array_resize(&array, 6);
    TEST_ASSERT_EQUAL_UINT32(6, lv_array_length(&array));
}

void test_array_copy(void)
{
    for(int32_t i = 0; i < 10; i++) {
        LV_ARRAY_APPEND_VALUE(&array, i);
    }

    lv_array_t array2;
    lv_memset(&array2, 0, sizeof(lv_array_t));
    TEST_ASSERT_EQUAL_UINT32(8, lv_array_length(&array));
    lv_array_copy(&array2, &array);
    TEST_ASSERT_EQUAL_UINT32(8, lv_array_length(&array2));
    uint32_t * r = LV_ARRAY_GET(&array2, 1, uint32_t);
    TEST_ASSERT_EQUAL_UINT32(1, *r);
    lv_array_destroy(&array2);
}

#endif
