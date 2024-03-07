#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../demos/lv_demos.h"

#include "unity/unity.h"

#include "lv_test_helpers.h"
#include "lv_test_indev.h"

void test_demo_widgets(void)
{
#if LV_USE_DEMO_WIDGETS
    lv_demo_widgets();
#endif
}

#endif
