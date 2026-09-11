#if LV_BUILD_TEST

#include "unity/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/* Test lv_palette_main with invalid palette */
void test_palette_main_invalid(void)
{
    lv_color_t color = lv_palette_main(LV_PALETTE_LAST);
    TEST_ASSERT_EQUAL_COLOR(lv_color_black(), color);

    color = lv_palette_main(LV_PALETTE_NONE);
    TEST_ASSERT_EQUAL_COLOR(lv_color_black(), color);
}

/* Test lv_palette_main with out-of-range palette index */
void test_palette_main_out_of_range(void)
{
    lv_color_t color = lv_palette_main(LV_PALETTE_LAST + 1);
    TEST_ASSERT_EQUAL_COLOR(lv_color_black(), color);
}

/* Test lv_palette_lighten with invalid levels */
void test_palette_lighten_invalid(void)
{
    lv_color_t color = lv_palette_lighten(LV_PALETTE_RED, 0);
    TEST_ASSERT_EQUAL_COLOR(lv_color_black(), color);

    color = lv_palette_lighten(LV_PALETTE_RED, 6);
    TEST_ASSERT_EQUAL_COLOR(lv_color_black(), color);
}

/* Test lv_palette_lighten with out-of-range palette index */
void test_palette_lighten_out_of_range(void)
{
    lv_color_t color = lv_palette_lighten(LV_PALETTE_LAST + 1, 1);
    TEST_ASSERT_EQUAL_COLOR(lv_color_black(), color);
}

/* Test lv_palette_darken with invalid levels */
void test_palette_darken_invalid(void)
{
    lv_color_t color = lv_palette_darken(LV_PALETTE_RED, 0);
    TEST_ASSERT_EQUAL_COLOR(lv_color_black(), color);

    color = lv_palette_darken(LV_PALETTE_RED, 5);
    TEST_ASSERT_EQUAL_COLOR(lv_color_black(), color);
}

/* Test lv_palette_darken with out-of-range palette index */
void test_palette_darken_out_of_range(void)
{
    lv_color_t color = lv_palette_darken(LV_PALETTE_LAST + 1, 1);
    TEST_ASSERT_EQUAL_COLOR(lv_color_black(), color);
}

#endif /*LV_BUILD_TEST*/
