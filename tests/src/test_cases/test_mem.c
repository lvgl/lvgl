#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

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
#ifdef LVGL_CI_USING_DEF_HEAP
    void * buf1 = lv_malloc(20);
    void * buf2 = lv_realloc(buf1, LV_MEM_SIZE + 16384);
    TEST_ASSERT_NULL(buf2);
#endif
}

#endif
