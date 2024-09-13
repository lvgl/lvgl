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
#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/

#define LV_TREE_NODE(n) ((lv_tree_node_t*)(n))

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

typedef bool (*lv_tree_traverse_cb_t)(const lv_tree_node_t * node, void * user_data);
typedef bool (*lv_tree_before_cb_t)(const lv_tree_node_t * node, void * user_data);
typedef void (*lv_tree_after_cb_t)(const lv_tree_node_t * node, void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

extern const lv_tree_class_t lv_tree_node_class;

/**
 * @brief Create a tree node
 * @param class_p pointer to a class of the node
 * @param parent pointer to the parent node (or NULL if it's the root node)
 * @return pointer to the new node
 */
lv_tree_node_t * lv_tree_node_create(const lv_tree_class_t * class_p, lv_tree_node_t * parent);

/**
 * @brief Delete a tree node and all its children recursively
 * @param node pointer to the node to delete
 */
void lv_tree_node_delete(lv_tree_node_t * node);

/**
 * @brief Walk the tree recursively and call a callback function on each node
 * @param node pointer to the root node of the tree
 * @param mode LV_TREE_WALK_PRE_ORDER or LV_TREE_WALK_POST_ORDER
 * @param cb callback function to call on each node
 * @param bcb callback function to call before visiting a node
 * @param acb callback function to call after visiting a node
 * @param user_data user data to pass to the callback functions
 * @return true: traversal is finished; false: traversal breaked
 */
bool lv_tree_walk(const lv_tree_node_t * node,
                  lv_tree_walk_mode_t mode,
                  lv_tree_traverse_cb_t cb,
                  lv_tree_before_cb_t bcb,
                  lv_tree_after_cb_t acb,
                  void * user_data);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
