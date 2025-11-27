/**
 * @file test_flex_size_content.c
 *
 * Tests for SIZE_CONTENT propagation in nested flex containers.
 * Verifies that parent containers with SIZE_CONTENT correctly resize
 * when child flex containers position their children.
 */

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
    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_margin_all(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_pad(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(obj, 0, LV_PART_MAIN);
}

/**
 * Test: Simple flex container with SIZE_CONTENT wrapping fixed-size children
 * Expected: Container height should equal child height
 */
void test_flex_size_content_simple(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, 200, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    simple_style(cont);

    lv_obj_t * child = lv_obj_create(cont);
    lv_obj_set_size(child, 50, 80);
    simple_style(child);

    lv_obj_update_layout(lv_screen_active());

    int32_t cont_height = lv_obj_get_height(cont);
    TEST_ASSERT_EQUAL_INT32(80, cont_height);
}

/**
 * Test: Nested containers where both have SIZE_CONTENT height
 * Expected: Both containers should have non-zero height equal to innermost child
 */
void test_flex_size_content_nested(void)
{
    /* Outer container with SIZE_CONTENT */
    lv_obj_t * outer = lv_obj_create(lv_screen_active());
    lv_obj_set_size(outer, 300, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(outer, LV_FLEX_FLOW_COLUMN);
    simple_style(outer);
    lv_obj_center(outer);

    /* Inner container with SIZE_CONTENT */
    lv_obj_t * inner = lv_obj_create(outer);
    lv_obj_set_size(inner, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(inner, LV_FLEX_FLOW_ROW);
    simple_style(inner);

    /* Fixed-size child */
    lv_obj_t * child = lv_obj_create(inner);
    lv_obj_set_size(child, 60, 100);
    simple_style(child);

    lv_obj_update_layout(lv_screen_active());

    int32_t child_height = lv_obj_get_height(child);
    int32_t inner_height = lv_obj_get_height(inner);
    int32_t outer_height = lv_obj_get_height(outer);

    TEST_ASSERT_EQUAL_INT32(100, child_height);
    TEST_ASSERT_EQUAL_INT32(100, inner_height);
    TEST_ASSERT_EQUAL_INT32(100, outer_height);
}

/**
 * Test: Triple-nested containers with SIZE_CONTENT
 * Expected: All three levels should propagate height correctly
 */
void test_flex_size_content_triple_nested(void)
{
    /* Level 1: Outermost */
    lv_obj_t * l1 = lv_obj_create(lv_screen_active());
    lv_obj_set_size(l1, 400, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(l1, LV_FLEX_FLOW_COLUMN);
    simple_style(l1);
    lv_obj_center(l1);

    /* Level 2: Middle */
    lv_obj_t * l2 = lv_obj_create(l1);
    lv_obj_set_size(l2, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(l2, LV_FLEX_FLOW_ROW);
    simple_style(l2);

    /* Level 3: Innermost */
    lv_obj_t * l3 = lv_obj_create(l2);
    lv_obj_set_size(l3, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(l3, LV_FLEX_FLOW_COLUMN);
    simple_style(l3);

    /* Leaf child */
    lv_obj_t * leaf = lv_obj_create(l3);
    lv_obj_set_size(leaf, 50, 120);
    simple_style(leaf);

    lv_obj_update_layout(lv_screen_active());

    int32_t leaf_height = lv_obj_get_height(leaf);
    int32_t l3_height = lv_obj_get_height(l3);
    int32_t l2_height = lv_obj_get_height(l2);
    int32_t l1_height = lv_obj_get_height(l1);

    TEST_ASSERT_EQUAL_INT32(120, leaf_height);
    TEST_ASSERT_EQUAL_INT32(120, l3_height);
    TEST_ASSERT_EQUAL_INT32(120, l2_height);
    TEST_ASSERT_EQUAL_INT32(120, l1_height);
}

/**
 * Test: Container with SIZE_CONTENT wrapping label
 * Expected: Container should size to fit label content
 */
void test_flex_size_content_with_label(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    simple_style(cont);
    lv_obj_center(cont);

    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, "Hello World");

    lv_obj_update_layout(lv_screen_active());

    int32_t cont_width = lv_obj_get_width(cont);
    int32_t cont_height = lv_obj_get_height(cont);
    int32_t label_width = lv_obj_get_width(label);
    int32_t label_height = lv_obj_get_height(label);

    /* Container should match label dimensions */
    TEST_ASSERT_EQUAL_INT32(label_width, cont_width);
    TEST_ASSERT_EQUAL_INT32(label_height, cont_height);
}

/**
 * Test: Nested SIZE_CONTENT with sibling containers
 * Expected: Both siblings should correctly propagate to parent
 */
void test_flex_size_content_with_siblings(void)
{
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_set_size(parent, 400, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW);
    simple_style(parent);
    lv_obj_center(parent);

    /* First sibling - shorter */
    lv_obj_t * sib1 = lv_obj_create(parent);
    lv_obj_set_size(sib1, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(sib1, LV_FLEX_FLOW_COLUMN);
    simple_style(sib1);

    lv_obj_t * child1 = lv_obj_create(sib1);
    lv_obj_set_size(child1, 40, 60);
    simple_style(child1);

    /* Second sibling - taller */
    lv_obj_t * sib2 = lv_obj_create(parent);
    lv_obj_set_size(sib2, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(sib2, LV_FLEX_FLOW_COLUMN);
    simple_style(sib2);

    lv_obj_t * child2 = lv_obj_create(sib2);
    lv_obj_set_size(child2, 40, 100);
    simple_style(child2);

    lv_obj_update_layout(lv_screen_active());

    int32_t sib1_height = lv_obj_get_height(sib1);
    int32_t sib2_height = lv_obj_get_height(sib2);
    int32_t parent_height = lv_obj_get_height(parent);

    TEST_ASSERT_EQUAL_INT32(60, sib1_height);
    TEST_ASSERT_EQUAL_INT32(100, sib2_height);
    /* Parent height should be the max of siblings (the taller one) */
    TEST_ASSERT_EQUAL_INT32(100, parent_height);
}

/**
 * Test: SIZE_CONTENT with mixed fixed and auto-sized children
 */
void test_flex_size_content_mixed_children(void)
{
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_set_size(parent, 300, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    simple_style(parent);

    /* SIZE_CONTENT child */
    lv_obj_t * auto_child = lv_obj_create(parent);
    lv_obj_set_size(auto_child, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(auto_child, LV_FLEX_FLOW_ROW);
    simple_style(auto_child);

    lv_obj_t * inner = lv_obj_create(auto_child);
    lv_obj_set_size(inner, 50, 40);
    simple_style(inner);

    /* Fixed-size child */
    lv_obj_t * fixed_child = lv_obj_create(parent);
    lv_obj_set_size(fixed_child, 100, 80);
    simple_style(fixed_child);

    lv_obj_update_layout(lv_screen_active());

    int32_t auto_height = lv_obj_get_height(auto_child);
    int32_t parent_height = lv_obj_get_height(parent);

    TEST_ASSERT_EQUAL_INT32(40, auto_height);
    /* Parent should be sum of children heights: 40 + 80 = 120 */
    TEST_ASSERT_EQUAL_INT32(120, parent_height);
}

#endif
