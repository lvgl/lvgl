#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}

/* #3324 */
void test_mem_buf_realloc(void)
{
#if LV_MEM_CUSTOM == 0
    void * buf1 = lv_mem_alloc(20);
    void * buf2 = lv_mem_realloc(buf1, LV_MEM_SIZE + 16384);
    TEST_ASSERT_NULL(buf2);
#endif
}

#endif
