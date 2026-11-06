#if LV_BUILD_TEST
#include "../lvgl.h"
#include "lv_test_init.h"
#include "unity/unity.h"

void test_draw_sw_thread_shutdown(void);

void test_draw_sw_thread_shutdown(void)
{
#if LV_USE_OS != LV_OS_NONE && LV_USE_DRAW_SW
    for(unsigned iteration = 0; iteration < 64; iteration++) {
        lv_test_deinit();
        lv_test_init();
        TEST_ASSERT_TRUE(lv_is_initialized());
    }
#else
    TEST_IGNORE_MESSAGE("Requires an OS and the software renderer");
#endif
}
#endif
