#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * spinner = NULL;

void setUp(void)
{
    active_screen = lv_screen_active();
    spinner = lv_spinner_create(lv_screen_active());
    lv_obj_set_size(spinner, 100, 100);
    lv_obj_center(spinner);
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_spinner_spinning(void)
{
    for(int i = 0; i < 10; ++i) {
        lv_tick_inc(50);
        lv_timer_handler();

        char filename[32];
        lv_snprintf(filename, sizeof(filename), "widgets/spinner_%02d.png", i);
        TEST_ASSERT_EQUAL_SCREENSHOT(filename);
    }
}

void test_spinner_properties(void)
{
#if LV_USE_OBJ_PROPERTY
    lv_obj_t * sp = lv_spinner_create(lv_screen_active());

    lv_spinner_set_anim_params(sp, 2000, 270);

    lv_property_t prop = { };

    prop.id = LV_PROPERTY_SPINNER_ANIM_DURATION;
    TEST_ASSERT_EQUAL_INT(2000, lv_obj_get_property(sp, prop.id).num);

    prop.id = LV_PROPERTY_SPINNER_ARC_SWEEP;
    TEST_ASSERT_EQUAL_INT(270, lv_obj_get_property(sp, prop.id).num);

    /* Test setter */
    prop.id = LV_PROPERTY_SPINNER_ANIM_DURATION;
    prop.num = 1500;
    TEST_ASSERT_TRUE(lv_obj_set_property(sp, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(1500, lv_spinner_get_anim_duration(sp));

    lv_obj_delete(sp);
#endif
}

#endif
