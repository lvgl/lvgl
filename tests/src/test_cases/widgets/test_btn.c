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

void test_button_creation(void)
{
    lv_obj_t * btn;
    btn = lv_button_create(active_screen);
    TEST_ASSERT_NOT_NULL(btn);
    /* These flags are set in the object's constructor. */
    TEST_ASSERT_TRUE(lv_obj_is_scroll_on_focus(btn));
    TEST_ASSERT_FALSE(lv_obj_is_scrollable(btn));
}

#endif
