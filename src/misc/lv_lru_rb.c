/**
 * @file lv_lru_rb.c
 *
 */

/**
 * @brief cache model
 * */

/*************************************************************************\
*                                                                         *
*                                             ┏ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ┓ *
* ┏ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━          ┌ ─ ─ ─ ┐              *
*             ┌ ─ ─ ─ ─ ─ ─ ─            ┃    ┃      Cache   insert     ┃ *
* ┃               RB Tree    │                     │Hitting│  head        *
*             └ ─ ─ ─ ─ ─ ─ ─            ┃    ┃     ─ ─ ─ ─             ┃ *
* ┃      ┌─┬─┬─┬─┐                                  ┌─────┐               *
*     ┌──│◄│B│►│ │─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─┃─ ─ ╋ ─ ─▶│  B  │             ┃ *
* ┃   │  └─┴─┴─┴─┘                                  └──▲──┘               *
*     │       │                          ┃    ┃        │                ┃ *
* ┃   │       │                                     ┌──┴──┐               *
*     │       └──────┐                ┌ ─┃─ ─ ╋ ─ ─▶│  E  │             ┃ *
* ┃   ▼         ┌ ─ ─│─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ┐   └──▲──┘               *
*  ┌─┬─┬─┬─┐         ▼                │  ┃    ┃        │                ┃ *
* ┃│◄│A│►│ │─ ─ ┘ ┌─┬─┬─┬─┐                     │   ┌──┴──┐               *
*  └─┴─┴─┴─┘  ┌───│◄│D│►│ │─ ─ ─ ─ ─ ─│─ ╋ ┐  ┃  ─ ▶│  A  │ ┌ ─ ─ ─ ─ ┐ ┃ *
* ┃           │   └─┴─┴─┴─┘                         └──▲──┘     LRU       *
*             │        │              │  ┃ │  ┃        │    │  Cache  │ ┃ *
* ┃           ▼        └──────┐                     ┌──┴──┐  ─ ─ ─ ─ ─    *
*          ┌─┬─┬─┬─┐          ▼       │  ┃ └ ─┃─ ─ ▶│  D  │             ┃ *
* ┃        │◄│C│►│ │─ ─    ┌─┬─┬─┬─┐                └──▲──┘               *
*          └─┴─┴─┴─┘   │   │◄│E│►│ │─ ┘  ┃    ┃        │                ┃ *
* ┃                        └─┴─┴─┴─┘                ┌──┴──┐               *
*                      │        │      ─ ╋ ─ ─┃─ ─ ▶│  C  │             ┃ *
* ┃                     ─ ─ ─ ─ ┼ ─ ─ ┘             └──▲──┘               *
*                               ▼        ┃    ┃   ┌ ─ ─│─ ─ ┐           ┃ *
* ┃                          ┌─┬─┬─┬─┐              ┌──┴──┐               *
*                            │◄│F│►│ │─ ─┃─ ─ ╋ ─ ┼▶│  F  │ │           ┃ *
* ┃                          └─┴─┴─┴─┘              └─────┘               *
*                                        ┃    ┃   └ ─ ─ ─ ─ ┘           ┃ *
* ┃                                                 remove                *
*                                        ┃    ┃      tail               ┃ *
* ┗ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━      ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━  *
*                                                                         *
\*************************************************************************/

/*********************
 *      INCLUDES
 *********************/
#include "lv_lru_rb.h"
#include "lv_ll.h"
#include "lv_rb.h"
#include "../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_lru_rb_t {
    lv_rb_t rb;
    lv_ll_t lru_ll;

    size_t max_size;
    size_t size;
    lv_lru_rb_create_cb_t create_cb;
    lv_lru_rb_free_cb_t free_cb;
};
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void * alloc_new_node(lv_lru_rb_t * lru, void * key, void * user_data);
inline static void ** get_lru_node(lv_lru_rb_t * lru, lv_rb_node_t * node);
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

lv_lru_rb_t * lv_lru_rb_create(size_t node_size, size_t max_size, lv_lru_rb_compare_cb_t compare_cb,
                               lv_lru_rb_create_cb_t create_cb, lv_lru_rb_free_cb_t free_cb)
{
    LV_ASSERT_NULL(compare_cb);
    LV_ASSERT_NULL(create_cb);
    LV_ASSERT_NULL(free_cb);
    LV_ASSERT(node_size > 0);
    LV_ASSERT(max_size > 0);

    if(node_size == 0 || max_size == 0 || compare_cb == NULL || create_cb == NULL || free_cb == NULL) {
        return NULL;
    }

    lv_lru_rb_t * lru;
    lru = lv_malloc(sizeof(lv_lru_rb_t));
    LV_ASSERT_MALLOC(lru);
    if(lru == NULL) {
        return NULL;
    }

    lv_memzero(lru, sizeof(lv_lru_rb_t));

    /*add void* to store the ll node pointer*/
    if(!lv_rb_init(&lru->rb, (lv_rb_compare_t)compare_cb, node_size + sizeof(void *))) {
        return NULL;
    }
    _lv_ll_init(&lru->lru_ll, sizeof(void *));

    lv_lru_rb_set_max_size(lru, max_size, NULL);
    lv_lru_rb_set_compare_cb(lru, compare_cb, NULL);
    lv_lru_rb_set_create_cb(lru, create_cb, NULL);
    lv_lru_rb_set_free_cb(lru, free_cb, NULL);

    return lru;
}

void lv_lru_rb_destroy(lv_lru_rb_t * lru, void * user_data)
{
    LV_ASSERT_NULL(lru);

    if(lru == NULL) {
        return;
    }

    lv_lru_rb_drop_all(lru, user_data);

    lv_free(lru);
}

void * lv_lru_rb_get_or_create(lv_lru_rb_t * lru, const void * key, void * user_data)
{
    LV_ASSERT_NULL(lru);
    LV_ASSERT_NULL(key);

    if(lru == NULL || key == NULL) {
        return NULL;
    }

    /*try the first ll node first*/
    void * head = _lv_ll_get_head(&lru->lru_ll);
    if(head) {
        lv_rb_node_t * node = *(lv_rb_node_t **)head;
        if(lru->rb.compare(node->data, key) == 0) {
            return node->data;
        }
    }

    lv_rb_node_t * node = lv_rb_find(&lru->rb, key);
    /*cache hit*/
    if(node) {
        void * lru_node = *get_lru_node(lru, node);
        head = _lv_ll_get_head(&lru->lru_ll);
        _lv_ll_move_before(&lru->lru_ll, lru_node, head);
        return node->data;
    }

    while(lru->size >= lru->max_size) {
        lv_rb_node_t * tail = *(lv_rb_node_t **)_lv_ll_get_tail(&lru->lru_ll);
        lv_lru_rb_drop(lru, tail->data, user_data);
    }

    /*cache miss*/
    lv_rb_node_t * new_node = alloc_new_node(lru, (void *)key, user_data);
    if(new_node == NULL) {
        return NULL;
    }

    lru->size++;
    return new_node->data;
}

void lv_lru_rb_drop(lv_lru_rb_t * lru, const void * key, void * user_data)
{
    LV_ASSERT_NULL(lru);
    LV_ASSERT_NULL(key);

    if(lru == NULL || key == NULL) {
        return;
    }

    lv_rb_node_t * node = lv_rb_find(&lru->rb, key);
    if(node == NULL) {
        return;
    }

    lru->free_cb(node->data, user_data);

    void * lru_node = *get_lru_node(lru, node);
    lv_rb_drop(&lru->rb, key);
    _lv_ll_remove(&lru->lru_ll, lru_node);

    lv_free(lru_node);

    lru->size--;
}

void lv_lru_rb_drop_all(lv_lru_rb_t * lru, void * user_data)
{
    LV_ASSERT_NULL(lru);

    if(lru == NULL) {
        return;
    }

    lv_rb_node_t ** node;
    _LV_LL_READ(&lru->lru_ll, node) {
        /*free user handled data and do other clean up*/
        lru->free_cb((*node)->data, user_data);
    }

    lv_rb_destroy(&lru->rb);
    _lv_ll_clear(&lru->lru_ll);

    lru->size = 0;
}

void lv_lru_rb_set_max_size(lv_lru_rb_t * lru, size_t max_size, void * user_data)
{
    LV_UNUSED(user_data);
    lru->max_size = max_size;
}

size_t lv_lru_rb_get_max_size(lv_lru_rb_t * lru, void * user_data)
{
    LV_UNUSED(user_data);
    return lru->max_size;
}

size_t lv_lru_rb_get_size(lv_lru_rb_t * lru, void * user_data)
{
    LV_UNUSED(user_data);
    return lru->size;
}

size_t lv_lru_rb_get_free_size(lv_lru_rb_t * lru, void * user_data)
{
    return lv_lru_rb_get_max_size(lru, user_data) - lv_lru_rb_get_size(lru, user_data);
}

void lv_lru_rb_set_compare_cb(lv_lru_rb_t * lru, lv_lru_rb_compare_cb_t compare_cb, void * user_data)
{
    LV_UNUSED(user_data);
    lru->rb.compare = (lv_rb_compare_t)compare_cb;
}

void lv_lru_rb_set_create_cb(lv_lru_rb_t * lru, lv_lru_rb_create_cb_t create_cb, void * user_data)
{
    LV_UNUSED(user_data);
    lru->create_cb = create_cb;
}

void lv_lru_rb_set_free_cb(lv_lru_rb_t * lru, lv_lru_rb_free_cb_t free_cb, void * user_data)
{
    LV_UNUSED(user_data);
    lru->free_cb = free_cb;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void * alloc_new_node(lv_lru_rb_t * lru, void * key, void * user_data)
{
    LV_ASSERT_NULL(lru);
    LV_ASSERT_NULL(key);

    if(lru == NULL || key == NULL) {
        return NULL;
    }

    lv_rb_node_t * node = lv_rb_insert(&lru->rb, key);
    if(node == NULL)
        goto FAILED_HANDLER3;

    lv_memcpy(node->data, key, lru->rb.size - sizeof(void *));

    bool alloc_res = lru->create_cb(node->data, user_data);
    if(alloc_res == false)
        goto FAILED_HANDLER2;

    void * lru_node = _lv_ll_ins_head(&lru->lru_ll);
    if(lru_node == NULL)
        goto FAILED_HANDLER1;

    lv_memcpy(lru_node, &node, sizeof(void *));
    lv_memcpy(get_lru_node(lru, node), &lru_node, sizeof(void *));
    goto FAILED_HANDLER3;

FAILED_HANDLER1:
    lru->free_cb(node->data, user_data);
FAILED_HANDLER2:
    node = NULL;
    lv_rb_drop(&lru->rb, key);
FAILED_HANDLER3:
    return node;
}

inline static void ** get_lru_node(lv_lru_rb_t * lru, lv_rb_node_t * node)
{
    return (void **)((char *)node->data + lru->rb.size - sizeof(void *));
}
