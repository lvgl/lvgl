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

static void simple_style(lv_obj_t * obj)
{
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_margin_all(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_pad(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(obj, 0, LV_PART_MAIN);
}

static lv_obj_t * create_row(lv_flex_align_t main_place, int n_children)
{
    lv_obj_t * row = lv_obj_create(lv_screen_active());
    lv_obj_set_size(row, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, main_place, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    simple_style(row);
    for(int i = 0; i < n_children; i++) {
        lv_obj_t * child = lv_obj_create(row);
        lv_obj_set_size(child, 40, 40);
        lv_obj_set_style_bg_color(child, lv_palette_main(LV_PALETTE_BLUE), 0);
        simple_style(child);
    }
    return row;
}

void test_align(void)
{
    create_row(LV_FLEX_ALIGN_CENTER, 1);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_center1.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_CENTER, 2);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_center2.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_SPACE_EVENLY, 1);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_space_evenly1.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_SPACE_EVENLY, 2);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_space_evenly2.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_SPACE_AROUND, 1);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_space_around1.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_SPACE_AROUND, 2);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_space_around2.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_SPACE_BETWEEN, 1);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_space_between1.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_SPACE_BETWEEN, 2);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_space_between2.png");
    lv_obj_clean(active_screen);
}

/*See https://github.com/lvgl/lvgl/issues/7035*/
void test_wrap_grow_min_width(void)
{

    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);


    for(int i = 0; i < 14; i++) {
        lv_obj_t * obj = lv_obj_create(lv_screen_active());
        lv_obj_set_style_min_width(obj, 100, 0);
        lv_obj_set_style_pad_all(obj, 0, 0);
        lv_obj_set_height(obj, 70 + (i % 3) * 20);

        uint8_t grow = (i % 3);
        lv_obj_set_flex_grow(obj, grow);

        if(i == 9) {
            lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        }

        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "grow:%d%s", grow, i == 9 ? "\nnew track" : "");
        lv_obj_center(label);
    }
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_wrap_grow_min_width.png");
}

#endif
