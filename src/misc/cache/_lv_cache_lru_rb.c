/**
* @file _lv_cache_lru_rb.c
*
*/

/*********************
 *      INCLUDES
 *********************/
#include "_lv_cache_lru_rb.h"
#include "../../stdlib/lv_sprintf.h"
#include "../../stdlib/lv_string.h"
#include "../lv_ll.h"
#include "../lv_rb.h"
#include "lv_cache_entry_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_lru_rb_t {
    lv_cache_t cache;

    lv_rb_t rb;
    lv_ll_t ll;
};
typedef struct _lv_lru_rb_t lv_lru_rb_t_;
/**********************
 *  STATIC PROTOTYPES
 **********************/

static void * alloc_cb(void);
static bool  init_cb(lv_cache_t * cache);
static void  destroy_cb(lv_cache_t * cache, void * user_data);

static lv_cache_entry_t * get_cb(lv_cache_t * cache, const void * key, void * user_data);
static lv_cache_entry_t * add_cb(lv_cache_t * cache, const void * key, void * user_data);
static void remove_cb(lv_cache_t * cache, lv_cache_entry_t * entry, void * user_data);
static void drop_cb(lv_cache_t * cache, const void * key, void * user_data);
static void drop_all_cb(lv_cache_t * cache, void * user_data);

static void * alloc_new_node(lv_lru_rb_t_ * lru, void * key, void * user_data);
inline static void ** get_lru_node(lv_lru_rb_t_ * lru, lv_rb_node_t * node);
/**********************
 *  GLOBAL VARIABLES
 **********************/
const lv_cache_class_t lv_cache_class_lru_rb = {
    .alloc_cb = alloc_cb,
    .init_cb = init_cb,
    .destroy_cb = destroy_cb,

    .get_cb = get_cb,
    .add_cb = add_cb,
    .remove_cb = remove_cb,
    .drop_cb = drop_cb,
    .drop_all_cb = drop_all_cb
};
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void * alloc_new_node(lv_lru_rb_t_ * lru, void * key, void * user_data)
{
    LV_UNUSED(user_data);

    LV_ASSERT_NULL(lru);
    LV_ASSERT_NULL(key);

    if(lru == NULL || key == NULL) {
        return NULL;
    }

    lv_rb_node_t * node = lv_rb_insert(&lru->rb, key);
    if(node == NULL)
        goto FAILED_HANDLER2;

    void * data = node->data;
    lv_cache_entry_t * entry = lv_cache_entry_get_entry(data, lru->cache.node_size);
    lv_memcpy(data, key, lru->cache.node_size);

    void * lru_node = _lv_ll_ins_head(&lru->ll);
    if(lru_node == NULL)
        goto FAILED_HANDLER1;

    lv_memcpy(lru_node, &node, sizeof(void *));
    lv_memcpy(get_lru_node(lru, node), &lru_node, sizeof(void *));

    lv_cache_entry_init(entry, &lru->cache, lru->cache.node_size);
    goto FAILED_HANDLER2;

FAILED_HANDLER1:
    lv_rb_drop_node(&lru->rb, node);
    node = NULL;
FAILED_HANDLER2:
    return node;
}

inline static void ** get_lru_node(lv_lru_rb_t_ * lru, lv_rb_node_t * node)
{
    return (void **)((char *)node->data + lru->rb.size - sizeof(void *));
}

static void * alloc_cb(void)
{
    void * res = lv_malloc(sizeof(lv_lru_rb_t_));
    LV_ASSERT_MALLOC(res);
    if(res == NULL) {
        LV_LOG_ERROR("malloc failed");
        return NULL;
    }

    lv_memzero(res, sizeof(lv_lru_rb_t_));
    return res;
}

static bool init_cb(lv_cache_t * cache)
{
    lv_lru_rb_t_ * lru = (lv_lru_rb_t_ *)cache;

    LV_ASSERT_NULL(lru->cache.ops.compare_cb);
    LV_ASSERT_NULL(lru->cache.ops.free_cb);
    LV_ASSERT(lru->cache.node_size > 0);

    if(lru->cache.node_size == 0 || lru->cache.max_size == 0
       || lru->cache.ops.compare_cb == NULL || lru->cache.ops.free_cb == NULL) {
        return false;
    }

    /*add void* to store the ll node pointer*/
    if(!lv_rb_init(&lru->rb, lru->cache.ops.compare_cb, lv_cache_entry_get_size(lru->cache.node_size) + sizeof(void *))) {
        return NULL;
    }
    _lv_ll_init(&lru->ll, sizeof(void *));

    return lru;
}

static void destroy_cb(lv_cache_t * cache, void * user_data)
{
    LV_UNUSED(user_data);

    lv_lru_rb_t_ * lru = (lv_lru_rb_t_ *)cache;

    LV_ASSERT_NULL(lru);

    if(lru == NULL) {
        return;
    }

    cache->clz->drop_all_cb(cache, user_data);
}

static lv_cache_entry_t * get_cb(lv_cache_t * cache, const void * key, void * user_data)
{
    LV_UNUSED(user_data);

    lv_lru_rb_t_ * lru = (lv_lru_rb_t_ *)cache;

    LV_ASSERT_NULL(lru);
    LV_ASSERT_NULL(key);

    if(lru == NULL || key == NULL) {
        return NULL;
    }

    /*try the first ll node first*/
    void * head = _lv_ll_get_head(&lru->ll);
    if(head) {
        lv_rb_node_t * node = *(lv_rb_node_t **)head;
        void * data = node->data;
        lv_cache_entry_t * entry = lv_cache_entry_get_entry(data, cache->node_size);
        if(lru->cache.ops.compare_cb(data, key) == 0) {
            return entry;
        }
    }

    lv_rb_node_t * node = lv_rb_find(&lru->rb, key);
    /*cache hit*/
    if(node) {
        void * lru_node = *get_lru_node(lru, node);
        head = _lv_ll_get_head(&lru->ll);
        _lv_ll_move_before(&lru->ll, lru_node, head);

        lv_cache_entry_t * entry = lv_cache_entry_get_entry(node->data, cache->node_size);
        return entry;
    }
    return NULL;
}

static lv_cache_entry_t * add_cb(lv_cache_t * cache, const void * key, void * user_data)
{
    LV_UNUSED(user_data);

    lv_lru_rb_t_ * lru = (lv_lru_rb_t_ *)cache;

    LV_ASSERT_NULL(lru);
    LV_ASSERT_NULL(key);

    if(lru == NULL || key == NULL) {
        return NULL;
    }

    if(cache->max_size <= 0) {
        return NULL;
    }

    void * tail = _lv_ll_get_tail(&lru->ll);
    void * curr = tail;
    while(lru->cache.size >= lru->cache.max_size) {
        if(curr == NULL) {
            LV_LOG_ERROR("failed to drop cache");
            return NULL;
        }

        lv_rb_node_t * tail_node = *(lv_rb_node_t **)curr;
        void * search_key = tail_node->data;
        lv_cache_entry_t * entry = lv_cache_entry_get_entry(search_key, cache->node_size);
        if(lv_cache_entry_get_ref(entry) == 0) {
            cache->clz->drop_cb(cache, search_key, user_data);
            continue;
        }

        curr = _lv_ll_get_prev(&lru->ll, curr);
    }

    /*cache miss*/
    lv_rb_node_t * new_node = alloc_new_node(lru, (void *)key, user_data);
    if(new_node == NULL) {
        return NULL;
    }

    lru->cache.size++;

    lv_cache_entry_t * entry = lv_cache_entry_get_entry(new_node->data, cache->node_size);
    return entry;
}

static void remove_cb(lv_cache_t * cache, lv_cache_entry_t * entry, void * user_data)
{
    LV_UNUSED(user_data);

    lv_lru_rb_t_ * lru = (lv_lru_rb_t_ *)cache;

    LV_ASSERT_NULL(lru);
    LV_ASSERT_NULL(entry);

    if(lru == NULL || entry == NULL) {
        return;
    }

    void * data = lv_cache_entry_get_data(entry);
    lv_rb_node_t * node = lv_rb_find(&lru->rb, data);
    if(node == NULL) {
        return;
    }

    void * lru_node = *get_lru_node(lru, node);
    lv_rb_remove_node(&lru->rb, node);
    _lv_ll_remove(&lru->ll, lru_node);
    lv_free(lru_node);

    lru->cache.size--;
}

static void drop_cb(lv_cache_t * cache, const void * key, void * user_data)
{
    lv_lru_rb_t_ * lru = (lv_lru_rb_t_ *)cache;

    LV_ASSERT_NULL(lru);
    LV_ASSERT_NULL(key);

    if(lru == NULL || key == NULL) {
        return;
    }

    lv_rb_node_t * node = lv_rb_find(&lru->rb, key);
    if(node == NULL) {
        return;
    }

    void * data = node->data;

    lru->cache.ops.free_cb(data, user_data);

    void * lru_node = *get_lru_node(lru, node);
    lv_rb_remove_node(&lru->rb, node);
    lv_cache_entry_delete(lv_cache_entry_get_entry(data, cache->node_size));
    _lv_ll_remove(&lru->ll, lru_node);
    lv_free(lru_node);

    lru->cache.size--;
}

static void drop_all_cb(lv_cache_t * cache, void * user_data)
{
    lv_lru_rb_t_ * lru = (lv_lru_rb_t_ *)cache;

    LV_ASSERT_NULL(lru);

    if(lru == NULL) {
        return;
    }

    uint32_t used_cnt = 0;
    lv_rb_node_t ** node;
    _LV_LL_READ(&lru->ll, node) {
        /*free user handled data and do other clean up*/
        void * search_key = (*node)->data;
        lv_cache_entry_t * entry = lv_cache_entry_get_entry(search_key, cache->node_size);
        if(lv_cache_entry_get_ref(entry) == 0) {
            lru->cache.ops.free_cb(search_key, user_data);
        }
        else {
            LV_LOG_WARN("entry (%p) is still referenced (%" LV_PRId32 ")", entry, lv_cache_entry_get_ref(entry));
            used_cnt++;
        }
    }
    if(used_cnt > 0) {
        LV_LOG_WARN("%" LV_PRId32 " entries are still referenced", used_cnt);
    }

    lv_rb_destroy(&lru->rb);
    _lv_ll_clear(&lru->ll);

    lru->cache.size = 0;
}
