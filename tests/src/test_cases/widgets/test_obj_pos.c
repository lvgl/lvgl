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

static bool area_is_invalidated(const lv_area_t * area)
{
    lv_display_t * disp = lv_display_get_default();
    for(uint32_t i = 0; i < disp->inv_p; i++) {
        if(lv_area_is_in(area, &disp->inv_areas[i], 0)) return true;
    }
    return false;
}

/**
 * See https://github.com/lvgl/lvgl/issues/6837
 */
void test_content_parent_pct_child_pos_1(void)
{
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_set_pos(parent, 20, 20);
    lv_obj_set_size(parent, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(parent, 10, 0);

    lv_obj_t * child1 = lv_obj_create(parent);
    lv_obj_set_pos(child1, 100, 50);
    lv_obj_set_size(child1, 100, 100);

    lv_obj_t * child2 = lv_obj_create(parent);

    /*Simple case*/
    lv_obj_set_size(child2, 50, 50);
    lv_obj_set_pos(child2, 0, 0);
    lv_obj_update_layout(child2);
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_x(child2));
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_y(child2));

    /*Simple case*/
    lv_obj_set_pos(child2, 30, 200);
    lv_obj_update_layout(child2);
    TEST_ASSERT_EQUAL_INT32(30, lv_obj_get_x(child2));
    TEST_ASSERT_EQUAL_INT32(200, lv_obj_get_y(child2));

    /*A percentage x and y is resolved against the parent's content size. It's not circular:
     *the parent measures this child as if x and y were 0, so moving it can't resize the
     *parent, the child only overflows it.*/
    lv_obj_set_pos(child2, LV_PCT(10), LV_PCT(50));
    lv_obj_update_layout(child2);

    const int32_t parent_w = lv_obj_get_content_width(parent);
    const int32_t parent_h = lv_obj_get_content_height(parent);
    TEST_ASSERT_EQUAL_INT32(parent_w * 10 / 100, lv_obj_get_x(child2));
    TEST_ASSERT_EQUAL_INT32(parent_h * 50 / 100, lv_obj_get_y(child2));

    /*It settles: a second pass moves nothing. The parent doesn't grow to fit the child it
     *just moved, which is what would make the two chase each other.*/
    lv_obj_update_layout(child2);
    TEST_ASSERT_EQUAL_INT32(parent_w, lv_obj_get_content_width(parent));
    TEST_ASSERT_EQUAL_INT32(parent_h, lv_obj_get_content_height(parent));
    TEST_ASSERT_EQUAL_INT32(parent_w * 10 / 100, lv_obj_get_x(child2));
    TEST_ASSERT_EQUAL_INT32(parent_h * 50 / 100, lv_obj_get_y(child2));
}

void test_chaining_invalidation_layout(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_name(cont, "cont");
    lv_obj_set_size(cont, 500, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(cont, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);

    lv_obj_t * label = lv_label_create(cont);
    lv_obj_set_name(label, "label");
    lv_label_set_text(label, "Dropdown with size content:");

    lv_obj_t * sub_cont = lv_obj_create(cont);
    lv_obj_set_name(sub_cont, "sub_cont");
    lv_obj_set_style_bg_color(sub_cont, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_set_style_bg_opa(sub_cont, LV_OPA_COVER, 0);
    lv_obj_set_height(sub_cont, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(sub_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_grow(sub_cont, 1);

    lv_obj_t * dd = lv_dropdown_create(sub_cont);
    lv_obj_set_name(dd, "dropdown");
    lv_dropdown_set_options(dd, "Short\nA bit longer option\nThe longest option in the list");
    lv_obj_set_width(dd, 0);
    lv_obj_set_flex_grow(dd, 1);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_pos_chained_layout_invalidation_pre.png");
    lv_dropdown_set_selected(dd, 2);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_pos_chained_layout_invalidation_post.png");
}

static lv_obj_t * cont_create(lv_obj_t * parent, const char * text)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_remove_style(cont, NULL, LV_PART_MAIN);
    lv_obj_set_style_border_width(cont, 1, LV_PART_MAIN);
    lv_obj_set_size(cont, 150, 30);

    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, text);

    lv_obj_t * rect_top_right = lv_obj_create(cont);
    lv_obj_remove_style(rect_top_right, NULL, LV_PART_MAIN);
    lv_obj_set_size(rect_top_right, 10, 10);
    lv_obj_set_align(rect_top_right, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_style_bg_color(rect_top_right, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(rect_top_right, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_t * rect_bottom_left = lv_obj_create(cont);
    lv_obj_remove_style(rect_bottom_left, NULL, LV_PART_MAIN);
    lv_obj_set_size(rect_bottom_left, 10, 10);
    lv_obj_set_align(rect_bottom_left, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_style_bg_color(rect_bottom_left, lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(rect_bottom_left, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_t * rect_bottom_right = lv_obj_create(cont);
    lv_obj_remove_style(rect_bottom_right, NULL, LV_PART_MAIN);
    lv_obj_set_size(rect_bottom_right, 10, 10);
    lv_obj_set_align(rect_bottom_right, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_set_style_bg_color(rect_bottom_right, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(rect_bottom_right, LV_OPA_COVER, LV_PART_MAIN);

    return cont;
}

static void cont_create_x_y(lv_obj_t * parent, const char * text, int32_t x, int32_t y)
{
    lv_obj_t * cont = cont_create(parent, text);
    lv_obj_set_pos(cont, x, y);
}

void test_rtl_pos_x_y(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());

    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_base_dir(cont, LV_BASE_DIR_RTL, LV_PART_MAIN);

    cont_create_x_y(cont, "(0,0)", 0, 0);
    cont_create_x_y(cont, "(50,50)", 50, 50);
    cont_create_x_y(cont, "(100,100)", 100, 100);
    cont_create_x_y(cont, "(150,150)", 150, 150);
    cont_create_x_y(cont, "(100,200)", 100, 200);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/rtl_obj_pos_x_y.png");
}

static void cont_create_align(lv_obj_t * parent, const char * text, lv_align_t align)
{
    lv_obj_t * cont = cont_create(parent, text);
    lv_obj_set_align(cont, align);
}

static void cont_create_align_offset(lv_obj_t * parent, const char * text, lv_align_t align, int32_t x_ofs,
                                     int32_t y_ofs)
{
    lv_obj_t * cont = cont_create(parent, text);
    lv_obj_align(cont, align, x_ofs, y_ofs);
}

void test_align_left(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_base_dir(cont, LV_BASE_DIR_LTR, LV_PART_MAIN);

    cont_create_align(cont, "TOP_LEFT", LV_ALIGN_TOP_LEFT);
    cont_create_align(cont, "LEFT_MID", LV_ALIGN_LEFT_MID);
    cont_create_align(cont, "BOTTOM_LEFT", LV_ALIGN_BOTTOM_LEFT);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_align_left.png");
}

void test_align_right(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_base_dir(cont, LV_BASE_DIR_LTR, LV_PART_MAIN);

    cont_create_align(cont, "TOP_RIGHT", LV_ALIGN_TOP_RIGHT);
    cont_create_align(cont, "RIGHT_MID", LV_ALIGN_RIGHT_MID);
    cont_create_align(cont, "BOTTOM_RIGHT", LV_ALIGN_BOTTOM_RIGHT);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_align_right.png");
}

void test_align_center(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_base_dir(cont, LV_BASE_DIR_LTR, LV_PART_MAIN);

    cont_create_align_offset(cont, "TOP_MID (150,0)", LV_ALIGN_TOP_MID, 150, 0);
    cont_create_align_offset(cont, "CENTER (150,0)", LV_ALIGN_CENTER, 150, 0);
    cont_create_align_offset(cont, "BOTTOM_MID (150,0)", LV_ALIGN_BOTTOM_MID, 150, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_align_center.png");
}

void test_rtl_align_left(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_base_dir(cont, LV_BASE_DIR_RTL, LV_PART_MAIN);

    cont_create_align(cont, "TOP_LEFT", LV_ALIGN_TOP_LEFT);
    cont_create_align(cont, "LEFT_MID", LV_ALIGN_LEFT_MID);
    cont_create_align(cont, "BOTTOM_LEFT", LV_ALIGN_BOTTOM_LEFT);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/rtl_obj_align_left.png");
}

void test_rtl_align_right(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_base_dir(cont, LV_BASE_DIR_RTL, LV_PART_MAIN);

    cont_create_align(cont, "TOP_RIGHT", LV_ALIGN_TOP_RIGHT);
    cont_create_align(cont, "RIGHT_MID", LV_ALIGN_RIGHT_MID);
    cont_create_align(cont, "BOTTOM_RIGHT", LV_ALIGN_BOTTOM_RIGHT);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/rtl_obj_align_right.png");
}

void test_rtl_align_center(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_base_dir(cont, LV_BASE_DIR_RTL, LV_PART_MAIN);

    cont_create_align_offset(cont, "TOP_MID (150,0)", LV_ALIGN_TOP_MID, 150, 0);
    cont_create_align_offset(cont, "CENTER (150,0)", LV_ALIGN_CENTER, 150, 0);
    cont_create_align_offset(cont, "BOTTOM_MID (150,0)", LV_ALIGN_BOTTOM_MID, 150, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/rtl_obj_align_center.png");
}

/*An invalidation discarded while invalidation is disabled must not block the next one*/
void test_invalidate_while_disabled(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_pos(obj, 10, 10);
    lv_obj_set_size(obj, 50, 50);
    lv_refr_now(NULL);

    lv_display_t * disp = lv_display_get_default();
    lv_display_enable_invalidation(disp, false);
    lv_obj_invalidate(obj);
    lv_display_enable_invalidation(disp, true);
    TEST_ASSERT_FALSE(area_is_invalidated(&obj->coords));

    lv_obj_invalidate(obj);
    TEST_ASSERT_TRUE(area_is_invalidated(&obj->coords));
}

/*A screen has no parent to take a percentage of*/
void test_calc_dynamic_size_pct_on_a_screen(void)
{
    lv_obj_t * scr = lv_obj_create(NULL);
    lv_obj_set_style_min_width(scr, LV_PCT(50), 0);
    lv_obj_set_style_max_width(scr, LV_PCT(50), 0);
    lv_obj_set_style_min_height(scr, LV_PCT(50), 0);
    lv_obj_set_style_max_height(scr, LV_PCT(50), 0);

    TEST_ASSERT_EQUAL_INT32(0, lv_obj_calc_dynamic_width(scr, LV_STYLE_MIN_WIDTH));
    TEST_ASSERT_EQUAL_INT32(LV_COORD_MAX, lv_obj_calc_dynamic_width(scr, LV_STYLE_MAX_WIDTH));
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_calc_dynamic_height(scr, LV_STYLE_MIN_HEIGHT));
    TEST_ASSERT_EQUAL_INT32(LV_COORD_MAX, lv_obj_calc_dynamic_height(scr, LV_STYLE_MAX_HEIGHT));

    lv_obj_delete(scr);
}
#endif
