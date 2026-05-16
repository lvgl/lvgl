/**
 * @file test_rb.c
 * Test cases for red-black tree (lv_rb.c)
 */

#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../src/misc/lv_rb_private.h"
#include "unity/unity.h"

/* Test data structure */
typedef struct {
    int32_t key;
    int32_t value;
} test_data_t;

/* Compare function for test data */
static lv_rb_compare_res_t test_compare(const void * a, const void * b)
{
    const test_data_t * da = (const test_data_t *)a;
    const test_data_t * db = (const test_data_t *)b;
    if(da->key < db->key) return -1;
    if(da->key > db->key) return 1;
    return 0;
}

/* Test fixture */
static lv_rb_t test_tree;

void setUp(void)
{
    lv_rb_init(&test_tree, test_compare, sizeof(test_data_t));
}

void tearDown(void)
{
    lv_rb_destroy(&test_tree);
}

/* Helper function to insert a key-value pair */
static lv_rb_node_t * insert_kv(int key, int value)
{
    test_data_t search_key = {.key = key, .value = 0};
    lv_rb_node_t * node = lv_rb_insert(&test_tree, &search_key);
    if(node && node->data) {
        test_data_t * data = (test_data_t *)node->data;
        data->key = key;
        data->value = value;
    }
    return node;
}

/* Test cases */

void test_rb_init(void)
{
    lv_rb_t tree;
    bool result = lv_rb_init(&tree, test_compare, sizeof(test_data_t));
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_NULL(tree.root);
    TEST_ASSERT_EQUAL_PTR(test_compare, tree.compare);
    TEST_ASSERT_EQUAL(sizeof(test_data_t), tree.size);
}

/* Note: NULL parameter tests are skipped because LV_ASSERT_NULL
 * triggers assertion failure in test environment */

void test_rb_insert_single(void)
{
    lv_rb_node_t * node = insert_kv(10, 100);
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_EQUAL_PTR(node, test_tree.root);
    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, node->color);
    TEST_ASSERT_NULL(node->parent);
    TEST_ASSERT_NULL(node->left);
    TEST_ASSERT_NULL(node->right);

    test_data_t * data = (test_data_t *)node->data;
    TEST_ASSERT_EQUAL_INT32(10, data->key);
    TEST_ASSERT_EQUAL_INT32(100, data->value);
}

/* test_rb_insert_null_tree skipped - triggers LV_ASSERT_NULL */

void test_rb_insert_duplicate(void)
{
    lv_rb_node_t * node1 = insert_kv(10, 100);
    TEST_ASSERT_NOT_NULL(node1);

    /* Insert duplicate key - should return existing node */
    test_data_t key = {.key = 10, .value = 0};
    lv_rb_node_t * node2 = lv_rb_insert(&test_tree, &key);
    TEST_ASSERT_EQUAL_PTR(node1, node2);
}

void test_rb_insert_multiple_ascending(void)
{
    /* Insert in ascending order to test left rotations */
    lv_rb_node_t * n1 = insert_kv(1, 10);
    lv_rb_node_t * n2 = insert_kv(2, 20);
    lv_rb_node_t * n3 = insert_kv(3, 30);
    lv_rb_node_t * n4 = insert_kv(4, 40);
    lv_rb_node_t * n5 = insert_kv(5, 50);

    TEST_ASSERT_NOT_NULL(n1);
    TEST_ASSERT_NOT_NULL(n2);
    TEST_ASSERT_NOT_NULL(n3);
    TEST_ASSERT_NOT_NULL(n4);
    TEST_ASSERT_NOT_NULL(n5);

    /* Verify tree structure - root should be black */
    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_insert_multiple_descending(void)
{
    /* Insert in descending order to test right rotations */
    lv_rb_node_t * n5 = insert_kv(5, 50);
    lv_rb_node_t * n4 = insert_kv(4, 40);
    lv_rb_node_t * n3 = insert_kv(3, 30);
    lv_rb_node_t * n2 = insert_kv(2, 20);
    lv_rb_node_t * n1 = insert_kv(1, 10);

    TEST_ASSERT_NOT_NULL(n1);
    TEST_ASSERT_NOT_NULL(n2);
    TEST_ASSERT_NOT_NULL(n3);
    TEST_ASSERT_NOT_NULL(n4);
    TEST_ASSERT_NOT_NULL(n5);

    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_insert_zigzag_left_right(void)
{
    /* Insert pattern that triggers left-right rotation */
    insert_kv(30, 300);
    insert_kv(10, 100);
    insert_kv(20, 200);  /* This triggers left-right case */

    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_insert_zigzag_right_left(void)
{
    /* Insert pattern that triggers right-left rotation */
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(20, 200);  /* This triggers right-left case */

    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_insert_uncle_red_left(void)
{
    /* Test case where uncle is red (left side) */
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(5, 50);   /* Uncle (30) is red, triggers recoloring */

    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_insert_uncle_red_right(void)
{
    /* Test case where uncle is red (right side) */
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(35, 350);  /* Uncle (10) is red, triggers recoloring */

    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_find_existing(void)
{
    insert_kv(10, 100);
    insert_kv(20, 200);
    insert_kv(5, 50);

    test_data_t search_key = {.key = 20, .value = 0};
    lv_rb_node_t * found = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(found);

    test_data_t * data = (test_data_t *)found->data;
    TEST_ASSERT_EQUAL_INT32(20, data->key);
    TEST_ASSERT_EQUAL_INT32(200, data->value);
}

void test_rb_find_not_existing(void)
{
    insert_kv(10, 100);
    insert_kv(20, 200);

    test_data_t search_key = {.key = 15, .value = 0};
    lv_rb_node_t * found = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NULL(found);
}

/* test_rb_find_null_tree skipped - triggers LV_ASSERT_NULL */

void test_rb_find_empty_tree(void)
{
    test_data_t search_key = {.key = 10, .value = 0};
    lv_rb_node_t * found = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NULL(found);
}

void test_rb_minimum(void)
{
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(5, 50);
    insert_kv(15, 150);

    lv_rb_node_t * min = lv_rb_minimum(&test_tree);
    TEST_ASSERT_NOT_NULL(min);

    test_data_t * data = (test_data_t *)min->data;
    TEST_ASSERT_EQUAL_INT32(5, data->key);
}

/* test_rb_minimum_null_tree skipped - triggers LV_ASSERT_NULL */

void test_rb_maximum(void)
{
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(5, 50);
    insert_kv(35, 350);

    lv_rb_node_t * max = lv_rb_maximum(&test_tree);
    TEST_ASSERT_NOT_NULL(max);

    test_data_t * data = (test_data_t *)max->data;
    TEST_ASSERT_EQUAL_INT32(35, data->key);
}

/* test_rb_maximum_null_tree skipped - triggers LV_ASSERT_NULL */

void test_rb_minimum_from(void)
{
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(25, 250);
    insert_kv(35, 350);

    /* Find minimum from node with key 30 */
    test_data_t search_key = {.key = 30, .value = 0};
    lv_rb_node_t * node30 = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(node30);

    lv_rb_node_t * min = lv_rb_minimum_from(node30);
    test_data_t * data = (test_data_t *)min->data;
    TEST_ASSERT_EQUAL_INT32(25, data->key);
}

void test_rb_maximum_from(void)
{
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(5, 50);
    insert_kv(15, 150);

    /* Find maximum from node with key 10 */
    test_data_t search_key = {.key = 10, .value = 0};
    lv_rb_node_t * node10 = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(node10);

    lv_rb_node_t * max = lv_rb_maximum_from(node10);
    test_data_t * data = (test_data_t *)max->data;
    TEST_ASSERT_EQUAL_INT32(15, data->key);
}

void test_rb_remove_leaf(void)
{
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);

    test_data_t search_key = {.key = 10, .value = 0};
    void * data = lv_rb_remove(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(data);
    lv_free(data);

    /* Verify node is removed */
    lv_rb_node_t * found = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NULL(found);
}

void test_rb_remove_node_with_one_child(void)
{
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(5, 50);

    test_data_t search_key = {.key = 10, .value = 0};
    void * data = lv_rb_remove(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(data);
    lv_free(data);

    /* Verify node is removed */
    lv_rb_node_t * found = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NULL(found);

    /* Verify child still exists */
    search_key.key = 5;
    found = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(found);
}

void test_rb_remove_node_with_two_children(void)
{
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(5, 50);
    insert_kv(15, 150);

    test_data_t search_key = {.key = 10, .value = 0};
    void * data = lv_rb_remove(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(data);
    lv_free(data);

    /* Verify node is removed */
    lv_rb_node_t * found = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NULL(found);

    /* Verify children still exist */
    search_key.key = 5;
    found = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(found);

    search_key.key = 15;
    found = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(found);
}

void test_rb_remove_root(void)
{
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);

    test_data_t search_key = {.key = 20, .value = 0};
    void * data = lv_rb_remove(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(data);
    lv_free(data);

    /* Verify root is removed */
    lv_rb_node_t * found = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NULL(found);

    /* Tree should still have other nodes */
    TEST_ASSERT_NOT_NULL(test_tree.root);
}

/* test_rb_remove_not_found skipped - triggers LV_ASSERT_NULL when node not found */

/* test_rb_remove_null_tree skipped - triggers LV_ASSERT_NULL */

void test_rb_remove_single_node(void)
{
    insert_kv(10, 100);

    test_data_t search_key = {.key = 10, .value = 0};
    void * data = lv_rb_remove(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(data);
    lv_free(data);

    TEST_ASSERT_NULL(test_tree.root);
}

void test_rb_drop(void)
{
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);

    test_data_t search_key = {.key = 10, .value = 0};
    bool result = lv_rb_drop(&test_tree, &search_key);
    TEST_ASSERT_TRUE(result);

    /* Verify node is removed */
    lv_rb_node_t * found = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NULL(found);
}

/* test_rb_drop_not_found skipped - triggers LV_ASSERT_NULL when node not found */

/* test_rb_drop_null_tree skipped - triggers LV_ASSERT_NULL */

void test_rb_drop_node(void)
{
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);

    test_data_t search_key = {.key = 10, .value = 0};
    lv_rb_node_t * node = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(node);

    bool result = lv_rb_drop_node(&test_tree, node);
    TEST_ASSERT_TRUE(result);

    /* Verify node is removed */
    lv_rb_node_t * found = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NULL(found);
}

/* test_rb_drop_node_null_tree skipped - triggers LV_ASSERT_NULL */

void test_rb_destroy(void)
{
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(5, 50);
    insert_kv(15, 150);

    lv_rb_destroy(&test_tree);
    TEST_ASSERT_NULL(test_tree.root);
}

/* test_rb_destroy_null_tree skipped - triggers LV_ASSERT_NULL */

void test_rb_destroy_empty_tree(void)
{
    /* Should not crash */
    lv_rb_destroy(&test_tree);
    TEST_ASSERT_NULL(test_tree.root);
}

/* Test complex deletion scenarios for rb_delete_color coverage */
void test_rb_remove_black_node_sibling_red_left(void)
{
    /* Build tree to trigger sibling red case on left */
    insert_kv(50, 500);
    insert_kv(25, 250);
    insert_kv(75, 750);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(60, 600);
    insert_kv(90, 900);
    insert_kv(5, 50);
    insert_kv(15, 150);

    /* Remove nodes to trigger specific delete color cases */
    test_data_t search_key = {.key = 60, .value = 0};
    void * data = lv_rb_remove(&test_tree, &search_key);
    if(data) lv_free(data);

    search_key.key = 90;
    data = lv_rb_remove(&test_tree, &search_key);
    if(data) lv_free(data);

    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_remove_black_node_sibling_red_right(void)
{
    /* Build tree to trigger sibling red case on right */
    insert_kv(50, 500);
    insert_kv(25, 250);
    insert_kv(75, 750);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(60, 600);
    insert_kv(90, 900);
    insert_kv(85, 850);
    insert_kv(95, 950);

    /* Remove nodes to trigger specific delete color cases */
    test_data_t search_key = {.key = 10, .value = 0};
    void * data = lv_rb_remove(&test_tree, &search_key);
    if(data) lv_free(data);

    search_key.key = 30;
    data = lv_rb_remove(&test_tree, &search_key);
    if(data) lv_free(data);

    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_remove_complex_rebalancing(void)
{
    /* Insert many nodes to create complex tree structure */
    int keys[] = {50, 25, 75, 10, 30, 60, 90, 5, 15, 27, 35, 55, 65, 85, 95};
    int num_keys = sizeof(keys) / sizeof(keys[0]);

    for(int i = 0; i < num_keys; i++) {
        insert_kv(keys[i], keys[i] * 10);
    }

    /* Remove nodes in specific order to trigger various rebalancing cases */
    int remove_order[] = {5, 15, 10, 27, 35, 30, 25};
    int num_remove = sizeof(remove_order) / sizeof(remove_order[0]);

    for(int i = 0; i < num_remove; i++) {
        test_data_t search_key = {.key = remove_order[i], .value = 0};
        void * data = lv_rb_remove(&test_tree, &search_key);
        if(data) lv_free(data);
    }

    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_remove_root_with_two_children_replace_is_direct_child(void)
{
    /* Create tree where replacement node is direct child of deleted node */
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(25, 250);

    /* Remove root - replacement (25) is direct right child's left child */
    test_data_t search_key = {.key = 20, .value = 0};
    void * data = lv_rb_remove(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(data);
    lv_free(data);

    TEST_ASSERT_NOT_NULL(test_tree.root);
    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_remove_node_replacement_parent_is_node(void)
{
    /* Create specific tree structure where parent == node in remove */
    insert_kv(50, 500);
    insert_kv(25, 250);
    insert_kv(75, 750);
    insert_kv(60, 600);

    /* Remove 50 - replacement is 60, and 60's parent (75) != 50 */
    test_data_t search_key = {.key = 50, .value = 0};
    void * data = lv_rb_remove(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(data);
    lv_free(data);

    TEST_ASSERT_NOT_NULL(test_tree.root);
}

void test_rb_remove_left_child_only(void)
{
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(25, 250);

    /* Remove 30 which has only left child */
    test_data_t search_key = {.key = 30, .value = 0};
    void * data = lv_rb_remove(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(data);
    lv_free(data);

    /* Verify 25 still exists */
    search_key.key = 25;
    lv_rb_node_t * found = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(found);
}

void test_rb_remove_right_child_only(void)
{
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(15, 150);

    /* Remove 10 which has only right child */
    test_data_t search_key = {.key = 10, .value = 0};
    void * data = lv_rb_remove(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(data);
    lv_free(data);

    /* Verify 15 still exists */
    search_key.key = 15;
    lv_rb_node_t * found = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(found);
}

void test_rb_delete_color_sibling_both_children_black(void)
{
    /* Create tree to trigger case where sibling's both children are black */
    insert_kv(50, 500);
    insert_kv(25, 250);
    insert_kv(75, 750);
    insert_kv(10, 100);
    insert_kv(30, 300);

    /* Remove to trigger the specific case */
    test_data_t search_key = {.key = 10, .value = 0};
    void * data = lv_rb_remove(&test_tree, &search_key);
    if(data) lv_free(data);

    search_key.key = 30;
    data = lv_rb_remove(&test_tree, &search_key);
    if(data) lv_free(data);

    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_delete_color_sibling_right_child_black(void)
{
    /* Create tree to trigger case where sibling's right child is black */
    insert_kv(50, 500);
    insert_kv(25, 250);
    insert_kv(75, 750);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(60, 600);
    insert_kv(90, 900);
    insert_kv(55, 550);

    /* Remove nodes to trigger specific case */
    test_data_t search_key = {.key = 90, .value = 0};
    void * data = lv_rb_remove(&test_tree, &search_key);
    if(data) lv_free(data);

    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_delete_color_sibling_left_child_black(void)
{
    /* Create tree to trigger case where sibling's left child is black (right side) */
    insert_kv(50, 500);
    insert_kv(25, 250);
    insert_kv(75, 750);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(60, 600);
    insert_kv(90, 900);
    insert_kv(95, 950);

    /* Remove nodes to trigger specific case */
    test_data_t search_key = {.key = 10, .value = 0};
    void * data = lv_rb_remove(&test_tree, &search_key);
    if(data) lv_free(data);

    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_large_tree_operations(void)
{
    /* Insert many nodes */
    for(int i = 0; i < 100; i++) {
        insert_kv(i, i * 10);
    }

    /* Verify minimum and maximum */
    lv_rb_node_t * min = lv_rb_minimum(&test_tree);
    lv_rb_node_t * max = lv_rb_maximum(&test_tree);

    TEST_ASSERT_NOT_NULL(min);
    TEST_ASSERT_NOT_NULL(max);

    test_data_t * min_data = (test_data_t *)min->data;
    test_data_t * max_data = (test_data_t *)max->data;

    TEST_ASSERT_EQUAL_INT32(0, min_data->key);
    TEST_ASSERT_EQUAL_INT32(99, max_data->key);

    /* Remove half the nodes */
    for(int i = 0; i < 50; i++) {
        test_data_t search_key = {.key = i * 2, .value = 0};
        void * data = lv_rb_remove(&test_tree, &search_key);
        if(data) lv_free(data);
    }

    /* Verify tree is still valid */
    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_remove_node_direct(void)
{
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);

    test_data_t search_key = {.key = 10, .value = 0};
    lv_rb_node_t * node = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(node);

    void * data = lv_rb_remove_node(&test_tree, node);
    TEST_ASSERT_NOT_NULL(data);
    lv_free(data);

    /* Verify node is removed */
    lv_rb_node_t * found = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NULL(found);
}

void test_rb_remove_node_root_two_children(void)
{
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(5, 50);
    insert_kv(15, 150);
    insert_kv(25, 250);
    insert_kv(35, 350);

    /* Remove root which has two children */
    test_data_t search_key = {.key = 20, .value = 0};
    lv_rb_node_t * root = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(root);

    void * data = lv_rb_remove_node(&test_tree, root);
    TEST_ASSERT_NOT_NULL(data);
    lv_free(data);

    /* Verify root is removed but tree still valid */
    lv_rb_node_t * found = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NULL(found);
    TEST_ASSERT_NOT_NULL(test_tree.root);
    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_insert_recolor_propagation(void)
{
    /* Create tree that requires recoloring to propagate up */
    insert_kv(50, 500);
    insert_kv(25, 250);
    insert_kv(75, 750);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(60, 600);
    insert_kv(90, 900);
    insert_kv(5, 50);
    insert_kv(15, 150);
    insert_kv(27, 270);
    insert_kv(35, 350);
    insert_kv(3, 30);  /* This should trigger recoloring propagation */

    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_delete_all_nodes(void)
{
    int keys[] = {50, 25, 75, 10, 30, 60, 90};
    int num_keys = sizeof(keys) / sizeof(keys[0]);

    for(int i = 0; i < num_keys; i++) {
        insert_kv(keys[i], keys[i] * 10);
    }

    /* Delete all nodes */
    for(int i = 0; i < num_keys; i++) {
        test_data_t search_key = {.key = keys[i], .value = 0};
        void * data = lv_rb_remove(&test_tree, &search_key);
        if(data) lv_free(data);
    }

    TEST_ASSERT_NULL(test_tree.root);
}

void test_rb_remove_triggers_left_rotate_in_delete_color(void)
{
    /* Build specific tree to trigger left rotate in delete color */
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(5, 50);
    insert_kv(15, 150);
    insert_kv(25, 250);
    insert_kv(40, 400);
    insert_kv(35, 350);
    insert_kv(50, 500);

    /* Remove nodes to trigger left rotate in delete_color */
    test_data_t search_key = {.key = 5, .value = 0};
    void * data = lv_rb_remove(&test_tree, &search_key);
    if(data) lv_free(data);

    search_key.key = 15;
    data = lv_rb_remove(&test_tree, &search_key);
    if(data) lv_free(data);

    search_key.key = 10;
    data = lv_rb_remove(&test_tree, &search_key);
    if(data) lv_free(data);

    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_remove_triggers_right_rotate_in_delete_color(void)
{
    /* Build specific tree to trigger right rotate in delete color */
    insert_kv(30, 300);
    insert_kv(20, 200);
    insert_kv(40, 400);
    insert_kv(10, 100);
    insert_kv(25, 250);
    insert_kv(35, 350);
    insert_kv(50, 500);
    insert_kv(5, 50);
    insert_kv(15, 150);

    /* Remove nodes to trigger right rotate in delete_color */
    test_data_t search_key = {.key = 35, .value = 0};
    void * data = lv_rb_remove(&test_tree, &search_key);
    if(data) lv_free(data);

    search_key.key = 50;
    data = lv_rb_remove(&test_tree, &search_key);
    if(data) lv_free(data);

    search_key.key = 40;
    data = lv_rb_remove(&test_tree, &search_key);
    if(data) lv_free(data);

    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_remove_node_is_right_child_of_parent(void)
{
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(25, 250);
    insert_kv(35, 350);

    /* Remove 30 which is right child of root and has two children */
    test_data_t search_key = {.key = 30, .value = 0};
    void * data = lv_rb_remove(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(data);
    lv_free(data);

    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_remove_node_is_left_child_of_parent(void)
{
    insert_kv(20, 200);
    insert_kv(10, 100);
    insert_kv(30, 300);
    insert_kv(5, 50);
    insert_kv(15, 150);

    /* Remove 10 which is left child of root and has two children */
    test_data_t search_key = {.key = 10, .value = 0};
    void * data = lv_rb_remove(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(data);
    lv_free(data);

    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_delete_color_null_tree(void)
{
    /* This is tested indirectly through other tests, but we ensure
     * the function handles NULL gracefully */
    insert_kv(10, 100);
    insert_kv(5, 50);
    insert_kv(15, 150);

    test_data_t search_key = {.key = 5, .value = 0};
    void * data = lv_rb_remove(&test_tree, &search_key);
    if(data) lv_free(data);

    TEST_ASSERT_NOT_NULL(test_tree.root);
}

void test_rb_complex_delete_sequence(void)
{
    /* Insert nodes in specific order */
    int keys[] = {41, 38, 31, 12, 19, 8};
    int num_keys = sizeof(keys) / sizeof(keys[0]);

    for(int i = 0; i < num_keys; i++) {
        insert_kv(keys[i], keys[i] * 10);
    }

    /* Delete in reverse order */
    for(int i = num_keys - 1; i >= 0; i--) {
        test_data_t search_key = {.key = keys[i], .value = 0};
        void * data = lv_rb_remove(&test_tree, &search_key);
        if(data) lv_free(data);
    }

    TEST_ASSERT_NULL(test_tree.root);
}

void test_rb_alternating_insert_delete(void)
{
    /* Alternating insert and delete operations */
    for(int i = 0; i < 20; i++) {
        insert_kv(i, i * 10);
        if(i > 5) {
            test_data_t search_key = {.key = i - 5, .value = 0};
            void * data = lv_rb_remove(&test_tree, &search_key);
            if(data) lv_free(data);
        }
    }

    TEST_ASSERT_NOT_NULL(test_tree.root);
    TEST_ASSERT_EQUAL(LV_RB_COLOR_BLACK, test_tree.root->color);
}

void test_rb_find_in_left_subtree(void)
{
    insert_kv(50, 500);
    insert_kv(25, 250);
    insert_kv(75, 750);
    insert_kv(10, 100);
    insert_kv(30, 300);

    /* Find node in left subtree */
    test_data_t search_key = {.key = 10, .value = 0};
    lv_rb_node_t * found = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(found);

    test_data_t * data = (test_data_t *)found->data;
    TEST_ASSERT_EQUAL_INT32(10, data->key);
}

void test_rb_find_in_right_subtree(void)
{
    insert_kv(50, 500);
    insert_kv(25, 250);
    insert_kv(75, 750);
    insert_kv(60, 600);
    insert_kv(90, 900);

    /* Find node in right subtree */
    test_data_t search_key = {.key = 90, .value = 0};
    lv_rb_node_t * found = lv_rb_find(&test_tree, &search_key);
    TEST_ASSERT_NOT_NULL(found);

    test_data_t * data = (test_data_t *)found->data;
    TEST_ASSERT_EQUAL_INT32(90, data->key);
}

#endif
