#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include "../demos/lv_demos.h"

#ifndef NON_AMD64_BUILD
    #define EXT_NAME ".lp64.png"
#else
    #define EXT_NAME ".lp32.png"
#endif

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

void test_demo_vector_graphic(void)
{
#if !LV_USE_VECTOR_GRAPHIC
    TEST_IGNORE_MESSAGE("LV_USE_VECTOR_GRAPHIC is disabled");
#endif
    lv_demo_vector_graphic_buffered();
    TEST_ASSERT_EQUAL_SCREENSHOT("demo_vector_graphic_buffered" EXT_NAME);

    lv_obj_clean(lv_screen_active());

    lv_demo_vector_graphic_not_buffered();
    TEST_ASSERT_EQUAL_SCREENSHOT("demo_vector_graphic_not_buffered" EXT_NAME);
}

#endif
