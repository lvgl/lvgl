/**
 * @file lv_tree.c
 * Tree.
 * The nodes are dynamically allocated by the 'lv_mem' module,
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_tree.h"
#include "../stdlib/lv_mem.h"
#include "../stdlib/lv_string.h"

#include "lv_assert.h"

/*********************
 *      DEFINES
 *********************/
#define INIT_CHILDREN_CAP 4

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_tree_class_t lv_tree_node_class = {
    .base_class = NULL,
    .instance_size = sizeof(lv_tree_node_t),
    .constructor_cb = NULL,
    .destructor_cb = NULL,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static void _lv_tree_node_construct(const lv_tree_class_t * class_p, lv_tree_node_t * node)
{
    if(node->class_p->base_class) {
        const lv_tree_class_t * original_class_p = node->class_p;

        node->class_p = node->class_p->base_class;

        _lv_tree_node_construct(class_p, node);

        node->class_p = original_class_p;
    }

    if(node->class_p->constructor_cb) node->class_p->constructor_cb(class_p, node);
}

static void _lv_tree_node_destruct(lv_tree_node_t * node)
{
    if(node->class_p->destructor_cb) node->class_p->destructor_cb(node->class_p, node);

    if(node->class_p->base_class) {
        node->class_p = node->class_p->base_class;

        _lv_tree_node_destruct(node);
    }
}

static uint32_t get_instance_size(const lv_tree_class_t * class_p)
{
    const lv_tree_class_t * base = class_p;
    while(base && base->instance_size == 0)
        base = base->base_class;

    return base->instance_size;
}

static lv_tree_node_t * _lv_tree_class_create_node(const lv_tree_class_t * class_p, lv_tree_node_t * parent)
{
    uint32_t s = get_instance_size(class_p);
    lv_tree_node_t * node = lv_malloc(s);
    if(node == NULL) return NULL;
    lv_memzero(node, s);
    node->class_p = class_p;
    node->parent = parent;
    node->child_cap = INIT_CHILDREN_CAP;
    node->children = lv_malloc(sizeof(lv_tree_node_t *) * node->child_cap);
    if(parent != NULL) {
        parent->child_cnt++;
        if(parent->child_cnt == parent->child_cap) {
            parent->child_cap <<= 1;
            parent->children = lv_realloc(parent->children, sizeof(lv_tree_node_t *) * parent->child_cap);
        }
        parent->children[parent->child_cnt - 1] = node;
    }
    return node;
}

lv_tree_node_t * lv_tree_node_create(const lv_tree_class_t * class_p, lv_tree_node_t * parent)
{
    lv_tree_node_t * node = _lv_tree_class_create_node(class_p, parent);
    LV_ASSERT_NULL(node);
    _lv_tree_node_construct(node->class_p, node);
    return node;
}

static bool _lv_tree_node_destructor_cb(const lv_tree_node_t * n, void * user_data)
{
    LV_UNUSED(user_data);
    if(n) {
        lv_tree_node_t * node = (lv_tree_node_t *)n;
        _lv_tree_node_destruct(node);
        lv_free(node->children);
        lv_free(node);
    }
    return true;
}

void lv_tree_node_delete(lv_tree_node_t * node)
{
    if(node) {
        if(node->parent) {
            /* Remove from parent */
            lv_tree_node_t * parent = node->parent;
            for(uint32_t i = 0; i < parent->child_cnt; i++) {
                if(parent->children[i] == node) {
                    parent->children[i] = NULL;
                }
            }
        }
        lv_tree_walk(node, LV_TREE_WALK_POST_ORDER, _lv_tree_node_destructor_cb, NULL, NULL, NULL);
    }
}

bool lv_tree_walk(const lv_tree_node_t * node,
                  lv_tree_walk_mode_t mode,
                  lv_tree_traverse_cb_t cb,
                  lv_tree_before_cb_t bcb,
                  lv_tree_after_cb_t acb,
                  void * user_data)
{
    if(node && cb) {
        if(mode == LV_TREE_WALK_PRE_ORDER) {
            if(bcb && !bcb(node, user_data)) {
                return false;
            }
            if(!cb(node, user_data)) {
                return false;
            }
        }
        for(uint32_t i = 0; i < node->child_cnt; i++) {
            if(!lv_tree_walk(node->children[i], mode, cb, bcb, acb, user_data)) {
                return false;
            }
        }
        if(mode == LV_TREE_WALK_PRE_ORDER) {
            if(acb) {
                acb(node, user_data);
            }
        }

        if(mode == LV_TREE_WALK_POST_ORDER) {
            if(bcb && !bcb(node, user_data)) {
                return false;
            }
            if(!cb(node, user_data)) {
                return false;
            }
            if(acb) {
                acb(node, user_data);
            }
            return true;
        }
        else {
            return true;
        }
    }
    else {
        return true;
    }
}

