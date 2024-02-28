/**
 * @file lv_rb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_rb.h"
#include "../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_rb_node_t * rb_create_node(lv_rb_t * tree);
static lv_rb_node_t * rb_find_leaf_parent(lv_rb_t * tree, lv_rb_node_t * node);
static void rb_right_rotate(lv_rb_t * tree, lv_rb_node_t * node);
static void rb_left_rotate(lv_rb_t * tree, lv_rb_node_t * node);
static void rb_insert_color(lv_rb_t * tree, lv_rb_node_t * node);
static void rb_delete_color(lv_rb_t * tree, lv_rb_node_t * node1, lv_rb_node_t * node2);

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_rb_init(lv_rb_t * tree, lv_rb_compare_t compare, size_t node_size)
{
    LV_ASSERT_NULL(tree);
    LV_ASSERT_NULL(compare);
    LV_ASSERT(node_size > 0);

    if(tree == NULL || compare == NULL || node_size == 0) {
        return false;
    }

    lv_memzero(tree, sizeof(lv_rb_t));

    tree->root = NULL;
    tree->compare = compare;
    tree->size = node_size;

    return true;
}

lv_rb_node_t * lv_rb_insert(lv_rb_t * tree, void * key)
{
    LV_ASSERT_NULL(tree);
    if(tree == NULL) {
        return NULL;
    }

    lv_rb_node_t * node = lv_rb_find(tree, key);
    if(node) return node;
    else {
        node = rb_create_node(tree);
        if(node == NULL) return NULL;

        if(tree->root == NULL) {
            tree->root = node;
            node->parent = NULL;
            node->color = LV_RB_COLOR_BLACK;
            return node;
        }
    }

    void * new_data = node->data;
    node->data = key;
    lv_rb_node_t * parent = rb_find_leaf_parent(tree, node);

    node->parent = parent;
    node->color = LV_RB_COLOR_RED;

    if(tree->compare(key, parent->data) < 0) parent->left = node;
    else parent->right = node;

    rb_insert_color(tree, node);

    node->data = new_data;
    return node;
}

lv_rb_node_t * lv_rb_find(lv_rb_t * tree, const void * key)
{
    LV_ASSERT_NULL(tree);
    if(tree == NULL) {
        return NULL;
    }

    lv_rb_node_t * current = tree->root;

    while(current != NULL) {
        lv_rb_compare_res_t cmp = tree->compare(key, current->data);

        if(cmp == 0) {
            return current;
        }
        else if(cmp < 0) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }

    return NULL;
}

void * lv_rb_remove_node(lv_rb_t * tree, lv_rb_node_t * node)
{
    lv_rb_node_t * child = NULL;
    lv_rb_node_t * parent = NULL;
    lv_rb_color_t color = LV_RB_COLOR_BLACK;

    if(node->left != NULL && node->right != NULL) {
        lv_rb_node_t * replace = node;
        replace = lv_rb_minimum_from(replace->right);

        if(node->parent != NULL) {
            if(node->parent->left == node) {
                node->parent->left = replace;
            }
            else {
                node->parent->right = replace;
            }
        }
        else {
            tree->root = replace;
        }

        child = replace->right;
        parent = replace->parent;
        color = replace->color;

        if(parent == node) {
            parent = replace;
        }
        else {
            if(child != NULL) {
                child->parent = parent;
            }
            parent->left = child;
            replace->right = node->right;
            node->right->parent = replace;
        }

        replace->parent = node->parent;
        replace->color = node->color;
        replace->left = node->left;
        node->left->parent = replace;

        if(color == LV_RB_COLOR_BLACK) {
            rb_delete_color(tree, child, parent);
        }

        void * data = node->data;
        lv_free(node);
        return data;
    }

    child = node->right != NULL ? node->right : node->left;
    parent = node->parent;
    color = node->color;

    if(child != NULL) {
        child->parent = parent;
    }

    if(parent != NULL) {
        if(parent->left == node) {
            parent->left = child;
        }
        else {
            parent->right = child;
        }
    }
    else {
        tree->root = child;
    }

    if(color == LV_RB_COLOR_BLACK) {
        rb_delete_color(tree, child, parent);
    }

    void * data = node->data;
    lv_free(node);
    return data;
}

void * lv_rb_remove(lv_rb_t * tree, const void * key)
{
    LV_ASSERT_NULL(tree);
    if(tree == NULL) {
        return NULL;
    }

    lv_rb_node_t * node = lv_rb_find(tree, key);
    LV_ASSERT_NULL(node);
    if(node == NULL) {
        LV_LOG_WARN("rb delete %d not found", (int)(uintptr_t)key);
        return NULL;
    }

    return lv_rb_remove_node(tree, node);
}

bool lv_rb_drop_node(lv_rb_t * tree, lv_rb_node_t * node)
{
    LV_ASSERT_NULL(tree);
    if(tree == NULL) {
        return false;
    }

    void * data = lv_rb_remove_node(tree, node);
    if(data) {
        lv_free(data);
        return true;
    }
    return false;
}

bool lv_rb_drop(lv_rb_t * tree, const void * key)
{
    LV_ASSERT_NULL(tree);
    if(tree == NULL) {
        return false;
    }

    void * data = lv_rb_remove(tree, key);
    if(data) {
        lv_free(data);
        return true;
    }
    return false;
}

void lv_rb_destroy(lv_rb_t * tree)
{
    LV_ASSERT_NULL(tree);

    if(tree == NULL) {
        return;
    }

    lv_rb_node_t * node = tree->root;
    lv_rb_node_t * parent = NULL;

    while(node != NULL) {
        if(node->left != NULL) {
            node = node->left;
        }
        else if(node->right != NULL) {
            node = node->right;
        }
        else {
            parent = node->parent;
            if(parent != NULL) {
                if(parent->left == node) {
                    parent->left = NULL;
                }
                else {
                    parent->right = NULL;
                }
            }
            lv_free(node->data);
            lv_free(node);
            node = parent;
        }
    }
    tree->root = NULL;
}

lv_rb_node_t * lv_rb_minimum(lv_rb_t * tree)
{
    LV_ASSERT_NULL(tree);
    if(tree == NULL) {
        return NULL;
    }
    return lv_rb_minimum_from(tree->root);
}

lv_rb_node_t * lv_rb_maximum(lv_rb_t * tree)
{
    LV_ASSERT_NULL(tree);
    if(tree == NULL) {
        return NULL;
    }
    return lv_rb_maximum_from(tree->root);
}

lv_rb_node_t * lv_rb_minimum_from(lv_rb_node_t * node)
{
    while(node->left != NULL) {
        node = node->left;
    }

    return node;
}

lv_rb_node_t * lv_rb_maximum_from(lv_rb_node_t * node)
{
    while(node->right != NULL) {
        node = node->right;
    }

    return node;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_rb_node_t * rb_create_node(lv_rb_t * tree)
{
    lv_rb_node_t * node = lv_malloc_zeroed(sizeof(lv_rb_node_t));
    LV_ASSERT_MALLOC(node);
    if(node == NULL) {
        return NULL;
    }

    node->data = lv_malloc_zeroed(tree->size);
    LV_ASSERT_MALLOC(node->data);
    if(node->data == NULL) {
        lv_free(node);
        return NULL;
    }

    node->color = LV_RB_COLOR_RED;
    node->left = NULL;
    node->right = NULL;

    return node;
}

static lv_rb_node_t * rb_find_leaf_parent(lv_rb_t * tree, lv_rb_node_t * node)
{
    lv_rb_node_t * current = tree->root;
    lv_rb_node_t * parent = current;

    while(current != NULL) {
        parent = current;

        if(tree->compare(node->data, current->data) < 0) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }

    return parent;
}

static void rb_right_rotate(lv_rb_t * tree, lv_rb_node_t * node)
{
    lv_rb_node_t * left = node->left;
    node->left = left->right;

    if(left->right != NULL) {
        left->right->parent = node;
    }

    left->parent = node->parent;

    if(node->parent == NULL) {
        tree->root = left;
    }
    else if(node == node->parent->right) {
        node->parent->right = left;
    }
    else {
        node->parent->left = left;
    }

    left->right = node;
    node->parent = left;
}

static void rb_left_rotate(lv_rb_t * tree, lv_rb_node_t * node)
{
    lv_rb_node_t * right = node->right;
    node->right = right->left;

    if(right->left != NULL) {
        right->left->parent = node;
    }

    right->parent = node->parent;

    if(node->parent == NULL) {
        tree->root = right;
    }
    else if(node == node->parent->left) {
        node->parent->left = right;
    }
    else {
        node->parent->right = right;
    }

    right->left = node;
    node->parent = right;
}

static void rb_insert_color(lv_rb_t * tree, lv_rb_node_t * node)
{
    lv_rb_node_t * parent = NULL;
    lv_rb_node_t * gparent = NULL;

    while((parent = node->parent) && parent->color == LV_RB_COLOR_RED) {
        gparent = parent->parent;

        if(parent == gparent->left) {
            {
                lv_rb_node_t * uncle = gparent->right;
                if(uncle && uncle->color == LV_RB_COLOR_RED) {
                    uncle->color = LV_RB_COLOR_BLACK;
                    parent->color = LV_RB_COLOR_BLACK;
                    gparent->color = LV_RB_COLOR_RED;
                    node = gparent;
                    continue;
                }
            }

            if(parent->right == node) {
                lv_rb_node_t * tmp;
                rb_left_rotate(tree, parent);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            parent->color = LV_RB_COLOR_BLACK;
            gparent->color = LV_RB_COLOR_RED;
            rb_right_rotate(tree, gparent);
        }
        else {
            {
                lv_rb_node_t * uncle = gparent->left;
                if(uncle && uncle->color == LV_RB_COLOR_RED) {
                    uncle->color = LV_RB_COLOR_BLACK;
                    parent->color = LV_RB_COLOR_BLACK;
                    gparent->color = LV_RB_COLOR_RED;
                    node = gparent;
                    continue;
                }
            }

            if(parent->left == node) {
                lv_rb_node_t * tmp;
                rb_right_rotate(tree, parent);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            parent->color = LV_RB_COLOR_BLACK;
            gparent->color = LV_RB_COLOR_RED;
            rb_left_rotate(tree, gparent);
        }
    }

    tree->root->color = LV_RB_COLOR_BLACK;
}

static void rb_delete_color(lv_rb_t * tree, lv_rb_node_t * node1, lv_rb_node_t * node2)
{
    LV_ASSERT_NULL(tree);
    if(tree == NULL) {
        return;
    }

    while((node1 == NULL || node1->color == LV_RB_COLOR_BLACK) && node1 != tree->root) {
        if(node2->left == node1) {
            lv_rb_node_t * pNode2 = node2->right;
            if(pNode2->color == LV_RB_COLOR_RED) {
                pNode2->color = LV_RB_COLOR_BLACK;
                node2->color = LV_RB_COLOR_RED;
                rb_left_rotate(tree, node2);
                pNode2 = node2->right;
            }

            if((pNode2->left == NULL || pNode2->left->color == LV_RB_COLOR_BLACK)
               && (pNode2->right == NULL || pNode2->right->color == LV_RB_COLOR_BLACK)) {
                pNode2->color = LV_RB_COLOR_RED;
                node1 = node2;
                node2 = node2->parent;
            }
            else {
                if(pNode2->right == NULL || pNode2->right->color == LV_RB_COLOR_BLACK) {
                    pNode2->left->color = LV_RB_COLOR_BLACK;
                    pNode2->color = LV_RB_COLOR_RED;
                    rb_right_rotate(tree, pNode2);
                    pNode2 = node2->right;
                }
                pNode2->color = node2->color;
                node2->color = LV_RB_COLOR_BLACK;
                pNode2->right->color = LV_RB_COLOR_BLACK;
                rb_left_rotate(tree, node2);
                node1 = tree->root;
                break;
            }
        }
        else {
            lv_rb_node_t * pNode2 = node2->left;
            if(pNode2->color == LV_RB_COLOR_RED) {
                pNode2->color = LV_RB_COLOR_BLACK;
                node2->color = LV_RB_COLOR_RED;
                rb_right_rotate(tree, node2);
                pNode2 = node2->left;
            }

            if((pNode2->left == NULL || pNode2->left->color == LV_RB_COLOR_BLACK)
               && (pNode2->right == NULL || pNode2->right->color == LV_RB_COLOR_BLACK)) {
                pNode2->color = LV_RB_COLOR_RED;
                node1 = node2;
                node2 = node2->parent;
            }
            else {
                if(pNode2->left == NULL || pNode2->left->color == LV_RB_COLOR_BLACK) {
                    pNode2->right->color = LV_RB_COLOR_BLACK;
                    pNode2->color = LV_RB_COLOR_RED;
                    rb_left_rotate(tree, pNode2);
                    pNode2 = node2->left;
                }
                pNode2->color = node2->color;
                node2->color = LV_RB_COLOR_BLACK;
                pNode2->left->color = LV_RB_COLOR_BLACK;
                rb_right_rotate(tree, node2);
                node1 = tree->root;
                break;
            }
        }
    }
    if(node1 != NULL)
        node1->color = LV_RB_COLOR_BLACK;
}
