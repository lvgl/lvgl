#if LV_BUILD_TEST
#include "../../lvgl.h"

#include "unity/unity.h"

#include <string.h>

void setUp(void)
{
}

void tearDown(void)
{
    lv_log_register_print_cb(NULL);
    lv_obj_clean(lv_screen_active());
}

void test_one_line_textarea_in_content_parent_settles(void)
{
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_set_size(parent, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    lv_obj_t * ta = lv_textarea_create(parent);
    lv_textarea_set_one_line(ta, true);
    lv_textarea_set_text(ta, "item value");
    lv_obj_set_size(ta, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    lv_obj_update_layout(lv_screen_active());

    lv_obj_t * label = lv_obj_get_child(ta, 0);
    TEST_ASSERT_GREATER_THAN_INT32(0, lv_obj_get_width(label));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(label), lv_obj_get_content_width(ta));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(ta), lv_obj_get_content_width(parent));
}

/* more generic approach to test above*/
void test_content_size_chain_with_pct_min_width_settles(void)
{
    lv_obj_t * grandparent = lv_obj_create(lv_screen_active());
    lv_obj_set_size(grandparent, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    lv_obj_t * parent = lv_obj_create(grandparent);
    lv_obj_set_size(parent, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label, "item value");
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_set_style_min_width(label, LV_PCT(100), LV_PART_MAIN);

    lv_obj_update_layout(lv_screen_active());

    /*The label is sized to its text and the parents wrap it instead of collapsing*/
    TEST_ASSERT_GREATER_THAN_INT32(0, lv_obj_get_width(label));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(label), lv_obj_get_content_width(parent));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(parent), lv_obj_get_content_width(grandparent));
}

/**
 * Contradictory bounds (`min_width > max_width`) resolve to the min width, like CSS does. The
 * percentage min width must therefore still count as the width in effect, even when the width style
 * is above both bounds. Otherwise the object is credited to its `LV_SIZE_CONTENT` parent's content
 * width and the two sizes chase each other upwards.
 */
void test_inverted_width_bounds_resolve_to_min_width(void)
{
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_set_size(parent, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(parent, 0, LV_PART_MAIN);

    /*Fixes the parent's content width, so the percentage min width below has a stable reference*/
    lv_obj_t * sibling = lv_obj_create(parent);
    lv_obj_set_size(sibling, 200, 20);

    /*The width is above both bounds, and the bounds are inverted, so the min width wins*/
    lv_obj_t * child = lv_obj_create(parent);
    lv_obj_set_size(child, 1000, 20);
    lv_obj_set_style_min_width(child, LV_PCT(150), LV_PART_MAIN);
    lv_obj_set_style_max_width(child, 10, LV_PART_MAIN);

    lv_obj_update_layout(lv_screen_active());

    TEST_ASSERT_EQUAL_INT32(200, lv_obj_get_content_width(parent));
    TEST_ASSERT_EQUAL_INT32(300, lv_obj_get_width(child));
}

/*The same, vertically*/
void test_inverted_height_bounds_resolve_to_min_height(void)
{
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_set_size(parent, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(parent, 0, LV_PART_MAIN);

    lv_obj_t * sibling = lv_obj_create(parent);
    lv_obj_set_size(sibling, 20, 200);

    lv_obj_t * child = lv_obj_create(parent);
    lv_obj_set_size(child, 20, 1000);
    lv_obj_set_style_min_height(child, LV_PCT(150), LV_PART_MAIN);
    lv_obj_set_style_max_height(child, 10, LV_PART_MAIN);

    lv_obj_update_layout(lv_screen_active());

    TEST_ASSERT_EQUAL_INT32(200, lv_obj_get_content_height(parent));
    TEST_ASSERT_EQUAL_INT32(300, lv_obj_get_height(child));
}

#endif
