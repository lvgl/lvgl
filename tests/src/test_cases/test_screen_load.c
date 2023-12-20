#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void test_screen_load_no_crash(void)
{
    /*Delete active screen and load new screen should not crash*/
    lv_obj_t * screen = lv_screen_active();
    lv_obj_del(screen);
    screen = lv_obj_create(NULL);
    lv_screen_load(screen);

    /*Consecutively loading multiple screens with transition animations should not crash*/
    lv_obj_t * screen_with_anim_1 = lv_obj_create(NULL);
    lv_obj_t * screen_with_anim_2 = lv_obj_create(NULL);
    lv_screen_load_anim(screen_with_anim_1, LV_SCR_LOAD_ANIM_OVER_LEFT, 2000, 0, false);
    lv_screen_load_anim(screen_with_anim_2, LV_SCR_LOAD_ANIM_OVER_RIGHT, 1000, 500, false);
}

#endif
