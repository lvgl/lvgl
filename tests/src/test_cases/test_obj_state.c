#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void test_obj_state_setters_and_getters(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());

    lv_obj_set_alt(obj, true);
    TEST_ASSERT_TRUE(lv_obj_is_alt(obj));
    lv_obj_set_alt(obj, false);
    TEST_ASSERT_FALSE(lv_obj_is_alt(obj));

    lv_obj_set_checked(obj, true);
    TEST_ASSERT_TRUE(lv_obj_is_checked(obj));
    lv_obj_set_checked(obj, false);
    TEST_ASSERT_FALSE(lv_obj_is_checked(obj));

    lv_obj_set_focused(obj, true);
    TEST_ASSERT_TRUE(lv_obj_is_focused(obj));
    lv_obj_set_focused(obj, false);
    TEST_ASSERT_FALSE(lv_obj_is_focused(obj));

    lv_obj_set_focus_key(obj, true);
    TEST_ASSERT_TRUE(lv_obj_is_focus_key(obj));
    lv_obj_set_focus_key(obj, false);
    TEST_ASSERT_FALSE(lv_obj_is_focus_key(obj));

    lv_obj_set_edited(obj, true);
    TEST_ASSERT_TRUE(lv_obj_is_edited(obj));
    lv_obj_set_edited(obj, false);
    TEST_ASSERT_FALSE(lv_obj_is_edited(obj));

    lv_obj_set_hovered(obj, true);
    TEST_ASSERT_TRUE(lv_obj_is_hovered(obj));
    lv_obj_set_hovered(obj, false);
    TEST_ASSERT_FALSE(lv_obj_is_hovered(obj));

    lv_obj_set_pressed(obj, true);
    TEST_ASSERT_TRUE(lv_obj_is_pressed(obj));
    lv_obj_set_pressed(obj, false);
    TEST_ASSERT_FALSE(lv_obj_is_pressed(obj));

    lv_obj_set_scrolled(obj, true);
    TEST_ASSERT_TRUE(lv_obj_is_scrolled(obj));
    lv_obj_set_scrolled(obj, false);
    TEST_ASSERT_FALSE(lv_obj_is_scrolled(obj));

    lv_obj_set_disabled(obj, true);
    TEST_ASSERT_TRUE(lv_obj_is_disabled(obj));
    lv_obj_set_disabled(obj, false);
    TEST_ASSERT_FALSE(lv_obj_is_disabled(obj));

    lv_obj_set_state_user_1(obj, true);
    TEST_ASSERT_TRUE(lv_obj_is_state_user_1(obj));
    lv_obj_set_state_user_1(obj, false);
    TEST_ASSERT_FALSE(lv_obj_is_state_user_1(obj));

    lv_obj_set_state_user_2(obj, true);
    TEST_ASSERT_TRUE(lv_obj_is_state_user_2(obj));
    lv_obj_set_state_user_2(obj, false);
    TEST_ASSERT_FALSE(lv_obj_is_state_user_2(obj));

    lv_obj_set_state_user_3(obj, true);
    TEST_ASSERT_TRUE(lv_obj_is_state_user_3(obj));
    lv_obj_set_state_user_3(obj, false);
    TEST_ASSERT_FALSE(lv_obj_is_state_user_3(obj));

    lv_obj_set_state_user_4(obj, true);
    TEST_ASSERT_TRUE(lv_obj_is_state_user_4(obj));
    lv_obj_set_state_user_4(obj, false);
    TEST_ASSERT_FALSE(lv_obj_is_state_user_4(obj));

    /*Setting one state should leave the others untouched*/
    lv_obj_set_checked(obj, true);
    lv_obj_set_disabled(obj, true);
    TEST_ASSERT_TRUE(lv_obj_is_checked(obj));
    TEST_ASSERT_TRUE(lv_obj_is_disabled(obj));
    lv_obj_set_checked(obj, false);
    TEST_ASSERT_FALSE(lv_obj_is_checked(obj));
    TEST_ASSERT_TRUE(lv_obj_is_disabled(obj));

    lv_obj_delete(obj);
}

#endif
