#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include "lv_test_indev.h"

void test_screen_load_no_crash(void)
{
    /*load new screen should not crash*/
    lv_obj_t * screen = lv_obj_create(NULL);
    lv_scr_load(screen);

    /*Consecutively loading multiple screens with transition animations should not crash*/
    lv_obj_t * screen_with_anim_1 = lv_obj_create(NULL);
    lv_obj_t * screen_with_anim_2 = lv_obj_create(NULL);
    lv_scr_load_anim(screen_with_anim_1, LV_SCR_LOAD_ANIM_OVER_LEFT, 2000, 0, false);
    lv_scr_load_anim(screen_with_anim_2, LV_SCR_LOAD_ANIM_OVER_RIGHT, 1000, 500, false);
}

void test_screen_load_with_delete_no_crash(void)
{
    /*load new screen should not crash*/
    lv_obj_t * screen = lv_obj_create(NULL);
    lv_scr_load(screen);

    /*Consecutively loading multiple screens (while deleting one) with transition animations should not crash*/
    lv_obj_t * screen_with_anim_1 = lv_obj_create(NULL);
    lv_obj_t * screen_with_anim_2 = lv_obj_create(NULL);
    lv_obj_t * screen_with_anim_3 = lv_obj_create(NULL);

    lv_scr_load_anim(screen_with_anim_1, LV_SCR_LOAD_ANIM_OVER_LEFT, 0, 0, false);
    lv_scr_load_anim(screen_with_anim_2, LV_SCR_LOAD_ANIM_OVER_RIGHT, 1000, 0, true);

    /*Wait to trigger the animation start callbacks*/
    lv_test_indev_wait(100);

    lv_scr_load_anim(screen_with_anim_3, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 0, true);

    /*The active screen should be immediately replaced*/
    TEST_ASSERT_EQUAL(lv_scr_act(), screen_with_anim_2);

    lv_test_indev_wait(400);

    /*Check for the screens status after the transition*/
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_1), false);
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_2), false);
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_3), true);
}

void test_screen_load_with_delete_no_crash2(void)
{
    /*load new screen should not crash*/
    lv_obj_t * screen = lv_obj_create(NULL);
    lv_scr_load(screen);

    /*Consecutively loading multiple screens (while deleting one) with transition animations should not crash*/
    lv_obj_t * screen_with_anim_1 = lv_obj_create(NULL);
    lv_obj_t * screen_with_anim_2 = lv_obj_create(NULL);
    lv_obj_t * screen_with_anim_3 = lv_obj_create(NULL);
    lv_obj_t * screen_with_anim_4 = lv_obj_create(NULL);

    lv_scr_load_anim(screen_with_anim_1, LV_SCR_LOAD_ANIM_OVER_LEFT, 0, 0, false);
    lv_scr_load_anim(screen_with_anim_2, LV_SCR_LOAD_ANIM_OVER_RIGHT, 1000, 0, true);
    lv_scr_load_anim(screen_with_anim_3, LV_SCR_LOAD_ANIM_OVER_LEFT, 0, 0, true);

    /*Wait to trigger the animation start callbacks*/
    lv_test_indev_wait(100);

    lv_scr_load_anim(screen_with_anim_4, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 0, true);

    /*The active screen should be immediately replaced*/
    TEST_ASSERT_EQUAL(lv_scr_act(), screen_with_anim_3);

    lv_test_indev_wait(400);

    /*Check for the screens status after the transition*/
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_1), false);
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_2), false);
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_3), false);
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_4), true);
}

static bool screen_1_unloaded_called = false;

static void screen_with_anim_1_unloaded_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    screen_1_unloaded_called = true;
}

void test_screen_load_with_delete_event(void)
{
    /*load new screen should not crash*/
    lv_obj_t * screen = lv_obj_create(NULL);
    lv_scr_load(screen);

    /*Consecutively loading multiple screens (while deleting one) with transition animations should not crash*/
    lv_obj_t * screen_with_anim_1 = lv_obj_create(NULL);
    lv_obj_t * screen_with_anim_2 = lv_obj_create(NULL);
    lv_obj_t * screen_with_anim_3 = lv_obj_create(NULL);
    lv_obj_t * screen_with_anim_4 = lv_obj_create(NULL);

    lv_scr_load_anim(screen_with_anim_1, LV_SCR_LOAD_ANIM_OVER_LEFT, 0, 0, false);
    lv_obj_add_event_cb(screen_with_anim_1, screen_with_anim_1_unloaded_cb, LV_EVENT_SCREEN_UNLOADED, NULL);
    lv_scr_load_anim(screen_with_anim_2, LV_SCR_LOAD_ANIM_OVER_RIGHT, 1000, 0, true);
    lv_scr_load_anim(screen_with_anim_3, LV_SCR_LOAD_ANIM_OVER_LEFT, 0, 0, true);

    /*Wait to trigger the animation start callbacks*/
    lv_test_indev_wait(100);

    TEST_ASSERT_EQUAL(screen_1_unloaded_called, true);

    lv_scr_load_anim(screen_with_anim_4, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 0, true);

    /*The active screen should be immediately replaced*/
    TEST_ASSERT_EQUAL(lv_scr_act(), screen_with_anim_3);

    lv_test_indev_wait(400);

    /*Check for the screens status after the transition*/
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_1), false);
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_2), false);
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_3), false);
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_4), true);
}

#endif
