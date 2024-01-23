#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_array_t array;

void setUp(void)
{
    lv_array_init(&array, LV_ARRAY_DEFAULT_CAPACITY, sizeof(int32_t));
}

void tearDown(void)
{
    lv_array_deinit(&array);
}

void test_array_append_values(void)
{
    const int32_t new_size = LV_ARRAY_DEFAULT_CAPACITY + 2;
    TEST_ASSERT_EQUAL_UINT32(0, lv_array_size(&array));
    for(int32_t i = 0; i < new_size; i++) {
        lv_array_push_back(&array, &i);
    }
    /*push back will automatically extent the array size.*/
    TEST_ASSERT_EQUAL_UINT32(new_size, lv_array_size(&array));
}

void test_array_set_get(void)
{
    int32_t v = 100;
    lv_array_push_back(&array, &v);
    int32_t * r = lv_array_at(&array, 0);
    TEST_ASSERT_EQUAL_UINT32(100, *r);
}

void test_array_size(void)
{
    for(int32_t i = 0; i < 10; i++) {
        lv_array_push_back(&array, &i);
    }

    TEST_ASSERT_EQUAL_UINT32(1, lv_array_is_full(&array) ? 1 : 0);
    lv_array_clear(&array);
    TEST_ASSERT_EQUAL_UINT32(1, lv_array_is_empty(&array) ? 1 : 0);
    TEST_ASSERT_EQUAL_UINT32(0, lv_array_size(&array));
}

void test_array_resize(void)
{
    for(int32_t i = 0; i < LV_ARRAY_DEFAULT_CAPACITY; i++) {
        lv_array_push_back(&array, &i);
    }

    TEST_ASSERT_EQUAL_UINT32(LV_ARRAY_DEFAULT_CAPACITY, lv_array_size(&array));
    lv_array_resize(&array, 12);
    TEST_ASSERT_EQUAL_UINT32(LV_ARRAY_DEFAULT_CAPACITY, lv_array_size(&array));
    TEST_ASSERT_EQUAL_UINT32(12, lv_array_capacity(&array));
    lv_array_resize(&array, 6);
    TEST_ASSERT_EQUAL_UINT32(6, lv_array_size(&array));
    TEST_ASSERT_EQUAL_UINT32(6, lv_array_capacity(&array));
}

void test_array_copy(void)
{
    for(int32_t i = 0; i < LV_ARRAY_DEFAULT_CAPACITY; i++) {
        lv_array_push_back(&array, &i);
    }

    uint32_t array_size = lv_array_size(&array);
    lv_array_t array2 = { 0 };
    lv_array_copy(&array2, &array);
    TEST_ASSERT_EQUAL_UINT32(array_size, lv_array_size(&array2));

    uint32_t * r = lv_array_at(&array2, 1);
    TEST_ASSERT_EQUAL_UINT32(1, *r);
    lv_array_deinit(&array2);
}

void test_array_concat(void)
{
    lv_array_t a, b;
    lv_array_init(&a, 4, sizeof(int32_t));
    lv_array_init(&b, 4, sizeof(int32_t));
    for(int32_t i = 0; i < 4; i++) {
        lv_array_push_back(&a, &i);
        lv_array_push_back(&b, &i);
    }

    TEST_ASSERT_TRUE(lv_array_concat(&a, &b) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_UINT32(8, lv_array_size(&a));

    for(int32_t i = 0; i < 8; i++) {
        int32_t * v = lv_array_at(&a, i);
        TEST_ASSERT_EQUAL_INT32(i % 4, *v);
    }

    lv_array_deinit(&a);
    lv_array_deinit(&b);
}

#endif
