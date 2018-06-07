/**
 * @file lv_ll.c
 * Handle linked lists. The nodes are dynamically allocated by the 'lv_mem' module.
 */

#ifndef LV_LL_H
#define LV_LL_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#include "lv_mem.h"
#include <stdint.h>
#include <stddef.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Dummy type to make handling easier*/
typedef uint8_t lv_ll_node_t;

/*Description of a linked list*/
typedef struct
{
    uint32_t n_size;
    lv_ll_node_t* head;
    lv_ll_node_t* tail;
}lv_ll_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize linked list
 * @param ll_dsc pointer to ll_dsc variable
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
 * @return pointer to the new head
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
 * It Dose not free the the memory of node.
 * @param ll_p pointer to the linked list of 'node_p'
 * @param node_p pointer to node in 'll_p' linked list
 */
void lv_ll_rem(lv_ll_t  * ll_p, void * node_p);

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
 */
void lv_ll_chg_list(lv_ll_t * ll_ori_p, lv_ll_t * ll_new_p, void * node);

/**
 * Return with head node of the linked list
 * @param ll_p pointer to linked list
 * @return pointer to the head of 'll_p'
 */
void * lv_ll_get_head(lv_ll_t * ll_p);

/**
 * Return with tail node of the linked list
 * @param ll_p pointer to linked list
 * @return pointer to the head of 'll_p'
 */
void * lv_ll_get_tail(lv_ll_t * ll_p);

/**
 * Return with the pointer of the next node after 'n_act'
 * @param ll_p pointer to linked list
 * @param n_act pointer a node
 * @return pointer to the next node
 */
void * lv_ll_get_next(lv_ll_t * ll_p, void * n_act);

/**
 * Return with the pointer of the previous node after 'n_act'
 * @param ll_p pointer to linked list
 * @param n_act pointer a node
 * @return pointer to the previous node
 */
void * lv_ll_get_prev(lv_ll_t * ll_p, void * n_act);

/**
 * Move a nodw before an other node in the same linked list
 * @param ll_p pointer to a linked list
 * @param n_act pointer to node to move
 * @param n_after pointer to a node which should be after `n_act`
 */
void lv_ll_move_before(lv_ll_t * ll_p, void * n_act, void * n_after);

/**********************
 *      MACROS
 **********************/

#define LL_READ(list, i) for(i = lv_ll_get_head(&list); i != NULL; i = lv_ll_get_next(&list, i))

#define LL_READ_BACK(list, i) for(i = lv_ll_get_tail(&list); i != NULL; i = lv_ll_get_prev(&list, i))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
