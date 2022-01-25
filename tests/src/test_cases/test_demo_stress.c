#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../demos/lv_demos.h"

#include "unity/unity.h"

#include "lv_test_helpers.h"
#include "lv_test_indev.h"

static void loop_through_stress_test(void)
{
#if LV_USE_DEMO_STRESS
    lv_test_indev_wait(LV_DEMO_STRESS_TIME_STEP*33); /* FIXME: remove magic number of states */
#endif
}
void test_demo_stress(void)
{
#if LV_USE_DEMO_STRESS
    lv_demo_stress();
#endif
    /* loop once to allow objects to be created */
    loop_through_stress_test();
    uint32_t mem_before = lv_test_get_free_mem();
    /* loop 10 more times */
    for(uint32_t i = 0; i < 10; i++) {
        loop_through_stress_test();
    }
    TEST_ASSERT_EQUAL(mem_before, lv_test_get_free_mem());
}

#endif

