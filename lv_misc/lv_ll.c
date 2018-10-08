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
#define LL_NODE_META_SIZE (sizeof(lv_ll_node_t*) + sizeof(lv_ll_node_t*))
#define LL_PREV_P_OFFSET(ll_p) (ll_p->n_size)
#define LL_NEXT_P_OFFSET(ll_p) (ll_p->n_size + sizeof(lv_ll_node_t*))

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
 * @param n_size the size of 1 node in bytes
 */
void lv_ll_init(lv_ll_t * ll_p, uint32_t n_size)
{
    ll_p->head = NULL;
    ll_p->tail = NULL;
#ifdef LV_MEM_ENV64
    /*Round the size up to 8*/
    if(n_size & 0x7) {
        n_size = n_size & (~0x7);
        n_size += 8;
    }
#else
    /*Round the size up to 4*/
    if(n_size & 0x3) {
        n_size = n_size & (~0x3);
        n_size += 4;
    }
#endif

    ll_p->n_size = n_size;
}

/**
 * Add a new head to a linked list
 * @param ll_p pointer to linked list
 * @return pointer to the new head
 */
void * lv_ll_ins_head(lv_ll_t * ll_p)
{
    lv_ll_node_t * n_new;

    n_new = lv_mem_alloc(ll_p->n_size + LL_NODE_META_SIZE);

    if(n_new != NULL) {
        node_set_prev(ll_p, n_new, NULL);           /*No prev. before the new head*/
        node_set_next(ll_p, n_new, ll_p->head);     /*After new comes the old head*/

        if(ll_p->head != NULL) { /*If there is old head then before it goes the new*/
            node_set_prev(ll_p, ll_p->head, n_new);
        }

        ll_p->head = n_new;         /*Set the new head in the dsc.*/
        if(ll_p->tail == NULL) {/*If there is no tail (1. node) set the tail too*/
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
void * lv_ll_ins_prev(lv_ll_t * ll_p, void * n_act)
{
    lv_ll_node_t * n_new;
    lv_ll_node_t * n_prev;

    if(NULL == ll_p || NULL == n_act) return NULL;

    if(lv_ll_get_head(ll_p) == n_act) {
        n_new = lv_ll_ins_head(ll_p);
        if(n_new == NULL) return NULL;
    } else {
        n_new = lv_mem_alloc(ll_p->n_size + LL_NODE_META_SIZE);
        if(n_new == NULL) return NULL;

        n_prev = lv_ll_get_prev(ll_p, n_act);
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
void * lv_ll_ins_tail(lv_ll_t * ll_p)
{
    lv_ll_node_t * n_new;

    n_new = lv_mem_alloc(ll_p->n_size + LL_NODE_META_SIZE);
    if(n_new == NULL) return NULL;

    if(n_new != NULL) {
        node_set_next(ll_p, n_new, NULL); /*No next after the new tail*/
        node_set_prev(ll_p, n_new, ll_p->tail); /*The prev. before new is tho old tail*/
        if(ll_p->tail != NULL) {    /*If there is old tail then the new comes after it*/
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
 * It Dose not free the the memory of node.
 * @param ll_p pointer to the linked list of 'node_p'
 * @param node_p pointer to node in 'll_p' linked list
 */
void lv_ll_rem(lv_ll_t  * ll_p, void * node_p)
{
    if(lv_ll_get_head(ll_p) == node_p) {
        /*The new head will be the node after 'n_act'*/
        ll_p->head = lv_ll_get_next(ll_p, node_p);
        if(ll_p->head == NULL) {
            ll_p->tail = NULL;
        } else {
            node_set_prev(ll_p, ll_p->head, NULL);
        }
    } else if(lv_ll_get_tail(ll_p) == node_p) {
        /*The new tail will be the  node before 'n_act'*/
        ll_p->tail = lv_ll_get_prev(ll_p, node_p);
        if(ll_p->tail == NULL) {
            ll_p->head = NULL;
        } else {
            node_set_next(ll_p, ll_p->tail, NULL);
        }
    } else {
        lv_ll_node_t * n_prev = lv_ll_get_prev(ll_p, node_p);
        lv_ll_node_t * n_next = lv_ll_get_next(ll_p, node_p);

        node_set_next(ll_p, n_prev, n_next);
        node_set_prev(ll_p, n_next, n_prev);
    }
}

/**
 * Remove and free all elements from a linked list. The list remain valid but become empty.
 * @param ll_p pointer to linked list
 */
void lv_ll_clear(lv_ll_t * ll_p)
{
    void * i;
    void * i_next;

    i = lv_ll_get_head(ll_p);
    i_next = NULL;

    while(i != NULL) {
        i_next = lv_ll_get_next(ll_p, i);

        lv_ll_rem(ll_p, i);
        lv_mem_free(i);

        i = i_next;
    }
}

/**
 * Move a node to a new linked list
 * @param ll_ori_p pointer to the original (old) linked list
 * @param ll_new_p pointer to the new linked list
 * @param node pointer to a node
 */
void lv_ll_chg_list(lv_ll_t * ll_ori_p, lv_ll_t * ll_new_p, void * node)
{
    lv_ll_rem(ll_ori_p, node);

    /*Set node as head*/
    node_set_prev(ll_new_p, node, NULL);
    node_set_next(ll_new_p, node, ll_new_p->head);

    if(ll_new_p->head != NULL) { /*If there is old head then before it goes the new*/
        node_set_prev(ll_new_p, ll_new_p->head, node);
    }

    ll_new_p->head = node;        /*Set the new head in the dsc.*/
    if(ll_new_p->tail == NULL) {     /*If there is no tail (first node) set the tail too*/
        ll_new_p->tail = node;
    }
}

/**
 * Return with head node of the linked list
 * @param ll_p pointer to linked list
 * @return pointer to the head of 'll_p'
 */
void * lv_ll_get_head(const lv_ll_t * ll_p)
{
    void * head = NULL;

    if(ll_p != NULL)    {
        head = ll_p->head;
    }

    return head;
}

/**
 * Return with tail node of the linked list
 * @param ll_p pointer to linked list
 * @return pointer to the head of 'll_p'
 */
void * lv_ll_get_tail(const lv_ll_t * ll_p)
{
    void * tail = NULL;

    if(ll_p != NULL)    {
        tail = ll_p->tail;
    }

    return tail;
}

/**
 * Return with the pointer of the next node after 'n_act'
 * @param ll_p pointer to linked list
 * @param n_act pointer a node
 * @return pointer to the next node
 */
void * lv_ll_get_next(const lv_ll_t * ll_p, const  void * n_act)
{
    void * next = NULL;

    if(ll_p != NULL)    {
        const lv_ll_node_t * n_act_d = n_act;
        memcpy(&next, n_act_d + LL_NEXT_P_OFFSET(ll_p), sizeof(void *));
    }

    return next;
}

/**
 * Return with the pointer of the previous node after 'n_act'
 * @param ll_p pointer to linked list
 * @param n_act pointer a node
 * @return pointer to the previous node
 */
void * lv_ll_get_prev(const lv_ll_t * ll_p, const  void * n_act)
{
    void * prev = NULL;

    if(ll_p != NULL) {
        const lv_ll_node_t * n_act_d = n_act;
        memcpy(&prev, n_act_d + LL_PREV_P_OFFSET(ll_p), sizeof(void *));
    }

    return prev;
}

void lv_ll_swap(lv_ll_t * ll_p, void * n1_p, void * n2_p)
{
    (void)(ll_p);
    (void)(n1_p);
    (void)(n2_p);
    /*TODO*/
}

/**
 * Move a nodw before an other node in the same linked list
 * @param ll_p pointer to a linked list
 * @param n_act pointer to node to move
 * @param n_after pointer to a node which should be after `n_act`
 */
void lv_ll_move_before(lv_ll_t * ll_p, void * n_act, void * n_after)
{
    if(n_act == n_after) return;    /*Can't move before itself*/


    void * n_before = lv_ll_get_prev(ll_p, n_after);
    if(n_act == n_before) return;   /*Already before `n_after`*/

    /*It's much easier to remove from the list and add again*/
    lv_ll_rem(ll_p, n_act);

    /*Add again by setting the prev. and next nodes*/
    node_set_next(ll_p, n_before, n_act);
    node_set_prev(ll_p, n_act, n_before);
    node_set_prev(ll_p, n_after, n_act);
    node_set_next(ll_p, n_act, n_after);

    /*If `n_act` was moved before NULL then it become the new tail*/
    if(n_after == NULL) ll_p->tail = n_act;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Set the 'pervious node pointer' of a node
 * @param ll_p pointer to linked list
 * @param act pointer to a node which prev. node pointer should be set
 * @param prev pointer to a node which should be the previous node before 'act'
 */
static void node_set_prev(lv_ll_t * ll_p, lv_ll_node_t * act, lv_ll_node_t * prev)
{
    if(act == NULL) return;     /*Can't set the prev node of `NULL`*/

    uint32_t node_p_size = sizeof(lv_ll_node_t *);
    if(prev) memcpy(act + LL_PREV_P_OFFSET(ll_p), &prev, node_p_size);
    else memset(act + LL_PREV_P_OFFSET(ll_p), 0, node_p_size);
}

/**
 * Set the 'next node pointer' of a node
 * @param ll_p pointer to linked list
 * @param act pointer to a node which next node pointer should be set
 * @param next pointer to a node which should be the next node before 'act'
 */
static void node_set_next(lv_ll_t * ll_p, lv_ll_node_t * act, lv_ll_node_t * next)
{
    if(act == NULL) return;     /*Can't set the next node of `NULL`*/

    uint32_t node_p_size = sizeof(lv_ll_node_t *);
    if(next) memcpy(act + LL_NEXT_P_OFFSET(ll_p), &next, node_p_size);
    else memset(act + LL_NEXT_P_OFFSET(ll_p), 0, node_p_size);
}

