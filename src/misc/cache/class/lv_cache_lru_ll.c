/**
* @file lv_cache_lru_ll.c
*
*/

/*********************************\
*                                *
*  ┏ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ┓ *
*       ┌ ─ ─ ─ ┐                *
*  ┃      Cache   insert       ┃ *
*       │Hitting│  head          *
*  ┃     ─ ─ ─ ─               ┃ *
*        ┌─────┐                 *
*  ┃     │  B  │               ┃ *
*        └──▲──┘                 *
*  ┃        │                  ┃ *
*        ┌──┴──┐                 *
*  ┃     │  E  │               ┃ *
*        └──▲──┘                 *
*  ┃        │                  ┃ *
*        ┌──┴──┐                 *
*  ┃     │  A  │ ┌ ─ ─ ─ ─ ─ ┐ ┃ *
*        └──▲──┘      LRU        *
*  ┃        │    │   Cache   │ ┃ *
*        ┌──┴──┐  ─ ─ ─ ─ ─ ─    *
*  ┃     │  D  │               ┃ *
*        └──▲──┘                 *
*  ┃        │                  ┃ *
*        ┌──┴──┐                 *
*  ┃     │  C  │               ┃ *
*        └──▲──┘                 *
*  ┃   ┌ ─ ─│─ ─ ┐             ┃ *
*        ┌──┴──┐                 *
*  ┃   │ │  F  │ │             ┃ *
*        └─────┘                 *
*  ┃   └ ─ ─ ─ ─ ┘             ┃ *
*        remove                  *
*  ┃      tail                 ┃ *
*   ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━  *
*                                *
\*********************************/

/*********************
 *      INCLUDES
 *********************/

#include "lv_cache_lru_ll.h"
#include "../lv_cache_entry.h"
#include "../../../stdlib/lv_sprintf.h"
#include "../../../stdlib/lv_string.h"
#include "../../lv_ll.h"

#include "../../lv_iter.h"
#include "../../lv_assert.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef uint32_t (get_data_size_cb_t)(const void * data);

struct _lv_cache_lru_ll_t {
    lv_cache_t cache;
    lv_ll_t ll;
    get_data_size_cb_t * get_data_size_cb;
};

typedef struct _lv_cache_lru_ll_t lv_cache_lru_ll_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void * alloc_cb(void);
static bool init_cnt_cb(lv_cache_t * cache);
static bool init_size_cb(lv_cache_t * cache);
static void  destroy_cb(lv_cache_t * cache, void * user_data);

static lv_cache_entry_t * get_cb(lv_cache_t * cache, const void * key, void * user_data);
static lv_cache_entry_t * add_cb(lv_cache_t * cache, const void * key, void * user_data);
static void remove_cb(lv_cache_t * cache, lv_cache_entry_t * entry, void * user_data);
static void drop_cb(lv_cache_t * cache, const void * key, void * user_data);
static void drop_all_cb(lv_cache_t * cache, void * user_data);
static lv_cache_entry_t * get_victim_cb(lv_cache_t * cache, void * user_data);
static lv_cache_reserve_cond_res_t reserve_cond_cb(lv_cache_t * cache, const void * key, size_t reserved_size,
                                                   void * user_data);

static void * alloc_new_node(lv_cache_lru_ll_t * lru, void * key, void * user_data);

static uint32_t cnt_get_data_size_cb(const void * data);
static uint32_t size_get_data_size_cb(const void * data);

static lv_iter_t * cache_iter_create_cb(lv_cache_t * cache);
static lv_result_t cache_iter_next_cb(void * instance, void * context, void * elem);

/**********************
 *  GLOBAL VARIABLES
 **********************/
const lv_cache_class_t lv_cache_class_lru_ll_count = {
    .alloc_cb = alloc_cb,
    .init_cb = init_cnt_cb,
    .destroy_cb = destroy_cb,

    .get_cb = get_cb,
    .add_cb = add_cb,
    .remove_cb = remove_cb,
    .drop_cb = drop_cb,
    .drop_all_cb = drop_all_cb,
    .get_victim_cb = get_victim_cb,
    .reserve_cond_cb = reserve_cond_cb,
    .iter_create_cb = cache_iter_create_cb,
};

const lv_cache_class_t lv_cache_class_lru_ll_size = {
    .alloc_cb = alloc_cb,
    .init_cb = init_size_cb,
    .destroy_cb = destroy_cb,

    .get_cb = get_cb,
    .add_cb = add_cb,
    .remove_cb = remove_cb,
    .drop_cb = drop_cb,
    .drop_all_cb = drop_all_cb,
    .get_victim_cb = get_victim_cb,
    .reserve_cond_cb = reserve_cond_cb,
    .iter_create_cb = cache_iter_create_cb,
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

static void * alloc_new_node(lv_cache_lru_ll_t * lru, void * key, void * user_data)
{
    LV_UNUSED(user_data);

    LV_ASSERT_NULL(lru);
    LV_ASSERT_NULL(key);

    if(lru == NULL || key == NULL) {
        return NULL;
    }

    void * new_node = lv_ll_ins_head(&lru->ll);
    if(new_node == NULL) {
        return NULL;
    }

    lv_memcpy(new_node, key, lru->cache.node_size);
    lv_cache_entry_t * entry = lv_cache_entry_get_entry(new_node, lru->cache.node_size);
    lv_cache_entry_init(entry, &lru->cache, lru->cache.node_size);
    return entry;
}

static void * alloc_cb(void)
{
    void * res = lv_malloc(sizeof(lv_cache_lru_ll_t));
    LV_ASSERT_MALLOC(res);
    if(res == NULL) {
        LV_LOG_ERROR("malloc failed");
        return NULL;
    }

    lv_memzero(res, sizeof(lv_cache_lru_ll_t));
    return res;
}

static bool init_cnt_cb(lv_cache_t * cache)
{
    lv_cache_lru_ll_t * lru = (lv_cache_lru_ll_t *)cache;

    LV_ASSERT_NULL(lru->cache.ops.compare_cb);
    LV_ASSERT_NULL(lru->cache.ops.free_cb);
    LV_ASSERT(lru->cache.node_size > 0);

    if(lru->cache.node_size <= 0 || lru->cache.ops.compare_cb == NULL || lru->cache.ops.free_cb == NULL) {
        return false;
    }

    lv_ll_init(&lru->ll, lv_cache_entry_get_size(lru->cache.node_size));
    lru->get_data_size_cb = cnt_get_data_size_cb;

    return true;
}

static bool init_size_cb(lv_cache_t * cache)
{
    lv_cache_lru_ll_t * lru = (lv_cache_lru_ll_t *)cache;

    LV_ASSERT_NULL(lru->cache.ops.compare_cb);
    LV_ASSERT_NULL(lru->cache.ops.free_cb);
    LV_ASSERT(lru->cache.node_size > 0);

    if(lru->cache.node_size <= 0 || lru->cache.ops.compare_cb == NULL || lru->cache.ops.free_cb == NULL) {
        return false;
    }

    lv_ll_init(&lru->ll, lv_cache_entry_get_size(lru->cache.node_size));
    lru->get_data_size_cb = size_get_data_size_cb;

    return true;
}

static void destroy_cb(lv_cache_t * cache, void * user_data)
{
    LV_UNUSED(user_data);

    lv_cache_lru_ll_t * lru = (lv_cache_lru_ll_t *)cache;

    LV_ASSERT_NULL(lru);

    if(lru == NULL) {
        return;
    }

    cache->clz->drop_all_cb(cache, user_data);
    cache->size = 0;
}

static lv_cache_entry_t * get_cb(lv_cache_t * cache, const void * key, void * user_data)
{
    LV_UNUSED(user_data);

    lv_cache_lru_ll_t * lru = (lv_cache_lru_ll_t *)cache;

    LV_ASSERT_NULL(lru);
    LV_ASSERT_NULL(key);

    if(lru == NULL || key == NULL) {
        return NULL;
    }

    /* Linear search */
    void * node = NULL;
    LV_LL_READ(&lru->ll, node) {
        if(lru->cache.ops.compare_cb(node, key) == 0) {
            break;
        }
    }
    /*cache hit*/
    if(node) {
        void * head = lv_ll_get_head(&lru->ll);
        lv_ll_move_before(&lru->ll, node, head);

        lv_cache_entry_t * entry = lv_cache_entry_get_entry(node, cache->node_size);
        return entry;
    }
    return NULL;
}

static lv_cache_entry_t * add_cb(lv_cache_t * cache, const void * key, void * user_data)
{
    LV_UNUSED(user_data);

    lv_cache_lru_ll_t * lru = (lv_cache_lru_ll_t *)cache;

    LV_ASSERT_NULL(lru);
    LV_ASSERT_NULL(key);

    if(lru == NULL || key == NULL) {
        return NULL;
    }

    lv_cache_entry_t * entry = alloc_new_node(lru, (void *)key, user_data);
    if(entry == NULL) {
        return NULL;
    }

    cache->size += lru->get_data_size_cb(key);

    return entry;
}

static void remove_cb(lv_cache_t * cache, lv_cache_entry_t * entry, void * user_data)
{
    LV_UNUSED(user_data);

    lv_cache_lru_ll_t * lru = (lv_cache_lru_ll_t *)cache;

    LV_ASSERT_NULL(lru);
    LV_ASSERT_NULL(entry);

    if(lru == NULL || entry == NULL) {
        return;
    }

    void * node = lv_cache_entry_get_data(entry);

    lv_ll_remove(&lru->ll, node);

    cache->size -= lru->get_data_size_cb(node);
}

static void drop_cb(lv_cache_t * cache, const void * key, void * user_data)
{
    lv_cache_lru_ll_t * lru = (lv_cache_lru_ll_t *)cache;

    LV_ASSERT_NULL(lru);
    LV_ASSERT_NULL(key);

    if(lru == NULL || key == NULL) {
        return;
    }

    void * node = cache->clz->get_cb(cache, key, user_data);
    if(node == NULL) {
        return;
    }
    lv_ll_remove(&lru->ll, node);

    lru->cache.ops.free_cb(node, user_data);
    cache->size -= lru->get_data_size_cb(node);

    lv_cache_entry_t * entry = lv_cache_entry_get_entry(node, cache->node_size);
    lv_cache_entry_delete(entry);
}

static void drop_all_cb(lv_cache_t * cache, void * user_data)
{
    lv_cache_lru_ll_t * lru = (lv_cache_lru_ll_t *)cache;

    LV_ASSERT_NULL(lru);

    if(lru == NULL) {
        return;
    }

    uint32_t used_cnt = 0;
    void * node;
    LV_LL_READ(&lru->ll, node) {
        /*free user handled data and do other clean up*/
        lv_cache_entry_t * entry = lv_cache_entry_get_entry(node, cache->node_size);
        if(lv_cache_entry_get_ref(entry) == 0) {
            lru->cache.ops.free_cb(node, user_data);
        }
        else {
            LV_LOG_WARN("entry (%p) is still referenced (%" LV_PRId32 ")", (void *)entry, lv_cache_entry_get_ref(entry));
            used_cnt++;
        }
    }
    if(used_cnt > 0) {
        LV_LOG_WARN("%" LV_PRId32 " entries are still referenced", used_cnt);
    }

    lv_ll_clear(&lru->ll);

    cache->size = 0;
}

static lv_cache_entry_t * get_victim_cb(lv_cache_t * cache, void * user_data)
{
    LV_UNUSED(user_data);

    lv_cache_lru_ll_t * lru = (lv_cache_lru_ll_t *)cache;

    LV_ASSERT_NULL(lru);

    void * tail;
    LV_LL_READ_BACK(&lru->ll, tail) {
        lv_cache_entry_t * entry = lv_cache_entry_get_entry(tail, cache->node_size);
        if(lv_cache_entry_get_ref(entry) == 0) {
            return entry;
        }
    }

    return NULL;
}

static lv_cache_reserve_cond_res_t reserve_cond_cb(lv_cache_t * cache, const void * key, size_t reserved_size,
                                                   void * user_data)
{
    LV_UNUSED(user_data);

    lv_cache_lru_ll_t * lru = (lv_cache_lru_ll_t *)cache;

    LV_ASSERT_NULL(lru);

    if(lru == NULL) {
        return LV_CACHE_RESERVE_COND_ERROR;
    }

    uint32_t data_size = key ? lru->get_data_size_cb(key) : 0;
    if(data_size > lru->cache.max_size) {
        LV_LOG_ERROR("data size (%" LV_PRIu32 ") is larger than max size (%" LV_PRIu32 ")", data_size, lru->cache.max_size);
        return LV_CACHE_RESERVE_COND_TOO_LARGE;
    }

    return cache->size + reserved_size + data_size > lru->cache.max_size
           ? LV_CACHE_RESERVE_COND_NEED_VICTIM
           : LV_CACHE_RESERVE_COND_OK;
}

static uint32_t cnt_get_data_size_cb(const void * data)
{
    LV_UNUSED(data);
    return 1;
}

static uint32_t size_get_data_size_cb(const void * data)
{
    lv_cache_slot_size_t * slot = (lv_cache_slot_size_t *)data;
    return slot->size;
}

static lv_iter_t * cache_iter_create_cb(lv_cache_t * cache)
{
    return lv_iter_create(cache, lv_cache_entry_get_size(cache->node_size), 0, cache_iter_next_cb);
}

static lv_result_t cache_iter_next_cb(void * instance, void * context, void * elem)
{
    lv_cache_lru_ll_t * lru = (lv_cache_lru_ll_t *)instance;
    void ** ll_node = context;

    LV_ASSERT_NULL(ll_node);

    if(*ll_node == NULL) *ll_node = lv_ll_get_head(&lru->ll);
    else *ll_node = lv_ll_get_next(&lru->ll, *ll_node);

    void * node = *ll_node;

    if(node == NULL) return LV_RESULT_INVALID;

    lv_memcpy(elem, node, lv_cache_entry_get_size(lru->cache.node_size));

    return LV_RESULT_OK;
}
