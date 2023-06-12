/**
* @file lv_rb.c
*
*/

/*********************
 *      INCLUDES
 *********************/
#include "lv_rb.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_rb_node_t * rb_create_node(lv_rb_t * tree, void * key, void * data);
static void rb_right_rotate(lv_rb_t * tree, lv_rb_node_t * node);
static void rb_left_rotate(lv_rb_t * tree, lv_rb_node_t * node);
static lv_rb_node_t * lv_rb_minimum(lv_rb_t * tree, struct lv_rb_node_t * pNode);
static void rb_insert_color(lv_rb_t * tree, lv_rb_node_t * node);
static void rb_delete_color(lv_rb_t * tree, lv_rb_node_t * pNode, lv_rb_node_t * pNode1);

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

lv_rb_t * lv_rb_create(lv_rb_compare_t compare)
{
    lv_rb_t * tree = lv_malloc(sizeof(lv_rb_t));
    LV_ASSERT_NULL(tree);
    LV_ASSERT_NULL(compare);
    lv_memzero(tree, sizeof(lv_rb_t));

    if(tree == NULL || compare == NULL) {
        return NULL;
    }

    tree->root = NULL;
    tree->compare = compare;

    return tree;
}
bool lv_rb_insert(lv_rb_t * tree, void * key, void * data)
{
    LV_ASSERT_NULL(tree);
    if(tree == NULL) {
        return false;
    }

    lv_rb_node_t * node = rb_create_node(tree, key, data);
    LV_ASSERT_NULL(node);

    if(node == NULL) {
        return false;
    }

    if(tree->root == NULL) {
        tree->root = node;
        node->parent = NULL;
        node->color = LV_RB_COLOR_BLACK;
        return true;
    }

    lv_rb_node_t * parent = NULL;
    lv_rb_node_t * current = tree->root;

    while(current != NULL) {
        parent = current;

        if(tree->compare(key, current->key) < 0) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }

    node->parent = parent;

    if(tree->compare(key, parent->key) < 0) {
        parent->left = node;
    }
    else {
        parent->right = node;
    }

    rb_insert_color(tree, node);

    return true;
}
void * lv_rb_search(lv_rb_t * tree, const void * key)
{
    LV_ASSERT_NULL(tree);
    LV_ASSERT_NULL(key);

    if(tree == NULL || key == NULL) {
        return NULL;
    }

    lv_rb_node_t * current = tree->root;

    while(current != NULL) {
        int cmp = tree->compare(key, current->key);

        if(cmp == 0) {
            return current->data;
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
void lv_rb_delete(lv_rb_t * tree, const void * key)
{
    LV_ASSERT_NULL(tree);
    if(tree == NULL) {
        return;
    }

    lv_rb_node_t * node = tree->root;

    while(node != NULL) {
        int cmp = tree->compare(key, node->key);

        if(cmp == 0) {
            break;
        }
        else if(cmp < 0) {
            node = node->left;
        }
        else {
            node = node->right;
        }
    }


    if(node == NULL) {
        return;
    }

    lv_rb_node_t * child = NULL;
    lv_rb_node_t * parent = NULL;
    lv_rb_color_t color = LV_RB_COLOR_BLACK;

    if(node->left != NULL && node->right != NULL) {
        lv_rb_node_t * replace = node;
        replace = replace->right;
        while(replace->left != NULL) {
            replace = replace->left;
        }

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

        lv_free(node);
        return;
    }

    if(node->left != NULL) {
        child = node->left;
    }
    else {
        child = node->right;
    }

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

    lv_free(node);
}
void lv_rb_destroy(lv_rb_t * tree)
{
    LV_ASSERT_NULL(tree);

    if(tree == NULL) {
        return;
    }

    lv_rb_node_t * current = tree->root;
    lv_rb_node_t * parent = NULL;

    while(current != NULL) {
        if(current->left != NULL) {
            lv_rb_node_t * left = current->left;
            current->left = parent;

            parent = current;
            current = left;
        }
        else if(current->right != NULL) {
            lv_rb_node_t * right = current->right;
            current->right = parent;

            parent = current;
            current = right;
        }
        else {
            lv_free(current);
            current = parent;
            if(current != NULL) {
                if(current->left != NULL) {
                    current->left = NULL;
                }
                else {
                    current->right = NULL;
                }
            }
            parent = NULL;
        }
    }
    lv_free(tree);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_rb_node_t * rb_create_node(lv_rb_t * tree, void * key, void * data)
{
    lv_rb_node_t * node = lv_malloc(sizeof(lv_rb_node_t));
    LV_ASSERT_MALLOC(node);
    if(node == NULL) {
        return NULL;
    }

    node->key = key;
    node->data = data;
    node->color = LV_RB_COLOR_RED;
    node->left = NULL;
    node->right = NULL;

    return node;
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

static lv_rb_node_t * lv_rb_minimum(lv_rb_t * tree, struct lv_rb_node_t * pNode)
{
    while(pNode->left != NULL) {
        pNode = pNode->left;
    }

    return pNode;
}

static lv_rb_node_t * lv_rb_maximum(lv_rb_t * tree, struct lv_rb_node_t * pNode)
{
    while(pNode->right != NULL) {
        pNode = pNode->right;
    }

    return pNode;
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
static void rb_delete_color(lv_rb_t * tree, lv_rb_node_t * pNode, lv_rb_node_t * pNode1)
{
    LV_ASSERT_NULL(tree);
    if(tree == NULL) {
        return;
    }

    while((pNode == NULL || pNode->color == LV_RB_COLOR_BLACK) && pNode != tree->root) {
        if(pNode1->left == pNode) {
            lv_rb_node_t * pNode2 = pNode1->right;
            if(pNode2->color == LV_RB_COLOR_RED) {
                pNode2->color = LV_RB_COLOR_BLACK;
                pNode1->color = LV_RB_COLOR_RED;
                rb_left_rotate(tree, pNode1);
                pNode2 = pNode1->right;
            }

            if((pNode2->left == NULL || pNode2->left->color == LV_RB_COLOR_BLACK) && (pNode2->right == NULL ||
                                                                                      pNode2->right->color == LV_RB_COLOR_BLACK)) {
                pNode2->color = LV_RB_COLOR_RED;
                pNode = pNode1;
                pNode1 = pNode1->parent;
            }
            else {
                if(pNode2->right == NULL || pNode2->right->color == LV_RB_COLOR_BLACK) {
                    pNode2->left->color = LV_RB_COLOR_BLACK;
                    pNode2->color = LV_RB_COLOR_RED;
                    rb_right_rotate(tree, pNode2);
                    pNode2 = pNode1->right;
                }
                pNode2->color = pNode1->color;
                pNode1->color = LV_RB_COLOR_BLACK;
                pNode2->right->color = LV_RB_COLOR_BLACK;
                rb_left_rotate(tree, pNode1);
                pNode = tree->root;
                break;
            }
        }
        else {
            lv_rb_node_t * pNode2 = pNode1->left;
            if(pNode2->color == LV_RB_COLOR_RED) {
                pNode2->color = LV_RB_COLOR_BLACK;
                pNode1->color = LV_RB_COLOR_RED;
                rb_right_rotate(tree, pNode1);
                pNode2 = pNode1->left;
            }

            if((pNode2->left == NULL || pNode2->left->color == LV_RB_COLOR_BLACK) && (pNode2->right == NULL ||
                                                                                      pNode2->right->color == LV_RB_COLOR_BLACK)) {
                pNode2->color = LV_RB_COLOR_RED;
                pNode = pNode1;
                pNode1 = pNode1->parent;
            }
            else {
                if(pNode2->left == NULL || pNode2->left->color == LV_RB_COLOR_BLACK) {
                    pNode2->right->color = LV_RB_COLOR_BLACK;
                    pNode2->color = LV_RB_COLOR_RED;
                    rb_left_rotate(tree, pNode2);
                    pNode2 = pNode1->left;
                }
                pNode2->color = pNode1->color;
                pNode1->color = LV_RB_COLOR_BLACK;
                pNode2->left->color = LV_RB_COLOR_BLACK;
                rb_right_rotate(tree, pNode1);
                pNode = tree->root;
                break;
            }
        }
    }
    if(pNode != NULL)
        pNode->color = LV_RB_COLOR_BLACK;
}
