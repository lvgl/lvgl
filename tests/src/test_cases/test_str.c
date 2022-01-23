#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

#include "../lvgl/src/misc/lv_str.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}

void test_lv_strncpy_should_null_terminate_dst_when_count_is_smaller_than_src_len(void)
{
    const char *src = "Hello world";
    char dst[6] = {0, 1, 2, 3, 4, 5};
    size_t count = 5;

    lv_strncpy(dst, src, count);

    TEST_ASSERT_EQUAL_STRING("Hello", dst);
    TEST_ASSERT_EQUAL_INT(count, strlen(dst));
    /* Last character of dst gets NULL terminated */
    TEST_ASSERT_EQUAL_UINT8('\0', dst[count]);
}

#endif
