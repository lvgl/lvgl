#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

#define MY_CLASS    &lv_test_tree_class

typedef struct {
    lv_tree_node_t base;
    int data1;
    float data2;
} lv_test_node_t;

static void lv_test_constructor(const lv_tree_class_t * class_p, lv_tree_node_t * node)
{
    LV_UNUSED(class_p);
    lv_test_node_t * t = (lv_test_node_t *)node;
    t->data1 = 100;
    t->data2 = 100.0f;
}

static void lv_test_destructor(const lv_tree_class_t * class_p, lv_tree_node_t * node)
{
    LV_UNUSED(class_p);
    lv_test_node_t * t = (lv_test_node_t *)node;
    t->data1 = 0;
    t->data2 = 0.0f;
}

const lv_tree_class_t lv_test_tree_class = {
    .base_class = &lv_tree_node_class,
    .instance_size = sizeof(lv_test_node_t),
    .constructor_cb = lv_test_constructor,
    .destructor_cb = lv_test_destructor,
};

static lv_test_node_t * lv_test_create(lv_test_node_t * parent)
{
    lv_tree_node_t * node = lv_tree_node_create(MY_CLASS, (lv_tree_node_t *)parent);
    return (lv_test_node_t *)node;
}

static void lv_test_delete(lv_test_node_t * node)
{
    lv_tree_node_delete((lv_tree_node_t *)node);
}

static lv_test_node_t * test;

void setUp(void)
{
    test = lv_test_create(NULL);
}

void tearDown(void)
{
    lv_test_delete(test);
}

void testCreateAndDelete(void)
{
    lv_test_node_t * node = lv_test_create(test);
    TEST_ASSERT_EQUAL_UINT32(1, test->base.child_cnt);
    TEST_ASSERT_EQUAL_UINT32(0, node->base.child_cnt);
    TEST_ASSERT_EQUAL(test, node->base.parent);
    TEST_ASSERT_EQUAL(NULL, test->base.parent);

    TEST_ASSERT_EQUAL_INT32(100, node->data1);
    TEST_ASSERT_EQUAL_FLOAT(100.0f, node->data2);

    lv_test_delete(node);
    TEST_ASSERT_EQUAL(NULL, test->base.children[0]);
}

struct _result {
    int32_t result[8];
    int32_t num;
};

static bool test_walk_cb(const lv_tree_node_t * node, void * data)
{
    lv_test_node_t * n = (lv_test_node_t *)node;
    struct _result * ret = (struct _result *)data;

    ret->result[ret->num++] = n->data1;
    return true;
}

void testBuildTree(void)
{
    lv_test_node_t * node = lv_test_create(test);
    node->data1 = 0;

    for(int i = 0; i < 16; i++) {
        lv_test_node_t * snode = lv_test_create(node);
        snode->data1 = i;
        TEST_ASSERT_NOT_EQUAL(NULL, snode);
    }
    TEST_ASSERT_EQUAL(16, node->base.child_cnt);
    lv_test_delete(node);
}

void testWalkTree(void)
{
    lv_test_node_t * node = lv_test_create(test);
    node->data1 = 1;

    lv_test_node_t * node1 = lv_test_create(node);
    node1->data1 = 2;
    lv_test_node_t * node2 = lv_test_create(node);
    node2->data1 = 3;

    lv_test_node_t * node11 = lv_test_create(node1);
    node11->data1 = 4;

    lv_test_node_t * node12 = lv_test_create(node1);
    node12->data1 = 5;

    lv_test_node_t * node21 = lv_test_create(node2);
    node21->data1 = 6;

    lv_test_node_t * node111 = lv_test_create(node11);
    node111->data1 = 7;

    struct _result result = {.num = 0};

    lv_tree_walk((lv_tree_node_t *)node, LV_TREE_WALK_POST_ORDER, test_walk_cb, NULL, NULL, &result);

    int32_t ret[8] = {7, 4, 5, 2, 6, 3, 1};

    TEST_ASSERT_EQUAL_INT32_ARRAY(ret, result.result, 8);

    result.num = 0;
    lv_memset(&result, 0, sizeof(struct _result));
    lv_tree_walk((lv_tree_node_t *)node, LV_TREE_WALK_PRE_ORDER, test_walk_cb, NULL, NULL, &result);

    int32_t ret2[8] = {1, 2, 4, 7, 5, 3, 6};

    TEST_ASSERT_EQUAL_INT32_ARRAY(ret2, result.result, 8);

    lv_test_delete(node111);

    result.num = 0;
    lv_memset(&result, 0, sizeof(struct _result));
    lv_tree_walk((lv_tree_node_t *)node, LV_TREE_WALK_PRE_ORDER, test_walk_cb, NULL, NULL, &result);

    int32_t ret3[8] = {1, 2, 4, 5, 3, 6};

    TEST_ASSERT_EQUAL_INT32_ARRAY(ret3, result.result, 8);

    lv_test_delete(node);
}

static bool test_search_cb(const lv_tree_node_t * node, void * data)
{
    lv_test_node_t * n = (lv_test_node_t *)node;
    struct _result * ret = (struct _result *)data;

    if(n->data1 == 3) {
        return false;
    }

    ret->result[ret->num++] = n->data1;
    return true;
}

static bool test_before_search_cb(const lv_tree_node_t * node, void * data)
{
    LV_UNUSED(node);
    LV_UNUSED(data);
    return true;
}

static void test_after_search_cb(const lv_tree_node_t * node, void * data)
{
    LV_UNUSED(node);
    LV_UNUSED(data);
}

void testTreeSearch(void)
{
    lv_test_node_t * node = lv_test_create(test);
    node->data1 = 1;

    lv_test_node_t * node1 = lv_test_create(node);
    node1->data1 = 2;
    lv_test_node_t * node2 = lv_test_create(node);
    node2->data1 = 3;

    lv_test_node_t * node11 = lv_test_create(node1);
    node11->data1 = 4;

    struct _result result = {.num = 0};

    lv_tree_walk((lv_tree_node_t *)node, LV_TREE_WALK_PRE_ORDER, test_search_cb, test_before_search_cb,
                 test_after_search_cb, &result);

    int32_t ret2[4] = {1, 2};

    TEST_ASSERT_EQUAL_INT32_ARRAY(ret2, result.result, 2);

    result.num = 0;
    lv_memset(&result, 0, sizeof(struct _result));
    lv_tree_walk((lv_tree_node_t *)node, LV_TREE_WALK_POST_ORDER, test_search_cb, test_before_search_cb,
                 test_after_search_cb, &result);

    int32_t ret3[4] = {4, 2};

    TEST_ASSERT_EQUAL_INT32_ARRAY(ret3, result.result, 2);
    lv_test_delete(node);
}

#endif
