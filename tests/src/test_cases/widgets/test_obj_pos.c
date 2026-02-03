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

    /*x and y should be 0 to avoid circural dependency*/
    lv_obj_set_pos(child2, LV_PCT(10), LV_PCT(50));
    lv_obj_update_layout(child2);
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_x(child2));
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_y(child2));
}

void test_style_min_size(void)
{
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_set_size(parent, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    // lv_obj_set_style_pad_all(parent, 10, 0);

    lv_obj_t * child = lv_button_create(parent);
    lv_obj_t * label = lv_label_create(child);
    lv_label_set_text(label, "Button");
    lv_obj_set_size(child, LV_PCT(100), LV_PCT(100)); // will evaluate to 0
    lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_pos_no_min_size.png");
    TEST_ASSERT_TRUE(lv_obj_is_width_min(child));
    TEST_ASSERT_TRUE(lv_obj_is_height_min(child));
    TEST_ASSERT_EQUAL(0, lv_obj_get_style_clamped_width(child));
    TEST_ASSERT_EQUAL(0, lv_obj_get_style_clamped_height(child));

    lv_obj_set_style_min_width(child, 200, 0);
    lv_obj_set_style_min_height(child, 300, 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_pos_fixed_min_size.png");
    TEST_ASSERT_EQUAL_INT32(200, lv_obj_get_width(child));
    TEST_ASSERT_EQUAL_INT32(300, lv_obj_get_height(child));
    TEST_ASSERT_TRUE(lv_obj_is_width_min(child));
    TEST_ASSERT_TRUE(lv_obj_is_height_min(child));
    TEST_ASSERT_EQUAL(200, lv_obj_get_style_clamped_width(child));
    TEST_ASSERT_EQUAL(300, lv_obj_get_style_clamped_height(child));

    lv_obj_set_style_min_width(child, LV_SIZE_CONTENT, 0);
    lv_obj_set_style_min_height(child, LV_SIZE_CONTENT, 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_pos_content_min_size.png");
    TEST_ASSERT_TRUE(lv_obj_is_width_min(child));
    TEST_ASSERT_TRUE(lv_obj_is_height_min(child));

    lv_obj_set_size(parent, 100, 100);
    lv_obj_set_style_min_width(child, 0, 0);
    lv_obj_set_style_min_height(child, 0, 0);
    lv_refr_now(NULL);
    TEST_ASSERT_FALSE(lv_obj_is_width_min(child));
    TEST_ASSERT_FALSE(lv_obj_is_height_min(child));
    TEST_ASSERT_EQUAL(LV_PCT(100), lv_obj_get_style_clamped_width(child));
    TEST_ASSERT_EQUAL(LV_PCT(100), lv_obj_get_style_clamped_height(child));
}

void test_circular_height_dependency(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_name(cont, "cont");
    lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(cont, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);

    lv_obj_t * item1 = lv_obj_create(cont);
    lv_obj_set_name(item1, "item1");
    lv_obj_set_style_bg_color(item1, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_set_style_bg_opa(item1, LV_OPA_COVER, 0);
    /**
     * Because parent is size content this will evaluate after all the fixed/clamped children are sized
     * This means item1 should size to 100 because of item2
     */
    lv_obj_set_height(item1, LV_PCT(100));
    lv_obj_set_flex_grow(item1, 1);

    lv_obj_t * item2 = lv_obj_create(cont);
    lv_obj_set_name(item2, "item2");
    lv_obj_set_style_bg_color(item2, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_bg_opa(item2, LV_OPA_COVER, 0);
    lv_obj_set_height(item2, 100); // fixed size
    lv_obj_set_flex_grow(item2, 1);

    lv_obj_t * item3 = lv_obj_create(cont);
    lv_obj_set_name(item3, "item3");
    lv_obj_set_style_bg_color(item3, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_set_style_bg_opa(item3, LV_OPA_COVER, 0);
    lv_obj_set_height(item3, LV_PCT(100)); // same as item1 but checking if child order matters
    lv_obj_set_flex_grow(item3, 1);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_circular_height.png");
    TEST_ASSERT_EQUAL_INT32(100, lv_obj_get_height(item2));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_height(item2), lv_obj_get_height(item1));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_height(item2), lv_obj_get_height(item3));

    /**
     * Decreasing item2 height should also decrease item1 and item3 height
     */
    lv_obj_set_height(item2, 40);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_INT32(40, lv_obj_get_height(item2));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_height(item2), lv_obj_get_height(item1));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_height(item2), lv_obj_get_height(item3));

    lv_obj_set_height(item2, LV_PCT(100));
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_height(item1));
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_height(item2));
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_height(item3));

    lv_obj_set_style_min_height(item2, 50, 0);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_INT32(50, lv_obj_get_height(item2));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_height(item2), lv_obj_get_height(item1));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_height(item2), lv_obj_get_height(item3));
}

void test_circular_width_dependency(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_name(cont, "cont");
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_PCT(100));
    lv_obj_set_style_bg_color(cont, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);

    lv_obj_t * item1 = lv_obj_create(cont);
    lv_obj_set_name(item1, "item1");
    lv_obj_set_style_bg_color(item1, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_set_style_bg_opa(item1, LV_OPA_COVER, 0);
    /**
     * Because parent is size content this will evaluate after all the fixed/clamped children are sized
     * This means item1 should size to 100 because of item2
     */
    lv_obj_set_width(item1, LV_PCT(100));
    lv_obj_set_flex_grow(item1, 1);

    lv_obj_t * item2 = lv_obj_create(cont);
    lv_obj_set_name(item2, "item2");
    lv_obj_set_style_bg_color(item2, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_bg_opa(item2, LV_OPA_COVER, 0);
    lv_obj_set_width(item2, 100); // fixed size
    lv_obj_set_flex_grow(item2, 1);

    lv_obj_t * item3 = lv_obj_create(cont);
    lv_obj_set_name(item3, "item3");
    lv_obj_set_style_bg_color(item3, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_set_style_bg_opa(item3, LV_OPA_COVER, 0);
    lv_obj_set_width(item3, LV_PCT(100)); // same as item1 but checking if child order matters
    lv_obj_set_flex_grow(item3, 1);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_circular_width.png");
    TEST_ASSERT_EQUAL_INT32(100, lv_obj_get_width(item2));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(item2), lv_obj_get_width(item1));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(item2), lv_obj_get_width(item3));

    /**
     * Decreasing item2 width should also decrease item1 and item3 width
     */
    lv_obj_set_width(item2, 40);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_INT32(40, lv_obj_get_width(item2));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(item2), lv_obj_get_width(item1));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(item2), lv_obj_get_width(item3));

    lv_obj_set_width(item2, LV_PCT(100));
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_width(item1));
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_width(item2));
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_width(item3));

    lv_obj_set_style_min_width(item2, 50, 0);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_INT32(50, lv_obj_get_width(item2));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(item2), lv_obj_get_width(item1));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(item2), lv_obj_get_width(item3));
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
#endif
