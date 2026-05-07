/**
 * @file lv_ll.h
 *
 * Doubly-linked list that embeds user data directly inside each node allocation.
 *
 * ## Memory layout
 *
 * Unlike a textbook linked list that holds a `data` pointer inside each node,
 * `lv_ll` allocates a single flat block per node and places the user data at
 * the **beginning** of that block.  The `prev` / `next` bookkeeping pointers
 * are appended **after** the user data:
 *
 * @code
 * +---------------------------------+------------------+------------------+
 * |  user data  (n_size bytes,      |  prev  pointer   |  next  pointer   |
 * |  rounded up for alignment)      |  (sizeof ptr)    |  (sizeof ptr)    |
 * |  ^--- pointer returned to       |                  |                  |
 * |       the caller                |                  |                  |
 * +---------------------------------+------------------+------------------+
 *  offset 0                         offset n_size      offset n_size+ptrsize
 * @endcode
 *
 * Because the returned pointer already **is** the user data pointer, callers
 * only need a cast — no `->data` indirection is required.  This keeps usage
 * sites clean and avoids a level of indirection on every access:
 *
 * @code
 * my_item_t *item = lv_ll_ins_tail(&my_list);
 * item->x = 10;   // write directly, no ->data needed
 * @endcode
 *
 * Pointer arithmetic using the offsets defined by `LL_PREV_P_OFFSET` /
 * `LL_NEXT_P_OFFSET` (in lv_ll.c) is used internally to read and write the
 * `prev` / `next` fields without exposing them to callers.
 *
 * ## Ownership semantics
 *
 * `lv_ll` **owns the node allocation** but **not the resources referenced by
 * the data stored in each node**.
 *
 * * `lv_ll_ins_head` / `lv_ll_ins_prev` / `lv_ll_ins_tail` allocate a node
 *   block with `lv_malloc` and return a pointer to it.
 * * `lv_ll_remove` unlinks a node from the list but does **not** free the
 *   block — the caller must call `lv_free(node)` afterwards if appropriate.
 * * `lv_ll_clear` frees every node block (the flat allocation described
 *   above).  It does **not** chase any pointers stored inside the user data
 *   portion. If the stored structs themselves contain heap-allocated members,
 *   those will leak unless the caller frees them first (or last).
 * * `lv_ll_clear_custom` accepts a `cleanup` callback that is invoked for
 *   each node **instead** of the default `lv_free`.  Use this when the stored
 *   data needs deep cleanup:
 *
 * @code
 * static void my_cleanup(void * node)
 * {
 *     my_item_t * item = node;
 *     lv_free(item->inner_buf);   // free owned sub-allocation
 *     lv_ll_remove(&my_list, node);
 *     lv_free(node);              // free the node block itself
 * }
 * lv_ll_clear_custom(&my_list, my_cleanup);
 * @endcode
 */

#ifndef LV_LL_H
#define LV_LL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Dummy type to make handling easier*/
typedef uint8_t lv_ll_node_t;

/** Description of a linked list*/
typedef struct {
    uint32_t n_size;
    lv_ll_node_t * head;
    lv_ll_node_t * tail;
} lv_ll_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize linked list
 * @param ll_p pointer to lv_ll_t variable
 * @param node_size the size of 1 node in bytes
 */
void lv_ll_init(lv_ll_t * ll_p, uint32_t node_size);

/**
 * Add a new head to a linked list
 * @param ll_p pointer to linked list
 * @return pointer to the new head
 */
void * lv_ll_ins_head(lv_ll_t * ll_p);

/**
 * Insert a new node in front of the n_act node
 * @param ll_p pointer to linked list
 * @param n_act pointer a node
 * @return pointer to the new node
 */
void * lv_ll_ins_prev(lv_ll_t * ll_p, void * n_act);

/**
 * Add a new tail to a linked list
 * @param ll_p pointer to linked list
 * @return pointer to the new tail
 */
void * lv_ll_ins_tail(lv_ll_t * ll_p);

/**
 * Remove the node 'node_p' from 'll_p' linked list.
 * It does not free the memory of node.
 * @param ll_p pointer to the linked list of 'node_p'
 * @param node_p pointer to node in 'll_p' linked list
 */
void lv_ll_remove(lv_ll_t * ll_p, void * node_p);

/**
 * Remove and optionally clean up all elements from a linked list via a custom callback.
 * If `cleanup` is NULL the node block is simply freed with `lv_free`.
 * If `cleanup` is provided it is called for every node **instead** of `lv_free`;
 * the callback is then responsible for both releasing any owned sub-resources
 * and freeing the node block itself.
 * @param ll_p    pointer to linked list
 * @param cleanup optional per-node cleanup callback, or NULL for a plain free
 */
void lv_ll_clear_custom(lv_ll_t * ll_p, void(*cleanup)(void *));

/**
 * Remove and free all elements from a linked list. The list remain valid but become empty.
 * @param ll_p pointer to linked list
 */
void lv_ll_clear(lv_ll_t * ll_p);

/**
 * Move a node to a new linked list
 * @param ll_ori_p pointer to the original (old) linked list
 * @param ll_new_p pointer to the new linked list
 * @param node pointer to a node
 * @param head true: be the head in the new list
 *             false be the tail in the new list
 */
void lv_ll_chg_list(lv_ll_t * ll_ori_p, lv_ll_t * ll_new_p, void * node, bool head);

/**
 * Return with head node of the linked list
 * @param ll_p pointer to linked list
 * @return pointer to the head of 'll_p'
 */
void * lv_ll_get_head(const lv_ll_t * ll_p);

/**
 * Return with tail node of the linked list
 * @param ll_p pointer to linked list
 * @return pointer to the tail of 'll_p'
 */
void * lv_ll_get_tail(const lv_ll_t * ll_p);

/**
 * Return with the pointer of the next node after 'n_act'
 * @param ll_p pointer to linked list
 * @param n_act pointer a node
 * @return pointer to the next node
 */
void * lv_ll_get_next(const lv_ll_t * ll_p, const void * n_act);

/**
 * Return with the pointer of the previous node after 'n_act'
 * @param ll_p pointer to linked list
 * @param n_act pointer a node
 * @return pointer to the previous node
 */
void * lv_ll_get_prev(const lv_ll_t * ll_p, const void * n_act);

/**
 * Return the length of the linked list.
 * @param ll_p pointer to linked list
 * @return length of the linked list
 */
uint32_t lv_ll_get_len(const lv_ll_t * ll_p);

/*
 * TODO
 * @param ll_p
 * @param n1_p
 * @param n2_p
void lv_ll_swap(lv_ll_t * ll_p, void * n1_p, void * n2_p);
 */

/**
 * Move a node before another node in the same linked list
 *
 * @param ll_p pointer to a linked list
 * @param n_act pointer to node to move
 * @param n_after pointer to a node which should be after `n_act`
 */
void lv_ll_move_before(lv_ll_t * ll_p, void * n_act, void * n_after);

/**
 * Check if a linked list is empty
 * @param ll_p pointer to a linked list
 * @return true: the linked list is empty; false: not empty
 */
bool lv_ll_is_empty(lv_ll_t * ll_p);

/**********************
 *      MACROS
 **********************/

#define LV_LL_READ(list, i) for(i = lv_ll_get_head(list); i != NULL; i = lv_ll_get_next(list, i))

#define LV_LL_READ_BACK(list, i) for(i = lv_ll_get_tail(list); i != NULL; i = lv_ll_get_prev(list, i))

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
