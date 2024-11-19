#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

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

void test_keyboard_properties(void)
{
#if LV_USE_OBJ_PROPERTY
    lv_obj_t * obj = lv_keyboard_create(lv_screen_active());
    lv_property_t prop = { };

    lv_obj_t * test_area = lv_textarea_create(lv_screen_active());

    prop.id = LV_PROPERTY_KEYBOARD_TEXTAREA;
    prop.ptr = test_area;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_PTR(test_area, lv_keyboard_get_textarea(obj));
    TEST_ASSERT_EQUAL_PTR(test_area, lv_obj_get_property(obj, LV_PROPERTY_KEYBOARD_TEXTAREA).ptr);

    prop.id = LV_PROPERTY_KEYBOARD_MODE;
    prop.num = LV_KEYBOARD_MODE_TEXT_UPPER;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(LV_KEYBOARD_MODE_TEXT_UPPER, lv_keyboard_get_mode(obj));
    TEST_ASSERT_EQUAL_INT(LV_KEYBOARD_MODE_TEXT_UPPER, lv_obj_get_property(obj, LV_PROPERTY_KEYBOARD_MODE).num);

    prop.id = LV_PROPERTY_KEYBOARD_POPOVERS;
    prop.num = 1;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(1, lv_keyboard_get_popovers(obj));
    TEST_ASSERT_EQUAL_INT(1, lv_obj_get_property(obj, LV_PROPERTY_KEYBOARD_POPOVERS).num);

    prop.id = LV_PROPERTY_KEYBOARD_SELECTED_BUTTON;
    prop.num = 1;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(1, lv_keyboard_get_selected_button(obj));
    TEST_ASSERT_EQUAL_INT(1, lv_obj_get_property(obj, LV_PROPERTY_KEYBOARD_SELECTED_BUTTON).num);
#endif
}

#endif
