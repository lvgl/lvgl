#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

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

lv_obj_t * obj_create(lv_obj_t * parent, lv_color_t color)
{
    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_set_style_bg_color(obj, color, LV_PART_MAIN);
    lv_obj_set_width(obj, 150);
    lv_obj_set_height(obj, 150);

    return obj;
}

void obj_set_margin(lv_obj_t * obj, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    lv_obj_set_style_margin_left(obj, left, LV_PART_MAIN);
    lv_obj_set_style_margin_top(obj, top, LV_PART_MAIN);
    lv_obj_set_style_margin_right(obj, right, LV_PART_MAIN);
    lv_obj_set_style_margin_bottom(obj, bottom, LV_PART_MAIN);
}

void test_flex(void)
{
#define N 5
    lv_obj_t * obj0 = active_screen;

    lv_obj_set_flex_flow(obj0, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(obj0, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_gap(obj0, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj0, 0, LV_PART_MAIN);

    lv_obj_t * obj0s[N] = {
        obj_create(obj0, lv_palette_main(LV_PALETTE_BLUE)),
        obj_create(obj0, lv_palette_main(LV_PALETTE_RED)),
        obj_create(obj0, lv_palette_main(LV_PALETTE_PURPLE)),
        obj_create(obj0, lv_palette_main(LV_PALETTE_GREEN)),
        obj_create(obj0, lv_palette_main(LV_PALETTE_PINK))
    };

    lv_obj_t * o;
    for(int i = 0; i < N; i++) {
        o = obj0s[i];
        lv_obj_set_style_radius(o, 0, LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(o, LV_SCROLLBAR_MODE_OFF);
        lv_obj_set_style_border_width(o, 0, LV_PART_MAIN);
    }

    lv_obj_set_width(obj0s[0], LV_PCT(50));
    lv_obj_set_height(obj0s[0], LV_PCT(50));

    obj_set_margin(obj0s[0], 50, 3, 10, 50);
    obj_set_margin(obj0s[1], 25, 6, 20, 50);
    obj_set_margin(obj0s[2], 12, 12, 30, 50);
    obj_set_margin(obj0s[3], 12, 25, 30, 50);
    obj_set_margin(obj0s[4], 24, 50, 50, 50);

    lv_obj_update_layout(obj0);

    TEST_ASSERT_EQUAL_SCREENSHOT("margin_flex_0.png");

    lv_obj_set_flex_flow(obj0, LV_FLEX_FLOW_ROW_WRAP);
    TEST_ASSERT_EQUAL_SCREENSHOT("margin_flex_1.png");

    lv_obj_set_style_flex_main_place(obj0, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("margin_flex_2.png");

    lv_obj_set_style_flex_cross_place(obj0, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("margin_flex_3.png");

    lv_obj_set_style_flex_track_place(obj0, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("margin_flex_4.png");

    lv_obj_set_flex_flow(obj0, LV_FLEX_FLOW_COLUMN_WRAP);
    TEST_ASSERT_EQUAL_SCREENSHOT("margin_flex_5.png");

#undef N
}

#endif
