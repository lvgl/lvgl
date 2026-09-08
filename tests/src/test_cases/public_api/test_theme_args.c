#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_theme_args_set_user_data_null_theme(void)
{
    int dummy = 42;

    /*No theme to store it on, so the call is a no-op rather than a crash*/
    lv_theme_set_user_data(NULL, &dummy);
    TEST_PASS();
}

#endif /*LV_BUILD_TEST*/
