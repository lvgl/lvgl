#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static lv_circle_buf_t * circle_buf;

#define circle_buf_CAPACITY 4

void setUp(void)
{
    circle_buf = lv_circle_buf_create(circle_buf_CAPACITY, sizeof(int32_t));

    TEST_ASSERT_EQUAL_UINT32(lv_circle_buf_capacity(circle_buf), circle_buf_CAPACITY);
    TEST_ASSERT_EQUAL_UINT32(0, lv_circle_buf_size(circle_buf));

    /**
     * Write values to the circle buffer. The max size of the buffer is circle_buf_CAPACITY.
     * When the buffer is full, the write operation should return LV_RESULT_INVALID.
     */
    for(int32_t i = 0; i < circle_buf_CAPACITY * 2; i++) {
        const lv_result_t res = lv_circle_buf_write(circle_buf, &i);

        if(i < circle_buf_CAPACITY) TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        else TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);
    }

    /**
     * After writing values to the buffer, the size of the buffer should be equal to the capacity.
     */
    TEST_ASSERT_EQUAL_UINT32(lv_circle_buf_size(circle_buf), circle_buf_CAPACITY);
}

void tearDown(void)
{
    lv_circle_buf_destroy(circle_buf);
    circle_buf = NULL;
}

void test_circle_buf_read_write_peek_values(void)
{
    /**
     * Read 1 value from the buffer.
     */
    {
        int32_t value;
        const lv_result_t res = lv_circle_buf_read(circle_buf, &value);

        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        TEST_ASSERT_EQUAL_INT32(0, value);
    }

    /**
     * Peek values will not advance the read and write cursors.
     * If the peek index is greater than the size of the buffer, it will returns looply.
     */
    for(int32_t i = 0, j = 1; i < circle_buf_CAPACITY * 10; i++, j++) {
        int32_t value;
        const lv_result_t res = lv_circle_buf_peek_at(circle_buf, i, &value);

        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        TEST_ASSERT_EQUAL_INT32(j, value);

        if(j == 3) j = 0;
    }

    /**
     * Read values from the circle buffer. The max size of the buffer is circle_buf_CAPACITY.
     * When the buffer is empty, the read operation should return LV_RESULT_INVALID.
     */
    for(int32_t i = 1; i < circle_buf_CAPACITY * 2; i++) {
        int32_t value;
        const lv_result_t res = lv_circle_buf_read(circle_buf, &value);

        if(i < circle_buf_CAPACITY) {
            TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
            TEST_ASSERT_EQUAL_INT32(i, value);
        }
        else {
            TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);
        }
    }

    /**
     * After reading values from the buffer, the size of the buffer should be equal to 0.
     */
    TEST_ASSERT_EQUAL_INT32(0, lv_circle_buf_size(circle_buf));
}

void test_circle_buf_skip_values(void)
{
    /**
     * Skip 1 value from the buffer.
     */
    {
        const lv_result_t res = lv_circle_buf_skip(circle_buf);

        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    }

    /**
     * Skip values from the circle buffer. The max size of the buffer is circle_buf_CAPACITY.
     * When the buffer is empty, the skip operation should return LV_RESULT_INVALID.
     */
    for(int32_t i = 1; i < circle_buf_CAPACITY * 2; i++) {
        const lv_result_t res = lv_circle_buf_skip(circle_buf);

        if(i < circle_buf_CAPACITY) {
            TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        }
        else {
            TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);
        }
    }

    /**
     * After skipping values from the buffer, the size of the buffer should be equal to 0.
     */
    TEST_ASSERT_EQUAL_INT32(0, lv_circle_buf_size(circle_buf));
}

void test_circle_buf_read_after_read_and_write(void)
{
    /**
     * Read 1 value from the buffer.
     */
    {
        int32_t value;
        const lv_result_t res = lv_circle_buf_read(circle_buf, &value);

        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        TEST_ASSERT_EQUAL_INT32(0, value);
    }

    /**
     * Write 1 value to the buffer.
     */
    {
        const int32_t value = 4;
        const lv_result_t res = lv_circle_buf_write(circle_buf, &value);

        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    }

    const int32_t expected[] = {4, 1, 2, 3};
    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, ((lv_array_t *)circle_buf)->data, 4);

    /**
     * Read values from the circle buffer. The max size of the buffer is circle_buf_CAPACITY.
     * When the buffer is empty, the read operation should return LV_RESULT_INVALID.
     */
    for(int32_t i = 1; i < circle_buf_CAPACITY * 2; i++) {
        int32_t value;
        const lv_result_t res = lv_circle_buf_read(circle_buf, &value);

        if(i <= circle_buf_CAPACITY) {
            TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
            TEST_ASSERT_EQUAL_INT32(i, value);
        }
        else {
            TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);
        }
    }
}

#endif
