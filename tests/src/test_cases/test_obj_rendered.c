#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

void test_obj_is_rendered_after_the_first_refresh(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    TEST_ASSERT_FALSE(lv_obj_is_rendered(obj));

    lv_refr_now(NULL);
    TEST_ASSERT_TRUE(lv_obj_is_rendered(obj));

    /*It stays rendered, also when it is hidden later*/
    lv_obj_set_hidden(obj, true);
    lv_refr_now(NULL);
    TEST_ASSERT_TRUE(lv_obj_is_rendered(obj));
}

void test_obj_is_not_rendered_while_hidden(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_hidden(obj, true);

    lv_refr_now(NULL);
    TEST_ASSERT_FALSE(lv_obj_is_rendered(obj));

    lv_obj_set_hidden(obj, false);
    lv_refr_now(NULL);
    TEST_ASSERT_TRUE(lv_obj_is_rendered(obj));
}

#endif
