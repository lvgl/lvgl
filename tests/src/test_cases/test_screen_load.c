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
}

#endif
