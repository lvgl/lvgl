#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * slider = NULL;

void setUp(void)
{
    active_screen = lv_scr_act();
    slider = lv_slider_create(active_screen);
}

void tearDown(void)
{
}

void test_textarea_should_have_valid_documented_default_values(void)
{
    lv_coord_t objw = lv_obj_get_width(slider);
    lv_coord_t objh = lv_obj_get_height(slider);

    /* Horizontal slider */
    TEST_ASSERT_TRUE(objw >= objh);
    TEST_ASSERT_FALSE(lv_obj_has_flag(slider, LV_OBJ_FLAG_SCROLL_CHAIN));
    TEST_ASSERT_FALSE(lv_obj_has_flag(slider, LV_OBJ_FLAG_SCROLLABLE));
}

#endif
