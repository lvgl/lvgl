#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "../demos/lv_demos.h"

#include "unity/unity.h"

#include "lv_test_helpers.h"
#include "lv_test_indev.h"

static void loop_through_stress_test(void)
{
#if LV_USE_DEMO_STRESS
    while(1) {
        lv_timer_handler();
        if(lv_demo_stress_finished()) {
            break;
        }
        lv_tick_inc(1);
    }
#endif
}
void test_demo_stress(void)
{
#if LV_USE_DEMO_STRESS
    lv_demo_stress();
#endif
    /* loop once to allow objects to be created */
    loop_through_stress_test();
    size_t mem_before = lv_test_get_free_mem();
    /* loop 5 more times */
    for(uint32_t i = 0; i < 5; i++) {
        loop_through_stress_test();
    }
    TEST_ASSERT_EQUAL(mem_before, lv_test_get_free_mem());
}

#endif
