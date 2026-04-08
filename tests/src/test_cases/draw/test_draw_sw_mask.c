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

void test_radius_mask_overflow(void)
{
    int width = 1280;
    int heigh = 1280;
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, width * 2, heigh * 2);
    lv_obj_set_style_radius(obj, heigh, LV_PART_MAIN);

    lv_timer_handler();
}

#endif
