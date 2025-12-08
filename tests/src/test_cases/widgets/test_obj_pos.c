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
    lv_obj_set_size(child, LV_PCT(100), LV_PCT(100));
    lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_pos_no_min_size.png");
    TEST_ASSERT_FALSE(lv_obj_is_width_min(child));
    TEST_ASSERT_FALSE(lv_obj_is_height_min(child));
    TEST_ASSERT_EQUAL(LV_PCT(100), lv_obj_get_style_clamped_width(child));
    TEST_ASSERT_EQUAL(LV_PCT(100), lv_obj_get_style_clamped_height(child));

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
    lv_obj_set_style_min_width(sub_cont, LV_SIZE_CONTENT, LV_PART_MAIN);

    lv_obj_t * dd = lv_dropdown_create(sub_cont);
    lv_obj_set_name(dd, "dropdown");
    lv_dropdown_set_options(dd, "Short\nA bit longer option\nThe longest option in the list");
    lv_obj_set_width(dd, 0);
    lv_obj_set_style_min_width(dd, LV_SIZE_CONTENT, 0);
    lv_obj_set_flex_grow(dd, 1);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_pos_chained_layout_invalidation_pre.png");
    lv_dropdown_set_selected(dd, 2);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_pos_chained_layout_invalidation_post.png");
}

#endif
