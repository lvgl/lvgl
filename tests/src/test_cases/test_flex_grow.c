#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    active_screen = lv_screen_active();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

static void simple_style(lv_obj_t * obj)
{
    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN);
}

/**
 * Tests that various flex tracks containing fixed and growing items are
 * always completely filled, no red background visible.
 */
void test_grid_fr(void)
{
    simple_style(active_screen);

    lv_obj_t * container = lv_obj_create(active_screen);
    simple_style(container);
    lv_obj_set_style_bg_color(container, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
    lv_obj_set_size(container, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_center(container);

    for(int32_t grow_1 = 1; grow_1 <= 3; grow_1++) {
        for(int32_t grow_2 = 1; grow_2 <= 3; grow_2++) {
            for(int32_t fixed = 20; fixed <= 24; fixed++) {
                lv_obj_t * child_grow_1 = lv_obj_create(container);
                simple_style(child_grow_1);
                lv_obj_set_style_bg_color(child_grow_1, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
                lv_obj_set_size(child_grow_1, 1, 10);
                lv_obj_set_flex_grow(child_grow_1, grow_1);
                lv_obj_add_flag(child_grow_1, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

                lv_obj_t * child_grow_2 = lv_obj_create(container);
                simple_style(child_grow_2);
                lv_obj_set_style_bg_color(child_grow_2, lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN);
                lv_obj_set_size(child_grow_2, 1, 10);
                lv_obj_set_flex_grow(child_grow_2, grow_2);

                lv_obj_t * child_fixed = lv_obj_create(container);
                simple_style(child_fixed);
                lv_obj_set_style_bg_color(child_fixed, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
                lv_obj_set_size(child_fixed, fixed, 10);
            }
        }
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("flex_grow.png");
}

#endif
