/**
 * @file lv_ll.c
 * Handle linked lists.
 * The nodes are dynamically allocated by the 'lv_mem' module,
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <string.h>

#include "lv_ll.h"
#include "lv_mem.h"

/*********************
 *      DEFINES
 *********************/
#define LL_NODE_META_SIZE (sizeof(lv_ll_node_t *) + sizeof(lv_ll_node_t *))
#define LL_PREV_P_OFFSET(ll_p) (ll_p->n_size)
#define LL_NEXT_P_OFFSET(ll_p) (ll_p->n_size + sizeof(lv_ll_node_t *))

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void node_set_prev(lv_ll_t * ll_p, lv_ll_node_t * act, lv_ll_node_t * prev);
static void node_set_next(lv_ll_t * ll_p, lv_ll_node_t * act, lv_ll_node_t * next);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize linked list
 * @param ll_dsc pointer to ll_dsc variable
 * @param node_size the size of 1 node in bytes
 */
void _lv_ll_init(lv_ll_t * ll_p, uint32_t node_size)
{
    ll_p->head = NULL;
    ll_p->tail = NULL;
#ifdef LV_ARCH_64
    /*Round the size up to 8*/
    node_size = (node_size + 7) & (~0x7);
#else
    /*Round the size up to 4*/
    node_size = (node_size + 3) & (~0x3);
#endif

    ll_p->n_size = node_size;
}

/**
 * Add a new head to a linked list
 * @param ll_p pointer to linked list
 * @return pointer to the new head
 */
void * _lv_ll_ins_head(lv_ll_t * ll_p)
{
    lv_ll_node_t * n_new;

    n_new = lv_mem_alloc(ll_p->n_size + LL_NODE_META_SIZE);

    if(n_new != NULL) {
        node_set_prev(ll_p, n_new, NULL);       /*No prev. before the new head*/
        node_set_next(ll_p, n_new, ll_p->head); /*After new comes the old head*/

        if(ll_p->head != NULL) { /*If there is old head then before it goes the new*/
            node_set_prev(ll_p, ll_p->head, n_new);
        }

        ll_p->head = n_new;      /*Set the new head in the dsc.*/
        if(ll_p->tail == NULL) { /*If there is no tail (1. node) set the tail too*/
            ll_p->tail = n_new;
        }
    }

    return n_new;
}

/**
 * Insert a new node in front of the n_act node
 * @param ll_p pointer to linked list
 * @param n_act pointer a node
 * @return pointer to the new head
 */
void * _lv_ll_ins_prev(lv_ll_t * ll_p, void * n_act)
{
    lv_ll_node_t * n_new;

    if(NULL == ll_p || NULL == n_act) return NULL;

    if(_lv_ll_get_head(ll_p) == n_act) {
        n_new = _lv_ll_ins_head(ll_p);
        if(n_new == NULL) return NULL;
    }
    else {
        n_new = lv_mem_alloc(ll_p->n_size + LL_NODE_META_SIZE);
        if(n_new == NULL) return NULL;

        lv_ll_node_t * n_prev;
        n_prev = _lv_ll_get_prev(ll_p, n_act);
        node_set_next(ll_p, n_prev, n_new);
        node_set_prev(ll_p, n_new, n_prev);
        node_set_prev(ll_p, n_act, n_new);
        node_set_next(ll_p, n_new, n_act);
    }

    return n_new;
}

/**
 * Add a new tail to a linked list
 * @param ll_p pointer to linked list
 * @return pointer to the new tail
 */
void * _lv_ll_ins_tail(lv_ll_t * ll_p)
{
    lv_ll_node_t * n_new;

    n_new = lv_mem_alloc(ll_p->n_size + LL_NODE_META_SIZE);

    if(n_new != NULL) {
        node_set_next(ll_p, n_new, NULL);       /*No next after the new tail*/
        node_set_prev(ll_p, n_new, ll_p->tail); /*The prev. before new is the old tail*/
        if(ll_p->tail != NULL) {                /*If there is old tail then the new comes after it*/
            node_set_next(ll_p, ll_p->tail, n_new);
        }

        ll_p->tail = n_new;      /*Set the new tail in the dsc.*/
        if(ll_p->head == NULL) { /*If there is no head (1. node) set the head too*/
            ll_p->head = n_new;
        }
    }

    return n_new;
}

/**
 * Remove the node 'node_p' from 'll_p' linked list.
 * It does not free the memory of node.
 * @param ll_p pointer to the linked list of 'node_p'
 * @param node_p pointer to node in 'll_p' linked list
 */
void _lv_ll_remove(lv_ll_t * ll_p, void * node_p)
{
    if(ll_p == NULL) return;

    if(_lv_ll_get_head(ll_p) == node_p) {
        /*The new head will be the node after 'n_act'*/
        ll_p->head = _lv_ll_get_next(ll_p, node_p);
        if(ll_p->head == NULL) {
            ll_p->tail = NULL;
        }
        else {
            node_set_prev(ll_p, ll_p->head, NULL);
        }
    }
    else if(_lv_ll_get_tail(ll_p) == node_p) {
        /*The new tail will be the node before 'n_act'*/
        ll_p->tail = _lv_ll_get_prev(ll_p, node_p);
        if(ll_p->tail == NULL) {
            ll_p->head = NULL;
        }
        else {
            node_set_next(ll_p, ll_p->tail, NULL);
        }
    }
    else {
        lv_ll_node_t * n_prev = _lv_ll_get_prev(ll_p, node_p);
        lv_ll_node_t * n_next = _lv_ll_get_next(ll_p, node_p);

        node_set_next(ll_p, n_prev, n_next);
        node_set_prev(ll_p, n_next, n_prev);
    }
}

/**
 * Remove and free all elements from a linked list. The list remain valid but become empty.
 * @param ll_p pointer to linked list
 */
void _lv_ll_clear(lv_ll_t * ll_p)
{
    void * i;
    void * i_next;

    i      = _lv_ll_get_head(ll_p);
    i_next = NULL;

    while(i != NULL) {
        i_next = _lv_ll_get_next(ll_p, i);

        _lv_ll_remove(ll_p, i);
        lv_mem_free(i);

        i = i_next;
    }
}

/**
 * Move a node to a new linked list
 * @param ll_ori_p pointer to the original (old) linked list
 * @param ll_new_p pointer to the new linked list
 * @param node pointer to a node
 * @param head true: be the head in the new list
 *             false be the head in the new list
 */
void _lv_ll_chg_list(lv_ll_t * ll_ori_p, lv_ll_t * ll_new_p, void * node, bool head)
{
    _lv_ll_remove(ll_ori_p, node);

    if(head) {
        /*Set node as head*/
        node_set_prev(ll_new_p, node, NULL);
        node_set_next(ll_new_p, node, ll_new_p->head);

        if(ll_new_p->head != NULL) { /*If there is old head then before it goes the new*/
            node_set_prev(ll_new_p, ll_new_p->head, node);
        }

        ll_new_p->head = node;       /*Set the new head in the dsc.*/
        if(ll_new_p->tail == NULL) { /*If there is no tail (first node) set the tail too*/
            ll_new_p->tail = node;
        }
    }
    else {
        /*Set node as tail*/
        node_set_prev(ll_new_p, node, ll_new_p->tail);
        node_set_next(ll_new_p, node, NULL);

        if(ll_new_p->tail != NULL) { /*If there is old tail then after it goes the new*/
            node_set_next(ll_new_p, ll_new_p->tail, node);
        }

        ll_new_p->tail = node;       /*Set the new tail in the dsc.*/
        if(ll_new_p->head == NULL) { /*If there is no head (first node) set the head too*/
            ll_new_p->head = node;
        }
    }
}

/**
 * Return with head node of the linked list
 * @param ll_p pointer to linked list
 * @return pointer to the head of 'll_p'
 */
void * _lv_ll_get_head(const lv_ll_t * ll_p)
{
    if(ll_p == NULL) return NULL;
    return ll_p->head;
}

/**
 * Return with tail node of the linked list
 * @param ll_p pointer to linked list
 * @return pointer to the head of 'll_p'
 */
void * _lv_ll_get_tail(const lv_ll_t * ll_p)
{
    if(ll_p == NULL) return NULL;
    return ll_p->tail;
}

/**
 * Return with the pointer of the next node after 'n_act'
 * @param ll_p pointer to linked list
 * @param n_act pointer a node
 * @return pointer to the next node
 */
void * _lv_ll_get_next(const lv_ll_t * ll_p, const void * n_act)
{
    /*Pointer to the next node is stored in the end of this node.
     *Go there and return the address found there*/
    const lv_ll_node_t * n_act_d = n_act;
    n_act_d += LL_NEXT_P_OFFSET(ll_p);
    return *((lv_ll_node_t **)n_act_d);
}

/**
 * Return with the pointer of the previous node after 'n_act'
 * @param ll_p pointer to linked list
 * @param n_act pointer a node
 * @return pointer to the previous node
 */
void * _lv_ll_get_prev(const lv_ll_t * ll_p, const void * n_act)
{
    /*Pointer to the prev. node is stored in the end of this node.
     *Go there and return the address found there*/
    const lv_ll_node_t * n_act_d = n_act;
    n_act_d += LL_PREV_P_OFFSET(ll_p);
    return *((lv_ll_node_t **)n_act_d);
}

/**
 * Return the length of the linked list.
 * @param ll_p pointer to linked list
 * @return length of the linked list
 */
uint32_t _lv_ll_get_len(const lv_ll_t * ll_p)
{
    uint32_t len = 0;
    void * node;

    for(node = _lv_ll_get_head(ll_p); node != NULL; node = _lv_ll_get_next(ll_p, node)) {
        len++;
    }

    return len;
}

/**
 * Move a node before an other node in the same linked list
 * @param ll_p pointer to a linked list
 * @param n_act pointer to node to move
 * @param n_after pointer to a node which should be after `n_act`
 */
void _lv_ll_move_before(lv_ll_t * ll_p, void * n_act, void * n_after)
{
    if(n_act == n_after) return; /*Can't move before itself*/

    void * n_before;
    if(n_after != NULL)
        n_before = _lv_ll_get_prev(ll_p, n_after);
    else
        n_before = _lv_ll_get_tail(ll_p); /*if `n_after` is NULL `n_act` should be the new tail*/

    if(n_act == n_before) return; /*Already before `n_after`*/

    /*It's much easier to remove from the list and add again*/
    _lv_ll_remove(ll_p, n_act);

    /*Add again by setting the prev. and next nodes*/
    node_set_next(ll_p, n_before, n_act);
    node_set_prev(ll_p, n_act, n_before);
    node_set_prev(ll_p, n_after, n_act);
    node_set_next(ll_p, n_act, n_after);

    /*If `n_act` was moved before NULL then it become the new tail*/
    if(n_after == NULL) ll_p->tail = n_act;

    /*If `n_act` was moved before `NULL` then it's the new head*/
    if(n_before == NULL) ll_p->head = n_act;
}

/**
 * Check if a linked list is empty
 * @param ll_p pointer to a linked list
 * @return true: the linked list is empty; false: not empty
 */
bool _lv_ll_is_empty(lv_ll_t * ll_p)
{
    if(ll_p == NULL) return true;

    if(ll_p->head == NULL && ll_p->tail == NULL) return true;

    return false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Set the previous node pointer of a node
 * @param ll_p pointer to linked list
 * @param act pointer to a node which prev. node pointer should be set
 * @param prev pointer to a node which should be the previous node before 'act'
 */
static void node_set_prev(lv_ll_t * ll_p, lv_ll_node_t * act, lv_ll_node_t * prev)
{
    if(act == NULL) return; /*Can't set the prev node of `NULL`*/

    uint8_t * act8 = (uint8_t *)act;

    act8 += LL_PREV_P_OFFSET(ll_p);

    lv_ll_node_t ** act_node_p = (lv_ll_node_t **) act8;
    lv_ll_node_t ** prev_node_p = (lv_ll_node_t **) &prev;

    *act_node_p = *prev_node_p;
}

/**
 * Set the 'next node pointer' of a node
 * @param ll_p pointer to linked list
 * @param act pointer to a node which next node pointer should be set
 * @param next pointer to a node which should be the next node before 'act'
 */
static void node_set_next(lv_ll_t * ll_p, lv_ll_node_t * act, lv_ll_node_t * next)
{
    if(act == NULL) return; /*Can't set the next node of `NULL`*/
    uint8_t * act8 = (uint8_t *)act;

    act8 += LL_NEXT_P_OFFSET(ll_p);
    lv_ll_node_t ** act_node_p = (lv_ll_node_t **) act8;
    lv_ll_node_t ** next_node_p = (lv_ll_node_t **) &next;

    *act_node_p = *next_node_p;
}
