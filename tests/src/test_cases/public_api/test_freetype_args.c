#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

#if LV_USE_FREETYPE

void setUp(void)
{
}

void tearDown(void)
{
}

void test_freetype_args_font_create_null_pathname(void)
{
    TEST_ASSERT_NULL(lv_freetype_font_create(NULL, LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 24,
                                             LV_FREETYPE_FONT_STYLE_NORMAL));
    TEST_ASSERT_NULL(lv_freetype_font_create(NULL, LV_FREETYPE_FONT_RENDER_MODE_OUTLINE, 24,
                                             LV_FREETYPE_FONT_STYLE_NORMAL));
}

#else /*LV_USE_FREETYPE*/

void setUp(void) { }
void tearDown(void) { }
void test_freetype_args_font_create_null_pathname(void) { }

#endif /*LV_USE_FREETYPE*/
#endif /*LV_BUILD_TEST*/
