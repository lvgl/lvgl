#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

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

    TEST_ASSERT_EQUAL_UINT32(0, lv_array_is_full(&array) ? 1 : 0);
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
    TEST_ASSERT_EQUAL_UINT32(LV_ARRAY_DEFAULT_CAPACITY, lv_array_size(&array));
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

void test_array_init_from_buf(void)
{
    int32_t buf[4] = { 0 };
    lv_array_t a;
    lv_array_init_from_buf(&a, buf, 4, sizeof(int32_t));

    TEST_ASSERT_FALSE(lv_array_resize(&a, 8));

    lv_array_t b;
    lv_array_init(&b, 4, sizeof(int32_t));
    lv_array_push_back(&b, NULL);

    TEST_ASSERT_EQUAL_UINT32(4, lv_array_capacity(&a));

    for(int32_t i = 0; i < 4; i++) {
        lv_array_push_back(&a, &i);
    }

    /* Test overflow handling, should fail */
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_array_push_back(&a, NULL));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_array_concat(&a, &b));

    TEST_ASSERT_EQUAL_UINT32(4, lv_array_size(&a));
    for(int32_t i = 0; i < 4; i++) {
        int32_t * v = lv_array_at(&a, i);
        TEST_ASSERT_EQUAL_INT32(i, *v);
    }

    TEST_ASSERT_EQUAL_PTR(buf, lv_array_front(&a));
    TEST_ASSERT_EQUAL_PTR(buf + 3, lv_array_back(&a));

    lv_array_deinit(&a);
    lv_array_deinit(&b);
}

void test_array_shrink(void)
{
    for(int32_t i = 0; i < 10; i++) {
        lv_array_push_back(&array, &i);
    }
    TEST_ASSERT_EQUAL_UINT32(10, lv_array_size(&array));

    lv_array_resize(&array, 20);
    TEST_ASSERT_EQUAL_UINT32(20, lv_array_capacity(&array));

    lv_array_shrink(&array);
    TEST_ASSERT_EQUAL_UINT32(10, lv_array_capacity(&array));

    /* Double shrink should not shrink more */
    lv_array_shrink(&array);
    TEST_ASSERT_EQUAL_UINT32(10, lv_array_capacity(&array));
}

void test_array_remove(void)
{
    /* NULL array is handled properly */
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_array_remove(NULL, 0));

    for(int32_t i = 0; i < 5; i++) {
        lv_array_push_back(&array, &i);
    }
    TEST_ASSERT_EQUAL_UINT32(5, lv_array_size(&array));

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_array_remove(&array, 4));
    TEST_ASSERT_EQUAL_UINT32(4, lv_array_size(&array));

    /* Test remove out of range, should fail */
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_array_remove(&array, 4));
    TEST_ASSERT_EQUAL_UINT32(4, lv_array_size(&array));

    /* remove the last element */
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_array_remove(&array, 3));
    TEST_ASSERT_EQUAL_UINT32(3, lv_array_size(&array));

    /* remove the first element */
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_array_remove(&array, 0));
    TEST_ASSERT_EQUAL_UINT32(2, lv_array_size(&array));

    /* verify the content */
    for(int32_t i = 0; i < 2; i++) {
        int32_t * v = lv_array_at(&array, i);
        TEST_ASSERT_EQUAL_INT32(i + 1, *v);
    }
}

void test_array_remove_unordered(void)
{
    /* NULL array is handled properly */
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_array_remove_unordered(NULL, 0));

    for(int32_t i = 0; i < 5; i++) {
        lv_array_push_back(&array, &i);
    }
    TEST_ASSERT_EQUAL_UINT32(5, lv_array_size(&array));

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_array_remove_unordered(&array, 4));
    TEST_ASSERT_EQUAL_UINT32(4, lv_array_size(&array));

    /* Test remove out of range, should fail */
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_array_remove_unordered(&array, 4));
    TEST_ASSERT_EQUAL_UINT32(4, lv_array_size(&array));

    /* remove the last element */
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_array_remove_unordered(&array, 3));
    TEST_ASSERT_EQUAL_UINT32(3, lv_array_size(&array));

    /* remove the first element */
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_array_remove_unordered(&array, 0));
    TEST_ASSERT_EQUAL_UINT32(2, lv_array_size(&array));

    int32_t * v0 = lv_array_at(&array, 0);
    int32_t * v1 = lv_array_at(&array, 1);

    /* Removing the first element should have moved the last element
     * to the first position*/
    TEST_ASSERT_EQUAL_INT32(2, *v0);
    TEST_ASSERT_EQUAL_INT32(1, *v1);
}

void test_array_erase(void)
{
    /* Test overlapping memory regions */
    for(int32_t i = 0; i < 5; i++) {
        lv_array_push_back(&array, &i);
    }

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_array_erase(&array, 0, 1));
    TEST_ASSERT_EQUAL_UINT32(4, lv_array_size(&array));
    for(int32_t i = 0; i < 4; i++) {
        int32_t * v = lv_array_at(&array, i);
        TEST_ASSERT_EQUAL_INT32(i + 1, *v);
    }

    for(int32_t i = 0; i < 10; i++) {
        lv_array_push_back(&array, &i);
    }

    /* Test erase from the middle */
    lv_array_clear(&array);
    for(int32_t i = 0; i < 10; i++) {
        lv_array_push_back(&array, &i);
    }

    TEST_ASSERT_EQUAL_UINT32(10, lv_array_size(&array));
    lv_array_erase(&array, 3, 7);
    TEST_ASSERT_EQUAL_UINT32(6, lv_array_size(&array));
    for(int32_t i = 0; i < 6; i++) {
        int32_t * v = lv_array_at(&array, i);
        if(i < 3) {
            TEST_ASSERT_EQUAL_INT32(i, *v);
        }
        else {
            TEST_ASSERT_EQUAL_INT32(i + 4, *v);
        }
    }

    /* Test edge cases */
    lv_array_clear(&array);
    for(int32_t i = 0; i < 10; i++) {
        lv_array_push_back(&array, &i);
    }

    /* end > array->size */
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_array_erase(&array, 3, 15));
    TEST_ASSERT_EQUAL_UINT32(3, lv_array_size(&array));
    for(int32_t i = 0; i < 3; i++) {
        int32_t * v = lv_array_at(&array, i);
        TEST_ASSERT_EQUAL_INT32(i, *v);
    }

    /* Reset array */
    lv_array_clear(&array);
    for(int32_t i = 0; i < 10; i++) {
        lv_array_push_back(&array, &i);
    }

    /* start >= end */
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_array_erase(&array, 5, 5));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_array_erase(&array, 7, 6));

    /* end == array->size */
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_array_erase(&array, 5, 10));
    TEST_ASSERT_EQUAL_UINT32(5, lv_array_size(&array));
    for(int32_t i = 0; i < 5; i++) {
        int32_t * v = lv_array_at(&array, i);
        TEST_ASSERT_EQUAL_INT32(i, *v);
    }
}

void test_array_assign(void)
{
    for(int32_t i = 0; i < 5; i++) {
        lv_array_push_back(&array, &i);
    }

    int32_t v = 100;
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_array_assign(&array, 2, &v));

    int32_t * r = lv_array_at(&array, 2);
    TEST_ASSERT_EQUAL_INT32(100, *r);

    /* Test out of range, should fail */
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_array_assign(&array, 5, &v));
}

#endif
