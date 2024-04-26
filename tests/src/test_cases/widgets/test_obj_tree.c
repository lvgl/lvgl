#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void test_obj_tree_1(void);
void test_obj_tree_2(void);

void test_obj_tree_1(void)
{
    TEST_ASSERT_EQUAL(lv_obj_get_child_count(lv_screen_active()), 0);
}

void test_obj_tree_2(void)
{

    lv_obj_create(lv_screen_active());
    lv_obj_t * o2 = lv_obj_create(lv_screen_active());
    lv_obj_create(lv_screen_active());
    TEST_ASSERT_EQUAL(lv_obj_get_child_count(lv_screen_active()), 3);

    lv_obj_delete(o2);
    TEST_ASSERT_EQUAL(lv_obj_get_child_count(lv_screen_active()), 2);

    lv_obj_clean(lv_screen_active());
    TEST_ASSERT_EQUAL(lv_obj_get_child_count(lv_screen_active()), 0);

    lv_color_t c1 = lv_color_hex(0x444444);
    lv_color_t c2 = lv_color_hex3(0x444);
    TEST_ASSERT_EQUAL_COLOR(c1, c2);

    lv_obj_remove_style_all(lv_screen_active());
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x112233), 0);
    lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_COVER, 0);

    //TEST_ASSERT_EQUAL_SCREENSHOT("widgets/scr1.png")
}

void test_obj_tree_3(void)
{
    /* tests lv_obj_swap */
    lv_obj_t * parent1 = lv_obj_create(lv_screen_active());
    lv_obj_t * parent2 = lv_obj_create(lv_screen_active());
    lv_obj_t * child1 = lv_obj_create(parent1);
    lv_obj_t * child2 = lv_obj_create(parent2);

    /* were the parents set correctly for the children? */
    lv_obj_t * child1_parent_before = lv_obj_get_parent(child1);
    lv_obj_t * child2_parent_before = lv_obj_get_parent(child2);

    TEST_ASSERT_EQUAL(child1_parent_before, parent1);
    TEST_ASSERT_EQUAL(child2_parent_before, parent2);

    /* were the children set correctly for the parents? */
    TEST_ASSERT_EQUAL(lv_obj_get_child_count(parent1), 1);
    TEST_ASSERT_EQUAL(lv_obj_get_index(child1), 0);
    TEST_ASSERT_EQUAL(lv_obj_get_child(parent1, 0), child1);

    TEST_ASSERT_EQUAL(lv_obj_get_child_count(parent2), 1);
    TEST_ASSERT_EQUAL(lv_obj_get_index(child2), 0);
    TEST_ASSERT_EQUAL(lv_obj_get_child(parent2, 0), child2);

    /* swap the children */
    lv_obj_swap(child1, child2);

    /* test for properly swapped parents */
    lv_obj_t * child1_parent_after = lv_obj_get_parent(child1);
    lv_obj_t * child2_parent_after = lv_obj_get_parent(child2);

    TEST_ASSERT_EQUAL(child1_parent_after, parent2);
    TEST_ASSERT_EQUAL(child2_parent_after, parent1);

    /* test for correctly set children */
    TEST_ASSERT_EQUAL(lv_obj_get_child_count(parent1), 1);
    TEST_ASSERT_EQUAL(lv_obj_get_index(child2), 0);
    TEST_ASSERT_EQUAL(lv_obj_get_child(parent1, 0), child2);

    TEST_ASSERT_EQUAL(lv_obj_get_child_count(parent2), 1);
    TEST_ASSERT_EQUAL(lv_obj_get_index(child1), 0);
    TEST_ASSERT_EQUAL(lv_obj_get_child(parent2, 0), child1);
}

/** lv_obj_move_to_index **/

void test_obj_move_to_index_move_to_the_background(void)
{
    lv_obj_t * parent = NULL;
    lv_obj_t * child1 = NULL;
    lv_obj_t * child2 = NULL;

    parent = lv_obj_create(lv_screen_active());
    /* index is 0 */
    child1 = lv_obj_create(parent);
    /* index is 1 */
    child2 = lv_obj_create(parent);

    lv_obj_move_to_index(child2, 0);

    TEST_ASSERT_EQUAL(1, lv_obj_get_index(child1));
    TEST_ASSERT_EQUAL(0, lv_obj_get_index(child2));
}

void test_obj_move_to_index_move_forward(void)
{
    lv_obj_t * parent = NULL;
    lv_obj_t * child1 = NULL;
    lv_obj_t * child2 = NULL;

    parent = lv_obj_create(lv_screen_active());
    /* index is 0 */
    child1 = lv_obj_create(parent);
    /* index is 1 */
    child2 = lv_obj_create(parent);

    lv_obj_move_to_index(child1, lv_obj_get_index(child1) - 1);

    TEST_ASSERT_EQUAL(1, lv_obj_get_index(child1));
    TEST_ASSERT_EQUAL(0, lv_obj_get_index(child2));
}

/* Tests scenarios when no operation is performed */
void test_obj_move_to_index_no_operation_when_parent_is_null(void)
{
    lv_obj_t * parent = NULL;
    lv_obj_t * child1 = NULL;

    /* index is 0 */
    child1 = lv_obj_create(parent);

    lv_obj_move_to_index(child1, 0);

    TEST_ASSERT_EQUAL_INT32(0xFFFFFFFF, lv_obj_get_index(child1));
}

void test_obj_move_to_index_no_operation_when_index_is_same_or_bigger_than_parent_child_count(void)
{
    lv_obj_t * parent = NULL;
    lv_obj_t * child1 = NULL;

    parent = lv_obj_create(lv_screen_active());
    /* index is 0 */
    child1 = lv_obj_create(parent);

    lv_obj_move_to_index(child1, 3U);

    TEST_ASSERT_EQUAL(0, lv_obj_get_index(child1));
}

void test_obj_move_to_index_no_operation_when_new_index_is_the_same_as_previous_index(void)
{
    lv_obj_t * parent = NULL;
    lv_obj_t * child1 = NULL;
    lv_obj_t * child2 = NULL;

    parent = lv_obj_create(lv_screen_active());
    /* index is 0 */
    child1 = lv_obj_create(parent);
    /* index is 1 */
    child2 = lv_obj_create(parent);

    lv_obj_move_to_index(child2, 1U);

    TEST_ASSERT_EQUAL(0, lv_obj_get_index(child1));
    TEST_ASSERT_EQUAL(1, lv_obj_get_index(child2));
}

void test_obj_move_to_index_no_operation_when_requested_negative_index_is_greater_than_child_count(void)
{
    lv_obj_t * parent = NULL;
    lv_obj_t * child1 = NULL;
    lv_obj_t * child2 = NULL;

    parent = lv_obj_create(lv_screen_active());
    /* index is 0 */
    child1 = lv_obj_create(parent);
    /* index is 1 */
    child2 = lv_obj_create(parent);

    lv_obj_move_to_index(child1, -4);

    TEST_ASSERT_EQUAL(0, lv_obj_get_index(child1));
    TEST_ASSERT_EQUAL(1, lv_obj_get_index(child2));
}

#endif
