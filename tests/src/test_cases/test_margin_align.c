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

// takes no effect on position but size.
void test_align(void)
{
    lv_obj_t * obj0 = active_screen;

    lv_obj_set_width(obj0, 800);
    lv_obj_set_height(obj0, 400);
    lv_obj_center(obj0);
    lv_obj_set_style_pad_all(obj0, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj0, 0, LV_PART_MAIN);

    lv_obj_t * obj0s[] = {
        obj_create(obj0, lv_palette_main(LV_PALETTE_BLUE)),
        obj_create(obj0, lv_palette_main(LV_PALETTE_RED)),
        obj_create(obj0, lv_palette_main(LV_PALETTE_DEEP_PURPLE)),
        obj_create(obj0, lv_palette_main(LV_PALETTE_GREEN)),
        obj_create(obj0, lv_palette_main(LV_PALETTE_PINK))
    };

    lv_obj_align(obj0s[0], LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_align_to(obj0s[1], obj0s[0], LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_align_to(obj0s[2], obj0s[1], LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    lv_obj_align_to(obj0s[3], obj0s[2], LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_update_layout(obj0);

    obj_set_margin(obj0s[0], 20, 50, 60, 50);
    lv_obj_set_style_width(obj0s[0], LV_PCT(60), LV_PART_MAIN);
    obj_set_margin(obj0s[1], 15, 12, 50, 20);
    obj_set_margin(obj0s[2], 25, 25, 100, 20);
    obj_set_margin(obj0s[3], 12, 50, 100, 100);
    lv_obj_set_style_width(obj0s[3], LV_PCT(100), LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("margin_align_0.png");
    obj_set_margin(obj0s[4], 6, 100, 100, 100);

    lv_obj_set_style_radius(obj0s[0], 0, LV_PART_MAIN);

    lv_obj_align(obj0s[4], LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_update_layout(obj0);

    TEST_ASSERT_EQUAL_SCREENSHOT("margin_align_1.png");
}

#endif
