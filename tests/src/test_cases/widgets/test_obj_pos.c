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
    lv_obj_t * parent = lv_obj_create(lv_scr_act());
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
    lv_obj_t * parent = lv_obj_create(lv_scr_act());
    lv_obj_set_size(parent, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    // lv_obj_set_style_pad_all(parent, 10, 0);

    lv_obj_t * child = lv_button_create(parent);
    lv_obj_t * label = lv_label_create(child);
    lv_label_set_text(label, "Button");
    lv_obj_set_size(child, LV_PCT(100), LV_PCT(100));
    lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_pos_no_min_size.png");

    lv_obj_set_style_min_width(child, 200, 0);
    lv_obj_set_style_min_height(child, 300, 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_pos_fixed_min_size.png");

    lv_obj_set_style_min_width(child, LV_SIZE_CONTENT, 0);
    lv_obj_set_style_min_height(child, LV_SIZE_CONTENT, 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_pos_content_min_size.png");
}

#endif
