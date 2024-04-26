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

#endif
