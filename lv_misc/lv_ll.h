/**
 * @file linked_list.c
 * Handle linked lists. The nodes are dynamically allocated by the dyn_mem module.
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"

#if USE_LINKED_LIST != 0
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
typedef uint8_t ll_node_t;

/*Description of a linked list*/
typedef struct
{
    uint32_t n_size;
    ll_node_t* head;
    ll_node_t* tail;
}ll_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize linked list
 * @param ll_dsc pointer to ll_dsc variable
 * @param n_size the size of 1 node in bytes
 */
void ll_init(ll_dsc_t * ll_p, uint32_t n_size);

/**
 * Add a new head to a linked list
 * @param ll_p pointer to linked list
 * @return pointer to the new head
 */
void * ll_ins_head(ll_dsc_t * ll_p);

/**
 * Add a new tail to a linked list
 * @param ll_p pointer to linked list
 * @return pointer to the new tail
 */
void * ll_ins_tail(ll_dsc_t * ll_p);

/**
 * Remove the node 'node_p' from 'll_p' linked list.
 * It Dose not free the the memory of node.
 * @param ll_p pointer to the linked list of 'node_p'
 * @param node_p pointer to node in 'll_p' linked list
 */
void ll_rem(ll_dsc_t  * ll_p, void * node_p);

/**
 * Remove and free all elements from a linked list.
 * @param ll_p pointer to linked list
 */
void ll_clear(ll_dsc_t * ll_p);

/**
 * Move a node to a new linked list
 * @param ll_ori_p pointer to the original (old) linked list
 * @param ll_new_p pointer to the new linked list
 * @param node pointer to a node
 */
void ll_chg_list(ll_dsc_t * ll_ori_p, ll_dsc_t * ll_new_p, void * node);

/**
 * Return with head node of the linked list
 * @param ll_p pointer to linked list
 * @return pointer to the head of 'll_p'
 */
void * ll_get_head(ll_dsc_t * ll_p);

/**
 * Return with tail node of the linked list
 * @param ll_p pointer to linked list
 * @return pointer to the head of 'll_p'
 */
void * ll_get_tail(ll_dsc_t * ll_p);

/**
 * Return with the pointer of the next node after 'n_act'
 * @param ll_p pointer to linked list
 * @param n_act pointer a node
 * @return pointer to the next node
 */
void * ll_get_next(ll_dsc_t * ll_p, void * n_act);

/**
 * Return with the pointer of the previous node after 'n_act'
 * @param ll_p pointer to linked list
 * @param n_act pointer a node
 * @return pointer to the previous node
 */
void * ll_get_prev(ll_dsc_t * ll_p, void * n_act);

/**********************
 *      MACROS
 **********************/

#define LL_READ(list, i) for(i = ll_get_head(&list); i != NULL; i = ll_get_next(&list, i))

#define LL_READ_BACK(list, i) for(i = ll_get_tail(&list); i != NULL; i = ll_get_prev(&list, i))

#endif /*USE_LINKED_LIST*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
