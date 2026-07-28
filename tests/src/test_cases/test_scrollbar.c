#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

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

void test_scrollbar_vertical(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW);

    const int32_t inner_sizes[] = {100, 500, 1000, 5000, 100000, 500000, 1000000, 4000000};
    for(size_t i = 0; i < sizeof(inner_sizes) / sizeof(inner_sizes[0]); i++) {
        lv_obj_t * wrapper = lv_obj_create(lv_screen_active());
        lv_obj_set_flex_grow(wrapper, 1);
        lv_obj_set_height(wrapper, LV_PCT(100));

        lv_obj_t * inner = lv_obj_create(wrapper);
        lv_obj_set_size(inner, LV_PCT(100), inner_sizes[i]);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("scrollbar_vertical.png");
}

void test_scrollbar_horizontal(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_COLUMN);

    const int32_t inner_sizes[] = {100, 500, 1000, 5000, 100000, 500000, 1000000, 4000000};
    for(size_t i = 0; i < sizeof(inner_sizes) / sizeof(inner_sizes[0]); i++) {
        lv_obj_t * wrapper = lv_obj_create(lv_screen_active());
        lv_obj_set_flex_grow(wrapper, 1);
        lv_obj_set_width(wrapper, LV_PCT(100));

        lv_obj_t * inner = lv_obj_create(wrapper);
        lv_obj_set_size(inner, inner_sizes[i], LV_PCT(100));
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("scrollbar_horizontal.png");
}

#endif
