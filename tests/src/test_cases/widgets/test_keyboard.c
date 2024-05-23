#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    active_screen = lv_screen_active();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_keyboard_mode(void)
{
    lv_obj_t * keyboard  = lv_keyboard_create(active_screen);
    lv_obj_set_size(keyboard, LV_PCT(100), LV_PCT(50));
    lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_text_font(keyboard, &lv_font_dejavu_16_persian_hebrew, LV_PART_MAIN);
    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_TEXT_LOWER);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/keyboard_1.png");

    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_TEXT_UPPER);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/keyboard_2.png");

    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_SPECIAL);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/keyboard_3.png");

    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_NUMBER);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/keyboard_4.png");
}

#endif
