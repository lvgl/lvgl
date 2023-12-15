/**
* @file lv_cache.c
*
*/

/*********************
 *      INCLUDES
 *********************/
#include "lv_cache.h"
#include "../lv_assert.h"
#include "../../stdlib/lv_sprintf.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_cache_entry_t_ {
    const lv_cache_t_ * cache;
    int32_t ref_cnt;
    uint32_t generation;
    uint32_t node_size;
    bool is_invalid;
};
/**********************
 *  STATIC PROTOTYPES
 **********************/

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

lv_cache_t_ * lv_cache_create_(const lv_cache_class_t * cache_class,
                               size_t node_size, size_t max_size,
                               lv_cache_compare_cb_t compare_cb,
                               lv_cache_create_cb_t create_cb,
                               lv_cache_free_cb_t free_cb)
{
    lv_cache_t_ * cache = cache_class->alloc_cb();
    LV_ASSERT_MALLOC(cache);

    cache->clz = cache_class;
    cache->node_size = node_size;
    cache->max_size = max_size;
    cache->size = 0;
    cache->compare_cb = compare_cb;
    cache->create_cb = create_cb;
    cache->free_cb = free_cb;

    cache->clz->init_cb(cache);

    lv_mutex_init(&cache->lock);

    return cache;
}

void   lv_cache_destroy_(lv_cache_t_ * cache, void * user_data)
{
    LV_ASSERT_NULL(cache);
    cache->clz->destroy_cb(cache, user_data);
    lv_mutex_delete(&cache->lock);
}

lv_cache_entry_t_ * lv_cache_get_(lv_cache_t_ * cache, const void * key, void * user_data)
{
    LV_ASSERT_NULL(cache);
    LV_ASSERT_NULL(key);

    lv_mutex_lock(&cache->lock);
    lv_cache_entry_t_ * entry = cache->clz->get_cb(cache, key, user_data);
    lv_mutex_unlock(&cache->lock);
    return entry;
}
lv_cache_entry_t_ * lv_cache_get_or_create_(lv_cache_t_ * cache, const void * key, void * user_data)
{
    LV_ASSERT_NULL(cache);
    LV_ASSERT_NULL(key);

    lv_mutex_lock(&cache->lock);
    lv_cache_entry_t_ * entry = cache->clz->get_cb(cache, key, user_data);
    if(entry == NULL) {
        entry = cache->clz->create_entry_cb(cache, key, user_data);
    }
    lv_mutex_unlock(&cache->lock);
    return entry;
}
void   lv_cache_drop_(lv_cache_t_ * cache, const void * key, void * user_data)
{
    LV_ASSERT_NULL(cache);
    LV_ASSERT_NULL(key);

    lv_mutex_lock(&cache->lock);
    lv_cache_entry_t_ * entry = cache->clz->get_cb(cache, key, user_data);
    if(entry == NULL) {
        lv_mutex_unlock(&cache->lock);
        return;
    }

    if(lv_cache_entry_ref_get(entry) == 0) {
        cache->clz->drop_cb(cache, key, user_data);
    }
    else {
        lv_cache_entry_set_invalid(entry, true);
        lv_cache_remove_(cache, entry, user_data);
    }
    lv_mutex_unlock(&cache->lock);
}
void   lv_cache_remove_(lv_cache_t_* cache, lv_cache_entry_t_* entry, void * user_data)
{
    LV_ASSERT_NULL(cache);
    LV_ASSERT_NULL(entry);

    lv_mutex_lock(&cache->lock);
    cache->clz->remove_cb(cache, entry, user_data);
    lv_mutex_unlock(&cache->lock);
}
void   lv_cache_drop_all_(lv_cache_t_ * cache, void * user_data)
{
    LV_ASSERT_NULL(cache);

    lv_mutex_lock(&cache->lock);
    cache->clz->drop_all_cb(cache, user_data);
    lv_mutex_unlock(&cache->lock);
}
void   lv_cache_set_max_size_(lv_cache_t_ * cache, size_t max_size, void * user_data)
{
    LV_UNUSED(user_data);
    cache->max_size = max_size;
}
size_t lv_cache_get_max_size_(lv_cache_t_ * cache, void * user_data)
{
    LV_UNUSED(user_data);
    return cache->max_size;
}
size_t lv_cache_get_size_(lv_cache_t_ * cache, void * user_data)
{
    LV_UNUSED(user_data);
    return cache->size;
}
size_t lv_cache_get_free_size_(lv_cache_t_ * cache, void * user_data)
{
    LV_UNUSED(user_data);
    return cache->max_size - cache->size;
}
void   lv_cache_set_compare_cb(lv_cache_t_ * cache, lv_cache_compare_cb_t compare_cb, void * user_data)
{
    LV_UNUSED(user_data);
    cache->compare_cb = compare_cb;
}
void   lv_cache_set_create_cb(lv_cache_t_ * cache, lv_cache_create_cb_t alloc_cb, void * user_data)
{
    LV_UNUSED(user_data);
    cache->create_cb = alloc_cb;
}
void   lv_cache_set_free_cb(lv_cache_t_ * cache, lv_cache_free_cb_t free_cb, void * user_data)
{
    LV_UNUSED(user_data);
    cache->free_cb = free_cb;
}

void     lv_cache_entry_ref_reset(lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);
    entry->ref_cnt = 0;
}
void     lv_cache_entry_ref_inc(lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);
    entry->ref_cnt++;
}
void     lv_cache_entry_ref_dec(lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);
    entry->ref_cnt--;
    if(entry->ref_cnt < 0) {
        LV_LOG_WARN("lv_cache_entry_ref_dec: ref_cnt(%" LV_PRIu32 ") < 0", entry->ref_cnt);
        entry->ref_cnt = 0;
    }
}
int32_t  lv_cache_entry_ref_get(lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);
    return entry->ref_cnt;
}
uint32_t lv_cache_entry_get_generation(lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);
    return entry->generation;
}
void     lv_cache_entry_inc_generation(lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);
    entry->generation++;
}
void     lv_cache_entry_set_generation(lv_cache_entry_t_ * entry, uint32_t generation)
{
    LV_ASSERT_NULL(entry);
    entry->generation = generation;
}
uint32_t lv_cache_entry_get_node_size(lv_cache_entry_t_ * entry)
{
    return entry->node_size;
}
void     lv_cache_entry_set_node_size(lv_cache_entry_t_ * entry, uint32_t node_size)
{
    entry->node_size = node_size;
}
void     lv_cache_entry_set_invalid(lv_cache_entry_t_ * entry, bool is_invalid)
{
    LV_ASSERT_NULL(entry);
    entry->is_invalid = is_invalid;
}
bool     lv_cache_entry_is_invalid(lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);
    return entry->is_invalid;
}
void  *  lv_cache_entry_get_data(lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);
    return (uint8_t *)entry - entry->node_size;
}
void  * lv_cache_entry_acquire_data(lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);

    lv_mutex_lock(&entry->cache->lock);
    lv_cache_entry_ref_inc(entry);
    lv_mutex_unlock(&entry->cache->lock);
    return lv_cache_entry_get_data(entry);
}
void    lv_cache_entry_release_data(lv_cache_entry_t_ * entry, void * user_data)
{
    LV_ASSERT_NULL(entry);
    if(lv_cache_entry_ref_get(entry) == 0) {
        LV_LOG_ERROR("lv_cache_entry_release_data: ref_cnt(%" LV_PRIu32 ") == 0", entry->ref_cnt);
        return;
    }

    lv_mutex_lock(&entry->cache->lock);
    lv_cache_entry_ref_dec(entry);

    if(lv_cache_entry_ref_get(entry) == 0 && lv_cache_entry_is_invalid(entry)) {
        lv_cache_t_ * cache = (lv_cache_t_ *)lv_cache_entry_get_cache(entry);
        lv_cache_drop_(cache, lv_cache_entry_get_data(entry), user_data);
    }
    lv_mutex_unlock(&entry->cache->lock);
}
lv_cache_entry_t_ * lv_cache_entry_get_entry(void * data, const uint32_t node_size)
{
    LV_ASSERT_NULL(data);
    return (lv_cache_entry_t_ *)((uint8_t *)data + node_size);
}
void     lv_cache_entry_set_cache(lv_cache_entry_t_ * entry, const lv_cache_t_ * cache)
{
    LV_ASSERT_NULL(entry);
    entry->cache = cache;
}
const lv_cache_t_ * lv_cache_entry_get_cache(const lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);
    return entry->cache;
}

uint32_t lv_cache_entry_get_size(const uint32_t node_size)
{
    return node_size + sizeof(lv_cache_entry_t_);
}
lv_cache_entry_t_ * lv_cache_entry_alloc(const uint32_t node_size)
{
    void * res = lv_malloc_zeroed(lv_cache_entry_get_size(node_size));
    LV_ASSERT_MALLOC(res)
    if(res == NULL) {
        LV_LOG_ERROR("lv_cache_entry_alloc: malloc failed");
        return NULL;
    }
    lv_cache_entry_t_ * entry = (lv_cache_entry_t_ *)res;
    entry->node_size = node_size;
    return (lv_cache_entry_t_ *)((uint8_t *)entry + node_size);
}
void lv_cache_entry_free(lv_cache_entry_t_ * entry)
{
    void * data = lv_cache_entry_get_data(entry);
    lv_free(data);
}
/**********************
 *   STATIC FUNCTIONS
 **********************/
