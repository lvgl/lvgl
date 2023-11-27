/**
 * @file lv_tree.h
 * Tree. The tree nodes are dynamically allocated by the 'lv_mem' module.
 */

#ifndef LV_TREE_H
#define LV_TREE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_tree_node_t;

/**
 * Describe the common methods of every object.
 * Similar to a C++ class.
 */
typedef struct _lv_tree_class_t {
    const struct _lv_tree_class_t * base_class;
    uint32_t instance_size;
    void (*constructor_cb)(const struct _lv_tree_class_t * class_p, struct _lv_tree_node_t * node);
    void (*destructor_cb)(const struct _lv_tree_class_t * class_p, struct _lv_tree_node_t * node);
} lv_tree_class_t;

/** Description of a tree node*/
typedef struct _lv_tree_node_t {
    struct _lv_tree_node_t * parent;
    struct _lv_tree_node_t ** children;
    uint32_t child_cnt;
    uint32_t child_cap;
    const struct _lv_tree_class_t * class_p;
} lv_tree_node_t;

enum {
    LV_TREE_WALK_PRE_ORDER = 0,
    LV_TREE_WALK_POST_ORDER,
};
typedef uint8_t lv_tree_walk_mode_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

extern const lv_tree_class_t lv_tree_node_class;

lv_tree_node_t * lv_tree_node_create(const lv_tree_class_t * class_p, lv_tree_node_t * parent);

void lv_tree_node_delete(lv_tree_node_t * node);

typedef bool (*traverse_cb)(const lv_tree_node_t * node, void * user_data);
typedef bool (*before_cb)(const lv_tree_node_t * node, void * user_data);
typedef void (*after_cb)(const lv_tree_node_t * node, void * user_data);

bool lv_tree_walk(const lv_tree_node_t * node, lv_tree_walk_mode_t mode, traverse_cb cb, before_cb bcb, after_cb acb, void * data);

/**********************
 *      MACROS
 **********************/
#define LV_TREE_NODE(n) ((lv_tree_node_t*)(n))

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
